#ifndef _ROS_actionlib_TestActionFeedback_h
#define _ROS_actionlib_TestActionFeedback_h

#include <ros_lib/actionlib/TestFeedback.h>
#include <ros_lib/actionlib_msgs/GoalStatus.h>
#include <ros_lib/ros/msg.h>
#include <ros_lib/std_msgs/Header.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace actionlib
{

  class TestActionFeedback : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef actionlib_msgs::GoalStatus _status_type;
      _status_type status;
      typedef actionlib::TestFeedback _feedback_type;
      _feedback_type feedback;

    TestActionFeedback():
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

    const char * getType(){ return "actionlib/TestActionFeedback"; };
    const char * getMD5(){ return "6d3d0bf7fb3dda24779c010a9f3eb7cb"; };

  };

}
#endif