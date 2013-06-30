require './std.rb'
require './train/product/lexicon.rb'

input do
  glu "input", :count => LEXICON_COUNT, :debug => true
  glu_signal "signal", :count => LEXICON_COUNT
end

output do
  no_learn :output, :count => LEXICON_COUNT
end

noun_filter = gen_filter :name => "noun_filter", :input => "input", :watch => "_LNOUN", :input_delay => 30, :open_length => 40
verb_filter = gen_filter :name => "verb_filter", :input => "input", :watch => "_LVERB", :input_delay => 30, :open_length => 40

noun_buffer = gen_packetizer :name => "noun_buffer", :input_name => noun_filter, :watch => "_LNOUN"
verb_buffer = gen_packetizer :name => "verb_buffer", :input_name => verb_filter, :watch => "_LVERB"

#Basic input layer
connect :input, :input, :many_to_many, :weight => 0
connect :signal, :input, :linear

connect verb_buffer, :output, :linear
connect noun_buffer, :output, :linear
