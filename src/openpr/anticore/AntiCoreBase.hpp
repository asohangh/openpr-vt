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

#ifndef ADB_ANTICORE_H
#define ADB_ANTICORE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <ctype.h>
#include "torc/Architecture.hpp"
#include "openpr/anticore/ProhibitRange.hpp"
#include "openpr/netlist/NetList.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>


namespace openpr {
using namespace std;
typedef std::string string;
/** Whether object is in Static or Partial mode determines behavior. */
enum eMode {
	eStatic, ePartial
};
/** Stores relationship between the name of a bus macro to its type */
typedef boost::unordered_map<string, string> bmNameToTypeMap;

class AntiCoreBase {
protected:
	/** database reference */
	torc::architecture::DDB& mDB;
	/** tile and tile type information reference */
	const torc::architecture::Tiles& mTiles;
	/** segment information reference */
	const torc::architecture::Segments& mSegments;

	/** scratch segment buffer */
	torc::architecture::TilewireVector wires_buf;
	/** scratch sink buffer */
	torc::architecture::TilewireVector sinks_buf;
	/** scratch source buffer */
	torc::architecture::TilewireVector sources_buf;

	/** Two tile info objects store x,y coordinates that define AntiCore rectangle. */
	torc::architecture::TileInfo startTile;
	torc::architecture::TileInfo endTile;

	/** Local variables to integrate with torc's col and row */
	torc::architecture::xilinx::TileCol xMin;
	torc::architecture::xilinx::TileCol xMax;
	torc::architecture::xilinx::TileRow yMin;
	torc::architecture::xilinx::TileRow yMax;
	/** Stores the current operating mode of the AntiCore */
	eMode currentMode;

	/** Maps between a site type CLB, IOB, etc. and a range of those sites to be prohibited. */
	map<std::string, openpr::prohibitRange> prohibitedSites;
	/** Maps between Tile indices and the sites within that tile. */
	typedef std::multimap<torc::architecture::xilinx::TileIndex,
			torc::architecture::Sites::Site> tileToSiteMap;
	std::multimap<torc::architecture::xilinx::TileIndex,
			torc::architecture::Sites::Site> siteMap;
	/** A Vector of the tiles that contain bus macros.  Currently this only knows about macros generated by AntiCore code.*/
	vector<torc::architecture::xilinx::TileIndex> macroTiles;
	/** Current macros have a fixed width of 8, perhaps this will change in the future. */
	const int macroWidth;// = 8;
	/** Maps between the uniquely generated name of the bus macro and the type of bus macro. */
	bmNameToTypeMap busMacroMap;

	/** Pointer to NetList object representing a post-place XDL netlist to be route blocked. */
	openpr::netlist::NetList* placedXDLInput;
	/** Pointer to Net object representing the net on which we will insert blocking pips. */
	openpr::netlist::Net* blockingNet;

	/** Mask representing which tiles are within the region and which are without. */
	int** mask;

	/** Vector storing valid row indices for region boundaries. */
	vector<torc::architecture::xilinx::TileRow> validBoundaries;

public:
	/**
	 * construct Anticore object.
	 * @param inDB CDB database.
	 */
	AntiCoreBase(torc::architecture::DDB & inDB);
	/**
	 * Construct an AntiCore object, and define boundaries of AntiCore based
	 * upon sliceA and sliceB tile coordinates.
	 * @param inDB CDB  enabled database for access to browser data.
	 * @param sliceA String representing bottom corner of AntiCore region.
	 * @param sliceB String representing top corner of AntiCore region.
	 */
	AntiCoreBase(torc::architecture::DDB & inDB, string sliceA, string sliceB);
	/**
	 * Allocate memory for bitmask.
	 * @todo Replace integer matrix with dynamic_bitset.
	 */
	void allocateMask();
	/**
	 * Set the mode to determine behavior of anticore.
	 * @param newMode enum value of mode to switch to.
	 */
	void setMode(eMode newMode);
	/**
	 * Build vector which stores all valid boundary rows for this device.
	 * @param tilesPerRegion Number of tile rows per clock region.
	 */
	void buildValidBoundaries(const int tilesPerRegion);
	std::string exportPipFromArc(torc::architecture::Tilewire source, torc::architecture::Tilewire sink, torc::architecture::DDB& mDB);
	/**
	 * Build map between tile index and sites contained within.
	 */
	void buildSiteMap();
	/**
	 * Import an XDL file and reserve all currently used pips.
	 */
	void importXDL(void);
	/**
	 * Return values of four region vertices by reference.
	 * @param _xMin Minimum x-coordinate.
	 * @param _xMax Maximum x-coordinate.
	 * @param _yMin Miniumum y-coordinate.
	 * @param _yMax Maximum y-coordinate.
	 */
	void getRegionVertices(torc::architecture::xilinx::TileCol& _xMin,
			torc::architecture::xilinx::TileCol& _xMax,
			torc::architecture::xilinx::TileRow& _yMin,
			torc::architecture::xilinx::TileRow& _yMax);
	/**
	 * Set values of four region vertices.
	 * @param _xMin Minimum x-coordinate.
	 * @param _xMax Maximum x-coordinate.
	 * @param _yMin Miniumum y-coordinate.
	 * @param _yMax Maximum y-coordinate.
	 */
	void setRegionVertices(torc::architecture::xilinx::TileCol _xMin,
			torc::architecture::xilinx::TileCol _xMax,
			torc::architecture::xilinx::TileRow _yMin,
			torc::architecture::xilinx::TileRow _yMax);
	/**
	 * Update boundaries of Anticore region based upon siteA and siteB tile
	 * coordinates.
	 * @param siteA String representing bottom corner of AntiCore region.
	 * @param siteB String representing top corner of AntiCore region.
	 */
	void updateRegion(string siteA, string siteB) ;
	/**
	 * Update boundaries of Anticore region based upon sliceA and sliceB tile
	 * coordinates.
	 * @param siteA String representing bottom corner of AntiCore region.
	 * @param siteB String representing top corner of AntiCore region.
	 */
	void updateRegionExpand(string siteA, string siteB) ;
	/**
	 * Expand the region so that the first column includes the interconnect tiles.
	 */
	void expandRegionToINT();
	/**
	 * Expand the region by the specified number of tiles.
	 * @param increment The number of tiles the region needs to be expanded by.
	 */
	void expandRegion(int increment);
	/**
	 * Shrink the region by the specified number of tiles.
	 * @param decrement The number of tiles the region needs to be shrunken by.
	 */
	void shrinkRegion(int decrement) ;
	/**
	 * Ensure that region spans multiples of clock regions.
	 */
	bool validateRegion() ;
	/**
	 * Generate PROHIBIT constraints that can be copied into the ucf file.
	 */
	void blockSites(void);

	/**
	 * Write the necessary Prohibit constraints to stdout
	 */
	void genProhibitConstraints() ;
	/**
	 * Parse the specified UCF file and update the region to reflect the AREA_GROUP constraints found within.
	 * @param ucfFile Path/filename of UCF File.
	 * @param dynamicAGName the name of the PR region in the UCF file.
	 */
	bool retrieveDynamicRegion(fstream& constraintsFile, string dynamicAGName);
	/**
	 * Write the necessary Prohibit constraints to a specified ucf file.
	 * @param ucfFile fstream object representing the ucf file
	 */
	void genProhibitConstraints(fstream & ucfFile) ;
	/**
	 * Generate placement of bus macros, and return map representing relationship of busmacro name to type.
	 * @param busWidth The width of bus entering the region.
	 * @param ucfFile fstream for output of constraints
	 * @param busMacroPrefix String containing the prefix of teh busMacro name.
	 */
	virtual bmNameToTypeMap& genMacroPlacement(int busWidth, fstream& ucfFile,
			string busMacroPrefix) = 0;
	/**
	 * Generate placement constraints to block site placement within the region.
	 * @param busWidth The width of bus entering the region.
	 * @param ucfFile fstream for output of constraints
	 */
	void genPlaceConstraints(int busWidth, fstream & ucfFile) ;
	/**
	 * Given a specific tile, find a SLICE to place the macro in.
	 * @param ti torc::architecture::xilinx::TileIndex of tile to look in.
	 * @param minOrMax Indicates whether macro should be placed at most extreme
	 * slice or least extreme slice in tile.
	 */
	virtual string placeMacro(torc::architecture::xilinx::TileIndex ti,
			bool minOrMax) = 0;
	/**
	 * Return site type (CLB, IOB, etc.).
	 * @param site Site to be queried.
	 */
	string getSiteType(string site);

	/**
	 * Determine whether tile is within the dynamic region.  This function
	 * is used by the blockSites function currently.
	 * @param queryTileIndex Tile index to be queried.
	 */
	bool inRegion(torc::architecture::xilinx::TileIndex queryTileIndex);

	/**
	 * Translate site name to tile index.
	 * @param siteName Name of site to be translated.
	 */
	torc::architecture::xilinx::TileIndex siteNameToTileIndex(string siteName) ;

	/**
	 * Setup bitmask.
	 */
	void setupRouteBlocker();
	/**
	 * Dump the bitmask as ASCII art using 1s and 0s.
	 */
	void dumpMask(void);
	/**
	 * Return a vector of tilenames for all tiles that exist within the region.
	 */
	vector<string> getRegionTiles();
	/**
	 * Block routing access to the to the tile specified by r,c.
	 * @param r Row of tile to be blocked.
	 * @param c Column of tile to be blocked.
	 */
	void blockTileRoutes(torc::architecture::xilinx::TileRow r,
			torc::architecture::xilinx::TileCol c) ;
	/**
	 * Block routing access to the to the tile specified by r,c.
	 * @param r Row of tile to be blocked.
	 * @param c Column of tile to be blocked.
	 */
	void blockTileRoutesPartial(torc::architecture::xilinx::TileRow r,
			torc::architecture::xilinx::TileCol c);
	/**
	 * Block routing access to the tiles that surround the region.
	 */
	void blockRoutes() ;
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
	boost::uint32_t checkEfficacy(torc::architecture::xilinx::TileRow r,
			torc::architecture::xilinx::TileCol c,
			boost::uint32_t& noSourceCount) ;
	/**
	 * Block routing access to the tiles that surround the region.
	 * @param placedXDL Pointer to a NetList object that represents the XDL file.
	 * @param blockingNet name of the net that blocking routes should be added to.
	 */
	void blockRoutes(string placedXDLPath, string blockedXDLPath,
			string blockingNetName);
	/**
	 * Merge the clock tree net from the static design into the partial design's xdl file.
	 * @param staticFullXdlPath string storing full path to static xdl file.
	 * @param partialPlacedXdlPath string storing full path to partial design's placed xdl file.
	 * @param partialMergedXdlPath string storing full path to desired output file.
	 * @param clockNetName Name of clock net in both design files...these MUST be the same (for now).
	 */
	bool mergeClockTree(string staticFullXdlPath, string partialPlacedXdlPath,
			string partialMergedXdlPath, vector<string> clockNetNames);
	~AntiCoreBase() ;
}; // class CAntiCore
} // namespace adb
#endif // ADB_CANTICORE_H
