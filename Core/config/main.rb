input do 
 glu "input", :count => 2
 glu_signal "signal", :debug => true, :count => 2
end 

output do
  glu "output", :debug => true, :count => 2
end

main do
  no_learn "clock", :debug => true, :count => 1

  gaba "invG", 5, :debug => true, :count => 2

  no_learn "inv", :debug => true, :count => 2
  no_learn "inputA", :debug => true, :count => 2

  #Multi-layer
  no_learn "layer0", :debug => true, :count => 20
end

connect "input", "clock", :many_to_one
connect "clock", "inv", :one_to_many
connect "input", "invG", :linear
connect "invG", "inv", :linear

#Align input
connect "input", "inputA", :linear, :delay => 5

#Connect to forest
connect "inputA", "layer0", :some_to_many, :count => 1, :weight => 12

connect "inv", "layer0", :some_to_many, :count => 1, :weight => 12

#Connec to output
#connect "inputA", "output", :many_to_many, :weight => 0
#connect "inv", "output", :many_to_many, :weight => 0
connect "layer0", "output", :many_to_many, :weight => 0
connect "signal", "output", :linear, :delay => 13
