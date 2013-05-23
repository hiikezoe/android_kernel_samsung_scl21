/* filename: ISSP_Driver_Routines.c */
#include "issp_revision.h"
#ifdef PROJECT_REV_304
/*
* Copyright 2006-2007, Cypress Semiconductor Corporation.

* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
* MA  02110-1301, USA.
*/

/*
#include <m8c.h>
#include "PSoCAPI.h"
*/
#include <mach/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/module.h>
#include <linux/ethtool.h>
#include <mach/msm8960-gpio.h>

#include "issp_defs.h"
#include "issp_errors.h"
#include "issp_directives.h"
#include "issp_extern.h"
#define _CYPRESS_TKEY_FW_H
#include "cypress_tkey_fw.h"

#define SECURITY_DATA	0x00

#define SCLK_PIN    0x20   /* p2_5 */
#define SDATA_PIN   0x08   /* p2_3 */
#define XRES_PIN    0x10   /* p1_5 */
#define TARGET_VDD  0x08   /* p1_3 */

void InitTargetTestData(unsigned char bBlockNum, unsigned char bBankNum)
{
    /* create unique data for each block */
	int dataNum = 0;
	for (dataNum = 0; dataNum < TARGET_DATABUFF_LEN; dataNum++) {
		abTargetDataOUT[dataNum] =
		    firmware_data[bBlockNum * TARGET_DATABUFF_LEN + dataNum];
	}
}

 /*============================================================================
 LoadArrayWithSecurityData()
 !!!!!!!!!!!!!!!!!!FOR TEST!!!!!!!!!!!!!!!!!!!!!!!!!!
 PROCESSOR_SPECIFIC
 Most likely this data will be fed to the Host by some other means, ie: I2C,
 RS232, etc., or will be fixed in the host. The security data should come
 from the hex file.
   bStart  - the starting byte in the array for loading data
   bLength - the number of byte to write into the array
   bType   - the security data to write over the range defined by bStart and
			bLength
 ============================================================================
*/
void LoadArrayWithSecurityData(unsigned char bStart,
			unsigned char bLength, unsigned char bType)
{
    /* Now, write the desired security-bytes for the range specified */
	for (bTargetDataPtr = bStart;
			bTargetDataPtr < bLength; bTargetDataPtr++)
		abTargetDataOUT[bTargetDataPtr] = bType;
}


void Delay(unsigned int n)
{
	while (n) {
		asm("nop");
		n -= 1;
	}
}

 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 LoadProgramData()
 The final application should load program data from HEX file generated by
 PSoC Designer into a 64 byte host ram buffer.
    1. Read data from next line in hex file into ram buffer. One record
      (line) is 64 bytes of data.
    2. Check host ram buffer + record data (Address, # of bytes) against hex
       record checksum at end of record line
    3. If error reread data from file or abort
    4. Exit this Function and Program block or verify the block.
 This demo program will, instead, load predetermined data into each block.
 The demo does it this way because there is no comm link to get data.
 ****************************************************************************
*/
void LoadProgramData(unsigned char bBankNum, unsigned char bBlockNum)
{
 /*   >>> The following call is for demo use only. <<<
     Function InitTargetTestData fills buffer for demo
*/
	int dataNum;
	for (dataNum = 0; dataNum < TARGET_DATABUFF_LEN; dataNum++) {
		abTargetDataOUT[dataNum] =
		 firmware_data[bBlockNum * TARGET_DATABUFF_LEN + dataNum];
	}

}


/*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 fLoadSecurityData()
 Load security data from hex file into 64 byte host ram buffer. In a fully
 functional program (not a demo) this routine should do the following:
    1. Read data from security record in hex file into ram buffer.
    2. Check host ram buffer + record data (Address, # of bytes) against hex
       record checksum at end of record line
    3. If error reread security data from file or abort
    4. Exit this Function and Program block
 In this demo routine, all of the security data is set to unprotected (0x00)
 and it returns.
 This function always returns PASS. The flag return is reserving
 functionality for non-demo versions.
 ****************************************************************************
*/
signed char fLoadSecurityData(unsigned char bBankNum)
{
/* >>> The following call is for demo use only. <<<
    Function LoadArrayWithSecurityData fills buffer for demo
*/
	LoadArrayWithSecurityData(0, SecurityBytesPerBank, SECURITY_DATA);
    /* PTJ: 0x1B (00 01 10 11) is more interesting security data
		than 0x00 for testing purposes */

    /* Note:
     Error checking should be added for the final version as noted above.
    For demo use this function just returns PASS. */
	return PASS;
}


/*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 fSDATACheck()
 Check SDATA pin for high or low logic level and return value to calling
 routine.
 Returns:
     0 if the pin was low.
     1 if the pin was high.
 ****************************************************************************
*/
unsigned char fSDATACheck(void)
{
#ifdef PSOC3_5
	if (gpio_get_value(GPIO_TOUCHKEY_SDA))
		return 1;
	else
		return 0;
#else
	if (gpio_get_value(GPIO_TOUCHKEY_SDA))
		return 1;
	else
		return 0;
#endif
}


/*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SCLKHigh()
 Set the SCLK pin High
 ****************************************************************************
*/
void SCLKHigh(void)
{
	gpio_direction_output(GPIO_TOUCHKEY_SCL, 1);
}


 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SCLKLow()
 Make Clock pin Low
 ****************************************************************************
*/
void SCLKLow(void)
{
	gpio_direction_output(GPIO_TOUCHKEY_SCL, 0);
}

#ifndef RESET_MODE
 /*Only needed for power cycle mode
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetSCLKHiZ()
 Set SCLK pin to HighZ drive mode.
 ****************************************************************************
*/
void SetSCLKHiZ(void)
{
	gpio_direction_input(GPIO_TOUCHKEY_SCL);
}
#endif

 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetSCLKStrong()
 Set SCLK to an output (Strong drive mode)
 ****************************************************************************
*/
void SetSCLKStrong(void)
{
	gpio_direction_output(GPIO_TOUCHKEY_SCL, 0);
}


 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetSDATAHigh()
 Make SDATA pin High
 ****************************************************************************
*/
void SetSDATAHigh(void)
{
	gpio_direction_output(GPIO_TOUCHKEY_SDA, 1);
}

 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetSDATALow()
 Make SDATA pin Low
 ****************************************************************************
*/
void SetSDATALow(void)
{
	gpio_direction_output(GPIO_TOUCHKEY_SDA, 0);
}

 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetSDATAHiZ()
 Set SDATA pin to an input (HighZ drive mode).
 ****************************************************************************
*/
void SetSDATAHiZ(void)
{
	gpio_direction_input(GPIO_TOUCHKEY_SDA);
}

 /* 0BIT 1BIT
  10 :strong, 11:open drain low, 00:pull-up,01:hi-z
*/

void SetSDATAOpenDrain(void)
{

}

/*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetSDATAStrong()
 Set SDATA for transmission (Strong drive mode) -- as opposed to being set to
 High Z for receiving data.
 ****************************************************************************
*/
void SetSDATAStrong(void)
{
	gpio_direction_output(GPIO_TOUCHKEY_SDA, 0);
}

#ifdef RESET_MODE
 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetXRESStrong()
 Set external reset (XRES) to an output (Strong drive mode).
 ****************************************************************************
*/
void SetXRESStrong(void)
{

}

 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 AssertXRES()
 Set XRES pin High
 ****************************************************************************
*/
void AssertXRES(void)
{

}

/*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 DeassertXRES()
 Set XRES pin low.
 ****************************************************************************
*/
void DeassertXRES(void)
{

}
#else

 /*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 SetTargetVDDStrong()
 Set VDD pin (PWR) to an output (Strong drive mode).
 ****************************************************************************
*/
void SetTargetVDDStrong(void)
{

}

/*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 ApplyTargetVDD()
 Provide power to the target PSoC's Vdd pin through a GPIO.
 ****************************************************************************
*/
#ifdef CONFIG_KEYBOARD_CYPRESS_TOUCH_236
static void cypress_power_onoff(int onoff)
{
	int ret, rc;
	static struct regulator *reg_l29, *reg_l10;

	if (!reg_l29) {
		reg_l29 = regulator_get(NULL, "8921_l29");

		if (IS_ERR(reg_l29)) {
			pr_err("could not get 8921_l29, rc = %ld\n",
				PTR_ERR(reg_l29));
			return;
		}
		ret = regulator_set_voltage(reg_l29, 1800000, 1800000);
	}

	if (!reg_l10) {
		reg_l10 = regulator_get(NULL, "8921_l10");
		ret = regulator_set_voltage(reg_l10, 3000000, 3000000);

		if (IS_ERR(reg_l10)) {
			pr_err("could not get 8921_l10, rc = %ld\n",
				PTR_ERR(reg_l10));
			return;
		}
	}

	if (onoff) {
		ret = regulator_enable(reg_l29);
		rc =  regulator_enable(reg_l10);
		if (ret) {
			pr_err("enable l29failed, rc=%d\n", ret);
			return;
		}
		if (rc) {
			pr_err("enable l10 failed, rc=%d\n", ret);
			return;
		}
		pr_info("cypress_power_on is finished.\n");
	} else {
		ret = regulator_disable(reg_l29);
		rc =  regulator_disable(reg_l10);
		if (ret) {
			pr_err("disable l29 failed, rc=%d\n", ret);
			return;
		}
		if (rc) {
			pr_err("enable l29failed, rc=%d\n", ret);
			return;
		}
		pr_info("cypress_power_off is finished.\n");
	}
}

#else

#if defined(CONFIG_MACH_SUPERIORLTE_SKT)
static void cypress_power_onoff(int onoff)
{
	int ret, rc, rt;
	static struct regulator *reg_l29, *reg_l10, *reg_l11;

	if (!reg_l29) {
		reg_l29 = regulator_get(NULL, "8921_l29");
		ret = regulator_set_voltage(reg_l29, 1800000, 1800000);

		if (IS_ERR(reg_l29)) {
			pr_err("could not get 8921_l29, ret = %ld\n",
				PTR_ERR(reg_l29));
			return;
		}
	}

	if (!reg_l10) {
		reg_l10 = regulator_get(NULL, "8921_l10");
		ret = regulator_set_voltage(reg_l10, 3000000, 3000000);

		if (IS_ERR(reg_l10)) {
			pr_err("could not get 8921_l10, ret = %ld\n",
				PTR_ERR(reg_l10));
			return;
		}
	}

	if (!reg_l11) {
		reg_l11 = regulator_get(NULL, "8921_l11");
		ret = regulator_set_voltage(reg_l11, 3000000, 3000000);

		if (IS_ERR(reg_l11)) {
			pr_err("could not get 8921_l11, ret = %ld\n",
				PTR_ERR(reg_l11));
			return;
		}
	}

	if (onoff) {
		ret = regulator_enable(reg_l29);
		rc =  regulator_enable(reg_l10);
		rt =  regulator_enable(reg_l11);
		if (ret) {
			pr_err("enable l29 failed, ret=%d\n", ret);
			return;
		}
		if (rc) {
			pr_err("enable l10 failed, rc=%d\n", rc);
			return;
		}
		if (rt) {
			pr_err("enable l11 failed, rt=%d\n", rt);
			return;
		}
		pr_info("cypress_power_on is finished.\n");
	} else {
		rc =  regulator_disable(reg_l10);
		rt =  regulator_disable(reg_l11);
		if (rc) {
			pr_err("enable l10 failed, rc=%d\n", rc);
			return;
		}
		if (rt) {
			pr_err("enable l11 failed, rc=%d\n", rt);
			return;
		}
		pr_info("cypress_power_off is finished.\n");
	}
}
#elif defined(CONFIG_MACH_EXPRESS)
static void cypress_power_onoff(int onoff)
{
	int ret;
	static struct regulator *reg_l17;
	static struct regulator *reg_lvs6;
	static struct regulator *reg_l29;

	if (system_rev < BOARD_REV06) {
		if (!reg_lvs6) {
			reg_lvs6 = regulator_get(NULL, "8921_lvs6");
			if (IS_ERR(reg_lvs6)) {
				pr_err("could not get 8921_lvs6, ret = %ld\n",
					PTR_ERR(reg_lvs6));
				return;
			}
		}

		if (onoff) {
			ret = regulator_enable(reg_lvs6);
			if (ret) {
				pr_err("enable lvs6 failed, ret=%d\n", ret);
				return;
			}
			pr_info("cypress_1.8V on is finished.\n");
		} else {
			ret = regulator_disable(reg_lvs6);
			if (ret) {
				pr_err("disable lvs6 failed, ret=%d\n", ret);
				return;
			}
			pr_info("cypress_1.8V off is finished.\n");
		}

		if (!reg_l17) {
			reg_l17 = regulator_get(NULL, "8921_l17");
			if (IS_ERR(reg_l17)) {
				pr_err("could not get 8921_l17, ret = %ld\n",
					PTR_ERR(reg_l17));
				return;
			}

			ret = regulator_set_voltage(reg_l17, 3300000, 3300000);
			if (ret) {
				pr_err("%s: unable to set ldo17 voltage to 3.3V\n",
					__func__);
				return;
			}
		}

		if (onoff) {
			ret = regulator_enable(reg_l17);
			if (ret) {
				pr_err("enable l17 failed, ret=%d\n", ret);
				return;
			}
			pr_info("cypress_power_on is finished.\n");
		} else {
			ret = regulator_disable(reg_l17);
			if (ret) {
				pr_err("disable l17 failed, ret=%d\n", ret);
				return;
			}
			pr_info("cypress_power_off is finished.\n");
		}

	} else {
		if (!reg_l29) {
			reg_l29 = regulator_get(NULL, "8921_l29");
			if (IS_ERR(reg_l29)) {
				pr_err("could not get 8921_l29, ret = %ld\n",
					PTR_ERR(reg_l29));
				return;
			}

			ret = regulator_set_voltage(reg_l29, 1800000, 1800000);
			if (ret) {
				pr_err("%s: unable to set ldo17 voltage to 1.8V\n",
					__func__);
				return;
			}
		}

		if (onoff) {
			ret = regulator_enable(reg_l29);
			if (ret) {
				pr_err("enable l29 failed, ret=%d\n", ret);
				return;
			}
			gpio_direction_output(GPIO_TKEY_LED, 1);
			pr_info("cypress_power_on is finished.\n");
		} else {
			ret = regulator_disable(reg_l29);
			if (ret) {
				pr_err("disable l29 failed, ret=%d\n", ret);
				return;
			}
			gpio_direction_output(GPIO_TKEY_LED, 0);
			pr_info("cypress_power_off is finished.\n");
		}
	}
}
#else
static void cypress_power_onoff(int onoff)
{
		int ret;
	static struct regulator *reg_lvs5;

	if (!reg_lvs5) {
		reg_lvs5 = regulator_get(NULL, "8921_lvs5");
		if (IS_ERR(reg_lvs5)) {
			pr_err("could not get 8921_lvs5, rc = %ld\n",
				PTR_ERR(reg_lvs5));
				return;
			}
		}

		if (onoff) {
			ret = regulator_enable(reg_lvs5);
			if (ret) {
				pr_err("enable lvs5 failed, rc=%d\n", ret);
				return;
			}
		pr_info("cypress_power_on is finished.\n");
		} else {
			ret = regulator_disable(reg_lvs5);
			if (ret) {
				pr_err("disable lvs5 failed, rc=%d\n", ret);
				return;
			}
		pr_info("cypress_power_off is finished.\n");
	}

	if (onoff)
		gpio_direction_output(GPIO_TOUCH_KEY_EN, 1);
	else
		gpio_direction_output(GPIO_TOUCH_KEY_EN, 0);
}
#endif
#endif
void ApplyTargetVDD(void)
{
	gpio_direction_input(GPIO_TOUCHKEY_SDA);
	gpio_direction_input(GPIO_TOUCHKEY_SCL);
	cypress_power_onoff(1);
}

/*
 ********************* LOW-LEVEL ISSP SUBROUTINE SECTION ********************
 ****************************************************************************
 ****                        PROCESSOR SPECIFIC                          ****
 ****************************************************************************
 ****                      USER ATTENTION REQUIRED                       ****
 ****************************************************************************
 RemoveTargetVDD()
 Remove power from the target PSoC's Vdd pin.
 ****************************************************************************
*/
void RemoveTargetVDD(void)
{
	cypress_power_onoff(0);
}
#endif


#endif  /* (PROJECT_REV_) */
/* end of file ISSP_Drive_Routines.c */
