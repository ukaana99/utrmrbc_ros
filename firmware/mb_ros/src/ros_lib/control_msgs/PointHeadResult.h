#ifndef _ROS_control_msgs_PointHeadResult_h
#define _ROS_control_msgs_PointHeadResult_h

#include <ros_lib/ros/msg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace control_msgs
{

  class PointHeadResult : public ros::Msg
  {
    public:

    PointHeadResult()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
     return offset;
    }

    const char * getType(){ return "control_msgs/PointHeadResult"; };
    const char * getMD5(){ return "d41d8cd98f00b204e9800998ecf8427e"; };

  };

}
#endif