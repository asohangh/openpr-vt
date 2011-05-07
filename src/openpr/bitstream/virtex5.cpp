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

#include"openpr/bitstream/virtex5.h"
namespace openpr
{
namespace bitstream
{

const int
 virtex5::virtex5_tile_frames[NUM_TILE_TYPES] = {
    54,				// IOB
    4,				// GCLK
    36,				// CLB
    28,				// DSP48
    128,			// BRAM
    30,				// BRAM_INT
    32,				// TRANSCV
    2				// PAD
};

const int
 virtex5::virtex5_block_type[NUM_TILE_TYPES] = {
    0,				// IOB
    0,				// GCLK
    0,				// CLB
    0,				// DSP48
    1,				// BRAM
    0,				// BRAM_INT
    0,				// TRANSCV
    0				// PAD
};

const char* virtex5::virtex5_routing_table = "Virtex5Bits";
const char* virtex5::virtex5_logic_table = "Virtex5LogicBits";

int
virtex5::tile_offset(int x, int y)
{
  int byte_offset = (y%frame_height)*tile_width;
  if(y%frame_height >= frame_height/2)
    byte_offset += 4;
  return byte_offset;
}


virtex5::~virtex5()
{
}
}
}
