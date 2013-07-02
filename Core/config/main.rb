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
cojnt = 0
@lexicon_symbols = @lexicon_symbols.find_all {|x| /[[:upper:]]/.match(x[2]) != nil}
@lexicon_symbols = ["_LSOLUTION"]

@lexicon_symbols.each_with_index do |symbol, index|
  puts "#{index+1} / #{@lexicon_symbols.count} #{symbol}"

  filters[symbol] = (gen_filter :name => "S#{symbol}_filter", :input => "input", :watch => symbol, :input_delay => 10, :open_length => 15)

  #Build question
  question_check = "S#{symbol}QQ"
  main { glu_signal question_check }
  connect "#{symbol}", question_check, :one_to_one, :weight => 12.0
  connect "_LQQ", question_check, :one_to_one, :weight => 12.0

  buffers[symbol] = gen_packetizer :name => "S#{symbol}_buffer", :input_name => filters[symbol], :watch => question_check

  serialized = gen_serialize :name=> "S#{symbol}_serialize", :input => buffers[symbol]
  connect serialized, :output, :linear
end

main {
  gaba :input_die, 23
}

connect :input_die, :input, :one_to_many
connect :input, :input_die, :many_to_one, :delay => 30

connect :input, :input, :many_to_many, :weight => 0, :delay => 20
connect :signal, :input, :linear, :delay => 22
connect :input, :output, :linear
