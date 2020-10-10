#include "include.h"
#include "ROS/bot1_rns_core.h"

/*********************************************************************************
*********************************************************************************/
uint32_t t;
uint32_t prev_debug_time = 0;
int counter = 0;

/*********************************************************************************
 	 	 	 	 	 	 	 * ROS NodeHandle *
 *********************************************************************************/
ros::NodeHandle nh;
ros::Time current_time;
uint32_t current_offset;

/*******************************************************************************
 	 	 	 	 	 	 	  * ROS Parameter *
 *******************************************************************************/
char get_prefix[10];
char *get_tf_prefix = get_prefix;

char odom_header_frame_id[30];
char odom_child_frame_id[30];

char imu_frame_id[30];
char mag_frame_id[30];

char joint_state_header_frame_id[30];

/*******************************************************************************
 * Subscriber
 *******************************************************************************/
ros::Subscriber<geometry_msgs::Twist> cmd_vel_sub("cmd_vel", cmdVelCallback);

/*******************************************************************************
 * Publisher
 *******************************************************************************/
char buffer[200];
std_msgs::String info_msg;
ros::Publisher info_pub("rns/info", &info_msg);

// IMU data
sensor_msgs::Imu imu_msg;
ros::Publisher imu_pub("imu", &imu_msg);

// Odometry data
nav_msgs::Odometry odom;
ros::Publisher odom_pub("odom", &odom);

/*******************************************************************************
 * Transform Broadcaster
 *******************************************************************************/
// TF data
geometry_msgs::TransformStamped odom_tf;
tf::TransformBroadcaster tf_broadcaster;

/*******************************************************************************
 * SoftwareTimer
 *******************************************************************************/
static uint32_t tTime[10];

/*******************************************************************************
 * Declaration for RNS
 *******************************************************************************/
float fprevyaw = 0;
float fdifyaw = 0;
float linear_x, linear_y, angular_z;

/*******************************************************************************
 * Declaration for SLAM and navigation
 *******************************************************************************/
unsigned long prev_update_time;
float odom_pose[3];
double odom_vel[3];

/*******************************************************************************
 * Main function
 *******************************************************************************/
int main(void)
{
	Initialize();
	ROS_Init();
	BIOSTIMx_Init(TIM7, 1000, 84, 2, 0); //1ms
	BIOSTIMx_Init(TIM6, 5000, 84, 2, 0); //10ms
	MODNRobotBaseInit(MODN_FWD_OMNI, 2.0, 0.0, &modn);
	MODNRobotVelInit(&linear_x, &linear_y, &angular_z, &modn);
	MODNWheelVelInit(&vel1, &vel2, &vel3, &vel4, &modn);

	main_board_1_data_receive.common_instruction = RNS_PENDING;
	main_board_1_data_receive.common_buffer[0].data = 0;
	main_board_1_data_receive.common_buffer[1].data = 0;
	main_board_1_data_receive.common_buffer[2].data = 0;
	main_board_1_data_receive.common_buffer[3].data = 0;

	SYSSystemInit();
	STTStateInit();
	APPStop();
	APPResetPos();
	wheel_config();
	while (1)
	{
		t = millis();
		STTEventChecker();
		main_board_1_data_receive.common_instruction = RNS_PENDING;
		button();
		if (state != RNS_S_PENDING)
		{
			LED2 = 0;
		}
		else
		{
			LED2 = 1;
		}
		if ((t - prev_debug_time) >= 1000 / PUBLISH_FREQUENCY)
			ROS_Update();
		nh.spinOnce();
	}
}

#ifdef __cplusplus
extern "C"
{
#endif

	void TIM6_DAC_IRQHandler(void)
	{
		if (TIM_GetITStatus(TIM6, TIM_IT_Update) % 200)
		{
			if (counter % 4 == 0)
				LED1 = !LED1;
			SYSSystem5ms();
			SYSSystemAct();
			counter++;
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		}
		return;
	}

#ifdef __cplusplus
}
#endif

void ROS_Init()
{
	nh.initNode();
	nh.getHardware()->setBaud(115200);

	nh.subscribe(cmd_vel_sub);

	nh.advertise(info_pub);
	nh.advertise(imu_pub);
	nh.advertise(odom_pub);

	tf_broadcaster.init(nh);

	// Setting for SLAM and navigation (odometry, joint states, TF)
	initOdom();
	// initJointStates();
	prev_update_time = millis();
}

void ROS_Update()
{
	updateTime();
	updateTFPrefix();
	publishDriveInformation();
	publishImuMsg();

	// sprintf(buffer, "posX:%f, posY:%f, fyaw:%f, fnyaw:%f, velX:%f, velY:%f, velW:%f", fXPos, fYPos, fyaw, fnyaw, fXVel, fYVel, fWVel);
	// info_msg.data = buffer;
	// info_pub.publish(&info_msg);

	prev_debug_time = t;
}

/*******************************************************************************
* Callback functions
*******************************************************************************/
void cmdVelCallback(const geometry_msgs::Twist &cmd_vel_msg)
{
//	 note that the convention for forward directon in ROS is using x-axis
	linear_y = constrain(cmd_vel_msg.linear.x, -2.0, 2.0);
	linear_x = constrain(cmd_vel_msg.linear.y, -2.0, 2.0);
	angular_z = constrain(cmd_vel_msg.angular.z, -2.0, 2.0);
	MODN(&modn);
}

/*******************************************************************************
* Publish msgs (IMU data: angular velocity, linear acceleration, orientation)
*******************************************************************************/
void publishImuMsg(void)
{
	imu_msg.angular_velocity.x = 0;		//imu.gyroData[0];
	imu_msg.angular_velocity.y = 0;		//imu.gyroData[1];
	imu_msg.angular_velocity.z = fWVel; //imu.gyroData[2];
	// imu_msg.angular_velocity_covariance[0] = 0.02;
	// imu_msg.angular_velocity_covariance[1] = 0;
	// imu_msg.angular_velocity_covariance[2] = 0;
	// imu_msg.angular_velocity_covariance[3] = 0;
	// imu_msg.angular_velocity_covariance[4] = 0.02;
	// imu_msg.angular_velocity_covariance[5] = 0;
	// imu_msg.angular_velocity_covariance[6] = 0;
	// imu_msg.angular_velocity_covariance[7] = 0;
	// imu_msg.angular_velocity_covariance[8] = 0.02;

	imu_msg.linear_acceleration.x = fXAcc; //imu.accData[0];
	imu_msg.linear_acceleration.y = fYAcc; //imu.accData[1];
	imu_msg.linear_acceleration.z = 0;	   //imu.accData[2];
	// imu_msg.linear_acceleration_covariance[0] = 0.04;
	// imu_msg.linear_acceleration_covariance[1] = 0;
	// imu_msg.linear_acceleration_covariance[2] = 0;
	// imu_msg.linear_acceleration_covariance[3] = 0;
	// imu_msg.linear_acceleration_covariance[4] = 0.04;
	// imu_msg.linear_acceleration_covariance[5] = 0;
	// imu_msg.linear_acceleration_covariance[6] = 0;
	// imu_msg.linear_acceleration_covariance[7] = 0;
	// imu_msg.linear_acceleration_covariance[8] = 0.04;

	//	imu_msg.orientation.w = imu.quat[0]; ??
	//	imu_msg.orientation.x = imu.quat[1]; ??
	//	imu_msg.orientation.y = imu.quat[2]; ??
	//	imu_msg.orientation.z = imu.quat[3]; ??
	imu_msg.orientation = odom.pose.pose.orientation;

	// imu_msg.orientation_covariance[0] = 0.0025;
	// imu_msg.orientation_covariance[1] = 0;
	// imu_msg.orientation_covariance[2] = 0;
	// imu_msg.orientation_covariance[3] = 0;
	// imu_msg.orientation_covariance[4] = 0.0025;
	// imu_msg.orientation_covariance[5] = 0;
	// imu_msg.orientation_covariance[6] = 0;
	// imu_msg.orientation_covariance[7] = 0;
	// imu_msg.orientation_covariance[8] = 0.0025;

	imu_msg.header.stamp = rosNow();
	imu_msg.header.frame_id = imu_frame_id;

	imu_pub.publish(&imu_msg);
}

/*******************************************************************************
 * Publish msgs (odometry, joint states, tf)
 *******************************************************************************/
void publishDriveInformation(void)
{
	unsigned long time_now = millis();
	unsigned long step_time = time_now - prev_update_time;

	prev_update_time = time_now;
	ros::Time stamp_now = rosNow();

	//odometry
	updateOdometry();
	odom.header.stamp = stamp_now;
	odom_pub.publish(&odom);

	// odometry tf
	updateTF(odom_tf);
	odom_tf.header.stamp = stamp_now;
	tf_broadcaster.sendTransform(odom_tf);

	// joint states
	// updateJointStates();
	// joint_states.header.stamp = stamp_now;
	// joint_states_pub.publish(&joint_states);
}

/*******************************************************************************
* Update TF Prefix
*******************************************************************************/
void updateTFPrefix()
{
	static bool isChecked = false;
	char log_msg[50];

	// if (isConnected)
	// {
	if (isChecked == false)
	{
		nh.getParam("~tf_prefix", &get_tf_prefix);

		if (!strcmp(get_tf_prefix, ""))
		{
			sprintf(odom_header_frame_id, "odom");
			sprintf(odom_child_frame_id, "base_footprint");

			sprintf(imu_frame_id, "imu_link");
			sprintf(mag_frame_id, "mag_link");
			sprintf(joint_state_header_frame_id, "base_link");
		}
		else
		{
			strcpy(odom_header_frame_id, get_tf_prefix);
			strcpy(odom_child_frame_id, get_tf_prefix);

			strcpy(imu_frame_id, get_tf_prefix);
			strcpy(mag_frame_id, get_tf_prefix);
			strcpy(joint_state_header_frame_id, get_tf_prefix);

			strcat(odom_header_frame_id, "/odom");
			strcat(odom_child_frame_id, "/base_footprint");

			strcat(imu_frame_id, "/imu_link");
			strcat(mag_frame_id, "/mag_link");
			strcat(joint_state_header_frame_id, "/base_link");
		}

		sprintf(log_msg, "Setup TF on Odometry [%s]", odom_header_frame_id);
		nh.loginfo(log_msg);

		sprintf(log_msg, "Setup TF on IMU [%s]", imu_frame_id);
		nh.loginfo(log_msg);

		sprintf(log_msg, "Setup TF on MagneticField [%s]", mag_frame_id);
		nh.loginfo(log_msg);

		sprintf(log_msg, "Setup TF on JointState [%s]", joint_state_header_frame_id);
		nh.loginfo(log_msg);

		isChecked = true;
	}
	// }
	// else
	// {
	// isChecked = false;
	// }
}

/*******************************************************************************
 * Update the odometry
 *******************************************************************************/
void updateOdometry(void)
{
// move to system.c SYSSystem5ms() //////////////////////
// original fyaw val +-180

	fdifyaw = fyaw - fprevyaw;
	if (fdifyaw <= -180.0)
		fdifyaw = 360.0 + fdifyaw;
	else if (fdifyaw >= 180.0)
		fdifyaw = 360.0 - fdifyaw;
	else
		fdifyaw = fdifyaw;

	fWVel = DEG2RAD(fdifyaw) / (SAMPLE_TIME * 100);
	fprevyaw = fyaw;

	fnyaw = fyaw / 360.0;

//////////////////////////////////////////////////////////

	odom.header.frame_id = odom_header_frame_id;
	odom.child_frame_id = odom_child_frame_id;

	odom.pose.pose.position.x = fXPos;
	odom.pose.pose.position.y = fYPos;
	odom.pose.pose.position.z = 0;
	odom.pose.pose.orientation = tf::createQuaternionFromYaw(DEG2RAD(fnyaw));

	odom.twist.twist.linear.x = fXVel;
	odom.twist.twist.linear.y = fYVel;
	odom.twist.twist.angular.z = fWVel;
}

/*******************************************************************************
* CalcUpdateulate the TF
*******************************************************************************/
void updateTF(geometry_msgs::TransformStamped &odom_tf)
{
	odom_tf.header = odom.header;
	odom_tf.child_frame_id = odom.child_frame_id;
	odom_tf.transform.translation.x = odom.pose.pose.position.x;
	odom_tf.transform.translation.y = odom.pose.pose.position.y;
	odom_tf.transform.translation.z = odom.pose.pose.position.z;
	odom_tf.transform.rotation = odom.pose.pose.orientation;
}

/*******************************************************************************
 * Update the base time for interpolation
 *******************************************************************************/
void updateTime()
{
	current_offset = millis();
	current_time = nh.now();
}

/*******************************************************************************
 * ros::Time::now() implementation
 *******************************************************************************/
ros::Time rosNow()
{
	return nh.now();
}

/*******************************************************************************
 * Initialization odometry data
 *******************************************************************************/
void initOdom(void)
{
	odom.pose.pose.position.x = 0.0;
	odom.pose.pose.position.y = 0.0;
	odom.pose.pose.position.z = 0.0;

	odom.pose.pose.orientation.x = 0.0;
	odom.pose.pose.orientation.y = 0.0;
	odom.pose.pose.orientation.z = 0.0;
	odom.pose.pose.orientation.w = 0.0;

	odom.twist.twist.linear.x = 0.0;
	odom.twist.twist.linear.y = 0.0;
	odom.twist.twist.angular.z = 0.0;
}

/*******************************************************************************
* Initialization joint states data
*******************************************************************************/
// void initJointStates(void)
// {
//   static char *joint_states_name[] = {(char*)"wheel_left_joint", (char*)"wheel_right_joint"};
//
//   joint_states.header.frame_id = joint_state_header_frame_id;
//   joint_states.name            = joint_states_name;
//
//   joint_states.name_length     = WHEEL_NUM;
//   joint_states.position_length = WHEEL_NUM;
//   joint_states.velocity_length = WHEEL_NUM;
//   joint_states.effort_length   = WHEEL_NUM;
// }
