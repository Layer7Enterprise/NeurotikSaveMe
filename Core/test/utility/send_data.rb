require 'socket'

MULTICAST_ADDR = '224.4.5.6'
PORT = 3000

begin
  socket = UDPSocket.open

  socket.setsockopt Socket::IPPROTO_IP, Socket::IP_TTL, [1].pack('i')
  socket.send ARGV[0], 0, MULTICAST_ADDR, PORT
ensure
  socket.close
end
