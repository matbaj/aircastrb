module OSX
  class CIImage      
    def blob( format = OSX::NSJPEGFileType, properties = nil)
      bitmapRep = OSX::NSBitmapImageRep.alloc.initWithCIImage(self)
      data = bitmapRep.representationUsingType_properties(format, properties)
      blob = data.bytes.bytestr(data.length)
      return blob
    end
  end
end