#include "simpletools.h"  
#include "servo.h"     
#include <stdbool.h>
#include <stdio.h>

#define RIGHT_WHEEL_PIN 12
#define LEFT_WHEEL_PIN  13
#define LEFT_IR_PIN     16
#define RIGHT_IR_PIN    17

#define FORWARD_SPEED   15
#define STOP_SPEED      0
#define ADJUST_SPEED    25
#define ADJUST_DELAY    100
#define TURN_SPEED      30
#define TURN_DELAY      2000

// Headers
void drive();
void checkIR();
void detectObject();
void detectObstacle();
void handleIntersectionDetected();
void driveForward();
void adjustRight();
void adjustLeft();
void turnRight();
void turnLeft();
void stopWheels();
void end();

// Cogs
volatile int *driveCog;
volatile int *irCog;
volatile int *detectObjectCog;
volatile int *detectObstacleCog;

// Variables used to communicate between cogs
volatile bool shouldTurnRight = false;
volatile bool shouldTurnLeft = false;
volatile bool shouldAdjustRight = false;
volatile bool shouldAdjustLeft = false;

int numIntersection = 0;
 
int main()
{
  //printf("starting%d\n",123);
  //simpleterm_close();
  irCog = cog_run(checkIR, 128);
  driveCog = cog_run(drive, 128);
}

void drive() {
  while(1) {
    
    //simpleterm_open();
    //printf("Left IR: %d\n", 123);
    //printf("Right IR: %d\n", 123);
    //simpleterm_close();    
    
    if(shouldTurnRight) {
      turnRight();
      shouldTurnRight = false;
    } else if (shouldTurnLeft) {
      turnLeft();
      shouldTurnLeft = false;
    } else if (shouldAdjustRight) {
       adjustRight();
       shouldAdjustRight= false;
    } else if (shouldAdjustLeft) {
      adjustLeft();
      shouldAdjustLeft = false;
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
    if(shouldAdjustRight || shouldAdjustLeft || shouldTurnRight || shouldTurnLeft) {
      return;
    } 
    
    int leftIR = input(LEFT_IR_PIN); 
    int rightIR = input(RIGHT_IR_PIN);
    
    /*pause(200);
    simpleterm_open();
    pause(200);
    printf("Left IR: %d\n", leftIR);
    printf("Right IR: %d\n", rightIR);*/
    //simpleterm_close(); 
         
    if(leftIR == 1 && rightIR == 0) {
      shouldAdjustLeft = true;
      intersectionDetected = false;
    } else if(leftIR == 0 && rightIR == 1) {
      shouldAdjustRight = true;
      intersectionDetected = false;
    } else if(leftIR == 0 && rightIR == 0) {
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
    bool objectDetected = false;
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
    bool obstacleDetected = false;
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

  switch(numIntersection) {
    case 2: // i1
      shouldTurnLeft = true;
      break;
    case 6: // B4
    case 8: // A4
    case 11: // A1
    case 13: // B1
      shouldTurnRight = true;
      break;
    case 3: // B1
      shouldTurnRight = true;
      //detectObjectCog = cog_run(detectObject, 128);
      //detectObstacleCog = cog_run(detectObstacle, 128);
      break;
     case 17: // B5
       //end();
       break;
     default:
      //driveForward();
      //pause(200);
      break;
  }
}

void driveForward() {
  servo_speed(RIGHT_WHEEL_PIN, FORWARD_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, FORWARD_SPEED);   
}  

void adjustRight() {
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, ADJUST_SPEED); 
  pause(ADJUST_DELAY);  
  servo_speed(LEFT_WHEEL_PIN, FORWARD_SPEED); 
}  

void adjustLeft() {
  servo_speed(RIGHT_WHEEL_PIN, ADJUST_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
  pause(ADJUST_DELAY);
  servo_speed(RIGHT_WHEEL_PIN, FORWARD_SPEED * -1);
}  

void turnRight(){
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, TURN_SPEED); 
  pause(TURN_DELAY);  
  driveForward();
} 

 
void turnLeft(){
  servo_speed(RIGHT_WHEEL_PIN, TURN_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
  pause(TURN_DELAY);
  driveForward();
}
 
void stopWheels() {
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
}

void end() {
  stopWheels();
 
  cog_end(driveCog);
  cog_end(irCog);

  // TODO: celebrate
}
