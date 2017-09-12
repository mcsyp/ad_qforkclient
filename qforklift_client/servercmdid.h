#ifndef SERVERCMDID_H
#define SERVERCMDID_H
#include <stdint.h>
namespace server_protocol_cmd{
  //for server protocol communication:
  //ack server->client
  //req server<-client
  enum SERVER_CMDID {
    SERVER_TIME_REQ=0x1,
    SERVER_TIME_ACK=0x2,

    SERVER_UPLOAD_REQ=0x3,
    SERVER_UPLOAD_ACK=0x4,

    SERVER_HB_REQ=0x5,
    SERVER_HB_ACK=0x6,

    SERVER_REALTIME_REQ=0x7,
    SERVER_REALTIME_ACK=0x8,

    SERVER_MASTER_BATTERY_REQ=0x9,
    SERVER_SLAVE_BATTERY_REQ=0xa,

    SERVER_LOGIN_REQ=0x0c,
    SERVER_LOG_REQ=0xaa,

    SERVER_UPLOAD_TRACK_REQ=0xcd,
  };
  //this explains
  enum SERVER_SET_CMDID{
    SERVER_SET_LED=0xab,
    SERVER_SET_DEBUG=0xac,
    SERVER_SET_CLEAR_RECORD=0xad,
    SERVER_SET_RESET=0xae,
    SERVER_SET_UPLOAD_INTERVAL=0xaf,
    SERVER_SET_SLAVE_MAC=0xb1,
  };

  //client upload server struct
  #pragma pack(1)
  typedef struct trajectory_point_s{
    uint64_t timestamp;
    float heading;//indegree
    float distance;//the accumulated distance in meter
    float distance_x;
    float distance_y;
    float distance_abs;
    float delta_x;
    float delta_y;
    float velocity;
  }trajectory_point_t;
  #pragma pack()

  //ble upload data structure
  typedef struct ble_upload_s{
    uint16_t laser_dist;//in mm
    uint16_t ir_dist;//in mm
    uint16_t volt;//in mv
  }ble_upload_t;
}
#endif // PROTOCOLCMD_H
