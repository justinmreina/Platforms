/*
 * Original: MMA7760.h
 * Library for accelerometer_MMA7760
 *
 * Copyright (c) 2013 seeed technology inc.
 * Author	:	FrankieChu
 * Create Time	:	Jan 2013
 * Change Log	:	Converted to c file, renamed
 *			and updated for AWS sensor integration
 * Changed by	:	Prafulla Wadaskar, Marvell International Ltd.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <wm_os.h>
#include <wmstdio.h>
#include <wmtime.h>
#include <wmsdk.h>
#include <board.h>
#include <mdev_gpio.h>
#include <mdev_pinmux.h>
#include <lowlevel_drivers.h>
#include <math.h>
#include <sensor_acc_drv.h>



#include <wm_os.h>
#include <mdev_i2c.h>

/*------------------Macro Definitions ------------------*/
#define BUF_LEN		16
#define I2X_WR_DLY	50
#define MMA7660TIMEOUT	500	/* us */

/*------------------Global Variables -------------------*/
static mdev_t *i2c0;
static uint8_t read_data[BUF_LEN];
static uint8_t write_data[BUF_LEN];
static struct MMA7660_LOOKUP accLookup[64];
int8_t prev_x=0;
int8_t prev_y=0;
int8_t prev_z=0;
/*
 *********************************************************
 **** Accelerometer Sensor H/W Specific code
 **********************************************************
 */

/*Function: Writes a byte to the register of the MMA7660*/
void MMA7660_write(uint8_t _register, uint8_t _data)
{
	if (!i2c0)
		return;

	write_data[0] = _register;
	write_data[1] = _data;
	i2c_drv_write(i2c0, write_data, 2);
	os_thread_sleep(I2X_WR_DLY);
}

/*Function: Writes only register byte of the MMA7660 */
void MMA7660_From(uint8_t _register)
{
	if (!i2c0)
		return;

	write_data[0] = _register;
	i2c_drv_write(i2c0, write_data, 1);
}

/*Function: Read a byte from the regitster of the MMA7660*/
uint8_t MMA7660_read(uint8_t _register)
{
	if (!i2c0)
		return 0;

	write_data[0] = _register;
	i2c_drv_write(i2c0, write_data, 1);
	i2c_drv_read(i2c0, read_data, 1);
	return read_data[0];
}

/* Populate lookup table based on the MMA7660 datasheet at
	http://www.farnell.com/datasheets/1670762.pdf
*/
void MMA7660_initAccelTable() {
	int i;
	float val, valZ;

	for (i = 0, val = 0; i < 32; i++) {
		accLookup[i].g = val;
		val += 0.047;
	}

	for (i = 63, val = -0.047; i > 31; i--) {
		accLookup[i].g = val;
		val -= 0.047;
	}

	for (i = 0, val = 0, valZ = 90; i < 22; i++) {
		accLookup[i].xyAngle = val;
		accLookup[i].zAngle = valZ;

		val += 2.69;
		valZ -= 2.69;
	}

	for (i = 63, val = -2.69, valZ = -87.31; i > 42; i--) {
		accLookup[i].xyAngle = val;
		accLookup[i].zAngle = valZ;

		val -= 2.69;
		valZ += 2.69;
	}

	for (i = 22; i < 43; i++) {
		accLookup[i].xyAngle = 255;
		accLookup[i].zAngle = 255;
	}
}

void MMA7660_setMode(uint8_t mode) {
	MMA7660_write(MMA7660_MODE,mode);
}

void MMA7660_setSampleRate(uint8_t rate) {
	MMA7660_write(MMA7660_SR,rate);
}

void MMA7660_init(mdev_t *i2c_handle)
{
	i2c0 = i2c_handle;
	MMA7660_initAccelTable();
	MMA7660_setMode(MMA7660_STAND_BY);
	MMA7660_setSampleRate(AUTO_SLEEP_64);
	/* MMA7660_write(MMA7660_INTSU, interrupts); */
	MMA7660_setMode(MMA7660_ACTIVE);
}

/* Function: Get the contents of the registers in the MMA7660
	so as to calculate the acceleration.
*/
bool MMA7660_getXYZ(int8_t *x,int8_t *y,int8_t *z)
{
	int8_t val[3];
	val[0] = val[1] = val[2] = 64;

	if (!i2c0)
		return 0;

	MMA7660_From(0);
	i2c_drv_read(i2c0, (uint8_t *)val, 3);

	/* Abstracting signed int8_t value from 6bit signed read
		value from device, range of input value is -31 to +31 */
	*x = val[0] & 0x1f;
	*y = val[1] & 0x1f;
	*z = val[2] & 0x1f;
	if (val[0] & (1 << 5))
		*x -= 32;
	if (val[1] & (1 << 5))
		*y -= 32;
	if (val[2] & (1 << 5))
		*z -= 32;

	/*wmprintf("%s POS, %2d(%02x), %2d(%02x), %2d(%02x)\r\n",
		__FUNCTION__, *x, val[0], *y, val[1], *z, val[2]);*/
	return 1;
}

