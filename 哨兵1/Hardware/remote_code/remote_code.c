#include "remote_code.h"
#include "DJi_remote.h"
#include "FS_remote.h"
#include "speed_pid.h"
#include "motor.h"
#include "kinematic.h"
#include "fric.h"
#include "stm32f4xx_tim.h"
#include "gimbal.h"
#include "imuReader.h"              
#include <math.h>
#include "delay.h"
u8 Control_Mode = control_mode;

//�ڲ�ȫ�ֱ������������
float x_speed=0,y_speed=0,r_speed=0,trigger_speed=0,theta=0;
extern float distance;
int flag = 0;
//�ڲ���������
float caculate_linear_speed(int width,int mid,int min,int max);
float caculate_rotational_speed(int width,int mid,int min,int max);
float caculate_gimbal_pitch_angle(int width,int mid,int min,int max);
float caculate_gimbal_yaw_angle(int width,int mid,int min,int max);
float yaw_max_angular(float yaw);
float x_max_acceleration_caculator(float acc);
float y_max_acceleration_caculator(float acc);
float z_max_acceleration_caculator(float acc);
float pwm_xunhang_pitch(void);
float pwm_xunhang_yaw(void);

int pwm_flag=0;
// ����: Remote_Control()
// ����: ң�ش��룬��ң���������Ӧ�������˾��嶯���ϣ����ڶ�ʱ���ﲻ�ϵ�ˢ
// ��������
// �������
void Remote_Control()    //���������Ͳ��ϵ��ж�ÿ��ͨ����ֵ�������������������Ӧ����
{	
	if(chassis_CH_width>remote_min_value && chassis_CH_width<remote_max_value)		//�������ң������
	{
		//��־λ��Ϊң��ģʽ
		Control_Mode &= remote_control;											//�޸�Control_Mode�ڶ�λΪ0
	}
	else
	{
		//��־λ��Ϊ�Զ�ģʽ
		Control_Mode |= auto_control;												//�޸�Control_Mode�ڶ�λΪ1
		
	}
	
	if(1)//((Control_Mode & auto_control) != auto_control)			//�������ģʽ�������Զ����ƣ���ң�ؿ���
	{
		
			if(chassis_CH_width==3)
			{
				x_speed=caculate_linear_speed(x_CH_width,x_initial_value,x_min_value,x_max_value);
			 
				switch(trigger_CH_width)
				{
					case  1:
		 pwm_pulse1=pwm_xunhang_pitch();
     pwm_pulse2=pwm_xunhang_yaw();
          fric1_on(1000);
				  fric2_on(1000);
          trigger_speed = 0;
					break;
					
					case 2:
					pwm_pulse2=ljy(distance);

					pwm_pulse1=1462;
          fric1_on(1800);
				  fric2_on(1800);
         static int count_1=1;	
					count_1++;
					if(count_1>100)
					{trigger_speed = 150;
					    count_1=1;                       }
					if(motor5.actual_speed<20&&motor5.actual_speed>-20)    						//��ת
					{ 
						static int count_=1;
					  count_++;
						trigger_speed =pow(-1,count_)*50;
						if(count_>100)
							count_=1;
					}

					 /*trigger_speed = 150;
				   fric1_on(1000);
				   fric2_on(1000);
					if(motor5.actual_speed<20&&motor5.actual_speed>-20)    						//��ת
					{ 
						static int count_=1;
					  count_++;
						trigger_speed =pow(-1,count_)*50;
						if(count_>100)
							count_=1;
					}*/
					break;
					
					case 3:
					pwm_pulse2=ljy(distance);
					pwm_pulse1=1462;
				  fric1_on(1000);
				  fric2_on(1000);
          trigger_speed = 0;
					break;
					
					default:
	      	break;
				}
				//pwm_pulse1=caculate_gimbal_pitch_angle(i_CH_width,i_initial_value,i_min_value,i_max_value);
				//pwm_pulse2=caculate_gimbal_yaw_angle(x_CH_width,x_initial_value,x_min_value,x_max_value);
			
			                      }
			if(gimbal_CH_width==1)
			{
				switch (trigger_CH_width)
				{
					case 1:
				  fric1_on(1800);
				  fric2_on(1800);
         static int count_1=1;	
					count_1++;
					if(count_1>100)
					{trigger_speed = 150;
					    count_1=1;                       }
					if(motor5.actual_speed<20&&motor5.actual_speed>-20)    						//��ת
					{ 
						static int count_=1;
					  count_++;
						trigger_speed =pow(-1,count_)*50;
						if(count_>100)
							count_=1;
					}
					break;
					case 2:
					trigger_speed = -50;
				  fric1_on(1000);
				  fric2_on(1000);
					if(motor5.actual_speed<20&&motor5.actual_speed>-20)    						//��ת
					{ 
						static int count_=1;
					  count_++;
						trigger_speed =pow(-1,count_)*50;
						if(count_>100)
							count_=1;
					}
					break;
					case 3:
					trigger_speed = 0;
				  fric1_on(1000);
				  fric2_on(1000);
					break;
					
					default:
	      	break;
				                  }
						
					
				pwm_pulse1=caculate_gimbal_pitch_angle(i_CH_width,i_initial_value,i_min_value,i_max_value);
				pwm_pulse2=caculate_gimbal_yaw_angle(x_CH_width,x_initial_value,x_min_value,x_max_value);
				       		}
			
			
		
		if(stop_CH_width==2)	//���ֹͣ����
		{ 	   
		 pwm_pulse1=pwm_xunhang_pitch();
     pwm_pulse2=pwm_xunhang_yaw();
 
			
		}
		if((Control_Mode&DJi_Remote_Control) == DJi_Remote_Control)
		{
			y_speed = y_speed;
			r_speed = -r_speed; //ȡ����ʹ��ʱ����תΪ����
		}
		else if((Control_Mode&FS_Remote_Control) == FS_Remote_Control)		//��ΪFS_Remote_Control = 0������ж�ʱ�������else if��
		{
			y_speed = -y_speed;
		}
		speed_control(x_speed,y_speed,r_speed);
		trigger_control(trigger_speed);
		TIM_SetCompare1(TIM1,pwm_pulse1);
		TIM_SetCompare2(TIM1,pwm_pulse2);
	
	}
	/*ax=x_max_acceleration_caculator(x_accelerationRead());
  ay=y_max_acceleration_caculator(y_accelerationRead());
  z_max_acceleration_caculator(z_accelerationRead());
  v_yaw=yaw_max_angular(yaw_angularRead());*/
}
/***********************************************���ٶȲ��Դ���******************************************/


/*float x_max_acceleration_caculator(float acc)
{
	static float x_acceleration=0;
	if(acc>x_acceleration);
	x_acceleration=acc;
	return x_acceleration;
}
float y_max_acceleration_caculator(float acc)
{
	if(acc>y_acceleration);
	y_acceleration=acc;
	return y_acceleration;
}
float z_max_acceleration_caculator(float acc)
{
	if(acc>z_acceleration);
	z_acceleration=acc;
	return z_acceleration;
}

float yaw_max_angular(float yaw)
{
if(yaw>imu_yaw_angular)
	imu_yaw_angular=yaw;
return imu_yaw_angular;
}
float x_max_speed_caculator(float x)
{
   static float x_last=0;
	 if(x>x_last)
		x_last=x;
	 return x_last;
}
float y_max_speed_caculator(float y)
{
    static float y_last=0;
		if(y>y_last)
		y_last=y;
		return y_last;
}
float z_max_speed_caculator(float z)
{
   static float z_last=0;
	 if(z>z_last)
		z_last=z;
    return z_last;
}
*/
// ����: caculate_speed()
// ����: ��ң����ҡ�����ӳ�䵽�����������ٶ���
// ������width��ͨ��ֵ 
//			 mid��ͨ���м�ֵ 
//			 min��ͨ�������Сֵ
//       max��ͨ��������ֵ
// �������Ӧ���ٶ�ֵ
//�ڲ��������û��������
 float pwm_xunhang_pitch()
 {
   static float cout=0;
	 static float pwm_pulse1=1500;
	 cout=cout+1.0f;
	 if(cout>=0&&cout<125)
	 {
		pwm_pulse1=pwm_pulse1-1; 
	 }
	 if(cout>=125&&cout<425)
	 {
	 pwm_pulse1=1375;
	 }
	 if(cout>=425&&cout<550)
	 {
	 pwm_pulse1=pwm_pulse1+1;
	 }
	 if(cout>=550&&cout<850)
	 {
	 pwm_pulse1=1500;
	 }
	 if(cout>=850&&cout<925)
	 {
	 pwm_pulse1=pwm_pulse1-1;
	 }
	 if(cout>=925)
	 {
	 cout=125;
	 }
	 return pwm_pulse1;
 }
 
 float pwm_xunhang_yaw()
 {
   static float cout=0;
	 static float pwm_pulse2=1640;
	 cout=cout+1.0f;
	 if(cout>=0&&cout<125)
	 {
	 pwm_pulse2=pwm_pulse2-1;
	 }
	 if(cout>=125&&cout<425)
	 {
	 pwm_pulse2=pwm_pulse2+1;
	 }
	 if(cout>=425&&cout<550)
	 {
	 pwm_pulse2=1765;
	 }
	 if(cout>=550&&cout<850)
	 {
	 pwm_pulse2=pwm_pulse2-1;
	 }
	 if(cout>=850&&cout>925)
	 {
	 pwm_pulse2=1515;
	 }
	 if(cout>=925)
	 {
	 cout=125;
	 }
return pwm_pulse2;
 }

static float caculate_linear_speed(int width,int mid,int min,int max)
{
  float speed=0;
  if(width>=(mid+2))		//�м���������
    speed=(1.0*(width-(mid+2))/(max-(mid+2))*max_base_linear_speed);
  else if(width<=(mid-2))
    speed=(1.0*(width-(mid-2))/((mid-2)-min)*max_base_linear_speed);
  else
    speed=0;
  return speed;                
}

/*static float caculate_rotational_speed(int width,int mid,int min,int max)
{
  float speed=0;
  if(width>=(mid+2))		//�м���������
    speed=(1.0*(width-(mid+2))/(max-(mid+2))*max_base_rotational_speed);
  else if(width<=(mid-2))
    speed=(1.0*(width-(mid-2))/((mid-2)-min)*max_base_rotational_speed);
  else
    speed=0;
  return speed;                
}
*/
static float caculate_gimbal_pitch_angle(int width,int mid,int min,int max)
{
	float pwm_pulse=1500;
		if(width>=(mid+2))
		pwm_pulse=(1500 - 1.0*(width-(mid+2))/(max-(mid+2))*210);
	else if(width<=(mid-2))
	  pwm_pulse=(1500 + 1.0*((mid-2)-width)/((mid-2)-min)*105);
	else
		pwm_pulse=1500;
	return pwm_pulse;
	/*
	float pwm_pulse=1500;
	if(width>=(mid+2))
  pwm_pulse=pwm_pulse-1;
	else if(width<=(mid-2))
*/
}

static float caculate_gimbal_yaw_angle(int width,int mid,int min,int max)
{
	float pwm_pulse=1640;
		if(width>=(mid+2))
		pwm_pulse=(1640 - 1.0*(width-(mid+2))/(max-(mid+2))*420);
	else if(width<=(mid-2))
	  pwm_pulse=(1640 + 1.0*((mid-2)-width)/((mid-2)-min)*420);
	else
		pwm_pulse=1640;
	return pwm_pulse;
	
}

float ljy(float dis)
{
  float sita;
	float pwm_pulse2=1640;
	sita=atan((85-dis)*0.333333f*0.01f);
	sita=sita*180*0.31831f;
	pwm_pulse2=1640-sita*2.777f;
	return pwm_pulse2;
}


