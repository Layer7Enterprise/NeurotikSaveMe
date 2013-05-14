#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include <Windows.h>
#include <winsock.h>
#include <mmsystem.h>
#include <cuda_runtime.h>
#include <queue>
#include "Splash.h"
#include "Core.h"
#include "GetEnv.h"
#include "DebugFile.h"
#include "JSON.h"

//#define DEBUG_TO_FILE 120000
#define DEBUG_STDOUT

//1 is 1ms realtime, 10 is 10 times as slow
#define CLOCK_MULTIPLIER 1

//Init
#ifdef NEURON_DEBUG
	float debug[10];
#endif
int NN = -1;
int ND = -1;
int globalTime = 0;

float dopa;
float oldDopa;
int death;
int *dConnections;
float *dWeights;
int *dDelays;
int *dLastSpikeTimes;
long long *dSpikeQues;

unsigned char *nTypes;

float *nV;
float *nU;
float *nI;
int *nLastSpikeTime;
int *nInh;

float *nPlasticity;
int *nInhibitoryTimes;
float *nMaximumG;
int *nIb;

//Cuda copies
#ifdef NEURON_DEBUG
	float *_debug;
#endif
int _NN = -1;
int _ND = -1;
int _globalTime = 0;

float *_dopa;
float *_oldDopa;
int *_death;
int *_dConnections;
float *_dWeights;
int *_dDelays;
int *_dLastSpikeTimes;
long long *_dSpikeQues;

unsigned char *_nTypes;

float *_nV;
float *_nU;
float *_nI;
int *_nLastSpikeTime;
int *_nInh;
float *_nPlasticity;
int *_nInhibitoryTimes;
float *_nMaximumG;
int *_nIb;


DWORD WINAPI CoreThread(LPVOID lpParam);
HANDLE transmitSync, transmitCompleteSignal, rcvIsHolding, rcvCompleteSignal, transmitRcv;

HANDLE inboundQueueMutex;

int serverBasePort = -1;
char hostName[100];
DWORD WINAPI InboundServerThread(LPVOID lpParam);
DWORD WINAPI InboundServerThreadQueue(LPVOID lpParam);
std::queue<std::vector<unsigned char>> inboundQueue;
DWORD WINAPI OutboundServerThread(LPVOID lpParam);
SOCKET inboundSocket;
SOCKET outboundSocket;

#define INBOUND_SERVER_PORT (serverBasePort)
#define OUTBOUND_SERVER_PORT (serverBasePort+1)
#define DEBUG_SERVER_PORT (serverBasePort+2)

//Where the boundaries are for transmitting network data
int networkInStartPos = -1;
int networkInEndPos = -1;
int networkOutStartPos = -1;
int networkOutEndPos = -1;

//Which neuron to debug
int debugNeuron = -1;

//What idx range the thread executes
struct CoreThreadRange {
	int lower;
	int upper;
	int idx;
};

//Millisecond timer
void CALLBACK MillisecondTimerCallback(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2); 
void CALLBACK SkewCalc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);  //Called every second
HANDLE coreThreadTimer;
int skewCount = 0;

//Outputable neuron cache
std::map<std::string, int> neuronNameToLocation;
std::map<int, std::string> neuronLocationToName;

void InitUDPServer();

void assert(bool is) {
	if (!is) {
		printf("error");
		exit(-2);
	}
}

DebugFile *df;

int main() {
	#ifdef NEURON_DEBUG
		cudaMalloc(&_debug, sizeof(float)*10);
	#endif

#ifdef DEBUG_TO_FILE
		df = new DebugFile("/Users/sotownsend/Dropbox/data.txt");
#endif
	//Some init for the core
	dopa = 1.0f;
	oldDopa = dopa;
	death = false;

	IGNeurotikSplash();

	//Set ENV
	NeurotikEnv_t env;
	IGGetEnv("net.ntke", &env);
	NN = env.NN;
	ND = env.ND;
	strcpy(hostName, env.host);
	serverBasePort = env.port;
	debugNeuron = env.debug;

	//Load instance
	printf(">Loading instances...\n");
	dConnections = new int[NN*ND];
	dWeights = new float[NN*ND];
	dDelays = new int[NN*ND];
	dLastSpikeTimes = new int[NN*ND];
	dSpikeQues = new long long[NN*ND];

	nTypes = new unsigned char[NN];

	nV = new float[NN];
	nU = new float[NN];
	nI = new float[NN];
	nLastSpikeTime = new int[NN];
	nInh = new int[NN];
	nPlasticity = new float[NN];
	nInhibitoryTimes = new int[NN];
	nMaximumG = new float[NN];
	nIb = new int[NN];

	//Cuda versions
#define CND(var) \
	j = cudaMalloc(&var, NN*ND*sizeof(*var)); \
	printf("%d\n", j); \

	int j;
	cudaMalloc(&_dopa, sizeof(*_dopa));
	cudaMalloc(&_oldDopa, sizeof(*_oldDopa));
	cudaMalloc(&_death, sizeof(*_death));
	CND(_dConnections);
	CND(_dWeights);
	CND(_dDelays);
	CND(_dLastSpikeTimes);
	CND(_dSpikeQues);

#define CNN(var) \
	j = cudaMalloc(&var, NN*sizeof(*var)); \
	printf("%d\n", j); \

	CNN(_nTypes);
	CNN(_nV);
	CNN(_nU);
	CNN(_nI);
	CNN(_nLastSpikeTime);
	CNN(_nInh);
	CNN(_nPlasticity);
	CNN(_nInhibitoryTimes);
	CNN(_nMaximumG);
	CNN(_nIb);

	printf(">Loading Schema (%s)...\n", env.schemaFile);

	for (int i = 0; i < NN*ND; ++i) {
		dConnections[i] = -1;
		dLastSpikeTimes[i] = -1;
	}

	//Load file
	FILE *f = fopen(env.schemaFile, "r");
	if (!f) {
		fprintf(stderr, "Couldn't open net.ntks...\n");
		getchar();
		exit(EXIT_FAILURE);
	}

	//Parse schema
	enum PARSE_STATE { PS_NEURONS, PS_CONNECTIONS, PS_NETWORKIN, PS_NETWORKOUT, PS_ENDNETWORKIN, PS_ENDNETWORKOUT } parseState = PS_NEURONS;
	int openPosition = 0;  //Keep track of open position of neurons
	while (!feof(f)) {
		char header[100];
		fscanf(f, "%s", header);

		if (!strcmp(header, "Neuron"))
			parseState = PS_NEURONS;
		else if (!strcmp(header, "Connection"))
			parseState = PS_CONNECTIONS;
		else if (!strcmp(header, "NetworkIn"))
			parseState = PS_NETWORKIN;
		else if (!strcmp(header, "NetworkOut"))
			parseState = PS_NETWORKOUT;
		else if (!strcmp(header, "EndNetworkIn"))
			parseState = PS_ENDNETWORKIN;
		else if (!strcmp(header, "EndNetworkOut"))
			parseState = PS_ENDNETWORKOUT;
		else {
			fprintf(stderr, "Error, invalid parse state '%s'...\n", header);
			getchar();
			exit(EXIT_FAILURE);
		}

		char trash[100];
		switch (parseState) {
		case PS_NETWORKIN:
			networkInStartPos = openPosition;
			break;
		case PS_NETWORKOUT:
			networkOutStartPos = openPosition;
			break;
		case PS_ENDNETWORKIN:
			networkInEndPos = openPosition;
			break;
		case PS_ENDNETWORKOUT:
			networkOutEndPos = openPosition;
			break;
		case PS_NEURONS:
			char neuronName[100];
			NEURON_TYPES neuronType;
			int neuronInhibitoryTime;
			float neuronMaximumG;
			int neuronIb;
			fscanf(f, "%s", neuronName);
			fscanf(f, "%s %d", trash, &neuronType);
			fscanf(f, "%s %d", trash, &neuronInhibitoryTime);
			fscanf(f, "%s %f", trash, &neuronMaximumG);
			fscanf(f, "%s %d", trash, &neuronIb);
			fscanf(f, "%s %s", trash, trash);

			if (strcmp(trash, "End")) {
				fprintf(stderr, "Error, last neuron comment wasn't an End");
				getchar();
				exit(EXIT_FAILURE);
			}

			neuronNameToLocation[neuronName] = openPosition;
			neuronLocationToName[openPosition] = neuronName;

			nV[openPosition] = -70.0f;
			nI[openPosition] = 0.0f;
			nLastSpikeTime[openPosition] = -1000;
			nInh[openPosition] = 0;
			nPlasticity[openPosition] = 1.0f;
			nInhibitoryTimes[openPosition] = neuronInhibitoryTime;
			nMaximumG[openPosition] = neuronMaximumG;
			nIb[openPosition] = neuronIb;

			nTypes[openPosition] = neuronType;

			if (neuronType == GLU)
				nU[openPosition] = NEURON_GLU_B * NEURON_GLU_C;
			else if (neuronType == GABA) {
				nU[openPosition] = NEURON_GLU_B * NEURON_GLU_C;
			}

			++openPosition;

			break;
		case PS_CONNECTIONS:
			char from[100];
			char to[100];
			float weight;
			int delay;
			fscanf(f, "%s %s", trash, from);
			fscanf(f, "%s %s", trash, to);
			fscanf(f, "%s %f", trash, &weight);
			fscanf(f, "%s %d", trash, &delay);
			fscanf(f, "%s %s", trash, trash);

			int fromNode = neuronNameToLocation[from];
			int toNode = neuronNameToLocation[to];

			//Get an open connection
			int pos = -1;
			for (int i = 0; i < ND; ++i) {
				pos = toNode*ND + i;
				if (dConnections[pos] == -1) {
					dConnections[pos] = fromNode;
					dWeights[pos] = weight;
					dDelays[pos] = delay;
					dSpikeQues[pos] = 0;
					break;
				}
			}
			
			if (pos == -1)
				printf(">WARINNG: RAN OUT OF DENDRITES!!!\n");
		}
	}

	CoreThreadRange thread0;
	thread0.lower = 0;
	thread0.upper = NN;
	thread0.idx = 0;

	//Core thread
	transmitSync = CreateEvent(NULL, FALSE, FALSE, NULL);
	transmitCompleteSignal = CreateEvent(NULL, FALSE, FALSE, NULL);
	rcvCompleteSignal = CreateEvent(NULL, FALSE, FALSE, NULL);
	transmitRcv = CreateEvent(NULL, FALSE, FALSE, NULL);
	rcvIsHolding = CreateEvent(NULL, FALSE, FALSE, NULL);
	inboundQueueMutex = CreateMutex(NULL, FALSE, NULL);

	//Configure timer
	coreThreadTimer = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (timeBeginPeriod(1) == TIMERR_NOCANDO) {
		fprintf(stderr, "Couldn't set timer to 1 millisecond resolution...\n");
		getchar();
		exit(EXIT_FAILURE);
	};

	//Millisecond timer
	if (!timeSetEvent(CLOCK_MULTIPLIER, 1, (LPTIMECALLBACK)MillisecondTimerCallback, NULL, TIME_PERIODIC)) {
		fprintf(stderr, "Couldn't set timer callback up...\n");
		getchar();
		exit(EXIT_FAILURE);
	}

	//Skew checker (Called every millisecond)
	if (!timeSetEvent(1000*CLOCK_MULTIPLIER, 1, (LPTIMECALLBACK)SkewCalc, NULL, TIME_PERIODIC)) {
		fprintf(stderr, "Couldn't set timer callback up...\n");
		getchar();
		exit(EXIT_FAILURE);
	}

	HANDLE coreThread = CreateThread(NULL, 0, CoreThread, &thread0, 0, NULL);
	SetThreadPriority(coreThread, THREAD_PRIORITY_ABOVE_NORMAL);
	printf(">Loaded %d Nodes\n", neuronLocationToName.size());
	printf(">Core Online!\n");

	//UDP servers
	InitUDPServer();
	CreateThread(NULL, 0, InboundServerThread, NULL, 0, NULL);
	CreateThread(NULL, 0, InboundServerThreadQueue, NULL, 0, NULL);
	CreateThread(NULL, 0, OutboundServerThread, NULL, 0, NULL);
	SetThreadPriority(OutboundServerThread, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(InboundServerThreadQueue, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(InboundServerThread, THREAD_PRIORITY_ABOVE_NORMAL);

	while (true) {
		Sleep(1000);
	}
}

DWORD WINAPI CoreThread(LPVOID lpParam) {
	CoreThreadRange params = *(CoreThreadRange *)lpParam;

//NN*ND copies (Dentrites)
#define C2HD(var) cudaMemcpy(var, _##var, sizeof(*var)*NN*ND, cudaMemcpyDeviceToHost);
#define H2CD(var) cudaMemcpy(_##var, var, sizeof(*var)*NN*ND, cudaMemcpyHostToDevice);

//NN copies (Neurons)
#define C2HN(var) cudaMemcpy(var, _##var, sizeof(*var)*NN, cudaMemcpyDeviceToHost);
#define H2CN(var) cudaMemcpy(_##var, var, sizeof(*var)*NN, cudaMemcpyHostToDevice);

	//Initial copy
	H2CD(dDelays);
	H2CD(dConnections);
	H2CD(dWeights);
	H2CD(dSpikeQues);
	H2CN(nTypes);
	H2CN(nV);
	H2CN(nU);
	H2CN(nI);
	H2CN(nLastSpikeTime);
	H2CN(nInh);
	H2CN(nPlasticity);
	H2CN(nInhibitoryTimes);
	H2CN(nMaximumG);
	H2CN(nIb);
	cudaMemcpy(_oldDopa, &oldDopa, sizeof(oldDopa), cudaMemcpyHostToDevice);
	cudaMemcpy(_death, &death, sizeof(death), cudaMemcpyHostToDevice);

	while (true) {
		H2CN(nI);
		cudaMemcpy(_dopa, &dopa, sizeof(dopa), cudaMemcpyHostToDevice);

#define N_THREAD 512
#define N_BLOCKS (NN / N_THREAD) + 1
#ifdef NEURON_DEBUG
		IGNeurotikCore <<< N_BLOCKS, N_THREAD >>>(NN, ND, globalTime, _dopa, _oldDopa, _death, _dConnections, _dWeights, _dDelays, _dLastSpikeTimes, _dSpikeQues, (unsigned char *)_nTypes, _nV, _nU, _nI, _nLastSpikeTime, _nInh, _nPlasticity, _nInhibitoryTimes, _nMaximumG, _nIb, _debug);
#else
		IGNeurotikCore <<< N_BLOCKS, N_THREAD >>>(NN, ND, globalTime, _dConnections, _dWeights, _dDelays, _dLastSpikeTimes, _dSpikeQues, (unsigned char *)_nTypes, _nV, _nU, _nI, _nDopa, _nLastSpikeTime, _nInh, _nPlasticity, _nInhibitoryTimes, _nMaximumG, _nI);
#endif

		//cudaMemcpy(&dopa, _dopa, sizeof(dopa), cudaMemcpyDeviceToHost);
		C2HN(nLastSpikeTime);
		C2HN(nI);
		C2HN(nInh);

		if (debugNeuron > 0) {
			cudaMemcpy(dWeights+debugNeuron*ND, _dWeights+debugNeuron*ND, sizeof(*dWeights)*ND, cudaMemcpyDeviceToHost);
		}

#ifdef NEURON_DEBUG
#ifdef DEBUG_STDOUT
		cudaMemcpy(debug, _debug, sizeof(float)*10, cudaMemcpyDeviceToHost);
		if (debug[0] || debug[1] || debug[2] || debug[3] || debug[4] || debug[5] || debug[6] || debug[7] || debug[8] || debug[9]) {
			//if (globalTime % 100 == 0)
			printf("DEBUG: %f %f %f %f %f %f %f %f %f %f\n", debug[0], debug[1], debug[2], debug[3], debug[4], debug[5], debug[6], debug[7], debug[8], debug[9]);
		}
#endif
#if DEBUG_TO_FILE
		for (int i = 0; i < ND; ++i)
			*df << debug[i];
		df->Next();

		if (globalTime % 10000 == 0)
			df->Dump();
#endif
#endif

		//Wait for network transmission
		SetEvent(transmitSync);
		WaitForSingleObject(transmitCompleteSignal, INFINITE);

		//Is there something ready to be ready?
		if (WaitForSingleObject(rcvIsHolding, 0) == WAIT_OBJECT_0) {
			//Signal to write
			SetEvent(transmitRcv);

			//Wait for recieve
			WaitForSingleObject(rcvCompleteSignal, INFINITE);
		}

		++globalTime;
		++skewCount;

		//Make sure timing is correct
		WaitForSingleObject(coreThreadTimer, INFINITE);
	}

	return 0;
}

DWORD WINAPI InboundServerThreadQueue(LPVOID lpParam) {
	while (true) {
		//Wait till we get the green light to sync
		WaitForSingleObject(transmitRcv, INFINITE);
		
		//Lock the queue mutex
		WaitForSingleObject(inboundQueueMutex, INFINITE);

		while (!inboundQueue.empty()) {
			std::vector<unsigned char> data = inboundQueue.front();

			for (int i = 0; i < data.size()+1; ++i) {
				//Dopamine uptake
				if (i == 0) {
					dopa = data[i];
				} else {
					//Offset I
					if (data[i] == '1') {
						nI[i-1] += 30.0f;
					}
				}
			}

			inboundQueue.pop();
		}

		ReleaseMutex(inboundQueueMutex);

		//Signal that we are done
		SetEvent(rcvCompleteSignal);
	}

	return 0;
}

DWORD WINAPI InboundServerThread(LPVOID lpParam) {
	//Incomming information
	sockaddr_in client;
	const int BUFFER_SIZE = 10000;
	char *buffer = new char[BUFFER_SIZE];
	int client_length = (int)sizeof(struct sockaddr_in);
	while (true) {
		/* Receive bytes from client */
		int bytes_received = recvfrom(inboundSocket, buffer, BUFFER_SIZE, 0, 
					(struct sockaddr *)&client, &client_length);

		//Not the right data (Including dopamine)
		if (bytes_received-1 != networkInEndPos - networkInStartPos) {
			fprintf(stderr, ">WARNING: Datagram was the wrong size.  Length recieved: %d, supposed to be: %d\n", 
				bytes_received, networkInEndPos - networkInStartPos + 1);
		}

		std::vector<unsigned char> dataToPush;
		dataToPush.insert(dataToPush.end(), buffer, buffer+bytes_received);

		WaitForSingleObject(inboundQueueMutex, INFINITE);
		inboundQueue.push(dataToPush);
		ReleaseMutex(inboundQueueMutex);

		//Claim we have data
		SetEvent(rcvIsHolding);
	}
		
	return 0;
}


DWORD WINAPI OutboundServerThread(LPVOID lpParam) {
	static int packetCount = 0;

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(hostName);
	addr.sin_port = htons(OUTBOUND_SERVER_PORT);

	sockaddr_in debugAddr;
	memset(&debugAddr, 0, sizeof(debugAddr));
	debugAddr.sin_family = AF_INET;
	debugAddr.sin_addr.s_addr = inet_addr(hostName);
	debugAddr.sin_port = htons(DEBUG_SERVER_PORT);

	//Create a buffer
	//{N__COUNTER__DATA__}
	const int nOutput = networkOutEndPos - networkOutStartPos;
	const int bufferSize = nOutput + sizeof(int)*2;
	char *buffer = new char[bufferSize];
	memcpy(buffer, &nOutput, sizeof(nOutput));

	while (true) {
		WaitForSingleObject(transmitSync, INFINITE);
		memcpy(buffer+sizeof(nOutput), &packetCount, sizeof(packetCount));

		//Make sure there is output to send
		int isOutput = 0;

		//Load data
		for (int i = 0; i < nOutput; ++i) {
			//Neuron is under inhibitory influences
			if (nInh[networkOutStartPos+i] > 0) {
				buffer[i+sizeof(int)*2] = -1;
				//isOutput += 1;
			} else {
				buffer[i+sizeof(int)*2] = (nLastSpikeTime[networkOutStartPos+i] == globalTime);
				isOutput += nLastSpikeTime[networkOutStartPos+i] == globalTime;
			}
		}

		//Wait till ready to transmit
		if (isOutput) {
			sendto(outboundSocket, buffer, bufferSize, 0,  (sockaddr *)&addr,sizeof(addr));
			++packetCount;
		}


		if (globalTime % 150 == 0 && debugNeuron > 0) {
			std::string name = neuronLocationToName[debugNeuron];
			printf("Debugging: %s\n", name.c_str());
			std::vector<std::string> titles;
			for (int i = 0; i < ND; ++i) {
				if (dConnections[debugNeuron*ND+i] >= 0) {
					std::stringstream name;
					if (nTypes[dConnections[debugNeuron*ND+i]] & GABA) {
						name << "GABA";
					} else {
						name << i;
					}

					titles.push_back(name.str());
				}
			}
			char *debugBuffer = FloatsToJson(name.c_str(), titles, titles.size(), dWeights+debugNeuron*ND);
			sendto(outboundSocket, (const char *)debugBuffer, strlen(debugBuffer), 0,  (sockaddr *)&debugAddr,sizeof(addr));
		}

		//Signal that we are done
		SetEvent(transmitCompleteSignal);
	}

	return 0;
}

void InitUDPServer() {
	/* Open windows connection */
	WSADATA w;
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		getchar();
		exit(0);
	}

	/* Open a datagram socket */
	inboundSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (inboundSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "Could not create socket.\n");
		WSACleanup();
		getchar();
		exit(0);
	}

	int ttl = 1;
	outboundSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	setsockopt(outboundSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)ttl, sizeof(ttl));
	if (inboundSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "Could not create socket.\n");
		WSACleanup();
		getchar();
		exit(0);
	}

	/* Clear out server struct */
	sockaddr_in inboundServer;
	memset((void *)&inboundServer, '\0', sizeof(struct sockaddr_in));
	inboundServer.sin_family = AF_INET;
	inboundServer.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	inboundServer.sin_port = htons(INBOUND_SERVER_PORT);

	sockaddr_in outboundServer;
	memset(&outboundServer, '\0', sizeof(sockaddr_in));
	outboundServer.sin_family = AF_INET;
	outboundServer.sin_port = htons(OUTBOUND_SERVER_PORT);

    hostent *hp = gethostbyname(hostName);

    /* Check for NULL pointer */
    if (hp == NULL)
    {
        fprintf(stderr, "Could not get host name.\n");
        closesocket(inboundSocket);
        WSACleanup();
        exit(0);
    }

	if (bind(inboundSocket, (struct sockaddr *)&inboundServer, 
						 sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, ">ERROR: Could not bind name to inbound socket, WSA error: %d\n", WSAGetLastError());
		closesocket(inboundSocket);
		WSACleanup();
		getchar();
		exit(0);
	}

	ip_mreq group;
	group.imr_interface.S_un.S_addr = inet_addr("0.0.0.0");
	group.imr_multiaddr.S_un.S_addr = inet_addr(hostName);
	if (setsockopt(inboundSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) == -1) {
		fprintf(stderr, ">ERROR: Could not set multicast recieve options, WSA error: %d\n", WSAGetLastError());
		closesocket(inboundSocket);
		WSACleanup();
		getchar();
		exit(0);
	}

	printf(">Inbound server (%s:%d) Online! [%d-%d)\n", hostName, INBOUND_SERVER_PORT, networkInStartPos, networkInEndPos);
	printf(">Outbound server (%s:%d) Online! [%d-%d)\n", hostName, OUTBOUND_SERVER_PORT, networkOutStartPos, networkOutEndPos);
	printf(">Debug server (%s:%d) Online! %d\n", hostName, DEBUG_SERVER_PORT, debugNeuron);
}

//Called 1000 times
void CALLBACK SkewCalc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
	float skew = (1000.0f - (float)skewCount)/1000.0f;
	skewCount = 0;

	printf(">SKEW: %.1f%%\n", skew*100.0f);
	printf(">TIME: %d\n", globalTime);
}

void CALLBACK MillisecondTimerCallback(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
	SetEvent(coreThreadTimer);
}
