#ifndef _ROS_control_msgs_PointHeadActionFeedback_h
#define _ROS_control_msgs_PointHeadActionFeedback_h

#include <ros_lib/actionlib_msgs/GoalStatus.h>
#include <ros_lib/control_msgs/PointHeadFeedback.h>
#include <ros_lib/ros/msg.h>
#include <ros_lib/std_msgs/Header.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace control_msgs
{

  class PointHeadActionFeedback : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef actionlib_msgs::GoalStatus _status_type;
      _status_type status;
      typedef control_msgs::PointHeadFeedback _feedback_type;
      _feedback_type feedback;

    PointHeadActionFeedback():
      header(),
      status(),
      feedback()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      offset += this->status.serialize(outbuffer + offset);
      offset += this->feedback.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      offset += this->status.deserialize(inbuffer + offset);
      offset += this->feedback.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "control_msgs/PointHeadActionFeedback"; };
    const char * getMD5(){ return "33c9244957176bbba97dd641119e8460"; };

  };

}
#endif