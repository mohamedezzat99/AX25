/*--------------------------------------------------------------------------*
 * AX25_CRC.c
 * FCS operations of the AX.25 frame.
 *--------------------------------------------------------------------------*/

#include "AX25_CRC.h"


/*--------------------------------------------------------------------------*
 * AX.25 FCS calculation (bitwise method).
 * CRC-16-CCITT G(x) = x16 + x12 + x5 + 1.
 * Polynom = 0x1021.
 *
 * PARAMETERS:
 * *buffer       pointer of the frame buffer.
 * size_frame    length of the frame (in bytes).
 *
 * RETURN:
 * the CRC with a final XORed operation.
 *--------------------------------------------------------------------------*/
void AX25_computeCRC(uint8 *buffer, uint16 * OpArrSize) {
	unsigned int i, j;
	unsigned short shiftRegister, outBit;
	char byte;
	uint16 crc;
	// The last flag and the 2 bytes for FCS are removed.
	// size_frame = size_frame - 3;

	// Initialization of the Shift Register to 0xFFFF
	shiftRegister = 0xFFFF;

	for(i=1 ; i<*OpArrSize; i++) {  // The first flag is not calculated so i=1.
		byte = buffer[i];

		for(j=0; j<8; j++) {
			outBit = shiftRegister & 0x0001;
			shiftRegister >>= 0x01;  // Shift the register to the right.

			if(outBit != (byte & 0x01)) {
				shiftRegister ^= 0x8408;  // Mirrored polynom.
				byte >>= 0x01;
				continue;
			}
			byte >>= 0x01;
		}
	}
	crc = shiftRegister ^ 0xFFFF;  // Final XOR, FCS calculation.
	// Put the FCS in the right place with the 15th bit to be sent first.
	buffer[*OpArrSize] = (crc & 0xff);
	*OpArrSize=*OpArrSize+1;
	buffer[*OpArrSize] = ((crc >> 8) & 0xff);
	*OpArrSize=*OpArrSize+1;
}


