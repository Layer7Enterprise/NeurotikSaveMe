require './std.rb'
require './train/product/lexicon.rb'

input do
  glu "input", :count => LEXICON_COUNT, :debug => true
  glu_signal "signal", :count => LEXICON_COUNT
end

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

#goal = gen_lex_graph :name => "GOAL", :input => :input, :signal => "signal"
#muscle_group = gen_lex_graph :name => "MUSCLE_GROUP", :input => :input, :signal => "signal"
#therefore = gen_lex_graph :name => "THEREFORE", :input => :input, :signal => "signal"

#Level 0 POS detector
goal_detector = gen_filter :name => "goal_detector", :input => "input", :watch => :_LGOAL, :input_delay => 10, :open_length => 15
muscle_group_detector = gen_filter :name => "muscle_group_detector", :input => "input", :watch => "_LMUSCLE_GROUP", :input_delay => 10, :open_length => 15
