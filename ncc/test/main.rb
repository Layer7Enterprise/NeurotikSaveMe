connect :test, :testA, :one_to_one

input do
  glu :test, :count => 2
end

output do
  glu :testA, :count => 2, :debug => true
end

if ARGV.count != 1
  puts "ncc [input]"
  exit
end

