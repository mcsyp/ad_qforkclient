QT += core serialport network
QT -= gui

CONFIG += c++11

TARGET = forklift_client
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    edison_i2c/edison_i2c.cpp \
    I2Cdev/I2Cdev.cpp \
    MPU6050/MPU6050.cpp \
    ArduinoWrapper.cpp \
    configparser.cpp \
    distanceestimater.cpp \
    seriallaser.cpp \
    serialpix4flow.cpp \
    edison_i2c/i2c.c \
    dmpthread.cpp \
    dataframe.cpp \
    featurestandarddeviation.cpp \
    featuremse.cpp \
    serverprotocol.cpp \
    forkliftclient.cpp \
    serialslave.cpp \
    myhdlc.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += ./edison_i2c
INCLUDEPATH += ./I2Cdev
INCLUDEPATH += ./MPU6050

HEADERS += \
    edison_i2c/edison_i2c.h \
    edison_i2c/i2c-dev-user.h \
    edison_i2c/i2c.h \
    I2Cdev/I2Cdev.h \
    mavlink_flow_v1.0/common/common.h \
    mavlink_flow_v1.0/common/mavlink.h \
    mavlink_flow_v1.0/common/mavlink_msg_attitude.h \
    mavlink_flow_v1.0/common/mavlink_msg_attitude_quaternion.h \
    mavlink_flow_v1.0/common/mavlink_msg_attitude_quaternion_cov.h \
    mavlink_flow_v1.0/common/mavlink_msg_attitude_target.h \
    mavlink_flow_v1.0/common/mavlink_msg_auth_key.h \
    mavlink_flow_v1.0/common/mavlink_msg_autopilot_version.h \
    mavlink_flow_v1.0/common/mavlink_msg_battery_status.h \
    mavlink_flow_v1.0/common/mavlink_msg_change_operator_control.h \
    mavlink_flow_v1.0/common/mavlink_msg_change_operator_control_ack.h \
    mavlink_flow_v1.0/common/mavlink_msg_command_ack.h \
    mavlink_flow_v1.0/common/mavlink_msg_command_int.h \
    mavlink_flow_v1.0/common/mavlink_msg_command_long.h \
    mavlink_flow_v1.0/common/mavlink_msg_data_stream.h \
    mavlink_flow_v1.0/common/mavlink_msg_data_transmission_handshake.h \
    mavlink_flow_v1.0/common/mavlink_msg_debug.h \
    mavlink_flow_v1.0/common/mavlink_msg_debug_vect.h \
    mavlink_flow_v1.0/common/mavlink_msg_distance_sensor.h \
    mavlink_flow_v1.0/common/mavlink_msg_encapsulated_data.h \
    mavlink_flow_v1.0/common/mavlink_msg_file_transfer_protocol.h \
    mavlink_flow_v1.0/common/mavlink_msg_global_position_int.h \
    mavlink_flow_v1.0/common/mavlink_msg_global_position_int_cov.h \
    mavlink_flow_v1.0/common/mavlink_msg_global_vision_position_estimate.h \
    mavlink_flow_v1.0/common/mavlink_msg_gps2_raw.h \
    mavlink_flow_v1.0/common/mavlink_msg_gps2_rtk.h \
    mavlink_flow_v1.0/common/mavlink_msg_gps_global_origin.h \
    mavlink_flow_v1.0/common/mavlink_msg_gps_inject_data.h \
    mavlink_flow_v1.0/common/mavlink_msg_gps_raw_int.h \
    mavlink_flow_v1.0/common/mavlink_msg_gps_rtk.h \
    mavlink_flow_v1.0/common/mavlink_msg_gps_status.h \
    mavlink_flow_v1.0/common/mavlink_msg_heartbeat.h \
    mavlink_flow_v1.0/common/mavlink_msg_highres_imu.h \
    mavlink_flow_v1.0/common/mavlink_msg_hil_controls.h \
    mavlink_flow_v1.0/common/mavlink_msg_hil_gps.h \
    mavlink_flow_v1.0/common/mavlink_msg_hil_optical_flow.h \
    mavlink_flow_v1.0/common/mavlink_msg_hil_rc_inputs_raw.h \
    mavlink_flow_v1.0/common/mavlink_msg_hil_sensor.h \
    mavlink_flow_v1.0/common/mavlink_msg_hil_state.h \
    mavlink_flow_v1.0/common/mavlink_msg_hil_state_quaternion.h \
    mavlink_flow_v1.0/common/mavlink_msg_local_position_ned.h \
    mavlink_flow_v1.0/common/mavlink_msg_local_position_ned_cov.h \
    mavlink_flow_v1.0/common/mavlink_msg_local_position_ned_system_global_offset.h \
    mavlink_flow_v1.0/common/mavlink_msg_log_data.h \
    mavlink_flow_v1.0/common/mavlink_msg_log_entry.h \
    mavlink_flow_v1.0/common/mavlink_msg_log_erase.h \
    mavlink_flow_v1.0/common/mavlink_msg_log_request_data.h \
    mavlink_flow_v1.0/common/mavlink_msg_log_request_end.h \
    mavlink_flow_v1.0/common/mavlink_msg_log_request_list.h \
    mavlink_flow_v1.0/common/mavlink_msg_manual_control.h \
    mavlink_flow_v1.0/common/mavlink_msg_manual_setpoint.h \
    mavlink_flow_v1.0/common/mavlink_msg_memory_vect.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_ack.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_clear_all.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_count.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_current.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_item.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_item_int.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_item_reached.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_request.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_request_list.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_request_partial_list.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_set_current.h \
    mavlink_flow_v1.0/common/mavlink_msg_mission_write_partial_list.h \
    mavlink_flow_v1.0/common/mavlink_msg_named_value_float.h \
    mavlink_flow_v1.0/common/mavlink_msg_named_value_int.h \
    mavlink_flow_v1.0/common/mavlink_msg_nav_controller_output.h \
    mavlink_flow_v1.0/common/mavlink_msg_optical_flow.h \
    mavlink_flow_v1.0/common/mavlink_msg_optical_flow_rad.h \
    mavlink_flow_v1.0/common/mavlink_msg_param_request_list.h \
    mavlink_flow_v1.0/common/mavlink_msg_param_request_read.h \
    mavlink_flow_v1.0/common/mavlink_msg_param_set.h \
    mavlink_flow_v1.0/common/mavlink_msg_param_value.h \
    mavlink_flow_v1.0/common/mavlink_msg_ping.h \
    mavlink_flow_v1.0/common/mavlink_msg_position_target_global_int.h \
    mavlink_flow_v1.0/common/mavlink_msg_position_target_local_ned.h \
    mavlink_flow_v1.0/common/mavlink_msg_power_status.h \
    mavlink_flow_v1.0/common/mavlink_msg_radio_status.h \
    mavlink_flow_v1.0/common/mavlink_msg_raw_imu.h \
    mavlink_flow_v1.0/common/mavlink_msg_raw_pressure.h \
    mavlink_flow_v1.0/common/mavlink_msg_rc_channels.h \
    mavlink_flow_v1.0/common/mavlink_msg_rc_channels_override.h \
    mavlink_flow_v1.0/common/mavlink_msg_rc_channels_raw.h \
    mavlink_flow_v1.0/common/mavlink_msg_rc_channels_scaled.h \
    mavlink_flow_v1.0/common/mavlink_msg_request_data_stream.h \
    mavlink_flow_v1.0/common/mavlink_msg_safety_allowed_area.h \
    mavlink_flow_v1.0/common/mavlink_msg_safety_set_allowed_area.h \
    mavlink_flow_v1.0/common/mavlink_msg_scaled_imu.h \
    mavlink_flow_v1.0/common/mavlink_msg_scaled_imu2.h \
    mavlink_flow_v1.0/common/mavlink_msg_scaled_pressure.h \
    mavlink_flow_v1.0/common/mavlink_msg_serial_control.h \
    mavlink_flow_v1.0/common/mavlink_msg_servo_output_raw.h \
    mavlink_flow_v1.0/common/mavlink_msg_set_attitude_target.h \
    mavlink_flow_v1.0/common/mavlink_msg_set_gps_global_origin.h \
    mavlink_flow_v1.0/common/mavlink_msg_set_mode.h \
    mavlink_flow_v1.0/common/mavlink_msg_set_position_target_global_int.h \
    mavlink_flow_v1.0/common/mavlink_msg_set_position_target_local_ned.h \
    mavlink_flow_v1.0/common/mavlink_msg_sim_state.h \
    mavlink_flow_v1.0/common/mavlink_msg_statustext.h \
    mavlink_flow_v1.0/common/mavlink_msg_sys_status.h \
    mavlink_flow_v1.0/common/mavlink_msg_system_time.h \
    mavlink_flow_v1.0/common/mavlink_msg_terrain_check.h \
    mavlink_flow_v1.0/common/mavlink_msg_terrain_data.h \
    mavlink_flow_v1.0/common/mavlink_msg_terrain_report.h \
    mavlink_flow_v1.0/common/mavlink_msg_terrain_request.h \
    mavlink_flow_v1.0/common/mavlink_msg_v2_extension.h \
    mavlink_flow_v1.0/common/mavlink_msg_vfr_hud.h \
    mavlink_flow_v1.0/common/mavlink_msg_vicon_position_estimate.h \
    mavlink_flow_v1.0/common/mavlink_msg_vision_position_estimate.h \
    mavlink_flow_v1.0/common/mavlink_msg_vision_speed_estimate.h \
    mavlink_flow_v1.0/common/testsuite.h \
    mavlink_flow_v1.0/common/version.h \
    mavlink_flow_v1.0/checksum.h \
    mavlink_flow_v1.0/mavlink_conversions.h \
    mavlink_flow_v1.0/mavlink_helpers.h \
    mavlink_flow_v1.0/mavlink_protobuf_manager.hpp \
    mavlink_flow_v1.0/mavlink_types.h \
    mavlink_flow_v1.0/protocol.h \
    MPU6050/helper_3dmath.h \
    MPU6050/MPU6050.h \
    MPU6050/MPU6050_6Axis_MotionApps20.h \
    MPU6050/MPU6050_9Axis_MotionApps41.h \
    ArduinoWrapper.h \
    configparser.h \
    distanceestimater.h \
    seriallaser.h \
    serialpix4flow.h \
    dmpthread.h \
    dataframe.h \
    featurestandarddeviation.h \
    featuremse.h \
    serverprotocol.h \
    forkliftclient.h \
    serialslave.h \
    myhdlc.h \
    servercmdid.h
