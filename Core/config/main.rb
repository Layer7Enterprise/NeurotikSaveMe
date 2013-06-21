input do 
 glu "input", :count => 2
 glu_signal "signal", :debug => true, :count => 3
end 

output do
  glu "output", :debug => true, :count => 3
end

main do
end

connect "signal", "output", :linear, :delay => 5
connect "input", "output", :many_to_many, :weight => 0
