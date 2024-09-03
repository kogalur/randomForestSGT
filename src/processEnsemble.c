
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "processEnsemble.h"
#include "regressionDerived.h"
#include "shared/classification.h"
#include "shared/regression.h"
#include "shared/termBaseOps.h"
#include "shared/nrutil.h"
#include "shared/error.h"
void processEnsembleInSitu(char mode, uint treeID) {
  char perfFlag;
  double **responsePtr;
    if ((RF_opt & OPT_PERF) ||
        (RF_opt & OPT_OENS) ||
        (RF_opt & OPT_IENS) ||
        (RF_opt & OPT_FENS)) {
      if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
        omp_set_lock(&RF_lockPerf);
#endif
      }
      RF_serialTreeIndex[++RF_serialTreeID] = treeID;
      perfFlag = getPerfFlag(mode, RF_serialTreeID);
      if (!perfFlag) {
        if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
          omp_unset_lock(&RF_lockPerf);
#endif
        }
      }
      if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
        omp_set_lock(&RF_lockEnsbUpdtCount);
#endif
      }
      RF_ensembleUpdateCount++;
      if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
        omp_unset_lock(&RF_lockEnsbUpdtCount);
#endif
      }
      updateEnsemble(mode, treeID);
      if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
        omp_set_lock(&RF_lockEnsbUpdtCount);
#endif
      }
      RF_ensembleUpdateCount--;
      if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
        omp_unset_lock(&RF_lockEnsbUpdtCount);
#endif
      }
      if (perfFlag) {
        char ensbUpdtCountFlag = FALSE;
        while (!ensbUpdtCountFlag) {
          if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
            omp_set_lock(&RF_lockEnsbUpdtCount);
#endif
          }
          if (RF_ensembleUpdateCount  == 0) {
            ensbUpdtCountFlag = TRUE;
          }
          if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
            omp_unset_lock(&RF_lockEnsbUpdtCount);
#endif
          }
        }
        normalizeEnsembleEstimates(mode);
        switch (mode) {
        case RF_PRED:
          responsePtr = RF_fresponse[treeID];
          break;
        default:
          responsePtr = RF_response[treeID];
        }
        summarizeFaithfulBlockPerformance(mode,
                                          treeID,
                                          RF_serialTreeID,
                                          (mode == RF_PRED) ? RF_fullEnsembleCLSptr : RF_oobEnsembleCLSptr,
                                          (mode == RF_PRED) ? SG_cdl_fullEnsembleRGRptr : SG_cdl_oobEnsembleRGRptr,
                                          (mode == RF_PRED) ? RF_fullEnsembleDen    : RF_oobEnsembleDen,
                                          responsePtr,
                                          RF_perfCLSptr,
                                          RF_perfRGRptr);
        if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
          omp_unset_lock(&RF_lockPerf);
#endif
        }
      }  
    }  
}
void updateEnsemble (char mode, uint treeID) {
  LeafLinkedObj *leafLinkedPtr;
  if (RF_rTargetFactorCount > 0) {
    updateEnsembleClas(mode, treeID);
  }
  else if (RF_rTargetNonFactorCount > 0) {
      updateEnsembleMeanCDL(mode, treeID);
  }
  switch (mode) {
  case RF_GROW:
    if (!(RF_optHigh & OPT_TERM_OUTG)) {
      leafLinkedPtr = RF_leafLinkedObjHead[treeID] -> fwdLink;
      while (leafLinkedPtr != NULL) {
        leafLinkedPtr = leafLinkedPtr -> fwdLink;
      }
    }
    break;
  default:
    leafLinkedPtr = RF_leafLinkedObjHead[treeID] -> fwdLink;
    while (leafLinkedPtr != NULL) {
      leafLinkedPtr = leafLinkedPtr -> fwdLink;
    }
    break;
  }
}
void normalizeEnsembleEstimates(char mode) {
  char oobFlag, fullFlag;
  uint      obsSize;
  double ***ensembleCLSptr;
  double  **ensembleRGRptr;
  double ***ensembleCLSnum;
  double  **ensembleRGRnum;
  double   *ensembleDen;
  uint i, j, k;
  oobFlag = fullFlag = FALSE;
  switch (mode) {
  case RF_PRED:
    obsSize = RF_fobservationSize;
    oobFlag = FALSE;
    if (RF_opt & OPT_FENS) {
      fullFlag = TRUE;
    }
    break;
  default:
    obsSize = RF_observationSize;
    if (RF_opt & OPT_OENS) {
      oobFlag = TRUE;
    }
    if (RF_opt & OPT_IENS) {
      fullFlag = TRUE;
    }
    break;
  }
  while ((oobFlag == TRUE) || (fullFlag == TRUE)) {
    if (oobFlag == TRUE) {
      ensembleDen    = RF_oobEnsembleDen;
      ensembleCLSptr = RF_oobEnsembleCLSptr;
      ensembleRGRptr = SG_cdl_oobEnsembleRGRptr;
      ensembleCLSnum = RF_oobEnsembleCLSnum;
      ensembleRGRnum = SG_cdl_oobEnsembleRGRnum;
    }
    else {
      ensembleDen    = RF_fullEnsembleDen;
      ensembleCLSptr = RF_fullEnsembleCLSptr;
      ensembleRGRptr = SG_cdl_fullEnsembleRGRptr;
      ensembleCLSnum = RF_fullEnsembleCLSnum;
      ensembleRGRnum = SG_cdl_fullEnsembleRGRnum;
    }
      if (RF_rTargetFactorCount > 0) {
        for (i = 1; i <= obsSize; i++) {
          if (ensembleDen[i] != 0) {
            for (j = 1; j <= RF_rTargetFactorCount; j++) {
              for (k = 1; k <= RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]; k++) {
                ensembleCLSptr[j][k][i] = ensembleCLSnum[j][k][i] / ensembleDen[i];
              }
            }
          }
          else {
            for (j = 1; j <= RF_rTargetFactorCount; j++) {
              for (k = 1; k <= RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]; k++) {
                ensembleCLSptr[j][k][i] = RF_nativeNaN;
              }
            }
          }
        }
      }
      if (RF_rTargetNonFactorCount > 0) {      
        for (i = 1; i <= obsSize; i++) {
          if (ensembleDen[i] != 0) {
            for (j = 1; j <= RF_rTargetNonFactorCount; j++) {
              ensembleRGRptr[j][i] = ensembleRGRnum[j][i] / ensembleDen[i];
            }
          }
          else {
            for (j = 1; j <= RF_rTargetNonFactorCount; j++) {
              ensembleRGRptr[j][i] = RF_nativeNaN;
            }
          }
        }
      }
    if (oobFlag == TRUE) {
      oobFlag = FALSE;
    }
    else {
      fullFlag = FALSE;
    }
  }  
}
void summarizeFaithfulBlockPerformance (char        mode,
                                        uint        b,
                                        uint        blockID,
                                        double   ***blkEnsembleCLSnum,
                                        double    **blkEnsembleRGRnum,
                                        double     *blkEnsembleDen,
                                        double    **responsePtr,
                                        double   ***perfCLSblk,
                                        double    **perfRGRblk) {
  uint      obsSize;
  obsSize = (mode == RF_PRED) ?  RF_fobservationSize : RF_observationSize;
    if (RF_rTargetFactorCount > 0) {
      getPerformance(b,
                     mode,
                     obsSize,
                     responsePtr,
                     blkEnsembleDen,
                     blkEnsembleCLSnum,
                     NULL,
                     perfCLSblk[blockID],
                     NULL);
    }
    if (RF_rTargetNonFactorCount > 0) {
      getPerformance(b,
                     mode,
                     obsSize,
                     responsePtr,
                     blkEnsembleDen,
                     NULL,
                     blkEnsembleRGRnum,
                     NULL,
                     perfRGRblk[blockID]);
    }
}
void getPerformance(uint      serialTreeID,
                    char      mode,
                    uint      obsSize,
                    double  **responsePtr,
                    double    *denomPtr,
                    double  ***outcomeCLS,
                    double   **outcomeRGR,
                    double  **perfCLSptr,
                    double   *perfRGRptr) {
  uint j, k;
    if (perfCLSptr != NULL) {
      for (j = 1; j <= RF_rTargetFactorCount; j++) {
        if (TRUE) {
          double *cpv = dvector(1, RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]);
          double *maxVote = dvector(1, obsSize);
          getMaxVote(obsSize,
                     RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]],
                     outcomeCLS[j],
                     denomPtr,
                     maxVote);
          perfCLSptr[j][1] = getClassificationIndex(obsSize,
                                                    responsePtr[RF_rTargetFactor[j]],
                                                    denomPtr,
                                                    maxVote);
          getConditionalClassificationIndex(obsSize,
                                            RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]],
                                            responsePtr[RF_rTargetFactor[j]],
                                            outcomeCLS[j],
                                            maxVote,
                                            denomPtr,
                                            cpv);
          for (k = 1; k <= RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]; k++) {
            perfCLSptr[j][1+k] = cpv[k];
          }
          free_dvector(cpv, 1, RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]);
          free_dvector(maxVote, 1, obsSize);
        }
      }
    }
    if (perfRGRptr != NULL) {
      for (j = 1; j <= RF_rTargetNonFactorCount; j++) {
        perfRGRptr[j] = getMeanSquareError(obsSize,
                                           responsePtr[RF_rTargetNonFactor[j]],
                                           outcomeRGR[j],
                                           denomPtr);
      }
    }
}
char getPerfFlag (char mode, uint serialTreeID) {
  char result;
  if (RF_opt & OPT_PERF) {
    result = TRUE;
  }
  else {
    result = FALSE;
  }
  if (result) {
    if (serialTreeID % RF_perfBlock == 0){
    }
    else {
      if (serialTreeID == RF_ntree) {
      }
      else {
        result = FALSE;
      }
    }
  }
  return result;
}
