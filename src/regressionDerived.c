
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
#include "shared/error.h"
void calculateAllTerminalNodeOutcomesSGT(char mode,
                                         uint treeID,
                                         TerminalBase  *term) {
  Terminal         *termSuper;
  NodeBase         *termMate;
  uint *allMembrIndx;
  uint  allMembrCount;
  uint *oobMembrIndx;
  uint  oobMembrCount;
  uint *repMembrIndx;
  uint  repMembrCount;
  uint  i;
  termSuper = (Terminal *) term;
  termMate  = term -> mate;
  term -> membrCount = termMate -> repMembrSize;
  repMembrCount = termSuper -> repMembrCount = termMate -> repMembrSize;
  oobMembrCount = termSuper -> oobMembrCount = termMate -> oobMembrSize;
  allMembrIndx = termMate -> allMembrIndx;
  oobMembrIndx = termMate -> oobMembrIndx;
  repMembrIndx = termMate -> repMembrIndx;
  termSuper -> ibgMembrCount = 0;
  allMembrCount = termSuper -> allMembrCount = termMate -> allMembrSize;
  termSuper -> repMembrIndx = uivector(1, repMembrCount);
  termSuper -> oobMembrIndx = uivector(1, allMembrCount);
  termSuper -> ibgMembrIndx = uivector(1, repMembrCount);
  for (i = 1; i <= oobMembrCount; i++) {
    termSuper -> oobMembrIndx[i] = oobMembrIndx[i];
  }
  for (i = 1; i <= repMembrCount; i++) {
    termSuper -> repMembrIndx[i] = repMembrIndx[i];
  }
  for (i = 1; i <= allMembrCount; i++) {
    if (!RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {  
      (termSuper -> ibgMembrCount) ++;
      termSuper -> ibgMembrIndx[ (termSuper -> ibgMembrCount) ] = allMembrIndx[i];
    }
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
        parent -> hcut = 0;
        parent -> outcome = OUTC_CART;
        yArray = parent -> augm -> common -> yArray;
        yHatAbsolute = parent -> yHatAbsolute;
        for (i = 1; i <= allMembrSize; i++) {
          yHatAbsolute[allMembrIndx[i]] = term -> mean;
        }
      }
    }
  }
  if ((SG_optLocal & SG_OPT_NODE_RESTRICT) && parent -> hcut > 0 &&
      term -> oobMembrCount > 0) {
    yArray = parent -> augm -> common -> yArray;
    yHatAbsolute = parent -> yHatAbsolute;
    NodeSupport support;
    support.lower = dvector(1, SG_basisSourceSize);
    support.upper = dvector(1, SG_basisSourceSize);
    double *nodeCenter = dvector(1, SG_basisSourceSize);
    if (!computeNodeCenterAndSupport(term -> repMembrCount, term -> repMembrIndx,
                                     SG_weight, yArray, nodeCenter, &support)) {
      free_dvector(support.lower, 1, SG_basisSourceSize);
      free_dvector(support.upper, 1, SG_basisSourceSize);
      free_dvector(nodeCenter, 1, SG_basisSourceSize);
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: Unable to reconstruct grow support for tree %u, node %u.",
                     treeID, parentBase -> nodeID);
      RF_nativeExit();
    }
    double *scratch = dvector(1, SG_basisSourceSize);
    for (i = 1; i <= term -> oobMembrCount; i++) {
      uint observationID = term -> oobMembrIndx[i];
      yHatAbsolute[observationID] =
        evaluateRestrictedNodePrediction(SG_basisSource, nodeCenter, &support,
                                         parent -> beta, observationID, scratch);
    }
    free_dvector(scratch, 1, SG_basisSourceSize);
    free_dvector(support.lower, 1, SG_basisSourceSize);
    free_dvector(support.upper, 1, SG_basisSourceSize);
    free_dvector(nodeCenter, 1, SG_basisSourceSize);
  }
}
void updateEnsembleMeanCDL(char mode, uint treeID) {
  char oobFlag, fullFlag;
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
  uint *oobMembrIndx, *repMembrIndx;
  uint  oobMembrCount, repMembrCount;
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
  repMembrCount = term -> repMembrCount;
  oobMembrIndx  = term -> oobMembrIndx;
  repMembrIndx  = term -> repMembrIndx;
  yHatAbsolute = parent -> yHatAbsolute;
  if (RF_nativeIsNaN(parent -> mean)) {
    hcutCnt = parent -> augm -> common -> hcutCnt;    
    xArray = parent -> augm -> common -> xArray;
    beta = parent -> beta;
    char restrictOOB = (SG_optLocal & SG_OPT_NODE_RESTRICT) && oobMembrCount > 0;
    NodeSupport support;
    double *scratch = NULL;
    double *nodeCenter = NULL;
    if ((SG_optLocal & SG_OPT_SWTCH_NINE) || restrictOOB) {
      nodeCenter = dvector(1, SG_basisSourceSize);
      char valid;
      if (restrictOOB) {
        support.lower = dvector(1, SG_basisSourceSize);
        support.upper = dvector(1, SG_basisSourceSize);
        valid = computeNodeCenterAndSupport(repMembrCount, repMembrIndx, SG_weight,
                                            parent -> augm -> common -> yArray,
                                            nodeCenter, &support);
      }
      else {
        valid = computeNodeCenter(repMembrCount, repMembrIndx, SG_weight, nodeCenter);
      }
      if (!valid) {
        free_dvector(nodeCenter, 1, SG_basisSourceSize);
        if (restrictOOB) {
          free_dvector(support.lower, 1, SG_basisSourceSize);
          free_dvector(support.upper, 1, SG_basisSourceSize);
        }
        RF_nativeError("\nRF-SGT:  *** ERROR *** ");
        RF_nativeError("\nRF-SGT: Unable to reconstruct %s for tree %u, node %u.",
                       restrictOOB ? "support" : "center", treeID, parentBase -> nodeID);
        RF_nativeExit();
      }
      if (restrictOOB) {
        scratch = dvector(1, SG_basisSourceSize);
      }
    }
    for (i = 1; i <= oobMembrCount; i++) {
      if (restrictOOB) {
        yHatAbsolute[oobMembrIndx[i]] =
          evaluateRestrictedNodePrediction(SG_basisSource, nodeCenter, &support,
                                           beta, oobMembrIndx[i], scratch);
        continue;
      }
      predicted = beta[1];
      for (jj = 1; jj <= hcutCnt; jj++) {
        if (nodeCenter == NULL) {
          predicted = predicted + (beta[jj+1] * xArray[jj][oobMembrIndx[i]]);
        }
        else if (fabs(beta[jj+1]) > 0.0) {
          predicted = predicted + (beta[jj+1] *
                                   evaluateNodeBasisTerm(SG_basisSource, nodeCenter,
                                                         oobMembrIndx[i], jj));
        }
      }
      yHatAbsolute[oobMembrIndx[i]] = predicted;
    }
    for (i = 1; i <= repMembrCount; i++) {
      predicted = beta[1];
      for (jj = 1; jj <= hcutCnt; jj++) {
        if (!(SG_optLocal & SG_OPT_SWTCH_NINE)) {
          predicted = predicted + (beta[jj+1] * xArray[jj][repMembrIndx[i]]);
        }
        else if (fabs(beta[jj+1]) > 0.0) {
          predicted = predicted + (beta[jj+1] *
                                   evaluateNodeBasisTerm(SG_basisSource, nodeCenter,
                                                         repMembrIndx[i], jj));
        }
      }
      yHatAbsolute[repMembrIndx[i]] = predicted;
    }
    if (restrictOOB) {
      free_dvector(scratch, 1, SG_basisSourceSize);
      free_dvector(support.lower, 1, SG_basisSourceSize);
      free_dvector(support.upper, 1, SG_basisSourceSize);
    }
    if (nodeCenter != NULL) {
      free_dvector(nodeCenter, 1, SG_basisSourceSize);
    }
  }
  else {
    term -> mean = parent -> mean;
    for (i = 1; i <= oobMembrCount; i++) {
      yHatAbsolute[oobMembrIndx[i]] = term -> mean;
    }
    for (i = 1; i <= repMembrCount; i++) {
      yHatAbsolute[repMembrIndx[i]] = term -> mean;
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
