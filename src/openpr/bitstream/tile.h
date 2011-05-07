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

namespace openpr
{
namespace bitstream
{
	struct tile_coord
	{
		int x;
		int y;
		tile_coord()
		{
			x = -1;
			y = -1;
		}
		tile_coord(int x, int y) : x(x), y(y){};
		void set(int _x, int _y)
		{
		 x = _x;
		 y = _y; 
		};
		bool operator== (tile_coord const &other) const
		{
			return (x == other.x && y == other.y);
		}

		friend std::size_t hash_value (tile_coord const &to_hash)
		{
			std::size_t seed = 0;
			boost::hash_combine (seed, to_hash.x);
			boost::hash_combine (seed, to_hash.y);
			return seed;
		}
	};

	struct tile_data
	{
		string name;
		tile_coord coord;
		frame_addr far;
		int frame_num;
		int byte_off;
		int num_frames;
		char *first_frame;
		tile_data(const string _name, const int x, const int y, const frame_addr _far, 
				const int _num_frames, const int _frame_num, const int _byte_off,
				char *const _first_frame)
		{
			name = _name;
			coord.set(x,y);
			far = _far;
			num_frames = _num_frames;
			frame_num = _frame_num;
			byte_off = _byte_off;
			first_frame = _first_frame;
		}
		void print()
		{
			printf("%s:\tTile(%d,%d)-\tfar: %10s,\tnum_frames: %d,\tframe_num: %6d,\tbyte_off: %4d,\tfirst_frame: %p\n",
					name.c_str(),coord.x,coord.y,far.str().c_str(),num_frames,frame_num,byte_off,first_frame);
		}
	};
}
}
