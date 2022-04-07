#include "simpletools.h"  
#include "servo.h"     

#define RIGHT_WHEEL_PIN 12
#define LEFT_WHEEL_PIN  13

#define FORWARD_SPEED 80

// Headers
void drive();
void checkIR();
void detectObject();
void detectObstacle();
void handleIntersectionDetected();
void driveForward();
void stopWheels();
void end();

// Cogs
volatile int *driveCog;
volatile int *irCog;
volatile int *detectObjectCog;
volatile int *detectObstacleCog;

// Variables used to communicate between cogs
volatile bool turnRight = false;
volatile bool turnLeft = false;
volatile bool adjustRight = false;
volatile bool adjustLeft = false;
 
int main()
{
  irCog = cog_run(detectIR, 128);
  driveCog = cog_run(drive, 128);
}

void drive() {
  while(1) {
    if(turnRight) {
      // TODO: turn wheels to right
      turnRight = false;
    } else if (turnLeft) {
      // TODO: turn wheels to left
      turnLeft = false;
    } else if (adjustRight) {
      // TODO: adjust wheels to right
      adjustRight = false;
    } else if (adjustLeft) {
      // TODO: adjust wheels to left
      adjustLeft = false;
    } else {
      driveForward();
    }
  }
}

void checkIR() {
  // Use to ensure not handling the same intersection multiple times
  // TODO: confirm that this works
  bool intersectionDetected = false;
 
  while(1) {
   // TODO: bool leftIR = 
   // TODO: bool rightIR =
   
    if(leftIR == 1 && rightIR == 0) {
      adjustLeft = true;
      intersectionDetected = false;
    } else if(leftIR == 0 && rightIR == 1) {
      adjustRight = true;
      intersectionDetected = false;
    } else if(!intersectionDetected && leftIR == 0 && rightIR == 0) {
      intersectionDetected = true;
      handleIntersectionDetected();
    } else {
      intersectionDetected = false;
    }
  }
}  

void detectObject() {
  int numObjectsDetected = 0;

  while(1) {
    // TODO: ping left ultrasonic distance sensor
    if(objectDetected) {
       numObjectsDetected++;
        // TODO: indicated object detected
       
        if(numObjectsDetected == 2) {
          cog_end(detectObjectCog);
          end(); // TODO: remove if we still need to find the obstacle
        }
    }
  }
}

void detectObstacle() {
  while(1) {
    // TODO: ping right ultrasonic distance sensor
    if(obstacleDetected) {
       // TODO: indicated obstacle detected
       cog_end(detectObjectCog);
    }
  }
}

void handleIntersectionDetected() {
  numIntersection++;
  // TODO: Show intersection detected on LCD

  switch(intersectionNum) {
    case 2: // i1
      turnLeft = true;
      break;
    case 6: // B4
    case 8: // A4
    case 11: // A1
    case 13: // B1
      turnRight = true;
      break;
    case 3: // B1
      turnRight = true;
      detectObjectCog = cog_run(detectObject, 128);
      detectObstacleCog = cog_run(detectObstacle, 128);
      break;
     case 17: // B5
       end();
       break;
  }
}

void driveForward() {
  servo_speed(RIGHT_WHEEL_PIN, FORWARD_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, FORWARD_SPEED);   
}  

void stopWheels() {
  servo_speed(RIGHT_WHEEL_PIN, 0);
  servo_speed(LEFT_WHEEL_PIN, 0);   
}

void end() {
  stopWheels();
 
  cog_end(driveCog);
  cog_end(irCog);

  // TODO: celebrate
}
