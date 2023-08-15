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

from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch import LaunchDescription
from launch.substitutions import FindExecutable

import yaml
from yaml import SafeLoader

document = """
    target_group: "arm"
    named_state: "home"
    # config:
    #     planning_pipeline: "ompl"
    #     planner_id: "RRTstarkConfigDefault"
    """


def generate_launch_description():
    ld = LaunchDescription()

    req = yaml.load(document, Loader=SafeLoader)

    cmd_str = 'action send_goal --feedback /compute_path_to_state moveit_skills/action/ComputePathToState "{}"'.format(
        str(req)
    )

    ld.add_action(
        ExecuteProcess(cmd=[[FindExecutable(name="ros2"), " {}".format(cmd_str)]], shell=True)
    )
    return ld