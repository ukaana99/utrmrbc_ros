#ifndef _ROS_nav_msgs_GetMapGoal_h
#define _ROS_nav_msgs_GetMapGoal_h

#include <ros_lib/ros/msg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace nav_msgs
{

  class GetMapGoal : public ros::Msg
  {
    public:

    GetMapGoal()
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

    const char * getType(){ return "nav_msgs/GetMapGoal"; };
    const char * getMD5(){ return "d41d8cd98f00b204e9800998ecf8427e"; };

  };

}
#endif