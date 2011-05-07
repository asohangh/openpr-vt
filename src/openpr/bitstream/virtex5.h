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

#ifndef VIRTEX5_H
#define VIRTEX5_H

#include"openpr/bitstream/device.h"
namespace openpr
{
namespace bitstream
{

class virtex5:public device {
  protected:
    // Enum defining architecture parameters 
    enum { virtex5_frame_words = 41, virtex5_frame_height =
	    20, virtex5_clb_slices = 2, virtex5_num_blk_types = 2
    };
    // Number of frames per tile type
    static const int virtex5_tile_frames[];
    // Block type for each Tile Type 
    static const int virtex5_block_type[];
		// Name of Virtex5 routing table in MySqlBits
		static const char* virtex5_routing_table;
		// Name of Virtex5 logic table in MySqlBits
		static const char* virtex5_logic_table;
	/**
	 * Virtex4 Class Constructor.
	 * Send V4 specific parameters to device class to be initialized
	 */
     virtex5(const int num_rows, const int num_cols,
	     const tile_types row_layout[], const string name,
	     const int id):device(num_rows, num_cols, row_layout, name, id,
				  virtex5_frame_words,
				  virtex5_frame_height,
				  virtex5_tile_frames, virtex5_block_type,
				  virtex5_clb_slices,
				  virtex5_num_blk_types,
					virtex5_routing_table,
					virtex5_logic_table) {
    };
	/**
	 * Return number of addressable block types for Virtex5.
	 */
	int get_addressable_blk_types()
	{
		return 1;
	};
	/**
	 * Return byte offset of tile within frames.
	 * Only works for CLB Block Types
	 * @param x X coordinate of tile
	 * @param y Y coordinate of tile
	 * @return byte offset of tile within a frame
	 */
	virtual int tile_offset(int x,int y);
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
	~virtex5();
  public:
};
}
}
#endif
