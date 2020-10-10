/************************************************/
/*		 	 	Include Header	       		  	*/
/************************************************/

#include "I2C/i2c.h"

/************************************************/
/*		 	 	Variables	      	 		  	*/
/************************************************/

/************************************************/
/*		 	 	Functions		       		  	*/
/************************************************/

/*
 * Function Name		: I2C3Init
 * Function Description : This function is called to initialize I2C3.
 * Function Remarks		: None
 * Function Arguments	: OwnAdress		device�s own 7-bit address
 * 						  ClockSpeed	clock speed of I2C3
 * 						  rh			callback function for slave receive
 * 						  mh			callback function for master receive
 * 						  st			callback function for slave transmit
 * Function Return		: None
 * Function Example		: I2C3Init (main_board_1, CLOCK_SPEED_400KHz, slave_rcv, mas_rcv, slave_trans);
 */

void I2C3Init (uint8_t OwnAdress,uint32_t ClockSpeed,i2c_rcv_handler rh,i2c_rcv_handler mh, i2c_trans_handler st){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	I2C3_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	I2C3_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	I2C3_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	I2C3_GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	I2C3_GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &I2C3_GPIO_InitStructure);

	I2C3_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &I2C3_GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);

	I2C3_InitStructure.I2C_ClockSpeed = ClockSpeed;
	I2C3_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C3_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C3_InitStructure.I2C_OwnAddress1 = OwnAdress<<1;
	I2C3_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C3_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C3, &I2C3_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	I2C3_NVIC_InitStructure.NVIC_IRQChannel = I2C3_EV_IRQn;
	I2C3_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	I2C3_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	I2C3_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&I2C3_NVIC_InitStructure);

	I2C3_NVIC_InitStructure.NVIC_IRQChannel = I2C3_ER_IRQn;
	I2C3_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	I2C3_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	I2C3_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&I2C3_NVIC_InitStructure);

	I2C_ITConfig(I2C3, I2C_IT_EVT, ENABLE);
    I2C_ITConfig(I2C3, I2C_IT_ERR, ENABLE);
    I2C_ITConfig(I2C3, I2C_IT_BUF, ENABLE);

    srcvhandlerI2C3 = rh;
  	mrcvhandlerI2C3 = mh;
  	stranshandlerI2C3 = st;

    I2C_StretchClockCmd(I2C3, ENABLE);
	I2C_Cmd(I2C3, ENABLE);

}

/*
 * Function Name		: I2C3_EV_IRQHandler
 * Function Description : I2C3 event interrupt handler.
 * Function Remarks		: This interrupt handle slave receive mode, master receive mode and slave transmit mode.
 * Function Arguments	: None
 * Function Return		: None
 * Function Example		: None
 */

void I2C3_EV_IRQHandler(void){

//	CPU_SR_ALLOC();
//	OS_ENTER_CRITICAL();
//	OSIntEnter();
//	OS_EXIT_CRITICAL();

	switch(I2C_GetLastEvent(I2C3)){

/**********************************************I2C MASTER MODE****************************************************/

	/*THIS STATE WILL CHECK WHETHER MASTER ENTER TRANSMITTER OR RECEIVER MODE*/
		case I2C_EVENT_MASTER_MODE_SELECT :

			I2C_Send7bitAddress(I2C3, I2C3slaveadress << 1, I2C3_direction);
			break;

	/*MASTER TRANSMITTER MODE*/
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:

			I2C3_Txlength = 0;
			I2C_SendData(I2C3, I2C3_master_snd_buf[I2C3_Txlength++]);
			break;

		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:

			if(I2C3_Txlength ==  I2C3txlen){
				while (!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
				I2C_GenerateSTOP(I2C3, ENABLE);
				I2C3_Master_Flag = 0;
			}else{
				I2C_SendData(I2C3, I2C3_master_snd_buf[I2C3_Txlength++]);
			}
			break;

	/*MASTER RECEIVER MODE*/
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:

			I2C3_Rxlength = 0;
			if(I2C3rxlen == 1){
				I2C_AcknowledgeConfig(I2C3, DISABLE);
				I2C_GenerateSTOP(I2C3, ENABLE);
			}
			break;

		case I2C_EVENT_MASTER_BYTE_RECEIVED:

			if(I2C3rxlen == 1){

				I2C3_master_rcv_buf[I2C3_Rxlength++] = I2C_ReceiveData(I2C3);
				if(mrcvhandlerI2C3)
					mrcvhandlerI2C3(I2C3rxlen, I2C3_master_rcv_buf);
				I2C3_Master_Flag = 0;

			}else{

				I2C3_master_rcv_buf[I2C3_Rxlength++] = I2C_ReceiveData(I2C3);

				if (I2C3_Rxlength == (I2C3rxlen-1)){
					I2C_AcknowledgeConfig(I2C3, DISABLE);
					I2C_GenerateSTOP(I2C3, ENABLE);
				}

				if(I2C3_Rxlength == (I2C3rxlen)){

					I2C_AcknowledgeConfig(I2C3, ENABLE);
					if(mrcvhandlerI2C3)
						mrcvhandlerI2C3(I2C3rxlen, I2C3_master_rcv_buf);
					I2C3_Master_Flag = 0;
				}

			}
			break;

/****************************************************************************************************************/

/**********************************************I2C SLAVE MODE****************************************************/

	/*SLAVE TRANSMITTER MODE*/
		case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:

			I2C3_Txlength = 0;
			if(stranshandlerI2C3)
				stranshandlerI2C3(&I2C3txlen, I2C3_slave_snd_buf);
			I2C_SendData(I2C3, I2C3_slave_snd_buf[I2C3_Txlength++]);

			break;

		case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:
		case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:

			if (I2C3_Txlength < I2C3txlen){
				I2C_SendData(I2C3, I2C3_slave_snd_buf[I2C3_Txlength++]);
			}
			break;

	/*SLAVE RECEIVER MODE*/
		case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:

			I2C3_Rxlength = 0;
			break;

		case I2C_EVENT_SLAVE_BYTE_RECEIVED:

			I2C3_slave_rcv_buf[I2C3_Rxlength++] = I2C_ReceiveData(I2C3);
			break;

		case I2C_EVENT_SLAVE_STOP_DETECTED:

			I2C_Cmd(I2C3, ENABLE);
			if(srcvhandlerI2C3)
				srcvhandlerI2C3(I2C3_slave_rcv_buf[1] + 3, I2C3_slave_rcv_buf);
			break;

/*****************************************************************************************************************/

		default: break;

	}

//	OSIntExit();
}

/*
 * Function Name		: I2C3_ER_IRQHandler
 * Function Description : I2C3 Error interrupt handler.
 * Function Remarks		: This interrupt handle the error event of I2C3.
 * Function Arguments	: None
 * Function Return		: None
 * Function Example		: None
 */

void I2C3_ER_IRQHandler(void){

//	CPU_SR_ALLOC();
//	OS_ENTER_CRITICAL();
//	OSIntEnter();
//	OS_EXIT_CRITICAL();

	if(I2C_GetLastEvent(I2C3)== I2C_EVENT_SLAVE_ACK_FAILURE)
		I2C_ClearFlag(I2C3, I2C_FLAG_AF);
	else if(I2C_GetFlagStatus(I2C3, I2C_FLAG_OVR) == SET)
		I2C_ClearFlag(I2C3, I2C_FLAG_OVR);
	else if(I2C_GetFlagStatus(I2C3, I2C_FLAG_ARLO) == SET)
		I2C_ClearFlag(I2C3, I2C_FLAG_ARLO);
	else if(I2C_GetFlagStatus(I2C3, I2C_FLAG_BERR) == SET)
		I2C_ClearFlag(I2C3, I2C_FLAG_BERR);
	else if (I2C3_Master_Flag == 1)
		I2C3ReInit();

//	OSIntExit();
}

