
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "splitGreedy.h"
#include "greedyInfo.h"
#include "splitInfoDerived.h"
#include "augmentationOpsCommon.h"
#include "augmentationOpsCustom.h"
#include "augmentationOpsSimple.h"
#include "augmentationOps.h"
#include "splitUtil.h"
#include "node.h"
#include "splitRegr.h"
#include "shared/nrutil.h"
#include "shared/error.h"
#include "cvRegr.h"
#include "cdlUtil.h"
#include "cdlInfo.h"
GreedyObj *makeGreedyObj(Node *parent, GreedyObj *head) {
  GreedyObj *greedyObj = (GreedyObj*) gblock((size_t) sizeof(GreedyObj));
  greedyObj -> parent = parent;
  greedyObj -> parentCart = parent;
  greedyObj -> left = NULL;
  greedyObj -> right= NULL;
  greedyObj -> leftCart = NULL;
  greedyObj -> rightCart = NULL;
  greedyObj -> fwdLink = NULL;
  greedyObj -> bakLink = NULL;
  greedyObj -> head = head;
  greedyObj -> splitInfoDerived = NULL;
  greedyObj -> splitInfoDerivedCart = NULL;
  greedyObj -> leafFlag = FALSE;
  greedyObj -> eRiskLoss = RF_nativeNaN;
  greedyObj -> bestSplitType = 0;
  return greedyObj;
}
void freeGreedyObj(GreedyObj *gObj) {
  if (gObj -> splitInfoDerived != NULL) {
    freeSplitInfoDerived(gObj -> splitInfoDerived, gObj -> parent -> augm -> common);
  }
  if (gObj -> splitInfoDerivedCart != NULL) {
    freeSplitInfoDerived(gObj -> splitInfoDerivedCart, gObj -> parent -> augm -> common);
  }
  if (gObj -> left != NULL) {
    freeNode(gObj -> left);
  }
  if (gObj -> right != NULL) {
    freeNode(gObj -> right);
  }
  if (gObj -> leftCart != NULL) {
    freeNode(gObj -> leftCart);
  }
  if (gObj -> rightCart != NULL) {
    freeNode(gObj -> rightCart);
  }
  free_gblock(gObj, (size_t) sizeof(GreedyObj));
}
void freeGreedyObjList(GreedyObj *gObj) {
  if (gObj -> fwdLink != NULL) {
    freeGreedyObjList(gObj -> fwdLink);
  }
  freeGreedyObj(gObj);
}
double getNegLogLikelihood(uint    treeID,
                           double *response,
                           uint    maxLevel,
                           char    selectFlag,
                           char   *membershipFlag,
                           uint   *repMembrIndx,
                           uint    repMembrSize,
                           uint   *allMembrIndx,
                           uint    allMembrSize) {
  uint i, k;
  double *piHat;
  double result;
  piHat = dvector(1, maxLevel);
  for (k = 1; k <= maxLevel; k++) {
    piHat[k] = 0.0;
  }
  for (i = 1; i <= repMembrSize; i++) {
    piHat[(uint) response[repMembrIndx[i]]] += 1.0; 
  }
  for (k = 1; k <= maxLevel; k++) {
    piHat[k] = piHat[k] / repMembrSize;
  }
  result = 0.0;
  if (allMembrSize == 0) {
    for (i = 1; i <= repMembrSize; i++) {
      if (piHat[(uint) response[repMembrIndx[i]]] > 0) {
        result -= log(piHat[(uint) response[repMembrIndx[i]]]);
      }
    }
  }
  else {
    for (i = 1; i <= allMembrSize; i++) {
      if (membershipFlag[allMembrIndx[i]] == selectFlag) {
        if (piHat[(uint) response[allMembrIndx[i]]] > 0) {
          result -= log(piHat[(uint) response[allMembrIndx[i]]]);
        }
      }
    }
  }
  free_dvector(piHat, 1, maxLevel);
  return result;
}
LatOptTreeObj *makeLatOptTreeObj(uint lag) {
  LatOptTreeObj *lotObj = (LatOptTreeObj*) gblock((size_t) sizeof(LatOptTreeObj));
  lotObj -> lag = lag;
  lotObj -> risk  = dvector(1, lag + 1);
  for (uint i = 1; i <= lag; i++) {
    lotObj -> risk[i]  = 0.0;
  }
  lotObj -> firstIn = 0;
  lotObj -> lastIn  = 0;
  lotObj -> size = 0;
  lotObj -> strikeout = 0;
  lotObj -> firstOD = 0.0;
  lotObj -> treeSize = 1;
  return lotObj;
}
void freeLatOptTreeObj(LatOptTreeObj *lotObj) {
  free_dvector(lotObj -> risk,  1, lotObj -> lag + 1);
  free_gblock(lotObj, (size_t) sizeof(LatOptTreeObj));
}
void insertRisk(uint treeID, LatOptTreeObj *obj, double value) {
  double normalizedValue;
  if (obj -> lag > 0) {
    normalizedValue = value / (obj -> lag);
    if (obj -> size < (obj -> lag)) {
      if (obj -> size == 0) {
        (obj -> firstIn) ++;
      }
      else {
      }
      obj -> firstOD = obj -> firstOD + normalizedValue;
      (obj -> size) ++;
      (obj -> lastIn) ++;
    }
    else {
      double saveFirstOD = obj -> firstOD; 
      obj -> firstOD = obj -> firstOD - obj -> risk[obj -> firstIn] + normalizedValue;
      obj -> strikeout = (obj -> strikeout) + (((obj -> firstOD - saveFirstOD)) > 0 ? 1:0);
      if ((++ (obj -> lastIn))  > (obj -> lag)) obj -> lastIn  = 1;
      if ((++ (obj -> firstIn)) > (obj -> lag)) obj -> firstIn = 1;
    }
    obj -> risk[obj -> lastIn] = normalizedValue;
  }
}
char getBestSplit(uint treeID, LatOptTreeObj *lotObj, GreedyObj *greedyMembr) {
  Node     *parent;
  NodeBase *parentBase;
  char *localSplitIndicator;
  uint  localSplitIndx;
  double localSplitValue;
  double *beta;
  double *yHatAbsolute;
  uint leftSize, rightSize;
  char result, resultLasso, resultCart;
  double sumLasso, sumCart;
  double eRiskLossLasso, eRiskLossCart;
  double *eRiskOOB;
  double predicted;
  double **xArray;
  double  *yArray;
  uint hcutCnt;
  uint hcut;
  uint i;
  uint jj;
  eRiskLossCart = 0.0;
  parent = greedyMembr -> parent;
  parentBase = (NodeBase *) parent;
  result = getPreSplitResult(treeID,
                             parentBase,
                             RF_nodeSize,
                             parent -> augm -> common -> yArray);
  resultLasso = resultCart = FALSE;
  if (result) {
    uint  allMembrSize = parentBase -> allMembrSize;
    uint *allMembrIndx = parentBase -> allMembrIndx;
    hcutCnt = parent -> augm -> common -> hcutCnt;    
    hcut = parent -> hcut;
    if (hcut >= 1) {
      greedyMembr -> splitInfoDerived = makeSplitInfoDerived(parent -> augm -> common);
      localSplitIndicator = ((SplitInfoMax *) (greedyMembr -> splitInfoDerived)) -> indicator;
      for (i = 1; i <= allMembrSize; i++) {
        localSplitIndicator[allMembrIndx[i]] = NEITHER;
      }
      resultLasso = virtuallySplitNodeGreedyLasso(treeID,
                                            greedyMembr,
                                            localSplitIndicator,
                                            & localSplitIndx,  
                                            & localSplitValue,
                                            & leftSize);
      if (resultLasso) {
        rightSize = parentBase -> repMembrSize - leftSize;
        if ((leftSize != 0) && (rightSize != 0)) {
        }
        else {
          RF_nativeError("\nRF-SRC:  *** ERROR *** ");
          RF_nativeError("\nRF-SRC:  left or right size is zero:  (%10d or %10d)", leftSize, rightSize);
          RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
          RF_nativeExit();
        }
        beta = greedyMembr -> splitInfoDerived -> beta = dvector(1, hcutCnt + 1);
        beta[1] = parent -> beta[1];
        for (jj = 1; jj <= hcutCnt; jj++) {
          beta[jj + 1] = parent -> beta[jj + 1];
        }
        greedyMembr -> splitInfoDerived -> yStar = localSplitValue;
        greedyMembr -> splitInfoDerived -> yBar  = RF_nativeNaN;
      }
      else {
          RF_nativeError("\nRF-SRC:  *** ERROR *** ");
          RF_nativeError("\nRF-SRC:  lasso failure on parent node virtual split is impossible.");
          RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
          RF_nativeExit();
      }
    }  
    greedyMembr -> splitInfoDerivedCart = makeSplitInfoDerived(parent -> augm -> common);
    localSplitIndicator = ((SplitInfoMax *) (greedyMembr -> splitInfoDerivedCart)) -> indicator;
    for (i = 1; i <= allMembrSize; i++) {
      localSplitIndicator[allMembrIndx[i]] = NEITHER;
    }
    resultCart = virtuallySplitNodeGreedyCart(treeID,
                                          greedyMembr,
                                          localSplitIndicator,
                                          & localSplitIndx,  
                                          & localSplitValue,
                                          & leftSize);
    if (resultCart) {
      rightSize = parentBase -> repMembrSize - leftSize;
      if ((leftSize != 0) && (rightSize != 0)) {
      }
      else {
        RF_nativeError("\nRF-SRC:  *** ERROR *** ");
        RF_nativeError("\nRF-SRC:  left or right size is zero:  (%10d or %10d)", leftSize, rightSize);
        RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
        RF_nativeExit();
      }
      greedyMembr -> splitInfoDerivedCart -> yStar = 0.0;
      greedyMembr -> splitInfoDerivedCart -> yBar = parentBase -> mean;
      beta = greedyMembr -> splitInfoDerivedCart -> beta = dvector(1, hcutCnt + 1);
      for (jj = 1; jj <= hcutCnt; jj++) {
        beta[1 + jj] = 0.0;
      }
      beta[1 + ((SplitInfoMax *) (greedyMembr -> splitInfoDerivedCart)) -> splitParameter] = 1.0;
      beta[1] = - ((SplitInfoMax *) (greedyMembr -> splitInfoDerivedCart)) -> splitValueCont;
    }
    else {
      freeSplitInfoDerived(greedyMembr -> splitInfoDerivedCart, parent -> augm -> common);
      greedyMembr -> splitInfoDerivedCart = NULL;
      greedyMembr -> leftCart  = NULL;
      greedyMembr -> rightCart = NULL;
    }
    if (parentBase -> depth == 0) {
      allMembrIndx = parentBase -> allMembrIndx;
      allMembrSize = parentBase -> allMembrSize; 
      yHatAbsolute = parent -> yHatAbsolute;
      xArray = parent -> augm -> common -> xArray;
      yArray = parent -> augm -> common -> yArray;
      if ((greedyMembr -> splitInfoDerived != NULL) && (greedyMembr -> splitInfoDerivedCart != NULL)) {
        sumLasso = 0.0;
        if (greedyMembr -> left -> outcome == OUTC_LASSO) {
          sumLasso += greedyMembr -> left -> eRisk;
        }
        else {
          sumLasso += greedyMembr -> left -> eRiskCart;
        }
        if (greedyMembr -> right -> outcome == OUTC_LASSO) {
          sumLasso += greedyMembr -> right -> eRisk;          
        }
        else {
          sumLasso += greedyMembr -> right -> eRiskCart;          
        }
        sumCart = greedyMembr -> leftCart -> eRiskCart + greedyMembr -> rightCart -> eRiskCart;
        if ((SG_optLocal & SG_OPT_SWTCH_FIVE) && (sumCart <= sumLasso)) {
          parent -> hcut = 0;
          parent -> outcome = OUTC_CART;                   
          if (RF_opt & OPT_EMPR_RISK) {
            SG_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCart;
          }
          eRiskOOB = & (parent -> eRiskOOB);
          *eRiskOOB = 0.0;
          for (i = 1; i <= allMembrSize; i++) {
            yHatAbsolute[allMembrIndx[i]] = parent -> mean;
            if (RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {
              *eRiskOOB += pow (yArray[allMembrIndx[i]] - yHatAbsolute[allMembrIndx[i]], 2.0);
            }
          }
          freeSplitInfoDerived(greedyMembr -> splitInfoDerived, parent -> augm -> common);
          greedyMembr -> splitInfoDerived = NULL;
          freeNode(greedyMembr -> left);
          freeNode(greedyMembr -> right);
          greedyMembr -> left  = NULL;
          greedyMembr -> right = NULL;
        }
        else {
          if (RF_opt & OPT_EMPR_RISK) {
            SG_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRisk;
          }
          beta = parent -> beta;
          eRiskOOB = & (parent -> eRiskOOB);
          *eRiskOOB = 0.0;
          for (i = 1; i <= allMembrSize; i++) {
            if (RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {                        
              predicted = beta[1];
              for (jj = 1; jj <= hcutCnt; jj++) {
                predicted = predicted + (beta[jj+1] * xArray[jj][allMembrIndx[i]]);
              }
              yHatAbsolute[allMembrIndx[i]] = predicted;
              *eRiskOOB += pow (yArray[allMembrIndx[i]] - yHatAbsolute[allMembrIndx[i]], 2.0);
            } 
          }          
        }
      }
      else {
        if (greedyMembr -> splitInfoDerived != NULL) {
          if (RF_opt & OPT_EMPR_RISK) {
            SG_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRisk;
          }
          beta = parent -> beta;
          eRiskOOB = & (parent -> eRiskOOB);
          *eRiskOOB = 0.0;
          for (i = 1; i <= allMembrSize; i++) {
            if (RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {                        
              predicted = beta[1];
              for (jj = 1; jj <= hcutCnt; jj++) {
                predicted = predicted + (beta[jj+1] * xArray[jj][allMembrIndx[i]]);
              }
              yHatAbsolute[allMembrIndx[i]] = predicted;
              *eRiskOOB += pow (yArray[allMembrIndx[i]] - yHatAbsolute[allMembrIndx[i]], 2.0);
            } 
          }          
        }
        else {
          parent -> hcut = 0;
          parent -> outcome = OUTC_CART;                   
          if (RF_opt & OPT_EMPR_RISK) {
            SG_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCart;
          }
          eRiskOOB = & (parent -> eRiskOOB);
          *eRiskOOB = 0.0;
          for (i = 1; i <= allMembrSize; i++) {
            yHatAbsolute[allMembrIndx[i]] = parent -> mean;
            if (RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {
              *eRiskOOB += pow (yArray[allMembrIndx[i]] - yHatAbsolute[allMembrIndx[i]], 2.0);
            }
          }
        }
      }
      if (RF_opt & OPT_EMPR_RISK) {
        SG_oobEmprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskOOB;
      }
      insertRisk(treeID, lotObj, parent -> eRiskOOB);
    }  
    if (greedyMembr -> splitInfoDerived != NULL) {
      eRiskLossLasso = parent -> eRisk;
      sumLasso = 0.0;
      if (greedyMembr -> left -> outcome == OUTC_LASSO) {
        sumLasso += greedyMembr -> left -> eRisk;
        eRiskLossLasso -= greedyMembr -> left -> eRisk;
      }
      else {
        sumLasso += greedyMembr -> left -> eRiskCart;
        eRiskLossLasso -= greedyMembr -> left -> eRiskCart;
      }
      if (greedyMembr -> right -> outcome == OUTC_LASSO) {
        sumLasso += greedyMembr -> right -> eRisk;
        eRiskLossLasso -= greedyMembr -> right -> eRisk;          
      }
      else {
        sumLasso += greedyMembr -> right -> eRiskCart;
        eRiskLossLasso -= greedyMembr -> right -> eRiskCart;          
      }
    }
    if (greedyMembr -> splitInfoDerivedCart != NULL) {
      sumCart = greedyMembr -> leftCart -> eRiskCart + greedyMembr -> rightCart -> eRiskCart;      
      if (parent -> hcut >= 1) {
        eRiskLossCart = parent -> eRisk - greedyMembr -> leftCart -> eRiskCart - greedyMembr -> rightCart -> eRiskCart;
      }
      else {
        eRiskLossCart = parent -> eRiskCart - greedyMembr -> leftCart -> eRiskCart - greedyMembr -> rightCart -> eRiskCart;
      }
    }
    if ((greedyMembr -> splitInfoDerived != NULL) && (greedyMembr -> splitInfoDerivedCart != NULL)) {    
      if (SG_optLocal & SG_OPT_SWTCH_FOUR) {
        if (sumLasso >= sumCart) {
          greedyMembr -> bestSplitType = REGR_CRT;
          greedyMembr -> eRiskLoss = eRiskLossCart;
        }
        else {
          greedyMembr -> bestSplitType = REGR_CDL;
          greedyMembr -> eRiskLoss = eRiskLossLasso;
        }
      }
      else {
        greedyMembr -> bestSplitType = REGR_CDL;
        greedyMembr -> eRiskLoss = eRiskLossLasso;
      }
    }
    else {
      if (greedyMembr -> splitInfoDerived != NULL) {
        greedyMembr -> bestSplitType = REGR_CDL;
        greedyMembr -> eRiskLoss = eRiskLossLasso;          
      }
      else {
        greedyMembr -> bestSplitType = REGR_CRT;
        greedyMembr -> eRiskLoss = eRiskLossCart;
      }
    }
  }  
  return (result && (resultLasso || resultCart));
}
char virtuallySplitNodeGreedyLasso(uint       treeID,
                                  GreedyObj *greedyMembr,
                                  char   *localSplitIndicator,
                                  uint   *localSplitIndx,
                                  double *localSplitValue,
                                  uint   *leftSize) {
  Node     *parent;
  NodeBase *baseParent;
  uint  repMembrSize;
  uint *repMembrIndx;
  double *yHatAbsoluteLasso;
  uint  rightSize;
  uint i, k;
  parent     = greedyMembr -> parent;
  baseParent = (NodeBase *) parent;
  AugmentationObjCommon *augmObjCommon = parent -> augm -> common;  
  repMembrSize = baseParent -> repMembrSize;
  repMembrIndx = baseParent -> repMembrIndx;
  yHatAbsoluteLasso = parent -> yHatAbsolute;
  double *yHat = dvector(1, repMembrSize);
  for (i = 1; i <= repMembrSize; i++) {
    yHat[i] = yHatAbsoluteLasso[repMembrIndx[i]];
  }
  *localSplitIndx = 0;
  if (SG_splitRule == REGR_CDL) {
    *localSplitValue = splitRuleGreedyOne(treeID, repMembrSize, yHat);
  }
  else {
    *localSplitValue = splitRuleGreedyTwo(treeID, repMembrSize, yHat);
  }
  Node *left  = makeNode(augmObjCommon -> pSize);
  Node *right = makeNode(augmObjCommon -> pSize);
  left  -> nSize = augmObjCommon -> nSize;
  right -> nSize = augmObjCommon -> nSize;
  greedyMembr -> left  = left;
  greedyMembr -> right = right;
  NodeBase *baseLeft  = (NodeBase *) left;
  NodeBase *baseRight = (NodeBase *) right;
  baseLeft ->  repMembrIndx  = uivector(1, repMembrSize);
  baseRight -> repMembrIndx  = uivector(1, repMembrSize);
  baseLeft  -> repMembrSizeAlloc = repMembrSize;
  baseRight -> repMembrSizeAlloc = repMembrSize;  
  *leftSize = 0;
  rightSize = 0;
  for (i = 1; i <= repMembrSize; i++) {
    if (yHat[i] <=  *localSplitValue) {
      localSplitIndicator[repMembrIndx[i]] = LEFT;
      (*leftSize) ++;
      baseLeft -> repMembrIndx[*leftSize] = repMembrIndx[i];
    }
    else {
      localSplitIndicator[repMembrIndx[i]] = RIGHT;
      rightSize ++;
      baseRight -> repMembrIndx[rightSize] = repMembrIndx[i];
    }
  }
  baseLeft  -> repMembrSize = *leftSize;
  baseRight -> repMembrSize = rightSize;
  if ((*leftSize == 0) || (*leftSize == repMembrSize)) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Left or Right Daughter of size zero:  (%10d, %10d)", *leftSize, repMembrSize);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  free_dvector(yHat, 1, repMembrSize);
  for (k = 1; k <= baseParent -> xSize; k++) {
    baseLeft -> permissible[k] = baseRight -> permissible[k] = baseParent -> permissible[k];
  }
  left  -> augm =  getAugmentationObjGeneric(augmObjCommon, treeID, left);
  right -> augm =  getAugmentationObjGeneric(augmObjCommon, treeID, right);
  left -> yHatAbsoluteLasso  = dvector(1, augmObjCommon -> nSize);
  left -> yHatAbsoluteCart   = dvector(1, augmObjCommon -> nSize);
  right -> yHatAbsoluteLasso = dvector(1, augmObjCommon -> nSize);
  right -> yHatAbsoluteCart  = dvector(1, augmObjCommon -> nSize);
  initializeBetaAndPredicted(treeID, left);
  initializeBetaAndPredicted(treeID, right);
  if (!RF_nativeIsNaN(left -> eRisk)  && !RF_nativeIsNaN(right -> eRisk)) {
    if (SG_optLocal & SG_OPT_SWTCH_TWO) {
      if ((left -> eRisk) + (right -> eRisk) >= (left -> eRiskCart) + (right -> eRiskCart)) {
        left -> outcome = OUTC_LASSO;
        right -> outcome = OUTC_LASSO;
      }
      else {
        left -> outcome = OUTC_CART;
        right -> outcome = OUTC_CART;
      }
    }
    else if (SG_optLocal & SG_OPT_SWTCH_THREE) {
      if (left -> eRisk >= left -> eRiskCart) {
        left -> outcome = OUTC_LASSO;
      }
      else {
        left -> outcome = OUTC_CART;
      }
      if (right -> eRisk >= right -> eRiskCart) {
        right -> outcome = OUTC_LASSO;
      }
      else {
        right -> outcome = OUTC_CART;
      }
    }
    else {
      left  -> outcome = OUTC_LASSO;
      right -> outcome = OUTC_LASSO;
    }
  }
  else {
    if (!RF_nativeIsNaN(left -> eRisk)) {
      left -> outcome = OUTC_LASSO;
    }
    else {
      left -> outcome = OUTC_CART;
    }
    if (!RF_nativeIsNaN(right -> eRisk)) {
      right -> outcome = OUTC_LASSO;
    }
    else {
      right -> outcome = OUTC_CART;
    }
  }
  return TRUE;
}
void initializeBetaAndPredicted(uint treeID, Node *parent) {
  double **xArray;
  double  *yArray;
  char    *perm;
  uint *repMembrIndx;
  uint  repMembrSize;
  uint  hcut, hcutCnt;
  double **beta;
  double  *yHatAbsoluteLasso, *yHatAbsoluteCart;
  double *eRiskPtr, *eRiskPtrCart;
  double  localMean;
  uint i, jj;
  hcut    = parent -> augm -> common -> hcut;
  hcutCnt = parent -> augm -> common -> hcutCnt;
  yArray = parent -> augm -> common -> yArray;
  xArray = parent -> augm -> common -> xArray;
  repMembrIndx = ((NodeBase *) parent) -> repMembrIndx;
  repMembrSize = ((NodeBase *) parent) -> repMembrSize;
  perm = parent -> augm -> perm;
  beta = & (parent -> beta);
  yHatAbsoluteLasso = parent -> yHatAbsoluteLasso;
  yHatAbsoluteCart  = parent -> yHatAbsoluteCart;
  *beta = dvector(1, hcutCnt + 1);
  if (hcut >= 1) {
    CDLInfo  *obj = crossValidationRegr(treeID,
                                        SG_lambdaGrid,
                                        SG_lambdaGridSize,
                                        SG_lambdaMinRatio,
                                        SG_lambdaEpsilon,                           
                                        SG_convergThreshold,
                                        SG_maxIter,
                                        SG_efficiency,
                                        SG_weight,
                                        repMembrSize,
                                        hcutCnt,
                                        repMembrIndx,
                                        perm,
                                        SG_nfolds,
                                        yArray,
                                        xArray);
    (*beta)[1] = obj -> beta[obj -> lambdaTargetIndx][1];
    for (jj = 1; jj <= hcutCnt; jj++) {
      (*beta)[jj + 1] = obj -> beta[obj -> lambdaTargetIndx][jj + 1];
    }
    localMean = 0.0;
    for (i = 1; i <= repMembrSize; i++) {
      localMean += yArray[repMembrIndx[i]];
      yHatAbsoluteLasso[repMembrIndx[i]] = obj -> yHat[obj -> lambdaTargetIndx][i];
    }
    localMean = localMean / repMembrSize;
    parent -> mean = localMean;
    for (i = 1; i <= repMembrSize; i++) {
      yHatAbsoluteCart[repMembrIndx[i]] = localMean;
    }
    eRiskPtr = & (parent -> eRisk);
    eRiskPtrCart = & (parent -> eRiskCart);
    *eRiskPtrCart = 0.0;
    for (i = 1; i <= repMembrSize; i++) {
      *eRiskPtrCart += pow (yArray[repMembrIndx[i]] - parent -> mean, 2.0);
    }
    if (obj -> valid == TRUE) {
      *eRiskPtr = 0.0;
      for (i = 1; i <= repMembrSize; i++) {
        *eRiskPtr += pow (yArray[repMembrIndx[i]] - yHatAbsoluteLasso[repMembrIndx[i]], 2.0);
      }
    }
    else {
      (*eRiskPtr) = RF_nativeNaN;
    }
    freeCDLInfo(obj);
  }
  else {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  CV Beta Initialization called when hcut is zero (0).");
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
}
double splitRuleGreedyOne(uint treeID, uint repMembrSize, double *yHat) {
  uint m;
  double localSplitValue;
  uint *yHatSortedIndx = uivector(1, repMembrSize);
  indexx(repMembrSize, yHat, yHatSortedIndx);
  double *sHatFwd = dvector(1, repMembrSize);
  double *sHatBak = dvector(1, repMembrSize);
  sHatFwd[1] = yHat[yHatSortedIndx[1]];
  sHatBak[repMembrSize] = yHat[yHatSortedIndx[repMembrSize]];
  for (m = 2; m <= repMembrSize; m++) {
    sHatFwd[m] = sHatFwd[m-1] + yHat[yHatSortedIndx[m]];
  }
  for (m = repMembrSize - 1 ; m >= 1; m--) {
    sHatBak[m] = sHatBak[m+1] + yHat[yHatSortedIndx[m]];
  }
  double *thetaHat = dvector(1, repMembrSize);
  double result1, result2;
  thetaHat[repMembrSize] = 0.0;
  for (m = 1; m <= repMembrSize - 1; m++) {
    result1 = sHatFwd[m] * sHatFwd[m];
    result1 = result1 / m;
    result2 = sHatBak[m+1] * sHatBak[m+1];
    result2 = result2 / (repMembrSize - m);
    thetaHat[m] = result1 + result2;
  }
  double maxValue;
  uint   maxIndx;
  maxIndx = 0;
  maxValue = -1.0;
  for (m = 1; m <= repMembrSize - 1; m++) {
    if (maxValue < thetaHat[m]) {
      maxValue = thetaHat[m];
      maxIndx = m;
    }
  }
  localSplitValue = yHat[yHatSortedIndx[maxIndx]];
  free_dvector(thetaHat, 1, repMembrSize);
  free_dvector(sHatFwd, 1, repMembrSize);
  free_dvector(sHatBak, 1, repMembrSize);
  free_uivector(yHatSortedIndx, 1, repMembrSize);
  return localSplitValue;
}
double splitRuleGreedyTwo(uint treeID, uint repMembrSize, double *yHat) {
  double normSumLeft, normSumRight;
  uint m, n;
  double localSplitValue;
  uint *yHatSortedIndx = uivector(1, repMembrSize);
  indexx(repMembrSize, yHat, yHatSortedIndx);
  double *sHatFwd = dvector(1, repMembrSize);
  double *sHatBak = dvector(1, repMembrSize);
  sHatFwd[1] = yHat[yHatSortedIndx[1]];
  sHatBak[repMembrSize] = yHat[yHatSortedIndx[repMembrSize]];
  for (m = 2; m <= repMembrSize; m++) {
    sHatFwd[m] = sHatFwd[m-1] + yHat[yHatSortedIndx[m]];
  }
  for (m = repMembrSize - 1 ; m >= 1; m--) {
    sHatBak[m] = sHatBak[m+1] + yHat[yHatSortedIndx[m]];
  }
  double *theta = dvector(1, repMembrSize);
  double result1, result2;
  theta[repMembrSize] = 0.0;  
  for (m = 1; m <= repMembrSize - 1; m++) {
    normSumLeft = sHatFwd[m] / m;
    normSumRight = sHatBak[m+1] / (repMembrSize - m);
    result1 = result2 = 0.0;
    for (n = 1; n <= m; n++) {
      result1 += (yHat[yHatSortedIndx[n]] - normSumLeft) * (yHat[yHatSortedIndx[n]] - normSumLeft);
    }
    for (n = m + 1; n <= repMembrSize; n++) {
      result2 += (yHat[yHatSortedIndx[n]] - normSumRight) * (yHat[yHatSortedIndx[n]] - normSumRight);
    }
    theta[m] = result1 + result2;
  }
  double minValue;
  uint   minIndx;
  minIndx = 0;
  minValue = DBL_MAX;
  for (m = 1; m <= repMembrSize - 1; m++) {
    if (minValue > theta[m]) {
      minValue = theta[m];
      minIndx = m;
    }
  }
  localSplitValue = yHat[yHatSortedIndx[minIndx]];
  free_dvector(theta, 1, repMembrSize);
  free_dvector(sHatFwd, 1, repMembrSize);
  free_dvector(sHatBak, 1, repMembrSize);
  free_uivector(yHatSortedIndx, 1, repMembrSize);
  return localSplitValue;
}
