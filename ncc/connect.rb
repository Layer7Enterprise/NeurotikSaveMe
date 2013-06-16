DEFAULT_WEIGHT = 24.0
DEFAULT_DELAY = 1

class Connection
  @@dendrite_count = {}

  #Get the most number of dendrites ever used
  def self.count_max_dendrites
    @@dendrite_count.max_by {|k, v| v}[1]
  end

  def initialize from, to
    @from = from
    @to = to
    @delay = 1
    @weight = 0

    if @@dendrite_count[to] != nil
      @@dendrite_count[to] += 1
    else
      @@dendrite_count[to] = 1
    end
  end

  def set_weight weight
    @weight = weight
  end

  def set_delay delay
    @delay = delay
  end

  def to_code
    code = "Connection\n"
    code += "From #{@from}\n"
    code += "To #{@to}\n"
    code += "Weight #{@weight}\n"
    code += "Delay #{@delay}\n"
    code += "End End\n"
  end
end

def connect from, to, method, params=nil
  begin_frame 1 do
    from_set = Neuron.name_to_neuron(from).name_array
    to_set = Neuron.name_to_neuron(to).name_array

    connection_array = send method, from_set, to_set, params

    connection_array.each do |connection|
      write connection.to_code
    end
  end
end

def one_to_one from, to, params=nil
  if from.count != to.count
    puts "#{from.count} != #{to.count} for one_to_one"
    exit
  end

  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
  end

  connections = []
  from.count.times do |index|
    connection = Connection.new from[index], to[index]
    connection.set_weight(weight)
    connection.set_delay(delay)
    connections << connection
  end

  return connections
end

def one_to_many from, to, params=nil
  if from.count != 1
    puts "#{from.count} != 1 for one_to_many"
    exit
  end

  if to.count <= 1
    puts "#{to.count} <= 1 for one_to_many"
    exit
  end

  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
  end

  connections = []
  to.count.times do |index|
    connection = Connection.new from[0], to[index]
    connection.set_weight(weight)
    connection.set_delay(delay)
    connections << connection
  end

  return connections
end

def many_to_one from, to, params=nil
  if from.count <= 1
    puts "#{from.count} <= 1 for many_to_one"
    exit
  end

  if to.count != 1
    puts "#{to.count} != 1 for many_to_one"
    exit
  end

  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
  end

  connections = []
  from.count.times do |index|
    connection = Connection.new from[index], to[0]
    connection.set_weight(weight)
    connection.set_delay(delay)
    connections << connection
  end

  return connections
end

def many_to_many from, to, params=nil
  if from.count <= 1
    puts "#{from.count} <= 1 for many_to_many"
    exit
  end

  if to.count <= 1
    puts "#{to.count} <= 1 for many_to_many"
    exit
  end

  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
  end

  connections = []
  from.count.times do |index|
    to.count.times do |index2|
      connection = Connection.new from[index], to[index2]
      connection.set_weight(weight)
      connection.set_delay(delay)
      connections << connection
    end
  end

  return connections
end

def linear from, to, params=nil
  if from.count != to.count
    puts "#{from.count} != #{to.count} for linear"
    exit
  end

  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
  end

  connections = []
  from.count.times do |index|
    connection = Connection.new from[index], to[index]
    connection.set_weight(weight)
    connection.set_delay(delay)
    connections << connection
  end

  return connections
end
