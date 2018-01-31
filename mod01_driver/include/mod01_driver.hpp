//#include <termios.h>
#include <string>

// iMCs01
#include "iMCs01_driver/urbtc.h"
#include "iMCs01_driver/urobotc.h"

#include <ros/ros.h>

using namespace std;

class Mod01Driver
{
	public:
		Mod01Driver(ros::NodeHandle &n);
		~Mod01Driver();

	private:
		
}