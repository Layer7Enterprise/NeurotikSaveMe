require 'socket'

MULTICAST_ADDR = '224.4.5.6'
PORT = 3000

begin
  socket = UDPSocket.open

    socket.setsockopt Socket::IPPROTO_IP, Socket::IP_TTL, [1].pack('i')

    ARGV.each_with_index do |data, index|
      data = sprintf "%05d%s", index, data
      socket.send data, 0, MULTICAST_ADDR, PORT
    end

    ensure
  socket.close
end

