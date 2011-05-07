// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.  If
// not, see <http://www.gnu.org/licenses/>.

#include"openpr/bitstream/v4_devices.h"
/**
 * XC4VLX15 Part
 */
namespace openpr
{
namespace bitstream
{

const string xc4vlx15::xc4vlx15_name = "XC4VLX15";
const int
 xc4vlx15::xc4vlx15_id = 0x01658093U;
const tile_types
 xc4vlx15::xc4vlx15_row_layout[35] = {
    IOB,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    DSP48,
    CLB,
    CLB,
    CLB,
    CLB,
    IOB,
    GCLK,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    IOB,
};

/**
 * XC4VLX60 Part
 */
const string xc4vlx60::xc4vlx60_name = "XC4VLX60";
const int
 xc4vlx60::xc4vlx60_id = 0x016B4093U;
const tile_types
 xc4vlx60::xc4vlx60_row_layout[67] = {
	IOB,
	CLB,
	CLB,
	CLB,
	CLB,
	BRAM_INT,
	BRAM,
	CLB,
	CLB,
	CLB,
	CLB,
	BRAM_INT,
	BRAM,
	CLB,
	CLB,
	CLB,
	CLB,
	DSP48,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	IOB,
	GCLK,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	CLB,
	BRAM_INT,
	BRAM,
	CLB,
	CLB,
	CLB,
	CLB,
	BRAM_INT,
	BRAM,
	CLB,
	CLB,
	CLB,
	CLB,
	BRAM_INT,
	BRAM,
	CLB,
	CLB,
	CLB,
	CLB,
	IOB,
};

/**
 * XC4VLX60 Part
 */
const string xc4vfx60::xc4vfx60_name = "XC4VFX60";
const int
 xc4vfx60::xc4vfx60_id = 0x01EB4093U;
const tile_types
 xc4vfx60::xc4vfx60_row_layout[76] = {
    TRANSCV,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    IOB,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    DSP48,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    IOB,
    GCLK,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    DSP48,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    CLB,
    CLB,
    IOB,
    CLB,
    CLB,
    CLB,
    CLB,
    BRAM_INT,
    BRAM,
    CLB,
    CLB,
    TRANSCV,
};
}
}
