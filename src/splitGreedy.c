
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "splitGreedy.h"
#include "greedyOps.h"
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
#include "cdlRegr.h"
#include "cdlUtil.h"
#include "cdlInfo.h"
char getBestSplit(uint treeID, LatOptTreeObj *lotObj, GreedyObj *greedyMembr) {
  Node     *parent;
  NodeBase *parentBase;
  char *localSplitIndicator;
  uint  localSplitIndx;
  double localSplitValue;
  double *beta;
  uint leftSize, rightSize;
  char result, resultLasso, resultCart;
  double sumLasso,    sumCart;
  double sumLassoIBG, sumCartIBG;
  double sumLassoOOB, sumCartOOB;
  double eRiskLossLasso, eRiskLossCart;
  double eRiskLossLassoIBG, eRiskLossCartIBG;
  double eRiskLossLassoOOB, eRiskLossCartOOB;
  uint hcutCnt;
  uint hcut;
  uint i;
  uint jj;
  eRiskLossCartIBG = 0.0;
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
      }
    }  
    if ( (parent -> augm -> common -> hcut >= 1) &&
         (resultLasso == FALSE) &&
         !(SG_optLocal & ((SG_OPT_SWTCH_TWO) | (SG_OPT_SWTCH_THREE) | (SG_OPT_SWTCH_FOUR) | (SG_OPT_SWTCH_FIVE) | (SG_OPT_SWTCH_SIX))) ) {
      result = FALSE;
    }
    else {
      if ( (parent -> augm -> common -> hcut == 0) ||
           (SG_optLocal & ((SG_OPT_SWTCH_TWO) | (SG_OPT_SWTCH_THREE) | (SG_OPT_SWTCH_FOUR) | (SG_OPT_SWTCH_FIVE) | (SG_OPT_SWTCH_SIX))) ) {
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
      }
      if (parentBase -> depth == 0) {
        allMembrIndx = parentBase -> allMembrIndx;
        allMembrSize = parentBase -> allMembrSize; 
        if (RF_opt & OPT_EMPR_RISK) {
          SG_cart_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCart;
          SG_cart_oobEmprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCartOOB;
        }
        if ((greedyMembr -> splitInfoDerived != NULL) && (greedyMembr -> splitInfoDerivedCart != NULL)) {
          sumLassoIBG = 0.0;
          sumLassoOOB = 0.0;
          if (greedyMembr -> left -> outcome == OUTC_LASSO) {
            sumLassoIBG += greedyMembr -> left -> eRisk;
            sumLassoOOB += greedyMembr -> left -> eRiskOOB;
          }
          else {
            sumLassoIBG += greedyMembr -> left -> eRiskCart;
            sumLassoOOB += greedyMembr -> left -> eRiskCartOOB;
          }
          if (greedyMembr -> right -> outcome == OUTC_LASSO) {
            sumLassoIBG += greedyMembr -> right -> eRisk;
            sumLassoOOB += greedyMembr -> right -> eRiskOOB;  
          }
          else {
            sumLassoIBG += greedyMembr -> right -> eRiskCart;
            sumLassoOOB += greedyMembr -> right -> eRiskCartOOB;
          }
          sumCartIBG = (greedyMembr -> leftCart -> eRiskCart) + (greedyMembr -> rightCart -> eRiskCart);
          sumCartOOB = (greedyMembr -> leftCart -> eRiskCartOOB) + (greedyMembr -> rightCart -> eRiskCartOOB);
          if (SG_optLocal & SG_OPT_SWTCH_EIGHT) {
            sumCart = sumCartOOB;
            sumLasso = sumLassoOOB;
          }
          else  {
            sumCart = sumCartIBG;
            sumLasso = sumLassoIBG;
          }
          if ((SG_optLocal & SG_OPT_SWTCH_FIVE) && (sumCart <= sumLasso)) {
            parent -> hcut = 0;
            parent -> outcome = OUTC_CART;                   
            if (RF_opt & OPT_EMPR_RISK) {
              SG_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCart;
              SG_oobEmprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCartOOB;
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
              SG_oobEmprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskOOB;
            }
          }
        }
        else {
          if (greedyMembr -> splitInfoDerived != NULL) {
            if (RF_opt & OPT_EMPR_RISK) {
              SG_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRisk;
              SG_oobEmprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskOOB;
            }
          }
          else {
            parent -> hcut = 0;
            parent -> outcome = OUTC_CART;                   
            if (RF_opt & OPT_EMPR_RISK) {
              SG_emprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCart;
              SG_oobEmprRSKptr[treeID][lotObj -> treeSize] = parent -> eRiskCartOOB;
            }
          }
        }
        if (RF_opt & OPT_EMPR_RISK) {
        }
        insertRisk(treeID, lotObj, parent -> eRiskOOB);
      }  
      if (greedyMembr -> splitInfoDerived != NULL) {
        eRiskLossLassoIBG = parent -> eRisk;
        eRiskLossLassoOOB = parent -> eRiskOOB;
        sumLassoIBG = 0.0;
        sumLassoOOB = 0.0;
        if (greedyMembr -> left -> outcome == OUTC_LASSO) {
          sumLassoIBG += greedyMembr -> left -> eRisk;
          sumLassoOOB += greedyMembr -> left -> eRiskOOB;
          eRiskLossLassoIBG -= greedyMembr -> left -> eRisk;
          eRiskLossLassoOOB -= greedyMembr -> left -> eRiskOOB;
        }
        else {
          sumLassoIBG += greedyMembr -> left -> eRiskCart;
          sumLassoOOB += greedyMembr -> left -> eRiskCartOOB;
          eRiskLossLassoIBG -= greedyMembr -> left -> eRiskCart;
          eRiskLossLassoOOB -= greedyMembr -> left -> eRiskCartOOB;
        }
        if (greedyMembr -> right -> outcome == OUTC_LASSO) {
          sumLassoIBG += greedyMembr -> right -> eRisk;
          sumLassoOOB += greedyMembr -> right -> eRiskOOB;
          eRiskLossLassoIBG -= greedyMembr -> right -> eRisk;
          eRiskLossLassoOOB -= greedyMembr -> right -> eRiskOOB;
        }
        else {
          sumLassoIBG += greedyMembr -> right -> eRiskCart;
          sumLassoOOB += greedyMembr -> right -> eRiskCartOOB;
          eRiskLossLassoIBG -= greedyMembr -> right -> eRiskCart;
          eRiskLossLassoOOB -= greedyMembr -> right -> eRiskCartOOB;
        }
      }
      if (greedyMembr -> splitInfoDerivedCart != NULL) {
        sumCartIBG = (greedyMembr -> leftCart -> eRiskCart) + (greedyMembr -> rightCart -> eRiskCart);
        sumCartOOB = (greedyMembr -> leftCart -> eRiskCartOOB) + (greedyMembr -> rightCart -> eRiskCartOOB);      
        if (parent -> hcut >= 1) {
          eRiskLossCartIBG = (parent -> eRisk) - (greedyMembr -> leftCart -> eRiskCart) - (greedyMembr -> rightCart -> eRiskCart);
          eRiskLossCartOOB = (parent -> eRiskOOB) - (greedyMembr -> leftCart -> eRiskCartOOB) - (greedyMembr -> rightCart -> eRiskCartOOB);
        }
        else {
          eRiskLossCartIBG = (parent -> eRiskCart) - (greedyMembr -> leftCart -> eRiskCart) - (greedyMembr -> rightCart -> eRiskCart);
          eRiskLossCartOOB = (parent -> eRiskCartOOB) - (greedyMembr -> leftCart -> eRiskCartOOB) - (greedyMembr -> rightCart -> eRiskCartOOB);
        }
      }
      if (SG_optLocal & SG_OPT_SWTCH_EIGHT) {
        sumCart = sumCartOOB;
        sumLasso = sumLassoOOB;
        eRiskLossCart = eRiskLossCartOOB;
        eRiskLossLasso = eRiskLossLassoOOB;
      }
      else  {
        sumCart = sumCartIBG;
        sumLasso = sumLassoIBG;
        eRiskLossCart = eRiskLossCartIBG;
        eRiskLossLasso = eRiskLossLassoIBG;
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
  }  
  return (result) && (resultLasso || resultCart);
}
char virtuallySplitNodeGreedyLasso(uint       treeID,
                                  GreedyObj *greedyMembr,
                                  char   *localSplitIndicator,
                                  uint   *localSplitIndx,
                                  double *localSplitValue,
                                  uint   *leftSize) {
  Node     *parent;
  NodeBase *baseParent;
  uint  repMembrSize,  oobMembrSize;
  uint *repMembrIndx, *oobMembrIndx;
  double *yHatAbsoluteLasso;
  uint  rightSize;
  uint i, k;
  char result;
  result = TRUE;
  parent     = greedyMembr -> parent;
  baseParent = (NodeBase *) parent;
  AugmentationObjCommon *augmObjCommon = parent -> augm -> common;  
  repMembrSize = baseParent -> repMembrSize;
  repMembrIndx = baseParent -> repMembrIndx;
  oobMembrSize = baseParent -> oobMembrSize;
  oobMembrIndx = baseParent -> oobMembrIndx;
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
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Invalid split rule specified:  ", SG_splitRule);
    RF_nativeError("\nRF-SRC:  Please specify the rule named:  REGR_CDL");
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
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
  if (oobMembrSize > 0) {
    baseLeft ->  oobMembrIndx  = uivector(1, oobMembrSize);
    baseRight -> oobMembrIndx  = uivector(1, oobMembrSize);
    baseLeft  -> oobMembrSizeAlloc = oobMembrSize;
    baseRight -> oobMembrSizeAlloc = oobMembrSize;  
    uint leftSizeOOB  = 0;
    uint rightSizeOOB = 0;
    for (i = 1; i <= oobMembrSize; i++) {
      if (yHatAbsoluteLasso[oobMembrIndx[i]] <=  *localSplitValue) {
        leftSizeOOB ++;
        baseLeft -> oobMembrIndx[leftSizeOOB] = oobMembrIndx[i];
      }
      else {
        rightSizeOOB ++;
        baseRight -> oobMembrIndx[rightSizeOOB] = oobMembrIndx[i];
      }
    }
    baseLeft  -> oobMembrSize = leftSizeOOB;
    baseRight -> oobMembrSize = rightSizeOOB;
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
    if ( !(SG_optLocal & ((SG_OPT_SWTCH_TWO) | (SG_OPT_SWTCH_THREE) | (SG_OPT_SWTCH_FOUR) | (SG_OPT_SWTCH_FIVE) | (SG_OPT_SWTCH_SIX))) ) {
      result = FALSE;
      freeNode(left);
      freeNode(right);
      greedyMembr -> left = greedyMembr -> right = NULL;
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
  }
  return result;
}
void initializeBetaAndPredicted(uint treeID, Node *parent) {
  double **xArray;
  double **nodeBasis = NULL;
  double  *yArray;
  char    *perm;
  char    *nodePerm = NULL;
  uint *repMembrIndx, *oobMembrIndx;
  uint  repMembrSize,  oobMembrSize;
  uint  hcut, hcutCnt;
  double **beta;
  double  *yHatAbsoluteLasso, *yHatAbsoluteCart;
  double *eRiskPtr, *eRiskPtrCart;
  double predicted;
  double  localMean;
  uint i, jj;
  hcut    = parent -> augm -> common -> hcut;
  hcutCnt = parent -> augm -> common -> hcutCnt;
  yArray = parent -> augm -> common -> yArray;
  xArray = parent -> augm -> common -> xArray;
  repMembrIndx = ((NodeBase *) parent) -> repMembrIndx;
  repMembrSize = ((NodeBase *) parent) -> repMembrSize;
  oobMembrIndx = ((NodeBase *) parent) -> oobMembrIndx;
  oobMembrSize = ((NodeBase *) parent) -> oobMembrSize;
  perm = parent -> augm -> perm;
  if ((hcut >= 1) && (SG_optLocal & SG_OPT_SWTCH_NINE)) {
    double *center = dvector(1, SG_basisSourceSize);
    if (computeNodeCenter(repMembrSize, repMembrIndx, SG_weight, center)) {
      nodeBasis = makeNodeBasis(center,
                                repMembrSize, repMembrIndx,
                                oobMembrSize, oobMembrIndx);
    }
    free_dvector(center, 1, SG_basisSourceSize);
    if (nodeBasis == NULL) {
      initializeBetaAndPredictedCart(treeID, parent);
      parent -> eRisk = parent -> eRiskOOB = RF_nativeNaN;
      return;
    }
    xArray = nodeBasis;
    nodePerm = cvector(1, hcutCnt);
    perm = nodePerm;
    for (jj = 1; jj <= hcutCnt; jj++) {
      perm[jj] = FALSE;
      if (repMembrSize > 1) {
        char finite = TRUE;
        for (i = 1; i <= repMembrSize; i++) {
          if (!isfinite(xArray[jj][repMembrIndx[i]])) {
            finite = FALSE;
            break;
          }
        }
        if (finite) {
          double variance;
          getVarianceSinglePass(repMembrSize, repMembrIndx,
                                xArray[jj], NULL, &variance);
          perm[jj] = isfinite(variance) && (variance > EPSILON);
        }
      }
    }
  }
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
    eRiskPtrCart = & (parent -> eRiskCart);
    *eRiskPtrCart = 0.0;
    for (i = 1; i <= repMembrSize; i++) {
      *eRiskPtrCart += pow (yArray[repMembrIndx[i]] - localMean, 2.0);
    }
    eRiskPtrCart = & (parent -> eRiskCartOOB);
    *eRiskPtrCart = 0.0;
    for (i = 1; i <= oobMembrSize; i++) {
      *eRiskPtrCart += pow (yArray[oobMembrIndx[i]] - localMean, 2.0);
      yHatAbsoluteCart[oobMembrIndx[i]] = localMean;
    }
    if (obj -> valid == TRUE) {
      eRiskPtr = & (parent -> eRisk);
      *eRiskPtr = 0.0;
      for (i = 1; i <= repMembrSize; i++) {
        *eRiskPtr += pow (yArray[repMembrIndx[i]] - yHatAbsoluteLasso[repMembrIndx[i]], 2.0);
      }
      eRiskPtr = & (parent -> eRiskOOB);
      *eRiskPtr = 0.0;
      for (i = 1; i <= oobMembrSize; i++) {
        predicted = (*beta)[1];
        for (jj = 1; jj <= hcutCnt; jj++) {
          if ((nodeBasis == NULL) || (fabs((*beta)[jj+1]) > 0.0)) {
            predicted = predicted + ((*beta)[jj+1] * xArray[jj][oobMembrIndx[i]]);
          }
        }
        yHatAbsoluteLasso[oobMembrIndx[i]] = predicted;
        *eRiskPtr += pow (yArray[oobMembrIndx[i]] - yHatAbsoluteLasso[oobMembrIndx[i]], 2.0);
      }
    }
    else {
      parent -> eRisk = parent -> eRiskOOB = RF_nativeNaN;
    }
    freeCDLInfo(obj);
    if (nodeBasis != NULL) {
      freeNodeBasis(nodeBasis);
      free_cvector(nodePerm, 1, hcutCnt);
    }
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
