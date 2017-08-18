#include "AM2320.h"
#include "smbus.h"
#include "delay.h"
uint8_t AM2320_buff[8];

void AM2320_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /* GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

		/* I2C1 clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		/* I2C1 SDA and SCL configuration */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Enable I2C1 reset state */
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
		/* Release I2C1 from reset state */
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
				
		/* I2C1 and I2C2 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x01;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
    I2C_Init(I2C1, &I2C_InitStructure);

		
}

void AM2320_wakeup(void)
{
    __IO uint32_t Timeout = 0;
		__IO uint8_t Address;
	    /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
    I2C1->CR2 |= I2C_IT_ERR;
	
		Timeout = 0xFFFF;
		/* Send START condition */
		I2C1->CR1 |= 0x0100;
		/* Wait until SB flag is set: EV5 */
		while ((I2C1->SR1&0x0001) != 0x0001)
		{
				if (Timeout-- == 0)
						return;
		}

		/* Send slave address */
		/* Reset the address bit0 for write*/
		//SlaveAddress &= OAR1_ADD0_Reset;
		Address = AM2320_ADDR & 0xFFFE;
		/* Send the slave address */
		I2C1->DR = Address;
		
		Delay_ms(1);

		/* Send STOP condition */
		I2C1->CR1 |= 0x0200;
		/* Make sure that the STOP bit is cleared by Hardware */
		while ((I2C1->CR1&0x200) == 0x200);	
}
		

void AM2320_write(unsigned char *pBuffer, unsigned char NumByteToWrite)
{
		__IO uint32_t temp = 0;
		__IO uint32_t Timeout = 0;
		__IO uint8_t Address;
	    /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
    I2C1->CR2 |= I2C_IT_ERR;
	
		Timeout = 0xFFFF;
		/* Send START condition */
		I2C1->CR1 |= 0x0100;
		/* Wait until SB flag is set: EV5 */
		while ((I2C1->SR1&0x0001) != 0x0001)
		{
				if (Timeout-- == 0)
						return;
		}

		/* Send slave address */
		/* Reset the address bit0 for write*/
		//SlaveAddress &= OAR1_ADD0_Reset;
		Address = AM2320_ADDR & 0xFFFE;
		/* Send the slave address */
		I2C1->DR = Address;
		Timeout = 0xFFFF;
		/* Wait until ADDR is set: EV6 */
		while ((I2C1->SR1 &0x0002) != 0x0002)
		{
				if (Timeout-- == 0)
							return;
		}

		/* Clear ADDR flag by reading SR2 register */
		temp = I2C1->SR2;
		/* Write the first data in DR register (EV8_1) */
		I2C1->DR = *pBuffer;
		/* Increment */
		pBuffer++;
		/* Decrement the number of bytes to be written */
		NumByteToWrite--;
		/* While there is data to be written */
		while (NumByteToWrite--)
		{
				/* Poll on BTF to receive data because in polling mode we can not guarantee the
					EV8 software sequence is managed before the current byte transfer completes */
				while ((I2C1->SR1 & 0x00004) != 0x000004);
				/* Send the current byte */
				I2C1->DR = *pBuffer;
				/* Point to the next byte to be written */
				pBuffer++;
		}
		/* EV8_2: Wait until BTF is set before programming the STOP */
		while ((I2C1->SR1 & 0x00004) != 0x000004);
		/* Send STOP condition */
		I2C1->CR1 |= 0x0200;
		/* Make sure that the STOP bit is cleared by Hardware */
		while ((I2C1->CR1&0x200) == 0x200);	
	
	/*
     unsigned char s = 0x00;
		uint32_t counter = SMBus_Max_Delay_Cycles;

    while (I2C_GetFlagStatus(SMBus_NAME, I2C_FLAG_BUSY) && counter) --counter;
    if(counter == 0) {
			s = 0x00;
	    return;
    }
		
     I2C_GenerateSTART(SMBus_NAME, ENABLE);
		
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
			s = 0x00;
	    return;
    }
		
     I2C_Send7bitAddress(SMBus_NAME, AM2320_ADDR>>1, I2C_Direction_Transmitter);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
			s = 0x00;
	    return;
    }
		
     for(s = 0x00; s < length; s++)
     {
			 I2C_SendData(SMBus_NAME,*value++);
				counter = SMBus_Max_Delay_Cycles;
				while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
				if(counter == 0) {
					s = 0x00;
					return;
				}
     }
     
     I2C_GenerateSTOP(SMBus_NAME, ENABLE);
		 */
}


void AM2320_read(unsigned char *pBuffer, unsigned char NumByteToRead)
{
	  __IO uint32_t temp = 0;
    __IO uint32_t Timeout = 0;
		__IO uint8_t Address;

    /* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
    I2C1->CR2 |= I2C_IT_ERR;
	
        if (NumByteToRead == 1)
        {
            Timeout = 0xFFFF;
            /* Send START condition */
            I2C1->CR1 |= 0x0100;
            /* Wait until SB flag is set: EV5  */
            while ((I2C1->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return;
            }
            /* Send slave address */
            /* Reset the address bit0 for read */
            //SlaveAddress |= OAR1_ADD0_Set;
            Address = AM2320_ADDR | 0x0001;
            /* Send the slave address */
            I2C1->DR = Address;
            /* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
            and program the STOP just after ADDR is cleared. The EV6_3 
            software sequence must complete before the current byte end of transfer.*/
            /* Wait until ADDR is set */
            Timeout = 0xFFFF;
            while ((I2C1->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return;
            }
            /* Clear ACK bit */
            I2C1->CR1 &= 0xFBFF;
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            /* Clear ADDR flag */
            temp = I2C1->SR2;
            /* Program the STOP */
            I2C1->CR1 |= 0x0200;
            /* Re-enable IRQs */
            __enable_irq();
            /* Wait until a data is received in DR register (RXNE = 1) EV7 */
            while ((I2C1->SR1 & 0x00040) != 0x000040);
            /* Read the data */
            *pBuffer = I2C1->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2C1->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2C1->CR1 |= 0x0400;

        }

        else if (NumByteToRead == 2)
        {
            /* Set POS bit */
            I2C1->CR1 |= 0x0800;
            Timeout = 0xFFFF;
            /* Send START condition */
            I2C1->CR1 |= 0x0100;
            /* Wait until SB flag is set: EV5 */
            while ((I2C1->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Set the address bit0 for read */
            //SlaveAddress |= OAR1_ADD0_Set;
            Address = AM2320_ADDR | 0x0001;
            /* Send the slave address */
            I2C1->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2C1->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return;
            }
            /* EV6_1: The acknowledge disable should be done just after EV6,
            that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
            ACK clearing */
            __disable_irq();
            /* Clear ADDR by reading SR2 register  */
            temp = I2C1->SR2;
            /* Clear ACK */
            I2C1->CR1 &= 0xFBFF;
            /*Re-enable IRQs */
            __enable_irq();
            /* Wait until BTF is set */
            while ((I2C1->SR1 & 0x00004) != 0x000004);
            /* Disable IRQs around STOP programming and data reading because of the limitation ?*/
            __disable_irq();
            /* Program the STOP */
            I2C_GenerateSTOP(I2C1, ENABLE);
            /* Read first data */
            *pBuffer = I2C1->DR;
            /* Re-enable IRQs */
            __enable_irq();
            /**/
            pBuffer++;
            /* Read second data */
            *pBuffer = I2C1->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2C1->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2C1->CR1  |= 0x0400;
            /* Clear POS bit */
            I2C1->CR1  &= 0xF7FF;

        }

        else

        {

            Timeout = 0xFFFF;
            /* Send START condition */
            I2C1->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5 */
            while ((I2C1->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Reset the address bit0 for write */
            //SlaveAddress |= OAR1_ADD0_Set;;
            Address = AM2320_ADDR | 0x0001;
            /* Send the slave address */
            I2C1->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2C1->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return;
            }
            /* Clear ADDR by reading SR2 status register */
            temp = I2C1->SR2;
            /* While there is data to be read */
            while (NumByteToRead)
            {
                /* Receive bytes from first byte until byte N-3 */
                if (NumByteToRead != 3)
                {
                    /* Poll on BTF to receive data because in polling mode we can not guarantee the
                    EV7 software sequence is managed before the current byte transfer completes */
                    while ((I2C1->SR1 & 0x00004) != 0x000004);
                    /* Read data */
                    *pBuffer = I2C1->DR;
                    /* */
                    pBuffer++;
                    /* Decrement the read bytes counter */
                    NumByteToRead--;
                }

                /* it remains to read three data: data N-2, data N-1, Data N */
                if (NumByteToRead == 3)
                {

                    /* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
                    while ((I2C1->SR1 & 0x00004) != 0x000004);
                    /* Clear ACK */
                    I2C1->CR1 &= CR1_ACK_Reset;

                    /* Disable IRQs around data reading and STOP programming because of the
                    limitation ? */
                    __disable_irq();
                    /* Read Data N-2 */
                    *pBuffer = I2C1->DR;
                    /* Increment */
                    pBuffer++;
                    /* Program the STOP */
                    I2C1->CR1 |= CR1_STOP_Set;
                    /* Read DataN-1 */
                    *pBuffer = I2C1->DR;
                    /* Re-enable IRQs */
                    __enable_irq();
                    /* Increment */
                    pBuffer++;
                    /* Wait until RXNE is set (DR contains the last data) */
                    while ((I2C1->SR1 & 0x00040) != 0x000040);
                    /* Read DataN */
                    *pBuffer = I2C1->DR;
                    /* Reset the number of bytes to be read by master */
                    NumByteToRead = 0;

                }
            }
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2C1->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2C1->CR1 |= CR1_ACK_Set;

        }	
	/*
     unsigned char s = 0x00;
		uint32_t counter = SMBus_Max_Delay_Cycles;
	    while (I2C_GetFlagStatus(SMBus_NAME, I2C_FLAG_BUSY) && counter) --counter;
    if(counter == 0) {
			s = 0x00;
	    return;
    }
		
     I2C_GenerateSTART(SMBus_NAME, ENABLE);
		
		    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
			s = 0x00;
	    return;
    }
		
     I2C_Send7bitAddress(SMBus_NAME, AM2320_ADDR, I2C_Direction_Receiver);
		
		    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
			s = 0x00;
	    return;
    }
     
     for(s = 0x00; s < length; s++)
     {
         value[s] = I2C_ReceiveData(SMBus_NAME);
			 
			     counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED) && counter) --counter;
    if(counter == 0) {
			s = 0x00;
	    return;
    }
     }
     
     I2C_GenerateSTOP(SMBus_NAME, ENABLE);
		 */
}

uint16_t AM2320_CRC16(unsigned char *ptr, unsigned char length)
{
      uint16_t crc = 0xFFFF;
      unsigned char s = 0x00;

      while(length--)
      {
        crc ^= *ptr++;
        for(s = 0; s < 8; s++)
        {
          if((crc & 0x01) != 0)
          {
            crc >>= 1;
            crc ^= 0xA001;
          }
          else
          {
            crc >>= 1;
          }
        }
      }

      return crc;
}

uint8_t AM2320_get_sensor_data(unsigned char start_address, unsigned char input_length, unsigned char output_length)
{
     unsigned char s = 0x00;
		 uint16_t recCRC, calcCRC;
	

     AM2320_wakeup();
     //Delay_10us(160);

     AM2320_buff[0] = 0x03;
     AM2320_buff[1] = start_address;
     AM2320_buff[2] = input_length;

     AM2320_write(AM2320_buff, 0x03);
     //Delay_10us(150);

     for(s = 0x00; s < output_length; s++)
     {
         AM2320_buff[s] = 0x00;
     }

     AM2320_read(AM2320_buff, output_length);
		 
		 //check CRC, if ok return 1 , else return 0
		 AM2320_get_CRC(&recCRC);
		 calcCRC=AM2320_CRC16(AM2320_buff,output_length-2);
		 if(calcCRC == recCRC)
			 return 1;
		 else
			 return 0;
		 
}


void AM2320_get_RH_and_temperature(uint16_t *data1, int16_t *data2)
{
	uint16_t temp;
  *data1 = ((uint16_t)((AM2320_buff[2] << 8) | AM2320_buff[3]));
  temp = ((AM2320_buff[4] << 8) | AM2320_buff[5]);
	if(temp & 0x8000)  //最高位为1，则是负温度，转化为有符号16bit数
		*data2 = -(temp&0x7FFF);
	else
		*data2 = temp;
}


void AM2320_get_CRC(uint16_t *CRC_data)
{
     *CRC_data = ((uint16_t)((AM2320_buff[7] << 8) | AM2320_buff[6]));
}

