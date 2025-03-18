#ifndef MUJOCO_ROS2_CONTROL_PLUGIN_HPP_
#define MUJOCO_ROS2_CONTROL_PLUGIN_HPP_

// ROS2
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include "pluginlib/class_loader.hpp"
#include "rosgraph_msgs/msg/clock.hpp"

#include "controller_manager/controller_manager.hpp"
#include "hardware_interface/component_parser.hpp"
#include "hardware_interface/resource_manager.hpp"
#include "hardware_interface/system_interface.hpp"
#include "mujoco_ros2_control/mujoco_system.hpp"

// Mujoco
#include <mujoco/mjdata.h>
#include <mujoco/mjmodel.h>
#include <mujoco/mjtnum.h>
#include <mujoco/mjvisualize.h>

#include <limits>
#include <string>

namespace mujoco_ros2_control
{

/** \brief Plugin to let ros2_controllers control the robot in Mujoco via ROS topic. */
class MujocoRos2ControlPlugin
{
public:
  /** \brief Register plugin. */
  static void RegisterPlugin();

  /** \brief Create an instance.
      \param m model
      \param d data
      \param plugin_id plugin ID
   */
  static MujocoRos2ControlPlugin* Create(const mjModel *m, mjData *d, int plugin_id);

public:
  /** \brief Copy constructor. */
  MujocoRos2ControlPlugin(MujocoRos2ControlPlugin &&) = default;

  ~MujocoRos2ControlPlugin()
  {
    RCLCPP_INFO(controller_manager_->get_logger(), "Called MujocoRos2ControlPlugin destructor");
  }

  /** \brief Reset.
   \param m model
   \param plugin_id plugin ID
   */
  void reset(const mjModel *m, int plugin_id);

  /** \brief Destroy.
   \param m model
   \param plugin_id plugin ID
   */
  void destroy();

  /** \brief Compute.
   \param m model
   \param d data
   \param plugin_id plugin ID
   */
  void compute(const mjModel *m, mjData *d, int plugin_id);

  /** \brief gets URDF robot description */
  std::string get_robot_description();

protected:
  /** \brief Constructor.
    \param m model
    \param d data
    \param actuator_id actuator ID
    \param topic_name topic name
  */
  MujocoRos2ControlPlugin(const mjModel *mj_model, mjData *mj_data)
    : mj_model_(mj_model), mj_data_(mj_data), last_update_sim_time_ros_(0, 0, RCL_ROS_TIME)
  {
  }

  bool initialise();

protected:
  const mjModel *mj_model_;
  mjData *mj_data_;
  std::thread cm_thread_;

  // ROS variables
  // rclcpp::Node::SharedPtr node_;
  rclcpp::executors::MultiThreadedExecutor::SharedPtr cm_executor_;
  rclcpp::Duration control_period_ = rclcpp::Duration(1, 0);
  rclcpp::Time last_update_sim_time_ros_;
  std::shared_ptr<pluginlib::ClassLoader<MujocoSystemInterface>> robot_hw_sim_loader_;
  std::shared_ptr<controller_manager::ControllerManager> controller_manager_;
};

}  // namespace mujoco_ros2_control

#endif  // MUJOCO_ROS2_CONTROL_PLUGIN_HPP_
