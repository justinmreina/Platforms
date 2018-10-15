/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * This is a module specific configuration file for AW-CU300
 * based on schematic as of 8 Jun 2015.
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

uint8_t FCC_data[14][8] = {
	{1, 15, 12, 12, 12, 10, 10, 10},
	{2, 15, 12, 12, 12, 10, 10, 10},
	{3, 15, 12, 12, 12, 10, 10, 10},
	{4, 17, 14, 14, 14, 13, 13, 13},
	{5, 17, 14, 14, 14, 13, 13, 13},
	{6, 17, 14, 14, 14, 13, 13, 13},
	{7, 17, 14, 14, 14, 13, 13, 13},
	{8, 17, 14, 14, 14, 13, 13, 13},
	{9, 15, 12, 12, 12, 11, 11, 11},
	{10, 15, 12, 12, 12, 11, 11, 11},
	{11, 15, 12, 12, 12, 11, 11, 11},
};

uint8_t EU_data[14][8] = {
	{1, 13, 13, 13, 13, 13, 13, 13},
	{2, 13, 13, 13, 13, 13, 13, 13},
	{3, 13, 13, 13, 13, 13, 13, 13},
	{4, 13, 13, 13, 13, 13, 13, 13},
	{5, 13, 13, 13, 13, 13, 13, 13},
	{6, 13, 13, 13, 13, 13, 13, 13},
	{7, 13, 13, 13, 13, 13, 13, 13},
	{8, 13, 13, 13, 13, 13, 13, 13},
	{9, 13, 13, 13, 13, 13, 13, 13},
	{10, 13, 13, 13, 13, 13, 13, 13},
	{11, 13, 13, 13, 13, 13, 13, 13},
	{12, 13, 13, 13, 13, 13, 13, 13},
	{13, 13, 13, 13, 13, 13, 13, 13},
};

uint8_t JAPAN_data[14][8] = {
	{1, 16, 14, 14, 14, 13, 13, 13},
	{2, 16, 14, 14, 14, 13, 13, 13},
	{3, 16, 14, 14, 14, 13, 13, 13},
	{4, 16, 14, 14, 14, 13, 13, 13},
	{5, 16, 14, 14, 14, 13, 13, 13},
	{6, 16, 14, 14, 14, 13, 13, 13},
	{7, 16, 14, 14, 14, 13, 13, 13},
	{8, 16, 14, 14, 14, 13, 13, 13},
	{9, 16, 14, 14, 14, 13, 13, 13},
	{10, 16, 14, 14, 14, 13, 13, 13},
	{11, 16, 14, 14, 14, 13, 13, 13},
	{12, 16, 14, 14, 14, 13, 13, 13},
	{13, 16, 14, 14, 14, 13, 13, 13},
};

static struct pwr_table pt[] = {
	{
		BOARD_COUNTRY_US,
		11,
		(const uint8_t **)FCC_data
	},
	{
		BOARD_COUNTRY_EU,
		13,
		(const uint8_t **)EU_data
	},
	{
		BOARD_COUNTRY_JP,
		13,
		(const uint8_t **)JAPAN_data
	}
};

struct pwr_table *board_region_pwr_tbl(board_country_code_t country)
{
	switch (country) {
	case BOARD_COUNTRY_US:
		return &pt[0];
	case BOARD_COUNTRY_CA:
	case BOARD_COUNTRY_SG:
		return NULL;
	case BOARD_COUNTRY_EU:
		return &pt[1];
	case BOARD_COUNTRY_AU:
	case BOARD_COUNTRY_KR:
	case BOARD_COUNTRY_FR:
		return NULL;
	case BOARD_COUNTRY_JP:
		return &pt[2];
	case BOARD_COUNTRY_CN:
		return NULL;
	default:
		return &pt[0];
	}
}
