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

#include <senser_msgs/JointState.h>

class mod01_cntl
{
	public:
		mod01_cntl();

	private:
		void CntlCallback(const senser_msgs::JointState::ConstPtr &wheel_state);
		void RotDirCntl();		

}



