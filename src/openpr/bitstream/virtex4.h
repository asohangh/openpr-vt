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

#ifndef VIRTEX4_H
#define VIRTEX4_H

#include"openpr/bitstream/device.h"
namespace openpr
{
namespace bitstream
{
class virtex4:public device {
  protected:
    // Enum defining architecture parameters 
    enum { virtex4_frame_words = 41, virtex4_frame_height =
	    16, virtex4_clb_slices = 4, num_blk_types = 3
    };
    // Number of frames per tile type
    static const int virtex4_tile_frames[];
    // Block type for each Tile Type 
    static const int virtex4_block_type[];
		// Name of Virtex4 routing table in MySqlBits
		static const char* virtex4_routing_table;
		// Name of Virtex4 logic table in MySqlBits
		static const char* virtex4_logic_table;
		/**
		 * Virtex4 Class Constructor.
		 * Send V4 specific parameters to device class to be initialized
		 */
		virtex4(const int num_rows, const int num_cols,
			const tile_types row_layout[], const string name,
			const int id):device(num_rows, num_cols, row_layout, name, id,
					virtex4_frame_words,
					virtex4_frame_height,
					virtex4_tile_frames, 
					virtex4_block_type,
					virtex4_clb_slices, 
					num_blk_types,
					virtex4_routing_table,
					virtex4_logic_table) {
		};
		/**
		 * Return number of addressable block types for Virtex5.
		 */
		int get_addressable_blk_types()
		{
			return 2;
		};
		/**
		 * Return tile_offset within a frame (in bytes).
		 */
		int tile_offset(int x, int y);
		/**
		 * Return tile number within column when given byte offset.
		 * @param byte_offset The byte offset within the frame.
		 * @return Tile index within column (-1 if in ECC word).
		 */
		//virtual int byte_to_tile(int byte_offset);
		/**
		 * Return the type of interconnect tile.
		 */
		//const char* get_mysql_tile_type(int x);
		/**
		 * Virtex4 Class Destructor.
		 * Does nothing so far
		 * @todo Remove if not used
		 */
		~virtex4();
  public:
};
}
}
#endif
