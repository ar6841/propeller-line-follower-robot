#include "simpletools.h"  
#include "servo.h"     
#include <stdbool.h>
#include <stdio.h>
#include "ping.h"

#define RIGHT_WHEEL_PIN         12
#define LEFT_WHEEL_PIN          13
#define LEFT_IR_PIN             16
#define RIGHT_IR_PIN            17

#define RIGHT_ULTRASONIC_TRIG   11
#define RIGHT_ULTRASONIC_ECHO   8
#define LEFT_ULTRASONIC_PIN     0
#define OBJECT_DETECTION_LED    3
#define OBSTACLE_DETECTION_LED  6

#define MAX_OBJECT_DISTANCE     20

#define FORWARD_SPEED           35
#define STOP_SPEED              0
#define ADJUST_SPEED            20
#define ADJUST_DELAY            10
#define TURN_SPEED              50
#define TURN_DELAY              1500

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
  //while(1){
    //  driveForward();
    //stopWheels();
    //followLine();
   
    //    int cmDist = ping_cm(LEFT_ULTRASONIC_PIN);
      //  print("D %d\n",cmDist);
       // pause(200);

 // }    
}

void followLine() {
  //driveForward();   
  while(1){
    int leftIR = input(LEFT_IR_PIN); 
    int rightIR = input(RIGHT_IR_PIN);
   //printf("\n%d",leftIR);
   //printf("\n%d",rightIR);
   //pause(1000);
         
    if(leftIR == 1 && rightIR == 0) {
      adjustLeft();
    } else if(leftIR == 0 && rightIR == 1) {
      adjustRight();
    } else if(leftIR == 1 && rightIR == 1) {
      handleIntersectionDetected();
    }
    else{
      driveForward();
    }     
  }         
}  

void detectObject() {
  int numObjectsDetected = 0;
  bool shouldDetectObject = true;

  while(1) {
    int cmDist = ping_cm(LEFT_ULTRASONIC_PIN);
    bool objectDetected = cmDist != 0 && cmDist < MAX_OBJECT_DISTANCE;
    
    if(objectDetected) {
      if(shouldDetectObject) {
       shouldDetectObject = false;
       numObjectsDetected++;
       
       high(OBJECT_DETECTION_LED);
       pause(500);
       low(OBJECT_DETECTION_LED);
              
       if(numObjectsDetected == 2) {
         end();
         cog_end(detectObjectCog);
       }
     }       
    } else {
      shouldDetectObject = true;
    }
    
    pause(200);      
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
    case 3: // i1
      turnLeft();
      break;
    case 4: // B1
      turnRight();
      detectObjectCog = cog_run(detectObject, 128);
      // detectObstacleCog = cog_run(detectObstacle, 128);
      break;
    case 7:
      //if loop completed go forward
      turnRight();
      break; // B4
    case 9:
      turnRight();
      break; // A4
    case 12: // A1
    case 14: // B1
      turnRight();
      break;
    case 18: // B5
       end();
       break;
    default:
      driveForward();
      pause(450); // Clear interesection
      break;
  }
}

void driveForward() {
  servo_speed(RIGHT_WHEEL_PIN, FORWARD_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, FORWARD_SPEED-7);   
}  

void adjustRight() {
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, ADJUST_SPEED); 
  pause(ADJUST_DELAY);  
  //driveForward();
}  

void adjustLeft() {
  servo_speed(RIGHT_WHEEL_PIN, ADJUST_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
  pause(ADJUST_DELAY);
  //driveForward();
}  

void turnRight(){
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, TURN_SPEED); 
  pause(TURN_DELAY+250);  
  //driveForward();
} 

 
void turnLeft(){
  servo_speed(RIGHT_WHEEL_PIN, TURN_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
  pause(TURN_DELAY);
  //driveForward();
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
