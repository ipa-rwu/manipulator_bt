#include "ros/ros.h"
#include "manipulator_skills/skills/service_servers/update_arm_goal.hpp"
#include <manipulator_skills/skill_names.hpp>

int main(int argc, char **argv)
{
    ros::init(argc, argv, "update_arm_goal");
    ros::NodeHandle nh("");
    std::string group_name, service_name;
    std::string ns = ros::this_node::getName();
    ros::param::param<std::string>(ns + "/group_name", group_name, "manipulator");
    ros::param::param<std::string>(ns + "/service_name", service_name, manipulator_skills::UPDATE_ARM_GOAL);
    ros::AsyncSpinner spinner(1); 
    spinner.start();

    manipulator_skills::ArmUpdateGoal ArmUpdateStateSkill(service_name, group_name, nh);

    while (ros::ok())
    {

    }
    return 0;
}