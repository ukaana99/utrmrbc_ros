#ifndef _ROS_actionlib_tutorials_AveragingActionFeedback_h
#define _ROS_actionlib_tutorials_AveragingActionFeedback_h

#include <ros_lib/actionlib_msgs/GoalStatus.h>
#include <ros_lib/actionlib_tutorials/AveragingFeedback.h>
#include <ros_lib/ros/msg.h>
#include <ros_lib/std_msgs/Header.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace actionlib_tutorials
{

  class AveragingActionFeedback : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef actionlib_msgs::GoalStatus _status_type;
      _status_type status;
      typedef actionlib_tutorials::AveragingFeedback _feedback_type;
      _feedback_type feedback;

    AveragingActionFeedback():
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

    const char * getType(){ return "actionlib_tutorials/AveragingActionFeedback"; };
    const char * getMD5(){ return "78a4a09241b1791069223ae7ebd5b16b"; };

  };

}
#endif