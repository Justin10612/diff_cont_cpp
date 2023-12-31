#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/vector3.hpp"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

const double robot_width = 0.35;       // meter
const double wheel_radius = 0.0625; // meter
const double b = robot_width/2.0;

// bool idle_state = true;

class DiffDriveController : public rclcpp::Node
{
  public:
    DiffDriveController() : Node("diff_drive_controller")
    {
      wheel_cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Vector3>("wheel_cmd_vel", 10);
      cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", 10, std::bind(&DiffDriveController::cmd_vel_callback, this, std::placeholders::_1));
      // idle_mode_sub_ = this->create_subscription<std_msgs::msg::Bool>(
      //   "idle_bool", 10, std::bind(&DiffDriveController::idle_callback, this, std::placeholders::_1));
    }

  private:
    rclcpp::Publisher<geometry_msgs::msg::Vector3>::SharedPtr wheel_cmd_vel_pub_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    // rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr idle_mode_sub_;
    
    void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr cmd_message) const
    {
      auto out_message = geometry_msgs::msg::Vector3();
      // Get the cmd vel
      double v = cmd_message->linear.x;
      double w = cmd_message->angular.z;
      out_message.x=0;
      out_message.y=0;
      // Calculate the wheel speed
      out_message.x = (v-w*b)/wheel_radius;  // left wheel speed
      out_message.y = (v+w*b)/wheel_radius; // right wheel speed
      // Publish data
      wheel_cmd_vel_pub_->publish(out_message);
    }

    // void idle_callback(const std_msgs::msg::Bool::SharedPtr idle_msgs) const
    // {
    //   idle_state = idle_msgs->data;
    //   if(idle_state==true){
    //     auto out_message = geometry_msgs::msg::Vector3();
    //     out_message.x = 0;  // left wheel speed
    //     out_message.y = 0; // right wheel speed
    //     // Publish data
    //     wheel_cmd_vel_pub_->publish(out_message);
    //   }
    // }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DiffDriveController>());
  rclcpp::shutdown();
  return 0;
}