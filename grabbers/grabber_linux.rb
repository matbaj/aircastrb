require './linux/screenshot'
require './grabbers/grabber'

class GrabberLinux < Grabber
  include ScreenshotLinux

  def grab
    ptr = FFI::MemoryPointer.new(:char, getX()*getY()*3)
    len = screenshot(ptr)
    ptr.read_string_length(len)
    
  end

  def grab_imagemagick
    %x{import -window root jpeg:-} #old
  end

end