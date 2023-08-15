# Copyright (c) 2023 Ruichao Wu
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    SetEnvironmentVariable,
)
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder


def generate_launch_description():
    # Get the launch directory
    bringup_dir = get_package_share_directory("moveit_skills")

    # Create the launch configuration variables
    use_sim_time = LaunchConfiguration("use_sim_time")
    params_file = LaunchConfiguration("params_file")

    stdout_linebuf_envvar = SetEnvironmentVariable("RCUTILS_LOGGING_BUFFERED_STREAM", "1")

    declare_use_sim_time_cmd = DeclareLaunchArgument(
        "use_sim_time",
        default_value="false",
        description="Use simulation (Gazebo) clock if true",
    )

    declare_params_file_cmd = DeclareLaunchArgument(
        "params_file",
        default_value=os.path.join(bringup_dir, "config", "params.yaml"),
        description="Full path to the ROS2 parameters file to use for all launched nodes",
    )

    moveit_config = (
        MoveItConfigsBuilder("ur5e_workcell", package_name="ur5e_cell_moveit_config")
        .robot_description(file_path="config/ur5e_workcell.urdf.xacro")
        .moveit_cpp(
            file_path=get_package_share_directory("man2_bt_bringup") + "/config/moveitcpp.yaml"
        )
        .to_moveit_configs()
    )

    # Specify the actions
    start_server = Node(
        package="moveit_skills",
        executable="moveit_skill_server_node",
        name="moveit_skill_server",
        output="screen",
        parameters=[
            {"use_sim_time": use_sim_time},
            params_file,
            moveit_config.to_dict(),
        ],
        # prefix=["xterm -e gdb -ex run --args"],
    )

    # Create the launch description and populate
    ld = LaunchDescription()

    # Set environment variables
    ld.add_action(stdout_linebuf_envvar)

    # Declare the launch options
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(declare_params_file_cmd)

    ld.add_action(start_server)

    return ld