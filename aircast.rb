#!/usr/bin/env ruby
require 'rubygems'
require 'bundler/setup'
require './streamers/airplay'


if RbConfig::CONFIG['host_os']== "mswin32"
  require 'grabber/grabber_windows'
  $grab_screen = GrabberWindows.new
else
  if RbConfig::CONFIG['host_os'] == "darwin12.0"
    require 'grabber/grabber_osx'
    $grab_screen = GrabberOSX.new
  else
    require './grabbers/grabber_linux'
    $grab_screen = GrabberLinux.new
  end
end
air = AirplayStreamer.new
air.push
require "sinatra/base"

class Server < Sinatra::Base
  set :server, :thin
  set :bind, '0.0.0.0'
  set :port, 4567

  get '/' do
    erb :'index.html'
  end

  get '/out.jpeg' do

    fps = 5
    boundary = 'image_end'

    headers \
      "Cache-Control" => "no-cache, private",
      "Pragma"        => "no-cache",
      "Content-type"  => "multipart/x-mixed-replace; boundary=#{boundary}"

    stream do |out|
      fps_meter =0
      Thread.new do
        while true
          puts fps_meter
          fps_meter = 0
          sleep 1
        end
      end
      while true
        t = Time.now
        fps_meter += 1
        content     = $grab_screen.grab
        out << "Content-type: image/jpeg\n\n"
        out << content
        out << "\n\n--#{boundary}\n\n"
        delay = ((1.0/fps)-(Time.now - t))
        sleep delay if delay > 0
      end
    end
  end
end
Server.run!



