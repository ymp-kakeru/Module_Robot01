#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>         /* for read, write, close */
#include <sys/ioctl.h>      /* for ioctl */
#include <sys/types.h>      /* for open */
#include <sys/stat.h>       /* for open */
#include <fcntl.h>          /* for open */

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <mod01_driver/mod_drive.h>
#include <mod01_driver/mod_joy.h>

#include <signal.h>

#include "../include/ixis_imcs01/driver/urbtc.h"          /* Linux specific part */
#include "../include/ixis_imcs01/driver/urobotc.h"        /* OS independent part */

static char *devfiles[] = {"/dev/urbtc1", "/dev/urbtc2", "/dev/urbtc3"};

#define max_enc 65535
#define wheel_diameters 0.2 //[m]
#define geer_ratio 4 ;


#define right_motor_CH 2 	// CN103
#define left_motor_CH 3 	// CN104
#define right_encoder_CH 2	// CN107
#define left_encoder_CH	 3	// CN108

using namespace std;
class Mod_wheel_cntl
{
	public:
		//Mod_enc_read();
		Mod_wheel_cntl();

	private:
		imcs_func(int numOfControllers, const sensor_msgs::Joy::ConstPtr& joy);
		//void mod_encCallback(const mod01_driver::mod_drive::ConstPtr& drive);
		void mod_joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

		ros::NodeHandle nh_;
		ros::Subscriber joy_sub;
		//ros::Subscriber enc_sub;
		//ros::Publisher enc_pub;
		int axis_linear;
		int axis_angular;
		int angular_offset;
		//l_scale(0.1),
		//a_scale(60),
		int drive_brake;
		int numOfControllers;
		int first;

};
void Mod_wheel_cntl::mod_joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
	ROS_INFO("joy get");
	imcs_func(numOfControllers, sensor_msgs::Joy joy);

}
/*Mod_wheel_cntl::Mod_enc_read():
{
	ros::init(argc.argv,"Mod_Enc_Read");
	ros::NodeHandle nh_;
	enc_pub = nh_advertise<mod01_driver::mod_drive>("mod_enc/state",1);
	enc_read();
}*/

Mod_wheel_cntl::Mod_wheel_cntl()
{
	//ros::init(argc,argv,"")
	//enc_sub = nh_advertise<mod01_driver::mod_drive>("mod_enc/state",1000,mod_encCallback);
	nh_.getParam("/numOfControllers",numOfControllers);
	nh_.getParam("/first",first);
	joy_sub = nh_.subscribe<sensor_msgs::Joy>("joy",1,&Mod_wheel_cntl::mod_joyCallback);
}

Mod_wheel_cntl::imcs_func(int numOfControllers,const sensor_msgs::Joy joy):
	axis_linear(1),	axis_angular(0),angular_offset(0.3),
	drive_brake(6)
{
	struct uin ibuf;
 	struct uout obuf[numOfControllers];
  	struct ccmd cmd;
  	static double time[4]	= {1,0,0};			//------array [0 1 2]----------//
	static int enc_ct_right[4] = {0,0,0};		//  0:current, 1:last, 2:diff  //
	static int enc_ct_left[4]	= {0,0,0};		//-----------------------------//
  	int fd, fds[numOfControllers];
  	int i, j;
  	double length_right[numOfControllers] = {0,};
  	double length_left[numOfControllers] = {0,};
  	double vel_R[numOfControllers] = {0,};
  	double vel_L[numOfControllers] = {0,};
	double vel_Rd=0 , vel_Ld=0 ; 

	while(first==0){
  	for (i=0; i<numOfControllers; i++) {
    if ((fd = open(devfiles[i], O_RDWR)) == -1) {
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
    cmd.retval = 0 /* RETURN_VAL */;
    cmd.setoffset  = CH0 | CH1 | CH2 | CH3;
    cmd.setcounter = CH0 | CH1 | CH2 | CH3;
    cmd.resetint   = CH0 | CH1 | CH2 | CH3;
    cmd.selin = CH2 | CH3 | SET_SELECT; /* SET_CH2_HIN | SET_SELECT;*/
    cmd.selout = SET_SELECT | CH2 | CH3; /* EEPROM データが正しければ不要 */

//#if __BYTE_ORDER == __LITTLE_ENDIAN
    cmd.offset[0] = cmd.offset[1] = cmd.offset[2] = cmd.offset[3] = 0x7fff;
    cmd.counter[0] = cmd.counter[1] = cmd.counter[2] = cmd.counter[3] = 0;
/*#else
    cmd.offset[0] = cmd.offset[1] = cmd.offset[2] = cmd.offset[3] = 0xff7f;
    cmd.counter[0] = cmd.counter[1] = cmd.counter[2] = cmd.counter[3] = 0;
#endif*/

    cmd.posneg = SET_POSNEG | CH0 | CH1 | CH2 | CH3;
    cmd.breaks = SET_BREAKS | CH0 | CH1 | CH2 | CH3;

    if (ioctl(fd, URBTC_COUNTER_SET) < 0){
      fprintf(stderr, "ioctl: URBTC_COUNTER_SET error\n");
      exit(1);
    }
    if (write(fd, &cmd, sizeof(cmd)) < 0) {
      fprintf(stderr, "write error\n");
      exit(1);
    }
    if (ioctl(fd, URBTC_DESIRE_SET) < 0){
      fprintf(stderr, "ioctl: URBTC_DESIRE_SET error\n");
      exit(1);
    }
  }


  for (j=0; j<numOfControllers; j++) {
    for (i=0; i<4; i++) {
//#if __BYTE_ORDER == __LITTLE_ENDIAN
      obuf[j].ch[i].x = 0;
      obuf[j].ch[i].d = 0;
      obuf[j].ch[i].kp = 0;
      obuf[j].ch[i].kpx = 1;
      obuf[j].ch[i].kd = 0;
      obuf[j].ch[i].kdx = 1;
      obuf[j].ch[i].ki = 0;
      obuf[j].ch[i].kix = 1;
/*#else
      obuf[j].ch[i].x = 0;
      obuf[j].ch[i].d = 0;
      obuf[j].ch[i].kp = 0;
      obuf[j].ch[i].kpx = 0x0100;
      obuf[j].ch[i].kd = 0;
      obuf[j].ch[i].kdx = 0x0100;
      obuf[j].ch[i].ki = 0;
      obuf[j].ch[i].kix = 0x0100;
#endif*/
    }
//#if __BYTE_ORDER == __LITTLE_ENDIAN
    obuf[j].ch[2].kp = 0x10;
    obuf[j].ch[3].kp = 0x10;
/*#else
    obuf[j].ch[2].kp = 0x1000;
    obuf[j].ch[3].kp = 0x1000;
#endif
  }*/
  nh_.setpalam("/first",1) ;
	}
}
for(j=0;j<numOfControllers;j++){
	//ioctl(fds[j],URBTC_CONTINUOUS_READ);
	read(fds[j],&ibuf,sizeof(ibuf));
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
	length_right[j] = enc_ct_right[2]/max_enc * circumference * geer_ratio;
	length_left[j] = enc_ct_left[2]/max_enc * circumference * geer_ratio;
	delta_time = time[2]/1000 ;

	vel_R[j] = length_right[j]/delta_time;
	vel_L[j] = length_left[j]/delta_time;
	ROS_INFO("velocity_R = %.5lf, velocity_L = %.5lf",vel_R[j],vel_L[j]);
	}
	// turn mode


	while((joy->axes[axis_angular] > angular_offset) || (joy->axes[axis_angular] < angular_offset) {
		if(joy->axes[axis_angular] > 0){
			i=0;
			unsigned short a = 300.0*sin(i*3.14/655.360) + 512.0;
    		a <<= 5;
    		for (j=0; j<numOfControllers; j++) {
			// #if __BYTE_ORDER == __LITTLE_ENDIAN
    			obuf[j].ch[left_motor_CH].x = a;
    			obuf[j].ch[right_motor_CH].x = -a;
			// #else
   //  		  obuf[j].ch[2].x = obuf[j].ch[3].x = ((a & 0xff) << 8 | (a & 0xff00) >> 8);
			// #endif
    			if (write(fds[j], &obuf[j], sizeof(obuf[j])) > 0) {
					i++;
    	 		} else {
					printf("write err\n");
				break;
      			}
    		}	
		}else{
			i=0;
			unsigned short a = 300.0*sin(i*3.14/655.360) + 512.0;
    		a <<= 5;
    		for (j=0; j<numOfControllers; j++) {
			// #if __BYTE_ORDER == __LITTLE_ENDIAN
    		obuf[j].ch[left_motor_CH].x = -a;
    		obuf[j].ch[right_motor_CH].x = a;
			// #else
   //  		  obuf[j].ch[2].x = obuf[j].ch[3].x = ((a & 0xff) << 8 | (a & 0xff00) >> 8);
			// #endif
    			if (write(fds[j], &obuf[j], sizeof(obuf[j])) > 0) {
					i++;
    	 		} else {
					printf("write err\n");
					break;
      			}
    		}
		} 
	}

	// break mode
	if(joy->buttons[drive_brake] == 1){
		vel_Ld=vel_Rd=joy->axes[axis_linear]
	} else {
		vel_Ld=vel_Rd=0;
	}

	for(j=0;j<numOfControllers;j++){
		cmd.offset[right_motor_CH]	= vel_Rd - vel_R[j];
		cmd.offset[left_motor_CH]	= vel_Ld - vel_L[j];
		if (write(fds[j], &obuf[j], sizeof(obuf[j])) > 0) {
			i++;
    	} else {
			printf("write err\n");
			break;
      	}
	}



int main(int argc, char **argv)
{
	ros::init(argc,argv,"Mod_drive");
	ros::Rate loop_rate(10);
	Mod_wheel_cntl::Mod_wheel_cntl();
	ros::spin();
	loop_rate.sleep();
}


