require './std.rb'

count = 21

main {alien "_IF", "input", [8, 8]}
main {alien "_PRONOUN", "input", [9, 9]}
main {alien "_IS", "input", [10, 10]}
main {alien "_ADJ", "input", [11, 11]}
main {alien "_THEN", "input", [12, 12]}
main {alien "_VERB", "input", [13, 13]}
main {alien "_NOUN", "input", [14, 14]}
main {alien "_VERB_OBJECT", "input", [15, 15]}
main {alien "_OBJECT", "input", [16, 16]}

main {alien "_PRONOUNQQ", "input", [17, 17]}
main {alien "_ISQQ", "input", [18, 18]}
main {alien "_ADJQQ", "input", [19, 19]}
main {alien "_THENQQ", "input", [20, 20]}
main {alien "_VERBQQ", "input", [21, 21]}
main {alien "_NOUNQQ", "input", [22, 22]}
main {alien "_VERB_OBJECTQQ", "input", [23, 23]}
main {alien "_OBJECTQQ", "input", [24, 24]}
main {alien "_IFQQ", "input", [25, 25]}

input do
  glu "input", :count => count, :debug => true
  glu_signal "signal", :count => count
end

output do
  no_learn :output, :count => count
end

noun_filter = gen_filter :name => "noun_filter", :input => "input", :watch => "_NOUN", :input_delay => 30, :open_length => 40
verb_filter = gen_filter :name => "verb_filter", :input => "input", :watch => "_VERB", :input_delay => 30, :open_length => 40
vo_filter = gen_filter :name => "vo_filter", :input => "input", :watch => "_VO", :input_delay => 30, :open_length => 40

noun_buffer = gen_packetizer :name => "noun_buffer", :input_name => noun_filter, :watch => "_OBJ"
verb_buffer = gen_packetizer :name => "verb_buffer", :input_name => verb_filter, :watch => "_CALL"
vo_buffer = gen_packetizer :name => "vo_buffer", :input_name => vo_filter, :watch => "_CALL2"

#Basic input layer
connect :input, :input, :many_to_many, :weight => 0
connect :signal, :input, :linear

connect verb_buffer, :output, :linear
connect noun_buffer, :output, :linear
connect vo_buffer, :output, :linear
