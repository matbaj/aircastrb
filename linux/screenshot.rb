require 'rubygems'
require 'ffi'

module ScreenshotLinux
  include FFI
  extend FFI::Library
  ffi_lib "./linux/libscreenshot_linux.so" 
  
  attach_function "getX", "getX", [], :int
  attach_function "getY", "getY", [], :int
  attach_function "screenshot", "screenshot", [:pointer], :int
end
