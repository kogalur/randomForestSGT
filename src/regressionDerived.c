
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "regressionDerived.h"
#include "augmentationOpsCommon.h"
#include "augmentationOpsCustom.h"
#include "augmentationOpsSimple.h"
#include "augmentationOps.h"
#include "cdlRegr.h"
#include "cdlUtil.h"
#include "cdlInfo.h"
#include "terminal.h"
#include "termOps.h"
#include "shared/nrutil.h"
void calculateAllTerminalNodeOutcomesSGT(char mode,
                                         uint treeID,
                                         TerminalBase  *term) {
  Terminal         *termSuper;
  NodeBase         *termMate;
  uint  allMembrSize;
  uint *allMembrIndx;
  uint *oobMembrIndx;
  uint *repMembrIndx;
  uint *ibgMembrIndx;
  uint  i;
  termSuper = (Terminal *) term;
  termMate  = term -> mate;
  term -> membrCount = termMate -> repMembrSize;
  termSuper -> repMembrCount = termMate -> repMembrSize;
  allMembrSize = termMate -> allMembrSize;
  allMembrIndx = termMate -> allMembrIndx;
  termSuper -> allMembrSize = allMembrSize;
  termSuper -> oobMembrCount = 0;
  termSuper -> ibgMembrCount = 0;
  termSuper -> oobMembrIndx = uivector(1, allMembrSize);
  termSuper -> repMembrIndx = uivector(1, allMembrSize);
  termSuper -> ibgMembrIndx = uivector(1, allMembrSize);
  oobMembrIndx = termSuper -> oobMembrIndx;
  repMembrIndx = termSuper -> repMembrIndx;
  ibgMembrIndx = termSuper -> ibgMembrIndx;
  for (i = 1; i <= allMembrSize; i++) {
    if (RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {
      oobMembrIndx[++ (termSuper -> oobMembrCount)] = allMembrIndx[i];
    }
    else {
      ibgMembrIndx[++ (termSuper -> ibgMembrCount)] = allMembrIndx[i];
    }
  }
  for (i = 1; i <= termSuper -> repMembrCount; i++) {
    repMembrIndx[i] = termMate -> repMembrIndx[i];
  }
  calculateMeanResponseCDL(treeID, termSuper);
}
void calculateMeanResponseCDL(uint treeID, Terminal *term) {
  TerminalBase *termBase;
  NodeBase *parentBase;
  Node *parent;
  uint  allMembrSize;
  uint *allMembrIndx;
  double *yArray;
  double *yHatAbsolute;
  uint i;
  termBase   = (TerminalBase *) term;
  parentBase = termBase -> mate;
  parent     = (Node *) parentBase;
  term -> mean = parent -> mean;
  allMembrSize = parentBase -> allMembrSize;
  allMembrIndx = parentBase -> allMembrIndx;
  if (SG_optLocal & SG_OPT_SWTCH_SIX) {
    if (!RF_nativeIsNaN(parent -> eRisk) && !RF_nativeIsNaN(parent -> eRiskCart)) {
      if (parent -> eRiskCart <= parent -> eRisk) {
        yArray = parent -> augm -> common -> yArray;
        yHatAbsolute = parent -> yHatAbsolute;
        for (i = 1; i <= allMembrSize; i++) {
          yHatAbsolute[allMembrIndx[i]] = term -> mean;
        }
      }
    }
  }
}
void updateEnsembleMeanCDL(char mode, uint treeID) {
  char oobFlag, fullFlag, outcomeFlag;
  uint    *membershipIndex;
  uint     membershipSize;
  double    **ensembleRGRnum;
  double     *ensembleDen;
  double   *yHatAbsolute;
  uint     i, ii;
#ifdef _OPENMP
  omp_lock_t   *lockDENptr;
#endif
  ensembleRGRnum = NULL;  
  ensembleDen    = NULL;  
  oobFlag = fullFlag = FALSE;
  switch (mode) {
  case RF_PRED:
    yHatAbsolute = SG_yHatAbsoluteTest[treeID];
    if (RF_opt & OPT_FENS) {
      fullFlag = TRUE;
    }
    break;
  default:
    yHatAbsolute = SG_yHatAbsolute[treeID];
    if (RF_opt & OPT_OENS) {
      if (RF_oobSize[treeID] > 0) {
        oobFlag = TRUE;
      }
    }
    if (RF_opt & OPT_IENS) {
      fullFlag = TRUE;
    }
    break;
  }
  outcomeFlag = TRUE;
  while ((oobFlag == TRUE) || (fullFlag == TRUE)) {
    if (oobFlag == TRUE) {
      ensembleRGRnum = SG_cdl_oobEnsembleRGRnum;
      ensembleDen    = RF_oobEnsembleDen;
      membershipSize  = RF_oobSize[treeID];
      membershipIndex = RF_oobMembershipIndex[treeID];
#ifdef _OPENMP
      lockDENptr      = RF_lockDENoens;
#endif
    }
    else {
      ensembleRGRnum = SG_cdl_fullEnsembleRGRnum;
      ensembleDen    = RF_fullEnsembleDen;
      switch (mode) {
      case RF_PRED:
        membershipSize = RF_fobservationSize;
        membershipIndex = RF_fidentityMembershipIndex;
        break;
      default:
        membershipSize  = RF_ibgSize[treeID];
        membershipIndex = RF_ibgMembershipIndex[treeID];
        break;
      }
#ifdef _OPENMP
      lockDENptr      = RF_lockDENfens;
#endif
    }
    for (i = 1; i <= membershipSize; i++) {
      ii = membershipIndex[i];
      if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
        omp_set_lock(&(lockDENptr[ii]));
#endif
      }
      ensembleDen[ii] ++;
      ensembleRGRnum[1][ii] += yHatAbsolute[ii];
      if (SG_threadV == SG_THREADV_NTREE) {
#ifdef _OPENMP
        omp_unset_lock(&(lockDENptr[ii]));
#endif
      }
    }  
    if (outcomeFlag == TRUE) {
      outcomeFlag = FALSE;
    }
    if (oobFlag == TRUE) {
      oobFlag = FALSE;
    }
    else {
      fullFlag = FALSE;
    }
  }  
}
void restoreMeanResponseCDL(uint treeID, Terminal *term) {
  TerminalBase *termBase;
  NodeBase *parentBase;
  Node *parent;
  uint *oobMembrIndx, *ibgMembrIndx;
  uint  oobMembrCount, ibgMembrCount;
  double **xArray;
  double *yHatAbsolute;
  double *beta;
  double  predicted;
  uint hcutCnt;
  uint i, jj;
  termBase   = (TerminalBase *) term;
  parentBase = termBase -> mate;
  parent     = (Node *) parentBase;
  oobMembrCount = term -> oobMembrCount;
  ibgMembrCount = term -> ibgMembrCount;
  oobMembrIndx  = term -> oobMembrIndx;
  ibgMembrIndx  = term -> ibgMembrIndx;
  yHatAbsolute = parent -> yHatAbsolute;
  if (RF_nativeIsNaN(parent -> mean)) {
    hcutCnt = parent -> augm -> common -> hcutCnt;    
    xArray = parent -> augm -> common -> xArray;
    beta = parent -> beta;
    for (i = 1; i <= oobMembrCount; i++) {
      predicted = beta[1];
      for (jj = 1; jj <= hcutCnt; jj++) {
        predicted = predicted + (beta[jj+1] * xArray[jj][oobMembrIndx[i]]);
      }
      yHatAbsolute[oobMembrIndx[i]] = predicted;
    }
    for (i = 1; i <= ibgMembrCount; i++) {
      predicted = beta[1];
      for (jj = 1; jj <= hcutCnt; jj++) {
        predicted = predicted + (beta[jj+1] * xArray[jj][ibgMembrIndx[i]]);
      }
      yHatAbsolute[ibgMembrIndx[i]] = predicted;
    }
  }
  else {
    term -> mean = parent -> mean;
    for (i = 1; i <= oobMembrCount; i++) {
      yHatAbsolute[oobMembrIndx[i]] = term -> mean;
    }
    for (i = 1; i <= ibgMembrCount; i++) {
      yHatAbsolute[ibgMembrIndx[i]] = term -> mean;
    }
  }
}
void updateEnsembleMeanRT(uint treeID) {
  uint    *membershipIndex;
  uint     membershipSize;
  double    **ensembleRGRnum;
  double     *ensembleDen;
  double   *yHatAbsolute;
  uint     i, ii;
#ifdef _OPENMP
  omp_lock_t   *lockDENptr;
#endif
  yHatAbsolute = SG_yHatAbsoluteTest[treeID];
  ensembleRGRnum = SG_cdl_fullEnsembleRGRnum;
  ensembleDen    = RF_fullEnsembleDen;
  membershipSize = RF_fobservationSize;
  membershipIndex = RF_fidentityMembershipIndex;
#ifdef _OPENMP
  lockDENptr      = RF_lockDENfens;
#endif
  for (i = 1; i <= membershipSize; i++) {
    ii = membershipIndex[i];
#ifdef _OPENMP
    omp_set_lock(&(lockDENptr[ii]));
#endif
    ensembleDen[ii] ++;
    ensembleRGRnum[1][ii] += yHatAbsolute[ii];
#ifdef _OPENMP
    omp_unset_lock(&(lockDENptr[ii]));
#endif
  }  
}
