input do 
  glu "input", :count => 10
  glu "signal", :debug => true, :count => 3
end 

output do
  glu_signal "output", :debug => true, :count => 3
end

main do
  no_learn "layer0", :count => 20
end

#Arbitrary connections
#connect "input", "layer0", :some_to_many, :count => 2, :weight => 12

#Establish output
connect "signal", "output", :linear, :delay => 6
connect "input", "layer0", :some_to_many, :count => 1
connect "layer0", "output", :many_to_many, :weight => 0
