// Torc - Copyright 2011-2013 University of Southern California.  All Rights Reserved.
// $HeadURL: https://svn.code.sf.net/p/torc-isi/code/tags/torc-1.0/src/torc/architecture/XdlImporter.hpp $
// $Id: XdlImporter.hpp 16 2013-11-12 22:50:42Z nsteiner $

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

/// \file
/// \brief Header for the XdlImporter class.

#ifndef OPENPR_XDL_XDLIMPORTER_HPP
#define OPENPR_XDL_XDLIMPORTER_HPP

#include "torc/architecture/XdlImporter.hpp"

namespace openpr {
namespace xdl {

	/// \brief Architecture aware importer from XDL format into a physical design.
	class XdlImporter : public torc::architecture::XdlImporter {
	protected:
	// typedefs
		/// \brief Type of the superclass.
		typedef torc::architecture::XdlImporter super;
		/// \brief Override device database allocation with null operation.
		void initializeDatabase(void) { }
	public:
	// constructors
		/// \brief Constructor.
		/// \param DDBPtr Existing Device Database to avoid multiple DDB copies
		XdlImporter(DDB* DDBPtr) : super() { mDDBPtr = DDBPtr; }
		/// \brief Destructor. Release DDB pointer before super destructor tries to deallocate it.
		~XdlImporter(void) {
			releaseDDBPtr();
		}
	};

} // namespace xdl
} // namespace openpr

#endif // OPENPR_XDL_XDLIMPORTER_HPP
