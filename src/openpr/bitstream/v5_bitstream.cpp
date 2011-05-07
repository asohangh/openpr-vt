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

#include "openpr/bitstream/v5_bitstream.h"


namespace openpr {
namespace bitstream
{
	const char* v5_bitstream::sTypeName[eTypeCount] = {
		"[UNKNOWN TYPE 0]", "TYPE1", "TYPE2", "[UNKNOWN TYPE 3]", "[UNKNOWN TYPE 4]", "[UNKNOWN TYPE 5]", 
		"[UNKNOWN TYPE 6]", "[UNKNOWN TYPE 7]"
	};

	const char* v5_bitstream::sOpcodeName[eOpcodeCount] = {
		"NOP", "READ", "WRITE", "RESERVED"
	};

	const char* v5_bitstream::sRegisterName[eRegCount] = {
		"CRC", "FAR", "FDRI", "FDRO", "CMD", "CTL0", "MASK", "STAT", "LOUT", "COR0", "MFWR", "CBC", "IDCODE", "AXSS", 
		"COR1", "CSOB", "WBSTAR", "TIMER", "[UNKNOWN REG 18]", "[UNKNOWN REG 19]", "[UNKNOWN REG 20]", "[UNKNOWN REG 21]", 
		"BOOTSTS", "[UNKNOWN REG 23]", "CTL1"
	};

	const char* v5_bitstream::sCommandName[eCmdCount] = {
		"NULL", "WCFG", "MFW", "DGHIGH/LFRM", "RCFG", "START", "RCAP", "RCRC", "AGHIGH", "SWITCH", "GRESTORE", "SHUTDOWN", 
		"GCAPTURE", "DESYNCH", "[UNKNOWN CMD 14]", "IPROG", "[UNKNOWN CMD 16]", "LTIMER"
	};

	// this constructor should only be used by the standalone bitstream reader
	v5_bitstream::v5_bitstream(void) : bitstream()
	{
		for(int i = 0; i < eRegCount; i++) mRegister[i] = 0;
	}

	v5_bitstream::v5_bitstream(string device_name) : bitstream(device_name)
	{
		for(int i = 0; i < eRegCount; i++) mRegister[i] = 0;
	}

	bool v5_bitstream::readPackets(fstream& inStream) {
		bool success = true;

		// loop until we have read the expected number of words
		boost::uint32_t word = 0;
		boost::uint32_t type = 0;
		boost::uint32_t opcode = 0;
		boost::uint32_t address = 0;
		boost::uint32_t reserved = 0;
		boost::uint32_t count = 0;
		int nopCount = 0;
		bool synched = false;

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

			// catch bus sync words
			if(!synched) {
				if(word == 0x000000bb) {
					cout << "    "; // 0x000000bb looks like a NOP, so it wasn't indented
					cout << "BUS WIDTH SYNC WORD" << endl;
					continue;
				} else if(word == 0x11220044) {
					cout << "BUS WIDTH DETECT" << endl;
					continue;
				}
			}
			// catch dummy words
			if(word == 0xffffffff) {
				cout << "DUMMY" << endl;
				continue;
			// catch sync words
			} else if(word == 0xaa995566) {
				synched = true;
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

		// display debugging information: beginning and end of frame data range, and all allocated frames
#if 0
		cout << "mFrameData: " << hex << setfill(' ') << setw(16) << (void*) mFrameData << dec << endl;
		for(int frame = 0; frame < num_frames; frame++) {
			cout << "frame " << frame << ": " << hex << setfill(' ') << setw(16) << (void*) frame_array[frame] << dec;
			cout << ": ";
			for(int i = 0; i < 41 << 2; i++) {
				cout << hex << setfill('0') << setw(2) << (boost::uint16_t) (*(frame_array[frame]+i) & 0xff) << dec;
				if(i < 80 && (i % 10) == 9) cout << ' ';
				if(i > 80 && (i % 10) == 3) cout << ' ';
			}
			cout << endl;
		}
#endif

		// return success
		return success;
	}

	bool v5_bitstream::writePacketHeader(fstream& outStream, boost::uint32_t packetType, boost::uint32_t opcode, boost::uint32_t address, boost::uint32_t reserved, boost::uint32_t count)
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

	bool v5_bitstream::writePackets(fstream& outStream)
	{
		bool success = true;
		// store position of bitstream size argument well need to write here later
		std::streampos sizePos = outStream.tellp();
		success &= write(outStream,(boost::uint32_t)0);
		// Write dummy words
		for(int i=0;i<8;i++)
			success &= write(outStream,(boost::uint32_t)eDummyWord);
		// Write BusWidthWords
		success &= write(outStream,(boost::uint32_t)eBusWidthWord);
		success &= write(outStream,(boost::uint32_t)eBusWidth);
		for(int i=0;i<2;i++)
			success &= write(outStream,(boost::uint32_t)eDummyWord);
		// Write sync word
		success &= write(outStream,(boost::uint32_t)eSyncWord);
		// Write noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write WBSTAR Packet Header
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegWBSTAR,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdNULL);
		// Write NULL to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdNULL);
		// Write noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write SWITCH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdRCRC);
		// Write NoOps
		for(int i=0;i<2;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write 0 to TIMER register 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegTIMER,0,1);
		success &= write(outStream,(boost::uint32_t)0);
		// Write 0 to 19 (Unknown) register 
		success &= writePacketHeader(outStream,eType1,eOpWrite,19,0,1);
		success &= write(outStream,(boost::uint32_t)0);
		// Write to COR0 register
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCOR0,0,1);
		success &= write(outStream,(boost::uint32_t)0x00003fe5);
		// Write to COR1 register
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCOR1,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write to IDCODE
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegIDCODE,0,1);
		success &= write(outStream,(boost::uint32_t)my_dev->get_chip_id());
		// Write SWITCH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdSWITCH);
		// Write noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write CTL0 Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegMASK,0,1);
		success &= write(outStream,(boost::uint32_t)0x00400000);
		// Write write SBITS value to CTL0?
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCTL0,0,1);
		success &= write(outStream,(boost::uint32_t)0x00400000);
		// Write CTL1 Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegMASK,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write CTL1 Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCTL1,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write NoOps
		for(int i=0;i<8;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		success &= writePartialFrames(outStream);
		// Write CRC value to CRC Reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCRC,0,1);
		success &= write(outStream,(boost::uint32_t)0x09e623ba1);
		// Write GRESTORE to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdGRESTORE);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write DGHIGH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdDGHIGH);
		// Write NoOps
		for(int i=0;i<100;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write GRESTORE to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdGRESTORE);
		// Write NoOps
		for(int i=0;i<30;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write START to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdSTART);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write something to FAR
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegFAR,0,1);
		success &= write(outStream,(boost::uint32_t)0x00ef8000);
		// Write CTL0 Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegMASK,0,1);
		success &= write(outStream,(boost::uint32_t)0x00400000);
		// Write write SBITS value to CTL0?
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCTL0,0,1);
		success &= write(outStream,(boost::uint32_t)0x00400000);
		// Write CRC value to CRC Reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCRC,0,1);
		success &= write(outStream,(boost::uint32_t)0x09e623ba1);
		// Write DESYNCH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdDESYNCH);
		// Write NoOps
		for(int i=0;i<61;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write size of bitstream in bytes to bitstream
		boost::uint64_t bytesWritten = outStream.tellp() - sizePos - sizeof(boost::uint32_t);
		outStream.seekp(sizePos);
		success &= write(outStream,(boost::uint32_t)bytesWritten);
		return success;
	}

	bool v5_bitstream::writePacketsPartial(fstream& outStream)
	{
		bool success = true;
		// store position of bitstream size argument well need to write here later
		std::streampos sizePos = outStream.tellp();
		success &= write(outStream,(boost::uint32_t)0);
		// Write dummy words
		for(int i=0;i<8;i++)
			success &= write(outStream,(boost::uint32_t)eDummyWord);
		// Write BusWidthWords
		success &= write(outStream,(boost::uint32_t)eBusWidthWord);
		success &= write(outStream,(boost::uint32_t)eBusWidth);
		for(int i=0;i<2;i++)
			success &= write(outStream,(boost::uint32_t)eDummyWord);
		// Write sync word
		success &= write(outStream,(boost::uint32_t)eSyncWord);
		// Write noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Reset CRC register 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdRCRC);
		// Write NoOps
		for(int i=0;i<2;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write to IDCODE
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegIDCODE,0,1);
		success &= write(outStream,(boost::uint32_t)my_dev->get_chip_id());
		/* Uncomment if you don't want Active Reconfiguration.
		// Write to COR0 register
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCOR0,0,1);
		success &= write(outStream,(boost::uint32_t)0x10003fe5);
		// Write SHUTDOWN to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdSHUTDOWN);
		// Write CRC value to CRC Reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCRC,0,1);
		success &= write(outStream,(boost::uint32_t)0x0000defc);
		// Write NoOps
		for(int i=0;i<4;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		// Write AGHIGH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdAGHIGH);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		*/
		// Write frames to bitstream
		success &= writePartialFrames(outStream);
		/* Uncomment if you don't want active reconfig.
		// Write GRESTORE to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdGRESTORE);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		*/
		// Write CTL1 Mask to MASK reg 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegMASK,0,1);
		success &= write(outStream,(boost::uint32_t)0x00001000);
		// Write 0 to CTL1 
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCTL1,0,1);
		success &= write(outStream,(boost::uint32_t)0x00000000);
		// Write DGHIGH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdDGHIGH);
		// Write NoOps
		for(int i=0;i<100;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		/* Write GRESTORE to CMD reg Uncomment for non-active reconfig
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdGRESTORE);
		*/
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		/* Uncomment for non-active reconfig
		// Write START to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdSTART);
		// Write Noop
		success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
		*/
		// Write something to FAR
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegFAR,0,1);
		success &= write(outStream,(boost::uint32_t)0x00ef8000);
		// Write CRC value to CRC Reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCRC,0,1);
		success &= write(outStream,(boost::uint32_t)0x0000defc);
		// Write DESYNCH to CMD reg
		success &= writePacketHeader(outStream,eType1,eOpWrite,eRegCMD,0,1);
		success &= write(outStream,(boost::uint32_t)eCmdDESYNCH);
		/* Write NoOps Uncomment for non-active reconfig
		for(int i=0;i<3;i++)
			success &= writePacketHeader(outStream,eType1,eOpNOP,0,0,0);
			*/
		// Write size of bitstream in bytes to bitstream
		boost::uint64_t bytesWritten = outStream.tellp() - sizePos - sizeof(boost::uint32_t);
		outStream.seekp(sizePos);
		success &= write(outStream,(boost::uint32_t)bytesWritten);
		return success;

	}

	bool v5_bitstream::writePartialFrames(fstream& outStream)
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
				cout << "Writing starting from far: " << findexToFaddr[i].str() << endl;
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

	frame_addr v5_bitstream::farToStruct(boost::uint32_t far)
	{
		/** @see Frame Address Register bitfields obtained from UG191 v3.6 Figure 6-10. */
		using std::bitset;
		frame_addr retVal;
		bitset<32> farBits(far);
		retVal.mna = farBits.to_ulong() & 0x7F;
		retVal.col = (farBits >> eShiftMajor).to_ulong() & 0xFF;
		retVal.row = (farBits >> eShiftRow).to_ulong() & 0x1F;
		retVal.type = (farBits >> eShiftBlockType).to_ulong() & 0x03;
		retVal.tb = (farBits >> eShiftTB).to_ulong() & 0x01;
		cout << "FAR converted to " << retVal.str() << endl;
		return retVal;
	}

	boost::uint32_t v5_bitstream::structToFar(frame_addr far)
	{
		/** @see Frame Address Register bitfields obtained from UG191 v3.6 Figure 6-10. */
		using std::bitset;
		boost::uint32_t retVal = 0;
		retVal |= (far.tb << eShiftTB);
		retVal |= (far.type << eShiftBlockType);
		retVal |= (far.row << eShiftRow);
		retVal |= (far.col << eShiftMajor);
		retVal |= far.mna;
		cout << "FAR struct converted to " << hex << retVal << endl;
		return retVal;
	}

	bool v5_bitstream::writeFrameData(fstream& inStream, int inWordCount) {

		// if we are writing zero words, return immediately
		if(inWordCount == 0) return true;
		int byteCount = inWordCount << 2;

		// determine where we're suppposed to write
		cout << ": ";
		boost::uint32_t far = mRegister[eRegFAR];
		if(far != 0) isPartial = true;
		frame_addr frameAddress = farToStruct(far);
		structToFar(frameAddress);
		// if we do not have device support, return immediately
		if(my_dev == NULL) {
			inStream.seekg(byteCount, ios_base::cur);
			return true;
		}

		boost::uint32_t frameOffset = my_dev->frame_offset(frameAddress);

		// read the frame data
		boost::uint8_t* ptr = (boost::uint8_t*) frame_array[frameOffset];
		inStream.read((char*) ptr, byteCount);
		// determine how many frames we read
		int wordsPerFrame = my_dev->get_frame_words();
		int frameCount = inWordCount / wordsPerFrame;
		int frameRoundoff = inWordCount % wordsPerFrame;
		if(frameRoundoff) {
			cerr << "Read " << frameCount << " frames plus " << frameRoundoff << " words." << endl;
		}
		num_frames = frameCount;

		// return success
		return true;
	}
}
}
