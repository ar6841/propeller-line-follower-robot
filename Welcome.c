#include "simpletools.h"  
#include "servo.h"     

#define RIGHT_WHEEL_PIN 12
#define LEFT_WHEEL_PIN  13

#define FORWARD_SPEED 80

void followLine();
void driveForward();
void stop();

volatile bool intersectionDetected;
 
int main()
{
  int *IRCog = cog_run(checkIR, 10);  
  int *followLineCog = cog_run(followLine, 128);   
}

void followLine() {
  while(1) {
    if(adjustRight) {
      // move wheels
      adjustRight = false;
    }      
  }    
}  

void checkIR() {
  while(1) {
    if(leftIR == 1 && rightIR == 0) {
      adjustLeft = true;
    } else if(leftIR == 0 && rightIR == 1) {
      adjustRight = true;
    }  else if(leftIR == 0 && rightIR == 0) {
      intersectionDetected = true;
    }         
  }
}  

void handleIntersections() {
  while(1) {
    if(intersectionDetected){
      numIntersections++;
      //show on lcd     
      intersectionDetected = false;
    }      
  }    
}      

void driveForward() {
  while(1) {
    servo_speed(RIGHT_WHEEL_PIN, FORWARD_SPEED * -1);
    servo_speed(LEFT_WHEEL_PIN, FORWARD_SPEED);
    pause(100);
    stopWheels();
  }     
}  

void stopWheels() {
  servo_speed(RIGHT_WHEEL_PIN, 0);
  servo_speed(LEFT_WHEEL_PIN, 0);   
}  