#ifndef _ROS_turtle_actionlib_ShapeActionFeedback_h
#define _ROS_turtle_actionlib_ShapeActionFeedback_h

#include <ros_lib/actionlib_msgs/GoalStatus.h>
#include <ros_lib/ros/msg.h>
#include <ros_lib/std_msgs/Header.h>
#include <ros_lib/turtle_actionlib/ShapeFeedback.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace turtle_actionlib
{

  class ShapeActionFeedback : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef actionlib_msgs::GoalStatus _status_type;
      _status_type status;
      typedef turtle_actionlib::ShapeFeedback _feedback_type;
      _feedback_type feedback;

    ShapeActionFeedback():
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

    const char * getType(){ return "turtle_actionlib/ShapeActionFeedback"; };
    const char * getMD5(){ return "aae20e09065c3809e8a8e87c4c8953fd"; };

  };

}
#endif