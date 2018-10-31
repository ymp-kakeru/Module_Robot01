# Module_Robot01
## How to Drive and Get data
1. Before check
  * Emergency switch was pushed.
  * All USB removed.
2. Turn on PC and Robot
  * Select carnel ubuntu14.04.24.
  * USB connect.
  * Emergency switch turn off.

3. Works on Terminal (Cntl + ALR + T)  
	1st. Open Arduino and connect to it.  
    ```cpp
    $ arduino ~/catkin_ws/src/Module_Robot01/joy_motor/joy_motor_ar_ino/joy_motor_ar_ino.ino
    ```
    if cant open that, please use explorer. After connect PC to Arduino, you can find the port. (ex.ttyACM0)
    
    2nd.
	```cpp
    $ cd ~/catkin_ws/src/Module_Robot01/mod01_cntl
    $ subl launch/mod01_simple.launch
    ```
    


