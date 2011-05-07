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

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H
#include<string>
#include<boost/lexical_cast.hpp>
namespace openpr{

namespace bitstream
{

using std::string;

// Enumerate possible tile types for V4/V5
enum tile_types {
	IOB,
	GCLK,
	CLB,
	DSP48,
	BRAM,
	BRAM_INT,
	TRANSCV,
	PAD,
	NUM_TILE_TYPES,
	MULTIPLE,
	INVALID
};

// Represents Frame Address Structure
struct frame_addr {
  int type;
  int tb;
  int row;
  int col;
  int mna;
	frame_addr()
	{
		type = -1;
		tb = -1;
		row = -1;
		col = -1;
		mna = -1;
	}
	/**
	 * Initialization constructor.
	 * @param type Block type.
	 * @param tb Top bottom indicator.
	 * @param row Row address.
	 * @param col column address.
	 * @param mna minor address.
	 */
	frame_addr(int type, int tb, int row, int col, int mna) : type(type), tb(tb), row(row), col(col), mna(mna)
	{};
	string str() const
	{
		string far_string;
			far_string = boost::lexical_cast<std::string> (type) + "_" +
			boost::lexical_cast<std::string> (tb) + "_" +
			boost::lexical_cast<std::string> (row) + "_" + 
			boost::lexical_cast<std::string> (col) + +"_" + 
			boost::lexical_cast<std::string> (mna);
			return far_string;
	}
};

class architecture {
	protected:
		// 32-bit words per frame
		int frame_words;
		// Height of frame in Tiles 
		int frame_height;
		// Array stores number of frames per tile type
	int tile_frames[NUM_TILE_TYPES];
  public:
		friend class virtex4;
		friend class virtex5;
		friend class device;
};
}
}
#endif
