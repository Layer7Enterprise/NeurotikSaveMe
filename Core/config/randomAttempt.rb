input do 
  glu "input", :count => 10
  glu "signal", :debug => true, :count => 3
end 

output do
  glu_signal "output", :debug => true, :count => 3
end

main do
  glu_signal "layer0", :debug => true, :count => 10
  glu_signal "layer1", :debug => true, :count => 10
end


#Signal to layers
connect "signal", "layer0", :some_to_many, :delay => 10, :count => 1
connect "signal", "layer1", :some_to_many, :delay => 10, :count => 1

#Arbitrary connections
connect "input", "layer0", :some_to_many, :count => 3, :weight => 0
connect "layer0", "layer1", :some_to_many, :count => 5, :weight => 0

#Establish output
connect "signal", "output", :linear, :delay => 15
connect "layer1", "output", :many_to_many, :weight => 0
