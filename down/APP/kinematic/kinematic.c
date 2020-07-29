/**
  ******************************************************************************
  * @file    Project/APP/kinematic.c 
  * @author  Zhuge Huayang
  * @version V1.0.0
  * @date    2-December-2019
  * @brief   底盘正逆运动学演算
  *          线速度单位： cm/s
  *          角速度单位： rad/s
	*          转速单位：   rpm
  ******************************************************************************
  * @attention
  ******************************************************************************
  */


#include "kinematic.h"
#include "motor.h"

#include "speed_pid.h"
#include "angle_pid.h"


Kinematics_t Kinematics;

float max_base_linear_speed=0;		//底盘最大线速度
float max_base_rotational_speed=0;		//底盘最大角速度

//逆运动学公式
//把想要得到的底盘速度转换为轮子的线速度
void BaseVel_To_WheelVel(float linear_x, float linear_y, float angular_z)
{
//	Kinematics.target_velocities.linear_x  = linear_x;
//	Kinematics.target_velocities.linear_y  = linear_y;
//	Kinematics.target_velocities.angular_z = angular_z;
	
	Kinematics.wheel1.target_speed.linear_vel = 1.04f*(linear_y - linear_x + angular_z*(half_width + half_length));
	Kinematics.wheel2.target_speed.linear_vel = linear_y + linear_x - angular_z*(half_width + half_length);
	Kinematics.wheel3.target_speed.linear_vel = linear_y - linear_x - angular_z*(half_width + half_length);
	Kinematics.wheel4.target_speed.linear_vel = linear_y + linear_x + angular_z*(half_width + half_length);
	
	Kinematics.wheel1.target_speed.rpm = Kinematics.wheel1.target_speed.linear_vel * VEL2RPM;
	Kinematics.wheel2.target_speed.rpm = Kinematics.wheel2.target_speed.linear_vel * VEL2RPM;
	Kinematics.wheel3.target_speed.rpm = Kinematics.wheel3.target_speed.linear_vel * VEL2RPM;
	Kinematics.wheel4.target_speed.rpm = Kinematics.wheel4.target_speed.linear_vel * VEL2RPM;
	
	motor1.target_speed = - (int)(Kinematics.wheel1.target_speed.rpm * M3508_REDUCTION_RATIO);
	motor2.target_speed =   (int)(Kinematics.wheel2.target_speed.rpm * M3508_REDUCTION_RATIO);
	motor3.target_speed =   (int)(Kinematics.wheel3.target_speed.rpm * M3508_REDUCTION_RATIO);
	motor4.target_speed = - (int)(Kinematics.wheel4.target_speed.rpm * M3508_REDUCTION_RATIO);
	
}

void trigger_to_motor(float trigger_angular)
{
     
	motor5.target_speed =(int)(trigger_angular*M2006_REDUCTION_RATIO);
	
}



//正运动学公式
//通过轮胎的实际转速计算底盘几何中心的三轴速度
void Get_Base_Velocities(void)
{
	//根据电机转速测算轮子转速
	Kinematics.wheel1.actual_speed.rpm = - motor1.actual_speed / M3508_REDUCTION_RATIO;
	Kinematics.wheel2.actual_speed.rpm =   motor2.actual_speed / M3508_REDUCTION_RATIO;
	Kinematics.wheel3.actual_speed.rpm =   motor3.actual_speed / M3508_REDUCTION_RATIO;
	Kinematics.wheel4.actual_speed.rpm = - motor4.actual_speed / M3508_REDUCTION_RATIO;
	//轮子转速转换为轮子线速度
	Kinematics.wheel1.actual_speed.linear_vel = Kinematics.wheel1.actual_speed.rpm * RPM2VEL;
	Kinematics.wheel2.actual_speed.linear_vel = Kinematics.wheel2.actual_speed.rpm * RPM2VEL;
	Kinematics.wheel3.actual_speed.linear_vel = Kinematics.wheel3.actual_speed.rpm * RPM2VEL;
	Kinematics.wheel4.actual_speed.linear_vel = Kinematics.wheel4.actual_speed.rpm * RPM2VEL;
	//轮子线速度转换为底盘中心三轴的速度
	Kinematics.actual_velocities.angular_z = ( Kinematics.wheel1.actual_speed.linear_vel - Kinematics.wheel2.actual_speed.linear_vel\
				- Kinematics.wheel3.actual_speed.linear_vel + Kinematics.wheel4.actual_speed.linear_vel)/(4.0f*(half_width + half_length));
	Kinematics.actual_velocities.linear_x  = (-Kinematics.wheel1.actual_speed.linear_vel + Kinematics.wheel2.actual_speed.linear_vel\
				- Kinematics.wheel3.actual_speed.linear_vel + Kinematics.wheel4.actual_speed.linear_vel)/(4.0f);
	Kinematics.actual_velocities.linear_y  = ( Kinematics.wheel1.actual_speed.linear_vel + Kinematics.wheel2.actual_speed.linear_vel\
				+ Kinematics.wheel3.actual_speed.linear_vel + Kinematics.wheel4.actual_speed.linear_vel)/(4.0f);
}



// 函数: speed_control()
// 描述: 将pid速度输出转换为电机速度，最终传递给速度pid
// 参数：三个方向的速度
// 输出：4个电机速度
// 注：电机1、4的默认旋转方向和车轮实际正方向相反，需要取反
int find_max(void);
int stop_flag_1=0;

void speed_control(float speed_x, float speed_y, float speed_r)
{
	int max;
	if(stop_flag_1 == 0 && speed_x == 0 && speed_y == 0 && speed_r == 0)
	{
		stop_flag_1 = 1;			//停止   此标志为了避免多次进入
		stop_chassis_motor();			//停下来  并角度闭环
	}
	else if(speed_x != 0 || speed_y != 0 || speed_r != 0)
	{
		stop_flag_1 = 0;
		//速度换算
		BaseVel_To_WheelVel(speed_x, speed_y, speed_r);
//		motor1.target_speed=-10.0f*(speed_y - speed_x + speed_r*30);
//		motor2.target_speed=10.0f*(speed_y + speed_x - speed_r*30);
//		motor3.target_speed=10.0f*(speed_y - speed_x - speed_r*30);
//		motor4.target_speed=-10.0f*(speed_y + speed_x + speed_r*30);
		//速度等比例压缩，用来防止多个速度均超速度上限而导致机器人难以调整姿态
		max=find_max();
		if(max>max_motor_speed)
		{
			motor1.target_speed=(int)(motor1.target_speed*max_motor_speed*1.0/max);
			motor2.target_speed=(int)(motor2.target_speed*max_motor_speed*1.0/max);
			motor3.target_speed=(int)(motor3.target_speed*max_motor_speed*1.0/max);
			motor4.target_speed=(int)(motor4.target_speed*max_motor_speed*1.0/max);
		}
			//改变速度pid目标速度
			set_chassis_motor_speed(motor1.target_speed, motor2.target_speed, motor3.target_speed, motor4.target_speed);
	}
}	

int stop_flag_2=0;

void trigger_control(float trigger_angular)
{
if(stop_flag_2 == 0 && trigger_angular==0)
	{
		stop_flag_2 = 1;			//停止   此标志为了避免多次进入
		stop_trigger_motor();			//停下来  并角度闭环
	}
else if(trigger_angular!=0)
	{
		stop_flag_2 = 0;
		
		trigger_to_motor(trigger_angular);
		
		set_trigger_motor_speed(motor5.target_speed);		
}
	}


int stop_flag_3=0;
	
void gimbal_control(float gimbal1_angle,float gimbal2_angle)
{
	//转换命令
	gimbal1_angle=gimbal1_angle*8191/360;
  gimbal2_angle=gimbal2_angle*8191/360;
	set_GIMBAL_angle(gimbal1_angle,gimbal2_angle);
}	
	
void break_jugement(void)
{
    if(motor1.actual_speed <=0.05)
		 stop_flag_3=1;
    
}
// 函数: find_max()
// 描述: 找到计算得到的电机速度最大值
// 参数：无
// 输出：计算而得的电机最大值
// 内部函数，用户无需调用
int find_max()
{
  int temp=0;
  
  temp=abs(motor1.target_speed);
  if(abs(motor2.target_speed)>temp)
    temp=abs(motor2.target_speed);
  if(abs(motor3.target_speed)>temp)
    temp=abs(motor3.target_speed);
  if(abs(motor4.target_speed)>temp)
    temp=abs(motor4.target_speed);
  return temp;
}






