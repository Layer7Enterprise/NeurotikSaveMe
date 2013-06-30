count = 18

input do
  glu "input", :count => count
  glu_signal "signal", :count => count
end

output do
  no_learn :output, :count => count
end

def gen_packetizer params=nil
  @name = params[:name]
  @input_name = params[:input_name]

  def check var, name
    if var.nil?
      puts "gen_packetizer, #{name} was nil"
      exit
    end
  end

  check @input_name, "input_name"

  #Generate unique identifier
  def mangle *subs
    full_sub = ""
    subs.each do |sub|
      full_sub += sub.to_s
    end

    return @name + full_sub.to_s
  end

  #Set up input
  ###################################################
  input = mangle(:input)
  count = sizeof(@input_name)
  main {gaba input, 30, :count => count}
end

def gen_hold_buffer params=nil
  #Get Paramaters
  ###################################################
  if params.nil?
    puts "gen_hold_buffer, paramaters nil"
    exit
  end

  @name = params[:name]
  @input_name = params[:input_name]
  @watch_name = params[:watch_name]

  def check var, name
    if var.nil?
      puts "gen_hold_buffer, #{name} was nil"
      exit
    end
  end

  check @input_name, "input_name"
  check @watch_name, "watch_name"

  #Generate a unique identifier
  def mangle *subs
    full_sub = ""
    subs.each do |sub|
      full_sub += sub.to_s
    end

    return @name + full_sub.to_s
  end

  #Set up scatter latch network
  ###################################################
  latch = mangle(:scatter_latch)

  count = sizeof(@input_name)
  main {no_learn latch, :count => count}
  connect @input_name, latch, :linear
  connect latch, latch, :linear, :delay => 20

  #Set up disgarding old data
  ###################################################
  free_inh = mangle(:discard_inh)
  main {gaba free_inh, 25, :debug => true}
  connect free_inh, latch, :one_to_many
  connect @watch_name, free_inh, :one_to_one, :delay => 1

  #Set up output
  ###################################################
  output = mangle(:output)
  count = sizeof(@input_name)
  main {glu_signal output, :count => count, :debug => true}
  connect latch, output, :linear, :delay => 33

  #Inhibit the output
  output_inh = mangle(:output_inh)
  main {gaba output_inh, 3, :ib => 2, :debug => true}
  connect output_inh, output, :one_to_many, :delay => 5

  #Inhibit the inhibitor
  output_inh_disable = mangle :output_inh_disable
  main {gaba output_inh_disable, 50, :debug => true}
  connect output_inh_disable, output_inh, :one_to_one
  connect @watch_name, output_inh_disable, :one_to_one

  return output
end

main {alien "_if", "input", [0, 0]}

output = gen_hold_buffer :name => "Test", :input_name => "input", :watch_name => "_if"

connect output, :output, :linear
