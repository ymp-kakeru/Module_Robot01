#include <iostream>
#include <fstream>
#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Vector3.h"
#include <std_msgs/String.h>

//#define debug() std::cout << __LINE__ << std::endl;

class Imu_samplinger
{
public:
  Imu_samplinger();

private:
  void imuCb(const sensor_msgs::Imu::ConstPtr& imu);
//  void imuCb(const geometry_msgs::Vector3::ConstPtr& imu);
//  void timeCb(const ros::TimerEvent& event);
  int count ;
  double SampTime ;
  std::string FileName ;//= "/home/ymp/test0.csv";

  ros::NodeHandle nh_;
  ros::Subscriber imu_sub;
};

Imu_samplinger::Imu_samplinger():
  count{0},SampTime{5},FileName{"/home/ymp/hoge.txt"}
{
  nh_.getParam("/imu_samplinger/SamplingTime",SampTime);
  nh_.getParam("/imu_samplinger/FileName",FileName);
  
  ros::Rate rate(SampTime);
  ROS_INFO("rate get %lf",SampTime) ;
  imu_sub = nh_.subscribe<sensor_msgs::Imu>("/hokuyo3d/imu",1000,&Imu_samplinger::imuCb,this);
//  imu_sub = nh_.subscribe<geometry_msgs::Vector3>("/hokuyo3d/imu/linear_acceleration",1000,&Imu_samplinger::imuCb,this);
  rate.sleep();
}

/*void Imu_samplinger::timeCb(const ros::TimerEvent& event)
{
  ros::Timer e = nh_.createTimer(ros::Duration(SampTime),&Imu_samplinger::imuCb);
}*/

void Imu_samplinger::imuCb(const sensor_msgs::Imu::ConstPtr& imu)
//void Imu_samplinger::imuCb(const geometry_msgs::Vector3::ConstPtr& imu)
{
//  sensor_msgs::Imu imu_data;
  geometry_msgs::Vector3 imu_data;
  imu_data.x = imu->linear_acceleration.x;
  imu_data.y = imu->linear_acceleration.y;
  imu_data.z = imu->linear_acceleration.z;

  float time = count / SampTime;
  ROS_INFO("Sampling Now %d",count) ;
  std::ofstream ofs(FileName.c_str(),std::ios::app);
  //std::ostream& ofs {std::cout};
  std::cout << imu_data.x << std::endl ;
   if (!ofs)
  {
    std::cout << "cannot open file " << FileName << std::endl;
    std::exit(EXIT_FAILURE);
  } 

  if(count==0){
    ofs << "time" << "," << "x" << "," << "y" << "," << "z" <<"\n" ;
  }else{
    ofs << time << ",";
    ofs << imu_data.x << ",";
    ofs << imu_data.y << ",";
    ofs << imu_data.z << "\n";
  }
  count++;
}
int main(int argc, char** argv)
{
  ros::init(argc, argv, "imu_samplinger");
  Imu_samplinger imu_samplinger;
  ros::spin();
}