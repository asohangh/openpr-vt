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


#include "openpr/bitstream/bitstream.h"
#include "openpr/bitstream/v4_bitstream.h"
#include "openpr/bitstream/v5_bitstream.h"
#include <fstream>
#include <netinet/in.h>
#include <boost/regex.hpp>

namespace openpr {
namespace bitstream {
bitstream::bitstream(string device_name, bool frame_ecc) :
	my_dev(NULL), frame_array(NULL), findexToFaddr(NULL), isPartial(false) {
	// preinitialize variables
	designName.clear();
	deviceName.clear();
	designDate.clear();
	designTime.clear();
	bitstreamLength = 0;
	bitstreamWordCount = 0;
	// If the constructor was passed a valid device, create the device object
	if (device_name == "xc4vlx15")
		my_dev = new xc4vlx15();
	else if (device_name == "xc4vfx60")
		my_dev = new xc4vfx60();
	else if (device_name == "xc4vlx60")
		my_dev = new xc4vlx60();
	else if (device_name == "xc5vlx50")
		my_dev = new xc5vlx50();
	else if (device_name == "xc5vlx50t")
		my_dev = new xc5vlx50t();
	else if (device_name == "xc5vsx95t")
		my_dev = new xc5vsx95t();
	else if (device_name == "xc5vlx110t")
		my_dev = new xc5vlx110t();
	else if (device_name == "xc5vfx70t")
		my_dev = new xc5vfx70t();
	else
		fprintf(stderr, "Error, device %s is not supported\n",
				device_name.c_str());
	// If the device was created successfully, allocate memory for frame data
	// and the findexToFaddr array
	if (my_dev) {
		boost::uint32_t cfgSize = my_dev->get_cfg_size();
		boost::uint32_t wordCount = cfgSize * my_dev->get_frame_words();
		boost::uint32_t byteCount = wordCount << 2;
		boost::uint32_t bytesPerFrame = my_dev->get_frame_words() << 2;

		frame_array = new char*[cfgSize];
		mFrameData = new boost::uint8_t[byteCount];
		boost::uint8_t* ptr = mFrameData;
		if (!mFrameData)
			cerr << "mFrameData not allocated in constructor" << endl;
		for (boost::uint32_t frame = 0; frame < cfgSize; frame++, ptr
				+= bytesPerFrame)
			frame_array[frame] = (char*) ptr;
		findexToFaddr = new frame_addr[my_dev->get_cfg_size()];
		frameECC = frame_ecc;
	}
}

int bitstream::mapBitstream() {
	int x = 0;
	int y = 0;
	int frame_num;
	frame_addr far;
	tile_data * tile;
	printf("Chip has height of %d and width of %d\n", my_dev->chip_height(),
			my_dev->chip_width());
	// Print out the CFG sizes of the different block types
	for (int i = 0; i < my_dev->device::get_addressable_blk_types(); i++)
		printf("CFG Size of blktype %d is %d\n", i, my_dev->get_cfg_size(i));

	printf("Total CFG Size is %d frames\n", my_dev->get_cfg_size());

	for (y = 0; y < my_dev->chip_height(); y++) {
		for (x = 0; x < my_dev->chip_width(); x++) {
			far = my_dev->tilecoord_to_major(x, y);
			frame_num = my_dev->frame_offset(far);
			// If frame is of type BRAM_INT, add cfg_size of CLB block
			for (int i = 0; i < my_dev->get_blk_type(x); i++)
				frame_num += my_dev->get_cfg_size(i);
			buildItoaMap(far, frame_num, my_dev->get_tile_frames(x));
			if (false) {
				printf("Tile (%d,%d) has frame address %d_%d_%d_%d_%d\n", x, y,
						far.type, far.tb, far.row, far.col, 0);
				printf("Frame offset is %d\n", frame_num);
				printf("Byte offset within frame is %d\n", my_dev->tile_offset(
						x, y));
				printf("Tile spans %d frames\n", my_dev->get_tile_frames(x));
			}
			tile = new tile_data("", x, y, far, my_dev->get_tile_frames(x),
					frame_num, my_dev->tile_offset(x, y),
					frame_array[frame_num]);
			tile_map[tile->coord] = tile;
			int tile_type = my_dev->get_tile_type(x);
			if (tile_type == IOB) {
				tileMap[buildXDLName("INT", tile->coord)] = tile;
				cout << buildXDLName("INT", tile->coord) << endl;
			} else if (tile_type == CLB) {
				tileMap[buildXDLName("INT", tile->coord)] = tile;
				tileMap[buildXDLName("CLB", tile->coord)] = tile;
				tileMap[buildXDLName("CLBLL", tile->coord)] = tile;
				tileMap[buildXDLName("CLBLM", tile->coord)] = tile;
			} else if (tile_type == DSP48) {
				tileMap[buildXDLName("INT", tile->coord)] = tile;
				tileMap[buildXDLName("DSP", tile->coord)] = tile;
			} else if (tile_type == BRAM_INT) {
				tileMap[buildXDLName("INT", tile->coord)] = tile;
				mapBRAM(x, y);
			} else if (tile_type == TRANSCV) {
				tileMap[buildXDLName("INT", tile->coord)] = tile;
			}
		}
	}
	if (false) {
		for (int i = 0; i < my_dev->get_cfg_size(); i++)
			printf("Frame %d, has FAR %s\n", i, findexToFaddr[i].str().c_str());
	}
	buildGCLKItoaMap();
	return 0;
}
int bitstream::mapBRAM(int x, int y) {
	if (my_dev->get_tile_type(x) != BRAM_INT)
		return -1;
	frame_addr far = my_dev->bramcoord_to_major(x, y);
	int frame_num = my_dev->frame_offset(far);
	// Add offset to start of BRAM region in bitstream
	for (int i = 0; i < my_dev->get_blk_type(BRAM); i++)
		frame_num += my_dev->get_cfg_size(i);
	buildItoaMap(far, frame_num, my_dev->get_tile_frames(BRAM));
	if (false) {
		printf("Tile (%d,%d) has frame address %d_%d_%d_%d_%d\n", x, y,
				far.type, far.tb, far.row, far.col, 0);
		printf("Frame offset is %d\n", frame_num);
		printf("Byte offset within frame is %d\n", my_dev->tile_offset(x, y));
		printf("Tile spans %d frames\n", my_dev->get_tile_frames(BRAM));
		farToStruct(structToFar(far));
	}
	tile_data* tile = new tile_data("", x, y, far,
			my_dev->get_tile_frames(BRAM), frame_num,
			my_dev->tile_offset(x, y), frame_array[frame_num]);
	tileMap[buildXDLName("BRAM", tile->coord)] = tile;
	return 0;
}

void bitstream::buildItoaMap(frame_addr majorAddress,
		boost::uint32_t startIndex, boost::uint32_t tileFrames) {
	if (findexToFaddr[startIndex].type != -1)
		return;
	for (boost::uint32_t i = 0; i < tileFrames; i++) {
		majorAddress.mna = i;
		findexToFaddr[startIndex + i] = majorAddress;
	}
}

void bitstream::buildGCLKItoaMap() {
	// For every row find GCLK column and fill in frame_index to frame_offset translation
	int x = my_dev->get_gclk_index();
	for (int row = 0; row < my_dev->get_num_rows(); row++) {
		cout << "buildGCLKItoaMap: loop " << row << endl;
		int y = row * my_dev->get_row_height();
		frame_addr gclkAddr = my_dev->tilecoord_to_major(x, y);
		int offset = my_dev->frame_offset(gclkAddr) + my_dev->get_tile_frames(
				my_dev->get_tile_type(x));
		cout << "buildGCLKItoaMap: offset " << offset << endl;
		gclkAddr.col++;
		for (int i = offset; i < offset + my_dev->get_tile_frames(GCLK); i++) {
			findexToFaddr[i] = gclkAddr;
			gclkAddr.mna++;
			cout << "Setting index " << i << " to " << gclkAddr.str() << endl;
		}
	}
}

string bitstream::buildXDLName(string tileType, tile_coord coord) {
	ostringstream retVal;
	retVal << tileType << "_X" << coord.x << "Y" << coord.y;
	return retVal.str();
}


bitstream::~bitstream() {
	boost::unordered_map<tile_coord, tile_data*>::iterator tile_iterator;
	for (tile_iterator = tile_map.begin(); tile_iterator != tile_map.end(); tile_iterator++) {
		//tile_iterator->second->print();
		delete tile_iterator->second;
		tile_iterator->second = NULL;
	}
	if (findexToFaddr != NULL) {
		delete[] (findexToFaddr);
		findexToFaddr = NULL;
	}
	if (mFrameData != NULL) {
		delete[] (mFrameData);
		mFrameData = NULL;
	}
	if (frame_array != NULL) {
		delete[] (frame_array);
		frame_array = NULL;
	}
	if (my_dev != NULL) {
		delete (my_dev);
		my_dev = NULL;
	}
}

bool bitstream::expect(fstream& inStream, boost::uint8_t inExpected) {
	// read the actual data from the stream
	boost::uint8_t actual = 0;
	inStream.read((char*) &actual, sizeof(actual));
	// return equality
	return inExpected == actual;
}

bool bitstream::expect(fstream& inStream, boost::uint16_t inExpected) {
	// read the actual data from the stream
	boost::uint16_t actual = 0;
	inStream.read((char*) &actual, sizeof(actual));
	// return equality
	return inExpected == ntohs(actual);
}

bool bitstream::expect(fstream& inStream, boost::uint32_t inExpected) {
	// read the actual data from the stream
	boost::uint32_t actual = 0;
	inStream.read((char*) &actual, sizeof(actual));
	// return equality
	return inExpected == ntohl(actual);
}


void bitstream::readXilinxString(fstream& inStream, string& outString) {
	// read the string length
	boost::uint16_t length = 0;
	inStream.read((char*) &length, sizeof(length));
	length = ntohs(length);
	if (length > 0) {
		// create a buffer
		char* buffer = new char[length];
		// read the null-terminated string
		inStream.read(buffer, length);
		// copy the data into the string
		outString.assign(buffer, length - 1);
		delete[] buffer;
	} else {
		outString.clear();
	}
}

bool bitstream::readHeader(fstream& inStream, string& outDesignName,
		string& outDeviceName, string& outDesignDate, string& outDesignTime,
		boost::uint32_t& outBitstreamLength) {
	// assume success until we find otherwise
	bool success = true;
	// read the magic length
	success &= expect(inStream, (boost::uint16_t) 0x0009);
	// read the magic bytes
	success &= expect(inStream, (boost::uint32_t) 0x0ff00ff0);
	success &= expect(inStream, (boost::uint32_t) 0x0ff00ff0);
	success &= expect(inStream, (boost::uint8_t) 0x00);
	// read the mysterious 0x0001
	success &= expect(inStream, (boost::uint16_t) 0x0001);
	// read the 'a' byte
	success &= expect(inStream, (boost::uint8_t) 'a');
	// read the design name length
	readXilinxString(inStream, outDesignName);
	// read the 'b' byte
	success &= expect(inStream, (boost::uint8_t) 'b');
	// read the device name length
	readXilinxString(inStream, outDeviceName);
	// read the 'c' byte
	success &= expect(inStream, (boost::uint8_t) 'c');
	// read the design date length
	readXilinxString(inStream, outDesignDate);
	// read the 'd' byte
	success &= expect(inStream, (boost::uint8_t) 'd');
	// read the design time length
	readXilinxString(inStream, outDesignTime);
	// read the 'e' byte
	success &= expect(inStream, (boost::uint8_t) 'e');
	// read the inStream length
	inStream.read((char*) &outBitstreamLength, sizeof(outBitstreamLength));
	outBitstreamLength = ntohl(outBitstreamLength);

	// some versions of the tools leave spaces inside the date and time
	size_t pos;
	pos = 0;
	while ((pos = outDesignDate.find(' ', pos)) != string::npos)
		outDesignDate[pos] = '0';
	pos = 0;
	while ((pos = outDesignTime.find(' ', pos)) != string::npos)
		outDesignTime[pos] = '0';

	// spit out a debugging string
	cout << "Design " << outDesignName << " (" << outDeviceName << ") @ "
			<< outDesignDate << " " << outDesignTime << ": "
			<< outBitstreamLength << " bytes (" << (outBitstreamLength >> 2)
			<< " words)" << endl;

	// return the result
	return success;
}
bool bitstream::write(fstream& outStream, boost::uint8_t outVal) {
	// read the actual data from the stream
	boost::uint8_t actual = outVal;
	outStream.write((char*) &actual, sizeof(actual));
	return true;
}

bool bitstream::write(fstream& outStream, boost::uint16_t outVal) {
	// read the actual data from the stream
	boost::uint16_t actual = htons(outVal);
	outStream.write((char*) &actual, sizeof(actual));
	// return equality
	return true;
}

bool bitstream::write(fstream& outStream, boost::uint32_t outVal) {
	// read the actual data from the stream
	boost::uint32_t actual = htonl(outVal);
	outStream.write((char*) &actual, sizeof(actual));
	// return equality
	return true;
}

void bitstream::writeXilinxString(fstream& outStream, string inString) {
	// write the string length
	boost::uint16_t length = inString.size() + 1;
	length = htons(length);
	outStream.write((char*) &length, sizeof(length));
	length = ntohs(length) - 1;
	if (length > 0) {
		// write the null-terminated string
		outStream.write(inString.c_str(), length);
		boost::uint8_t null = 0;
		outStream.write((char*) &null, 1);
	}
}

bool bitstream::writeHeader(fstream& outStream) {
	cout << "Writing header" << endl;
	bool success = false;
	// write the magic length
	success &= write(outStream, (boost::uint16_t) 0x0009);
	// write the magic bytes
	success &= write(outStream, (boost::uint32_t) 0x0ff00ff0);
	success &= write(outStream, (boost::uint32_t) 0x0ff00ff0);
	success &= write(outStream, (boost::uint8_t) 0x00);
	// write the mysterious 0x0001
	success &= write(outStream, (boost::uint16_t) 0x0001);
	// write the 'a' byte
	success &= write(outStream, (boost::uint8_t) 'a');
	// write the design name length
	string designName = "partial.ncd;UserID=0xFFFFFFFF";
	writeXilinxString(outStream, designName);
	// write the 'b' byte
	success &= write(outStream, (boost::uint8_t) 'b');
	// write device string
	string deviceName = my_dev->get_name();
	writeXilinxString(outStream, deviceName.substr(2) + "ff1136");
	// write the 'c' byte
	success &= write(outStream, (boost::uint8_t) 'c');
	// write design date
	string designDate = "2010/03/18";
	writeXilinxString(outStream, designDate);
	// write the 'd' byte
	success &= write(outStream, (boost::uint8_t) 'd');
	// write design date
	string designTime = "16:24:32";
	writeXilinxString(outStream, designTime);
	// write the 'e' byte
	success &= write(outStream, (boost::uint8_t) 'e');
	/* We won't write the size here, because we need to wait until we know the size
	 boost::uint32_t bitstreamSize = 4464512;
	 success &= write(outStream, (boost::uint32_t) bitstreamSize);
	 */
	return success;
}

bool bitstream::writeBitstream(const string& outBitstream) {
	// construct the file stream and open file for writing
	fstream outStream(outBitstream.c_str(), fstream::out);
	if (!outStream.good())
		cerr
				<< "bitstream::writeBitstream: Unable to open bitstream file for writing"
				<< endl;
	// enable fstream exceptions
	outStream.exceptions(fstream::eofbit | fstream::failbit | fstream::badbit);
	bool success = true;
	// Write bitstream header
	success &= writeHeader(outStream);
	// If buildPartial hasn't been called yet, we assume the user wants a full bitstream
	if (frameBitmap.size() == 0) {
		frameBitmap.resize(my_dev->get_cfg_size(), true);
		// Write register config and frame packets
		success &= writePackets(outStream);
	} else
		success &= writePacketsPartial(outStream);
	outStream.close();
	return success;
}

bool bitstream::loadFile(string bitstream_name) {

	bitstreamFile = bitstream_name;
	// determine whether our parsing was successful
	bool success = false;
	// fields of interest

	try {

		// construct the file stream and open the file in default mode
		fstream bitstream(bitstream_name.c_str());
		// enable fstream exceptions
		bitstream.exceptions(fstream::eofbit | fstream::failbit
				| fstream::badbit);
		success = true;

		// read the header
		success &= readHeader(bitstream, designName, deviceName, designDate,
				designTime, bitstreamLength);
		bitstreamWordCount = bitstreamLength >> 2;
		// read the contents
		success &= readPackets(bitstream);
	}

	// catch any file exceptions
	catch (fstream::failure& f) {

		success = false;
		cerr << "Unprocessed fstream::failure: " << f.what() << endl;

	} catch (std::exception& e) {
		cerr << "Unprocessed exception: " << e.what() << endl;
	}

	cout << "Parsing was " << (success ? "successful" : "unsuccessful") << endl;
	return success;
}

/**
 * Build the frameBitmap structure to represent which frames should be written out to
 * the bitstream when necessary.
 * @param regionTiles A vector of all the tiles in a partial region.
 */
void bitstream::buildPartial(vector<string> regionTiles) {
	frameBitmap.resize(my_dev->get_cfg_size(), false);
	for (boost::uint32_t i = 0; i < regionTiles.size(); i++) {
		if (tileMap.count(regionTiles[i])) {
			tile_data* curTile = tileMap[regionTiles[i]];
			int startFrame = curTile->frame_num;
			int endFrame = startFrame + curTile->num_frames;
			cout << "bitstream::buildPartial: Adding tile " << curTile->name
					<< " to bitmap with far: " << curTile->far.str()
					<< " and goes from frame " << startFrame << " to frame "
					<< endFrame << endl;
			for (int j = startFrame; j < endFrame; j++) {
				frameBitmap.set(j);
			}
			// Set frames for this clock region
			frame_addr gclkAddr = my_dev->tilecoord_to_major(
					my_dev->get_gclk_index(), curTile->coord.y);
			// Increment address since tilecoord_to_major actually returned address for IOB column before GCLK
			gclkAddr.col++;
			startFrame = my_dev->frame_offset(gclkAddr);
			endFrame = startFrame + my_dev->get_tile_frames(GCLK);
			for (int j = startFrame; j < endFrame; j++) {
				frameBitmap.set(j);
			}
		}
	}
}

bool bitstream::writeFrames(fstream& outStream, int startFrame, int numFrames) {
	try {
		cout << "writeFrames: Writing from " << startFrame << " to "
				<< startFrame + numFrames << endl;
		for (int i = startFrame; i < startFrame + numFrames; i++) {
			outStream.write(frame_array[i], my_dev->get_frame_words() << 2);
		}
	} catch (std::exception e) {
		cerr << "bitstream::writeFrames: Failed writing bitstream" << endl;
		return false;
	}
	return true;
}

}
}
