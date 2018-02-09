#include <iostream>
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <mod01_driver/mod_joy.h>

//****************************** Joy rule ****************************************/
//mod_type 				: Type_2=Square[0] , Type_4=Cross[1] , Typw_6=Circle[2]
//accept_mod_type		: option[9]
//drive_brake_button 	: L2 trigger[6]
//spin_turn_button		: R2 trigger[7]
//
//
//*******************************************************************************/
using namespace std ;

class Mod_joy_cntl
{
	public:
		Mod_joy_cntl();

	private:
		void mod_joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

		ros::NodeHandle nh;
		ros::Subscriber joy_sub;
		ros::Publisher drive_pub;

		int axis_linear , axis_angular ;
		double l_scale , a_scale ;
		int drive_brake ;
		int flag, type ;
};

Mod_joy_cntl::Mod_joy_cntl():
	axis_linear(1),
	axis_angular(0),
	l_scale(0.1),
	a_scale(60),
	drive_brake(6),flag(0),type(0)
{
	joy_sub=nh.subscribe<sensor_msgs::Joy>("joy",1,&Mod_joy_cntl::mod_joyCallback, this);
	drive_pub=nh.advertise<mod01_driver::mod_joy>("mod/joy_cntl",1);
}

void Mod_joy_cntl::mod_joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
	mod01_driver::mod_joy mod_joy_;
	std::string line ;
	ROS_INFO("hello");
	
	if(joy->buttons[drive_brake]==1){
		mod_joy_.linear = joy->axes[axis_linear]*l_scale;
	}else{
		mod_joy_.linear = 0;
	}
	mod_joy_.angular = a_scale*joy->axes[axis_angular];
	mod_joy_.spin_turn = joy->axes[7];
	drive_pub.publish(mod_joy_) ;
}

int main(int argc, char **argv)
{
	ros::init(argc,argv,"mod_joy_cntl");
	while(flag==1){
		ROS_INFO("please select module type (2:Square, 4:Cross, 6:Circle)\n");
		/*if(joy->axes[0]==1) {
			type = 2;
		}else if(joy->axes[1]==1){
			type = 4;
		}else if(joy->axes[2]==1){
			type = 6;
		}else{
			cout << "pls select";
		}*/
		cin >> type ;
		cout << "module is " << type << ". ok ? (Press Enter or else)";
		if((getline(cin,line)) == ""){
			cout << "JOB START" ;
			mod_joy_.mod_type = type/2 ; //mod num 1, 2 or 3 
			flag = 1;
		}
	}
	Mod_joy_cntl Mod_joy_cntl ;
	ros::spin() ;
}