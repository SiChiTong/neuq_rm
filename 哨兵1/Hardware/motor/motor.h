#ifndef MOTOR_H
#define MOTOR_H


//can stdid
#define CAN_LoopBack_ID				 0x200		//���ڻ���ģʽ�Լ����
#define	CAN_2006Motor1_ID  	   0x201
#define	CAN_2006Motor2_ID      0x202
#define	CAN_2006Motor3_ID      0x203
#define	CAN_2006Motor4_ID      0x204
#define CAN_TRIGGER_ID         0x205
#define CAN_GIMBAL1_ID         0x20A
#define CAN_GIMBAL2_ID         0x20B
typedef unsigned short     int uint16_t;
typedef   signed short     int int16_t;


//���ת��pid�����ṹ��
typedef struct{
	int err;
	int last_err;
	int err_integration;
	int target_speed;
	int actual_speed;
	
	int P_OUT;
	int I_OUT;
	int PID_OUT;
}VPID_t;

//�����е�ǶȲ���
typedef struct{
	
	float err;
	float last_err;
	float err_integration;
	float actual_angle;
	float target_angle;
	float P_OUT;
	float I_OUT;
	float D_OUT;
	float PID_OUT;
	float actual_speed;
	float target_speed;
}APID_t;
//��������ṹ��
typedef struct{
	
	int start_angle;			//�����ʼ�Ƕ�ֵ
	int start_angle_flag;	//��¼�����ʼ�Ƕ�ֵ��flag
	int stop_angle;				//����ֹͣ����ʱ��ĽǶ�ֵ
	
	float actual_angle;			//��ǰ��ʵ�Ƕ�ֵ
	int last_angle;				//��һ�η��صĽǶ�ֵ
	int round_cnt;				//��Կ���ʱת����Ȧ��
	int total_angle;			//�ܹ�ת���ļ���
	
	float actual_speed;			//�����ʵ�ٶ�,rpm
	int target_speed;			//���Ŀ���ٶ�,rpm  ת/min
	
	int actual_current;		//�����ʵ����
	int target_current;		//���Ŀ�����
	//int temp;							//����¶ȣ�2006�����֧�֣�3508֧�֣�
	VPID_t vpid;
	APID_t apid;
}MOTOR_t;

extern MOTOR_t motor1,motor2,motor3,motor4,motor5,motor6,gimbal1,gimbal2;//**********************************

extern int max_motor_speed;

//������������ṹ��
typedef struct{			
	
	int motor1_current;
	int motor2_current;
	int motor3_current;
	int motor4_current;
	int motor5_current;
	int motor6_current;
	int gimbal1_current;
	int gimbal2_current;
	
}LOOPBACK;

extern LOOPBACK loopback;
	

void record_motor_callback(MOTOR_t *motor, uint16_t angle, int16_t speed, int16_t current);
void motor_init(void);			//�����ʼ��
void set_chassis_current(void);	//���õ������
void set_trigger_current(void);
void set_gimbal_current(void);
void stop_chassis_motor(void);	//������Ƕȹ̶��ڵ�ǰֵ
void stop_trigger_motor(void);
void set_gimbal_current(void); //������̨���� 2020.7.24
void stop_gimbal_motor(void);

#endif



