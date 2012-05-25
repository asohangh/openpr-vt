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

/*
 * AntiCoreV4.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: jacouch
 */
#include "openpr/anticore/AntiCoreV4.hpp"
namespace openpr {
	/**
	 * construct Anticore object.
	 * @param inDB torc::architecture::DDB database.
	 */
	AntiCoreV4::AntiCoreV4(torc::architecture::DDB& inDB) :
		AntiCoreBase(inDB), tilesPerRegion(17) {
		buildValidBoundaries(tilesPerRegion);
	}
	;
	/**
	 * Construct an AntiCore object, and define boundaries of AntiCore based
	 * upon sliceA and sliceB tile coordinates.
	 * @param inDB torc::architecture::DDB  enabled database for access to browser data.
	 * @param sliceA String representing bottom corner of AntiCore region.
	 * @param sliceB String representing top corner of AntiCore region.
	 */
	AntiCoreV4::AntiCoreV4(torc::architecture::DDB& inDB, string siteA, string siteB) :
		AntiCoreBase(inDB, siteA, siteB), tilesPerRegion(17) {
		buildValidBoundaries(tilesPerRegion);
	}
	;
	/**
	 * Generate placement of bus macros, and return map representing relationship of busmacro name to type.
	 * @param busWidth The width of bus entering the region.
	 * @param ucfFile fstream for output of constraints
	 * @param busMacroPrefix String containing the prefix of teh busMacro name.
	 */
	bmNameToTypeMap& AntiCoreV4::genMacroPlacement(int busWidth, fstream& ucfFile,
			string busMacroPrefix) {
		try {
			if (!ucfFile.good())
				throw "CAntiCore:genProhibitConstraints: UCF file cannot be opened for writing.";
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:buildSiteMap: Region hasn't been initialized yet";
			if (siteMap.empty()) {
				buildSiteMap();
				if (siteMap.empty())
					throw "CAntiCore:buildSiteMap: There aren't any sites in the defined region.";
			}
			// Increase bus width until it divides evenly into macro widths.
			while ((busWidth % macroWidth) != 0) {
				busWidth++;
			}
			int numMacros = busWidth / macroWidth;
			int unplacedInputMacros = numMacros;
			int unplacedOutputMacros = numMacros;

			ucfFile << "# OpenPR generated Placement constraints." << endl;

			for (int row = yMin; row <= yMax; row++) {
				for (int col = xMin; col <= xMax; col++) {
					// check if tile is of supported type for macro
					// if it is find the min/max SLICE in tile and generate a placement constraint for macro
					torc::architecture::xilinx::TileIndex ti =
							mTiles.getTileIndex(
									torc::architecture::xilinx::TileRow(row),
									torc::architecture::xilinx::TileCol(col));
					torc::architecture::xilinx::TileTypeIndex tti =
							mTiles.getTileInfo(ti).getTypeIndex();
					string tileType = mTiles.getTileTypeName(tti);
					std::multimap<torc::architecture::xilinx::TileIndex,
							torc::architecture::Site>::iterator it;

					//cout << "(" << row << "," << col << ")" << " is " << mTiles.mTiles[ti].getName() << endl;
					string macroName;
					string macroType;
					bool foundPlacement = false;
					if (tileType == "CLB" && unplacedInputMacros > 0 && row
							== yMin) {
						macroName = busMacroPrefix + "_input";
						macroType = busMacroPrefix + "_in";
						foundPlacement = true;
						unplacedInputMacros--;
					}
					if (tileType == "CLB" && unplacedOutputMacros > 0 && row
							== yMax) {
						macroName = busMacroPrefix + "_output";
						macroType = busMacroPrefix + "_out";
						foundPlacement = true;
						unplacedOutputMacros--;
					}
					if (foundPlacement == true) {
						busMacroMap[macroName] = macroType;
						ucfFile << "INST \"" << macroName << "\" LOC=\""
								<< placeMacro(ti, true) << "\";" << endl;
						macroTiles.push_back(ti);
					}
				}
			}
			return busMacroMap;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	string AntiCoreV4::placeMacro(torc::architecture::xilinx::TileIndex ti,
			bool minOrMax) {
		const bool min = false;
		openpr::prohibitRange maxSite("SLICE_X0Y0", "SLICE_X0Y0");
		// This is really dirty, there should be a clean way to find a the largest possible site
		openpr::prohibitRange minSite("SLICE_X2000Y2000", "SLICE_X2000Y2000");
		tileToSiteMap::iterator it;
		tileToSiteMap::iterator upperBound = siteMap.upper_bound(ti);

		cout << "Start searching" << endl;
		cout << "There are " << siteMap.count(ti)
				<< " elements in siteMap matching tileIndex" << ti << endl;
		for (it = siteMap.find(ti); it != upperBound; it++) {
			cout << "it in tile " << mTiles.getTileInfo(it->first).getName()
					<< endl;
			const torc::architecture::Site& site = it->second;
			string siteName = site.getName();
			string siteType = getSiteType(siteName);
			openpr::prohibitRange siteRange(siteName, siteName);
			if (siteType == "SLICE") {
				if (siteRange > maxSite)
					maxSite = siteRange;
				if (siteRange < minSite)
					minSite = siteRange;
			}
		}
		cout << "End searching" << endl;
		// If we are at bottom of region, place output macro
		if (minOrMax == min) {
			return minSite.minSite;
		}
		// Else if at top of region, place input macro
		else {
			return maxSite.maxSite;
		}
	}

} // namespace adb
