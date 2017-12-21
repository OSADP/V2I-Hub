
#ifndef SRC_CCP_CCPREGISTERDEFINITIONS_H_
#define SRC_CCP_CCPREGISTERDEFINITIONS_H_

//Control Register Definitions
#define NOP 			(0x00 << 3)	//0000 NOP No operation 0x000
#define DAC_READBACK 	(0x01 << 3)	//0001 DAC readback Selects and enables DAC readback 0x000
#define ADC_SEQUENCE 	(0x02 << 3)	//0010 ADC sequence register Selects ADCs for conversion 0x000
#define GPC				(0x03 << 3)	//0011 General-purpose control register DAC and ADC control register 0x000
#define	ADC_PIN			(0x04 << 3)	//0100 ADC pin configuration Selects which pins are ADC inputs 0x000
#define DAC_PIN			(0x05 << 3)//0101 DAC pin configuration Selects which pins are DAC outputs 0x000
#define PULL_DOWN		(0x06 << 3)	//0110 Pull-down configuration Selects which pins have a 85 kO pull-down resistor to GND 0x0FF
#define READBACK		(0x07 << 3)	//0111 Readback and LDAC mode Selects the operation of the Load DAC (LDAC) function and/or
								//	which configuration register is read back

#define GPIO_WRITE_CNG 	(0x08 << 3)	//1000 GPIO write configuration1 Selects which pins are general-purpose outputs 0x000
#define GPIO_WRITE 		(0x09 << 3) 	//1001 GPIO write data Writes data to the general-purpose outputs 0x000
#define GPIO_READ_CFG 	(0x0A << 3) 	//1010 GPIO read configuration Selects which pins are general-purpose inputs 0x00
#define ENABLE_READBACK	(0x04)
#define PWR_DWN 		(0x0B << 3)	//1011 Power-down/reference control Powers down DACs and enables/disablesthe reference 0x000
#define GPIO_OPEN_DRAIN	(0x0C << 3)	//1100 GPIO open-drain configuration Selects open-drain or push/pull for general-purpose outputs 0x000
#define GPIO_3STATE 	(0x0D << 3)	//1101 Three-state configuration Selects which pins are three-state 0x000
//1110 Reserved Reserved
#define RESET 			(0x0F << 3) 	//1111 Software reset Resets the AD5592R/AD5592R-1 0x000

#endif  // SRC_CCP_CCPREGISTERDEFINITIONS_H_
