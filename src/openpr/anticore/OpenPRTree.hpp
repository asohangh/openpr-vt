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

#ifndef OPENPR_CTREE_H
#define OPENPR_CTREE_H

#include <boost/filesystem/convenience.hpp>

namespace openpr {

class OpenPRTree {

protected:
	static boost::filesystem::path sRelativePath;
	static boost::filesystem::path sWorkingPath;
	static boost::filesystem::path sExecutablePath;
	static boost::filesystem::path sEdaPath;
	static boost::filesystem::path sDatabasePath;
	static boost::filesystem::path sLogPath;

public:
	OpenPRTree(const char* argv0);

	static const boost::filesystem::path& relativePath(void) {
		return sRelativePath;
	}
	static const boost::filesystem::path& workingPath(void) {
		return sWorkingPath;
	}
	static const boost::filesystem::path& executablePath(void) {
		return sExecutablePath;
	}
	static const boost::filesystem::path& edaPath(void) {
		return sEdaPath;
	}
	static const boost::filesystem::path& databasePath(void) {
		return sDatabasePath;
	}
	static const boost::filesystem::path& logPath(void) {
		return sLogPath;
	}

};

}

#endif 
