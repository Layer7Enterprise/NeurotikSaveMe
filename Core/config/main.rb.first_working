require './std.rb'
require './train/product/lexicon.rb'

input do
  no_learn "inputA", :count => LEXICON_COUNT
  glu_signal "signalA", :count => LEXICON_COUNT

  no_learn "inputB", :count => LEXICON_COUNT
  glu_signal "signalB", :count => LEXICON_COUNT

  no_learn "inputC", :count => LEXICON_COUNT
  glu_signal "signalC", :count => LEXICON_COUNT

end

glu :outputA, :count => LEXICON_COUNT
glu :outputB, :count => LEXICON_COUNT
glu :outputC, :count => LEXICON_COUNT

output do
  no_learn :output, :count => LEXICON_COUNT
end

#filters = {}
#buffers = {}
#count = 0

#@lexicon_symbols = @lexicon_symbols.find_all {|x| /[[:upper:]]/.match(x[2]) != nil}
#@lexicon_symbols = ["_LSOLUTION"]

#@lexicon_symbols.each_with_index do |symbol, index|
  #puts "#{index+1} / #{@lexicon_symbols.count} #{symbol}"

  #filters[symbol] = (gen_filter :name => "S#{symbol}_filter", :input => "input", :watch => symbol, :input_delay => 10, :open_length => 15)

  ##Build question
  #question_check = "S#{symbol}QQ"
  #glu_signal question_check
  #connect "#{symbol}", question_check, :one_to_one, :weight => 12.0
  #connect "_LQQ", question_check, :one_to_one, :weight => 12.0

  #buffers[symbol] = gen_packetizer :name => "S#{symbol}_buffer", :input_name => filters[symbol], :watch => question_check

  #serialized = gen_serialize :name=> "S#{symbol}_serialize", :input => buffers[symbol]
  #connect serialized, :output, :linear
#end

connect :signalA, :outputA, :linear, :delay => 2
connect :inputA, :outputA, :many_to_many, :weight => 0
connect :inputB, :inputA, :linear
connect :inputC, :inputA, :linear

connect :signalB, :outputB, :linear, :delay => 10
connect :outputA, :outputB, :many_to_many, :weight => 0

connect :outputB, :outputC, :many_to_many, :weight => 0
connect :outputA, :outputC, :many_to_many, :weight => 0

connect :signalC, :outputC, :linear, :delay => 10

connect :inputA, :output, :linear
connect :outputA, :output, :linear
connect :outputB, :output, :linear
connect :outputC, :output, :linear

#connect :output1, :output, :linear

##Setup the primary lex grap3
#connect :signal, :input, :linear, :delay => 23
#connect :input, :input, :many_to_many, :weight => 0, :delay => 20

##Prevent loopback
#gaba :no_loopback, 25
#connect :input, :no_loopback, :many_to_one, :delay => 2n
#connect :no_loopback, :input, :one_to_many

##Level 0 POS detector
#goal_detector = gen_filter :name => "goal_detector", :input => "input", :watch => :_LGOAL, :input_delay => 30, :open_length => 60
##muscle_group_detector = gen_filter :name => "muscle_group_detector", :input => "input", :watch => "_LMUSCLE_GROUP", :input_delay => 30, :open_length => 60

##Packetizers
#goal_packetizer = gen_packetizer :name => "goal_packetizer", :input => goal_detector, :watch => "_LPERIOD" #muscle_group_packetizer = gen_packetizer :name => "muscle_group_packetizer", :input => muscle_group_detector, :watch => "_LPERIOD"
##goal_packetizer = gen_packetizer :name => "goal_packetizer", :input => goal_detector, :watch => "_LPERIOD" #muscle_group_packetizer = gen_packetizer :name => "muscle_group_packetizer", :input => muscle_group_detector, :watch => "_LPERIOD"

##lex graphs
#goal_lex = gen_lex_graph :name => "goal", :input => goal_packetizer, :signal => "signal", :debug => true
##muscle_group_lex = gen_lex_graph :name => "muscle_group", :input => muscle_group_packetizer, :signal => "signal"

#connect :input, :output, :linear
