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

#include "openpr/anticore/OpenPRTree.hpp"


namespace openpr {

boost::filesystem::path OpenPRTree::sRelativePath;
boost::filesystem::path OpenPRTree::sWorkingPath;
boost::filesystem::path OpenPRTree::sExecutablePath;
boost::filesystem::path OpenPRTree::sEdaPath;
boost::filesystem::path OpenPRTree::sDatabasePath;
boost::filesystem::path OpenPRTree::sLogPath;

const std::string cEdaNameConst("eda");
const std::string cArchitectureNameConst("architecture");
const std::string cXilinxNameConst("xilinx");

OpenPRTree::OpenPRTree(const char* argv0) {
	// get the working directory and the relative executable path
	sWorkingPath = boost::filesystem::initial_path();
	boost::filesystem::path argvPath(argv0);
	sRelativePath = argvPath.parent_path(); // so much for relative paths ...
	// get the executable path
	sExecutablePath = sRelativePath;
	// build the eda and log paths
	sEdaPath = sExecutablePath / cEdaNameConst;
	sLogPath = sExecutablePath;
	// build the database path
	sDatabasePath = sExecutablePath / cArchitectureNameConst / cXilinxNameConst;

	// normalize each of the paths
	sRelativePath.normalize();
	sWorkingPath.normalize();
	sExecutablePath.normalize();
	sEdaPath.normalize();
	sDatabasePath.normalize();
	sLogPath.normalize();
}

} /* namespace nmt */
