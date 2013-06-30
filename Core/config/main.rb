require './std.rb'

count = 18

main {alien "_if", "input", [0, 0]}
main {alien "_IF", "input", [8, 8]}
main {alien "_PRONOUN", "input", [9, 9]}
main {alien "_TOBE", "input", [10, 10]}
main {alien "_ADJ", "input", [11, 11]}
main {alien "_THEN", "input", [12, 12]}
main {alien "_VERB", "input", [13, 13]}
main {alien "_NOUN", "input", [14, 14]}
main {alien "_VO", "input", [15, 15]}
main {alien "_OBJ", "input", [16, 16]}
main {alien "_CALL", "input", [17, 17]}

input do
  glu "input", :count => count, :debug => true
  glu_signal "signal", :count => count
end

output do
  no_learn :output, :count => count
end

output = gen_packetizer :name => "Packet", :input_name => "input", :watch => "_if"

connect :input, :input, :many_to_many, :weight => 0
connect :signal, :input, :linear

connect output, :output, :linear
