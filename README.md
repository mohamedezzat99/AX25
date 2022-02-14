# AX25

AX25 Protocol

## tasks
- Receive array on serial
- implementation of N(S), N(R) and V(S) and V(R).
- V(S), V(R) are controlled by upper layer function

## done
- tracking of size
- crc calculation
- take different sub-fields from functions (currently : control, info)
- receiver takes frame and splits it into all the sub-fields.

## questions
how to implement N(S) and N(R) in the latest Manager Function
ask for next steps