#ifndef _ROS_move_base_msgs_MoveBaseActionResult_h
#define _ROS_move_base_msgs_MoveBaseActionResult_h

#include <ros_lib/actionlib_msgs/GoalStatus.h>
#include <ros_lib/move_base_msgs/MoveBaseResult.h>
#include <ros_lib/ros/msg.h>
#include <ros_lib/std_msgs/Header.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace move_base_msgs
{

  class MoveBaseActionResult : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef actionlib_msgs::GoalStatus _status_type;
      _status_type status;
      typedef move_base_msgs::MoveBaseResult _result_type;
      _result_type result;

    MoveBaseActionResult():
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

    const char * getType(){ return "move_base_msgs/MoveBaseActionResult"; };
    const char * getMD5(){ return "1eb06eeff08fa7ea874431638cb52332"; };

  };

}
#endif