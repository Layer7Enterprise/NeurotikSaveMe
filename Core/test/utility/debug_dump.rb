require 'socket'
require 'bindata'

class Packet < BinData::Record
  endian :little
  int32 :type
  int32 :idx
  int32 :globalTime
  float :voltage
  float :uu
  int32 :lastSpikeTime
  int32 :inh
end

sock = UDPSocket.new

sock.bind "127.0.0.1", 3002

100.times do
  msg, info = sock.recvfrom 1024

  p = Packet.new
  p.read msg
  puts msg

#  puts "#{p.globalTime} #{p.type} #{p.idx} #{p.voltage} #{p.uu} #{p.lastSpikeTime} #{p.inh}"
end
