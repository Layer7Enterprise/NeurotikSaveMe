require 'socket'

MULTICAST_ADDR = '224.4.5.6'
PORT = 3000

begin
  socket = UDPSocket.open

    socket.setsockopt Socket::IPPROTO_IP, Socket::IP_TTL, [1].pack('i')

    counter = 0
    loop do
      counter += 1

      if counter % 100 == 0
      end
      data = "#{rand(2)}#{rand(2)}"

      output = sprintf "%05d%s", counter, data
      socket.send output, 0, MULTICAST_ADDR, PORT
      sleep 0.001
      puts counter
    end

    ensure
  socket.close
end

