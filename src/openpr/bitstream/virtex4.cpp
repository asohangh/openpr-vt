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

#include"openpr/bitstream/virtex4.h"
namespace openpr
{
namespace bitstream
{

const int
 virtex4::virtex4_tile_frames[NUM_TILE_TYPES] = {
    30,				// IOB
    3,				// GCLK
    22,				// CLB
    21,				// DSP48
    64,				// BRAM
    20,				// BRAM_INT
    20,				// TRANSCV
    2					// PAD
};

const int
 virtex4::virtex4_block_type[NUM_TILE_TYPES] = {
    0,				// IOB
    0,				// GCLK
    0,				// CLB
    0,				// DSP48
    2,				// BRAM
    1,				// BRAM_INT
    0,				// TRANSCV
    0					// PAD
 };

const char* virtex4::virtex4_routing_table = "Virtex4Bits";
const char* virtex4::virtex4_logic_table = "Virtex4LogicBits";

int
virtex4::tile_offset(int x, int y)
{
  int byte_offset = 154 - (y%frame_height)*tile_width;
  if(y%frame_height >= frame_height/2)
    byte_offset -= 4;
  return byte_offset;
}

virtex4::~virtex4()
{
}
}
}
