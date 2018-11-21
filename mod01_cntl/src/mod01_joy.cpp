#include "ros/ros.h"
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>

class Joy_Cntl
{
  public:
    Joy_Cntl();
    // ~Joy_Cntl();

  private:
    void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);
    // void deCallback() ;
    ros::NodeHandle nh_;

    int  linear_, angular_ ;
    double l_scale_, a_scale_, bias;
    int button_, wheelie_;

    ros::Publisher twist_pub_;
    ros::Subscriber joy_sub_;
  
};

Joy_Cntl::Joy_Cntl():
  linear_(1),
  angular_(0),
  button_(10),
  wheelie_(8),
  l_scale_(1),
  a_scale_(180),
  bias(0.5)
{
  nh_.param("axis_linear", linear_, linear_);
  nh_.param("axis_angular", angular_, angular_);
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);
  nh_.param("start_button", button_, button_);
  nh_.param("wheelie_bias",bias,bias);

  twist_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel_s",1);
  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy",10,&Joy_Cntl::joyCallback,this);

}

void Joy_Cntl::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  geometry_msgs::Twist twist;
  // twist.linear.x = (joy->buttons[button_]==1) ? joy->axes[linear_]*l_scale_ : 0;
  if(joy->buttons[button_]==1){
    twist.linear.x = joy->axes[linear_]*l_scale_;
  }else if(joy->buttons[wheelie_]==1){
    twist.linear.x = joy->axes[linear_]*l_scale_;
    twist.linear.y = joy->axes[linear_]*l_scale_*bias;
  }else{
    twist.linear.x = 0;
  }
  twist.angular.z = a_scale_ * joy->axes[angular_];
  twist_pub_.publish(twist);
}

int main(int argc, char** argv)
{
  /* code */
  ros::init(argc, argv, "joy_cntl_");
  Joy_Cntl joy_cntl_ ;
  ros::spin();
}