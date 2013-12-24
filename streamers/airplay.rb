require 'airplay'
require './streamers/streamer'
class AirplayStreamer < Streamer

  def initialize
    @airplay = Airplay::Client.new()
  end

  def push
    Thread.new do
      sleep 1
      @airplay.send_video(get_url())
    end
  end
end