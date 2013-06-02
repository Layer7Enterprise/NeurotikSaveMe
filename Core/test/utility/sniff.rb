require 'socket'
require 'bindata'

class Packet < BinData::Record
  endian :little
  string :byteCount, :length => 5
  string :output, :length => 7
end

s = UDPSocket.new

s.bind nil, 3001

loop do
  msg, info = s.recvfrom 1024
  packet = Packet.new
  packet.read msg
  puts packet.output
end
