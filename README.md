# Module_Robot01
## How to Drive and Get data
### 1. Before check
  * Emergency switch was pushed.
  * All USBs removed.
### 2. Turn on PC and Robot
  * Select carnel ubuntu14.04.24.
  * USBs connect.
  * Emergency switch turn off.

### 3. Preparation
1st. Launch Terminal
> Cntl+ALT+T
`$ ympterm`

2nd. Open Arduino and connect to it.
```cpp
$ arduino ~/catkin_ws/src/Module_Robot01/joy_motor/joy_motor_ar_ino/joy_motor_ar_ino.ino
```
>if cant open that, please use explorer.
After connect PC to Arduino, you can find the port. (ex.ttyACM0)

3rd. settings
Give permission to device.
```cpp
$ sudo chmod 777 /dev/input/js0
$ sudo chmod 777 /dev/ttyACM*
```
Open lanuch file, set the port of Arduino. You have already found the port (ttyACM*).
```cpp
$ cd ~/catkin_ws/src/Module_Robot01/mod01_cntl
$ subl launch/mod01_simple.launch
```
4th. Launch
Launch simple experiment program. You have to choose sampling rate and filename.
After send this command, the Robot can move with ds3 controller.
It can move straight with L1 button. If without it, can move only turn (rotate).
```cpp
$ roslaunch mod01_cntl mod01_simple.launch samplingehz=0.5 filename=hoge.csv
```
The saved datas are  `/home/catkin_ws/Module_Robot01/DataBox/hoge.csv` .

### 4. After
Turn off Robot, remove battery and all USBs.

##How to convert Point Cloud2 (hokuyo3d) to PCD data.
use this command on your terminal.
```cpp
$ mkdir ~/catkin_ws/Module_Robot01/DataBox/<output_directory>
$ rosrun pcl_ros bag_to_pcd <input_file.bag> /hokuyo3d/hokuyo_cloud2 <output_directory>
```