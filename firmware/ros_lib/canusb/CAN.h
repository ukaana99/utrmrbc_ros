#ifndef _ROS_canusb_CAN_h
#define _ROS_canusb_CAN_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "ros/time.h"

namespace canusb
{

  class CAN : public ros::Msg
  {
    public:
      typedef ros::Time _timestamp_type;
      _timestamp_type timestamp;
      typedef uint16_t _stdId_type;
      _stdId_type stdId;
      typedef int32_t _extId_type;
      _extId_type extId;
      uint32_t data_length;
      typedef uint8_t _data_type;
      _data_type st_data;
      _data_type * data;

    CAN():
      timestamp(),
      stdId(0),
      extId(0),
      data_length(0), data(NULL)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->timestamp.sec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->timestamp.sec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->timestamp.sec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->timestamp.sec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->timestamp.sec);
      *(outbuffer + offset + 0) = (this->timestamp.nsec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->timestamp.nsec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->timestamp.nsec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->timestamp.nsec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->timestamp.nsec);
      *(outbuffer + offset + 0) = (this->stdId >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->stdId >> (8 * 1)) & 0xFF;
      offset += sizeof(this->stdId);
      union {
        int32_t real;
        uint32_t base;
      } u_extId;
      u_extId.real = this->extId;
      *(outbuffer + offset + 0) = (u_extId.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_extId.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_extId.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_extId.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->extId);
      *(outbuffer + offset + 0) = (this->data_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->data_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->data_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->data_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->data_length);
      for( uint32_t i = 0; i < data_length; i++){
      *(outbuffer + offset + 0) = (this->data[i] >> (8 * 0)) & 0xFF;
      offset += sizeof(this->data[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      this->timestamp.sec =  ((uint32_t) (*(inbuffer + offset)));
      this->timestamp.sec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->timestamp.sec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->timestamp.sec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->timestamp.sec);
      this->timestamp.nsec =  ((uint32_t) (*(inbuffer + offset)));
      this->timestamp.nsec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->timestamp.nsec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->timestamp.nsec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->timestamp.nsec);
      this->stdId =  ((uint16_t) (*(inbuffer + offset)));
      this->stdId |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->stdId);
      union {
        int32_t real;
        uint32_t base;
      } u_extId;
      u_extId.base = 0;
      u_extId.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_extId.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_extId.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_extId.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->extId = u_extId.real;
      offset += sizeof(this->extId);
      uint32_t data_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      data_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      data_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      data_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->data_length);
      if(data_lengthT > data_length)
        this->data = (uint8_t*)realloc(this->data, data_lengthT * sizeof(uint8_t));
      data_length = data_lengthT;
      for( uint32_t i = 0; i < data_length; i++){
      this->st_data =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->st_data);
        memcpy( &(this->data[i]), &(this->st_data), sizeof(uint8_t));
      }
     return offset;
    }

    const char * getType(){ return "canusb/CAN"; };
    const char * getMD5(){ return "538459a17b716b5d40481761364b2a7c"; };

  };

}
#endif
