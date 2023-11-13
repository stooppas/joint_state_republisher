#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/joint_state.hpp>

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class JointStateRepublisher : public rclcpp::Node
{
  public:
    JointStateRepublisher()
    : Node("joint_state_republisher")
    {
        this->declare_parameter("rate",10);

        int rate = this->get_parameter("rate").as_int();

        subscriber_ = this->create_subscription<sensor_msgs::msg::JointState>(
          "joint_states", 10,
          std::bind(&JointStateRepublisher::jointStateCallback, this,
                    std::placeholders::_1));

      publisher_ = this->create_publisher<sensor_msgs::msg::JointState>("joint_states_reduced", 10);
      timer_ = this->create_wall_timer((1000ms)/rate, std::bind(&JointStateRepublisher::timer_callback, this));
    }

  private:
    void timer_callback()
    {
        if(last_msg)
        {
            publisher_->publish(*last_msg);
        }
    }
    void jointStateCallback(
        const sensor_msgs::msg::JointState::SharedPtr msg) {
        last_msg = msg;
    }

    rclcpp::TimerBase::SharedPtr timer_;
    sensor_msgs::msg::JointState::SharedPtr last_msg;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscriber_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<JointStateRepublisher>());
  rclcpp::shutdown();
  return 0;
}