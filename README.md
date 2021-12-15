# AX25

AX25 Protocol

## tasks
- Receive array on serial
- fixed length of frame 256 bytes
- implementation of N(S), N(R) and V(S) and V(R).
- N(R), N(S) are given as a parameter to control build function
- V(S), V(R) are controlled by upper layer function

## done
- tracking of size
- crc calculation
- take different sub-fields from functions (currently : control, info)
- receiver takes frame and splits it into all the sub-fields.

## questions
