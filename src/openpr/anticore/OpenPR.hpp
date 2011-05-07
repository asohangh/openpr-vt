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

#ifndef OPENPR_H
#define OPENPR_H 

#include "openpr/anticore/AntiCoreV4.hpp"
#include "openpr/anticore/AntiCoreV5.hpp"
#include "torc/Architecture.hpp"
#include "openpr/bitstream/v5_bitstream.h"
#include "openpr/bitstream/v4_bitstream.h"
#include<fstream>
#include<string>
#include<boost/archive/xml_oarchive.hpp> 
#include<boost/archive/xml_iarchive.hpp>
#include<boost/serialization/vector.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/filesystem.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/filesystem/operations.hpp>
#include <stdio.h>
#include <unistd.h>

namespace openpr {
class openPR {
	friend class boost::serialization::access;
private:
	string designName;
	string projectPath;
	string dynamicAGName;
	string staticPath;
	string partialPath;
	string ucfPath;
	string staticPlacedXDLPath;
	string deviceName;
	string busMacroPrefix;
	string passThroughNetName;
	string passThroughNet2;
	vector<string> busMacroNames;
	vector<string> clkNetNames;
	bool regionDefined;
	bool isPartial;
	int busWidth;
	torc::architecture::xilinx::TileRow yMin;
	torc::architecture::xilinx::TileRow yMax;
	torc::architecture::xilinx::TileCol xMin;
	torc::architecture::xilinx::TileCol xMax;
	boost::uint16_t l_yMin;
	boost::uint16_t l_yMax;
	boost::uint16_t l_xMin;
	boost::uint16_t l_xMax;
	torc::architecture::DDB* db;
	openpr::AntiCoreBase* anticore;
	/** Constant defining relative location of build path */
	const boost::filesystem::path buildPath;
	/** Constant defining the relative path of a placed xdl file. */
	boost::filesystem::path placedXdlPath;
	/** Constant defining the relative path of the routeBlocked xdl file. */
	boost::filesystem::path blockedXdlPath;
	/** Constant defining the relative path of the routeBlocked xdl file. */
	boost::filesystem::path mergedXdlPath;
	/** Constant defining the relative path of the routeBlocked xdl file. */
	boost::filesystem::path routedXdlPath;
	/** Constant defining the relative path of the routeBlocked xdl file. */
	boost::filesystem::path fullXdlPath;
	/** Constant defining the relative path of the full bitstream file. */
	boost::filesystem::path fullBsPath;
	/** Constant defining the relative path of the partial bitstream file. */
	boost::filesystem::path partialBsPath;
	/** Constant defining the relative path of the Physical constraints file*/
	boost::filesystem::path pcfPath;
	/** Constant defining the relative path of the UCF file*/
	boost::filesystem::path fullUcfPath;
	/** Constant defining the relative path of the busmacros*/
	const boost::filesystem::path busMacroPath;
	/** Constant defining the relative path of the route passthrough script*/
	const boost::filesystem::path routePTScriptPath;
	/** Constant defining the relative path of the unroute passthrough script*/
	const boost::filesystem::path unroutePTScriptPath;
	/** Constant defining relative path to ucfFile
	 const boost::filesystem::path ucfPath;*/
	/**
	 * Use boost.serialize to define an XML archive for the OpenPR class.
	 * @param ar Archive template class.
	 * @param version Defines which version of the archive this is, currently unused.
	 */
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		l_yMin = (boost::uint16_t) yMin;
		l_yMax = (boost::uint16_t) yMax;
		l_xMin = (boost::uint16_t) xMin;
		l_xMax = (boost::uint16_t) xMax;

		ar & BOOST_SERIALIZATION_NVP(designName);
		ar & BOOST_SERIALIZATION_NVP(projectPath);
		ar & BOOST_SERIALIZATION_NVP(staticPath);
		ar & BOOST_SERIALIZATION_NVP(partialPath);
		ar & BOOST_SERIALIZATION_NVP(isPartial);
		ar & BOOST_SERIALIZATION_NVP(ucfPath);
		ar & BOOST_SERIALIZATION_NVP(dynamicAGName);
		ar & BOOST_SERIALIZATION_NVP(busMacroPrefix);
		ar & BOOST_SERIALIZATION_NVP(busMacroNames);
		ar & BOOST_SERIALIZATION_NVP(passThroughNetName);
		ar & BOOST_SERIALIZATION_NVP(passThroughNet2);
		ar & BOOST_SERIALIZATION_NVP(clkNetNames);
		ar & BOOST_SERIALIZATION_NVP(busWidth);
		ar & BOOST_SERIALIZATION_NVP(deviceName);
		ar & BOOST_SERIALIZATION_NVP(regionDefined);
		ar & BOOST_SERIALIZATION_NVP(l_yMin);
		ar & BOOST_SERIALIZATION_NVP(l_yMax);
		ar & BOOST_SERIALIZATION_NVP(l_xMin);
		ar & BOOST_SERIALIZATION_NVP(l_xMax);

		yMin = torc::architecture::xilinx::TileRow(l_yMin);
		yMax = torc::architecture::xilinx::TileRow(l_yMax);
		xMin = torc::architecture::xilinx::TileCol(l_xMin);
		xMax = torc::architecture::xilinx::TileCol(l_xMax);
	}
public:
	/**
	 * Default constructor.
	 */
	openPR();
	~openPR();
	void buildRelativePaths();
	/**
	 * Setup CAntiCore object depending on the device type.
	 */
	void setupAntiCore();
	/**
	 * Setup dimensions of dynamic region based on AREA_GROUPs defined in UCF file.
	 */
	void setupDynamicRegion();
	/**
	 * Generate all placement constraints and link appropriate bus macro files.
	 */
	bool genPlaceConstraints();
	/**
	 * Generate bus macro placement constraints and link the correct bus macro names to the right files.
	 */
	bool placeMacros();
	/**
	 * Generate constraints that block placement within sites.
	 */
	bool siteBlocker();
	/**
	 * Generate blocking routes in design.
	 */
	bool routeBlocker();
	/**
	 * Pull clocktree from static design and merge it with partial design.
	 */
	bool mergeClockTree() ;
	/**
	 * Take prefix of blocking net and create the full net name that can be blocked.
	 */
	std::string buildBlockingNet();
	/**
	 * Generate passthrough scripts automatically.
	 */
	bool genPassThroughScripts();
	/**
	 * Generate constraints that lock both busmacro nets and blocking nets
	 */
	bool genLockConstraints();
	/**
	 * Generate a partial bitstream for the defined region from a full bitstream of the partial design.
	 */
	bool genPartialBitstream();
};
}

#endif
