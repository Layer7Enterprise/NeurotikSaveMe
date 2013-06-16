input do 
  glu "input", :debug => true, :count => 10
  glu "signal", :debug => true, :count => 3
end 

output do
  glu_signal "output", :debug => true, :count => 3
end

main do
end

connect "signal", "output", :linear, :delay => 10
connect "input", "output", :many_to_many, :weight => 0
