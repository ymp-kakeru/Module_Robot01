#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <fstream>
#include <numeric>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>


// iMCs01
#include "../include/ixis_imcs01/driver/urbtc.h"          /* Linux specific part */
#include "../include/ixis_imcs01/driver/urobotc.h"        /* OS independent part */

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <sensor_msgs/JointState.h>

static char *devfiles[] = {"/dev/urbtc1", "/dev/urbtc2", "/dev/urbtc3"};

geometry_msgs::Twist target_vel;

using namespace std;

class ModRobotDriver
{
public:
	ModRobotDriver();
	~ModRobotDriver();

	//int getEncoderData(ros::Time &time);
	int getEncoderCounts(ros::Time &time);
	int Drive(geometry_msgs::Twist cmd);	

private:
	double ros_rate;
	double wheel_diameter;
	double max_linear_vel;
	double max_angular_vel;
	double dead_zone_vel;
	double target_vel;
	double target_angular;
	bool brake;

	int motor_ch_right;
	int motor_ch_left;
	int enc_ch_right;
	int enc_ch_left;
	int numOfControllers;

	int fd, fds[4];
	struct uin ibuf;
	struct uout obuf[4];
	struct ccmd cmd;

	double geer_ratio;
	double delta_time;
	double delta_dist_right;
	double delta_dist_left;
	double vel_right[2];
	double vel_left[2];
	double gain_p_right;
	double gain_p_left;
	double gain_i_right;
	double gain_i_left;
	//double max_vel_right;
	//double max_vel_left;
	int i,j;
	ros::NodeHandle n;
	//ros::Subscriber joy_sub;
	//ros::Publisher js_pub;

	int openSerialPort();
	int closeSerialPort();
	int driveDirect(double target_vel, double target_angular, bool brake);
	//void Drive(const sensor_msgs::Joy::ConstPtr& joy);


	
};

ModRobotDriver::ModRobotDriver():
	ros_rate(10),
	wheel_diameter(0.2),
	max_linear_vel(1.0),
	max_angular_vel(M_PI),
	dead_zone_vel(0.05),
	motor_ch_right(3),
	motor_ch_left(2),
	enc_ch_left(2),
	enc_ch_right(3),
	geer_ratio(4),
	delta_time(0),
	delta_dist_left(0),
	delta_dist_right(0),
	gain_p_right(10.0),
	gain_p_left(10.0),
	gain_i_right(0),
	gain_i_left(0),
	i(0),j(0)
{
	n.getParam("/numOfControllers",numOfControllers);
	max_linear_vel=wheel_diameter * M_PI * 100/60;
	vel_left[2]={};
	vel_right[2]={};

	if(openSerialPort() != 0){
	ROS_WARN("Could not connect to iMCS01.");
	ROS_BREAK();
  	}

  // ------ Joint State Publisher ------
	//js_pub = n.advertise<sensor_msgs::JointState>("/joint_states", 1);
  	
  	//joy_sub = n.subscribe<sensor_msgs::Joy>("joy",10,&ModRobotDriver::Drive,this);
};

ModRobotDriver::~ModRobotDriver()
{
	closeSerialPort();
}

/*nt ModRobotDriver::Joy_sub(ros::NodeHandle &n)
{
	fprintf(stderr, "////joy sub////\n");
	ros::Subscriber joy_sub = joy_sub = n.subscribe<sensor_msgs::Joy>("joy",10,&ModRobotDriver::Drive,this);		
	fprintf(stderr, "////joy sub end////\n");
	return 0;
}*/

int ModRobotDriver::openSerialPort()
{
	for (i=0; i<numOfControllers; i++) {
    if ((fd = open(devfiles[i], O_RDWR )) == -1) {
      fprintf(stderr, "%s: Open error\n", devfiles[i]);
      exit(1);
    }
    if (ioctl(fd, URBTC_CONTINUOUS_READ) < 0){
      fprintf(stderr, "ioctl: URBTC_CONTINUOUS_READ error\n");
      exit(1);
    }
    if (read(fd, &ibuf, sizeof(ibuf)) != sizeof(ibuf)) {
      fprintf(stderr, "Read size mismatch.\n");
      exit(1);
    }
#if __BYTE_ORDER == __BIG_ENDIAN
    ibuf.magicno = (0xff & ibuf.magicno)<<8 | (0xff00 & ibuf.magicno)>>8;
#endif
    if (ibuf.magicno == 0) {
      fds[0] = fd;
      fprintf(stderr, "Found controller #0.\n");
    } else if (ibuf.magicno == 1) {
      fds[1] = fd;
      fprintf(stderr, "Found controller #1.\n");
    } else if (ibuf.magicno == 2) {
      fds[2] = fd;
      fprintf(stderr, "Found controller #2.\n");
    } else {
      fprintf(stderr, "Wrong magic no: %d.\n", ibuf.magicno);
      exit(1);
    }
    cmd.retval = 0  /*RETURN_VAL*/ ;
    cmd.setoffset  = CH0 | CH1 | CH2 | CH3;
    cmd.setcounter = CH0 | CH1 | CH2 | CH3;
    cmd.resetint   = CH0 | CH1 | CH2 | CH3;
    cmd.selin =  SET_SELECT; /* SET_CH2_HIN | SET_SELECT;*/
    cmd.selout = SET_SELECT | CH0 | CH1 | CH2 | CH3; /* EEPROM $B%G!<%?$,@5$7$1$l$PITMW(B */

#if __BYTE_ORDER == __LITTLE_ENDIAN
    cmd.offset[0] = cmd.offset[1] = cmd.offset[2] = cmd.offset[3] = 0x7fff;
    cmd.counter[0] = cmd.counter[1] = cmd.counter[2] = cmd.counter[3] = 0;
#else
    cmd.offset[0] = cmd.offset[1] = cmd.offset[2] = cmd.offset[3] = 0xff7f;
    cmd.counter[0] = cmd.counter[1] = cmd.counter[2] = cmd.counter[3] = 0;
#endif

    cmd.posneg = SET_POSNEG | CH0 | CH1 | CH2 | CH3;
    cmd.breaks = SET_BREAKS ;

    if (ioctl(fd, URBTC_COUNTER_SET) < 0){
      fprintf(stderr, "ioctl: URBTC_COUNTER_SET error\n");
      exit(1);
    }
    if (write(fd, &cmd, sizeof(cmd)) < 0) {
      fprintf(stderr, "write error\n");
      exit(1);
    }
    /*if (ioctl(fd, URBTC_DESIRE_SET) < 0){
      fprintf(stderr, "ioctl: URBTC_DESIRE_SET error\n");
      exit(1);
    }*/
    cmd.setcounter = 0 ;	
	}
	fprintf(stderr, "-----------Open serial port---------\n" );
	return 0;

}

int ModRobotDriver::closeSerialPort()
{
	for(j=0;j<=numOfControllers;j++){
		cmd.offset[motor_ch_left] = 0x7fff;
		cmd.offset[motor_ch_right]= 0x7fff;
		cmd.breaks = SET_BREAKS;
		/*if (ioctl(fds[j], URBTC_COUNTER_SET) < 0){
     		 fprintf(stderr, "ioctl: URBTC_COUNTER_SET error l196\n");
      		exit(1);
    	}*/
    	if (write(fds[j], &cmd, sizeof(cmd)) < 0) {
      		fprintf(stderr, "write error\n");
      		exit(1);
    	}
    	close(fds[j]);
    }
	 cout << "[ModRobotDriver] Closed the iMCs01 Port." << endl;
	return 0;
}

int ModRobotDriver::getEncoderCounts(ros::Time &time)
{
	for(j=0;j<=numOfControllers;j++){
	if(read(fds[j], &ibuf, sizeof(ibuf)) != sizeof(ibuf)){
		ROS_WARN("Failed to get Encoder info.");
		return 1;
  	} 
  	else{ time = ros::Time::now();
	}
	static double times[3] = {1, 0, 0};
	static int enc_cnt_right[3] = {0, 0, 0};
	static int enc_cnt_left[3] = {0, 0, 0};

	sensor_msgs::JointState js;
	js.header.stamp = ros::Time::now();
	js.name.push_back("right_wheel_joint");
	js.name.push_back("left_wheel_joint");
  // for rotation
	static double sum_rad_right = 0;
 	static double sum_rad_left = 0;
 
  // ------ update current datas ------
  // get raw datas
 	times[0] = (double)ibuf.time;
 	enc_cnt_left[0] = (int)(ibuf.ct[enc_ch_left]);
 	enc_cnt_right[0] = (int)(ibuf.ct[enc_ch_right]);

 	times[2] = times[0] - times[1];

 	enc_cnt_left[2] = enc_cnt_left[0] - enc_cnt_left[1];
 	enc_cnt_right[2] = enc_cnt_right[0] - enc_cnt_right[1];

	// ------ check the overflow ------
  	// about diff time
  	if(times[2] < 0)
		 times[2] += 65536;     
	// about diff enc_cnt_right
  	if(enc_cnt_right[2] > 6553)
		enc_cnt_right[2] = enc_cnt_right[2] - 65536;
  	else if(enc_cnt_right[2] < -6553)
		enc_cnt_right[2] = enc_cnt_right[2] + 65536;
	// about diff enc_cnt_left
  	if(enc_cnt_left[2] > 6553)
		enc_cnt_left[2] = enc_cnt_left[2] - 65536;
	else if(enc_cnt_left[2] < 6553)
		enc_cnt_left[2] = enc_cnt_left[2] + 65536;
	// ------ finish to check the over flow ------
    // ------ finish to update current datas ------

	// ------ update the output datas ------
	// get dist datas
	delta_dist_right = (enc_cnt_right[2]/4000.0/geer_ratio)*(wheel_diameter*M_PI);
  	delta_dist_left = -(enc_cnt_left[2]/4000.0/geer_ratio)*(wheel_diameter*M_PI);
  	// get delta time (change from [ms] to [s] on diff time)
  	delta_time = times[2]/1000.0;
  	if(delta_time <=0)
	delta_time = 1./ros_rate;

	// calcurate rotation
  	sum_rad_right += enc_cnt_right[2]/4000.0*M_PI;
  	sum_rad_left -= enc_cnt_left[2]/4000.0*M_PI;
  	/*js.position.push_back(sum_rad_right);
  	js.position.push_back(sum_rad_left);
  	js_pub.publish(js);*/
  	// ------ finish to update the output datas ------
  
 	 // ------ update the past datas ------
 	times[1] = times[0];
  	enc_cnt_right[1] = enc_cnt_right[0];
  	enc_cnt_left[1] = enc_cnt_left[0];
  	// ------ finish to update the past data ------
  	}
  	fprintf(stderr, "------Read encoder-------\n" );
	return 0;
}

int ModRobotDriver::Drive(geometry_msgs::Twist cmd)
{
	fprintf(stderr, "L291\n" );
	double target_vel = 0 ;
	double target_angular = 0;
	bool brake = false ;

	if(abs(cmd.linear.x) > abs(max_linear_vel))
		cmd.linear.x = cmd.linear.x/abs(cmd.linear.x) * max_linear_vel;
 	if(abs(cmd.angular.z) > abs(max_angular_vel))
		cmd.angular.z = cmd.angular.z/abs(cmd.angular.z) * max_angular_vel;
  
  	if(cmd.linear.x == 0 && cmd.angular.z == 0)
		brake = true;
	target_vel = cmd.linear.x;
	target_angular = cmd.angular.z;

	fprintf(stderr, "target_vel = %lf\n",target_vel );
	fprintf(stderr, "-------Drive------\n" );
	return driveDirect(target_vel, target_angular, brake);
}	

int ModRobotDriver::driveDirect(double target_vel, double target_angular, bool brake)
{
	// for brake input
  unsigned char input_brake = SET_BREAKS;
  // for I-P control
  // ------ Rule ------
  //   current : 0
  //   last    : 1
  // ------------------
  static double control_input_vel_right[2] = {0, 0};
  static double control_input_vel_left[2] = {0, 0};
  static double error_vel_right[2] = {0, 0};
  static double error_vel_left[2] = {0, 0};
  static int over_vel_cnt_right = 0;
  static int over_vel_cnt_left = 0;
  static int cnt = 0;


  
// =============================================================
  //                      I-P Control
  // 指令値の急激な変化にある程度鈍感な制御手法．I(積分)の項がある
  // ので，内部補償原理を満たす．
  // =============================================================
  // === Braking =========================================
  // 入力速度が 0 の場合，現在速度の逆を入力にすることで
  // I-Pでも即応性のあるブレーキングを試みている．
  // また，速度 0 付近での振動を抑制するために不感帯を
  // 設けている．
  if(brake){
	target_vel = -vel_right[0];
	target_vel = -vel_left[0];
  }
  // ------ update current data ------  
  // error vel
  error_vel_right[0] = target_vel - vel_right[0];
  error_vel_left[0] = target_vel - vel_left[0];
  // control vel
  control_input_vel_right[0] = control_input_vel_right[1] + gain_p_right*(vel_right[0]-vel_right[1]) + (1.0/2.0)*gain_i_right*(error_vel_right[0]+error_vel_right[1]);
  control_input_vel_left[0] = control_input_vel_left[1] + gain_p_left*(vel_left[0]-vel_left[1]) + (1.0/2.0)*gain_i_left*(error_vel_left[0]+error_vel_left[1]);
  // ------ finish to update current data ------

  // ------ finish to update last data ------
  if(brake){
	if(abs(vel_right[0]) < dead_zone_vel && abs(vel_left[0]) < dead_zone_vel){
	  control_input_vel_right[0] = 0;
	  control_input_vel_left[0] = 0;
	  input_brake = input_brake | motor_ch_left | motor_ch_left;
	}
	else{
	  if(abs(vel_right[0]) < 0.05){
		control_input_vel_right[0] = 0;
		input_brake = input_brake | motor_ch_right;
	  }
	  if(abs(vel_left[0]) < 0.05){
		control_input_vel_left[0] = 0;
		input_brake = input_brake | motor_ch_left;
	  }
	}
}
if( (fabs(vel_right[0]-control_input_vel_right[0]) > 0.3) && fabs(vel_right[0]) < 0.25){
    ROS_WARN("Emergency Button Pushed...Right");
    control_input_vel_right[0] = 0;
  }
  if( (fabs(vel_left[0]-control_input_vel_left[0]) > 0.3) && fabs(vel_left[0]) < 0.25){
    ROS_WARN("Emergency Button Pushed...Left");
    control_input_vel_left[0] = 0;
  }  
  
  // ------ update last data ------
  // error vel
  error_vel_right[1] = error_vel_right[0];
  error_vel_left[1] = error_vel_left[0]; 
  // control vel
  control_input_vel_right[1] = control_input_vel_right[0];
  control_input_vel_left[1] = control_input_vel_left[0];
  // ==============================================================
	  
  // ------ write input datas to iMCs01 ------
  // culculate input data
  double input_rate_right = control_input_vel_right[0]/max_linear_vel;
  double input_rate_left = control_input_vel_left[0]/max_linear_vel;
  // ------ over flow ------
  //input_rate_right = input_rate_left = 0.3;

  if(abs(input_rate_right) > 1)
	input_rate_right /= abs(input_rate_right);
  if(abs(input_rate_left) > 1)
	input_rate_left /= abs(input_rate_left);

	fprintf(stderr, "input_rate_left = %lf\n",input_rate_left );
  // culculate input data
  cmd.offset[motor_ch_right] =  (int)(0x7fff + 0x7fff*input_rate_right); 
  cmd.offset[motor_ch_left] = (int)(0x7fff + 0x7fff*input_rate_left);
  //cout << cmd_ccmd.offset[motor_pin_right] << ", " << cmd_ccmd.offset[motor_pin_left] << endl;
  cmd.breaks = input_brake;
  // write
  
  for(j=0;j<=numOfControllers;j++){
  /*if(ioctl(fds[j], URBTC_COUNTER_SET) < 0){
	
	throw logic_error("Faild to ioctl: URBTC_COUNTER_SET_i");
  }*/
  if(write(fd, &cmd, sizeof(cmd)) < 0){
	
	throw logic_error("Faild to write");
  }
}
  // ------ finish to write input datas to iMCs01 ------
  return 0;
}

void CmdVelCallback(const geometry_msgs::Twist::ConstPtr& cmd_vel)
{
	target_vel.linear.x = cmd_vel->linear.x;
	target_vel.angular.z = cmd_vel->angular.z;
	fprintf(stderr, "vel_linear.x = %lf\n",cmd_vel->linear.x );
}


int main(int argc, char** argv)
{
	ros::init(argc, argv, "Mod_robot_Driver");
	ros::NodeHandle nh;
	double ros_rate = 10;
	ModRobotDriver Mod_robot_Driver;
	ros::Subscriber cmd_vel_sub = nh.subscribe<geometry_msgs::Twist>("/cmd_vel_s", 1, CmdVelCallback);
	target_vel.linear.x = 0;
	target_vel.angular.z = 0;
	geometry_msgs::Twist local_target_vel;

	ros::Rate r(ros_rate);
	ros::Time time;
	
	fprintf(stderr, "...Mod Robot Start up....\n" );
	while(ros::ok()){
		Mod_robot_Driver.getEncoderCounts(time);
		Mod_robot_Driver.Drive(local_target_vel);

		ros::spinOnce();
		r.sleep();
	}
	return 0;
};




