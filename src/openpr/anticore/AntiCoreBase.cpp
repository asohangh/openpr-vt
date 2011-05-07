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
 * AntiCoreBase.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: jacouch
 */
#include "openpr/anticore/AntiCoreBase.hpp"
namespace openpr
{

/**
	 * construct Anticore object.
	 * @param inDB CDB database.
	 */
	AntiCoreBase::AntiCoreBase(torc::architecture::DDB & inDB) :
		mDB(inDB), mTiles(mDB.getTiles()), mSegments(mDB.getSegments()),
				currentMode(eStatic), macroWidth(8) {
		allocateMask();
		xMin = torc::architecture::xilinx::TileCol(-1);
		xMax = torc::architecture::xilinx::TileCol(-1);
		yMax = torc::architecture::xilinx::TileRow(-1);
		yMin = torc::architecture::xilinx::TileRow(-1);
	}
	/**
	 * Construct an AntiCore object, and define boundaries of AntiCore based
	 * upon sliceA and sliceB tile coordinates.
	 * @param inDB CDB  enabled database for access to browser data.
	 * @param sliceA String representing bottom corner of AntiCore region.
	 * @param sliceB String representing top corner of AntiCore region.
	 */
	AntiCoreBase::AntiCoreBase(torc::architecture::DDB & inDB, string sliceA, string sliceB) :
		mDB(inDB), mTiles(mDB.getTiles()), mSegments(mDB.getSegments()),
				currentMode(eStatic), macroWidth(8) {
		allocateMask();
		updateRegion(sliceA, sliceB);
		importXDL();
		setupRouteBlocker();
		buildSiteMap();
	}
	/**
	 * Allocate memory for bitmask.
	 * @todo Replace integer matrix with dynamic_bitset.
	 */
	void AntiCoreBase::allocateMask() {
		mask = new int*[(boost::uint16_t) (mTiles.getRowCount())];
		for (int i = 0; i < mTiles.getRowCount(); i++) {
			mask[i] = new int[(boost::uint16_t) mTiles.getColCount()];
			if (!mask[i])
				cerr << "Failed to allocate column!" << endl;
		}
	}
	/**
	 * Set the mode to determine behavior of anticore.
	 * @param newMode enum value of mode to switch to.
	 */
	void AntiCoreBase::setMode(eMode newMode) {
		currentMode = newMode;
	}
	/**
	 * Build vector which stores all valid boundary rows for this device.
	 * @param tilesPerRegion Number of tile rows per clock region.
	 */
	void AntiCoreBase::buildValidBoundaries(const int tilesPerRegion) {
		int tileCount = 0;
		for (int row = 1; torc::architecture::xilinx::TileRow(row)
				< mTiles.getRowCount(); row++) {
			tileCount++;
			// First tile after clock divider is a valid boundary
			if (tileCount == 1)
				validBoundaries.push_back(torc::architecture::xilinx::TileRow(
						row));
			// Last tile before region divider is a valid boundary
			if (tileCount == tilesPerRegion) {
				validBoundaries.push_back(torc::architecture::xilinx::TileRow(
						row));
				tileCount = -1;
			}
		}
	}
	std::string AntiCoreBase::exportPipFromArc(torc::architecture::Tilewire source, torc::architecture::Tilewire sink, torc::architecture::DDB& mDB)
	{
		std::stringstream temp;
		torc::architecture::ExtendedWireInfo sourceWireInfo = torc::architecture::ExtendedWireInfo(mDB, source);
		torc::architecture::ExtendedWireInfo sinkWireInfo = torc::architecture::ExtendedWireInfo(mDB, sink);

			temp << "pip " << sourceWireInfo.mTileName << " "
				<< sourceWireInfo.mWireName << " -> "
				<< sinkWireInfo.mWireName;
		return temp.str();
	}
	/**
	 * Build map between tile index and sites contained within.
	 */
	void AntiCoreBase::buildSiteMap() {
		try {
			if (xMin == torc::architecture::xilinx::TileRow(-1))
				throw "CAntiCore:buildSiteMap: Region hasn't been initialized yet";
			const torc::architecture::Array<
					const torc::architecture::Sites::Site>& sites =
					mDB.getSites().getSites();
			torc::architecture::xilinx::SiteCount siteCount =
					torc::architecture::xilinx::SiteCount(sites.getSize());
			int i = 0;

			// Expand region by two so we can prohibit sites that would be blocked by our routing.
			expandRegion(3);
			for (i = 0; torc::architecture::xilinx::SiteCount(i) < siteCount; i++) {
				const torc::architecture::Sites::Site site = sites[i];
				torc::architecture::xilinx::TileIndex ti = site.getTileIndex();
				if (inRegion(ti))
					siteMap.insert(std::pair<
							torc::architecture::xilinx::TileIndex,
							torc::architecture::Sites::Site>(ti, site));
			}
			// Undo expansion to make our prohibiting easier.
			shrinkRegion(3);
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Import an XDL file and reserve all currently used pips.
	 */
	void AntiCoreBase::importXDL(void) {
		ifstream xdlFile("top_placed.xdl", ifstream::in);
		string xdlString;
		char buf[1024];
		// Match a pip in and extract tile, source and sink
		static const boost::regex extractPip(
				"\\s*pip\\s(\\w*)\\s(\\w*)\\s[=-]>\\s(\\w*).*");
		// Check if pip is a ROUTETHROUGH
		static const boost::regex notRouteThrough(".*ROUTETHROUGH.*");
		while (xdlFile.getline(buf, 1024)) {
			xdlString = buf;
			boost::smatch matches;
			// If ROUTETHROUGH, ignore
			if (regex_match(xdlString, matches, notRouteThrough))
				continue;
			// Otherwise import it into CDB
			else if (regex_match(xdlString, matches, extractPip)) {
				cout << matches[1] << " " << matches[2] << " " << matches[3]
						<< endl;

				torc::architecture::Arc tempArc = mDB.tilePipToArc(matches[1], matches[2], matches[3]);
				mDB.useArc(tempArc);
				//cout << "Import Arc from pip failed" << endl;
			}
		}
	}
	/**
	 * Return values of four region vertices by reference.
	 * @param _xMin Minimum x-coordinate.
	 * @param _xMax Maximum x-coordinate.
	 * @param _yMin Miniumum y-coordinate.
	 * @param _yMax Maximum y-coordinate.
	 */
	void AntiCoreBase::getRegionVertices(torc::architecture::xilinx::TileCol& _xMin,
			torc::architecture::xilinx::TileCol& _xMax,
			torc::architecture::xilinx::TileRow& _yMin,
			torc::architecture::xilinx::TileRow& _yMax) {
		_xMin = xMin;
		_xMax = xMax;
		_yMin = yMin;
		_yMax = yMax;
	}
	/**
	 * Set values of four region vertices.
	 * @param _xMin Minimum x-coordinate.
	 * @param _xMax Maximum x-coordinate.
	 * @param _yMin Miniumum y-coordinate.
	 * @param _yMax Maximum y-coordinate.
	 */
	void AntiCoreBase::setRegionVertices(torc::architecture::xilinx::TileCol _xMin,
			torc::architecture::xilinx::TileCol _xMax,
			torc::architecture::xilinx::TileRow _yMin,
			torc::architecture::xilinx::TileRow _yMax) {
		xMin = _xMin;
		xMax = _xMax;
		yMin = _yMin;
		yMax = _yMax;
		validateRegion();
		setupRouteBlocker();
	}
	/**
	 * Update boundaries of Anticore region based upon siteA and siteB tile
	 * coordinates.
	 * @param siteA String representing bottom corner of AntiCore region.
	 * @param siteB String representing top corner of AntiCore region.
	 */
	void AntiCoreBase::updateRegion(string siteA, string siteB) {
		torc::architecture::xilinx::TileIndex startIndex = siteNameToTileIndex(
				siteA);
		torc::architecture::xilinx::TileIndex endIndex = siteNameToTileIndex(
				siteB);
		startTile = mDB.getTiles().getTileInfo(startIndex);
		endTile = mDB.getTiles().getTileInfo(endIndex);
		yMin = (startTile.getRow() < endTile.getRow()) ? startTile.getRow()
				: endTile.getRow();
		yMax = (startTile.getRow() > endTile.getRow()) ? startTile.getRow()
				: endTile.getRow();
		xMin = (startTile.getCol() < endTile.getCol()) ? startTile.getCol()
				: endTile.getCol();
		xMax = (startTile.getCol() > endTile.getCol()) ? startTile.getCol()
				: endTile.getCol();
		expandRegionToINT();
		validateRegion();
		setupRouteBlocker();
	}
	/**
	 * Update boundaries of Anticore region based upon sliceA and sliceB tile
	 * coordinates.
	 * @param siteA String representing bottom corner of AntiCore region.
	 * @param siteB String representing top corner of AntiCore region.
	 */
	void AntiCoreBase::updateRegionExpand(string siteA, string siteB) {
		if (xMin == torc::architecture::xilinx::TileCol(-1)) {
			updateRegion(siteA, siteB);
			return;
		}
		torc::architecture::xilinx::TileIndex startIndex = siteNameToTileIndex(
				siteA);
		torc::architecture::xilinx::TileIndex endIndex = siteNameToTileIndex(
				siteB);
		torc::architecture::xilinx::TileRow startTempY =
				mDB.getTiles().getTileInfo(startIndex).getRow();
		torc::architecture::xilinx::TileRow endTempY =
				mDB.getTiles().getTileInfo(endIndex).getRow();
		torc::architecture::xilinx::TileCol startTempX =
				mDB.getTiles().getTileInfo(startIndex).getCol();
		torc::architecture::xilinx::TileCol endTempX =
				mDB.getTiles().getTileInfo(endIndex).getCol();
		torc::architecture::xilinx::TileRow newYMin =
				(startTempY < endTempY) ? startTempY : endTempY;
		torc::architecture::xilinx::TileRow newYMax =
				(startTempY > endTempY) ? startTempY : endTempY;
		torc::architecture::xilinx::TileCol newXMin =
				(startTempX < endTempX) ? startTempX : endTempX;
		torc::architecture::xilinx::TileCol newXMax =
				(startTempX > endTempX) ? startTempX : endTempX;
		xMin = (newXMin < xMin) ? newXMin : xMin;
		yMin = (newYMin < yMin) ? newYMin : yMin;
		xMax = (newXMax > xMax) ? newXMax : xMax;
		yMax = (newYMax > yMax) ? newYMax : yMax;
		expandRegionToINT();
		validateRegion();
		setupRouteBlocker();
		cout << "Region updated to dimensions: (" << xMin << "," << xMax << ")"
				<< "(" << yMin << "," << yMax << ")" << endl;
	}
	/**
	 * Expand the region so that the first column includes the interconnect tiles.
	 */
	void AntiCoreBase::expandRegionToINT() {
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCoreV5:expandRegionToINT: Region hasn't been initialized yet.";
			torc::architecture::xilinx::TileRow row = yMax;
			torc::architecture::xilinx::TileCol col = xMin;
			torc::architecture::xilinx::TileIndex ti = mTiles.getTileIndex(row,
					col);
			torc::architecture::xilinx::TileTypeIndex tti = mTiles.getTileInfo(
					ti).getTypeIndex();
			string tileType = mTiles.getTileTypeName(tti);

			while (col > 0 && tileType != "INT") {
				col--;
				ti = mTiles.getTileIndex(row, col);
				tti = mTiles.getTileInfo(ti).getTypeIndex();
				tileType = mTiles.getTileTypeName(tti);
			}
			if ((xMin - col) > 2) {
				cerr
						<< "CAntiCoreV5:expandRegionToINT: Region borders do not match expected patterns, please check"
						<< endl;
				return;
			} else
				xMin = col;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Expand the region by the specified number of tiles.
	 * @param increment The number of tiles the region needs to be expanded by.
	 */
	void AntiCoreBase::expandRegion(int increment) {
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:expandRegion: Region hasn't been defined yet";
			int opCount = 0;
			while (xMin > 0 && opCount < increment) {
				xMin--;
				opCount++;
			}
			opCount = 0;
			while (xMax < mTiles.getColCount() && opCount < increment) {
				xMax++;
				opCount++;
			}
			opCount = 0;
			while (yMin > 0 && opCount < increment) {
				yMin--;
				opCount++;
			}
			opCount = 0;
			while (yMax < mTiles.getRowCount() && opCount < increment) {
				yMax++;
				opCount++;
			}
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Shrink the region by the specified number of tiles.
	 * @param decrement The number of tiles the region needs to be shrunken by.
	 */
	void AntiCoreBase::shrinkRegion(int decrement) {
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:shrinkRegion: Region hasn't been defined yet";
			int opCount = 0;
			while (xMin < xMax && opCount < decrement) {
				xMin++;
				opCount++;
			}
			opCount = 0;
			while (xMax > xMin && opCount < decrement) {
				xMax--;
				opCount++;
			}
			opCount = 0;
			while (yMin < yMax && opCount < decrement) {
				yMin++;
				opCount++;
			}
			opCount = 0;
			while (yMax > yMin && opCount < decrement) {
				yMax--;
				opCount++;
			}
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Ensure that region spans multiples of clock regions.
	 */
	bool AntiCoreBase::validateRegion() {
		try {
			bool matchedYMin = false;
			bool matchedYMax = false;
			vector<torc::architecture::xilinx::TileRow>::iterator it =
					validBoundaries.begin();

			while (it != validBoundaries.end()) {
				if (yMin == *it)
					matchedYMin = true;
				if (yMax == *it)
					matchedYMax = true;
				it++;
			}
			if (!matchedYMin || !matchedYMax) {
				cerr
						<< "CAntiCoreBase::validateRegion: Region does not span multiples of clock regions."
						<< endl;
				return false;
			}
			return true;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Generate PROHIBIT constraints that can be copied into the ucf file.
	 */
	void AntiCoreBase::blockSites(void) {
		const torc::architecture::Array<const torc::architecture::Sites::Site>
				& sites = mDB.getSites().getSites();
		boost::uint32_t siteCount = sites.getSize();
		//torc::architecture::xilinx::SiteCount(sites.size());
		uint i = 0;//TSiteIndex
		prohibitedSites.clear();
		string prohibitString = "CONFIG PROHIBIT=\"";
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:blockSites: Region hasn't been defined yet";
			for (i = 0; i < siteCount; i++) {
				const torc::architecture::Sites::Site& site = sites[i];
				if (inRegion(site.getTileIndex())) {
					string siteName = site.getName();
					string siteType = getSiteType(siteName);
					openpr::prohibitRange siteRange(siteName, siteName);

					if (prohibitedSites.count(siteType) == 0) {
						prohibitedSites[siteType] = siteRange;
					} else {
						openpr::prohibitRange& currentRange =
								prohibitedSites[siteType];
						if (siteRange < currentRange)
							currentRange.minSite = siteName;
						else if (siteRange > currentRange)
							currentRange.maxSite = siteName;
					}
				}
			}

			map<std::string, openpr::prohibitRange>::iterator it;
			// Print out constraints, each one gets a seperate line because the ucf
			// interpretor does NOT seem to follow CGD rules regarding comma separated
			// sites
			for (it = prohibitedSites.begin(); it != prohibitedSites.end(); it++) {
				if (it->second.minSite == it->second.maxSite)
					cout << prohibitString << it->second.minSite << "\";"
							<< endl;
				else
					cout << prohibitString << it->second.minSite << ":"
							<< it->second.maxSite << "\";" << endl;
			}
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}

	/**
	 * Write the necessary Prohibit constraints to stdout
	 */
	void AntiCoreBase::genProhibitConstraints() {
		string prohibitString = "CONFIG PROHIBIT=\"";
		map<std::string, openpr::prohibitRange>::iterator it;
		// Print out constraints, each one gets a seperate line because the ucf
		// interpretor does NOT seem to follow CGD rules regarding comma separated
		// sites
		for (it = prohibitedSites.begin(); it != prohibitedSites.end(); it++) {
			static const boost::regex clkResource("BUFG.*");
			boost::smatch matches;
			if (regex_match(it->first, matches, clkResource))
				continue;
			if (it->second.minSite == it->second.maxSite)
				cout << prohibitString << it->second.minSite << "\";" << endl;
			else
				cout << prohibitString << it->second.minSite << ":"
						<< it->second.maxSite << "\";" << endl;
		}
	}
	/**
	 * Parse the specified UCF file and update the region to reflect the AREA_GROUP constraints found within.
	 * @param ucfFile Path/filename of UCF File.
	 * @param dynamicAGName the name of the PR region in the UCF file.
	 */
	bool AntiCoreBase::retrieveDynamicRegion(fstream& constraintsFile, string dynamicAGName) {
		try {
			if (!constraintsFile.good())
				throw "CAntiCore:retrieveDynamicRegion: failed to open UCF file.";

			cout << "Parsing UCF File" << endl;
			string ucfLine;

			while (std::getline(constraintsFile, ucfLine)) {
				// Extract AREA_GROUP constraints
				static const boost::regex
						extractAG(
								"AREA_GROUP\\s*\"(\\w*)\"\\s*RANGE\\s*=\\s*(\\w*):(\\w*);");
				boost::smatch curAG;
				if (regex_match(ucfLine, curAG, extractAG)) {
					if (curAG[1] == dynamicAGName) {
						updateRegionExpand(curAG[2], curAG[3]);
						cout << curAG[1] << " " << curAG[2] << " " << curAG[3]
								<< endl;
					}
				}
			}
			if (!validateRegion())
				throw "CAntiCoreBase::retrieveDynamicRegion: Region is not valid after all area groups were processed";

			return true;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Write the necessary Prohibit constraints to a specified ucf file.
	 * @param ucfFile fstream object representing the ucf file
	 */
	void AntiCoreBase::genProhibitConstraints(fstream & ucfFile) {
		string prohibitString = "CONFIG PROHIBIT=\"";
		map<std::string, openpr::prohibitRange>::iterator it;
		// Print out constraints, each one gets a seperate line because the ucf
		// interpretor does NOT seem to follow CGD rules regarding comma separated
		// sites
		for (it = prohibitedSites.begin(); it != prohibitedSites.end(); it++) {
			static const boost::regex clkResource("BUFG.*");
			boost::smatch matches;
			if (regex_match(it->first, matches, clkResource))
				continue;
			if (it->second.minSite == it->second.maxSite)
				ucfFile << prohibitString << it->second.minSite << "\";"
						<< endl;
			else
				ucfFile << prohibitString << it->second.minSite << ":"
						<< it->second.maxSite << "\";" << endl;
		}
	}

	/**
	 * Generate placement constraints to block site placement within the region.
	 * @param busWidth The width of bus entering the region.
	 * @param ucfFile fstream for output of constraints
	 */
	void AntiCoreBase::genPlaceConstraints(int busWidth, fstream & ucfFile) {
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
			if (busMacroMap.empty()) {
				cerr
						<< "CAntiCoreBase::genPlaceConstraints: No bus macros have been placed. Assuming this is desired behavior."
						<< endl;
			}

			// Expand region by two tiles in each direction, this ensures that ISE won't place things in unrouteable positions.
			expandRegion(3);
			for (int col = xMin; col <= xMax; col++) {
				genProhibitConstraints(ucfFile);
				prohibitedSites.clear();
				for (int row = yMin; row <= yMax; row++) {
					// check if tile is of supported type for macro
					// if it is find the min/max SLICE in tile and generate a placement constraint for macro
					torc::architecture::xilinx::TileIndex ti =
							mTiles.getTileIndex(
									torc::architecture::xilinx::TileRow(row),
									torc::architecture::xilinx::TileCol(col));
					torc::architecture::xilinx::TileTypeIndex tti =
							mTiles.getTileInfo(ti).getTypeIndex();
					std::string tileName = mTiles.getTileInfo(ti).getName();
					string tileType = mTiles.getTileTypeName(tti);
					std::multimap<torc::architecture::xilinx::TileIndex,
							torc::architecture::Sites::Site>::iterator it;
					it = siteMap.find(ti);

					if (siteMap.count(ti) == 0)
						cout << "No sites found in tile: " << tileName << endl;
					else {
						// Check to see if a bus macro has been placed in this tile
						bool isBmTile = false;
						for (boost::uint32_t i = 0; i < macroTiles.size(); i++) {
							if (ti == macroTiles[i])
								isBmTile = true;
						}
						// Since there shouldn't be anything other than SLICEs in the BusMacro tiles
						// we can just dump the existing prohibited sites
						if (isBmTile) {
							cout << tileName << "Is a busMacro tile" << endl;
							genProhibitConstraints(ucfFile);
							prohibitedSites.clear();
						}
						// Add tiles in sites to prohibit range
						else {
							tileToSiteMap::iterator upperBound =
									siteMap.upper_bound(ti);
							for (it = siteMap.find(ti); it != upperBound; it++) {
								const torc::architecture::Sites::Site& site =
										it->second;
								string siteName = site.getName();
								string siteType = getSiteType(siteName);
								openpr::prohibitRange siteRange(siteName,
										siteName);

								if (prohibitedSites.count(siteType) == 0)
									prohibitedSites[siteType] = siteRange;
								else {
									openpr::prohibitRange& currentRange =
											prohibitedSites[siteType];
									if (siteRange < currentRange)
										currentRange.minSite = siteName;
									else if (siteRange > currentRange)
										currentRange.maxSite = siteName;
								}
							}
						}
					}
				}
			}
			ucfFile << "# End OpenPR generated Placement constraints." << endl;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}

	/**
	 * Return site type (CLB, IOB, etc.).
	 * @param site Site to be queried.
	 */
	string AntiCoreBase::getSiteType(string site) {
		vector<string> splitSite;
		boost::split(splitSite, site, boost::is_any_of("_"));
		return splitSite[0];
	}

	/**
	 * Determine whether tile is within the dynamic region.  This function
	 * is used by the blockSites function currently.
	 * @param queryTileIndex Tile index to be queried.
	 */
	bool AntiCoreBase::inRegion(torc::architecture::xilinx::TileIndex queryTileIndex) {
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:inRegion: Region hasn't been defined yet";
			/*if (startTile == torc::architecture::TileInfo() || endTile
			 == torc::architecture::TileInfo()) {
			 cout << "CAntiCoreBase::Start or End Tile is NULL" << endl;
			 return false;
			 }*/
			torc::architecture::xilinx::TileRow queryTileRow =
					mDB.getTiles().getTileInfo(queryTileIndex).getRow();
			torc::architecture::xilinx::TileCol queryTileCol =
					mDB.getTiles().getTileInfo(queryTileIndex).getCol();
			if (queryTileRow < yMin)
				return false;
			if (queryTileRow > yMax)
				return false;
			if (queryTileCol < xMin)
				return false;
			if (queryTileCol > xMax)
				return false;
			return true;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}

	/**
	 * Translate site name to tile index.
	 * @param siteName Name of site to be translated.
	 */
	torc::architecture::xilinx::TileIndex AntiCoreBase::siteNameToTileIndex(string siteName) {
		bool found = false;
		const torc::architecture::Array<const torc::architecture::Sites::Site>
				& sites = mDB.getSites().getSites();
		boost::uint32_t siteCount = torc::architecture::xilinx::SiteCount(
				sites.getSize());

		boost::uint32_t i = 0;//SiteIndex
		for (i = 0; i < siteCount; i++) {
			const torc::architecture::Sites::Site& site = sites[i];
			if (site.getName() == siteName) {
				found = true;
				return site.getTileIndex();
				break;
			}
		}
		if (!found) {
			cout << "AntiCoreBase::Failed to find site " << siteName << endl;
			return torc::architecture::xilinx::TileIndex(-1);
		}

		const torc::architecture::Sites::Site& site = sites[i];
		return site.getTileIndex();
	}

	/**
	 * Setup bitmask.
	 */
	void AntiCoreBase::setupRouteBlocker() {
		try {
			int insideRegion = 0;
			int outsideRegion = 1;
			if (currentMode == ePartial)
				std::swap(insideRegion, outsideRegion);

			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCoreBase::setup Region hasn't been defined yet";
			for (int r = 0; r < mTiles.getRowCount(); r++)//torc::architecture::xilinx::TileRow
			{
				for (int c = 0; c < mTiles.getColCount(); c++) {
					if (!&mask[r][c])
						throw "Unallocated memory in mask matrix";
					else if (r >= yMin && r <= yMax && c >= xMin && c <= xMax)
						mask[r][c] = insideRegion;
					else
						mask[r][c] = outsideRegion;
				}
			}
			dumpMask();
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Dump the bitmask as ASCII art using 1s and 0s.
	 */
	void AntiCoreBase::dumpMask(void) {
		cout << endl;
		for (int r = 0; r < mTiles.getRowCount(); r++)//torc::architecture::xilinx::TileRow
		{
			for (int c = 0; c < mTiles.getColCount(); c++)//torc::architecture::xilinx::TileCol
			{
				if (mask[r][c])
					cout << "1";
				else
					cout << "0";
			}
			cout << endl;
		}
	}
	/**
	 * Return a vector of tilenames for all tiles that exist within the region.
	 */
	vector<string> AntiCoreBase::getRegionTiles() {
		vector<string> retVal;
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:getRegionTiles: Region hasn't been defined yet";
			for (int r = 0; r < mTiles.getRowCount(); r++)//torc::architecture::xilinx::TileRow
			{
				for (int c = 0; c < mTiles.getColCount(); c++)//torc::architecture::xilinx::TileCol
				{
					if (!&mask[r][c])
						cerr << "Unallocated memory in tile matrix row,col"
								<< r << "," << c << endl;
					else if (!mask[r][c]) {
						torc::architecture::xilinx::TileIndex i =
								mTiles.getTileIndex(
										torc::architecture::xilinx::TileRow(r),
										torc::architecture::xilinx::TileCol(c));
						retVal.push_back(
								mDB.getTiles().getTileInfo(i).getName());
					}
				}
			}
			for (unsigned int i = 0; i < retVal.size(); i++)
				cout << retVal[i] << " is in region" << endl;
			return retVal;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Block routing access to the to the tile specified by r,c.
	 * @param r Row of tile to be blocked.
	 * @param c Column of tile to be blocked.
	 */
	void AntiCoreBase::blockTileRoutes(torc::architecture::xilinx::TileRow r,
			torc::architecture::xilinx::TileCol c) {

		torc::architecture::xilinx::TileIndex tile = mTiles.getTileIndex(r, c);
		torc::architecture::xilinx::TileTypeIndex type = mTiles.getTileInfo(
				tile).getTypeIndex();
		bool done = false;
		//std::cout << r << "," << c << " type: " << type << std::endl;
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:block: Region hasn't been defined yet";

			// Loop over all wires in tile r,c
			for (boost::uint32_t wire = 0; wire
					< mTiles.getWireInfo(type).getSize(); wire++)//torc::architecture::xilinx::WireIndex
			{
				done = false;

				torc::architecture::Tilewire tilewire =
						torc::architecture::Tilewire(tile,
								torc::architecture::xilinx::WireIndex(wire));
				if (mDB.getWireUsage().isUsed(tilewire))
					continue;

				wires_buf.clear();
				mDB.expandSegment(tilewire, wires_buf, torc::architecture::DDB::eExpandDirectionSourceward);
				for (boost::uint32_t i = 0; i < wires_buf.size(); i++) {
					torc::architecture::ExtendedWireInfo info =
							torc::architecture::ExtendedWireInfo(mDB,
									wires_buf[i]);
					torc::architecture::xilinx::TileRow row = info.mTileRow;
					torc::architecture::xilinx::TileCol col = info.mTileCol;
					if (!mask[row][col]) {
						for (boost::uint32_t z = 0; z < wires_buf.size(); z++) {
							sources_buf.clear();
							mDB.expandTilewireSources(wires_buf[z], sources_buf);
							//mDB.expandWireSources(sources_buf, wires_buf[z]);

							// Loop over sources_buf until we find an unused source to enable the pip
							boost::uint32_t sbIndex = 0;
							// Ensure that we aren't blocking the clock tree
							static const boost::regex isClkWire(".*CLK_HROW.*");
							boost::smatch matches;
							while ((sbIndex < sources_buf.size()) && (!done)) {
								torc::architecture::ExtendedWireInfo
										sourceInfo =
												torc::architecture::ExtendedWireInfo(
														mDB,
														sources_buf[sbIndex]);
								torc::architecture::xilinx::TileRow sourceRow =
										sourceInfo.mTileRow;
								torc::architecture::xilinx::TileCol sourceCol =
										sourceInfo.mTileCol;
								torc::architecture::xilinx::TileIndex ti =
										mTiles.getTileIndex(sourceRow,
												sourceCol);
								torc::architecture::xilinx::TileTypeIndex tti =
										mTiles.getTileInfo(ti).getTypeIndex();

								string tileType = mTiles.getTileTypeName(tti);
								// If current source isn't used, enable to pip and output it to stdout
								if (!mDB.getWireUsage().isUsed(
										sources_buf[sbIndex]) && !regex_match(
										tileType, matches, isClkWire)) {
									cout << "# " << wire << ": "
											<< mTiles.getWireInfo(
													type,
													torc::architecture::xilinx::WireIndex(
															wire)).getName()
											<< " ";
									cout << endl;
									mDB.useArc(torc::architecture::Arc(sources_buf[sbIndex], wires_buf[z]));
									//mDB.markArcSegmentWires(
										//	sources_buf[sbIndex], wires_buf[z]);

									std::string pipString  = exportPipFromArc(sources_buf[sbIndex],wires_buf[z],  mDB);
									//std::string pipString = mDB.exportPipFromArc(
										//	sources_buf[sbIndex], wires_buf[z]);
;
									cout << pipString;
									if (placedXDLInput) {
										openpr::netlist::Pip* tempPip =
												new openpr::netlist::Pip(
														pipString, &mDB);
										placedXDLInput->insertPip(blockingNet,
												tempPip);
									}
									done = true;
								}
								// If current source isn't used, enable the pip and add it to the NetList
								else
									sbIndex++;
							}
						}
					}
					if (done)
						break;
				}
			}
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Block routing access to the to the tile specified by r,c.
	 * @param r Row of tile to be blocked.
	 * @param c Column of tile to be blocked.
	 */
  void AntiCoreBase::blockTileRoutesPartial(
		torc::architecture::xilinx::TileRow r,
		torc::architecture::xilinx::TileCol c) {
	torc::architecture::xilinx::TileIndex tile = mTiles.getTileIndex(r, c);
	torc::architecture::xilinx::TileTypeIndex type =
			mTiles.getTileInfo(tile).getTypeIndex();
	bool done = false;
	//std::cout << r << "," << c << " type: " << type << std::endl;
	try {
		if (xMin == torc::architecture::xilinx::TileCol(-1))
			throw "CAntiCore:block: Region hasn't been defined yet";

		// Loop over all wires in tile r,c
		for (boost::uint32_t wire = 0; wire
				< mTiles.getWireInfo(type).getSize(); wire++)//torc::architecture::xilinx::WireIndex
		{
			done = false;

			torc::architecture::Tilewire tilewire =
					torc::architecture::Tilewire(tile,
							torc::architecture::xilinx::WireIndex(wire));
			if (mDB.getWireUsage().isUsed(tilewire))
				continue;

			wires_buf.clear();
			mDB.expandSegment(tilewire, wires_buf,
					torc::architecture::DDB::eExpandDirectionSourceward);
			for (boost::uint32_t i = 0; i < wires_buf.size(); i++) {
				torc::architecture::ExtendedWireInfo info =
						torc::architecture::ExtendedWireInfo(mDB, wires_buf[i]);
				torc::architecture::xilinx::TileRow row = info.mTileRow;
				torc::architecture::xilinx::TileCol col = info.mTileCol;
				if (!mask[row][col]) {
					for (boost::uint32_t z = 0; z < wires_buf.size(); z++) {
						sources_buf.clear();
						mDB.expandTilewireSources(wires_buf[z], sources_buf);
						//mDB.expandWireSources(sources_buf, wires_buf[z]);

						// Loop over sources_buf until we find an unused source to enable the pip
						boost::uint32_t sbIndex = 0;
						// Ensure that we aren't blocking the clock tree
						static const boost::regex isClkWire(".*CLK_HROW.*");
						boost::smatch matches;
						while ((sbIndex < sources_buf.size()) && (!done)) {
							torc::architecture::ExtendedWireInfo sourceInfo =
									torc::architecture::ExtendedWireInfo(mDB,
											sources_buf[sbIndex]);
							torc::architecture::xilinx::TileRow sourceRow =
									sourceInfo.mTileRow;
							torc::architecture::xilinx::TileCol sourceCol =
									sourceInfo.mTileCol;
							torc::architecture::xilinx::TileIndex ti =
									mTiles.getTileIndex(sourceRow, sourceCol);
							torc::architecture::xilinx::TileTypeIndex tti =
									mTiles.getTileInfo(ti).getTypeIndex();

							string tileType = mTiles.getTileTypeName(tti);
							// If current source isn't used, enable to pip and output it to stdout
							if (!mDB.getWireUsage().isUsed(sources_buf[sbIndex])
									&& !mask[sourceRow][sourceCol]
									&& !regex_match(tileType, matches,
											isClkWire)) {
								cout << "# " << wire << ": "
										<< mTiles.getWireInfo(
												type,
												torc::architecture::xilinx::WireIndex(
														wire)).getName() << " ";
								cout << endl;
								mDB.useArc(
										torc::architecture::Arc(
												sources_buf[sbIndex],
												wires_buf[z]));
								//mDB.markArcSegmentWires(
								//	sources_buf[sbIndex], wires_buf[z]);

								std::string pipString =
										exportPipFromArc(sources_buf[sbIndex],
												wires_buf[z], mDB);
								//std::string pipString = mDB.exportPipFromArc(
								//	sources_buf[sbIndex], wires_buf[z]);
								;
								cout << pipString;
								if (placedXDLInput) {
									openpr::netlist::Pip* tempPip =
											new openpr::netlist::Pip(pipString,
													&mDB);
									placedXDLInput->insertPip(blockingNet,
											tempPip);
								}
								done = true;
							}
							// If current source isn't used, enable the pip and add it to the NetList
							else
								sbIndex++;
						}
					}
				}
				if (done)
					break;
			}
		}
	} catch (const char* err) {
		cerr << err << endl;
		exit(-1);
	}
}
	/**
	 * Block routing access to the tiles that surround the region.
	 */
	void AntiCoreBase::blockRoutes() {
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:run: Region hasn't been defined yet";
			boost::uint16_t maxRows = mTiles.getRowCount();
			for (boost::uint16_t i = 0; i < maxRows; i++) {
				std::cout << "#Blocking row " << i << std::endl;
				boost::uint16_t maxCols = mTiles.getColCount();
				for (boost::uint16_t j = 0; j < maxCols; j++) {
					if (mask[i][j] && currentMode == eStatic)
						blockTileRoutes(torc::architecture::xilinx::TileRow(i),
								torc::architecture::xilinx::TileCol(j));
					else if (mask[i][j])
						blockTileRoutesPartial(
								torc::architecture::xilinx::TileRow(i),
								torc::architecture::xilinx::TileCol(j));
				}
			}
			boost::uint32_t unusedWireCount = 0;
			boost::uint32_t noSourceCount = 0;
			boost::uint16_t rowCount = mTiles.getRowCount();
			for (boost::uint16_t i = 0; i < rowCount; i++) {
				std::cout << "#Blocking row " << i << std::endl;
				for (boost::uint16_t j = 0; j < mTiles.getColCount(); j++) {
					if (mask[i][j])
						unusedWireCount += checkEfficacy(
								torc::architecture::xilinx::TileRow(i),
								torc::architecture::xilinx::TileCol(j),
								noSourceCount);
				}
			}
			cerr << "After blocking, we have " << unusedWireCount
					<< " unused segments that leave the region" << endl;
			cerr << "Of these at least " << noSourceCount
					<< " were due to lack of available source wires" << endl;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Check how effectively par was blocked from entering the region.
	 * There are two important measurements here, first is how many region-crossing segments are left
	 * or "unused segments", this is the return value from this function.  Another important measurement
	 * is whether these segments were unused because of a bug, or because no suitable sources were available.
	 * If there were no sources available, then we have effectively blocked the segment even if it is unused!
	 * @param r Row of tile to be analysed.
	 * @param c Column of tile to be analysed.
	 * @param noSourceCount Keep track of how many segments are unused because the sources have been exhausted.
	 */
	boost::uint32_t AntiCoreBase::checkEfficacy(torc::architecture::xilinx::TileRow r,
			torc::architecture::xilinx::TileCol c,
			boost::uint32_t& noSourceCount) {
		torc::architecture::xilinx::TileIndex tile = mTiles.getTileIndex(r, c);
		//mTiles.mTileMap[boost::uint16_t(r)][boost::uint16(c)];
		torc::architecture::xilinx::TileTypeIndex type = mTiles.getTileInfo(
				tile).getTypeIndex();
		//mTiles.mTiles[mTiles.mTileMap[boost::uint16_t(r)][boost::uint16_t(
		//	c)]].getTileTypeIndex();
		//std::cout << r << "," << c << " type: " << type << std::endl;
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:block: Region hasn't been defined yet";

			// Loop over all wires in tile r,c
			torc::architecture::xilinx::WireIndex unusedWires;

			boost::uint16_t wireSize = mTiles.getWireCount(type);
			//mTiles.mWires[type].wireSize;
			for (boost::uint16_t wire = 0; wire < wireSize; wire++) {
				torc::architecture::Tilewire tilewire =
						torc::architecture::Tilewire(tile,
								torc::architecture::xilinx::WireIndex(wire));
				if (mDB.getWireUsage().isUsed(tilewire))
					continue;

				wires_buf.clear();
				mDB.expandSegment(tilewire, wires_buf, torc::architecture::DDB::eExpandDirectionSourceward);
				bool segmentUnused = false;

				for (boost::uint16_t i = 0; i < wires_buf.size(); i++) {
					torc::architecture::ExtendedWireInfo info =
							torc::architecture::ExtendedWireInfo(mDB,
									wires_buf[i]);
					torc::architecture::xilinx::TileRow row = info.mTileRow;
					torc::architecture::xilinx::TileCol col = info.mTileCol;
					if (!mask[(boost::uint16_t) row][(boost::uint16_t) col]) {
						if (!mDB.getWireUsage().isUsed(wires_buf[i])) {
							unusedWires++;
							segmentUnused = true;
							break;
						}
					}
				}
				if (segmentUnused) {
					bool done = false;
					for (boost::uint32_t z = 0; z < wires_buf.size(); z++) {
						sources_buf.clear();
						mDB.expandTilewireSources(wires_buf[z], sources_buf);

						// Loop over sources_buf until we find an unused source to enable the pip
						boost::uint32_t sbIndex = 0;
						while ((sbIndex < sources_buf.size()) && (!done)) {
							//								torc::architecture::ExtendedWireInfo info = mDB.getExtendedWireInfo(sources_buf[sbIndex]);
							//								int row = info.mRow;
							//								int col = info.mCol;
							// If current source isn't used, enable to pip and output it to stdout
							if (!mDB.getWireUsage().isUsed(sources_buf[sbIndex]))
								done = true;
							// If current source isn't used, enable the pip and add it to the NetList
							else
								sbIndex++;
						}
					}
					if (!done)
						noSourceCount++;
				}
			}
			return unusedWires;
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Block routing access to the tiles that surround the region.
	 * @param placedXDL Pointer to a NetList object that represents the XDL file.
	 * @param blockingNet name of the net that blocking routes should be added to.
	 */
	void AntiCoreBase::blockRoutes(string placedXDLPath, string blockedXDLPath,
			string blockingNetName) {
		try {
			if (xMin == torc::architecture::xilinx::TileCol(-1))
				throw "CAntiCore:run: Region hasn't been defined yet";
			if (placedXDLPath != "" && blockingNetName == "")
				throw "CAntiCoreBase::blockRoutes: Can't add blocking routes to netlist without name of net to add to";

			if (placedXDLPath != "") {
				placedXDLInput = new openpr::netlist::NetList(placedXDLPath,
						blockedXDLPath, &mDB);
				blockingNet = placedXDLInput->findNet(blockingNetName);
				if (!blockingNet) {
					delete placedXDLInput;
					placedXDLInput = NULL;
					throw "CAntiCoreBase::blockRoutes: Couldn't find blocking net in supplied XDL file.";
				}
			}
			blockRoutes();
			if (placedXDLInput) {
				placedXDLInput->printData(placedXDLInput->outputXDL);
				delete placedXDLInput;
			}
		} catch (const char* err) {
			cerr << err << endl;
			exit(-1);
		}
	}
	/**
	 * Merge the clock tree net from the static design into the partial design's xdl file.
	 * @param staticFullXdlPath string storing full path to static xdl file.
	 * @param partialPlacedXdlPath string storing full path to partial design's placed xdl file.
	 * @param partialMergedXdlPath string storing full path to desired output file.
	 * @param clockNetName Name of clock net in both design files...these MUST be the same (for now).
	 */
	bool AntiCoreBase::mergeClockTree(string staticFullXdlPath, string partialPlacedXdlPath,
			string partialMergedXdlPath, vector<string> clockNetNames) {
		openpr::netlist::NetList staticNetList(staticFullXdlPath, "temp.xdl",
				&mDB);
		openpr::netlist::NetList partialNetList(partialPlacedXdlPath,
				partialMergedXdlPath, &mDB);

		for (boost::uint32_t i = 0; i < clockNetNames.size(); i++) {
			std::string clockNetName = "\"" + clockNetNames[i] + "\"";
			openpr::netlist::Net* staticClockNet = staticNetList.findNet(
					clockNetName);
			std::cout << "COpenPR::mergeClockTree: Attemptin to merge "
					<< clockNetName << endl;
			if (!staticClockNet) {
				std::cerr
						<< "CAntiCoreBase::mergeClockTree: Unable to located clock net in static XDL file."
						<< std::endl;
				return false;
			}
			openpr::netlist::Net* partialClockNet = partialNetList.findNet(
					clockNetName);
			if (!partialClockNet) {
				partialClockNet = new openpr::netlist::Net(staticClockNet);
				partialClockNet->clearInPins();
				partialNetList.insertNet(partialClockNet);
				cerr
						<< "CAntiCoreBase::mergeClockTree: Unable to located clock net in partial XDL file."
						<< endl;
			}
			partialClockNet->mergePips(*staticClockNet);
		}
		partialNetList.printData();
		return true;
	}
	AntiCoreBase::~AntiCoreBase() {
		for (int i = 0; i < mTiles.getRowCount(); i++)
			delete[] mask[i];
		delete[] mask;
	}

}
