@current_frame = ""
@is_currently_in_a_frame = false

#Start a new frame (string)
def begin_frame_string priority, sub_priority
  @current_frame = ""

  yield

  add_compile_string priority, sub_priority, @current_frame
end

#Start a frame (Code)
def begin_frame priority, sub_priority, &block
  proc = Proc.new do
    begin_frame_string priority, sub_priority do 
      block.call
    end
  end

  add_compile_block priority, sub_priority, proc
end

def write string
  @current_frame += string + "\n"
end
