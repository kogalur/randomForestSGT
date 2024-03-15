
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "cdlMain.h"
#include "shared/error.h"
#include "shared/random.h"
#include "shared/stackPreDefined.h"
#include "shared/stackAuxiliaryInfo.h"
#include "shared/preprocessForestRecord.h"
#include "shared/stackIncoming.h"
#include "shared/nrutil.h"
#include "shared/nativeUtil.h"
#include "shared/sexpIO.h"
#include "shared/stack.h"
#include "treeOps.h"
#include "nodeOps.h"
#include "termOps.h"
#include "cdlRegr.h"
#include "cvRegr.h"
#include "cdlUtil.h"
#include "sexpOutgoing.h"
#include "stackOutput.h"
void cdlMain(char mode, int seedValue) {
  uint b;
  uint seedValueLC;
  seedValueLC    = 0; 
  if (seedValue >= 0) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Parameter verification failed.");
    RF_nativeError("\nRF-SRC:  Random seed must be less than zero.  \n");
    RF_nativeExit();
  }
  if (RF_observationSize < 1) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Parameter verification failed.");
    RF_nativeError("\nRF-SRC:  Number of individuals must be greater than one:  %10d \n", RF_observationSize);
    RF_nativeExit();
  }
  if (RF_xSize < 1) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Parameter verification failed.");
    RF_nativeError("\nRF-SRC:  Number of parameters must be greater than zero:  %10d \n", RF_xSize);
    RF_nativeExit();
  }
  if (SG_nfolds > RF_observationSize) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Parameter verification failed.");
    RF_nativeError("\nRF-SRC:  Number of folds must be less than or equal to number of individuals:  %10d vs. %10d \n", SG_nfolds, RF_observationSize);
    RF_nativeExit();
  }
  if (SG_nfolds == 0) {
    SG_nfolds ++;
  }
#ifdef _OPENMP
  if (RF_numThreads < 0) {
    RF_numThreads = omp_get_max_threads();
  }
  else if (RF_numThreads == 0) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Parameter verification failed.");
    RF_nativeError("\nRF-SRC:  Number of threads must not be zero:  %10d \n", RF_numThreads);
    RF_nativeExit();
  }
  else {
    RF_numThreads = (RF_numThreads < omp_get_max_threads()) ? (RF_numThreads) : (omp_get_max_threads());
  }
#endif
  SG_threadV = SG_THREADV_NFOLD;
  RF_ntree = 0;
  uint ranChainCnt = 1;
  ran1A = &randomChainParallel;
  ran1B = &randomChainParallel2;
  ran1D = &randomChainParallel3;
  randomSetChain     = &randomSetChainParallel;
  randomSetChain2    = &randomSetChainParallel2;
  randomSetChain3    = &randomSetChainParallel3;
  randomGetChain     = &randomGetChainParallel;
  randomGetChain2    = &randomGetChainParallel2;
  randomGetChain3    = &randomGetChainParallel3;
  stackRandom(ranChainCnt);
  seedValueLC = abs(seedValue);
  lcgenerator(&seedValueLC, TRUE);
  for (b = 1; b <= ranChainCnt; b++) {
    lcgenerator(&seedValueLC, FALSE);
    lcgenerator(&seedValueLC, FALSE);
    while(seedValueLC == 0) {
      lcgenerator(&seedValueLC, FALSE);
    }
    randomSetChain(b, -seedValueLC);
  }
  for (b = 1; b <= ranChainCnt; b++) {
    lcgenerator(&seedValueLC, FALSE);
    lcgenerator(&seedValueLC, FALSE);
    while(seedValueLC == 0) {
      lcgenerator(&seedValueLC, FALSE);
    }
    randomSetChain2(b, -seedValueLC);
  }
  for (b = 1; b <= ranChainCnt; b++) {
    lcgenerator(&seedValueLC, FALSE);
    lcgenerator(&seedValueLC, FALSE);
    while(seedValueLC == 0) {
      lcgenerator(&seedValueLC, FALSE);
    }
    randomSetChain3(b, -seedValueLC);
  }
  stackIncomingArrays(mode,
                      RF_ntree,
                      RF_timeInterestSize,
                      RF_ytry,
                      RF_mtry,
                      RF_xWeight,
                      RF_yWeight,
                      RF_subjSize,
                      RF_subjWeight,
                      RF_xWeightStat,
                      RF_nodeSize,
                      RF_bootstrapSize,
                      RF_splitRule,
                      RF_quantileSize,
                      RF_quantile,
                      RF_ySize,
                      RF_rType,
                      RF_frSize,
                      RF_subjIn,
                      RF_observationSize,
                      RF_responseIn,
                      RF_fresponseIn,
                      RF_xSize,
                      RF_xType,
                      RF_fobservationSize,
                      RF_observationIn,
                      RF_fobservationIn,
                      &RF_yIndex,
                      &RF_yIndexZero,
                      &RF_timeIndex,
                      &RF_startTimeIndex,
                      &RF_statusIndex,
                      &RF_masterTime,
                      &RF_masterTimeSize,
                      &RF_sortedTimeInterestSize,
                      &RF_startMasterTimeIndexIn,
                      &RF_masterTimeIndexIn,
                      &RF_ptnCount,
                      &RF_ySizeProxy,
                      &RF_yIndexZeroSize);
  stackFactorArrays(mode,
                    RF_rType,
                    RF_xType,
                    RF_ySize,
                    RF_xSize,
                    RF_xLevelsCnt,
                    RF_rTarget,
                    RF_rTargetCount,
                    RF_timeIndex,
                    RF_statusIndex,
                    &RF_rFactorCount,
                    &RF_xFactorCount,
                    &RF_rFactorMap,
                    &RF_xFactorMap,
                    &RF_rFactorIndex,
                    &RF_xFactorIndex,
                    &RF_rFactorSize,
                    &RF_xFactorSize,
                    &RF_rNonFactorCount,
                    &RF_xNonFactorCount,
                    &RF_rNonFactorMap,
                    &RF_xNonFactorMap,
                    &RF_rNonFactorIndex,
                    &RF_xNonFactorIndex,
                    &RF_rTargetFactor,
                    &RF_rTargetNonFactor,
                    &RF_rTargetFactorCount,
                    &RF_rTargetNonFactorCount,
                    &RF_xLevels);
  initializeFactorArrays(mode,
                         RF_rFactorCount,
                         RF_xFactorCount,
                         RF_rFactorIndex,
                         RF_xFactorIndex,
                         RF_rLevelsMax,
                         RF_xLevelsMax,
                         RF_rLevelsCnt,
                         RF_xLevelsCnt,
                         RF_ntree,
                         RF_rFactorSize,
                         RF_xFactorSize,
                         &RF_rMaxFactorLevel,
                         &RF_xMaxFactorLevel,
                         &RF_maxFactorLevel,
                         &RF_factorList);
  if ((RF_timeIndex > 0) && (RF_statusIndex > 0)) {
      stackCompetingArrays(mode,
                           RF_statusIndex,
                           RF_splitRule,
                           RF_eventTypeSize,
                           RF_eventType,
                           RF_crWeightSize,
                           RF_crWeight,
                           RF_frSize,
                           RF_observationSize,
                           RF_fobservationSize,
                           RF_responseIn,
                           RF_fresponseIn,
                           RF_mRecordMap,
                           RF_fmRecordMap,
                           RF_mRecordSize,
                           RF_fmRecordSize,
                           RF_mpSign,
                           RF_fmpSign,
                           &RF_eventTypeIndex,
                           &RF_feventTypeSize,
                           &RF_mStatusSize,
                           &RF_eIndividualSize,
                           &RF_eIndividualIn);
  }
  if (RF_rFactorCount > 0) {
    stackClassificationArrays(mode,
                              RF_rFactorSize,
                              RF_rLevelsCnt,
                              RF_rFactorCount,
                              RF_observationSize,
                              RF_responseIn,
                              RF_rFactorIndex,
                              &RF_rLevels,
                              &RF_classLevelSize,
                              &RF_classLevel,
                              &RF_classLevelIndex,
                              &RF_rFactorThreshold,
                              &RF_rFactorMinority,
                              &RF_rFactorMajority,
                              &RF_rFactorMinorityFlag);
  }
  RF_auxDimConsts = makeAuxDimConsts(RF_rFactorSize,
                                     RF_rFactorCount,
                                     RF_rFactorMap,
                                     RF_rTargetFactor,
                                     RF_rTargetFactorCount,
                                     RF_tLeafCount_,
                                     RF_holdBLKptr);
  RF_incomingStackCount = 0;
  RF_incomingAuxiliaryInfoList = NULL;
  makeNode = & makeNodeDerived;
  freeNode = & freeNodeDerived;
  makeTerminal = & makeTerminalDerived;
  freeTerminal = & freeTerminalDerived;
  RF_stackCount = 5;
  RF_stackCount += 5;
  initProtect(RF_stackCount);
  stackAuxiliaryInfoList(&RF_snpAuxiliaryInfoList, RF_stackCount);
  SG_cpuTime_ = (double*) stackAndProtect(RF_auxDimConsts,
                                          mode,
                                          &RF_nativeIndex,
                                          NATIVE_TYPE_NUMERIC,
                                          SG_CPU_TIME,
                                          1,
                                          0,
                                          SG_sexpStringOutgoingCDL,
                                          NULL,
                                          1,
                                          1);
  SG_cpuTime_ --;
  double **betaOutPtr;
  double **yHatOutPtr;
  uint *caseIndx;
  uint  caseIndxSize;
  char *xPerm;
  caseIndx = NULL;
  xPerm    = NULL;
  caseIndxSize = RF_observationSize;
  if (RF_rTargetNonFactorCount > 0) {
    CDLInfo  *cdlResult = crossValidationRegr(1, 
                                              SG_lambdaGrid,
                                              SG_lambdaGridSize,
                                              SG_lambdaMinRatio,
                                              SG_lambdaEpsilon,                           
                                              SG_convergThreshold,
                                              SG_maxIter,
                                              SG_efficiency,
                                              SG_weight,
                                              caseIndxSize,
                                              RF_xSize,
                                              caseIndx,
                                              xPerm,
                                              SG_nfolds,
                                              RF_responseIn[RF_rTarget[1]],
                                              RF_observationIn);
    stackCoordinateDescentOutput(cdlResult,
                                 & SG_lambdaCount_,
                                 & SG_lambda_,
                                 & SG_convergenceCount_,
                                 & SG_betaOut_,
                                 & betaOutPtr,
                                 & SG_yHatOut_,
                                 & yHatOutPtr,
                                 & SG_lambdaMinIndx_,
                                 & SG_lambda1SEIndx_,
                                 & SG_lambdaMeanErr_,
                                 & SG_lambdaCVSD_);
    SG_lambdaCount_[1] = cdlResult -> lambdaCnt;
    for (uint i = 1; i <= cdlResult -> lambdaCnt; i++) {
      SG_lambda_[i] = cdlResult -> lambda[i];
      SG_convergenceCount_[i] = cdlResult -> convgCnt[i];
      for (uint j = 1; j <= RF_xSize; j++) {
        betaOutPtr[i][j+1] = cdlResult -> beta[i][j+1];
      }
      betaOutPtr[i][1] = cdlResult -> beta[i][1];
    }
    if (cdlResult -> yHatCnt > 0) {
      for (uint i = 1; i <= cdlResult -> lambdaCnt; i++) {
        for (uint j = 1; j <= cdlResult -> yHatCnt; j++) {
          yHatOutPtr[i][j] = cdlResult -> yHat[i][j];
        }
      }
      SG_lambdaMinIndx_[1] = cdlResult -> lambdaTargetIndx;
      SG_lambda1SEIndx_[1] = cdlResult -> lambda1SEIndx;
      if (SG_nfolds > 1) {
        if (cdlResult -> valid) {
          for (uint i = 1; i <= cdlResult -> lambdaCnt; i++) {
            SG_lambdaMeanErr_[i] = cdlResult -> lambdaMeanErr[i];
            SG_lambdaCVSD_[i] = cdlResult -> lambdaCVSD[i];
          }
        }
      }
    }
    freeCDLInfo(cdlResult);
  }
  else {
  }
  unstackAuxiliaryInfoAndList(RF_auxDimConsts,
                              (mode == RF_GROW) ? FALSE : TRUE,
                              RF_snpAuxiliaryInfoList,
                              RF_stackCount);
  freeAuxDimConsts(RF_auxDimConsts);
  if (RF_rFactorCount > 0) {
    unstackClassificationArrays(mode,
                                RF_rFactorSize,
                                RF_rFactorCount,
                                RF_rLevels,
                                RF_classLevelSize,
                                RF_classLevel,
                                RF_classLevelIndex,
                                RF_rFactorThreshold,
                                RF_rFactorMinority,
                                RF_rFactorMajority,
                                RF_rFactorMinorityFlag);
  }
  if ((RF_timeIndex > 0) && (RF_statusIndex > 0)) {
      unstackCompetingArrays(mode,
                             RF_statusIndex,
                             RF_eventTypeSize,
                             RF_eventType,
                             RF_feventTypeSize,
                             RF_eventTypeIndex,
                             RF_mStatusSize,
                             RF_eIndividualSize,
                             RF_eIndividualIn);
  }
  unstackFactorArrays(mode,
                      RF_ntree,
                      RF_ySize,
                      RF_xSize,
                      RF_rTarget,
                      RF_rTargetCount,
                      RF_rTargetFactor,
                      RF_rTargetNonFactor,
                      RF_timeIndex,
                      RF_statusIndex,
                      RF_rFactorCount,
                      RF_xFactorCount,
                      RF_rFactorMap,
                      RF_xFactorMap,
                      RF_rFactorIndex,
                      RF_xFactorIndex,
                      RF_rFactorSize,
                      RF_xFactorSize,
                      RF_rNonFactorCount,
                      RF_xNonFactorCount,
                      RF_rNonFactorMap,
                      RF_xNonFactorMap,
                      RF_rNonFactorIndex,
                      RF_xNonFactorIndex,
                      RF_xLevels,
                      RF_factorList);
  unstackIncomingArrays(mode,
                        RF_ySize,
                        RF_yIndex,
                        RF_yIndexZero);
  unstackRandom(ranChainCnt);
}
