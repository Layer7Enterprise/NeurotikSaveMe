require './std.rb'
#require './train/product/lexicon.rb'
#

input do
  no_learn "neuronA", :debug => true
  no_learn "neuronB", :debug => true
  no_learn "neuronC", :debug => true
  glu_signal "signal", :count => 1, :debug => true
end

output do
  glu :output, :count => 1, :debug => true
end

#filters = {}
#buffers = {}
#count = 0
#@lexicon_symbols = @lexicon_symbols.find_all {|x| /[[:upper:]]/.match(x[2]) != nil}

#@lexicon_symbols.each_with_index do |symbol, index|
  #puts "#{index+1} / #{@lexicon_symbols.count} #{symbol}"

  #filters[symbol] = (gen_filter :name => "S#{symbol}_filter", :input => "input", :watch => symbol, :input_delay => 30, :open_length => 30)

  ##Build question
  #question_check = "S#{symbol}QQ"
  #main { glu_signal question_check }
  #connect "#{symbol}", question_check, :one_to_one, :weight => 12.0
  #connect "_L?", question_check, :one_to_one, :weight => 12.0

  #buffers[symbol] = gen_packetizer :name => "S#{symbol}_buffer", :input_name => filters[symbol], :watch => question_check

  ##connect buffers[symbol], :output, :linear
#end

#Basic input layer
#connect :input, :input, :many_to_many, :weight => 0, :delay => 20

#Inhibit initial response
#main{gaba :inhibit_initial, 12}
#connect :signal, :inhibit_initial, :many_to_one, :delay => 20
#connect :inhibit_initial, :output, :one_to_many

#Inhibit multi-step
#main{gaba :stop_loopback, 12}
#connect :input, :stop_loopback, :many_to_one
#connect :stop_loopback, :input, :one_to_many

main {
  glu :outputA, :debug => true
  glu :outputB, :debug => true
  glu :outputC, :debug => true
}

connect "neuronA", :outputA, :one_to_one, :weight => 0
connect :neuronB, :outputA, :one_to_one, :weight => 0

connect :neuronB, :outputB, :one_to_one, :weight => 0
connect :neuronC, :outputB, :one_to_one, :weight => 0

connect :neuronC, :outputC, :one_to_one, :weight => 0
connect :neuronA, :outputC, :one_to_one, :weight => 0

connect :signal, :outputA, :linear, :nelay => 4
connect :signal, :outputB, :linear, :nelay => 4
connect :signal, :outputC, :linear, :nelay => 4
