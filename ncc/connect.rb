class Connection
  def initialize from, to
    @from = from
    @to = to
    @delay = 1
    @weight = 0
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
    exit -1
  end

  connections = []
  from.count.times do |index|
    connection = Connection.new from[index], to[index]
    connections << connection
  end

  return connections
end

#connect :a, :b, :one_to_one, nil
