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

#ifndef DEVICE_H
#define DEVICE_H

#include<string>
#include<vector>
#include<iostream>
#include<stdio.h>
#include"openpr/bitstream/architecture.h"
namespace openpr
{
namespace bitstream
{

using std::vector;

struct frame_addr;

class device {
	protected:
		const string name;		// Device Name
		const unsigned int id;	// Device Hex ID
		const int num_rows;		// Number of Clock Rows
		const int num_cols;		// Number of Columns Per Row
		const int frame_words;	// Number of words per frame 
		const int frame_height;	// Height of Frame in tiles 
		int tile_width;		// Width of tile within a frame (bytes)
		const int *tile_frames;	// Number of Columns Per Row
		const int *block_type;	// Block Type for each Tile Type 
		const int clb_slices;	// Number of Slices per tile 
		const int num_blk_types;	// Number of block types for Architecture 
		const tile_types *row_layout;	// Row Layout
		vector < tile_types >xdl_layout;	// XDL Coordinate Map
		int gclk_index;		// GLCK Index
		int *row_width;		// Store row widths for each Block Type
		/**
		 * Inline Constructor to initialize const values passed from subclass 
		 * constructors.
		 */
		device(const int num_rows, const int num_cols,
			const tile_types row_layout[], const string name, const int id,
			const int frame_words, const int frame_height,
			const int tile_frames[], const int block_type[],
			int clb_slices, int num_blk_types, const char* routing_table, const char* logic_table) : 
			name(name), id(id), num_rows(num_rows),	num_cols(num_cols), frame_words(frame_words), 
			frame_height(frame_height), tile_frames(tile_frames), block_type(block_type),
			clb_slices(clb_slices), num_blk_types(num_blk_types),	row_layout(row_layout),
			routing_table(routing_table), logic_table(logic_table)
		{
			int address_space = 0;
			row_width = new int[num_blk_types];
			for (int i = 0; i < num_blk_types; i++)
				row_width[i] = get_row_width(i);
			tile_width = ((frame_words-1)*4)/frame_height;
			for (int i=0;i<get_addressable_blk_types();i++)
				address_space += get_cfg_size(i);

		};
		/**
		 * Initialize xdl_layout vector to make looking up tile types of XDL 
		 * coordinates easier.
		 */
		void build_xdl_layout();
	public:
		const char* routing_table;
		const char* logic_table;
		/**
		 * Return chip ID.
		 */
		int get_chip_id();
		/**
		 * Return device name.
		 */
		string get_name() const { return name; };
		/**
		 * Return number of frame address rows in device.
		 */
		int get_num_rows() const { return num_rows; };
		/**
		 * Return Width of Row in Frames.
		 */
		int get_row_width(int type);
		/**
		 * Return height of chip in Tiles
		 */
		int chip_height();
		/**
		 * Return height of one clock region.
		 */
		int get_row_height() { return frame_height; };
		/**
		 * Return number of tile columns
		 */
		int chip_width();
		/**
		 * Return size of config block for block type (frames).
		 * @param type block type to get configuration size for
		 */
		int get_cfg_size(int type);
		/**
		 * Return size of configuration memory.
		 */
		int get_cfg_size();
		/** 
		 * Return the index of the GCLK tile.
		 */
		int get_gclk_index() { return gclk_index; };
		/**
		 * Return tile type given an XDL name X coordinate. 
		 * Due to overlaps in XDL style tile coordinates, cannot we distinguish
		 * between IOBs and GCLKs or BRAM_INTs/BRAMs
		 * @param x X Coordinate of Tile
		 */
		tile_types get_tile_type(int x);

		/**
		 * Return block type given an XDL name X coordinate. 
		 * Due to overlaps in XDL style tile coordinates, we cannot distinguish
		 * between IOBs and GCLKs or BRAM_INTs/BRAMs
		 * @param x X Coordinate of Tile
		 */
		int get_blk_type(int x);
		/**
		 * Return block type given a tiletype.
		 * @param tile_type Enumerated tile_type id.
		 */
		int get_blk_type(tile_types tile_type) { return block_type[tile_type]; };
		/**
		 * Return the number of addressable block types.
		 */
		virtual int get_addressable_blk_types();

		/**
		 * Translate XDL Style Tile Coordinate to Major Frame Address.
		 * Due to overlaps in XDL style tile coordinates, cannot we distinguish
		 * between IOBs and GCLKs or BRAM_INTs/BRAMs
		 * @param x X Coordinate of Tile
		 * @param y Y Coordinate of Tile
		 */
		frame_addr tilecoord_to_major(int x, int y);
		/**
		 * Translate BRAM XDL Style Tile coordinate to Major Frame Address.
		 * @param x X Coordinate of Tile
		 * @param y Y Coordinate of Tile
		 * @todo Change this so it isn't a dirty hack
		 */
		frame_addr bramcoord_to_major(int x, int y);

		/**
		 * Return offset of frame within bitstream.
		 * Currently working for CLB Block Types and BRAM INT Type
		 * @param frame_address Frame address to be located
		 */
		int frame_offset(frame_addr frame_address);

		/**
		 * Return byte offset of tile within frames.
		 * Only works for CLB Block Types
		 * @param x X coordinate of tile
		 * @param y Y coordinate of tile
		 * @return byte offset of tile within a frame
		 */
		virtual int tile_offset(int x,int y) = 0;
		/**
		 * Return tile number within column when given byte offset.
		 * @param byte_offset The byte offset within the frame.
		 * @return Tile index within column (-1 if in ECC word).
		 */

		int get_tile_frames(int x);
		/**
		 * Return number of frames that this tile spans in bitstream.
		 * @param tile_type Tile type enum.
		 * @return number of frames that this tile spans in bitstream.
		 */
		int get_tile_frames(tile_types tile_type) { return tile_frames[tile_type]; };
		/**
		 * Return the number of words per frame for the architecture.
		 * @return Number of words per frame
		 */
		inline int get_frame_words(void) { return frame_words; }
		/**
		 * Destructor frees any dynamically allocated memory.
		 */
	  ~device()
	 	{
			delete[](row_width);
			row_width = NULL;	//Set to NULL to indicate deletion
    }
};
}
}

#endif
