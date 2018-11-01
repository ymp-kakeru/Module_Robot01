#include <iostream>
#include <fstream>
#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include <std_msgs/String.h>

class Imu_samplinger
{
public:
  Imu_samplinger();

private:
  void imuCb(const sensor_msgs::Imu::ConstPtr& imu);
//  void timeCb(const ros::TimerEvent& event);
  int count ;
  double SampTime,SamplingTime ;
  std::string FileName = "test.csv";

  ros::NodeHandle nh_;
  ros::Subscriber imu_sub;
};

Imu_samplinger::Imu_samplinger():
  count(0),SamplingTime(5)
{
  nh_.getParam("SampTime",SamplingTime);
  nh_.getParam("FileName",FileName);

  ROS_INFO("Sampling Now %d",count) ;

  ros::Rate rate(SampTime);
  //while(ros::ok){
    imu_sub = nh_.subscribe<sensor_msgs::Imu>("hokuyo3d/imu",10,&Imu_samplinger::imuCb,this);
    rate.sleep();
  //}
}

/*void Imu_samplinger::timeCb(const ros::TimerEvent& event)
{
  ros::Timer e = nh_.createTimer(ros::Duration(SampTime),&Imu_samplinger::imuCb);
}*/

void Imu_samplinger::imuCb(const sensor_msgs::Imu::ConstPtr& imu)
{
  sensor_msgs::Imu imu_data;
  float time = count * SampTime;

  std::ofstream ofs(FileName.c_str(),std::ios::app);
  if(count==0){
    ofs << "time","time_cpu","x","y","z","\n" ;;
  }else{
    ofs << time,imu_data.linear_acceleration,"\n" ;
  }
  ofs.close();
  count++;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "imu_samplinger");
  Imu_samplinger imu_samplinger;
  ros::spin();
}