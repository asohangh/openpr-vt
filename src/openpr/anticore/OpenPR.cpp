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
 * OpenPR.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: jacouch
 */
#include "openpr/anticore/OpenPR.hpp"
namespace openpr {
openPR::openPR() :
	buildPath("build"), busMacroPath("src/precompiled"), routePTScriptPath(
			"src/route_passthroughs.scr"), unroutePTScriptPath(
			"src/unroute_passthroughs.scr") {
	db = NULL;
	anticore = NULL;
	regionDefined = false;
}
;
openPR::~openPR() {

}
void openPR::buildRelativePaths() {
	try {
		if (designName == "")
			throw "COpenPR::buildRelativePaths: No design name specified in project file.";
		placedXdlPath = buildPath / (designName + "_placed.xdl");
		blockedXdlPath = buildPath / (designName + "_blocked.xdl");
		mergedXdlPath = buildPath / (designName + "_merged.xdl");
		routedXdlPath = buildPath / (designName + "_routed.xdl");
		fullXdlPath = buildPath / (designName + "_full.xdl");
		fullBsPath = buildPath / (designName + "_full.bit");
		partialBsPath = buildPath / (designName + "_partial.bit");
		pcfPath = buildPath / (designName + ".pcf");
		fullUcfPath = get_current_dir_name() / buildPath / ucfPath;
	} catch (const char* err) {
		cerr << err << endl;
		exit(-1);
	}
}
/**
 * Setup CAntiCore object depending on the device type.
 */
void openPR::setupAntiCore() {
	try {
		if (deviceName == "")
			throw "openPR:setupAntiCore: Invalid device name";
		// Strip package information from device name if its there
		std::size_t packageOffset = deviceName.find("-");
		string tempDevName;
		if (packageOffset != 0)
			tempDevName = deviceName.substr(0, packageOffset);
		// Allocate CDB object
		cout << "creating DDB" << endl;
		db = new torc::architecture::DDB(tempDevName);
		cout << "done creating DDB" << endl;
		if (db->getFamilyName() == "Virtex4")
			anticore = new openpr::AntiCoreV4(*db);
		else if (db->getFamilyName() == "Virtex5")
			anticore = new openpr::AntiCoreV5(*db);
		else
			throw "openPR:setupAntiCore: Unsupported device architecture.";
	} catch (const char* err) {
		cout << "SetupAntiCoreError" << endl;
		cerr << err << endl;
		exit(-1);
	} catch (ifstream::failure e) {
    cerr 
      << e.what()
      << ": Unable to open device database file, check OpenPR installation"
      << endl;
		exit(-1);
  }
}
/**
 * Setup dimensions of dynamic region based on AREA_GROUPs defined in UCF file.
 */
void openPR::setupDynamicRegion() {
	if (!anticore)
		setupAntiCore();
	try {
		fullUcfPath = get_current_dir_name();
		fullUcfPath = fullUcfPath / ucfPath;
		fullUcfPath.normalize();

		cerr << "Opening file " << fullUcfPath.string() << endl;

		fstream ucfFile(fullUcfPath.string().c_str(), fstream::in);
		if (!ucfFile.good())
			throw "COpenPR:siteBlocker: Unable to open UCF file for read.";
		if (dynamicAGName == "")
			throw "COpenPR:siteBlocker: No Dynamic AREA_GROUP name specified.";
		anticore->retrieveDynamicRegion(ucfFile, dynamicAGName);
		ucfFile.close();
		anticore->getRegionVertices(xMin, xMax, yMin, yMax);
		regionDefined = true;
	} catch (const char* err) {
		cerr << err << endl;
		exit(-1);
	}
}
/**
 * Generate all placement constraints and link appropriate bus macro files.
 */
bool openPR::genPlaceConstraints() {
	bool success = true;
	success &= placeMacros();
	success &= siteBlocker();
	return success;
}
/**
 * Generate bus macro placement constraints and link the correct bus macro names to the right files.
 */
bool openPR::placeMacros() {
	using boost::filesystem::exists;
	using boost::filesystem::path;

	if (!anticore)
		setupAntiCore();
	try {
		if (!regionDefined)
			setupDynamicRegion();
		else
			anticore->setRegionVertices(xMin, xMax, yMin, yMax);
		// Open ucf file for constraint output

		fullUcfPath = get_current_dir_name();
		fullUcfPath = fullUcfPath / ucfPath;
		fullUcfPath.normalize();
		fstream ucfFile(fullUcfPath.string().c_str(), fstream::out
				| fstream::app);
		if (!ucfFile.good())
			throw "COpenPR:placeMacros: Unable to open UCF file for write.";

		bmNameToTypeMap busMacroMap = anticore->genMacroPlacement(busWidth,
				ucfFile, busMacroPrefix);
		bmNameToTypeMap::iterator it = busMacroMap.begin();
		path fullPath;
		if (isPartial)
			fullPath = partialPath;
		else
			fullPath = staticPath;

		// Extract the busMacros id number from the filename
		static const boost::regex extractId(".*_(\\d*)");
		boost::smatch matches;

		while (it != busMacroMap.end()) {
			cout << "COpenPR::placeMacros: Linking macros to correct file.";
			path bmSourcePath = (std::string(get_current_dir_name()) + "/../")
					/ busMacroPath / (it->second + ".nmc");
			path bmDestPath = (std::string(get_current_dir_name()) + "/../")
					/ busMacroPath / (it->first + ".nmc");

			bmSourcePath.normalize();
			bmDestPath.normalize();

			cout << "Linking from bus macro source " << bmSourcePath.string()
					<< " to bus macro type " << bmDestPath.string() << endl;
			if (exists(bmDestPath) && is_symlink(bmDestPath))
				remove(bmDestPath);
			if (exists(bmSourcePath))
				boost::filesystem::create_symlink(bmSourcePath, bmDestPath);
			else
				throw "COpenPR::placeMacros: Unable to locate bus macro source file, please check project cfg. ";

			if (regex_match(it->first, matches, extractId))
				busMacroNames.push_back(it->first + "_" + boost::lexical_cast<
						string>(matches[1]));
			it++;
		}
		ucfFile.close();
		return true;
	} catch (const char* err) {
		cerr << err << endl;
		exit(-1);
	} catch (boost::filesystem::filesystem_error & e) {
		cerr
				<< "COpenPR::placeMacros: Symlink already exists, bus macro placement might not be correct."
				<< endl;
		cerr << e.what() << endl;
		return false;
	}
}
/**
 * Generate constraints that block placement within sites.
 */
bool openPR::siteBlocker() {
	if (!anticore)
		setupAntiCore();
	try {
		if (!regionDefined)
			setupDynamicRegion();
		else
			anticore->setRegionVertices(xMin, xMax, yMin, yMax);
		fullUcfPath = get_current_dir_name();
		fullUcfPath = fullUcfPath / ucfPath;
		fullUcfPath.normalize();

		fstream ucfFile(fullUcfPath.string().c_str(), fstream::out
				| fstream::app);
		if (!ucfFile.good())
			throw "COpenPR:siteBlocker: Unable to open UCF file for write.";
		anticore->genPlaceConstraints(busWidth, ucfFile);
		ucfFile.close();
		return true;
	} catch (const char* err) {
		cerr << err << endl;
		exit(-1);
	}
}
/**
 * Generate blocking routes in design.
 */
bool openPR::routeBlocker() {
	if (!anticore)
		setupAntiCore();
	try {
		if (isPartial)
			anticore->setMode(ePartial);
		if (!regionDefined)
			setupDynamicRegion();
		else
			anticore->setRegionVertices(xMin, xMax, yMin, yMax);

		buildRelativePaths();
		boost::filesystem::path blockRoutesInput = std::string(
				get_current_dir_name()) + "/../";
		boost::filesystem::path blockRoutesOutput = std::string(
				get_current_dir_name()) + "/../";
		if (isPartial) {
			blockRoutesInput = blockRoutesInput / partialPath / mergedXdlPath;
			blockRoutesOutput = blockRoutesOutput / partialPath
					/ blockedXdlPath;
		} else {
			blockRoutesInput = blockRoutesInput / staticPath / placedXdlPath;
			blockRoutesOutput = blockRoutesOutput / staticPath / blockedXdlPath;
		}

		passThroughNetName = buildBlockingNet();
		cout << "Blocking routes are in" << blockRoutesInput.string() << endl;
		anticore->blockRoutes(blockRoutesInput.string(),
				blockRoutesOutput.string(), passThroughNetName);

		return true;
	} catch (const char* err) {
		cerr << err << endl;
		exit(-1);
	}
}
/**
 * Pull clocktree from static design and merge it with partial design.
 */
bool openPR::mergeClockTree() {
	using boost::filesystem::exists;
	using boost::filesystem::path;

	buildRelativePaths();
	path staticFullXdlPath = get_current_dir_name();
	staticFullXdlPath = staticFullXdlPath / "/../" / staticPath / fullXdlPath;
	path partialPlacedXdlPath = get_current_dir_name();
	partialPlacedXdlPath = partialPlacedXdlPath / "/../" / partialPath
			/ placedXdlPath;
	path partialMergedXdlPath = get_current_dir_name();
	partialMergedXdlPath = partialMergedXdlPath / "/../" / partialPath
			/ mergedXdlPath;

	if (!anticore)
		setupAntiCore();
	if (!exists(staticFullXdlPath)) {
		cerr
				<< "COpenPR::mergeClockTree: Unable to open static routed xdl file "
				<< staticFullXdlPath.string() << endl;
		return false;
	}
	if (!exists(partialPlacedXdlPath)) {
		cerr
				<< "COpenPR::mergeClockTree: Unable to open partial placed xdl file"
				<< endl;
		return false;
	}
	if (exists(staticFullXdlPath) && exists(partialPlacedXdlPath))
		return anticore->mergeClockTree(staticFullXdlPath.string(),
				partialPlacedXdlPath.string(), partialMergedXdlPath.string(),
				clkNetNames);
	else
		return false;
}
/**
 * Take prefix of blocking net and create the full net name that can be blocked.
 */
std::string openPR::buildBlockingNet() {
	string blockingNet = "\"" + passThroughNetName + "<" + boost::lexical_cast<
			string>(busWidth - 4) + ">\"";
	return blockingNet;
}
/**
 * Generate passthrough scripts automatically.
 */
bool openPR::genPassThroughScripts() {
	using boost::filesystem::path;

	buildRelativePaths();
	path fullPath;
	path parentPath = "..";

	if (isPartial)
		fullPath = partialPath;
	else
		fullPath = staticPath;
	path rptFullPath = get_current_dir_name() / parentPath / fullPath / routePTScriptPath;
	path uptFullPath = get_current_dir_name() / parentPath / fullPath / unroutePTScriptPath;

	cout
		<< "Writing route_passthroughs.scr file to " << rptFullPath
		<< endl;
	cout
		<< "Writing unroute_passthroughs.scr file to " << uptFullPath
		<< endl;

	fstream rptScript(rptFullPath.string().c_str(), fstream::out);
	fstream uptScript(uptFullPath.string().c_str(), fstream::out);
	rptScript << "setattr main edit-mode Read-Write" << endl;
	uptScript << "setattr main edit-mode Read-Write" << endl;
	for (int i = 0; i < busWidth; i++) {
		rptScript << "select net " << passThroughNetName << "<" << i << ">"
				<< endl;
		uptScript << "select net " << passThroughNetName << "<" << i << ">"
				<< endl;
		if (isPartial) {
			rptScript << "select net " << passThroughNet2 << "<" << i << ">"
					<< endl;
			uptScript << "select net " << passThroughNet2 << "<" << i << ">"
					<< endl;
		}

	}
	rptScript << "autoroute -resource" << endl;
	uptScript << "delete" << endl;
	rptScript << "save" << endl;
	uptScript << "save" << endl;
	rptScript << "exit" << endl;
	uptScript << "exit" << endl;
	return true;
}
/**
 * Generate constraints that lock both busmacro nets and blocking nets
 */
bool openPR::genLockConstraints() {
	using boost::filesystem::path;
	buildRelativePaths();
	path fullPcfPath = std::string(get_current_dir_name()) + "/../";
	;
	if (isPartial)
		fullPcfPath = fullPcfPath / partialPath / pcfPath;
	else
		fullPcfPath = fullPcfPath / staticPath / pcfPath;
	fullPcfPath.normalize();
	if (!boost::filesystem::exists(fullPcfPath)) {
		cerr << "CLockConstraints: No PCF file exists, has map been run yet?"
				<< endl;
		return false;
	}

	// Write LOCK constraints to PCF file
	cout << "COpenPR::genLockConstraints: Writing to " << fullPcfPath.string()
			<< endl;
	fstream pcfFile(fullPcfPath.string().c_str(), fstream::out | fstream::app);
	for (boost::uint32_t i = 0; i < busMacroNames.size(); i++) {
		pcfFile << "NET \"" << busMacroNames[i] << "/*\" LOCK;" << endl;
		pcfFile << "NET \"" << busMacroNames[i] << "/*\" TIG;" << endl;
	}
	pcfFile << "NET " << buildBlockingNet() << " LOCK;" << endl;
	pcfFile << "NET " << buildBlockingNet() << " TIG;" << endl;
	pcfFile.close();

	return true;
}
/**
 * Generate a partial bitstream for the defined region from a full bitstream of the partial design.
 */
bool openPR::genPartialBitstream() {
	try {
		// ensure that we're generating a partial bitstream for the partial module
		if (!isPartial)
			throw "CPartialBitstream: Partial bitstreams can only be generated for partial module designs.";
		// If anticore hasn't been allocated, do it
		if (!anticore)
			setupAntiCore();
		// Set up the dynamic region, either from the UCF file or read from the project file
		if (!regionDefined)
			setupDynamicRegion();
		else
			anticore->setRegionVertices(xMin, xMax, yMin, yMax);

		buildRelativePaths();
		openpr::bitstream::bitstream* bsPointer;
		// Strip package information from device name if its there
		std::size_t packageOffset = deviceName.find("-");
		string tempDevName;
		if (packageOffset != 0)
			tempDevName = deviceName.substr(0, packageOffset);
		else
			tempDevName = deviceName;

		// allocate bitstream object
		if (db->getFamilyName() == "Virtex5")
			bsPointer = new openpr::bitstream::v5_bitstream(tempDevName);
		else if (db->getFamilyName() == "Virtex4")
			bsPointer = new openpr::bitstream::v4_bitstream(tempDevName);
		else {
			cerr << "INVALID ARCHITECTURE" << endl;
			exit(-1);
		}
		boost::filesystem::path fullPath = get_current_dir_name();
		boost::filesystem::path bsPath = fullPath / "/../" / partialPath
				/ fullBsPath;
		boost::filesystem::path pbsPath = fullPath / "/../" / partialPath
				/ partialBsPath;
		bsPath.normalize();
		pbsPath.normalize();
		if (!boost::filesystem::exists(bsPath))
			return false;
		cout << "COpenPR::genPartialBitstream: Opening bitstream file"
				<< pbsPath.string() << endl;
		bsPointer->mapBitstream();
		bsPointer->loadFile(bsPath.string());
		bsPointer->buildPartial(anticore->getRegionTiles());
		bsPointer->writeBitstream(pbsPath.string());
		delete bsPointer;
		return true;
	} catch (const char* err) {
		cerr << err << endl;
		exit(-1);
	}
}
}
