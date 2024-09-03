#ifndef  RF_TERM_BASE_OPS_H
#define  RF_TERM_BASE_OPS_H
#include "terminalBase.h"
TerminalBase *makeTerminalBase(void);
void preInitTerminalBase(TerminalBase *parent);
void initTerminalBase(TerminalBase *parent,
                      uint  eTypeSize,
                      uint  mTimeSize,
                      uint  sTimeSize,
                      uint  rnfCount,
                      uint *rnfIndex,
                      uint  rfCount,
                      uint *rfIndex,
                      uint *rfSize);
TerminalRegression     *makeRegressionBase(uint rnfCount, uint *rnfIndex, uint *dummy);
TerminalClassification *makeClassificationBase(uint rfCount, uint *rfIndex, uint *rfSize);
TerminalSurvival       *makeSurvivalBase(uint dummy, uint mTimeSize, uint sTimeSize);
TerminalCompetingRisk  *makeCompetingRiskBase(uint eTypeSize, uint mTimeSize, uint sTimeSize);
void stackMeanResponse(TerminalRegression *tTerm);
void unstackMeanResponse(TerminalRegression *tTerm);
void stackMultiClassProb(TerminalClassification *tTerm);
void unstackMultiClassProb(TerminalClassification *tTerm);
void stackSurvival(TerminalSurvival *tTerm);
void unstackSurvival(TerminalSurvival *tTerm);
void stackNelsonAalen(TerminalSurvival *tTerm);
void unstackNelsonAalen(TerminalSurvival *tTerm);
void stackHazard(TerminalSurvival *tTerm);
void unstackHazard(TerminalSurvival *tTerm);
void stackSurvivalOutcome(TerminalSurvival *tTerm);
void unstackSurvivalOutcome(TerminalSurvival *tTerm);
void stackCSH(TerminalCompetingRisk *tTerm);
void unstackCSH(TerminalCompetingRisk *tTerm);
void stackCIF(TerminalCompetingRisk *tTerm);
void unstackCIF(TerminalCompetingRisk *tTerm);
void stackCompetingRiskOutcome(TerminalCompetingRisk *tTerm);
void unstackCompetingRiskOutcome(TerminalCompetingRisk *tTerm);
void freeTerminalBase(TerminalBase *parent);
void deinitTerminalBase(TerminalBase *parent);
void freeRegressionBase(TerminalRegression *parent);
void freeClassificationBase(TerminalClassification *parent);
void freeSurvivalBase(TerminalSurvival *parent);
void freeCompetingRiskBase(TerminalCompetingRisk *parent);
void getMembrCountOnly (uint       treeID,
                        TerminalBase  *parent,
                        uint      *repMembrIndx,
                        uint       repMembrSize,
                        uint      *genMembrIndx,
                        uint       genMembrSize);
void assignTerminalNodeMembership(char             mode,
                                  uint             treeID,
                                  TerminalBase    *parent,
                                  uint            *genMembrIndx,
                                  uint             genMembrSize,
                                  uint            *gmbrIterator,
                                  TerminalBase  ***tTermMembership);
#endif
