class Graph
  constructor: (@id) ->
    @canvas = document.getElementById(@id)
    @ctx = @canvas.getContext '2d'

  #Draw the background blank
  drawBackground: =>
    @ctx.fillStyle = '#333'
    @ctx.fillRect 0, 0, @canvas.width, @canvas.height

  #Get width
  getWidth: =>
    @canvas.width

  #Draw data lines, long list of values
  drawLine: (x, color) =>
    @ctx.strokeStyle = color
    @ctx.beginPath()
    @ctx.moveTo(0, @canvas.height - x[0]*@canvas.height)

    for value, idx in x
      continue if idx == 0
      @ctx.lineTo(idx, @canvas.height - value*@canvas.height)
      @ctx.lineWidth = 2
      @ctx.stroke()

      @ctx.beginPath()
      @ctx.moveTo(idx, @canvas.height - value*@canvas.height)

  #Regions are just defined by start and end
  drawRegion: (x, color, opacity=0.5) =>
    @ctx.globalAlpha = opacity
    @ctx.fillStyle = color
    @ctx.fillRect x.start, 0, x.end - x.start, @canvas.height
    @ctx.globalAlpha = 1.0

MIN_VOLTAGE = -65.0
MAX_VOLTAGE = 50.0
THRESH_VOLTAGE = 30.0

class NeuronGraph
  constructor: (@id) ->
    @graph = new Graph id
    
  size: =>
    @graph.getWidth()

  begin: =>
    #Draw background
    @graph.drawBackground()

    #Draw Voltage threshold line
    @graph.ctx.strokeStyle = "white"
    @graph.ctx.lineWidth = 1
    @graph.ctx.beginPath()
    @graph.ctx.setLineDash([2, 3])
    heightVLine = @graph.canvas.height * (THRESH_VOLTAGE - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)
    @graph.ctx.moveTo(0, @graph.canvas.height - heightVLine)
    @graph.ctx.lineTo(@graph.canvas.width, @graph.canvas.height - heightVLine)
    @graph.ctx.stroke()
    @graph.ctx.setLineDash(null)

  noteSpike: (t) =>
    @graph.drawRegion {start: t, end: t+2}, "white", 1.0

  noteInh: (tStart, tEnd) =>
    @graph.drawRegion {start: tStart, end: tStart+1}, "red"

  addV: (points) =>
    @graph.drawLine points, "green"

  addU: (points) =>
    @graph.drawLine points, "yellow"

globalTime = 0

#Tracks voltage, u, etc.
class History
  constructor: (@idx, @name) ->
    @v = []
    @u = []
    @inh = []
    @spike = []
  
  record: (v, u, isInh, isSpike) ->
    @v.push v
    @u.push u
    @inh.push isInh
    @spike.push isSpike

#History -> Object to work on
#startTime -> Where do we start? (Relative to globalTime)
#zoom -> What is our zoom ratio [0..1.0] where 0 is 1ks/size and 1 is 5ks/size
compressAndFitHistory = (history, startTime, zoom, size) ->
  #Start values at the startTime
  v = history.v.slice startTime
  u = history.u.slice startTime
  inh = history.inh.slice startTime
  spike = history.spike.slice startTime

  if zoom == 1
    output =
      v: v
      u: u
      inh: inh
      spike: spike
    output.v.splice size
    output.u.splice size
    output.inh.splice size
    output.spike.splice size
    return output

class ScrollingNeuronGraph
  #Divider id and a history object
  constructor: (id, @history) ->
    @graph = new NeuronGraph id
    @startTime = 0
    @trackEnd = false

  draw: =>
      #Not filled up whole window
    tempHistory = compressAndFitHistory @history, @startTime, 1, @graph.size()
    @graph.begin()
    @graph.addV tempHistory.v
    @graph.addU tempHistory.u

    for x, i in tempHistory.spike 
      @graph.noteSpike i if x == true

    for x, i in tempHistory.inh 
      @graph.noteInh i if x == true

    #Follow the updates?
    if @trackEnd == true
      if tempHistory.v.length >= @graph.size()
        @startTime += 1

$ ->
  neuronA = new History 0, "neuronA"
  graph = new ScrollingNeuronGraph "graphA", neuronA

  running = false
  globalTime = 0
  #render = ->
    #return if !running
    #$("#time").html globalTime
    #globalTime += 500
    #for x in [1..500]
      #neuronA.record Math.random()*0.2, 0, Math.random() > 0.7 ,Math.random() > 0.9
    #graph.draw()

  #setInterval render, 500

  $("#start").click ->
    running = true
    graph.trackEnd = true
  $("#stop").click ->
    running = false
    graph.trackEnd = false

  $("#goto button").click ->
    pos = parseInt($("#goto input").val())
    graph.startTime = pos
    graph.draw()

  socket = io.connect 'http://localhost:80'

  socket.on 'data', (data) ->
    return if !running
    data = JSON.parse data

    $("#time").html globalTime
    globalTime += 10
    neuronA.record data.v, data.u, 0, 0
    graph.draw()

