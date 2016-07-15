/*
  Title:
  Movement

  Description:
  This tab is responsible for the interface between code and movement. It will 
  include functions that allow for PID tape following and turning of the robot.
. 
  Arduino Input:
    * Analog Pin 3: Right QRD for forward tape following.
    * Analog Pin 5: Left QRD for forward tape following.
    * Knob 6: Derivative gain control.
    * Knob 6: Proportional gain control.
  Arduino Output:
    * Motor 0: Right drive train motor.
    * Motor 1: Left drive train motor.
    
  Created: June 30, 2016
    By: Jacob Budzis
  Modified: June 30, 2016
    By: Jacob Budzis
*/
#define NORTH 'N'
#define SOUTH 'S'
#define EAST 'E'
#define WEST 'W'
#define RIGHT 'R'
#define LEFT 'L'
#define FORWARD 'F'
#define UNDEFINED 'U'
#define LEFT_INTERSECTION 0
#define RIGHT_INTERSECTION 1
#define LEFT_TAPE 5
#define RIGHT_TAPE 3
#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2
#define PROPORTIONAL 7
#define DERIVATIVE 6
#define LEFT_FWD_COLLISION 7
#define RIGHT_FWD_COLLISION 8

int lthresh = 30; //Threshold before left QRD detects paper 
int rthresh = 30; //Threshold before right QRD detects paper
double error = 0; //Current Error
double lasterr = 0; //Previous Error (i.e One time step ago)
double recerr = 0; //Error in Last Tape State (i.e. was on -1, now is on 0)
double q = 0; //#Clock pulses in last state, relates to recerr
double m = 0; //#Clock oulses in current state, relating to error
double d; //PID Derivative. m = y/x = (error-lasterr)/(q+m)
double p; //PID Proportional. 
double con; //Control Applied = kd*d + kp*p;
double vel = 60; // Current to motor
int c = 0; //Counter
int t = 0; //Counter

/*
  Function: followTape

  Description:
  This function uses PID control to allow the robot to follow the tape. This will 
  only allow for straight-line following.

  Code Inputs:
    * None
  Code Outputs:
    * None
  TINAH Inputs:
    * knob 6: Derivative gain
    * knob 7: Proportional gain
    * analog 3: Right tape QRD
    * analog 5: Left tape QRD
*/
void followTape(){ 

  int kd = knob(DERIVATIVE)/4; //Derivative Gain Multiplier 
  int kp = knob(PROPORTIONAL)/4; //Proportional Gain Multiplier
  int left = analogRead(LEFT_TAPE); //Left QRD Signal
  int right = analogRead(RIGHT_TAPE); //Right QRD Signal

   if ((left>lthresh)&&(right>rthresh)) error = 0;
   if ((left>lthresh)&&(right<rthresh)) error = -1;
   if ((left<lthresh)&&(right>rthresh)) error = +1;
   if ((left<lthresh)&&(right<rthresh))
   {
     if (lasterr>0) error = 5;
     if (lasterr<=0) error = -5;
   }
   if (!(error==lasterr))
   {
     recerr=lasterr;
     q=m;
     m=1;
   }
  
   p=kp*error;
   d=(int)((float)kd*(float)(error-recerr)/(float)(q+m));
   con = p+d;

   if (c==10)
     {
         LCD.clear();
         LCD.setCursor(0,0);
         LCD.print("lm:");
         LCD.print(analogRead(5));
         LCD.print("rm:");
         LCD.print(analogRead(3)); 
         LCD.print("kd:");
         LCD.print(kd); 
         LCD.setCursor(0,1);
         LCD.print("li:");
         LCD.print(analogRead(0));
         LCD.print("ri:");
         LCD.print(analogRead(1));
          LCD.print("kp:");
         LCD.print(kp); 
         c=0;
     }
   c=c+1;
   m=m+1;
   motor.speed(LEFT_MOTOR,vel-con); //left
   motor.speed(RIGHT_MOTOR,vel+con); //right
   lasterr=error;
}

// TO BE COMPLETED - WRITE DESCRIPTION
void r_followTape(){
  int kd = knob(DERIVATIVE)/4; //Derivative Gain Multiplier 
  int kp = knob(PROPORTIONAL)/4; //Proportional Gain Multiplier
  int left = analogRead(LEFT_TAPE); //Left QRD Signal
  int right = analogRead(RIGHT_TAPE); //Right QRD Signal

   if ((left>lthresh)&&(right>rthresh)) error = 0;
   if ((left>lthresh)&&(right<rthresh)) error = -1;
   if ((left<lthresh)&&(right>rthresh)) error = +1;
   if ((left<lthresh)&&(right<rthresh))
   {
     if (lasterr>0) error = 5;
     if (lasterr<=0) error = -5;
   }
   if (!(error==lasterr))
   {
     recerr=lasterr;
     q=m;
     m=1;
   }
  
   p=kp*error;
   d=(int)((float)kd*(float)(error-recerr)/(float)(q+m));
   con = p+d;

   if (c==10)
     {
         LCD.clear();
         LCD.setCursor(0,0);
         LCD.print("lm:");
         LCD.print(analogRead(5));
         LCD.print("rm:");
         LCD.print(analogRead(3)); 
         LCD.print("kd:");
         LCD.print(kd); 
         LCD.setCursor(0,1);
         LCD.print("li:");
         LCD.print(analogRead(0));
         LCD.print("ri:");
         LCD.print(analogRead(1));
          LCD.print("kp:");
         LCD.print(kp); 
         c=0;
     }
   c=c+1;
   m=m+1;
   motor.speed(RIGHT_MOTOR,-(vel-con)); //left
   motor.speed(LEFT_MOTOR,-(vel+con)); //right
   lasterr=error;
}

/*
  Function: detectIntersection

  Description:
  This function returns if an intersection was detected. The direction
  of the intersection to be detected is passed to this function.

  Code Inputs:
    * dir: Character corresponding to which side QRD must detect
           the intersection. If F (forward) is passed to this function,
           it will return true if EITHER of the QRDs detect something
  Code Outputs:
    * Boolean value if interesection detected.
  TINAH Inputs:
    * digital 0: Left QRD intersection
    * digital 1: Right QRD intersection
*/
bool detectIntersection(char dir){

  bool output = false;
  int left = analogRead(LEFT_INTERSECTION);
  int right = analogRead(RIGHT_INTERSECTION);

  if (dir == LEFT){
    if (left > lthresh) output = true;
  }
  else if (dir == RIGHT){
    if (right > rthresh) output = true;
  }
  else if (dir == FORWARD){
    if (left > lthresh || right > rthresh) output = true;
  }

  return output;
}

/*
  Function: turn

  Description:
  Rotates the robot corresponding to the turn direction given to the 
  function. This occurs by moving the wheels in opposite directions.

  Code Inputs:
    * Direction: (char) LEFT, RIGHT, FOWARD
  Code Outputs:
    * None
  TINAH Inputs:
    * Motor 0: Right Motor
    * Motor 1: Left Motor
*/
void turn(char dir){

   int L = 0; //Left middle QRD Signal
   int R = 0; //Right middle QRD Signal

   // Begin by going past the intersection. 
   // This will give us space to make a wider turn.

   if (dir == LEFT){
      delay(150); //Overshoot
      motor.speed(LEFT_MOTOR,-50); //left
      motor.speed(RIGHT_MOTOR,50); //right
      delay(500);
      while(R < rthresh){;
        R = analogRead(RIGHT_TAPE);
      }
      //motor.stop_all();
      error = 0; //Reset PID
      lasterr = 0;
   } 
   else if (dir == RIGHT){
      delay(150); //Overshoot
      motor.speed(LEFT_MOTOR,50); //left
      motor.speed(RIGHT_MOTOR,-50); //right
      delay(500); //Pause for 0.5s
      while(L < lthresh){
          L = analogRead(LEFT_TAPE);
      }
      //motor.stop_all();   
      error = 0; //Reset PID
      lasterr = 0;
   } 
   else{
      motor.speed(LEFT_MOTOR,vel-con); //left
      motor.speed(RIGHT_MOTOR,vel+con); //right
      delay(300); //Just pass the intersection
   }
}

//TO BE COMPLETED - WRITE DESCRIPTION
void wide_turn(char dir){

   int L = 0; //Left middle QRD Signal
   int R = 0; //Right middle QRD Signal

   // Begin by going past the intersection. 
   // This will give us space to make a wider turn.

   if (dir == LEFT){
      delay(150); //Overshoot
      motor.speed(LEFT_MOTOR,0); //left
      motor.speed(RIGHT_MOTOR,50); //right
      delay(500);
      while(R < rthresh){;
        R = analogRead(RIGHT_TAPE);
      }
      motor.stop_all();
      error = 0; //Reset PID
      lasterr = 0;
   } 
   else if (dir == RIGHT){
      delay(150); //Overshoot
      motor.speed(LEFT_MOTOR,50); //left
      motor.speed(RIGHT_MOTOR,0); //right
      delay(500); //Pause for 0.5s
      while(L < lthresh){
          L = analogRead(LEFT_TAPE);
      }
      motor.stop_all();   
      error = 0; //Reset PID
      lasterr = 0;
   } 
   else{
      //motor.speed(LEFT_MOTOR,20-con); //left
      //motor.speed(RIGHT_MOTOR,20+con); //right
      //delay(180); //Just pass the intersection
   }
}

bool detectCollision(){
  bool output = false;
  int left = digitalRead(LEFT_FWD_COLLISION); 
  int right = digitalRead(RIGHT_FWD_COLLISION); 
  if(left == LOW || right == LOW) {output = true;}
  return output;
}
