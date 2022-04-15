#include "simpletools.h"  
#include "servo.h"     
#include <stdbool.h>
#include <stdio.h>
#include "ping.h"

#define RIGHT_WHEEL_PIN             12
#define LEFT_WHEEL_PIN              13
#define LEFT_IR_PIN                 16
#define RIGHT_IR_PIN                17

#define RIGHT_ULTRASONIC_PIN        11
#define LEFT_ULTRASONIC_PIN         0

#define OBJECT_DETECTION_LED        3
#define OBSTACLE_DETECTION_LED      5
#define INTERSECTION_DETECTION_LED  4

#define MAX_OBJECT_DISTANCE         20
#define MAX_OBSTACLE_DISTANCE       35

#define FORWARD_SPEED               40
#define STOP_SPEED                  0
#define ADJUST_SPEED                20
#define ADJUST_DELAY                10
#define TURN_SPEED                  50
#define TURN_DELAY                  1500
#define EXTRA_RIGHT_TURN_DELAY      300

// Headers
void followLine();
void detectObject();
void detectObstacle();
void handleIntersectionDetected();
void intersectionBlink();
void driveForward();
void adjustRight();
void adjustLeft();
void turnRight();
void turnLeft();
void stopWheels();
void stopDriving();

// Cogs
volatile int *followLineCog;
volatile int *detectObjectCog;
volatile int *detectObstacleCog;
volatile int *intersectionBlinkCog;

static volatile int numIntersection = 0;
static volatile int numObjectsDetected = 0;
static volatile bool obstacleDetected = false;
static volatile bool lookForObstacleOnRight = true;
 
int main()
{ 
  //stopWheels();
  // print("Starting");
  // simpleterm_close();
  //detectObject();
 
  followLineCog = cog_run(followLine, 128);
  
  while(1){
    if(obstacleDetected && numObjectsDetected == 2) {
      stopWheels();
      cog_end(followLineCog);
    }      
  }    
}

void followLine() {
  bool shouldDetectIntersection = true;

  while(1){
    int leftIR = input(LEFT_IR_PIN); 
    int rightIR = input(RIGHT_IR_PIN);
         
    if(leftIR == 1 && rightIR == 0) {
      shouldDetectIntersection = true;
      adjustLeft();
    } else if(leftIR == 0 && rightIR == 1) {
      shouldDetectIntersection = true;
      adjustRight();
    } else if(leftIR == 1 && rightIR == 1) {
      shouldDetectIntersection = false;
      handleIntersectionDetected();
    }
    else{
      shouldDetectIntersection = true;
      driveForward();
    }     
  }         
}  

void detectObject() {
  bool shouldDetectObject = true;

  while(1) {
    int cmDist = ping_cm(LEFT_ULTRASONIC_PIN);
    bool objectDetected = cmDist != 0 && cmDist < MAX_OBJECT_DISTANCE;
    
    if(objectDetected) {
      if(shouldDetectObject) {
       shouldDetectObject = false;
       numObjectsDetected++;
       
       high(OBJECT_DETECTION_LED);
       pause(750);
       low(OBJECT_DETECTION_LED);
              
       if(numObjectsDetected == 2) {        
         cog_end(detectObjectCog);
       }
     } 
      pause(200);      
    } else {
      shouldDetectObject = true;
    }

  }
}

void detectObstacle() {    
  while(1) {
    int ultrasonicPin = lookForObstacleOnRight ? RIGHT_ULTRASONIC_PIN : LEFT_ULTRASONIC_PIN;
    int cmDist = ping_cm(ultrasonicPin);
    obstacleDetected = cmDist != 0 && cmDist < MAX_OBSTACLE_DISTANCE;
   
    if(obstacleDetected) {
       high(OBSTACLE_DETECTION_LED);
       pause(750);
       low(OBSTACLE_DETECTION_LED);
             
       cog_end(detectObstacleCog);
    }
  }
}

void handleIntersectionDetected() {
  numIntersection++;
  
  if(numIntersection > 1) {
    intersectionBlinkCog = cog_run(intersectionBlink, 128);
  }  

  switch(numIntersection) {
    case 1: // merge
      driveForward();
      pause(640);
      break;
    case 3: // i1
      turnLeft();
      break;
    case 4: // B1
      turnRight();
      detectObjectCog = cog_run(detectObject, 128);
      detectObstacleCog = cog_run(detectObstacle, 128);
      break;
    case 7: // B4
      turnRight();
      cog_end(detectObjectCog);
      lookForObstacleOnRight = false;
      driveForward();
      pause(450);
      break;
    case 9: // A4
      turnRight();
      detectObjectCog = cog_run(detectObject, 128);   
      break; 
    case 12: // A1 - finish first loop
      turnRight();
      break;
    case 14: // B1
      cog_end(detectObjectCog);
      turnRight();
      break;
    case 18: // B5 - last position
      detectObjectCog = cog_run(detectObject, 128);
      break;
    default:
      driveForward();
      pause(450);
      break;
  }
}

void intersectionBlink() {
  high(INTERSECTION_DETECTION_LED);
  pause(500);
  low(INTERSECTION_DETECTION_LED);
  
  cog_end(intersectionBlinkCog);           
}  

void driveForward() {
  servo_speed(RIGHT_WHEEL_PIN, FORWARD_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, FORWARD_SPEED - 7);   
}  

void adjustRight() {
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, ADJUST_SPEED); 
  pause(ADJUST_DELAY);  
}  

void adjustLeft() {
  servo_speed(RIGHT_WHEEL_PIN, ADJUST_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
  pause(ADJUST_DELAY);
}  

void turnRight(){
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, TURN_SPEED); 
  pause(TURN_DELAY + EXTRA_RIGHT_TURN_DELAY);  
} 

 
void turnLeft(){
  servo_speed(RIGHT_WHEEL_PIN, TURN_SPEED * -1);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
  pause(TURN_DELAY);
}
 
void stopWheels() {
  servo_speed(RIGHT_WHEEL_PIN, STOP_SPEED);
  servo_speed(LEFT_WHEEL_PIN, STOP_SPEED);   
}