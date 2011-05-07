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

#ifndef V4_DEVICES_H
#include"openpr/bitstream/virtex4.h"
#include"openpr/bitstream/device.h"
namespace openpr
{
namespace bitstream
{

class xc4vlx15:public virtex4 {
  protected:
    enum { xc4vlx15_num_rows = 4, xc4vlx15_num_cols = 35, };
    static const tile_types xc4vlx15_row_layout[];
    static const string xc4vlx15_name;
    static const int xc4vlx15_id;
  public:
     xc4vlx15():virtex4(xc4vlx15_num_rows, xc4vlx15_num_cols,
			xc4vlx15_row_layout, xc4vlx15_name, xc4vlx15_id) {
	build_xdl_layout();
    };
};

class xc4vlx60:public virtex4 {
  protected:
    enum { xc4vlx60_num_rows = 8, xc4vlx60_num_cols = 67, };
    static const tile_types xc4vlx60_row_layout[];
    static const string xc4vlx60_name;
    static const int xc4vlx60_id;
  public:
     xc4vlx60():virtex4(xc4vlx60_num_rows, xc4vlx60_num_cols,
			xc4vlx60_row_layout, xc4vlx60_name, xc4vlx60_id) {
	build_xdl_layout();
    };
};

class xc4vfx60:public virtex4 {
  protected:
    enum { xc4vfx60_num_rows = 8, xc4vfx60_num_cols = 76, };
    static const tile_types xc4vfx60_row_layout[];
    static const string xc4vfx60_name;
    static const int xc4vfx60_id;
  public:
     xc4vfx60():virtex4(xc4vfx60_num_rows, xc4vfx60_num_cols,
			xc4vfx60_row_layout, xc4vfx60_name, xc4vfx60_id) {
	build_xdl_layout();
    };
};
}
}

#endif
