input do 
  glu "input", :debug => true, :count => 2
  glu "signal", :debug => true
end 

output do
end

main do
  glu_signal "output", :debug => true
end

connect "signal", "output", :one_to_one, :delay => 10
connect "input", "output", :many_to_one, :weight => 0
