#include "serverprotocol.h"

ServerProtocol::ServerProtocol(QObject* parent):QObject(parent){
  rx_state_ = RxStateIdle;
  rx_payload_size_=0;
}

void ServerProtocol::Reset(){
  //reset rx message state
  rx_payload_.clear();
  rx_state_ = RxStateIdle;
  rx_payload_size_=0;
}

void ServerProtocol::PushToProtocol(QByteArray & raw_data){
  int i=0;
  while(i<raw_data.size()){
    uint8_t * current = (uint8_t*)(raw_data.data()+i);
    int delta=1;
    switch(rx_state_){
    case RxStateProcessing:
        //if there is a previous head
        if(rx_payload_.size()<rx_payload_size_){
          rx_payload_.push_back(raw_data.at(i));
        }
        break;
    case RxStateIdle:
        //step1. check if there is are head since this byte
        if(i<=raw_data.size()-MESSAGE_HEAD_LEN &&
           CheckMessageHead(current,MAGIC_NUM_LEN)){

          //step1.fill the head
           memcpy((uint8_t*)&rx_message_head_,current,sizeof(rx_message_head_));
           if(rx_message_head_.len>MAX_MESSAGE_LEN){
             break;
           }
           rx_payload_size_ = rx_message_head_.len-MESSAGE_HEAD_LEN;
           rx_payload_.clear();//clear the rx payload

           //step2.update  the state
           rx_state_ = RxStateProcessing;

           //step3. i jump to payload
           delta = MESSAGE_HEAD_LEN;
        }
        break;
    }

    if(rx_payload_.size()>=rx_payload_size_){
      //check if a package is full
      emit PayloadReady(rx_message_head_.cmdId,rx_payload_);
      rx_state_ = RxStateIdle;
      rx_payload_.clear();
    }

    //increase
    i=i+delta;
  }
}

bool ServerProtocol::CheckMessageHead(uint8_t * rx_buffer, int rx_len){
  if(rx_buffer==NULL || rx_len<MAGIC_NUM_LEN){
    return false;
  }
  return (rx_buffer[0]==MAGIC_NUM1 &&
          rx_buffer[1]==MAGIC_NUM2 &&
          rx_buffer[2]==MAGIC_NUM3 &&
          rx_buffer[3]==MAGIC_NUM4);
}

void ServerProtocol::FillHead(uint16_t cmd_id, uint32_t payload_len,ServerProtocol::message_head* out_header){
  if(out_header==NULL)return;

  out_header->cmdId =cmd_id;
  out_header->len = MESSAGE_HEAD_LEN + payload_len;
  out_header->magic_num1 = MAGIC_NUM1;
  out_header->magic_num2 = MAGIC_NUM2;
  out_header->magic_num3 = MAGIC_NUM3;
  out_header->magic_num4 = MAGIC_NUM4;
}
