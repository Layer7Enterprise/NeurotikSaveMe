input do 
  glu "input", :count => 10
  glu "signal", :debug => true, :count => 3
end 

output do
  glu_signal "output", :debug => true, :count => 3
end

main do
  no_learn "layer0", :count => 200
  no_learn "layer1", :count => 200
  gaba "layer1i", 2, :count => 200
  glu "layer2", :count => 200
end

#nArbitrary connections
#connect "input", "layer0", :some_to_many, :count => 2, :weight => 12

#Establish output
connect "signal", "output", :linear, :delay => 11

connect "input", "layer0", :some_to_many, :count => 3, :weight => 12
connect "input", "layer0", :some_to_many, :count => 3, :weight => 12
connect "layer0", "layer1", :linear
connect "layer0", "layer1i", :linear
connect "layer1", "layer2", :all_to_some, :count => 3, :weight => 12
connect "layer1i", "layer2", :some_to_many, :percent => 3

connect "layer2", "output", :many_to_many, :weight => 0
