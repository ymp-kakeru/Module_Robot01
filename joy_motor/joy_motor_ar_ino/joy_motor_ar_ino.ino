#include <ros.h> // Use ros_lib.
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int16.h>
#include <stdio.h>

#define num_of_motor 6
#define MIN_PWM -255
#define MAX_PWM 255

int dir_pin_array[num_of_motor] = {27, 26, 23, 22, 25, 24};
int dir_pin_left_array[3] = {22 , 24 , 26};
int dir_pin_right_array[3] = {23 , 25 , 27};

int pwm_pin_array[num_of_motor] = { 4,  5,  8,  9,  6,  7};

bool rotate_positive[num_of_motor] = {HIGH, LOW, HIGH, LOW, HIGH, LOW};
bool rotate_negative[num_of_motor] = {LOW, HIGH, LOW, HIGH, LOW, HIGH};
bool spin_positive[num_of_motor] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
bool spin_negative[num_of_motor] = {LOW, LOW, LOW, LOW, LOW, LOW};

void wheelCb(const geometry_msgs::Twist& cmd_vel_s) ;

/* Declare global variables. */
ros::NodeHandle nh; // The nodeHandle.
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel_s",&wheelCb);
std_msgs::Int16 angl;
ros::Publisher chatter("angul",  &angl);

void setup() {
  
  /* Set pins Mode. */
  for (int i = 0; i < 6; ++i) {
      pinMode(dir_pin_array[i], OUTPUT);
      pinMode(pwm_pin_array[i], OUTPUT);
  }
  /* Node handle setting. */
  //nh.getHardware()->setBaud(57600);
  nh.initNode(); // First setup the node handle.
  nh.subscribe(sub); // Start subscribe the "steer_ctrl" topic.
 nh.advertise(chatter); 
  //nh.advertise(chatter);
  
}

void loop() {
  delay(10);
  nh.spinOnce(); // Check topic and if change it, run the call back function.
 delay(10);
}


void wheelCb(const geometry_msgs::Twist& cmd_vel_s ) {
  int pwm = cmd_vel_s.linear.x *255/50;
  int ang = cmd_vel_s.angular.z;
  
  if(abs(pwm == 0){
    angl.data = 111;
    chatter.publish(&angl);
  
    for(int i = 0; i < num_of_motor; ++i)
  {
    if( ang <= 0)
    {
      digitalWrite(dir_pin_array[i], spin_negative[i]);
      angl.data = 777;
      chatter.publish(&angl);
      analogWrite(pwm_pin_array[i], fabs(ang));
    } else 
    {
      digitalWrite(dir_pin_array[i], spin_positive[i]);
      angl.data = -777;
      chatter.publish(&angl);
      analogWrite(pwm_pin_array[i], fabs(ang));
      
      //analogWrite(pwm_pin_array[i], 80);
      //ROS_INFO("NOW PWM is %d",msd.data[i]);
    }
  }  
 }else{    

  for(int i = 0; i < num_of_motor; ++i)
  {
    if( pwm >= 0)
    {
      angl.data = pwm;
  chatter.publish(&angl);
      digitalWrite(dir_pin_array[i], rotate_negative[i]);
      if( pwm > MAX_PWM)
      {
        pwm = MAX_PWM;
      }
      analogWrite(pwm_pin_array[i], fabs(pwm));
    }
    else
    {
      angl.data = pwm;
  chatter.publish(&angl);
      digitalWrite(dir_pin_array[i], rotate_positive[i]);
      if( pwm < MIN_PWM)
      {
        pwm = MIN_PWM;
      }
      analogWrite(pwm_pin_array[i], fabs(pwm));
      
      //analogWrite(pwm_pin_array[i], 80);
      //ROS_INFO("NOW PWM is %d",msd.data[i]);
    }
  }
  }
  
}
