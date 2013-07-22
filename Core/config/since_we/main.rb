require './std.rb'
require './train/product/lexicon.rb'

input do
  no_learn "input", :count => LEXICON_COUNT
  glu_signal "signal", :count => LEXICON_COUNT
end

output do
  glu :output, :count => LEXICON_COUNT
end

connect :signal, :output, :linear, :delay => 2
connect :input, :output, :many_to_many, :weight => 0
