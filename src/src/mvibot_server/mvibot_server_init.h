#include "../common/libary/libary_basic.h"
#include "../common/libary/libary_ros.h"
#include "../common/string_Iv2/string_Iv2.h"
#include "../common/send_tranfrom/send_tranfrom.h"

#include "mysql_connection.h"
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
using namespace std;
using namespace sql;
#if !defined(mvibot_server_define)
    // mysql var
    sql::mysql::MySQL_Driver *driver;
    Connection *con;
    sql::Statement *stmt;
    sql::ResultSet  *res;
    string user="mvibot",password="Mvibot@v1",host="tcp://127.0.0.1:3306",db="mvibot_database";
    // class table
    class colume_{
        public:
            string colume_name;
            string colume_type;
            int is_have=0;
    };
    class table_{
        public:
            string table_name;
            vector<colume_> table_colume;
            //
            void add_colume(string name, string type);
            void init_table();
    };
    void data_base_init();
    void table_init();
    // creat table
    table_ my_robot;
    table_ robot_status;
    table_ sensor_status;
    table_ battery_status;
    table_ battery_small_status;
    table_ battery_cell_status;
    table_ motor_right_status;
    table_ motor_left_status;
    table_ input_user_status;
    table_ output_user_status;
    table_ layer_emulator;
    table_ map_active;
    table_ robot_config_status;
    table_ my_module_gpio_v2;
    table_ my_robot_backup_mission;
    //
    class node_v2_3{
        public:
            string name_seri;
            // new for module v2.3
            int update_mision;
            string mission_normal;
            vector<std::string> history;
            ros::NodeHandle n;
            ros::Subscriber sub,sub2,sub3;
            ros::Publisher pub;
            void get_memory_missionf(const std_msgs::String & msg){
                if(msg.data=="1"){
                    std_msgs::String data;
                    data.data=mission_normal;
                    pub.publish(data);
                }
            }
            void mission_normalf(const std_msgs::String & msg){
                mission_normal=msg.data;
                update_mision=1;
            }       
            void historyf(const std_msgs::String & msg){
                history.resize(history.size()+1);
                history[history.size()-1]=msg.data;
                //history=msg.data;
            }
            void init(){
                sub =   n.subscribe("/"+name_seri+"/get_memory_mission", 1,&node_v2_3::get_memory_missionf,this);
                sub2 =  n.subscribe("/"+name_seri+"/mission_normal", 1,&node_v2_3::mission_normalf,this);
                sub3 =  n.subscribe("/"+name_seri+"/history", 100,&node_v2_3::historyf,this);
                pub =   n.advertise<std_msgs::String>("/"+this->name_seri+"/mission_normal",1);
            }
    };
    class robot_information{
        public:
            //
            int update_database;
            int id;
            float time_out=0;
            //
            string name_seri;
            string type;
            //
            string sensor_status;
            string robot_status;
            string battery_status;
            string battery_cell_status;
            string input_user_status;
            string output_user_status;
            string motor_left_status;
            string motor_right_status;
            string robot_config_status;
            string battery_small_status;
            //
            std::vector<string>     cmd_update_database();
            std::vector<string>     cmd_insert_database();
            string                  get_cmd_update_database(string data1_,string data2_);
            void                    update_status_robot(int n);
            void                    send_cmd_to_msyql(string cmd);
            node_v2_3 * node;
    };
    vector<robot_information> my_robots;
    float ts_my_robots;
    //
    class custom_layerv2{
        public:
            string name_layer;
            string type_layer;
            string name_map_active;
            float heigth,width;
            float xo,yo,yawo;
            int is_have=0;
            int is_pub=0;
    };
    vector<custom_layerv2> my_layers;
    void layer_process();
    int layer_change=1;
    // map var
    nav_msgs::OccupancyGrid my_map;
    nav_msgs::OccupancyGrid map_selector;
    string name_map_active="";
    int request_map_topic=0;
    //
    void free_res();
    void database_process();
    void get_robots_frist();
    // moudle gpio v2
    class node{
        public:
            std::string mission_normal;
            std::string output_user_set_string;
            std::string output_user_set_string2;
            std::string output_user_status_string;
            std::string input_user_status_string;
            std::string name_node;
            std::string history;
            //
            ros::NodeHandle n,n2,n3;
            ros::Subscriber sub,sub2,sub3;
            ros::Publisher pub;
            void output_user_set(const std_msgs::String & msg){
                // cout<<msg.data<<endl;
                // cout<<name_node<<endl;
                output_user_set_string2=name_node+"||"+msg.data;
                cout<<output_user_set_string2<<endl;
            }
            void mission_normalf(const std_msgs::String & msg){
                if(msg.data=="") mission_normal="RESET";
                else mission_normal=msg.data;
                //cout<<mission_normal<<endl;
            }
            void pub_test_pub(){
                static float creat_fun=0;
                static std_msgs::String msg;
                if(creat_fun==1){
                        msg.data=name_node;
                        pub.publish(msg);
                }else creat_fun=1;
            }
            void init(){
                sub  = n.subscribe("/"+name_node+"/output_user_set", 100,&node::output_user_set,this);
                sub2 = n.subscribe("/"+name_node+"/mission_normal", 100,&node::mission_normalf,this);
                pub  = n.advertise<std_msgs::String>("/"+this->name_node+"/test_pub",100);
            }
    };
    class module_gpio_v2_information{
        public:
            string name_seri;
            int    status;
            int    update_database;
            node*  my_node;
    };
    vector<module_gpio_v2_information> my_module_gpio_v2s;
    void get_module_gpio_v2_list();
    void update_module_gpio_v2();
    void pub_output_user_status_string(string data);
    void pub_input_user_status_string(string data);
    void pub_output_user_set_string(string data);
    #define mvibot_server_define 1
#endif
