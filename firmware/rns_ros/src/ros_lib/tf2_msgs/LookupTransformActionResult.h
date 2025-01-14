#ifndef _ROS_tf2_msgs_LookupTransformActionResult_h
#define _ROS_tf2_msgs_LookupTransformActionResult_h

#include <ros_lib/actionlib_msgs/GoalStatus.h>
#include <ros_lib/ros/msg.h>
#include <ros_lib/std_msgs/Header.h>
#include <ros_lib/tf2_msgs/LookupTransformResult.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace tf2_msgs
{

  class LookupTransformActionResult : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef actionlib_msgs::GoalStatus _status_type;
      _status_type status;
      typedef tf2_msgs::LookupTransformResult _result_type;
      _result_type result;

    LookupTransformActionResult():
      header(),
      status(),
      result()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      offset += this->status.serialize(outbuffer + offset);
      offset += this->result.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      offset += this->status.deserialize(inbuffer + offset);
      offset += this->result.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "tf2_msgs/LookupTransformActionResult"; };
    const char * getMD5(){ return "ac26ce75a41384fa8bb4dc10f491ab90"; };

  };

}
#endif