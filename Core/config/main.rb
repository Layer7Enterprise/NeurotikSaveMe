count = 18

input do
  glu "input", :count => count
  glu_signal "signal", :count => count
end

output do
  no_learn :output, :count => count
end

def grab var
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
  main {no_learn latch, :count => count, :debug => true}
  connect @input_name, latch, :linear
  connect latch, latch, :linear, :delay => 20

  #Set up disgarding old data
  ###################################################
  free_inh = mangle(:discard_inh)
  main {gaba free_inh, 24}
  connect free_inh, latch, :one_to_many
  connect @watch_name, free_inh, :one_to_one

  #Set up output
  ###################################################
  output = mangle(:output)
  count = sizeof(@input_name)
  main {no_learn output, :count => count, :debug => true}
  connect latch, output, :linear, :delay => 27

  #Inhibit the output
  output_inh = mangle(:output_inh)
  main {gaba output_inh, 21, :debug => true}
  connect latch, output_inh, :many_to_one
  connect output_inh, output, :one_to_many

  #Inhibit the inhibitor
  output_inh_disable = mangle :output_inh_disable
  main {gaba output_inh_disable, 48, :debug => true}
  connect output_inh_disable, output_inh, :one_to_one
  connect @watch_name, output_inh_disable, :one_to_one

  return output

  #Set up the outputpass through
  ###################################################
  #count = sizeof(@input_name)
  #output = mangle(:output)
  #output_inh = mangle(:output_inh)
  #output_inh_disable = mangle(:output_inh_disable)

  #main {
    #no_learn output, :count => count, :debug => true
    #gaba output_inh, 20
    #gaba output_inh_disable, 40 #Allows output
  #}

  ##Enabler
  #connect output_inh, output, :one_to_many #Disable output
  #connect @input_name, output_inh, :many_to_one #Fake IB
  #connect output_inh_disable, output_inh, :one_to_one #Enable output
  #connect @watch_name, output_inh_disable, :one_to_one #Enable output
  #connect output_inh_disable, scatter_latches[0], :one_to_many #Enable output

  #connect latch, output, :linear, :delay => 23

  #return output
end

main {alien "_if", "input", [0, 0]}

output = gen_hold_buffer :name => "Test", :input_name => "input", :watch_name => "_if"

connect output, :output, :linear
