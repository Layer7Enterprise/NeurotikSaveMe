input do 
 glu "input", :count => 2
  glu "signal", :debug => true, :count => 3
end 

output do
  glu_signal "output", :debug => true, :count => 3
end

main do
 no_learn "inv", :count => 2, :debug => true
  gaba "invG", 5, :count => 2, :debug => true
end

offset = 0
connect "signal", "output", :linear, :delay => 5 + offset
connect "input", "output", :many_to_many, :weight => 0, :delay => 3 + offset

connect "input", "inv", :many_to_many, :delay => 4
connect "invG", "inv", :linear
connect "input", "invG", :linear
connect "inv", "output", :many_to_many, :weight => 0
