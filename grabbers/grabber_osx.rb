ENV['RUBYCOCOA_THREAD_HOOK_DISABLE']='1'
require 'osx/cocoa'
require './osx/extension'
require './grabbers/grabber'

class GrabberOSX < Grabber

  def grab
    screenshot = OSX::CGWindowListCreateImage(OSX::CGRectInfinite, OSX::KCGWindowListOptionOnScreenOnly, OSX::KCGNullWindowID, OSX::KCGWindowImageDefault)
    OSX::CIImage.imageWithCGImage(screenshot).blob
  end

end