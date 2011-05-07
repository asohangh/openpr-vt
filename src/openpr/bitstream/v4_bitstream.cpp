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

#include "openpr/bitstream/v4_bitstream.h"


namespace openpr {
namespace bitstream{

	const char* v4_bitstream::sTypeName[eTypeCount] = {
		"[UNKNOWN TYPE 0]", "TYPE1", "TYPE2", "[UNKNOWN TYPE 3]", "[UNKNOWN TYPE 4]", "[UNKNOWN TYPE 5]", 
		"[UNKNOWN TYPE 6]", "[UNKNOWN TYPE 7]"
	};

	const char* v4_bitstream::sOpcodeName[eOpcodeCount] = {
		"NOP", "READ", "WRITE", "RESERVED"
	};

	const char* v4_bitstream::sRegisterName[eRegCount] = {
		"CRC", "FAR", "FDRI", "FDRO", "CMD", "CTL", "MASK", "STAT", "LOUT", "COR", "MFWR", "CBC", "IDCODE", "AXSS"
	};

	const char* v4_bitstream::sCommandName[eCmdCount] = {
		"NULL", "WCFG", "MFWR", "LFRM", "RCFG", "START", "RCAP", "RCRC", "AGHIGH", "SWITCH", "GRESTORE", "SHUTDOWN", 
		"GCAPTURE", "DESYNC"
	};

	// this constructor should only be used by the standalone bitstream reader
	v4_bitstream::v4_bitstream(void) : bitstream()
	{
		for(int i = 0; i < eRegCount; i++) mRegister[i] = 0;
	}

	v4_bitstream::v4_bitstream(string device_name, bool frame_ecc) : bitstream(device_name, frame_ecc)
	{
		for(int i = 0; i < eRegCount; i++) mRegister[i] = 0;
	}

	bool v4_bitstream::readPackets(fstream& inStream) {
		bool success = true;

		// loop until we have read the expected number of words
		boost::uint32_t word = 0;
		boost::uint32_t type = 0;
		boost::uint32_t opcode = 0;
		boost::uint32_t address = 0;
		boost::uint32_t reserved = 0;
		boost::uint32_t count = 0;
		int nopCount = 0;

		for(boost::uint32_t index = 0; index < bitstreamWordCount; index++) {

			// read the next word
			inStream.read((char*) &word, sizeof(word));
			word = ntohl(word);

			// extract the packet subfields
			type = (word >> eShiftPacketType) & eMaskPacketType;
			opcode = (word >> eShiftPacketOpcode) & eMaskPacketOpcode;

			// count NOP packets and compress them
			if(opcode != eOpNOP || index == bitstreamWordCount - 1) {
				if(nopCount) {
					cout << "    NOP x " << nopCount << endl;
					nopCount = 0;
				}
				if(index != bitstreamWordCount - 1) cout << "    ";
			}

			// catch dummy words
			if(word == 0xffffffff) {
				cout << "DUMMY" << endl;
				continue;
			// catch sync words
			} else if(word == 0xaa995566) {
				cout << "SYNC" << endl;
				continue;
			}

			// catch reserved packets
			if(opcode == eOpReserved) {
				cout << sOpcodeName[opcode] << endl;
				continue;
			// catch NOP packets
			} else if(opcode == eOpNOP) {
				//cout << sOpcodeName[opcode] << endl;
				nopCount++;
				continue;
			}

			// classify packet types
			switch(type) {
			case eType1: 
				address = (word >> eShiftType1Address) & eMaskType1Address;
				reserved = (word >> eShiftType1Reserved) & eMaskType1Reserved;
				count = (word >> eShiftType1Count) & eMaskType1Count;
				cout << sTypeName[type]; 
				break;
			case eType2: 
				count = (word >> eShiftType2Count) & eMaskType2Count;
				cout << sTypeName[type]; 
				break;
			default: 
				cout << sTypeName[type] << endl;
				continue;
			}

			// catch reads
			if(opcode == eOpRead) {
				cout << " READ " << sRegisterName[address];
			// catch writes
			} else if(opcode == eOpWrite) {
				cout << " WRITE " << sRegisterName[address];
				if(address == eRegFDRI) {
					cout << ": " << hex << setfill('0') << setw(8) << count << dec << " words";
					// Get pointer to start of configuration memory
					cfgMemoryStart = inStream.tellg();
					// read words into the frame data register
					success &= writeFrameData(inStream, count);
					index += count;
				} else if(count > 0) {
					// read words (most typically one) into the current register
					boost::uint32_t temp;
					for(size_t i = 0; i < count; i++, index++) {
						inStream.read((char*) &temp, sizeof(temp));
						mRegister[address] = ntohl(temp);
					}
					switch(address) {
					case eRegCMD: 
						// output the command
						cout << " " << sCommandName[mRegister[address]];
						break;
					default:
						// output the register contents
						cout << ": " << hex << setfill('0') << setw(8) << mRegister[address] << dec;
						break;
					}
				}
			}

			// terminate the line
			cout << endl;

		}

cout.flush();// exit(-1);
		return success;
	}
	
	bool v4_bitstream::writePacketHeader(fstream& outStream, boost::uint32_t packetType, boost::uint32_t opcode, boost::uint32_t address, boost::uint32_t reserved, boost::uint32_t count)
	{
		boost::uint32_t word = 0;

		if(packetType == eType1)
		{
			// Set address
			address = address << eShiftType1Address;
			// Set reserved
			reserved = reserved << eShiftType1Reserved;
			// Set count
			count = count << eShiftType1Count;
		}
		else
		{
			address = 0;
			reserved = 0;
			count = count << eShiftType2Count;
		}

		// Set packet type 
		packetType = packetType << eShiftPacketType;
		// Set opcode
		opcode = opcode << eShiftPacketOpcode;
		word = word | packetType | opcode | address | reserved | count;
		return write(outStream, word);
	}

	bool v4_bitstream::writePackets(fstream& outStream)
	{
		bool success = true;
		// store position of bitstream size argument well need to write here later
		std::streampos sizePos = outStream.tellp();
		success &= write(outStream,(boost::uint32_t)0);
		// write dummy word
		success &= write(outStream,(boost::uint32_t)eDummyWord);
		// Write sync word
		success &= write(outStream,(boost::uint32_t)eSyncWord);
		// Write RCRC to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdRCRC);
		// Write NoOps
		for(int i=0;i<2;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write to COR register
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCOR,0,1);
		success &= write(outStream,(boost::uint32_t)0x00043fe5);
		// Write to IDCODE
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegIDCODE,0,1);
		success &= write(outStream,(boost::uint32_t)my_dev->get_chip_id());
		// Write SWITCH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdSWITCH);
		// Write noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write NULL to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdNULL);
		// Write noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write CTL Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegMASK,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000600);
		// Write 0 value to CTL
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCTL,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write NoOps
		for(int i=0;i<1150;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write CTL Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegMASK,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000600);
		// Write 0 value to CTL
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCTL,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write frames to bitstream
		success &= writePartialFrames(outStream);
		// Write CRC value to CRC Reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCRC,0,1);
		success &= write(outStream,(boost::uint32_t)0x0000defc);
		// Write GRESTORE to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdGRESTORE);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write LFRM to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdLFRM);
		// Write NoOps
		for(int i=0;i<100;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write GRESTORE to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdGRESTORE);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write NULL to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdNULL);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write something to FAR
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegFAR,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write START to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdSTART);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write CTL Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegMASK,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000600);
		// Write 0 value to CTL
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCTL,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write CRC value to CRC Reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCRC,0,1);
		success &= write(outStream,(boost::uint32_t)0x0000defc);
		// Write DESYNC to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdDESYNC);
		// Write NoOps
		for(int i=0;i<15;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		return success;
	}
	bool v4_bitstream::writePacketsPartial(fstream& outStream)
	{
		bool success = false;
		success &= writePackets(outStream);
		return success;
	}
	frame_addr v4_bitstream::farToStruct(boost::uint32_t far)
	{
		using std::bitset;
		frame_addr retVal;
		bitset<32> farBits(far);
		retVal.mna = farBits.to_ulong() & 0x1F;
		retVal.col = (farBits >> eShiftMajor).to_ulong() & 0xFF;
		retVal.row = (farBits >> eShiftRow).to_ulong() & 0x1F;
		retVal.type = (farBits >> eShiftBlockType).to_ulong() & 0x07;
		retVal.tb = (farBits >> eShiftTB).to_ulong() & 0x01;
		cout << "FAR " << retVal.str();
		return retVal;
	}

	boost::uint32_t v4_bitstream::structToFar(frame_addr far)
	{
		using std::bitset;
		boost::uint32_t retVal = 0;
		retVal |= far.mna;
		retVal |= (far.col << eShiftMajor);
		retVal |= (far.row << eShiftRow);
		retVal |= (far.type << eShiftBlockType);
		retVal |= (far.tb << eShiftTB);
		cout << "FAR struct converted to " << retVal;
		return retVal;
	}
	bool v4_bitstream::writeFrameData(fstream& inStream, int inWordCount) {

		// if we are writing zero words, return immediately
		if(inWordCount == 0) return true;
		int byteCount = inWordCount << 2;

		// if we do not have device support, return immediately
		if(my_dev == NULL) {
			inStream.seekg(byteCount, ios_base::cur);
			return true;
		}

		// determine where we're suppposed to write
		cout << ": ";
		boost::uint32_t far = mRegister[eRegFAR];
		if(far != 0) isPartial = true;
		frame_addr frameAddress = farToStruct(far);
		boost::uint32_t frameOffset = my_dev->frame_offset(frameAddress);

		if(!mFrameData) cerr << "mFrameData has not been allocated" << endl;

		if(my_dev != NULL) {
			// read the frame data
			boost::uint8_t* ptr = (boost::uint8_t*) frame_array[frameOffset];
			inStream.read((char*) ptr, byteCount);
			// determine how many frames we read
			int wordsPerFrame = my_dev->get_frame_words();
			int bytesPerFrame = wordsPerFrame << 2;
			int frameCount = inWordCount / wordsPerFrame;
			int frameRoundoff = inWordCount % wordsPerFrame;
			if(frameRoundoff) {
				cerr << "Read " << frameCount << " frames plus " << frameRoundoff << " words." << endl;
			}
			// iterate over every frame
			for(size_t frame = frameOffset; frame < (frameOffset + frameCount); frame++, ptr += bytesPerFrame) {
				// flip the bit order in all top frames
				if(findexToFaddr[frame].tb == top) {
					reverseFrameBits(ptr);
				}
				// unmangle the first 16 tiles, and the last 16 tiles
				boost::uint16_t* tile_ptr = (boost::uint16_t*) ptr;
				for(int tile = 0; tile < 8; tile += 2, tile_ptr += 10) unmangleTilePair(tile_ptr);
					tile_ptr += 2;
				for(int tile = 0; tile < 8; tile += 2, tile_ptr += 10) unmangleTilePair(tile_ptr);
			}
			num_frames = frameCount;
		}
		// return success
		return true;
	}

	void v4_bitstream::unmangleTilePair(boost::uint16_t* ptr) {
		// unmangle the frame bytes from 00000000110011111111 to 00000000001111111111
		//                               01234567238967018945    01234567890123456789
		boost::uint16_t* word_ptr1 = ptr + 4;
		boost::uint16_t* word_ptr2 = ptr + 5;
		boost::uint16_t swap;
		// swap 23 and 89
		swap = *word_ptr1; *word_ptr1 = *word_ptr2; *word_ptr2 = swap; word_ptr1 += 2; word_ptr2 += 2;
		// swap 67 and 01
		swap = *word_ptr1; *word_ptr1 = *word_ptr2; *word_ptr2 = swap; word_ptr1 += 2; word_ptr2 += 2;
		// swap 89 and 45
		swap = *word_ptr1; *word_ptr1 = *word_ptr2; *word_ptr2 = swap; word_ptr1 -= 1; word_ptr2 -= 1;
		// swap 67 and 45
		swap = *word_ptr1; *word_ptr1 = *word_ptr2; *word_ptr2 = swap; word_ptr1 -= 2; word_ptr2 -= 2;
		// swap 23 and 01
		swap = *word_ptr1; *word_ptr1 = *word_ptr2; *word_ptr2 = swap;
	}


	void v4_bitstream::reverseFrameBits(boost::uint8_t* ptr) {
		// set up the swap frame and the pointers
		const boost::uint16_t frameLength = 41 << 2;
		boost::uint8_t swap[frameLength];
		boost::uint8_t* out_ptr = ptr;
		boost::uint8_t* swap_ptr = swap;
		// copy the bytes
		for(int i = 0; i < frameLength; i++) *swap_ptr++ = *ptr++;
		// iterate over the bytes
		for(int i = 0; i < frameLength; i++) {
			// skip the ECC/clock word
			if(i >= (20 << 2) && i < (21 << 2)) {
				--swap_ptr;
				out_ptr++;
				continue;
			} else {
				// read the byte
				boost::uint8_t a = *--swap_ptr;
				boost::uint8_t b = 0;
				// iterate over the bits
				for(int j = 0; j < 8; j++) { b <<= 1; b |= a & 1; a >>= 1; }
				*out_ptr++ = b;
			}
		}
	}


	bool v4_bitstream::writePartialFrames(fstream& outStream)
	{
		bool success = true;
		bool foundFrame = false;
		boost::uint32_t contigFrames = 0;
		boost::uint32_t tempFAR = 0;
		boost::uint32_t startFrame = 0;

		for(boost::uint32_t i=0;i<frameBitmap.size();i++)
		{
			if(!foundFrame && frameBitmap[i])
			{
				foundFrame = true;
				tempFAR = structToFar(findexToFaddr[i]);
				success &= writePacketHeader(outStream,eType1,eOpWrite,eRegFAR,0,1);
				success &= write(outStream,tempFAR);
				// Write WCFG to CMD register 
				success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
				success &= write(outStream,(boost::uint32_t)eCmdWCFG);
				// Write NoOp
				success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
				// Write to FDRI
				success &= writePacketHeader(outStream,eType1,eOpWrite,eRegFDRI,0,0);
		//		success &= writePacketHeader(outStream,eType2,eOpWrite,0,0,0x001106d0);
				startFrame = i;
				contigFrames++;
			}
			else if(foundFrame && frameBitmap[i])
			{
				contigFrames++;
			}
			else if(foundFrame && !frameBitmap[i])
			{
				foundFrame = false;
				success &= writePacketHeader(outStream,eType2,eOpWrite,0,0,contigFrames*my_dev->get_frame_words()); 
				// actually write frame data
				success &= writeFrames(outStream,startFrame,contigFrames);
				cout << dec << "contig: " << contigFrames << endl;
				contigFrames = 0;
			}
		}
		if(foundFrame)
		{
				success &= writePacketHeader(outStream,eType2,eOpWrite,0,0,contigFrames*my_dev->get_frame_words()); 
				// actually write frame data
				success &= writeFrames(outStream,startFrame,contigFrames);
		}
		return success;
	}
}
}
