#Assists with creating neurons


NEURON_TYPES = {
  :glu => 1,
  :gaba => 2,
  :no_learn => 9, #GLU & NO_LRN
  :glu_signal => 17 #GLU & Signal
}

class Neuron
  @@global_count = 0
  @@name_to_neuron = {}
  @@neurons = []

  def self.name_to_neuron name
    @@name_to_neuron[name]
  end

  #Number of neurons including the array
  def self.count
    return @@global_count
  end

  def initialize name, type, count
    @name = name
    @type = type
    @count = count
    @inhTime = 0
    @ib = 0
    @isDebug = false
    @@global_count = @@global_count + count
    @@name_to_neuron[@name] = self
    @@neurons << self
  end

  def set_inhibitory_time inhTime
    @inhTime = inhTime
  end

  def set_intrinsic_burst ib
    @ib = ib
  end

  def enable_debug
    @isDebug = true
  end

  def name_array
    output = []
    @count.times do |index|
      output << "#{@name}[#{index}]"
    end

    output
  end

  def to_code
    code = "\n"
    self.name_array.each do |name|
      code += "Neuron #{name}\n" 
      code += "Type #{NEURON_TYPES[@type]}\n"
      code += "InhTime #{@inhTime}\n"
      code += "IB #{@ib}\n"

      if @isDebug
        code += "Debug End\n"
      else
        code += "End End\n"
      end
    end

    code += "\n" 
    code
  end
end

#Create a basic neuron
  #Name - arbitrary name
  #Type - some type of NEURON_TYPES, e.g. :glu
  #params
    #:count - How many neurons (Could be an array)
    #:inh - How long this neuron inhibits for (Max 64)
    #:ib - What modulus does this neuron pulse out
    #:debug - Should this show up in studio?
def neuron name, type, params=nil
  count = 1
  count = params[:count] if !params.nil? and !params[:count].nil?
  new_neuron = Neuron.new name, type, count
  unless params.nil?
    ib = params[:ib]
    inhTime = params[:inh]
    isDebug = params[:debug]

    new_neuron.set_intrinsic_burst ib unless ib.nil?
    new_neuron.set_inhibitory_time inhTime unless inhTime.nil?
    new_neuron.enable_debug unless isDebug.nil? or isDebug == false
  end

  write new_neuron.to_code
end

def glu name, params=nil
  neuron name, :glu, params
end

def glu_signal name, params=nil
  neuron name, :glu_signal, params
end


def gaba name, ib, params=nil
  params = {} if params.nil?
  params[:ib] = ib

  neuron name, :gaba, params
end

def no_learn name, params=nil
  neuron name, :no_learn, params
end
