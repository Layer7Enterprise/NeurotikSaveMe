DEFAULT_WEIGHT = 24.0
DEFAULT_DELAY = 1

class Connection
  @@dendrite_count = {}

  #Get the most number of dendrites ever used
  def self.count_max_dendrites
    out = @@dendrite_count.max_by {|k, v| v}

    if out.nil?
      puts "Error.  No connections!"
      exit
    end

    return out[1]
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

def sizeof name
  begin_frame 0 do
    return Neuron.name_to_neuron(name).count
  end
end

@aliases = {}

def alias to_name, from_name, range
  if to_name[0] != "_"
    puts "Alias name (#{to_name}) must start with an underscore"
    exit
  end

  @aliases[to_name] = {:name => from_name, :range => range}
end

@structs = {}

def struct name, arr
  if name[0] != "*"
    puts "Structure name must start with a *"
    exit
  end

  @structs[name] = arr
end

def connect from, to, method, params=nil
  begin_frame 1 do
    from_set = []
    to_set = []

    if from[0] == "_"
      from_set = Neuron.name_to_neuron(@aliases[from][:name]).name_array
      range = @aliases[from][:range]
      from_set = from_set[range[0]..range[1]]
    elsif from[0] == "*"
      set_names = @structs[from]
      set_names.each do |set_name|
        from_set += Neuron.name_to_neuron(set_name).name_array
      end
    else
      from_set = Neuron.name_to_neuron(from).name_array
    end

    if to[0] == "_"
      to_set = Neuron.name_to_neuron(@aliases[to][:name]).name_array
      range = @aliases[to][:range]
      to_set = to_set[range[0]..range[1]]
    elsif to[0] == "*"
      set_names = @structs[to]
      set_names.each do |set_name|
        to_set += Neuron.name_to_neuron(set_name).name_array
      end
    else
      to_set = Neuron.name_to_neuron(to).name_array
    end

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

def point_to_point from, to, params=nil
  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY

  from_index = params[:from_index]
  to_index = params[:to_index]

  if from_index.nil?
    puts "No from index"
    exit
  end

  if to_index.nil?
    puts "No to index"
    exit
  end

  if from_index-1 > from.count
    puts "From was not large enough to meet that index"
    exit
  end


  if to_index-1 > to.count
    puts "To was not large enough to meet that index"
    exit
  end

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
  end

  connections = []
  connection = Connection.new from[from_index], to[to_index]
  connection.set_weight(weight)
  connection.set_delay(delay)
  connections << connection

  return connections
end

def all_to_some from, to, params=nil
  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY
  percent = 50
  count = -1

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
    percent = params[:percent] unless params[:percent].nil?
    count = params[:count] unless params[:count].nil?
  end

  if count <= 0
    connections = []
    from.count.times do |index|
      to.count.times do |index2|
        if rand(100) > percent
          next
        end

        connection = Connection.new from[index], to[index2]
        connection.set_weight(weight)
        connection.set_delay(delay)
        connections << connection
      end
    end
  else
    connections = []
    from.count.times do |index|
      count.times do
        connection = Connection.new from[index], to[rand(to.count)]
        connection.set_weight(weight)
        connection.set_delay(delay)
        connections << connection
      end
    end
  end

  return connections
end

def some_to_many from, to, params=nil
  if from.count <= 1
    puts "#{from.count} <= 1 for all_to_some"
    exit
  end

  if to.count <= 1
    puts "#{to.count} <= 1 for all_to_some"
    exit
  end

  weight = DEFAULT_WEIGHT
  delay = DEFAULT_DELAY
  percent = 50
  count = -1

  unless params.nil?
    weight = params[:weight] unless params[:weight].nil?
    delay = params[:delay] unless params[:delay].nil?
    percent = params[:percent] unless params[:percent].nil?
    count = params[:count] unless params[:count].nil?
  end

  count = count.to_i
  percent = percent.to_i

  if count <= 0
    connections = []
    to.count.times do |index|
      from.count.times do |index2|
        roll = rand(100)
        if roll >= percent
          next
        end

        connection = Connection.new from[index2], to[index]
        connection.set_weight(weight)
        connection.set_delay(delay)
        connections << connection
      end
    end
  else
    connections = []
    to.count.times do |index|
      count.times do
        connection = Connection.new from[rand(from.count)], to[index]
        connection.set_weight(weight)
        connection.set_delay(delay)
        connections << connection
      end
    end
  end

  return connections
end
