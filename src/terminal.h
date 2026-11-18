#ifndef RF_TERMINAL_H
#define RF_TERMINAL_H
typedef struct terminal Terminal;
struct terminal {
  struct terminalBase base;
  double mean;
  uint repMembrCount, oobMembrCount, ibgMembrCount, tstMembrCount;
  uint allMembrCount;
  uint *oobMembrIndx, *repMembrIndx, *ibgMembrIndx, *tstMembrIndx;
};
#endif
