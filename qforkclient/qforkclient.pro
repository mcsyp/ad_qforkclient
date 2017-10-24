QT += core serialport network
QT -= gui

CONFIG += c++11

TARGET = qforkclient
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    serverprotocol.cpp \
    forkliftclient.cpp \
    myhdlc.cpp \
    ../qdist_estimater/ArduinoWrapper.cpp \
    ../qdist_estimater/configparser.cpp \
    ../qdist_estimater/dataframe.cpp \
    ../qdist_estimater/distanceestimater.cpp \
    ../qdist_estimater/dmpthread.cpp \
    ../qdist_estimater/featuremse.cpp \
    ../qdist_estimater/seriallaser.cpp \
    ../qdist_estimater/serialpix4flow.cpp \
    ../qdist_estimater/edison_i2c/edison_i2c.cpp \
    ../qdist_estimater/edison_i2c/i2c.c \
    ../qdist_estimater/MPU6050/MPU6050.cpp \
    ../qdist_estimater/I2Cdev/I2Cdev.cpp \
    serialble.cpp \
    clientsocketthread.cpp \
    ../qgpio_test/neogpio.cpp \
    mytask.cpp \
    i2creader.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += ../qdist_estimater/edison_i2c
INCLUDEPATH += ../qdist_estimater/I2Cdev
INCLUDEPATH += ../qdist_estimater/MPU6050
INCLUDEPATH += ../qdist_estimater
INCLUDEPATH += ../qgpio_test

HEADERS += \
    serverprotocol.h \
    forkliftclient.h \
    myhdlc.h \
    servercmdid.h \
    ../qdist_estimater/ArduinoWrapper.h \
    ../qdist_estimater/configparser.h \
    ../qdist_estimater/dataframe.h \
    ../qdist_estimater/distanceestimater.h \
    ../qdist_estimater/dmpthread.h \
    ../qdist_estimater/featuremse.h \
    ../qdist_estimater/seriallaser.h \
    ../qdist_estimater/serialpix4flow.h \
    ../qdist_estimater/edison_i2c/edison_i2c.h \
    ../qdist_estimater/edison_i2c/i2c-dev-user.h \
    ../qdist_estimater/edison_i2c/i2c.h \
    ../qdist_estimater/MPU6050/helper_3dmath.h \
    ../qdist_estimater/MPU6050/MPU6050.h \
    ../qdist_estimater/MPU6050/MPU6050_6Axis_MotionApps20.h \
    ../qdist_estimater/MPU6050/MPU6050_9Axis_MotionApps41.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/common.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_attitude.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_attitude_quaternion.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_attitude_quaternion_cov.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_attitude_target.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_auth_key.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_autopilot_version.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_battery_status.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_change_operator_control.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_change_operator_control_ack.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_command_ack.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_command_int.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_command_long.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_data_stream.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_data_transmission_handshake.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_debug.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_debug_vect.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_distance_sensor.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_encapsulated_data.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_file_transfer_protocol.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_global_position_int.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_global_position_int_cov.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_global_vision_position_estimate.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_gps2_raw.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_gps2_rtk.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_gps_global_origin.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_gps_inject_data.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_gps_raw_int.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_gps_rtk.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_gps_status.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_heartbeat.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_highres_imu.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_hil_controls.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_hil_gps.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_hil_optical_flow.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_hil_rc_inputs_raw.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_hil_sensor.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_hil_state.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_hil_state_quaternion.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_local_position_ned.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_local_position_ned_cov.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_local_position_ned_system_global_offset.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_log_data.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_log_entry.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_log_erase.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_log_request_data.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_log_request_end.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_log_request_list.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_manual_control.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_manual_setpoint.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_memory_vect.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_ack.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_clear_all.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_count.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_current.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_item.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_item_int.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_item_reached.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_request.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_request_list.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_request_partial_list.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_set_current.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_mission_write_partial_list.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_named_value_float.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_named_value_int.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_nav_controller_output.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_optical_flow.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_optical_flow_rad.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_param_request_list.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_param_request_read.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_param_set.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_param_value.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_ping.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_position_target_global_int.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_position_target_local_ned.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_power_status.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_radio_status.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_raw_imu.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_raw_pressure.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_rc_channels.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_rc_channels_override.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_rc_channels_raw.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_rc_channels_scaled.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_request_data_stream.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_safety_allowed_area.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_safety_set_allowed_area.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_scaled_imu.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_scaled_imu2.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_scaled_pressure.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_serial_control.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_servo_output_raw.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_set_attitude_target.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_set_gps_global_origin.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_set_mode.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_set_position_target_global_int.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_set_position_target_local_ned.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_sim_state.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_statustext.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_sys_status.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_system_time.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_terrain_check.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_terrain_data.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_terrain_report.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_terrain_request.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_v2_extension.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_vfr_hud.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_vicon_position_estimate.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_vision_position_estimate.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/mavlink_msg_vision_speed_estimate.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/testsuite.h \
    ../qdist_estimater/mavlink_flow_v1.0/common/version.h \
    ../qdist_estimater/mavlink_flow_v1.0/checksum.h \
    ../qdist_estimater/mavlink_flow_v1.0/mavlink_conversions.h \
    ../qdist_estimater/mavlink_flow_v1.0/mavlink_helpers.h \
    ../qdist_estimater/mavlink_flow_v1.0/mavlink_protobuf_manager.hpp \
    ../qdist_estimater/mavlink_flow_v1.0/mavlink_types.h \
    ../qdist_estimater/mavlink_flow_v1.0/protocol.h \
    ../qdist_estimater/I2Cdev/I2Cdev.h \
    serialble.h \
    clientsocketthread.h \
    ../qgpio_test/neogpio.h \
    mytask.h \
    i2creader.h
