#include <iostream>
#include <fstream>
#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include <std_msgs/String.h>

void imuCb(const sensor_msgs::Imu::ConstPtr& imu_sub,int c)
{
  sensor_msgs::Imu imu_sub ;

  float SampTime = 0.2;
  char *FileName ="test.csv";

  nh_.getParam("SampTime",SamplingTime);
  nh_.getParam("FileName",FileName);

  float time = c * SampTime;
  std::ofstream ofs(FileName,std::ios::app);
  if(c==0){
    ofs << "time","x","y","z","\n" <<std::endl;
  }else{
    ofs << time,imu_sub.linear_acceleration/*[1],imu_sub.linear_acceleration[2],imu_sub.linear_acceleration[3]*/,"\n" << std::endl ;
  }
  ofs.close();
}

int main(int argc, char* argv)
{
  int *count = 0;

  ros::init(argc,argv,"imu_samplinger");
  ros::NodeHandle nh_;

  ros::Subscriber imu_sub = nh_.subscribe<sensor_msgs::Imu>("hokuyo3d/imu",10,imuCb(&count));

  if(SampTime>0){
    ros::Rate loop_rate(SampTime);
    while(ros::ok()){
      ros::spinOnce();
      loop_rate.sleep();
      count++;
    }
  }
  else ros::spin();
  return 0;
}

