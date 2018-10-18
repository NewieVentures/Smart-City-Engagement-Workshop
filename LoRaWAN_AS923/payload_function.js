function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  
  //Data has swapped to little endian... maybe because it's not word aligned any more?
  decoded.data1       = (bytes[0]<<0) + (bytes[1]<<8);
  decoded.data2       = (bytes[2]<<0) + (bytes[3]<<8);
  decoded.data3       = (bytes[4]<<0) + (bytes[5]<<8);
  decoded.data4       = (bytes[6]<<0) + (bytes[7]<<8);
  decoded.data5       = (bytes[8]<<0) + (bytes[9]<<8);
  decoded.data6       = (bytes[10]<<0) + (bytes[11]<<8);
  
  decoded.satellites  = (bytes[12]<<0) + (bytes[13]<<8);
  decoded.latitude    = (bytes[14]<<0) + (bytes[15]<<8) + (bytes[16]<<16) + (bytes[17]<<24);
  decoded.longitude   = (bytes[18]<<0) + (bytes[19]<<8) + (bytes[20]<<16) + (bytes[21]<<24);
  decoded.altitude    = (bytes[22]<<0) + (bytes[23]<<8) + (bytes[24]<<16) + (bytes[25]<<24);
  
  return decoded;
}