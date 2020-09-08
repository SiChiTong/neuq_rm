#!/usr/bin/env python

from __future__ import print_function

import roslib; roslib.load_manifest('teleop_twist_keyboard')
import rospy

from geometry_msgs.msg import Twist
from std_msgs.msg import String
#from pynput import keyboard
import string

import sys, select, termios, tty

msg = """
Reading from the keyboard  and Publishing to Twist!
---------------------------
Moving around:
   u    i    o
   j    k    l
   m    ,    .
For Holonomic mode (strafing), hold down the shift key:
---------------------------
   U    I    O
   J    K    L
   M    <    >
t : up (+z)
b : down (-z)
anything else : stop
q/z : increase/decrease max speeds by 10%
w/x : increase/decrease only linear speed by 10%
e/c : increase/decrease only angular speed by 10%
CTRL-C to quit
"""

moveBindings = {
        'i':(1,0,0,0),
        'o':(1,0,0,-1),
        'j':(0,0,0,1),
        'l':(0,0,0,-1),
        'u':(1,0,0,1),
        ',':(-1,0,0,0),
        '.':(-1,0,0,1),
        'm':(-1,0,0,-1),
        'O':(1,-1,0,0),
        'I':(1,0,0,0),
        'J':(0,1,0,0),
        'L':(0,-1,0,0),
        'U':(1,1,0,0),
        '<':(-1,0,0,0),
        '>':(-1,-1,0,0),
        'M':(-1,1,0,0),
        't':(0,0,1,0),
        'b':(0,0,-1,0),
 	


    }

speedBindings={
        'q':(1.1,1.1),
        'z':(.9,.9),
        'w':(1.1,1),
        'x':(.9,1),
        'e':(1,1.1),
        'c':(1,.9),
    }



schemaTranslation={
        #'1':"chassis_normal",
        #'2':"chassis_dodge",
       # '3':"chassis_motion",
       # '4':"chassis_follow",
        #'5':"gimbal_auto_aim",
        '6':"load",
        '7':"gimbal_turn",
        '8':"gimbal_side",
        '9':"gimbal_hand",
        #'0':"gimbal_shand",
        #'-':"gimbal_auto_fire",
        'f':"fire",
	'g':"stop_fire",
	'up':"up",
        'down':"down",
	'left':"left",
	'right':"right",
      
}


def getKey():
    tty.setraw(sys.stdin.fileno())
    select.select([sys.stdin], [], [], 0)
    key = sys.stdin.read(1)
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    if key == chr(58):
        key = 'up'
    if key == chr(62):
        key = 'down'
    if key == chr(60):
        key = 'left'
    if key == chr(63):
        key = 'right'
    return key


def vels(speed,turn):
    return "currently:\tspeed %s\tturn %s " % (speed,turn)

#def info(translation):
    #return "currently:\ttranslation %s " %(translation)

if __name__=="__main__":
    settings = termios.tcgetattr(sys.stdin)

    twistpub = rospy.Publisher('teleop_twist', Twist, queue_size = 1)
    translationpub = rospy.Publisher('schemaTranslation', String, queue_size = 1)
    rospy.init_node('teleop_keyboard')

    speed = rospy.get_param("~speed", 0.5)
    turn = rospy.get_param("~turn", 1.0)
    x = 0
    y = 0
    z = 0
    th = 0
    status = 0

    try:
        # print(msg)
        print(vels(speed,turn))
        while(1):
            key = getKey()
            if key in moveBindings.keys():
                x = moveBindings[key][0]
                y = moveBindings[key][1]
                z = moveBindings[key][2]
                th = moveBindings[key][3]
                print(vels(speed,turn))
                twist = Twist()
                twist.linear.x = x*speed; twist.linear.y = y*speed; twist.linear.z = z*speed
                twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = th*turn
                twistpub.publish(twist)
            elif key in speedBindings.keys():
                speed = speed * speedBindings[key][0]
                turn = turn * speedBindings[key][1]
                print(vels(speed,turn))		  
                twist = Twist()
                twist.linear.x = x*speed; twist.linear.y = y*speed; twist.linear.z = z*speed
                twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = th*turn
                twistpub.publish(twist)

            elif key in schemaTranslation.keys():
                translation = schemaTranslation[key]
                string = String()
                string =translation
                rospy.loginfo("%s",translation)
                translationpub.publish(string)
                
            else:
                        x=0
                        y=0
                        z=0
                        th =0
                        twist = Twist()
                        twist.linear.x = x*speed; twist.linear.y = y*speed; twist.linear.z = z*speed
                        twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = th*turn
                        twistpub.publish(twist)
                        

            if (key == '\x03'):
                break

	  
            

    except Exception as e:
        print(e)

    finally:
        twist = Twist()
        twist.linear.x = 0; twist.linear.y = 0; twist.linear.z = 0
        twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = 0
        twistpub.publish(twist)

        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)



 #else:
                #x = 0
                #y = 0
                #z = 0
                #th = 0

                #if (key == '\x03'):
                   # break
#twist = Twist()
		#    twist.linear.x = x*speed; twist.linear.y = y*speed; twist.linear.z = z*speed
		  #  twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = th*turn
		  #  twistpub.publish(twist)