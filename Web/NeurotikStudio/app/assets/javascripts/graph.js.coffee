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
    @graph.drawRegion {start: tStart, end: t+1}, "red"

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
    #Find up to the size if possible
    cutOffNum = v.length - size
    return output if cutOffNum <= 0

    output.v.splice cutOffNum
    output.u.splice cutOffNum
    output.inh.splice cutOffNum
    output.spike.splice cutOffNum
    return output

$ ->
  neuronA = new History 0, "neuronA"

  graph = new NeuronGraph "graphA"

  #Make up some data
  for x in [1..2000]
    neuronA.record Math.random()*1.0, Math.random()*1.0, Math.random() > 0.0,Math.random() > 0.9

  #Should be ... nil
  tempHistory = compressAndFitHistory neuronA, 0, 1, graph.size()

  graph.begin()
  graph.addV tempHistory.v
  graph.addU tempHistory.u

  for x, i in tempHistory.spike 
    graph.noteSpike i if x == true

  for x, i in tempHistory.inh 
    if x == true
      alert 'yes'
    graph.noteInh i if x == true


  #neuralGraphA = new NeuronGraph "graphA" 
  #neuralGraphB = new NeuronGraph "graphB" 
  #neuralGraphC = new NeuronGraph "graphC" 
  #delta = 0
  #render  = (neuralGraph) ->
    #neuralGraph.begin()
    #neuralGraph.noteSpike 100
    #neuralGraph.noteInh 150, 300

    #data = new Array(neuralGraph.size()+1)
    #data[0] = 0
    #for x, i in data
      #continue if i == 0
      #data[i] = Math.sin((i + delta) / 10)
      #data[i] /= 4
      #data[i] += 0.5

    #neuralGraph.addV data
    ##neuralGraph.addU data2
    #delta += 1.52

  #update = ->
    #render neuralGraphA
    #render neuralGraphB
    #render neuralGraphC

  #setInterval update, 60 / 1000

  #graph = new Graph "graph"

  #delta = 0
  #renderLoop = ->

    #graph.render()
    #graph.drawRegion {start: 0, end: 400}, "orange"
    #graph.drawRegion {start: 300, end: 500}, "red"
    #graph.drawRegion {start: 500, end: 600}, "white"

    
    #graph.drawLine data, "#DDD"

    #delta += 1.3

  #setInterval renderLoop, 20 / 1000

  ##canvas = document.getElementById("graph")
  ##ctx = canvas.getContext '2d'

  ##ctx.fillStyle = '#333'
  ##ctx.fillRect 0, 0, canvas.width, canvas.height

  ##ctx.moveTo 100, 150
  ##ctx.lineTo 400, 400
  ##ctx.stroke()
