#ifndef MANIPULATOR_SKILLS_EXECUTE_TRAJECTORY_SKILL_
#define MANIPULATOR_SKILLS_EXECUTE_TRAJECTORY_SKILL_

#include "ros/ros.h"
#include "manipulator_skills/skill.hpp"
#include "manipulator_skills/webots_elements.hpp"

#include <actionlib/server/simple_action_server.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>


#include "man_msgs/ExecuteTrajectorySkillAction.h"

namespace manipulator_skills
{

typedef actionlib::SimpleActionServer<man_msgs:: ExecuteTrajectorySkillAction> ExecuteTrajectorySkillServer;

class ArmExecuteTrajectorySkill  : public ManipulatorSkill
{
public:
  ArmExecuteTrajectorySkill(std::string group_name);
  ~ArmExecuteTrajectorySkill();

  void initialize() override;

private:
  void executeCB(const man_msgs::ExecuteTrajectorySkillGoalConstPtr &goal);
  void TouchsensorCallback(const webots_ros::BoolStamped::ConstPtr& touchsensor_msg);



  // void preemptComputerPathCallback();
  // void setComputerPathState(MoveGroupState state);

  std::unique_ptr<ExecuteTrajectorySkillServer> as_;
  std::unique_ptr<moveit::planning_interface::MoveGroupInterface> move_group_;

  // std::unique_ptr<actionlib::SimpleActionClient<moveit_msgs::MoveGroupAction> > move_action_client_;


  std::string group_name_;
  std::string action_name_;

  man_msgs::ExecuteTrajectorySkillResult action_res_;

  std::string webotsRobotName_;
  bool result_touchsensor_{false};

  ros::Subscriber touch_sensor_sub_;

  std::string touch_sensor_topic_name_;

  
  // public ros node handle
  ros::NodeHandle nh_;
  // private ros node handle
  ros::NodeHandle pnh_;

};
} //namespace manipulator_skills

#endif //MANIPULATOR_SKILLS_EXECUTE_TRAJECTORY_GRIPPER_SKILL_