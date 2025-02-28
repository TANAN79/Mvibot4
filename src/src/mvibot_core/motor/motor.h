#include"../../common/libary/libary_basic.h"
#include"../../common/libary/libary_ros.h"
//
using namespace std;
//
extern string mvibot_seri;
extern int motor_enable;
extern int motor_break;
extern int motor_reset;
extern int motor_right_disable,motor_left_disable;
extern int motor_right_break,motor_left_break;
extern int motor_right_state_live,motor_left_state_live;
extern int motor_right_state_error,motor_left_state_error;
extern float ivr,ivl,vr_out,vl_out;
extern float kpvrl,kivrl,kdvrl;
extern float vr,vl;
extern float tr,tl;
extern float vr_set,vl_set;
extern float torqueL_set,torqueR_set;
extern float vrl_max;
extern float ts_pid;
extern int motor_stop;
extern int robot_emg;
extern float v_set1,v_set2,v_set3,w_set1,w_set2,w_set3;
extern float ts_speed_control;
extern float time_out_cmd_vel;
extern void  send_history(string type,string data);
//
void pub_motor_right_status(){
    static ros::NodeHandle n,n1;
    static ros::Publisher pub_motor_status = n.advertise<std_msgs::String>("/"+mvibot_seri+"/motor_right_status",1);
    static ros::Publisher pub_motor_status_string = n1.advertise<std_msgs::String>("/motor_right_status_string",1);
    static float creat_fun=0;
    static std_msgs::String msg;
        if(creat_fun==1){ 
                msg.data=mvibot_seri+"|";
                msg.data=msg.data+"live:"+to_string((int)motor_right_state_live)+"|";
                msg.data=msg.data+"error:"+to_string((int)motor_right_state_error)+"|";
                msg.data=msg.data+"enable:"+to_string((int)motor_right_disable)+"|";
                msg.data=msg.data+"brake:"+to_string((int)motor_right_break);
                pub_motor_status.publish(msg);
                pub_motor_status_string.publish(msg);
                //
                static int live=-1,live_f=-1,brake=-1,brake_f=-1,enable=-1,enable_f=-1,error=-1,error_f=-1;
                live=motor_right_state_live;
                error=motor_right_state_error;
                enable=motor_right_disable;
                brake=motor_right_break;
                //
                if(live_f==-1 | (live!=live_f)){
                    if(live==0)  send_history("warning","Motor right turn off");
                    else send_history("warning","Motor right turn on");
                    live_f=live;
                }else{
                    if(live==1){
                        if(error_f!=error){
                            error_f=error;
                            if(error!=0) send_history("error","Motor right error id:"+to_string(error));
                            else send_history("normal","Motor right no error");
                        }
                        if(enable_f!=enable){
                            enable_f=enable;
                            if(enable!=0) send_history("normal","Motor right change to mode automation control");
                            else send_history("normal","Motor right change to mode manual handle");
                        }
                        if(brake_f!=brake){
                            brake_f=brake;
                            if(brake!=0) send_history("normal","Motor right disable brake");
                            else send_history("normal","Motor right  enable brake");
                        }
                    }
                }
        }else creat_fun=1;
}
void pub_motor_left_status(){
    static ros::NodeHandle n,n1;
    static ros::Publisher pub_motor_status = n.advertise<std_msgs::String>("/"+mvibot_seri+"/motor_left_status",1);
    static ros::Publisher pub_motor_status_string = n1.advertise<std_msgs::String>("/motor_left_status_string",1);
    static float creat_fun=0;
    static std_msgs::String msg;
        if(creat_fun==1){ 
                msg.data=mvibot_seri+"|";
                msg.data=msg.data+"live:"+to_string((int)motor_left_state_live)+"|";
                msg.data=msg.data+"error:"+to_string((int)motor_left_state_error)+"|";
                msg.data=msg.data+"enable:"+to_string((int)motor_left_disable)+"|";
                msg.data=msg.data+"brake:"+to_string((int)motor_left_break);
                pub_motor_status.publish(msg);
                pub_motor_status_string.publish(msg);
                //
                static int live=-1,live_f=-1,brake=-1,brake_f=-1,enable=-1,enable_f=-1,error=-1,error_f=-1;
                live=motor_left_state_live;
                error=motor_left_state_error;
                enable=motor_left_disable;
                brake=motor_left_break;
                if(live_f==-1 | (live!=live_f)){
                    if(live==0)  send_history("warning","Motor left turn off");
                    else send_history("warning","Motor left turn on");
                    live_f=live;
                }else{
                    if(live==1){
                        if(error_f!=error){
                            error_f=error;
                            if(error!=0) send_history("error","Motor left error id:"+to_string(error));
                            else send_history("normal","Motor left no error");
                        }
                        if(enable_f!=enable){
                            enable_f=enable;
                            if(enable!=0) send_history("normal","Motor left change to mode automation control");
                            else send_history("normal","Motor left change to mode manual handle");
                        }
                        if(brake_f!=brake){
                            brake_f=brake;
                            if(brake!=0) send_history("normal","Motor left disable brake");
                            else send_history("normal","Motor left  enable brake");
                        }
                    }
                }
                //
        }else creat_fun=1;
}
void pid_motor(){
    // caculate for speed motor
    vl_set=(2*v_set3-w_set3*L)/(2*R);
    vr_set=(2*v_set3+w_set3*L)/(2*R);
    // pid motor 
    ivr=ivr+(vr_set-vr)*(float)ts_pid;
    //
    if(fabs(ivr)*kivrl>vrl_max) {
        if(ivr>0) ivr=vrl_max*kivrl;
        if(ivr<0) ivr=-vrl_max*kivrl;
    }
    //
    vr_out=kpvrl*(vr_set-vr)+kivrl*ivr;	
    if(fabs(vr_out)>vrl_max) vr_out=fabs(vr_out)/vr_out*vrl_max;
    if(vr_set==0 & fabs(vr)<=M_PI/(2*2.5)) {
        vr_out=0;
        ivr=0;
    } 
    ivl=ivl+(vl_set-vl)*(float)ts_pid;
    //
    if(fabs(ivl)*kivrl>vrl_max){
        if(ivl>0) ivl=vrl_max*kivrl;
        if(ivl<0) ivl=-vrl_max*kivrl;
    }
    //
    vl_out=kpvrl*(vl_set-vl)+kivrl*ivl;
    if(fabs(vl_out)>vrl_max) vl_out=fabs(vl_out)/vl_out*vrl_max;
    if(vl_set==0 & fabs(vl) <= M_PI/(2*2.5)) {
        vl_out=0;
        ivl=0;
    }
    // robot emg
    if(robot_emg==1){
        vr_out=0;
        ivr=0;
        vl_out=0;
        ivl=0;
        robot_emg=0;
    }
    //
}
void torque_control(){
}
void reset_pid_motor(){
        vr_set=0;
        vl_set=0;
        vl_out=0;
		vr_out=0; 
		ivl=0;
		ivr=0;
}