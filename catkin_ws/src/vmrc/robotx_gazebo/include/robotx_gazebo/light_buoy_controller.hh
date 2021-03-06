/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef ROBOTX_GAZEBO_LIGHT_BUOY_CONTROLLER_HH_
#define ROBOTX_GAZEBO_LIGHT_BUOY_CONTROLLER_HH_

#include <ros/ros.h>
#include <std_msgs/ColorRGBA.h>
#include <std_srvs/Trigger.h>

#include <array>
#include <cstdint>
#include <mutex>
#include <string>
#include <utility>
#include <gazebo/gazebo.hh>

/// \brief Controls the RobotX light buoy (scan the code challenge).
///
/// Selects a random sequence of 3 colors selected from red, green, blue, and
/// yellow without consecutive repeats. This sequence is displayed
/// simultaneously on the light buoy's 3 panels, each for 1 second, followed by
/// a second with the panels off. A new sequence can be generated by calling a
/// ROS service. Be be configured with the following SDF tags:
///
///   <robotNamespace> ROS namespace of Node, can be used to have multiple light
///                    buoys.
class LightBuoyController : public gazebo::ModelPlugin
{
  /// \brief Constructor.
  public: LightBuoyController() = default;

  // Documentation inherited.
  public: void Load(gazebo::physics::ModelPtr _parent,
                    sdf::ElementPtr _sdf);

  /// \brief Creates a std_msgs::ColorRGBA message from 4 doubles.
  /// \param[in] _r Red.
  /// \param[in] _g Green.
  /// \param[in] _b Blue.
  /// \param[in] _a Alpha.
  /// \return The ColorRGBA message.
  private: static std_msgs::ColorRGBA CreateColor(const double _r,
                                                  const double _g,
                                                  const double _b,
                                                  const double _a);

  /// \brief Display the next color in the sequence, or start over if at the end
  /// \param[in] _event Not used.
  private: void IncrementState(const ros::TimerEvent &_event);

  /// \brief Callback for change pattern service, calls other changePattern
  /// internaly.
  /// \param[in] _req Not used.
  /// \param[out] _res The Response containing a message with the new pattern.
  /// \return True when the operation succeed or false otherwise.
  private: bool ChangePattern(std_srvs::Trigger::Request &_req,
                              std_srvs::Trigger::Response &_res);

  /// \brief Generate a new pattern and reset state to OFF.
  /// \param[in, out] _message The current pattern in string format, where
  /// each color is represented with its initial letter.
  /// E.g.: "RYG".
  private: void ChangePattern(std::string &_message);

  /// \def Colors_t
  /// \brief A pair of RGBA color and its name as a string.
  private: using Colors_t = std::pair<std_msgs::ColorRGBA, std::string>;

  /// \def Pattern_t
  /// \brief The current pattern to display, pattern_[3] is always OFF.
  private: using Pattern_t = std::array<uint8_t, 5>;

  /// \brief List of the color options (red, green, blue, yellow and no color)
  /// with their string name for logging.
  private: static const std::array<Colors_t, 5> kColors;

  /// \brief Publisher to set color to each of the 3 panels using the
  /// gazebo_ros_color plugin.
  private: std::array<ros::Publisher, 3> panelPubs;

  /// \brief Node handle.
  private: ros::NodeHandle nh;

  /// \brief Service to generate and display a new color sequence.
  private: ros::ServiceServer changePatternServer;

  /// \brief Timer triggered in simulated time to update the color.
  private: ros::Timer timer;

  /// \brief Track current index in pattern.
  /// \sa IncrementState(const ros::TimerEvent &_event)
  private: uint8_t state;

  /// \brief The current color pattern.
  /// \sa ChangePattern(std::string &_message).
  private: Pattern_t pattern;

  /// \brief Locks state and pattern member variables.
  private: std::mutex mutex;
};

#endif
