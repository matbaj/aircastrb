class Streamer
  def get_url
      ipaddr=Socket.ip_address_list.detect{|intf| intf.ipv4_private?}  
      ip = ipaddr.ip_address || '127.0.0.1'
      return "http://#{ip}:4567/out.jpeg"
  end
end