//  Copy and did modification from origin code

// Copyright (c) 2018 Intel Corporation
// Copyright (c) 2020 Florian Gramss
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "man2_behavior_tree/behavior_tree_engine.hpp"

#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"

namespace ros2_behavior_tree
{
static const rclcpp::Logger LOGGER = rclcpp::get_logger("ros2_behavior_tree_engine");

ROS2BehaviorTreeEngine::ROS2BehaviorTreeEngine(const std::vector<std::string>& plugin_libraries)
{
  BT::SharedLibrary loader;
  for (const auto& p : plugin_libraries)
  {
    factory_.registerFromPlugin(loader.getOSName(p));
  }
}

ros2_behavior_tree::BtStatus ROS2BehaviorTreeEngine::run_loop(BT::Tree* tree,
                                                              std::function<void()> onLoop,
                                                              std::function<bool()> cancelRequested,
                                                              std::chrono::milliseconds loopTimeout)
{
  rclcpp::WallRate loopRate(loopTimeout);
  BT::NodeStatus result = BT::NodeStatus::RUNNING;

  // Loop until something happens with ROS or the node completes
  try
  {
    while (rclcpp::ok() && (tree->rootNode()->status() != BT::NodeStatus::SUCCESS ||
                            tree->rootNode()->status() != BT::NodeStatus::FAILURE))
    {
      if (cancelRequested())
      {
        tree->haltTree();
        return ros2_behavior_tree::BtStatus::CANCELED;
      }

      tree->rootNode()->executeTick();

      onLoop();

      loopRate.sleep();
    }
  }
  catch (const std::exception& ex)
  {
    RCLCPP_ERROR(LOGGER, "Behavior tree threw exception: %s. Exiting with failure.", ex.what());
    return ros2_behavior_tree::BtStatus::FAILED;
  }

  return (result == BT::NodeStatus::SUCCESS) ? ros2_behavior_tree::BtStatus::SUCCEEDED :
                                               ros2_behavior_tree::BtStatus::FAILED;
}

BT::Tree ROS2BehaviorTreeEngine::createTreeFromText(const std::string& xml_string,
                                                    BT::Blackboard::Ptr blackboard)
{
  return factory_.createTreeFromText(xml_string, blackboard);
}

BT::Tree ROS2BehaviorTreeEngine::createTreeFromFile(const std::string& file_path,
                                                    BT::Blackboard::Ptr blackboard)
{
  return factory_.createTreeFromFile(file_path, blackboard);
}

}  // namespace ros2_behavior_tree
