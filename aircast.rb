#!/usr/bin/env ruby
require 'rubygems'
require 'bundler/setup'



def grab_screen_osx
  screenshot = OSX::CGWindowListCreateImage(OSX::CGRectInfinite, OSX::KCGWindowListOptionOnScreenOnly, OSX::KCGNullWindowID, OSX::KCGWindowImageDefault)
  OSX::CIImage.imageWithCGImage(screenshot).blob
end

def grab_screen_linux
  %x{import -window root jpeg:-}
end

def grab_screen_win
  image = ::MiniMagick::Image.read Win32::Screenshot::Take.of(:desktop).bitmap
  image.format 'jpg'
  image.to_blob
end

if RbConfig::CONFIG['host_os']== "mswin32"
  #Windows
  require 'win32/screenshot'
  grab_screen = method(:grab_screen_win) 
else
  if RbConfig::CONFIG['host_os'] == "darwin12.0"
    #OS X
    ENV['RUBYCOCOA_THREAD_HOOK_DISABLE']='1'
    require 'osx/cocoa'
    require 'extension'
    grab_screen = method(:grab_screen_osx)
  else
    #GNU/LINUX
    grab_screen = method(:grab_screen_linux)
  end
  require 'airplay'
  Thread.new do
    airplay = Airplay::Client.new()
    ipaddr=Socket.ip_address_list.detect{|intf| intf.ipv4_private?}  
    ip = ipaddr.ip_address || '127.0.0.1'
    airplay.send_video("http://#{ip}:4567/out.jpeg")
  end
end

#sinatra part
require 'sinatra'

set :server, :thin
set :bind, '0.0.0.0'
set :port, 4567

get '/' do
  erb :index
  
end

get '/out.jpeg' do

  fps = 15
  boundary = 'image_end'

  headers \
    "Cache-Control" => "no-cache, private",
    "Pragma"        => "no-cache",
    "Content-type"  => "multipart/x-mixed-replace; boundary=#{boundary}"

  stream(:keep_open) do |out|
    while true
      content     = grab_screen.call
      out << "Content-type: image/jpeg\n\n"
      out << content
      out << "\n\n--#{boundary}\n\n"
      sleep 1/fps
    end
  end
end

__END__


@@index

<html>
<head>
  <style>
    body {
      background: black;
      width:100%;
      height:100%;
      margin:0;
    }
    img {
      max-height:100%;
      max-width:100%;
      position: absolute;
      margin: auto;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
    }
  </style>
</head>
<body>
<img src='/out.jpeg'>
</body>
</html>