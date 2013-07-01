require './std.rb'
require './train/product/lexicon.rb'

input do
  glu "input", :count => LEXICON_COUNT, :debug => true
  glu_signal "signal", :count => LEXICON_COUNT
end

output do
  no_learn :output, :count => LEXICON_COUNT
end

filters = {}
buffers = {}
count = 0
@lexicon_symbols = @lexicon_symbols.find_all {|x| /[[:upper:]]/.match(x[2]) != nil}

@lexicon_symbols.each_with_index do |symbol, index|
  puts "#{index+1} / #{@lexicon_symbols.count} #{symbol}"

  filters[symbol] = (gen_filter :name => "S#{symbol}_filter", :input => "input", :watch => symbol, :input_delay => 30, :open_length => 30)

  #Build question
  question_check = "S#{symbol}QQ"
  main { glu_signal question_check }
  connect "#{symbol}", question_check, :one_to_one, :weight => 12.0
  connect "_L?", question_check, :one_to_one, :weight => 12.0

  buffers[symbol] = gen_packetizer :name => "S#{symbol}_buffer", :input_name => filters[symbol], :watch => question_check

  connect buffers[symbol], :output, :linear
end

#Basic input layer
connect :input, :input, :many_to_many, :weight => 0
connect :signal, :input, :linear, :delay => 3

connect :input, :output, :linear
