require 'socket'
require 'ipaddr'

MULTICAST_ADDR = "224.4.5.6" 
PORT = 3001

ip =  IPAddr.new(MULTICAST_ADDR).hton + IPAddr.new("0.0.0.0").hton
sock = UDPSocket.new
sock.setsockopt(Socket::IPPROTO_IP, Socket::IP_ADD_MEMBERSHIP, ip)
sock.bind(Socket::INADDR_ANY, PORT)

loop do
    msg, info = sock.recvfrom(1024)
    puts msg.split("").inspect
end
