
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "entry.h"
#include "shared/nrutil.h"
#include "shared/stackForestObjects.h"
#include "shared/nativeUtil.h"
#include "shared/error.h"
#include "sgtMain.h"
#include "sgtMainRT.h"
#include "cdlMain.h"
#include "entryGeneric.h"
#include "stackOutput.h"
static void validateGrowBasis(SEXP basisInfo, SEXP xInfo, SEXP augmxInfo,
                              uint nSize, uint optLocal);
static const char *validateRestoreReplicates(SEXP totalNodeCount, SEXP tLeafCount,
                                            SEXP treeID, SEXP nodeSZ, SEXP brnodeID,
                                            SEXP rmbrCT, SEXP rmbrID,
                                            uint treeCount, uint nSize);
SEXP entryGrow(SEXP traceFlag,
               SEXP seedPtr,
               SEXP optLow,
               SEXP optHigh,
               SEXP optLocal,
               SEXP ntree,
               SEXP observationSize,
               SEXP sampleInfo,
               SEXP splitRule,
               SEXP nsplit,
               SEXP mtry,
               SEXP nodeSize,
               SEXP lot,
               SEXP yTarget,
               SEXP yInfo,
               SEXP yLevels,
               SEXP yData,
               SEXP timeInterest,
               SEXP xInfo,
               SEXP xLevels,
               SEXP xData,
               SEXP augmxInfo,
               SEXP basisInfo,
               SEXP tuningParameters,
               SEXP perfBlock,
               SEXP numThreads) {
  char mode;
  char result;
  result = TRUE;
  clock_t cpuTimeStart = clock();
  setUserTraceFlag(INTEGER(traceFlag)[0]);
  setNativeGlobalEnv(&RF_nativeIndex, &RF_stackCount);
  int seedValue           = INTEGER(seedPtr)[0];
  RF_opt                  = INTEGER(optLow)[0];
  RF_optHigh              = INTEGER(optHigh)[0];
  SG_optLocal             = INTEGER(optLocal)[0];
  RF_ntree                = INTEGER(ntree)[0];
  RF_observationSize      = INTEGER(observationSize)[0];
  SG_basisSourceSize = SG_basisTermSize = 0;
  SG_basisSource = NULL;
  SG_basisTermOffset = SG_basisParentIndex = SG_basisParentPower = NULL;
  SG_basisCenterMask = NULL;
  validateGrowBasis(basisInfo, xInfo, augmxInfo, RF_observationSize, SG_optLocal);
  RF_subjSize             = RF_observationSize;
  RF_subjWeight           = NULL;
  RF_bootstrapSize        = RF_observationSize;
  RF_bootstrapIn          = NULL;
  if(VECTOR_ELT(sampleInfo, 0) != R_NilValue) {
    RF_subjSize = INTEGER(VECTOR_ELT(sampleInfo, 0))[0];
    if(VECTOR_ELT(sampleInfo, 1) != R_NilValue) {
      RF_subjWeight = REAL(VECTOR_ELT(sampleInfo, 1)); RF_subjWeight--;
    }
    if(VECTOR_ELT(sampleInfo, 2) != R_NilValue) {  
      RF_bootstrapSize = INTEGER(VECTOR_ELT(sampleInfo, 2))[0];
      if(VECTOR_ELT(sampleInfo, 3) != R_NilValue) {
        RF_bootstrapIn = (uint **) copy2DObject(VECTOR_ELT(sampleInfo, 3), NATIVE_TYPE_INTEGER, TRUE, RF_ntree, RF_subjSize);
      }
    }
    else {
      RF_bootstrapSize        = RF_subjSize;
    }
  }
  SG_splitRule            = INTEGER(splitRule)[0];
  RF_nsplit               = INTEGER(nsplit)[0];  
  RF_mtry                 = INTEGER(mtry)[0];  
  RF_nodeSize             = INTEGER(nodeSize)[0];
  SG_hcut         = 1;
  SG_lotSize      = 0;
  SG_lotLag       = 0;
  SG_lotStrikeout = 0;
  if (lot != R_NilValue) {
    if (VECTOR_ELT(lot, 0) != R_NilValue) {
      SG_hcut = INTEGER(VECTOR_ELT(lot, 0))[0];
    }
    if (VECTOR_ELT(lot, 1) != R_NilValue) {
      SG_lotSize = INTEGER(VECTOR_ELT(lot, 1))[0];
    }
    if (INTEGER(VECTOR_ELT(lot, 2))[0] > 0) {
      if (VECTOR_ELT(lot, 3) != R_NilValue) {
        SG_lotLag          = INTEGER(VECTOR_ELT(lot, 3))[0];
        if (SG_lotLag == 0) {
          SG_lotLag = 1;
        }
      }
      if (VECTOR_ELT(lot, 4) != R_NilValue) {
        SG_lotStrikeout    = INTEGER(VECTOR_ELT(lot, 4))[0];
      }
    }
  }
  RF_rTargetCount         = INTEGER(VECTOR_ELT(yTarget, 0))[0];
  if (VECTOR_ELT(yTarget, 1) != R_NilValue) {
    RF_rTarget         = (uint *) INTEGER(VECTOR_ELT(yTarget, 1));
    RF_rTarget --;
  }
  else {
    RF_rTarget = NULL;
  }
  RF_ySize                = INTEGER(VECTOR_ELT(yInfo, 0))[0];
  if(VECTOR_ELT(yInfo, 1) != R_NilValue) {
    RF_rType = (char *) copy1DObject(VECTOR_ELT(yInfo, 1), NATIVE_TYPE_CHARACTER, RF_ySize, TRUE);
  }
  else {
    RF_rType = NULL;
  }
  if(VECTOR_ELT(yInfo, 2) != R_NilValue) {
    RF_rLevelsMax         = (uint *) INTEGER(VECTOR_ELT(yInfo, 2)); RF_rLevelsMax--;
  }
  else {
    RF_rLevelsMax         = NULL;
  }
  if(VECTOR_ELT(yInfo, 3) != R_NilValue) {
    RF_rLevelsCnt         = (uint *) INTEGER(VECTOR_ELT(yInfo, 3)); RF_rLevelsCnt--;
  }
  else {
    RF_rLevelsCnt = NULL;
  }
  if(VECTOR_ELT(yInfo, 4) != R_NilValue) {
    RF_subjIn             =  (uint *) INTEGER(VECTOR_ELT(yInfo, 4));  RF_subjIn --;
  }
  else {
    RF_subjIn             = NULL;
  }
  RF_eventTypeSize      =  INTEGER(VECTOR_ELT(yInfo, 5))[0];
  RF_eventType        = NULL;
  if (RF_eventTypeSize > 0) {
    RF_eventType        =  (uint *) INTEGER(VECTOR_ELT(yInfo, 6));  RF_eventType --;
  }
  RF_rLevelsSEXP = yLevels;
  RF_rLevels = NULL;
  if(RF_ySize > 0) {
    RF_responseIn           = (double **) copy2DObject(yData, NATIVE_TYPE_NUMERIC, TRUE, RF_ySize, RF_observationSize);
  }
  else {
    RF_responseIn = NULL;
  }
  RF_timeInterest = NULL;
  RF_timeInterestSize = INTEGER(VECTOR_ELT(timeInterest, 0))[0];
  if (RF_timeInterestSize > 0) {
    RF_timeInterest = (double *) REAL(VECTOR_ELT(timeInterest, 1));
    RF_timeInterest --;
  }
  RF_xSize                 = INTEGER(VECTOR_ELT(xInfo, 0))[0];
  if(VECTOR_ELT(xInfo, 1) != R_NilValue) {
    RF_xType                = (char *) copy1DObject(VECTOR_ELT(xInfo, 1), NATIVE_TYPE_CHARACTER, RF_xSize, TRUE);
  }
  else {
    RF_xType = NULL;
  }
  if(VECTOR_ELT(xInfo, 2) != R_NilValue) {  
    RF_xLevelsMax           = (uint *) INTEGER(VECTOR_ELT(xInfo, 2)); RF_xLevelsMax--;
  }
  else {
    RF_xLevelsMax = NULL;
  }
  if(VECTOR_ELT(xInfo, 3) != R_NilValue) {
    RF_xLevelsCnt           = (uint *) INTEGER(VECTOR_ELT(xInfo, 3)); RF_xLevelsCnt --;
  }
  else {
    RF_xLevelsCnt = NULL;
  }
  RF_xtType = NULL;
  RF_stType = NULL;
  RF_xLevelsSEXP = xLevels;
  RF_xLevels = NULL;
  if (xData != R_NilValue) {
    RF_observationIn      = (double **) copy2DObject(xData, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_observationSize);
  }
  else {
    RF_observationIn = NULL;
  }
  SG_xSizeAugm = 0;
  SG_observationInAugm = NULL;
  if (augmxInfo != R_NilValue) {
    if(VECTOR_ELT(augmxInfo, 0) != R_NilValue) {
      SG_xSizeAugm = (uint) INTEGER(VECTOR_ELT(augmxInfo, 0))[0];
      if(VECTOR_ELT(augmxInfo, 1) != R_NilValue) {
        SG_observationInAugm = (double **) copy2DObject(VECTOR_ELT(augmxInfo, 1), NATIVE_TYPE_NUMERIC, TRUE, SG_xSizeAugm, RF_observationSize);
      }
      else {
        SG_xSizeAugm = 0;
      }
    }
  }
  if (basisInfo != R_NilValue) {
    SG_basisSourceSize = (uint) INTEGER(VECTOR_ELT(basisInfo, 1))[0];
    SG_basisTermSize = (uint) INTEGER(VECTOR_ELT(basisInfo, 3))[0];
    uint parentSize = (uint) XLENGTH(VECTOR_ELT(basisInfo, 5));
    double *source = REAL(VECTOR_ELT(basisInfo, 2));
    SG_basisSource = (double **) new_vvector(1, SG_basisSourceSize, NRUTIL_DPTR);
    SG_basisTermOffset = uivector(1, SG_basisTermSize + 1);
    SG_basisParentIndex = uivector(1, parentSize);
    SG_basisParentPower = uivector(1, parentSize);
    SG_basisCenterMask = cvector(1, SG_basisSourceSize);
    for (uint j = 1; j <= SG_basisSourceSize; j++) {
      SG_basisSource[j] = source + (R_xlen_t) (j - 1) * RF_observationSize - 1;
      SG_basisCenterMask[j] = (char) INTEGER(VECTOR_ELT(basisInfo, 7))[j - 1];
    }
    for (uint k = 1; k <= SG_basisTermSize + 1; k++) {
      SG_basisTermOffset[k] = (uint) INTEGER(VECTOR_ELT(basisInfo, 4))[k - 1];
    }
    for (uint r = 1; r <= parentSize; r++) {
      SG_basisParentIndex[r] = (uint) INTEGER(VECTOR_ELT(basisInfo, 5))[r - 1];
      SG_basisParentPower[r] = (uint) INTEGER(VECTOR_ELT(basisInfo, 6))[r - 1];
    }
  }
  if (tuningParameters != R_NilValue) {
    SG_lambdaGridSize = (uint) INTEGER(VECTOR_ELT(tuningParameters, 0))[0];
    if (VECTOR_ELT(tuningParameters, 1) != R_NilValue) {
      SG_lambdaGrid = REAL(VECTOR_ELT(tuningParameters, 1));
      SG_lambdaGrid --;
    }
    else {
      SG_lambdaGrid = NULL;
    }
    SG_lambdaMinRatio = REAL(VECTOR_ELT(tuningParameters, 2))[0];
    SG_convergThreshold = REAL(VECTOR_ELT(tuningParameters, 3))[0];
    SG_lambdaEpsilon = REAL(VECTOR_ELT(tuningParameters, 4))[0];
    SG_maxIter = INTEGER(VECTOR_ELT(tuningParameters, 5))[0];
    SG_efficiency = (char) INTEGER(VECTOR_ELT(tuningParameters, 6))[0];
    if (VECTOR_ELT(tuningParameters, 7) != R_NilValue) {
      SG_weight = REAL(VECTOR_ELT(tuningParameters, 7));
      SG_weight --;
    }
    else {
      SG_weight = NULL;
    }
    SG_nfolds = INTEGER(VECTOR_ELT(tuningParameters, 8))[0];
  }
  else {
    SG_lambdaGridSize = 0;
    SG_lambdaGrid = NULL;
    SG_lambdaMinRatio = 0;
    SG_convergThreshold = 0;
    SG_lambdaEpsilon = 0;
    SG_maxIter = 0;
    SG_efficiency = FALSE;
    SG_weight = NULL;
    SG_nfolds = 0;
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  CDL Tuning Parameters Absent.  Analysis cannot proceed. \n");
    result = FALSE;
  }
  RF_perfBlock            = INTEGER(perfBlock)[0];
  RF_numThreads           = INTEGER(numThreads)[0];
  mode = processDefaultGrow();
  if (result) {
    result = sgtMain(mode, seedValue);
  }
  if (SG_basisSource != NULL) {
    uint parentSize = SG_basisTermOffset[SG_basisTermSize + 1] - 1;
    free_cvector(SG_basisCenterMask, 1, SG_basisSourceSize);
    free_uivector(SG_basisParentPower, 1, parentSize);
    free_uivector(SG_basisParentIndex, 1, parentSize);
    free_uivector(SG_basisTermOffset, 1, SG_basisTermSize + 1);
    free_new_vvector(SG_basisSource, 1, SG_basisSourceSize, NRUTIL_DPTR);
  }
  SG_basisSourceSize = SG_basisTermSize = 0;
  SG_basisSource = NULL;
  SG_basisTermOffset = SG_basisParentIndex = SG_basisParentPower = NULL;
  SG_basisCenterMask = NULL;
  free_1DObject(RF_rType, NATIVE_TYPE_CHARACTER, RF_ySize);
  free_1DObject(RF_xType, NATIVE_TYPE_CHARACTER, RF_xSize);
  if (RF_responseIn != NULL) free_2DObject(RF_responseIn, NATIVE_TYPE_NUMERIC, RF_ySize > 0, RF_ySize, RF_observationSize);
  if (RF_observationIn != NULL) free_2DObject(RF_observationIn, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_observationSize);
  if (SG_observationInAugm != NULL) free_2DObject(SG_observationInAugm, NATIVE_TYPE_NUMERIC, TRUE, SG_xSizeAugm, RF_observationSize);
  if (RF_bootstrapIn != NULL) {
    free_2DObject(RF_bootstrapIn, NATIVE_TYPE_INTEGER, TRUE, RF_ntree, RF_subjSize);
    RF_bootstrapIn = NULL;
  }
  if (RF_nativeIndex != RF_stackCount) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Stack imbalance in PROTECT/UNPROTECT:  %10d versus %10d  ", RF_nativeIndex, RF_stackCount);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  if (result && (RF_stackCount > 0)) {
    SG_cpuTime_[1] = (double) (clock() - cpuTimeStart) / CLOCKS_PER_SEC;
    R_ReleaseObject(RF_sexpVector[RF_OUTP_ID]);
    R_ReleaseObject(RF_sexpVector[RF_STRG_ID]);
    return RF_sexpVector[RF_OUTP_ID];
  }
  else {
    return NULL;
  }
}
SEXP entryPred(SEXP traceFlag,
               SEXP seedPtr,
               SEXP optLow,
               SEXP optHigh,
               SEXP optLocal,
               SEXP ntree,
               SEXP observationSize,
               SEXP sampleInfo,
               SEXP hcut,
               SEXP splitRule,
               SEXP yTarget,
               SEXP yInfo,
               SEXP yLevels,
               SEXP yData,
               SEXP xInfo,
               SEXP xLevels,
               SEXP xData,
               SEXP augmxInfo,
               SEXP basisInfo,
               SEXP lassoWeight,
               SEXP fobservationSize,
               SEXP fyData,
               SEXP fxData,
               SEXP faugmxInfo,
               SEXP fbasisData,
               SEXP totalNodeCount,
               SEXP tLeafCount,
               SEXP seedInfo,
               SEXP treeID,
               SEXP nodeID,
               SEXP nodeSZ,
               SEXP brnodeID,
               SEXP yBar,
               SEXP yStar,
               SEXP betaZ,
               SEXP betaP,
               SEXP hcutCnt,
               SEXP rmbrCT,
               SEXP ombrCT,
               SEXP rmbrID,
               SEXP ombrID,
               SEXP getTree,
               SEXP perfBlock,
               SEXP realTimeOpt,
               SEXP numThreads) {
  char mode;
  char result;
  result = TRUE;
  clock_t cpuTimeStart = clock();
  setUserTraceFlag(INTEGER(traceFlag)[0]);
  setNativeGlobalEnv(&RF_nativeIndex, &RF_stackCount);
  int seedValue           = INTEGER(seedPtr)[0];
  RF_opt                  = INTEGER(optLow)[0];
  RF_optHigh              = INTEGER(optHigh)[0];
  SG_optLocal             = INTEGER(optLocal)[0];
  RF_ntree                = INTEGER(ntree)[0];
  RF_observationSize      = INTEGER(observationSize)[0];
  if (TYPEOF(totalNodeCount) != INTSXP || XLENGTH(totalNodeCount) != 1 ||
      INTEGER(totalNodeCount)[0] <= 0) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: Total forest node count must be a positive integer no greater than %d.", INT_MAX);
    RF_nativeExit();
  }
  SG_weight = NULL;
  if (lassoWeight != R_NilValue) {
    if (TYPEOF(lassoWeight) != REALSXP || XLENGTH(lassoWeight) != RF_observationSize) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: Restore lasso weights must have one value per training observation.");
      RF_nativeExit();
    }
    SG_weight = REAL(lassoWeight) - 1;
  }
  SG_basisSourceSize = SG_basisTermSize = 0;
  SG_basisSource = NULL;
  SG_fbasisSource = NULL;
  SG_basisTermOffset = SG_basisParentIndex = SG_basisParentPower = NULL;
  SG_basisCenterMask = NULL;
  validateGrowBasis(basisInfo, xInfo, augmxInfo, RF_observationSize, SG_optLocal);
  if ((SG_optLocal & (SG_OPT_SWTCH_NINE | SG_OPT_NODE_RESTRICT)) &&
      INTEGER(fobservationSize)[0] > 0) {
    R_xlen_t sourceSize = INTEGER(VECTOR_ELT(basisInfo, 1))[0];
    if (TYPEOF(fbasisData) != REALSXP ||
        XLENGTH(fbasisData) != sourceSize * INTEGER(fobservationSize)[0]) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: Test basis source data has invalid type or dimensions.");
      RF_nativeExit();
    }
  }
  else if (fbasisData != R_NilValue) {
    RF_nativeError("\nRF-SGT: Test basis source data requires centered or restricted new-data prediction.");
    RF_nativeExit();
  }
  if (basisInfo != R_NilValue &&
      (TYPEOF(hcutCnt) != INTSXP || XLENGTH(hcutCnt) != 1 ||
       INTEGER(hcutCnt)[0] != INTEGER(VECTOR_ELT(basisInfo, 3))[0])) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: basis term count does not match the saved beta width.");
    RF_nativeExit();
  }
  RF_subjSize             = RF_observationSize;
  RF_subjWeight           = NULL;
  RF_bootstrapSize        = RF_observationSize;
  RF_bootstrapIn          = NULL;
  if(VECTOR_ELT(sampleInfo, 0) != R_NilValue) {
    RF_subjSize = INTEGER(VECTOR_ELT(sampleInfo, 0))[0];
    if(VECTOR_ELT(sampleInfo, 1) != R_NilValue) {
      RF_subjWeight = REAL(VECTOR_ELT(sampleInfo, 1)); RF_subjWeight--;
    }
    if(VECTOR_ELT(sampleInfo, 2) != R_NilValue) {  
      RF_bootstrapSize = INTEGER(VECTOR_ELT(sampleInfo, 2))[0];
      if(VECTOR_ELT(sampleInfo, 3) != R_NilValue) {
        RF_bootstrapIn = (uint **) copy2DObject(VECTOR_ELT(sampleInfo, 3), NATIVE_TYPE_INTEGER, TRUE, RF_ntree, RF_subjSize);
      }
    }
    else {
      RF_bootstrapSize        = RF_subjSize;
    }
  }
  SG_hcut = INTEGER(hcut)[0];
  SG_splitRule = INTEGER(splitRule)[0];
  RF_rTargetCount         = INTEGER(VECTOR_ELT(yTarget, 0))[0];
  if (VECTOR_ELT(yTarget, 1) != R_NilValue) {
    RF_rTarget         = (uint *) INTEGER(VECTOR_ELT(yTarget, 1));
    RF_rTarget --;
  }
  else {
    RF_rTarget = NULL;
  }
  RF_ySize                = INTEGER(VECTOR_ELT(yInfo, 0))[0];
  if(VECTOR_ELT(yInfo, 1) != R_NilValue) {
    RF_rType = (char *) copy1DObject(VECTOR_ELT(yInfo, 1), NATIVE_TYPE_CHARACTER, RF_ySize, TRUE);
  }
  else {
    RF_rType = NULL;
  }
  if(VECTOR_ELT(yInfo, 2) != R_NilValue) {
    RF_rLevelsMax         = (uint *) INTEGER(VECTOR_ELT(yInfo, 2)); RF_rLevelsMax--;
  }
  else {
    RF_rLevelsMax         = NULL;
  }
  if(VECTOR_ELT(yInfo, 3) != R_NilValue) {
    RF_rLevelsCnt         = (uint *) INTEGER(VECTOR_ELT(yInfo, 3)); RF_rLevelsCnt--;
  }
  else {
    RF_rLevelsCnt = NULL;
  }
  if(VECTOR_ELT(yInfo, 4) != R_NilValue) {
    RF_subjIn             =  (uint *) INTEGER(VECTOR_ELT(yInfo, 4));  RF_subjIn --;
  }
  else {
    RF_subjIn             = NULL;
  }
  RF_eventTypeSize      =  INTEGER(VECTOR_ELT(yInfo, 5))[0];
  RF_eventType        = NULL;
  if (RF_eventTypeSize > 0) {
    RF_eventType        =  (uint *) INTEGER(VECTOR_ELT(yInfo, 6));  RF_eventType --;
  }
  RF_rLevelsSEXP = yLevels;
  RF_rLevels = NULL;
  if(RF_ySize > 0) {
    RF_responseIn           = (double **) copy2DObject(yData, NATIVE_TYPE_NUMERIC, TRUE, RF_ySize, RF_observationSize);
  }
  else {
    RF_responseIn = NULL;
  }
  RF_xSize                 = INTEGER(VECTOR_ELT(xInfo, 0))[0];
  if(VECTOR_ELT(xInfo, 1) != R_NilValue) {
    RF_xType                = (char *) copy1DObject(VECTOR_ELT(xInfo, 1), NATIVE_TYPE_CHARACTER, RF_xSize, TRUE);
  }
  else {
    RF_xType = NULL;
  }
  if(VECTOR_ELT(xInfo, 2) != R_NilValue) {  
    RF_xLevelsMax           = (uint *) INTEGER(VECTOR_ELT(xInfo, 2)); RF_xLevelsMax--;
  }
  else {
    RF_xLevelsMax = NULL;
  }
  if(VECTOR_ELT(xInfo, 3) != R_NilValue) {
    RF_xLevelsCnt           = (uint *) INTEGER(VECTOR_ELT(xInfo, 3)); RF_xLevelsCnt --;
  }
  else {
    RF_xLevelsCnt = NULL;
  }
  RF_xtType = NULL;
  RF_stType = NULL;
  RF_xLevelsSEXP = xLevels;
  RF_xLevels = NULL;
  if (xData != R_NilValue) {
    RF_observationIn      = (double **) copy2DObject(xData, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_observationSize);
  }
  else {
    RF_observationIn = NULL;
  }
  SG_xSizeAugm = 0;
  SG_observationInAugm = NULL;
  if (augmxInfo != R_NilValue) {
    if(VECTOR_ELT(augmxInfo, 0) != R_NilValue) {
      SG_xSizeAugm = (uint) INTEGER(VECTOR_ELT(augmxInfo, 0))[0];
      if(VECTOR_ELT(augmxInfo, 1) != R_NilValue) {
        SG_observationInAugm = (double **) copy2DObject(VECTOR_ELT(augmxInfo, 1), NATIVE_TYPE_NUMERIC, TRUE, SG_xSizeAugm, RF_observationSize);
      }
    }
  }
  if (basisInfo != R_NilValue) {
    SG_basisSourceSize = (uint) INTEGER(VECTOR_ELT(basisInfo, 1))[0];
    SG_basisTermSize = (uint) INTEGER(VECTOR_ELT(basisInfo, 3))[0];
    uint parentSize = (uint) XLENGTH(VECTOR_ELT(basisInfo, 5));
    double *source = REAL(VECTOR_ELT(basisInfo, 2));
    SG_basisSource = (double **) new_vvector(1, SG_basisSourceSize, NRUTIL_DPTR);
    SG_basisTermOffset = uivector(1, SG_basisTermSize + 1);
    SG_basisParentIndex = uivector(1, parentSize);
    SG_basisParentPower = uivector(1, parentSize);
    SG_basisCenterMask = cvector(1, SG_basisSourceSize);
    for (uint j = 1; j <= SG_basisSourceSize; j++) {
      SG_basisSource[j] = source + (R_xlen_t) (j - 1) * RF_observationSize - 1;
      SG_basisCenterMask[j] = (char) INTEGER(VECTOR_ELT(basisInfo, 7))[j - 1];
    }
    for (uint k = 1; k <= SG_basisTermSize + 1; k++) {
      SG_basisTermOffset[k] = (uint) INTEGER(VECTOR_ELT(basisInfo, 4))[k - 1];
    }
    for (uint r = 1; r <= parentSize; r++) {
      SG_basisParentIndex[r] = (uint) INTEGER(VECTOR_ELT(basisInfo, 5))[r - 1];
      SG_basisParentPower[r] = (uint) INTEGER(VECTOR_ELT(basisInfo, 6))[r - 1];
    }
  }
  RF_fobservationSize      = INTEGER(fobservationSize)[0];
  if (fbasisData != R_NilValue) {
    SG_fbasisSource = (double **) new_vvector(1, SG_basisSourceSize, NRUTIL_DPTR);
    for (uint j = 1; j <= SG_basisSourceSize; j++) {
      SG_fbasisSource[j] = REAL(fbasisData) +
        (R_xlen_t) (j - 1) * RF_fobservationSize - 1;
    }
  }
  RF_frSize = 0;
  RF_fresponseIn = NULL;
  RF_fobservationIn = NULL;
  SG_fobservationInAugm = NULL;
  if((RF_fobservationSize > 0)) {
    if (fyData != R_NilValue) {    
      RF_fresponseIn           = (double **) copy2DObject(fyData, NATIVE_TYPE_NUMERIC, TRUE, RF_ySize, RF_fobservationSize);
      RF_frSize = RF_ySize;
    }
    if (fxData != R_NilValue) {
      RF_fobservationIn      = (double **) copy2DObject(fxData, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_fobservationSize);
    }
    if (faugmxInfo != R_NilValue) {
      SG_fobservationInAugm = (double **) copy2DObject(faugmxInfo, NATIVE_TYPE_NUMERIC, TRUE, SG_xSizeAugm, RF_fobservationSize);
    }
  }
  RF_totalNodeCount       = (ulong) INTEGER(totalNodeCount)[0];
  RF_tLeafCount           = (uint *) INTEGER(tLeafCount); RF_tLeafCount --;
  RF_seed_                = (int *) INTEGER(seedInfo); RF_seed_ --;
  SG_treeID_              = (uint *) INTEGER(treeID);   SG_treeID_ --;
  SG_nodeID_              = (uint *) INTEGER(nodeID);   SG_nodeID_ --;
  SG_nodeSZ_              = (uint *) INTEGER(nodeSZ);   SG_nodeSZ_ --;
  SG_brnodeID_            = (uint *) INTEGER(brnodeID); SG_brnodeID_ --;
  SG_betaZ_               = (double *) REAL(betaZ); SG_betaZ_ --;
  SG_yBar_                = (double *) REAL(yBar); SG_yBar_ --;
  SG_yStar_               = (double *) REAL(yStar); SG_yStar_ --;
  SG_betaP_in = (double **) copy2DObject(betaP, NATIVE_TYPE_NUMERIC, TRUE, RF_totalNodeCount, INTEGER(hcutCnt)[0]);
  SG_rmbrTNodeCT_ = (uint *) INTEGER(rmbrCT); SG_rmbrTNodeCT_ --;
  SG_ombrTNodeCT_ = (uint *) INTEGER(ombrCT); SG_ombrTNodeCT_ --;
  SG_rmbrTNodeID_ = (uint *) INTEGER(rmbrID); SG_rmbrTNodeID_ --;
  SG_ombrTNodeID_ = (uint *) INTEGER(ombrID); SG_ombrTNodeID_ --;
  RF_perfBlock            = INTEGER(perfBlock)[0];
  if (getTree != R_NilValue) {
    RF_getTree = (uint *) INTEGER(getTree);  RF_getTree --;
  }
  else {
    RF_getTree = NULL;
  }
  RF_numThreads           = INTEGER(numThreads)[0];
  SG_tcpPort = 0;
  SG_tcpTimeOut = 0;
  if(realTimeOpt != R_NilValue) {
    SG_tcpPort = (uint) INTEGER(VECTOR_ELT(realTimeOpt, 0))[0];
    SG_tcpTimeOut = (uint) INTEGER(VECTOR_ELT(realTimeOpt, 1))[0];
  }
  mode = processDefaultPredict();
  if (mode == RF_REAL) {
    result = sgtMainRT(mode, seedValue);
  }
  else {
    result = sgtMain(mode, seedValue);
  }
  if (SG_fbasisSource != NULL) {
    free_new_vvector(SG_fbasisSource, 1, SG_basisSourceSize, NRUTIL_DPTR);
  }
  if (SG_basisSource != NULL) {
    uint parentSize = SG_basisTermOffset[SG_basisTermSize + 1] - 1;
    free_cvector(SG_basisCenterMask, 1, SG_basisSourceSize);
    free_uivector(SG_basisParentPower, 1, parentSize);
    free_uivector(SG_basisParentIndex, 1, parentSize);
    free_uivector(SG_basisTermOffset, 1, SG_basisTermSize + 1);
    free_new_vvector(SG_basisSource, 1, SG_basisSourceSize, NRUTIL_DPTR);
  }
  SG_basisSourceSize = SG_basisTermSize = 0;
  SG_basisSource = NULL;
  SG_fbasisSource = NULL;
  SG_basisTermOffset = SG_basisParentIndex = SG_basisParentPower = NULL;
  SG_basisCenterMask = NULL;
  free_1DObject(RF_rType, NATIVE_TYPE_CHARACTER, RF_ySize);
  free_1DObject(RF_xType, NATIVE_TYPE_CHARACTER, RF_xSize);
  if (RF_responseIn != NULL) free_2DObject(RF_responseIn, NATIVE_TYPE_NUMERIC, RF_ySize > 0, RF_ySize, RF_observationSize);
  if (RF_observationIn != NULL) free_2DObject(RF_observationIn, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_observationSize);
  if (SG_observationInAugm != NULL) free_2DObject(SG_observationInAugm, NATIVE_TYPE_NUMERIC, TRUE, SG_xSizeAugm, RF_observationSize);
  if (RF_bootstrapIn != NULL) {
    free_2DObject(RF_bootstrapIn, NATIVE_TYPE_INTEGER, TRUE, RF_ntree, RF_subjSize);
    RF_bootstrapIn = NULL;
  }
  if (mode != RF_REAL) {
    if (RF_fresponseIn != NULL) free_2DObject(RF_fresponseIn, NATIVE_TYPE_NUMERIC, RF_ySize > 0, RF_ySize, RF_fobservationSize);  
    if (RF_fobservationIn != NULL) free_2DObject(RF_fobservationIn, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_fobservationSize);
    if (SG_fobservationInAugm != NULL) free_2DObject(SG_fobservationInAugm, NATIVE_TYPE_NUMERIC, TRUE, SG_xSizeAugm, RF_fobservationSize);
  }
  free_2DObject(SG_betaP_in, NATIVE_TYPE_NUMERIC, TRUE, RF_totalNodeCount, INTEGER(hcutCnt)[0]);
  if (RF_nativeIndex != RF_stackCount) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Stack imbalance in PROTECT/UNPROTECT:  %10d versus %10d  ", RF_nativeIndex, RF_stackCount);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  if (result && (RF_stackCount > 0)) {
    SG_cpuTime_[1] = (double) (clock() - cpuTimeStart) / CLOCKS_PER_SEC;
    R_ReleaseObject(RF_sexpVector[RF_OUTP_ID]);
    R_ReleaseObject(RF_sexpVector[RF_STRG_ID]);
    return RF_sexpVector[RF_OUTP_ID];
  }
  else {
    return NULL;
  }
}
SEXP entryCDL(SEXP traceFlag,
              SEXP seedPtr,
              SEXP optLow,
              SEXP optHigh,
              SEXP optLocal,
              SEXP observationSize,
              SEXP yInfo,
              SEXP yLevels,
              SEXP yData,
              SEXP xInfo,
              SEXP xLevels,
              SEXP xData,
              SEXP yTarget,
              SEXP tuningParameters,
              SEXP numThreads) {
  char mode;
  clock_t cpuTimeStart = clock();
  setUserTraceFlag(INTEGER(traceFlag)[0]);
  setNativeGlobalEnv(&RF_nativeIndex, &RF_stackCount);
  int seedValue           = INTEGER(seedPtr)[0];
  RF_opt                  = INTEGER(optLow)[0];
  RF_optHigh              = INTEGER(optHigh)[0];
  SG_optLocal             = INTEGER(optLocal)[0];
  RF_observationSize      = INTEGER(observationSize)[0];
  RF_ySize                = INTEGER(VECTOR_ELT(yInfo, 0))[0];
  if(VECTOR_ELT(yInfo, 1) != R_NilValue) {
    RF_rType = (char *) copy1DObject(VECTOR_ELT(yInfo, 1), NATIVE_TYPE_CHARACTER, RF_ySize, TRUE);
  }
  else {
    RF_rType = NULL;
  }
  if(VECTOR_ELT(yInfo, 2) != R_NilValue) {
    RF_rLevelsMax         = (uint *) INTEGER(VECTOR_ELT(yInfo, 2)); RF_rLevelsMax--;
  }
  else {
    RF_rLevelsMax         = NULL;
  }
  if(VECTOR_ELT(yInfo, 3) != R_NilValue) {
    RF_rLevelsCnt         = (uint *) INTEGER(VECTOR_ELT(yInfo, 3)); RF_rLevelsCnt--;
  }
  else {
    RF_rLevelsCnt = NULL;
  }
  RF_subjIn             = NULL;
  RF_eventTypeSize      = 0;
  RF_eventType        = NULL;
  RF_rLevelsSEXP = yLevels;
  RF_rLevels = NULL;
  if(RF_ySize > 0) {
    RF_responseIn           = (double **) copy2DObject(yData, NATIVE_TYPE_NUMERIC, TRUE, RF_ySize, RF_observationSize);
  }
  else {
    RF_responseIn = NULL;
  }
  RF_xSize                 = INTEGER(VECTOR_ELT(xInfo, 0))[0];
  if(VECTOR_ELT(xInfo, 1) != R_NilValue) {
    RF_xType                = (char *) copy1DObject(VECTOR_ELT(xInfo, 1), NATIVE_TYPE_CHARACTER, RF_xSize, TRUE);
  }
  else {
    RF_xType = NULL;
  }
  if(VECTOR_ELT(xInfo, 2) != R_NilValue) {  
    RF_xLevelsMax           = (uint *) INTEGER(VECTOR_ELT(xInfo, 2)); RF_xLevelsMax--;
  }
  else {
    RF_xLevelsMax = NULL;
  }
  if(VECTOR_ELT(xInfo, 3) != R_NilValue) {
    RF_xLevelsCnt           = (uint *) INTEGER(VECTOR_ELT(xInfo, 3)); RF_xLevelsCnt --;
  }
  else {
    RF_xLevelsCnt = NULL;
  }
  RF_xtType = NULL;
  RF_stType = NULL;
  RF_xLevelsSEXP = xLevels;
  RF_xLevels = NULL;
  if (xData != R_NilValue) {
    RF_observationIn      = (double **) copy2DObject(xData, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_observationSize);
  }
  else {
    RF_observationIn = NULL;
  }
  RF_numThreads           = INTEGER(numThreads)[0];
  RF_rTargetCount         = INTEGER(VECTOR_ELT(yTarget, 0))[0];
  if (VECTOR_ELT(yTarget, 1) != R_NilValue) {
    RF_rTarget         = (uint *) INTEGER(VECTOR_ELT(yTarget, 1));
    RF_rTarget --;
  }
  else {
    RF_rTarget = NULL;
  }
  if (tuningParameters != R_NilValue) {
    SG_lambdaGridSize = (uint) INTEGER(VECTOR_ELT(tuningParameters, 0))[0];
    if (VECTOR_ELT(tuningParameters, 1) != R_NilValue) {
      SG_lambdaGrid = REAL(VECTOR_ELT(tuningParameters, 1));
      SG_lambdaGrid --;
    }
    else {
      SG_lambdaGrid = NULL;
    }
    SG_lambdaMinRatio = REAL(VECTOR_ELT(tuningParameters, 2))[0];
    SG_convergThreshold = REAL(VECTOR_ELT(tuningParameters, 3))[0];
    SG_lambdaEpsilon = REAL(VECTOR_ELT(tuningParameters, 4))[0];
    SG_maxIter = INTEGER(VECTOR_ELT(tuningParameters, 5))[0];
    SG_efficiency = (char) INTEGER(VECTOR_ELT(tuningParameters, 6))[0];
    if (VECTOR_ELT(tuningParameters, 7) != R_NilValue) {
      SG_weight = REAL(VECTOR_ELT(tuningParameters, 7));
      SG_weight --;
    }
    else {
      SG_weight = NULL;
    }
    SG_nfolds = INTEGER(VECTOR_ELT(tuningParameters, 8))[0];
  }    
  else {
    SG_lambdaGridSize = 0;
    SG_lambdaGrid = NULL;
    SG_lambdaMinRatio = 0;
    SG_convergThreshold = 0;
    SG_lambdaEpsilon = 0;
    SG_maxIter = 0;
    SG_efficiency = FALSE;
    SG_weight = NULL;
    SG_nfolds = 0;
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  CDL Tuning Parameters Absent.  Analysis cannot proceed. \n");
    RF_nativeExit();
  }
  mode = processDefaultCDL();
  cdlMain(mode, seedValue);
  free_1DObject(RF_rType, NATIVE_TYPE_CHARACTER, RF_ySize);
  free_1DObject(RF_xType, NATIVE_TYPE_CHARACTER, RF_xSize);
  if (RF_responseIn != NULL) free_2DObject(RF_responseIn, NATIVE_TYPE_NUMERIC, RF_ySize > 0, RF_ySize, RF_observationSize);
  if (RF_observationIn != NULL) free_2DObject(RF_observationIn, NATIVE_TYPE_NUMERIC, TRUE, RF_xSize, RF_observationSize);
  if (RF_nativeIndex != RF_stackCount) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Stack imbalance in PROTECT/UNPROTECT:  %10d versus %10d  ", RF_nativeIndex, RF_stackCount);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  if (RF_stackCount > 0) {
    SG_cpuTime_[1] = (double) (clock() - cpuTimeStart) / CLOCKS_PER_SEC;
    R_ReleaseObject(RF_sexpVector[RF_OUTP_ID]);
    R_ReleaseObject(RF_sexpVector[RF_STRG_ID]);
    return RF_sexpVector[RF_OUTP_ID];
  }
  else {
    return NULL;
  }
}
static void validateGrowBasis(SEXP basisInfo, SEXP xInfo, SEXP augmxInfo,
                              uint nSize, uint optLocal) {
  if (basisInfo == R_NilValue) {
    if (optLocal & (SG_OPT_SWTCH_NINE | SG_OPT_NODE_RESTRICT)) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: node.center or node.restrict requires basisXlist.");
      RF_nativeExit();
    }
    return;
  }
  if (!(optLocal & (SG_OPT_SWTCH_NINE | SG_OPT_NODE_RESTRICT))) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");    
    RF_nativeError("\nRF-SGT: basisXlist requires the node.center or node.restrict option bit.");
    RF_nativeExit();
  }
  if (TYPEOF(basisInfo) != VECSXP || XLENGTH(basisInfo) != 8) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: basisXlist must contain eight entries.");
    RF_nativeExit();
  }
  for (uint j = 0; j < 8; j++) {
    if (TYPEOF(VECTOR_ELT(basisInfo, j)) != (j == 2 ? REALSXP : INTSXP)) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: basisXlist has an invalid field type.");
      RF_nativeExit();
    }
  }
  if (XLENGTH(VECTOR_ELT(basisInfo, 0)) != 1 ||
      XLENGTH(VECTOR_ELT(basisInfo, 1)) != 1 ||
      XLENGTH(VECTOR_ELT(basisInfo, 3)) != 1) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: basis active and dimension fields must be scalars.");
    RF_nativeExit();
  }
  int pSource = INTEGER(VECTOR_ELT(basisInfo, 1))[0];
  int pTerm = INTEGER(VECTOR_ELT(basisInfo, 3))[0];
  if (INTEGER(VECTOR_ELT(basisInfo, 0))[0] != 1 ||
      pSource <= 0 || pTerm <= 0 || nSize == 0) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: basisXlist must be active with positive dimensions.");
    RF_nativeExit();
  }
  R_xlen_t expectedTerms = INTEGER(VECTOR_ELT(xInfo, 0))[0];
  if (augmxInfo != R_NilValue &&
      VECTOR_ELT(augmxInfo, 0) != R_NilValue &&
      VECTOR_ELT(augmxInfo, 1) != R_NilValue) {
    expectedTerms += INTEGER(VECTOR_ELT(augmxInfo, 0))[0];
  }
  if (pTerm != expectedTerms) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: basis term count does not match the lasso dictionary.");
    RF_nativeExit();
  }
  R_xlen_t parentSize = XLENGTH(VECTOR_ELT(basisInfo, 5));
  if (XLENGTH(VECTOR_ELT(basisInfo, 2)) != (R_xlen_t) pSource * nSize ||
      XLENGTH(VECTOR_ELT(basisInfo, 4)) != (R_xlen_t) pTerm + 1 ||
      parentSize < 1 || parentSize >= INT_MAX ||
      XLENGTH(VECTOR_ELT(basisInfo, 6)) != parentSize ||
      XLENGTH(VECTOR_ELT(basisInfo, 7)) != pSource) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: basisXlist has inconsistent array lengths.");
    RF_nativeExit();
  }
  int *offset = INTEGER(VECTOR_ELT(basisInfo, 4));
  int *parent = INTEGER(VECTOR_ELT(basisInfo, 5));
  int *power = INTEGER(VECTOR_ELT(basisInfo, 6));
  int *mask = INTEGER(VECTOR_ELT(basisInfo, 7));
  if (offset[0] != 1 || offset[pTerm] != parentSize + 1) {
    RF_nativeError("\nRF-SGT:  *** ERROR *** ");
    RF_nativeError("\nRF-SGT: basis term offsets have invalid endpoints.");
    RF_nativeExit();
  }
  for (int k = 0; k < pTerm; k++) {
    if (offset[k] < 1 || offset[k + 1] <= offset[k]) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: basis term offsets must increase strictly.");
      RF_nativeExit();
    }
  }
  for (R_xlen_t r = 0; r < parentSize; r++) {
    if (parent[r] < 1 || parent[r] > pSource || power[r] < 1) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: basis parent indices or powers are invalid.");
      RF_nativeExit();
    }
  }
  for (int j = 0; j < pSource; j++) {
    if (mask[j] != 0 && mask[j] != 1) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: basis center mask must contain only zero or one.");
      RF_nativeExit();
    }
    if (!(optLocal & SG_OPT_SWTCH_NINE) && mask[j] != 0) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: Uncentered basis requires a zero center mask.");
      RF_nativeExit();
    }
  }
}
static const char *validateRestoreReplicates(SEXP totalNodeCount, SEXP tLeafCount,
                                            SEXP treeID, SEXP nodeSZ, SEXP brnodeID,
                                            SEXP rmbrCT, SEXP rmbrID,
                                            uint treeCount, uint nSize) {
  if (treeCount == 0 || treeCount > INT_MAX || nSize == 0 || nSize > INT_MAX ||
      TYPEOF(totalNodeCount) != INTSXP || XLENGTH(totalNodeCount) != 1 ||
      TYPEOF(tLeafCount) != INTSXP || TYPEOF(treeID) != INTSXP ||
      TYPEOF(nodeSZ) != INTSXP || TYPEOF(brnodeID) != INTSXP ||
      TYPEOF(rmbrCT) != INTSXP || TYPEOF(rmbrID) != INTSXP) {
    return "missing or invalid forest dimensions or membership vectors.";
  }
  int nodeCount = INTEGER(totalNodeCount)[0];
  if (nodeCount <= 0 || XLENGTH(tLeafCount) != treeCount ||
      XLENGTH(treeID) != nodeCount || XLENGTH(nodeSZ) != nodeCount ||
      XLENGTH(brnodeID) != nodeCount) {
    return "forest row counts or leaf-count dimensions disagree.";
  }
  R_xlen_t *leafOffset = (R_xlen_t *) R_alloc((size_t) treeCount + 1, sizeof(R_xlen_t));
  char *seen = R_alloc(treeCount, sizeof(char));
  leafOffset[0] = 0;
  for (uint b = 0; b < treeCount; b++) {
    int leaves = INTEGER(tLeafCount)[b];
    if (leaves < 0 || leaves > XLENGTH(rmbrCT) - leafOffset[b]) {
      return "leaf counts exceed the saved terminal-count vector.";
    }
    leafOffset[b + 1] = leafOffset[b] + leaves;
    seen[b] = FALSE;
  }
  if (leafOffset[treeCount] != XLENGTH(rmbrCT)) {
    return "terminal-count vector length does not match the leaf counts.";
  }
  R_xlen_t memberCount = 0;
  for (R_xlen_t k = 0; k < XLENGTH(rmbrCT); k++) {
    int count = INTEGER(rmbrCT)[k];
    if (count <= 0 || count > XLENGTH(rmbrID) - memberCount) {
      return "replicate counts are invalid or exceed the saved ID vector.";
    }
    memberCount += count;
  }
  for (R_xlen_t row = 0; row < nodeCount; ) {
    int tree = INTEGER(treeID)[row];
    if (tree < 1 || (uint) tree > treeCount || seen[tree - 1]) {
      return "tree IDs are invalid or a tree occupies multiple row blocks.";
    }
    uint b = (uint) tree - 1;
    seen[b] = TRUE;
    int leaves = INTEGER(tLeafCount)[b];
    int rootSize = INTEGER(nodeSZ)[row];
    R_xlen_t firstRow = row;
    R_xlen_t leaf = leafOffset[b];
    R_xlen_t treeMembers = 0;
    R_xlen_t pending = 1;
    do {
      int branch = INTEGER(brnodeID)[row];
      int size = INTEGER(nodeSZ)[row];
      if (pending == 0 || branch < 0 || (leaves > 0 && size <= 0)) {
        return "invalid tree preorder or replicated node size.";
      }
      pending += (branch == 0) ? -1 : 1;
      if (branch == 0 && leaves > 0) {
        if (leaf == leafOffset[b + 1] || INTEGER(rmbrCT)[leaf] != size) {
          return "terminal replicate count does not match nodeSZ; unique counts are insufficient.";
        }
        treeMembers += INTEGER(rmbrCT)[leaf++];
      }
      row++;
    } while (row < nodeCount && INTEGER(treeID)[row] == tree);
    if (pending != 0 || leaf != leafOffset[b + 1] ||
        (leaves == 0 && row - firstRow != 1) ||
        (leaves > 0 && treeMembers != rootSize)) {
      return "tree topology, leaf counts, and replicated root size disagree.";
    }
  }
  for (uint b = 0; b < treeCount; b++) {
    if (!seen[b]) {
      return "a tree is missing from the saved topology.";
    }
  }
  for (R_xlen_t k = 0; k < memberCount; k++) {
    int id = INTEGER(rmbrID)[k];
    if (id < 1 || (uint) id > nSize) {
      return "a replicated observation ID is outside the training data.";
    }
  }
  return NULL;
}
