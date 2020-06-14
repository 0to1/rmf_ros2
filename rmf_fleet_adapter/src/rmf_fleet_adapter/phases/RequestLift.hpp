/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#ifndef SRC__RMF_FLEET_ADAPTER__PHASES__REQUESTLIFT_HPP
#define SRC__RMF_FLEET_ADAPTER__PHASES__REQUESTLIFT_HPP

#include "../Task.hpp"
#include "../agv/RobotContext.hpp"
#include "rmf_fleet_adapter/StandardNames.hpp"

namespace rmf_fleet_adapter {
namespace phases {

struct RequestLift
{
  class Action : public std::enable_shared_from_this<Action>
  {
  public:

    static std::shared_ptr<Action> make(
      std::string lift_name,
      std::string destination,
      agv::RobotContextPtr context,
      rmf_traffic::Time expected_finish);

    inline const rxcpp::observable<Task::StatusMsg>& get_observable() const
    {
      return _obs;
    }

  private:

    std::string _lift_name;
    std::string _destination;
    agv::RobotContextPtr _context;
    rmf_traffic::Time _expected_finish;
    rxcpp::observable<Task::StatusMsg> _obs;
    rclcpp::TimerBase::SharedPtr _timer;

    Action(
      std::string lift_name,
      std::string destination,
      agv::RobotContextPtr context,
      rmf_traffic::Time expected_finish);

    void _init_obs();

    Task::StatusMsg _get_status(const rmf_lift_msgs::msg::LiftState::SharedPtr& lift_state);

    void _do_publish();
  };

  class ActivePhase : public Task::ActivePhase
  {
  public:

    ActivePhase(
      std::string lift_name,
      std::string destination,
      agv::RobotContextPtr context,
      rmf_traffic::Time expected_finish);

    const rxcpp::observable<Task::StatusMsg>& observe() const override;

    rmf_traffic::Duration estimate_remaining_time() const override;

    void emergency_alarm(bool on) override;

    void cancel() override;

    const std::string& description() const override;

  private:

    std::string _lift_name;
    std::string _destination;
    rxcpp::observable<rmf_lift_msgs::msg::LiftState::SharedPtr> _lift_state_obs;
    std::string _description;
    std::shared_ptr<Action> _action;
  };

  class PendingPhase : public Task::PendingPhase
  {
  public:

    PendingPhase(
      std::string lift_name,
      std::string destination,
      agv::RobotContextPtr context,
      rmf_traffic::Time expected_finish);

    std::shared_ptr<Task::ActivePhase> begin() override;

    rmf_traffic::Duration estimate_phase_duration() const override;

    const std::string& description() const override;

  private:
    std::string _lift_name;
    std::string _destination;
    agv::RobotContextPtr _context;
    rmf_traffic::Time _expected_finish;
    std::string _description;
  };
};

} // namespace phases
} // namespace rmf_fleet_adapter

#endif // SRC__RMF_FLEET_ADAPTER__PHASES__REQUESTLIFT_HPP
