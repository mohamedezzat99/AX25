#ifndef AX25_CRC_H
#define AX25_CRC_H
#include "std_types.h"

uint16 AX25_computeCRC(uint8 *buffer, uint16 size_frame);
void AX25_putCRC(uint8 *frame, uint16 size_frame);

#endif /* AX25_CRC_H */
