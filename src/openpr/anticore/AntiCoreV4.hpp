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

#ifndef ADB_ANTICOREV4_H
#define ADB_ANTICOREV4_H

#include "openpr/anticore/AntiCoreBase.hpp"

namespace openpr {
class AntiCoreV4: public AntiCoreBase {
private:
	const int tilesPerRegion;
public:
	/**
	 * construct Anticore object.
	 * @param inDB torc::architecture::DDB database.
	 */
	AntiCoreV4(torc::architecture::DDB& inDB);
	/**
	 * Construct an AntiCore object, and define boundaries of AntiCore based
	 * upon sliceA and sliceB tile coordinates.
	 * @param inDB torc::architecture::DDB  enabled database for access to browser data.
	 * @param sliceA String representing bottom corner of AntiCore region.
	 * @param sliceB String representing top corner of AntiCore region.
	 */
	AntiCoreV4(torc::architecture::DDB& inDB, string siteA, string siteB);
	/**
	 * Generate placement of bus macros, and return map representing relationship of busmacro name to type.
	 * @param busWidth The width of bus entering the region.
	 * @param ucfFile fstream for output of constraints
	 * @param busMacroPrefix String containing the prefix of teh busMacro name.
	 */
	virtual bmNameToTypeMap& genMacroPlacement(int busWidth, fstream& ucfFile,
			string busMacroPrefix);
	virtual string placeMacro(torc::architecture::xilinx::TileIndex ti,
			bool minOrMax) ;

}; // namespace adb
}
#endif // ADB_CANTICOREV4_H

