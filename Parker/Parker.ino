#include <PS3BT.h>
#include <usbhub.h>

//===========Uncomment a drive train===================
#include "DriveTrains/BasicDrive.cpp"


//===========Uncomment to choose a Peripheral==========
//#define PERIPHERAL

//==========Uncomment if not using bag motors==========
//#define OldMotors

//This just enables and disables the old motors
#ifdef OldMotors
int motorType = -1;
#else
int motorType = 1;
#endif
//===================================

#define TACKLE_INPUT    6           // Tackle sensor is wired to pin 6
bool hasIndicated = false;
bool stayTackled = false;
int handicap = 3;
bool kidsMode = false;
int newconnect = 0;
int leftX, leftY, rightX, rightY;
USB Usb;
USBHub Hub1(&Usb);
BTD Btd(&Usb);
PS3BT PS3(&Btd);


void setup() {// This is stuff for connecting the PS3 to USB.
  driveSetup(motorType);//Setup the drive train
  
#ifdef PERIPHERAL
  peripheralSetup();//Call the peripheral setup
#endif


  int newconnect = 0;         // Variable(boolean) for connection to ps3, also activates rumble

  Serial.begin(115200);       //Begin Serial Communications
  if (Usb.Init() == -1)       // this is for an error message with USB connections
  {
    Serial.print(F("\r\nOSC did not start"));
    while (1);
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));
}

void loop() {

  Usb.Task();                           // This updates the input from the PS3 controller
  if (PS3.PS3Connected)                 // run if the controller is connected
  {
    if (newconnect == 0)                // this is the vibration that you feel when you first connect
    {
      PS3.moveSetRumble(64);
      PS3.setRumbleOn(100, 255, 100, 255); //VIBRATE!!!
      newconnect++;
    }
    if (PS3.getButtonClick(PS)) {
      PS3.disconnect();
      newconnect = 0;
    }
    //========================================Get Controller Input==========================================
    leftX = map(PS3.getAnalogHat(LeftHatX), 0, 255, -90, 90);     // Recieves PS3
    leftY = map(PS3.getAnalogHat(LeftHatY), 0, 255, -90, 90);     // Recieves PS3
    rightX = map(PS3.getAnalogHat(RightHatX), 0, 255, -90, 90);   // Recieves PS3
    rightY = map(PS3.getAnalogHat(RightHatY), 0, 255, -90, 90);   // Recieves PS3
    if (abs(leftX) < 8) leftX = 0;                                // deals with the stickiness
    if (abs(leftY) < 8) leftY = 0;
    if (abs(rightX) < 8) rightX = 0;
    if (abs(rightY) < 8) rightY = 0;
    //======================Specify the handicap================================
    if (PS3.getButtonClick(START) && (kidsMode == false)) {
      handicap = 7;
      kidsMode = true;
    } else if (PS3.getButtonClick(START) && (kidsMode == true)) {
      handicap = 3;
      kidsMode = false;
    } else if (PS3.getButtonPress(R2) && (kidsMode == false)) {
      handicap = 1;
    } else if (PS3.getButtonPress(L2)) {
      handicap = 6;
    } else {
      handicap = 3;
    }
    //==========================================================================

    driveCtrl(PS3,handicap, leftX, leftY, rightX, rightY);//Drive the drive train

#ifdef PERIPHERAL
    peripheral(PS3);//Call the peripheral
#endif
  }
  if (!PS3.PS3Connected) {
    driveStop();
  }
}
