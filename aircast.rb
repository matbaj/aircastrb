#!/usr/bin/env ruby
require 'rubygems'
require 'bundler/setup'


def grab_screen
  if RbConfig::CONFIG['host_os']= "mswin32"
    image = ::MiniMagick::Image.read Win32::Screenshot::Take.of(:desktop).bitmap
    image.format 'jpg'
    image.to_blob
  else
    %x{"import -window root jpeg:-"}
  end
end
if RbConfig::CONFIG['host_os']= "mswin32"
  require 'win32/screenshot'
else
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
      content     = grab_screen()
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
    <img src="/out.jpeg">
  </body>
</html>