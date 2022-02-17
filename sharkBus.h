#ifndef SHARKBUS_H
#define SHARKBUS_H

#include "Arduino.h"
#include "SoftwareSerial.h"

class sharkBus
{
  public:
    int Deadband = 20; //Stablilze the not moving position.
    unsigned char in_data[12]; //Used to build incoming data
    int currentSpeed;
    int currentDirection;

    sharkBus(int roPin, int dePin, int diPin, int dataSwitch, int statusLed); 
    void wake();
    void active(int speedVal, int directionVal);
    void stop();

    SoftwareSerial* sharkSerial;
    

  private:
    uint8_t _roPin; //Default 8, Read Data Pin, MAX485
    uint8_t _dePin; //Default 9, LOW Read is enables from MAX485 Chip.  HIGH write is enabled
    uint8_t _diPin; //Default 10, Write Data Pin, MAX485
    uint8_t _dataSwitch; //Default 11, Sets 24v trigger to Shark Bus High.  Wakes up the Power Module
    uint8_t _statusLed; //Default 13, Status Indicator
    uint16_t _maxSpeed = 1023;
    unsigned char _data[12]; //Used to build packets
    word speed; //signed_int16
    word direction; // signed_int16

    


    










};

#endif
