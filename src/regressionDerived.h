#ifndef  RF_REGRESSION_DERIVED_H
#define  RF_REGRESSION_DERIVED_H
#include "terminal.h"
void calculateAllTerminalNodeOutcomesSGT(char mode,
                                         uint treeID,
                                         TerminalBase  *term);
void calculateMeanResponseCDL(uint treeID, Terminal *term);
void restoreMeanResponseCDL(uint treeID, Terminal *term);
void updateEnsembleMeanCDL(char mode, uint treeID);
void updateEnsembleMeanRT(uint treeID);
#endif
