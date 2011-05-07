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

#include"openpr/bitstream/device.h"
namespace openpr
{
	namespace bitstream
	{
		int device::get_chip_id()
		{
			return id;
		}
		int device::get_row_width(int type)
		{
			int i;
			int width = 0;
			for (i = 0; i < num_cols; i++) {
				if (block_type[row_layout[i]] == type)
				width += tile_frames[row_layout[i]];
			}
			width += 2;
			return width;
		}

		int device::chip_height()
		{
			return num_rows*frame_height;
		}
		int device::chip_width()
		{
			return xdl_layout.size();
		}

		int
		device::get_cfg_size()
		{
			int cfg_size = 0;
			for(int i=0;i<num_blk_types;i++)
			cfg_size += get_cfg_size(i);
			return cfg_size;
		}

		int device::get_cfg_size(int type)
		{
			return row_width[type]*num_rows;
		}
		void device::build_xdl_layout()
		{
			int i;
			for (i = 0; i < num_cols; i++)
			{
				if (row_layout[i] != BRAM && row_layout[i] != GCLK)
				xdl_layout.push_back(row_layout[i]);
				if (row_layout[i] == GCLK)
				{
					gclk_index = xdl_layout.size() - 1;
					if(false) {printf("TT of gclk_index(%d) is %d\n", gclk_index, xdl_layout[gclk_index]);}
				}
			}
		}

		tile_types device::get_tile_type(int x)
		{
			if ((size_t)x < xdl_layout.size())
			return xdl_layout[x];
			else
			return NUM_TILE_TYPES;
		}

		int device::get_blk_type(int x)
		{
			int tile_type = get_tile_type(x);
			if(tile_type != NUM_TILE_TYPES)
			return block_type[tile_type];
			return -1;
		}

		int
		device::get_addressable_blk_types()
		{
			return num_blk_types;
		}

		frame_addr device::tilecoord_to_major(int x, int y)
		{
			struct frame_addr frame_address;
			int half_rows = num_rows / 2;
			int i;
			int tile_type = get_tile_type(x);

			if (tile_type == NUM_TILE_TYPES)
			{
				fprintf(stderr, "Invalid tile_type for tilecoord_to_major\n");
				frame_address.type = -1;
				return frame_address;
			}
			if ((size_t)x > xdl_layout.size()) {
				fprintf(stderr, "Invalid x coordinate for tilecoord_to_major\n");
				frame_address.type = -1;
				return frame_address;
			}
			if (y > num_rows * frame_height) {
				fprintf(stderr, "Invalid y coordinate %d for tilecoord_to_major\n",
						num_rows);
				frame_address.type = -1;
				return frame_address;
			}
			// Find Frame Row by dividing y/row_height
			frame_address.row = y / frame_height;

			if (frame_address.row + 1 > num_rows / 2) {
				frame_address.tb = 0;
				frame_address.row = frame_address.row - half_rows;
			} else {
				frame_address.tb = 1;
				frame_address.row = (half_rows - frame_address.row) - 1;
			}
			// Iterate over xdl tile layout and find column
			frame_address.col = 0;
			for (i = 0; i < x; i++) {
				if (block_type[xdl_layout[i]] == block_type[tile_type])
				frame_address.col++;
			}
			// Skip a frame address for GCLK tile which does not have its own coord
			//      if(x > xdl_layout.size()/2 && block_type[tile_type] == block_type[GCLK])
			//              frame_address.col++;
			//    printf("x is %d\n", x);
			if (x > gclk_index && block_type[tile_type] == block_type[GCLK])
			frame_address.col++;
			frame_address.mna = 0;
			frame_address.type = block_type[tile_type];
			return frame_address;
		}

		frame_addr device::bramcoord_to_major(int x, int y)
		{
			frame_addr frame_address = tilecoord_to_major(x,y);
			if(get_tile_type(x) != BRAM_INT)
			{
				fprintf(stderr, "Invalid tile_type for bramcoord_to_major\n");
				frame_address.type = -1;
				return frame_address;
			}

			// Iterate over xdl tile layout and find column
			frame_address.col = 0;
			for (int i = 0; i < x; i++) {
				if (xdl_layout[i] == BRAM_INT)
				frame_address.col++;
			}
			frame_address.type = block_type[BRAM];
			return frame_address;
		}

		int device::frame_offset(frame_addr frame_address)
		{
			int i;
			int half_rows = num_rows / 2;
			int frames = 0;

			if (frame_address.type > num_blk_types) {
				fprintf(stderr, "Unknown block type, please report bitstream\n");
				return -1;
			}
			if (frame_address.row > half_rows) {
				fprintf(stderr, "Invalid number of rows for this device\n");
				return -1;
			}
			//TODO: Fix this to check number of FAR cols, not xdlrc cols
			if (frame_address.col > num_cols) {
				fprintf(stderr, "Invalid number of cols for this device\n");
				return -1;
			}
			// Muliply row*row_width
			if (frame_address.tb == 0)
			frames += frame_address.row * row_width[frame_address.type];
			else
			frames +=
			(frame_address.row + half_rows) * row_width[frame_address.type];
			// Iterate over row_layout and increment frame address if tiles of type
			// frame_address.type are found
			i = 0;
			while (i < frame_address.col) {
				if (block_type[row_layout[i]] == frame_address.type)
				frames += tile_frames[row_layout[i]];
				else
				frame_address.col++;
				i++;
			}

			return frames;
		}

		int
		device::get_tile_frames(int x)
		{
			if(get_tile_type(x) == NUM_TILE_TYPES)
			return -1;
			else
			return tile_frames[get_tile_type(x)];
		}

	}
}
