#include "Arduino.h"
#include "sharkBus.h"
#include "SoftwareSerial.h"


sharkBus::sharkBus(int roPin,int dePin,int diPin,int dataSwitch,int statusLed)
{
  //Setting all input variables
  _roPin = roPin;
  _diPin = diPin;
  _dePin= dePin;
  _dataSwitch = dataSwitch;
  _statusLed= statusLed;

  sharkSerial = new SoftwareSerial(_roPin,_diPin, false); //Open the serial port
  sharkSerial->begin(38400);


  pinMode(dataSwitch, OUTPUT); //Setup 24v Pulse trigger
  digitalWrite(dataSwitch, LOW);

  pinMode(dePin, OUTPUT); //Setup Read/Write enable pin.  Should always be LOW except when writing to the bus
  digitalWrite(dePin, LOW);

  pinMode(statusLed, OUTPUT); //Setup Status Led as an output
  digitalWrite(statusLed, LOW);
}

void sharkBus::wake()
{
  digitalWrite(_dePin, HIGH); //Enable Write
  {
    digitalWrite(_dataSwitch, HIGH); //Turn on 24v
    delay(298);                     //24v pulse 300ms +/- 20ms
    digitalWrite(_dataSwitch, LOW);
    delay(10);
  }
  //Shark Remote Power Up Packet
  _data[0] = (0x74); //BIN 1110100 --> Bits 3-0 are the packet identifier, packet type is 04, SR power up information.
                    //See documentation for more info
  _data[1] = (130);  //BIN 10000010 --> Year of manufacture - 2000.  This is 2002
  _data[2] = (133);  //BIN 10000101 --> Bits 3-0 month of manufacture.  This is 05, May
  _data[3] = (130);  //BIN 10000010 --> Serial Number bits 20-14
  _data[4] = (128);  //BIN 10000000 --> Serial Number bits 13-7
  _data[5] = (136);  //BIN 10001000 --> Serial Number bits 6-0
  _data[6] = (205);  //BIN 11001101 --> Software Version.
  _data[7] = (160);  //BIN 10100000 --> Capabilities.  See Documentation.
  _data[8] = (128);  //BIN
  byte sum = 0;
  for (int i = 0; i < 9; i++)
    sum += _data[i] & 0x7f;
  _data[9] = 0x7f - sum; //Checksum  OK = (141)
  _data[10] = (15);      // All packets end with this identifier.  Transmission Complete.

  //Send the packet
  
  for (unsigned char i = 0; i < 11; i++){
    if (i == 10)
      delayMicroseconds(718);

    sharkSerial->write(_data[i]);
  }
  digitalWrite(_dePin, LOW); //Disable Write

}

void sharkBus::active(int speedVal, int directionVal)
{
  currentDirection = directionVal;
  currentSpeed = speedVal;

  digitalWrite(_dePin, HIGH); //Enable Write

  //Deadband Code

  direction = map(directionVal, 0, 255, 0, 1023);
  speed = map(speedVal, 0, 255, 0, 1023);

  //Building Packet
  _data[0] = (0x60);                                                                    // Packet type identifier
  _data[1] = 0x80 | ((speed >> 3) & 0x7f);                                              // Joystick speed reading (7 MSbs)
  _data[2] = 0x80 | ((direction >> 3) & 0x7f);                                          // Joystick direction reading (7 MSbs)
  _data[3] = 0x80 | ((_maxSpeed >> 1) & 0x7f);                                           // Speed pot reading (7 MSbs)
  _data[4] = 0x80 | ((_maxSpeed & 0x1) << 6) | ((speed & 0x7) << 3) | (direction & 0x7); // speed/direction/maxspeed checksum
  _data[5] = 128;                                                                       // Default horn off 128 , horn on value is 130
  _data[6] = 132;                                                                       // Value read during data capture, taken to be the 'on' value.
  _data[7] = 128;                                                                       // Chair mode - drive/tilt/aux

  byte sum = 0;
  for (int i = 0; i < 8; i++)
    sum += _data[i] & 0x7f;
  _data[8] = (255 - (sum & 127));
  _data[9] = 15; // all packets end with this identifier

  for (unsigned char i = 0; i < 10; i++){
    if (i == 9)
      delayMicroseconds(718);
    sharkSerial->write(_data[i]);
  }
  digitalWrite(_dePin, LOW); //Disable Write
  
}

void sharkBus::stop()
{

}

