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
		Mod_wheel_cntl();
		
	private:
		void imcs_func(const sensor_msgs::Joy::ConstPtr& joy);//int numOfControllers,int first);//, const sensor_msgs::Joy::ConstPtr& joy);
		
		ros::NodeHandle nh_;
		ros::Subscriber joy_sub;
		int numOfControllers,first;
};

Mod_wheel_cntl::Mod_wheel_cntl()
{
	joy_sub = nh_.subscribe<sensor_msgs::Joy>("joy",10,&Mod_wheel_cntl::imcs_func,this);
}

void Mod_wheel_cntl::imcs_func(const sensor_msgs::Joy::ConstPtr& joy)//,const sensor_msgs::Joy joy):
{
	
	//int numOfControllers,first;
	nh_.getParam("/numOfControllers",numOfControllers);
	nh_.getParam("/first",first);
	fprintf(stderr,"start\n");
	int axis_linear=1;
	int axis_angular=0;
	int angular_offset=0.3;
	int a=0;
	//l_scale(0.1),
	//a_scale(60),
	int drive_brake=6;
	
	struct uin ibuf;
 	struct uout obuf[numOfControllers];
  	struct ccmd cmd;
  	static double time[4]	= {1,0,0};			//------array [0 1 2]----------//
	static int enc_ct_right[4] = {0,0,0};		//  0:last, 1:current, 2:diff  //
	static int enc_ct_left[4]	= {0,0,0};		//-----------------------------//
  	int fd, fds[numOfControllers];
  	int i, j;
  	double length_right[4] = {};
  	double length_left[4]={};//numOfControllers] = {};
  	double vel_R=0;//numOfControllers]// = {};
  	double vel_L=0;//numOfControllers]// = {};
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

#if __BYTE_ORDER == __LITTLE_ENDIAN
    cmd.offset[0] = cmd.offset[1] = cmd.offset[2] = cmd.offset[3] = 0x7fff;
    cmd.counter[0] = cmd.counter[1] = cmd.counter[2] = cmd.counter[3] = 0;
#else
    cmd.offset[0] = cmd.offset[1] = cmd.offset[2] = cmd.offset[3] = 0xff7f;
    cmd.counter[0] = cmd.counter[1] = cmd.counter[2] = cmd.counter[3] = 0;
#endif

    cmd.posneg = SET_POSNEG | CH0 | CH1 | CH2 | CH3;
    cmd.breaks = SET_BREAKS | CH0 | CH1 | CH2 | CH3;
    fprintf(stderr, "Line 160\n");
    if (ioctl(fd, URBTC_COUNTER_SET) < 0){
      fprintf(stderr, "ioctl: URBTC_COUNTER_SET error\n");
      exit(1);
    }else{fprintf(stderr, "ioctl ok\n");}
    if (write(fd, &cmd, sizeof(cmd)) < 0) {
      fprintf(stderr, "write error\n");
      exit(1);
    }else{fprintf(stderr, "write168 ok\n");}
    if (ioctl(fd, URBTC_DESIRE_SET) < 0){
      fprintf(stderr, "ioctl: URBTC_DESIRE_SET error\n");
      exit(1);
    }
  }
  fprintf(stderr, "Line 175\n");


  for (j=0; j<numOfControllers; j++) {
    for (i=0; i<4; i++) {
      obuf[j].ch[i].x = 0;
      obuf[j].ch[i].d = 0;
      obuf[j].ch[i].kp = 0;
      obuf[j].ch[i].kpx = 1;
      obuf[j].ch[i].kd = 0;
      obuf[j].ch[i].kdx = 1;
      obuf[j].ch[i].ki = 0;
      obuf[j].ch[i].kix = 1;
    }

    obuf[j].ch[2].kp = 0x10;
    obuf[j].ch[3].kp = 0x10;

    fprintf(stderr,"1:first = %d\n",first);

   	nh_.deleteParam("/first");
   	nh_.setParam("/first",1);
   	nh_.getParam("/first",first);
    fprintf(stderr,"2:first = %d\n",first);
	}
}
    fprintf(stderr,"while out \n");
//-------------------------------------------------------
	if(joy->buttons[drive_brake] == 1){
		vel_Ld=vel_Rd=joy->axes[axis_linear];
	} else {
		vel_Ld=vel_Rd=0;
	}
	fprintf(stderr,"vel_Rd=%.3lf \n",vel_Rd);
	i=0;
while(i <= 10){
	if(vel_Rd==0){
		i = 100;
	}else{
	unsigned short a = 300.0*sin(i*3.14/655.360) + 512.0;
    a <<= 5;
    for (j=0; j<numOfControllers; j++) {
		obuf[j].ch[left_motor_CH].x = a;
    	obuf[j].ch[right_motor_CH].x = -a;
		if (write(fds[j], &obuf[j], sizeof(obuf[j])) > 0) {
			i++;
    	} else {
			printf("write err\n");
			break;
      	}
    }
}
}
	fprintf(stderr,"--------------------End Callback---------------------- \n");
};


int main(int argc, char **argv)
{
	ros::init(argc,argv,"Mod_drive");
	ros::NodeHandle n ;
	ros::Rate loop_rate(10);
	fprintf(stderr,"main\n" );
	Mod_wheel_cntl Mod_wheel_cntl_;
	ros::spin();
	loop_rate.sleep();
};


