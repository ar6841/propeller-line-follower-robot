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
void followLine();
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
volatile int *followLineCog;
volatile int *detectObjectCog;
volatile int *detectObstacleCog;

int numIntersection = 0;
 
int main()
{
  // print("Starting");
  // simpleterm_close();
 
  followLineCog = cog_run(followLine, 128);
}

void followLine() {
  driveForward();
 
  while(1) {   
    int leftIR = input(LEFT_IR_PIN); 
    int rightIR = input(RIGHT_IR_PIN);
         
    if(leftIR == 1 && rightIR == 0) {
      adjustLeft();
    } else if(leftIR == 0 && rightIR == 1) {
      adjustRight();
    } else if(leftIR == 0 && rightIR == 0) {
      handleIntersectionDetected();
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
      turnLeft();
      break;
    case 6: // B4
    case 8: // A4
    case 11: // A1
    case 13: // B1
      turnRight();
      break;
    case 3: // B1
      turnRight();
      // detectObjectCog = cog_run(detectObject, 128);
      // detectObstacleCog = cog_run(detectObstacle, 128);
      break;
    case 17: // B5
       //end();
       break;
    default:
      driveForward();
      pause(450); // Clear interesection
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
 
  cog_end(followLine);

  // TODO: celebrate
}
