count = 18
input do 
  glu "input", :debug => true, :count => count
  glu_signal "signal", :count => count
end 

output do
  glu "output", :count => count
end

main do
end

connect "input", "input", :many_to_many, :weight => 0, :delay => 5
#connect "input", "input", :many_to_many, :delay => 3
#connect "input", "input", :many_to_many, :weight => 0, :delay => 8
#connect "input", "input", :many_to_many, :weight => 0, :delay => 3

connect "signal", "input", :linear, :delay => 8
connect "input", "output", :linear
