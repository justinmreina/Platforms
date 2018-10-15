/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * This is a module specific configuration file for
 * the RD-88MW300 module based on schematic dated 19th June 2014.
 */

#include <wmtypes.h>
#include <wmerrno.h>
#include <wm_os.h>
#include <board.h>
#include <lowlevel_drivers.h>

int board_main_xtal()
{
	/* MAINXTAL: 38.4MHZ */
	return 38400000;
}

int board_main_osc()
{
	return -WM_FAIL;
}

int board_antenna_switch_ctrl()
{
	return true;
}

struct pwr_table *board_region_pwr_tbl(board_country_code_t country)
{
	return NULL;
}
