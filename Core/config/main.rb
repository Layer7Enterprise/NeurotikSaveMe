input do 
  glu "input", :debug => true, :count => 10
  glu "signal", :debug => true
end 

output do
  glu_signal "output", :debug => true
end

main do
end

connect "signal", "output", :one_to_one, :delay => 10
connect "input", "output", :many_to_one, :weight => 0
