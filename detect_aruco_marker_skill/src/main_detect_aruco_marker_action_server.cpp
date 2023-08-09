// Copyright (c) 2023 Ruichao Wu
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

#include <memory>

#include "detect_aruco_marker_skill/detect_aruco_marker_action_server.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto action_name = "detect_aruco_marker";
  auto node_option = rclcpp::NodeOptions();
  auto node =
    std::make_shared<perception_skills::DetectArucoMarkerActionServer>(action_name, node_option);
  rclcpp::spin(node->get_node_base_interface());
  rclcpp::shutdown();

  return 0;
}