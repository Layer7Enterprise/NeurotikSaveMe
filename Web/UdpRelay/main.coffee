jDataView = require 'jDataView'

io = (require 'socket.io').listen 80

io.configure ->
  io.set('log level', 1)

dgram = require 'dgram'
s = dgram.createSocket 'udp4'
s.bind 3002, ->
  s.on "message", (data, info) ->
    io.sockets.emit 'data', data.toString()
