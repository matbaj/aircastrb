require 'win32/screenshot'
require './grabbers/grabber'

class GrabberWindows < Grabber

  def grab
    image = ::MiniMagick::Image.read Win32::Screenshot::Take.of(:desktop).bitmap
    image.format 'jpg'
    image.to_blob
  end

end
