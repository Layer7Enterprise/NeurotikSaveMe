def gen_packetizer params=nil
  @name = params[:name]
  @input_name = params[:input_name]
  @watch = params[:watch]

  def check var, name
    if var.nil?
      puts "gen_packetizer, #{name} was nil"
      exit
    end
  end

  check @input_name, "input_name"
  check @watch, "watch"

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
  main {gaba input, 40, :count => count}
  connect @input_name, input, :linear

  #Set up level primary inhibitors
  ###################################################
  primary_inh = mangle(:primary_inh)
  count = sizeof(@input_name)
  main {gaba primary_inh, 3, :count => count, :ib => 2}
  connect input, primary_inh, :linear

  #Set up main excitory (main_exc)
  ###################################################
  main_exc = mangle(:main_exc)
  count = sizeof(@input_name)
  main {glu main_exc, :count => count}
  connect primary_inh, main_exc, :linear

  #Set up main intrinsic burster (main_ib)
  ###################################################
  main_ib = mangle(:main_ib)
  main {glu_signal main_ib, :ib => 25}
  connect main_ib, main_exc, :one_to_many

  #Set up sync output
  ###################################################
  sync = mangle(:sync)
  count = sizeof(@input_name)
  main {glu_signal sync, :count => count}
  connect main_exc, sync, :linear
  connect sync, input, :linear

  #Set up final stage isolation
  ###################################################
  output = mangle(:output)
  count = sizeof(@input_name)
  main {glu_signal output, :count => count}
  connect sync, output, :linear, :delay => 40

  #Set up final stage isolation enabler
  en = mangle(:en)
  disable = mangle(:disable)
  main {gaba en, 100}
  main {gaba disable, 3, :ib => 2}
  connect en, disable, :one_to_one
  connect en, sync, :one_to_many, :delay => 5
  connect @watch, en, :one_to_one
  connect disable, output, :one_to_many

  return output
end

#Allow things to pass through?
def gen_filter params=nil
  #Get Paramaters
  ###################################################
  if params.nil?
    puts "gen_filter, paramaters nil"
    exit
  end

  @name = params[:name]
  @input = params[:input]
  @watch = params[:watch]
  @input_delay = params[:input_delay]  #How long of a delay to add to input
  @open_length = params[:open_length]  #How long will the filter stay open for?
  @debug = params[:debug]
  @debug = false if @debug.nil?

  def check var, name
    if var.nil?
      puts "gen_filter, #{name} was nil"
      exit
    end
  end

  check @input, "input"
  check @watch, "watch"
  check @input_delay, "input_delay"
  check @open_length, "open_length"

  #Generate a unique identifier
  def mangle *subs
    full_sub = ""
    subs.each do |sub|
      full_sub += sub.to_s
    end

    return @name + full_sub.to_s
  end

  #Output layer
  ###################################################
  output = mangle(:output)
  count = sizeof(@input)
  debug = @debug
  main { glu_signal output, :count => count, :debug => debug}
  connect @input, output, :linear, :delay => @input_delay

  #Watch circuit
  ###################################################
  watch = mangle(:watch)
  ib = mangle(:ib)
  main {
    gaba ib, 3, :ib => 2
    gaba watch, @open_length
  }

  connect @watch, watch, :one_to_one
  connect watch, ib, :one_to_one
  connect ib, output, :one_to_many

  return output
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
  main {gaba free_inh, 25}
  connect free_inh, latch, :one_to_many
  connect @watch_name, free_inh, :one_to_one, :delay => 1

  #Set up output
  ###################################################
  output = mangle(:output)
  count = sizeof(@input_name)
  main {glu_signal output, :count => count}
  connect latch, output, :linear, :delay => 33

  #Inhibit the output
  output_inh = mangle(:output_inh)
  main {gaba output_inh, 3, :ib => 2}
  connect output_inh, output, :one_to_many, :delay => 5

  #Inhibit the inhibitor
  output_inh_disable = mangle :output_inh_disable
  main {gaba output_inh_disable, 50}
  connect output_inh_disable, output_inh, :one_to_one
  connect @watch_name, output_inh_disable, :one_to_one

  return output
end

