input do 
  glu "input", :count => 10
  glu "signal", :debug => true, :count => 3
end 

output do
  glu_signal "output", :debug => true, :count => 3
end

main do
  no_learn "layer0", :count => 30
  no_learn "layer1", :count => 30
end

#Arbitrary connections
#connect "input", "layer0", :some_to_many, :count => 2, :weight => 12

#Establish output
connect "signal", "output", :linear, :delay => 9

connect "input", "layer0", :some_to_many, :count => 3, :weight => 12
connect "layer0", "layer1", :linear
connect "layer1", "output", :many_to_many, :weight => 0
