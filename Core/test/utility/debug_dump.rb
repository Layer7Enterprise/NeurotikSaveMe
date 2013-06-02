require 'socket'

sock = UDPSocket.new

sock.bind "127.0.0.1", 3002

100.times do
  msg, info = sock.recvfrom 1024

  puts "got message"
end
