#include <sharkBus.h>
#include <SoftwareSerial.h>

//Variables for Recieving Data over Serial
const int numChars = 3;
byte receivedChars[numChars];
int tempChars[numChars];
int speedVal = 128;
int directionVal = 128;
int typeVal = 0;
boolean newData = false;

//Sharbus setup
sharkBus shark(8, 9, 10, 11, 13);


void setup() {
  Serial.begin(115200);
  delay(300);
  shark.wake();

}


bool cont = true;
int endmark = 15;
int data[12];
int indx = 0;
char rc;

unsigned long timeout = 15;
unsigned long current_time;
unsigned long wait = 10;

void loop() {

  //=============================Outgoing========================\\
  //Wait for Transmission Finished Packet
  cont = true;
  //Start timer
  unsigned long start_time = millis();
  while (shark.sharkSerial->available() && cont) { //Reading the Sharkbus
    rc = shark.sharkSerial->read();
    current_time = millis();
    if (current_time - start_time >= timeout) {
      cont = false;
      indx = 0;
      shark.wake();
      goto Incoming;
    }

    if (rc != endmark) {
      data[indx] = rc;
      indx++;
    }
    else {
      indx = 0;
      cont = false;
    }

  }


Incoming:
  //===========================Incoming=======================\\
  //Check for data in incoming serial buffer, process it and reply
  //serialFlush();
  start_time = millis();
  while (Serial.available() <= 0){
    current_time = millis();
    if (current_time - start_time >= wait) {
      goto Receive;
    }
  }
Receive:
  recvWithStartEndMarkers(); //Look for new movment data


  cont = true;
  if (newData == true) {
    //showParsedData();
    newData = false;

    //If new movement packet recived on input port, reply with 1 for sucess, 0 for error

    Serial.print(byte(1));
    //parseSPMdata(data); //Parse Sharkbus Data and send over serial




  }
  //==========================OUTGOING========================\\

  delay(5); //Delay after received transmission finished packet
  shark.active(speedVal, directionVal);

}

int type;
int error;
int battery;
int spedo;
int opmode;
int sysState;
bool driveInhibit;


char parseSPMdata(int data[12]) {

  if (data[0] == 97) {
    type = "SPM General Information";
    battery = data[1] & 0x1f;
    spedo = data[7] & 0x1f;
    sysState = data[2] & 0x0f;
    driveInhibit = data[2];

    //Serial.println(battery);


  }
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  int startMarker = 75;
  int endMarker = 15;
  int buffer_size = 8;
  byte rc;
  int len;
  byte sum;

  unsigned long start_time = millis();
  while (Serial.available() > 0 && newData == false) {
    current_time = millis();
    if (current_time - start_time >= timeout) {
      shark.wake();
      goto Incoming_Error;
    }
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx > numChars) {
          goto Incoming_Error;
        }
      }
      else {
        sum = receivedChars[0] ^ receivedChars[1];
        if (sum == receivedChars[2]) {
          recvInProgress = false;
          ndx = 0;
          newData = true;
          //Serial.println("Verified!");
          speedVal = receivedChars[0];
          directionVal = receivedChars[1];
        }
        else {
Incoming_Error:
          recvInProgress = false;
          ndx = 0;
          newData = false;
          Serial.print(byte(0));
        }


      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
      //Serial.println("Start");
    }

  }
}





void showParsedData() {
  Serial.print("Speed ");
  Serial.println(speedVal);
  Serial.print("Direction ");
  Serial.println(directionVal);
}

void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}
