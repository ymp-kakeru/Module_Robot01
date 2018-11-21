#include <ros.h> // Use ros_lib.
#include <geometry_msgs/Twist.h>
#include <stdio.h>

#define num_of_motor 6
#define MIN_PWM -255
#define MAX_PWM 255
#define bias 0.8
int dir_pin_array[6] = {22,23, 26,27, 24,25};

int pwm_pin_array[6] = {4, 5,8, 9, 6,7};

bool rotate_positive[6] = {HIGH, LOW, HIGH, LOW, HIGH, LOW};
bool rotate_negative[6] = {LOW, HIGH, LOW, HIGH, LOW, HIGH};
bool spin_positive[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
bool spin_negative[6] = {LOW, LOW, LOW, LOW, LOW, LOW};

void wheelCb(const geometry_msgs::Twist& cmd_vel_s) ;

/* Declare global variables. */
ros::NodeHandle nh; // The nodeHandle.
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel_s",&wheelCb);

void setup() {
  
  /* Set pins Mode. */
  for (int i = 0; i < num_of_motor; ++i) {
      pinMode(dir_pin_array[i], OUTPUT);
      pinMode(pwm_pin_array[i], OUTPUT);
  }
  /* Node handle setting. */
  //nh.getHardware()->setBaud(9600);
  
  nh.initNode(); // First setup the node handle.
  nh.subscribe(sub); // Start subscribe the "steer_ctrl" topic.
  
}

void loop() {
  nh.spinOnce(); // Check topic and if change it, run the call back function.
 delay(10);
}

void wheelCb(const geometry_msgs::Twist& cmd_vel_s ) {
  int pwm = cmd_vel_s.linear.x *255 ;
  int pwm_wheelie = cmd_vel_s.linear.y * 255;
  int ang = cmd_vel_s.angular.z;
  
  if(pwm == 0){  
    for(int i = 0; i < num_of_motor; ++i)
    {
    if( ang >= 0)
    {
      digitalWrite(dir_pin_array[i], rotate_negative[i]);
      if(num_of_motor == 6){ 
        if(i==1 || i==4) analogWrite(pwm_pin_array[i], fabs(ang*bias));
        else analogWrite(pwm_pin_array[i], fabs(ang));
      }else{
        analogWrite(pwm_pin_array[i], fabs(ang));
      }
    } else if(ang < 0){
      digitalWrite(dir_pin_array[i], rotate_positive[i]);
      if(num_of_motor == 6){
        if(i==0 || i==5) analogWrite(pwm_pin_array[i], fabs(ang*bias));
        else analogWrite(pwm_pin_array[i], fabs(ang));
      }else{
        analogWrite(pwm_pin_array[i], fabs(ang));
      }
    }
    }  
  }else{    
    for(int i = 0; i < num_of_motor; ++i)
    {
      if( pwm >= 0){
        digitalWrite(dir_pin_array[i], spin_negative[i]);
        if((pwm_wheelie >= 0)&&((i==1)||(i==4))){
          analogWrite(pwm_pin_array[i], fabs(pwm_wheelie));
        }else{
        analogWrite(pwm_pin_array[i], fabs(pwm));
        }
      }else{
        digitalWrite(dir_pin_array[i], spin_positive[i]);
        analogWrite(pwm_pin_array[i], fabs(pwm));
      }
    }
  }
}
