#include "man_bt_operator/bt_manipulator.hpp"


namespace man_bt_operator
{

BT_Manipulator::BT_Manipulator(
  const ros::NodeHandle &private_node_handle,
  std::string bt_namespace_param,
  std::string robot_namespace_param):
  pnh_(private_node_handle),
  bt_namespace_param_(bt_namespace_param),
  robot_namespace_param_(robot_namespace_param)
{
  this->initialize();
}

void BT_Manipulator::initialize()
{
    if(!pnh_.getParam(bt_namespace_param_ + "/plugin_lib_names", plugin_lib_names_))
    {
      const std::vector<std::string> plugin_libs = {
      "man_update_param_service_client_node"
      };
      plugin_lib_names_ = plugin_libs;
    }

    if(!pnh_.getParam(bt_namespace_param_ + "/bt_xml_file_name", default_bt_xml_filename_))
    {
      ROS_ERROR("Please provide bt_xml_file");
      ros::shutdown();
    }

    if(!pnh_.getParam(robot_namespace_param_ + "/arm_group_name", group_name_arm_))
    {
      ROS_ERROR("Please provide arm_group_name");
      ros::shutdown();
    }
    
    if(!pnh_.getParam(robot_namespace_param_ + "/gripper_group_name", group_name_gripper_))
    {
      ROS_ERROR("Please provide gripper_group_name");
      ros::shutdown();
    }

    initialize_robot();

    // wrap bt_engine
    bt_ = std::make_unique<man_behavior_tree_nodes::BT_Engine>(plugin_lib_names_);

    // add items to blackboard
    // Create the blackboard that will be shared by all of the nodes in the tree
    blackboard_ = BT::Blackboard::create();

    // Put items on the blackboard
    blackboard_->set<std::map<std::string, float>>("param_float", param_float_);  // NOLINT
    blackboard_->set<std::map<std::string, int8_t>>("param_int", param_int_);
    blackboard_->set<std::map<std::string, std::string>>("param_float", param_string_);
    blackboard_->set<std::map<std::string, bool>>("param_float", param_bool_);

    blackboard_->set<std::string>("arm_group_name", group_name_arm_);
    blackboard_->set<std::string>("gripper_group_name", group_name_gripper_);
    blackboard_->set<ros::NodeHandle>("node_handle", pnh_);  // NOLINT
    blackboard_->set<moveit::core::RobotStatePtr>("kinematic_state", kinematic_state_);
    
    // load bt xml file
    if(!loadBehaviorTree(default_bt_xml_filename_))
    {
      ROS_ERROR("Error loading XML file: %s", default_bt_xml_filename_.c_str());
      ros::shutdown();

    }

    #ifdef ZMQ_FOUND
      BT::PublisherZMQ publisher_zmq(tree_);
    #endif

    // This logger prints state changes on console
    BT::StdCoutLogger logger_cout(tree_);
    printTreeRecursively(tree_.rootNode());  

    bt_->run(&tree_);
    
}

void BT_Manipulator::initialize_robot()
{
  // get infomation from robot
    robot_model_loader_ = std::make_unique<robot_model_loader::RobotModelLoader>("robot_description");
    kinematic_model_ = robot_model_loader_->getModel();
    base_frame_ = kinematic_model_->getModelFrame();
    ROS_INFO_NAMED("BT_Manipulator", "[BT_Manipulator] Model frame: %s", base_frame_.c_str());  

    kinematic_state_ = std::make_shared<moveit::core::RobotState>(kinematic_model_);
    
    // const moveit::core::JointModelGroup* joint_model_group = kinematic_model_->getJointModelGroup("manipulator");

    // const std::vector<std::string>& joint_names = joint_model_group->getVariableNames();

    // std::vector<double> joint_values;
    // kinematic_state_->copyJointGroupPositions(joint_model_group, joint_values);
    // for (std::size_t i = 0; i < joint_names.size(); ++i)
    // {
    //   ROS_INFO("Joint %s: %f", joint_names[i].c_str(), joint_values[i]);
    // }

}

BT_Manipulator::~BT_Manipulator()
{

}



bool BT_Manipulator::loadBehaviorTree(const std::string & bt_xml_filename)
{
  // Use previous BT if it is the existing one
  if (current_bt_xml_filename_ == bt_xml_filename) {
    return true;
  }

  // Read the input BT XML from the specified file into a string
  std::ifstream xml_file(bt_xml_filename);

  if (!xml_file.good()) {
    ROS_ERROR( "Couldn't open input XML file: %s", bt_xml_filename.c_str());
    return false;
  }

  auto xml_string = std::string(
    std::istreambuf_iterator<char>(xml_file),
    std::istreambuf_iterator<char>());

//   ROSINFO(get_logger(), "Behavior Tree file: '%s'", bt_xml_filename.c_str());
//   ROSINFO(get_logger(), "Behavior Tree XML: %s", xml_string.c_str());

  // Create the Behavior Tree from the XML input
  tree_ = bt_->buildTreeFromText(xml_string, blackboard_);

  current_bt_xml_filename_ = bt_xml_filename;

  return true;
}



} //namespace