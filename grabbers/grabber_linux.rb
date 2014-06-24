require './linux/screenshot'
require './grabbers/grabber'

class GrabberLinux < Grabber
  include ScreenshotLinux
  def initialize
    @x = getX()
    @y = getY()
    @size = @x*@y*3
    @ptr = FFI::MemoryPointer.new(:char, @size)
  end

  def grab
    #t = Time.now
    #ptr = FFI::MemoryPointer.new(:char, @size)
    #pt = Time.now - t 
    len = screenshot(@ptr)
    #st = Time.now - t 
    #data =  
    #puts "screenshot #{st} data #{Time.now - t - st } (#{1/(Time.now - t)})" 
    return @ptr.read_string_length(len)
  end

  def grab_imagemagick
    %x{import -window root jpeg:-} #old
  end

end