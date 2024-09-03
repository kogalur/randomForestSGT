#ifndef RF_TERM_OPS_H
#define RF_TERM_OPS_H
#include "terminal.h"
void *makeTerminalDerived(void);
void  freeTerminalDerived(void *parent);
void restoreTerminalNodeOutcomesCDL(uint treeID, Terminal *parent);
void getTestTerminalNodeOutcomesCDL(uint treeID, Terminal *parent);
#endif 
