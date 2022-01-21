#pragma once
#ifndef BODYESTIMATOR_H
#define BODYESTIMATOR_H

#include <Eigen/Dense>
#include <vector>
#include "ros/ros.h"
#include "utils/husky_data.hpp"
#include "system/husky_state.hpp"
#include "core/InEKF.h"

#include "geometry_msgs/Point.h"
#include "nav_msgs/Path.h"
// #include "visualization_msgs/MarkerArray.h"
// #include "inekf_msgs/State.h"

namespace husky_inekf{
class BodyEstimator {

    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        BodyEstimator();
        bool enabled();
        void enableFilter();
        void disable();
        bool biasInitialized();
        void initBias(const ImuMeasurement<double>& imu_packet_in);
        // void initState();
        void initState(const ImuMeasurement<double>& imu_packet_in, 
                        const JointStateMeasurement& joint_state_packet_in, HuskyState& state);
        void initState(const ImuMeasurement<double>& imu_packet_in, 
                        const VelocityMeasurement& velocity_packet_in, HuskyState& state);
        void propagateIMU(const ImuMeasurement<double>& imu_packet_in, HuskyState& state);
        void correctVelocity(const JointStateMeasurement& joint_state_packet_in, HuskyState& state);
        void correctVelocity(const VelocityMeasurement& velocity_packet_in, HuskyState& state);
        
        inekf::InEKF getFilter() const;
        inekf::RobotState getState() const;

    private:
        // ROS related
        std::vector<geometry_msgs::PoseStamped> poses_;
        // inekf related
        inekf::InEKF filter_;
        bool enabled_ = false;
        bool bias_initialized_ = false;
        bool static_bias_initialization_ = false;
        bool estimator_debug_enabled_ = false;
        std::vector<Eigen::Matrix<double,6,1>,Eigen::aligned_allocator<Eigen::Matrix<double,6,1>>> bias_init_vec_;
        Eigen::Vector3d bg0_ = Eigen::Vector3d::Zero();
        Eigen::Vector3d ba0_ = Eigen::Vector3d::Zero();
        double t_prev_;
        uint32_t seq_;
        double velocity_t_thres_;
        Eigen::Matrix<double,6,1> imu_prev_;
        // const Eigen::Matrix<double,4,4> encoder_cov_ = 0.0174533*0.0174533 * Eigen::Matrix<double,4,4>::Identity(); // 1 deg std dev 
        Eigen::Matrix<double,3,3> velocity_cov_; // covariance of velocity measuremnts from wheel velocity.
};

} // end husky_inekf namespace
#endif // BODYESTIMATOR_H