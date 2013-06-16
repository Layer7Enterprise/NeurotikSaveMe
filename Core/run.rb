require 'colorize'

Dir.chdir 'config'
  system('ncc')
Dir.chdir '../'

sleep 1

def start name
  @lastName = name
end

def error
  puts "\n#################################".red
  puts "!!!Error!!! -- #{@lastName} Failed".yellow
  puts "#################################\n".red
  exit
end

Dir.chdir 'build'

`make clean`

start 'cmake'
  output = `cmake ..`
  unless $?.success?
    puts output
    puts "CMake failed!"
      exit
  end

start 'make'
  puts `make`
  unless $?.success?
    puts output
    error
  end

system 'clear'

start 'Running'
Dir.chdir '../'
exec './build//neurotik'
