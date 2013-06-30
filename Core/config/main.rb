count = 18

input do 
  glu "input", :count => count
  glu_signal "signal", :count => count
end 

main do
end

output do
  glu "output", :count => count
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
  def mangle sub
    return @name + sub.to_s
  end

  #Should track debug
  isDebug = false
  if params
    if params[:debug] == true
      isDebug = true
    end
  end

  #Set up network
  ###################################################

  #Feed inputs directly into a scatter-latch network
  glu mangle(:test), :count => sizeof(@input_name)
end

gen_hold_buffer :name => "Test", :input_name => "input", :watch_name => "input"
