#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>         /* for read, write, close */
#include <sys/ioctl.h>      /* for ioctl */
#include <sys/types.h>      /* for open */
#include <sys/stat.h>       /* for open */
#include <fcntl.h>          /* for open */

#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/String.h>
#include <mod01_driver/mod_drive.h>
#include <mod01_driver/mod_joy.h>

#include <signal.h>

#include "../include/ixis_imcs01/driver/urbtc.h"          /* Linux specific part */
#include "../include/ixis_imcs01/driver/urobotc.h"        /* OS independent part */

static char *devfiles[] = {"/dev/urbtc0", "/dev/urbtc1", "/dev/urbtc2"};
using namespace std;

//Mod01_property
#define max_enc 65535
#define wheel_diameters 0.2 //[m]
#define geer_ratio 4 ;


//#define numOfControllers 3
#define right_encoder_CH 2	// CN107
#define left_encoder_CH	 3	// CN108

int encoder_read()
{
	struct uin ibuf;
	struct uout obug[numOfControllers];
	static double time[4]	= {1,0,0};		//------array [0 1 2]----------//
	static int enc_ct_right[4] = {0,0,0};		//  0:current, 1:last, 2:diff  //
	static int enc_ct_left[4]	= {0,0,0};		//-----------------------------//
	int fd, fds[numOfControllers];
	int i, j;
	double circumference = wheel_diameters*M_PI; //2pi*r
	double length_right, length_left,delta_time ; 							


	mod01_driver::mod_drive wheel_state;

	for (i=0; i<numOfControllers; i++) {
    	if ((fd = open(devfiles[i], O_RDWR)) == -1) {				//devise open
    		  fprintf(stderr, "%s: Open error\n", devfiles[i]);
    		  exit(1);
    	}
    	if (ioctl(fd, URBTC_CONTINUOUS_READ) < 0){					//iMCs online
    		fprintf(stderr, "ioctl: URBTC_CONTINUOUS_READ error\n");
      		exit(1);
    	}
    	if (read(fd, &ibuf, sizeof(ibuf)) != sizeof(ibuf)) {		//data read to ibuf
      		fprintf(stderr, "Read size mismatch.\n");
      		exit(1);
    	}
	
	/*wheel_state.name.push_back("right_wheel_js_%d",i);
	wheel_state.name.push_back("left_wheel_js_%d",i);*/

	/*update current datas*/
	time[0] = time[1];
	time[1] = (double)ibuf.time;
	time[2] = time[1]-time[0];

	enc_ct_right[0] = enc_ct_right[1];
	enc_ct_right[1] = (int)ibuf.ct[right_encoder_CH];
	enc_ct_right[2] = enc_ct_right[1]-enc_ct_right[0];

	enc_ct_left[0] = enc_ct_left[1];
	enc_ct_left[1] = (int)ibuf.ct[left_encoder_CH];
	enc_ct_left[2] = enc_ct_left[1]-enc_ct_left[0];

	/*check the overflow*/
 	// about diff time
	if(time[2] < 0)
		time[2] += 65536;     
	// about diff enc_cnt_right
	if(enc_ct_right[2] > 6553)		// 6553 = 65536/10 
		enc_ct_right[2] = enc_ct_right[2] - 65536;
  	else if(enc_ct_right[2] < -6553)
		enc_ct_right[2] = enc_ct_right[2] + 65536;
  	// about diff enc_cnt_left
  	if(enc_ct_left[2] > 6553)
		enc_ct_left[2] = enc_ct_left[2] - 65536;
  	else if(enc_ct_left[2] < -6553)
		enc_ct_left[2] = enc_ct_left[2] + 65536;

	/*check end*/

	length_right = enc_ct_right[2]/max_enc * circumference * geer_ratio;
	length_left = enc_ct_left[2]/max_enc * circumference * geer_ratio;
	delta_time = time[2]/1000 ;

	wheel_state.velocity_R = (length_right/delta_time);
	wheel_state.velocity_L = (length_left/delta_time);
	wheel_state_pub.publish(wheel_state);

	}
	close(fd);

	return 0;

}

void mod_type_Callback(const mod01_driver::mod_joy msg)
{
	int numOfControllers = msg.mod_type ;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "wheel_state_pub");
	ros::NodeHandle nh ;
	ros::Subscriber mod_type_sub = nh.subscribe("mod/joy_cntl",10,mod_type_Callback);
	ros::Publisher wheel_state_pub = nh.advertise<mod01_driver::mod_drive>("wheel_state",1) ;

	ros::Rate loop_rate(1000) ;

	encoder_read();

	ros::spin();

}