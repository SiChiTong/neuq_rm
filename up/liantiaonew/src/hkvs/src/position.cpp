#include <ros/ros.h> 
#include<iostream>
#include"std_msgs/String.h"
#include<geometry_msgs/Point.h>
#include<position/square.h>
#include <opencv2/opencv.hpp>
#include "cv.h"
#include <pthread.h>
#include <unistd.h>
#include<vector>
#include<algorithm>
#include <cv_bridge/cv_bridge.h>
#include "math.h"
#include <dynamic_reconfigure/server.h>
#include <position/positionConfig.h>
using namespace cv;
using namespace std;

double new_x,new_y,new_z;
#define N 10 
  double a[N]={0};
  double b[N]={0};
  double c[N]={0};

/*void codeRotateByZ(double x, double y, double thetaz, double& outx, double& outy)
{
    double x1 = x;//将变量拷贝一次，保证&x == &outx这种情况下也能计算正确
    double y1 = y;
    double rz = thetaz * CV_PI / 180;
    outx = cos(rz) * x1 - sin(rz) * y1;
    outy = sin(rz) * x1 + cos(rz) * y1;
}

void codeRotateByY(double x, double z, double thetay, double& outx, double& outz)
{
    double x1 = x;
    double z1 = z;
    double ry = thetay * CV_PI / 180;
    outx = cos(ry) * x1 + sin(ry) * z1;
    outz = cos(ry) * z1 - sin(ry) * x1;
}

void codeRotateByX(double y, double z, double thetax, double& outy, double& outz)
{
    double y1 = y;//将变量拷贝一次，保证&y == &y这种情况下也能计算正确
    double z1 = z;
    double rx = thetax * CV_PI / 180;
    outy = cos(rx) * y1 - sin(rx) * z1;
    outz = cos(rx) * z1 + sin(rx) * y1;
}*/
void tf(double x, double z, double thetaA, double& outx, double& outz)
{
    double x1 = x;
    double z1 = z;
    double ry = thetaA * CV_PI / 180;
    outx = cos(ry) * x1 + sin(ry) * z1;
    outz = cos(ry) * z1 - sin(ry) * x1; 
}
    class SubscribeAndPublish  
    {  
    public:  

      SubscribeAndPublish()  
      {   
        point3dpub = n_.advertise<geometry_msgs::Point>("point3d", 1);  
        point3dpub1 = n_.advertise<geometry_msgs::Point>("point3d1", 1);  
        point2fsub = n_.subscribe("point2d", 1, &SubscribeAndPublish::callback1, this);
      }  
    /*服务请求该参数1  void initialize()
      {
       this->f = boost::bind(&SubscribeAndPublish::callback,this, _1, _2);
        this->server.setCallback(f);
        cout<<"initalize"<<endl;
      }*/

    /*  void callback(position::positionConfig &config, uint32_t level) {
        this->P[1].x=config.pointzs_x;
        this->P[1].y=config.pointzs_y;
        this->P[2].x=config.pointys_x;
        this->P[2].y=config.pointys_y;
        this->P[3].x=config.pointyx_x;
        this->P[3].y=config.pointyx_y;
        this->P[4].x=config.pointzx_x;
        this->P[4].y=config.pointzx_y;
        this->fx=config.fx;
        this->cx=config.cx;
        this->fy=config.fy;
        this->cy=config.cy;
        this->k1=config.k1;
        this->k2=config.k2;
        this->k3=config.k3;
        this->p1=config.p1;
        this->p2=config.p2;



}*/
    double Calculate_3d()
    {
       position::square squared;
        vector<cv::Point3f> Points3D;
            Points3D.push_back(cv::Point3f(-70, 30, 0));      //P2
        Points3D.push_back(cv::Point3f(70, 30, 0));      //P3
        Points3D.push_back(cv::Point3f(70, -30, 0));
         Points3D.push_back(cv::Point3f(-70,-30, 0));   //P1 三维坐标的单位是毫米
      
	  vector<cv::Point2f> Points2D;
        Point2f circle;
           //circle.x=msg->o_y;
        Points2D.push_back(P[1]);        //P1 单位是像素
        Points2D.push_back(P[2]);        //P2
        Points2D.push_back(P[3]);        //P3
        Points2D.push_back(P[4]);        //P4

          cv::Mat rvec = cv::Mat::zeros(3, 1, CV_64FC1);
          cv::Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);

 	 Mat camera_matrix;
              Mat distortion_coefficients;
camera_matrix = (Mat_<double>(3,3)<<2456.128,0,716.977,0,2462.651,554.82,0,0,1);
distortion_coefficients = (Mat_<double>(5,1)<<-0.078,0.67599,0,0,0);
//camera_matrix = (Mat_<double>(3,3)<<2366.5,0,742.4,0,2365.2,558.7,0,0,1);
//distortion_coefficients = (Mat_<double>(5,1)<<-0.1056,0.7207,0,0,0);

//camera_matrix = (Mat_<double>(3,3)<<fx,0,cx,0,fy,cy,0,0,1);
//distortion_coefficients = (Mat_<double>(5,1)<<k1,k2,p1,p2,k3);
           /* 

              FileStorage fs("/home/lucy/abc/src/position/out_camera_data.xml", FileStorage::READ);
              fs["camera_matrix"] >> camera_matrix;
              fs["distortion_coefficients"] >>  distortion_coefficients;
*/
          solvePnP(Points3D, Points2D, camera_matrix, distortion_coefficients, rvec, tvec,false, CV_ITERATIVE); 

  //        cout <<"tvec: "<<tvec<<endl;

 	 double *data1;
	data1 = tvec.ptr<double>(0);//data1为xyz

	double rm[9];
	cv::Mat rotM(3, 3, CV_64FC1, rm);
	Rodrigues(rvec, rotM);//旋转向量转矩阵

	double x,y,z;
	x=data1[0];
	y=data1[1];
	z=data1[2];

cout<<"x= "<<x<<"y="<<y<<"z= "<<z<<endl;

  a[0]=x;b[0]=y;c[0]=z;
double sum1,sum2,sum3;
sum1=sum2=sum3=0;

for(int i=N-1;i>0;i--)
  {
    a[i]=a[i-1];
    b[i]=b[i-1];
    c[i]=c[i-1];
  }
cout<<c[1]<<endl;
int t;
for(int i=0;i<N-1;i++)
{
	for(int j=0;j<N-i-1;j++)
	{
	if(a[j]>a[j+1])
		{
		t=a[j+1];
		a[j+1]=a[j];
		a[j]=t;
		}
	}
}
for(int i=0;i<N-1;i++)
{
	for(int j=0;j<N-i-1;j++)
	{
	if(b[j]>b[j+1])
		{
		t=b[j+1];
		b[j+1]=b[j];
		b[j]=t;
		}
	}
}
for(int i=0;i<N-1;i++)
{
	for(int j=0;j<N-i-1;j++)
	{
	if(c[j]>c[j+1])
		{
		t=c[j+1];
		c[j+1]=c[j];
		c[j]=t;
		}
	}
}
for(int i=0;i<N;i++)
{
cout<<c[i]<<endl;
}
for (int count=1;count<=(N-2);count++)
{
      sum1 += a[count];
      sum2 += b[count];
      sum3 += c[count];
}

  new_x=sum1/(N-2);
  new_y=sum2/(N-2);
  new_z=sum3/(N-2);

	double *data2;
	data2 = rotM.ptr<double>(0);//data2为旋转向量

	double r11,r12,r13,r21,r22,r23,r31,r32,r33;
	r11=data2[0];r12=data2[1];r13=data2[2];
	r21=data2[3];r22=data2[4];r23=data2[5];
	r31=data2[6];r32=data2[7];r33=data2[8];

	const double PI = 3.141592653;

/*	double thetaA = 30;
	double x1,y1,z1;
 	y1 = y;
	tf(x,z,thetaA, x1,z1);
*/
        geometry_msgs::Point point;
        geometry_msgs::Point point1;
         point.x= new_x;
         point.y= new_y;
         point.z= new_z;
	point1.x= x;
         point1.y= y;
         point1.z= z;
         point3dpub.publish(point); 
         point3dpub1.publish(point1); 
        ROS_INFO("i have send x%f y%f z%f", point.x,point.y,point.z);

    }

      void callback1(const position::square::ConstPtr& msg)  
      {  
        P[1].x=msg->zs_x;
        P[1].y=msg->zs_y;
        P[2].x=msg->ys_x;
        P[2].y=msg->ys_y;
        P[3].x=msg->yx_x;
        P[3].y=msg->yx_y;
        P[4].x=msg->zx_x;
        P[4].y=msg->zx_y;
        //circle.x=msg->o_x;

    /*    cout<<P[1].x<<".."<<P[1].y<<endl;
cout<<P[2].x<<".."<<P[2].y<<endl;
cout<<P[3].x<<".."<<P[3].y<<endl;
cout<<P[4].x<<".."<<P[4].y<<endl;*/
      }  
    private:  
      ros::NodeHandle n_;   
      ros::Publisher point3dpub; 
      ros::Publisher point3dpub1;   
      ros::Subscriber point2fsub;
      Point2f P[5];
      //1dynamic_reconfigure::Server<position::positionConfig> server;
      //1dynamic_reconfigure::Server<position::positionConfig>::CallbackType f;
      double fx,fy,cx,cy,k1,k2,k3,p1,p2;
      
    };
      
    int main(int argc, char **argv)  
    {   
      ros::init(argc, argv, "position"); 
      ros::NodeHandle n_;
      ros::Rate loop_rate(10);
      SubscribeAndPublish point3dpub;
      SubscribeAndPublish point3dpub1;
      //1point3dpub.initialize();
     while(ros::ok())  
     {
      

      point3dpub.Calculate_3d();

        //int x,y,z;
          
        //ros::Publisher point3dpub; 
        //geometry_msgs::Point point;
        //point3dpub = n_.advertise<geometry_msgs::Point>("point3d", 1);
       // int i=0;
     
       /*  for(i=0;i<12;i++)
      { point.x= i+1;
        point.y= 0.5*i+1;
        point. z= 4;
        loop_rate.sleep();
        point3dpub.publish(point); 
        ROS_INFO("i have send x%f y%f z%f", point.x,point.y,point.z);}*/
      ros::spinOnce();
      loop_rate.sleep();
     }
      return 0;  
    }  
