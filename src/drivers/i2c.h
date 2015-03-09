/*******************************************************************************************
Inter-Integrated Circuit (I2C) Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __I2C_H__
#define __I2C_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define I2C_ADDR_READ_BIT		(BIT0)
#define I2C_SLAVE_READ(addr)	(addr | I2C_ADDR_READ_BIT)
#define I2C_SLAVE_WRITE(addr)	(addr)


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
	I2C_DEV0 = 0,
	I2C_DEV1,
	NUM_I2C_DEVS
} I2C_DEV;

typedef void (*PFN_I2C_CB)(I2C_DEV);
typedef struct
{
  PFN_I2C_CB doneCallback;
  PFN_I2C_CB errorCallback;
} I2C_CBS;

typedef struct
{
	UINT8	destAddr;		// 8-bit address, LSB is don't-care
	UINT8	txLen;
	void	const *pTxData;
} I2C_TRANSFER_PARAMS;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void I2cOpen(I2C_DEV devId, u32 clockRateHz, I2C_CBS const *pCallBacks);
void I2cClose(I2C_DEV devId);

// interrupt-based call which manages all bytes
void I2cWriteRead(I2C_DEV dev, I2C_TRANSFER_PARAMS const *pTransfer);
bool I2cIsBusy(I2C_DEV dev);
bool I2c0IsBusy(void);
bool I2c1IsBusy(void);

// Blocking (polling) call of all bytes
bool I2cWriteReadWait(I2C_DEV dev, I2C_TRANSFER_PARAMS const *pTransfer);
bool I2cWait(I2C_DEV dev);

void TWI0_IrqHandler(void);
void Twi1_IrqHandler(void);

#endif
