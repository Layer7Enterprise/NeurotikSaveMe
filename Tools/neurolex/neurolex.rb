require 'socket'

file = ARGV[0]
type = ARGV[1]
portFrom = ARGV[2]
portTo = ARGV[3]

if ARGV.count != 4
  puts "[file type(vector|words) portFrom portTo]"
  exit
end

@vectorSize = 0
@lexicon = {}
#Read lexicon file
File.open(file, "r") do |file|
  #First line should be a length
  @vectorSize = file.readline.to_i

  while !file.eof
    line = file.readline
    line_split = line.split(" ")
    index = line_split[0]
    word = line_split[1]
    puts "#{index} => #{word}"
    @lexicon[index.to_i] = word
  end
end
@invertedLexicon = @lexicon.invert

#10010 => wordA wordC
def vectorToWords vector
  words = []

  vector.split("").each_with_index do |x, i|
    if vector[i] == "1"
      words << @lexicon[i]
    end
  end

  words = words.join(" ")
  return words
end

#wordA wordC => "10010"
def wordsToVector words
  vector = "0"*@vectorSize

  words.split(" ").each_with_index do |x, i|
    vector[@invertedLexicon[x]] = "1"
  end

  vector
end

#Vector means you put in 1010 => "WordA Wordb"
#Words means "WordA WordB" => 1010
if type == "vector"
  puts "Started vector mode 0.0.0.0:#{portFrom} => 0.0.0.0:#{portTo}"
  socket = UDPSocket.new
  socket.bind "0.0.0.0", portFrom.to_i
  loop do
    msg, info = socket.recvfrom 1024
    words = vectorToWords msg
    socket.send words, 0, "0.0.0.0", portTo
  end
#Input
elsif type == "words"
end
