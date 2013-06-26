input do 
  glu "input", :debug => true, :count => 11
  glu "signal", :debug => true, :count => 11
end 

output do
  glu_signal "output", :debug => true, :count => 11
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
