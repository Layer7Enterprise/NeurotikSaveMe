count = 18
input do 
  glu "input", :debug => true, :count => count
  glu_signal "signal", :debug => true, :count => count
end 

output do
  glu "output", :debug => true, :count => count
end

main do
  #no_learn "layer0", :count => 200
  #no_learn "layer1", :count => 200
  #gaba "layer1i", 2, :count => 200
  #glu "layer2", :count => 200
end

connect "input", "input", :many_to_many, :weight => 0
connect "signal", "input", :linear, :delay => 1
connect "input", "output", :linear
