#ifndef RF_TERMINAL_H
#define RF_TERMINAL_H
typedef struct terminal Terminal;
struct terminal {
  struct terminalBase base;
  double mean;
  uint repMembrCount, oobMembrCount, ibgMembrCount;
  uint allMembrSize;
  uint *oobMembrIndx, *repMembrIndx, *ibgMembrIndx;
};
#endif
