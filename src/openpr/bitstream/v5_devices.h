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

#ifndef XC5VLX50_H
#define XC5VLX50_H

#include"openpr/bitstream/virtex5.h"
#include"openpr/bitstream/device.h"
namespace openpr
{
namespace bitstream
{

class xc5vlx50:public virtex5 {
  protected:
    enum { xc5vlx50_num_rows = 6, xc5vlx50_num_cols = 39, };
    static const tile_types xc5vlx50_row_layout[];
    static const string xc5vlx50_name;
    static const int xc5vlx50_id;
  public:
     xc5vlx50():virtex5(xc5vlx50_num_rows, xc5vlx50_num_cols,
			xc5vlx50_row_layout, xc5vlx50_name, xc5vlx50_id) {
	build_xdl_layout();
    };
};

class xc5vlx50t:public virtex5 {
  protected:
    enum { xc5vlx50t_num_rows = 6, xc5vlx50t_num_cols = 42, };
    static const tile_types xc5vlx50t_row_layout[];
    static const string xc5vlx50t_name;
    static const int xc5vlx50t_id;
  public:
     xc5vlx50t():virtex5(xc5vlx50t_num_rows, xc5vlx50t_num_cols,
			xc5vlx50t_row_layout, xc5vlx50t_name, xc5vlx50t_id) {
	build_xdl_layout();
    };
};

class xc5vsx95t:public virtex5 {
  protected:
    enum { xc5vsx95t_num_rows = 8, xc5vsx95t_num_cols = 77, };
    static const tile_types xc5vsx95t_row_layout[];
    static const string xc5vsx95t_name;
    static const int xc5vsx95t_id;
  public:
     xc5vsx95t():virtex5(xc5vsx95t_num_rows, xc5vsx95t_num_cols,
			xc5vsx95t_row_layout, xc5vsx95t_name, xc5vsx95t_id) {
	build_xdl_layout();
    };
};

class xc5vlx110t:public virtex5 {
  protected:
    enum { xc5vlx110t_num_rows = 8, xc5vlx110t_num_cols = 70, };
    static const tile_types xc5vlx110t_row_layout[];
    static const string xc5vlx110t_name;
    static const int xc5vlx110t_id;
  public:
     xc5vlx110t():virtex5(xc5vlx110t_num_rows, xc5vlx110t_num_cols,
			xc5vlx110t_row_layout, xc5vlx110t_name, xc5vlx110t_id) {
	build_xdl_layout();
    };
};
}
}

#endif
