#include <Servo.h>
Servo lfmotor, rfmotor,lrmotor,rrmotor;        // Define motor objects
static int motorDirection = 1;

#define TANK 0
#define ARCADE 1
#define INITIAL_HANDICAP 4

#define TANK 0
#define ARCADE 1

#define FORWARD  -1
#define BACKWARD  1


//If one of the motors is spinning when stopped, adjust it's offset here
#define LEFT_ADJUST 0
#define RIGHT_ADJUST 0

int driveDirection = BACKWARD;
int driveMode = TANK; // 0 for yx control, 1 for yy control
int arcadeDrive = 0; //Initial speed before turning calculations
int arcadeTurn = 0; //Turn is adjustment to drive for each motor separately to create turns
int leftTankDrive  = 0;
int rightTankDrive = 0;
int motorCorrect = 0;
int turnhandicap = 1; //This value gets changed by the governor function to reduce trun speed when speed governor is removed

#define LF_MOTOR 10
#define RF_MOTOR 8
#define LR_MOTOR 11
#define RR_MOTOR 9

void driveSetup(int motorType) {
  /* These lines are attaching the motor objects to their output pins on the arduino
    1000, 2000 refers to the minimum and maximum pulse widths to send to the motors (AKA full forward/reverse)
    1500 represents stop
  */
  lfmotor.attach(LF_MOTOR);
  rfmotor.attach(RF_MOTOR);
  lrmotor.attach(LR_MOTOR);
  rrmotor.attach(RR_MOTOR);  
  lfmotor.writeMicroseconds(1500); //stop value
  rfmotor.writeMicroseconds(1500);
  lrmotor.writeMicroseconds(1500);
  rrmotor.writeMicroseconds(1500);
  motorDirection = motorType;
  #define MAX_DRIVE             90    // limited because of issues with calibrating victors to full 0-180 range
}

void driveCtrl(PS3BT PS3,int handicap, int leftXinput, int leftYinput, int rightXinput, int rightYinput)
{
  //Instead of following some sort of equation to slow down acceleration
  //We just increment the speed by one towards the desired speed.
  //The acceleration is then slowed because of the loop cycle time
  if(PS3.getButtonPress(RIGHT))
  {
    if(PS3.getButtonClick(R3))
    {
      if(TANK == driveMode)
      {
        driveMode = ARCADE;
        PS3.setRumbleOn(10,255,10,255);
        arcadeDrive = arcadeTurn = leftTankDrive = rightTankDrive = 0;
      }
      else
      {
        driveMode = TANK;
        PS3.setRumbleOn(10,255,10,255);
        arcadeDrive = arcadeTurn = leftTankDrive = rightTankDrive = 0;
      }
    }
  }

  if(PS3.getButtonClick(SELECT))
  {
    if(FORWARD == driveDirection)
    {
      driveDirection = BACKWARD;
      PS3.setRumbleOn(10,255,0,255);
    }
    else
    {
      driveDirection = FORWARD;
      PS3.setRumbleOn(0,255,10,255);
    }
  }

  if(ARCADE == driveMode)
  {
    if (arcadeDrive < leftYinput)arcadeDrive++; //Accelerates
    else if (arcadeDrive > leftYinput) arcadeDrive--; //Decelerates

    if (arcadeTurn < rightXinput) arcadeTurn++;
    else if (arcadeTurn > rightXinput) arcadeTurn--;

    int ThrottleL = ((arcadeDrive - (arcadeTurn / turnhandicap)) / handicap) + LEFT_ADJUST; //This is the final variable that decides motor speed.
    int ThrottleR = ((arcadeDrive + (arcadeTurn / turnhandicap)) / handicap) + RIGHT_ADJUST;
    if (BACKWARD == driveDirection) 
    { // This will flip the direction of the left stick to allow easier driving in reverse.
      ThrottleL = ((arcadeDrive + (arcadeTurn / turnhandicap)) / handicap) + LEFT_ADJUST;
      ThrottleR = ((arcadeDrive - (arcadeTurn / turnhandicap)) / handicap) + RIGHT_ADJUST;
      ThrottleL = -ThrottleL;
      ThrottleR = -ThrottleR;
    }
    if (ThrottleL > 90) ThrottleL = 90;
    if (ThrottleR > 90) ThrottleR = 90;
  
    int left = (-ThrottleL + 90 + motorCorrect);
    int right = (ThrottleR + 90 + motorCorrect);
    lfmotor.write(left); //Sending values to the speed controllers
    rfmotor.write(right);
    lrmotor.write(left); //Send values to the rear speed controllers.
    rrmotor.write(right);
  }
  else // TANK DRIVE
  {
    if(leftTankDrive < leftYinput)      leftTankDrive++;
    else if(leftTankDrive > leftYinput) leftTankDrive--;

    if(rightTankDrive < rightYinput)      rightTankDrive++;
    else if(rightTankDrive > rightYinput) rightTankDrive--;

    if(FORWARD == driveDirection)
    {
      lfmotor.write(-leftTankDrive +90); //Sending values to the speed controllers
      rfmotor.write(rightTankDrive+90);
      lrmotor.write(-leftTankDrive +90); //Send values to the rear speed controllers.
      rrmotor.write(rightTankDrive+90);
    }
    else
    {
      rfmotor.write(-leftTankDrive +90); //Sending values to the speed controllers
      lfmotor.write(rightTankDrive+90);
      rrmotor.write(-leftTankDrive +90); //Send values to the rear speed controllers.
      lrmotor.write(rightTankDrive+90);
    }
  }
  
}


void driveStop()
{
  lfmotor.writeMicroseconds(1500); //stop value
  rfmotor.writeMicroseconds(1500);
  lrmotor.writeMicroseconds(1500);
  rrmotor.writeMicroseconds(1500);
}
