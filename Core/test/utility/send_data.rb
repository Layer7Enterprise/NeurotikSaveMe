require 'socket'

MULTICAST_ADDR = '224.4.5.6'
PORT = 3000

begin
  socket = UDPSocket.open

    socket.setsockopt Socket::IPPROTO_IP, Socket::IP_TTL, [1].pack('i')

    index = 0
    ARGV.each do |data|
      if (data == 'd')
        index += 1
        next
      end

      data = sprintf "%05d%s", index, data
      socket.send data, 0, MULTICAST_ADDR, PORT
      index += 1
    end

    ensure
  socket.close
end
