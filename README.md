# ArduinoMAVLinkEndpoint

The design intent is to provide a flexible interface to MAVLink for Arduino sketches. There are objects for listeners, transmitters (both periodic and otherwise), and endpoints, along with a parameter manager.

## MAVLinkEndpoint

This class defines a MAVLink endpoint. In order to set it up, define any transmitters and receivers (see below) you wish to connect to it along with a writer function. The writer function is a function that takes a buffer of bytes and the length of that buffer and writes it somewhere. If you wanted to use, for example, the ``Serial`` device as an output, your writer function would look like this:
```
void my_writer (uint8_t *data, uint8_t len) {
  Serial.write(data, len);
}
```
The reason that this library requires the user to define this is in order to allow this library to work with any communications medium (serial, ethernet, wifi, CANbus, etc) that the user may desire without library modifications. 


