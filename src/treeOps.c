
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "treeOps.h"
#include "terminal.h"
#include "node.h"
#include "stackOutput.h"
#include "sgtMain.h"
#include "termOps.h"
#include "greedyInfo.h"
#include "greedyOps.h"
#include "splitGreedy.h"
#include "splitRegr.h"
#include "processEnsemble.h"
#include "shared/stackMembershipVectors.h"
#include "shared/stackPreDefined.h"
#include "shared/terminalBase.h"
#include "shared/nrutil.h"
#include "shared/restoreTree.h"
#include "shared/bootstrap.h"
#include "shared/nodeBaseOps.h"
#include "shared/termBaseOps.h"
#include "shared/polarityNew.h"
#include "shared/error.h"
void acquireTree(char mode, uint treeID) {
  Node     *root;
  NodeBase *rootBase;
  uint  nSize, xSize;
  uint  nSizeTest;
  uint *bootMembrIndx;
  uint  bootMembrSize;
  char  bootResult;
  ulong offsetTree;
  ulong offsetCT, offsetID_rmbr, offsetID_ombr;
  uint  i, k;
#ifdef _OPENMP
#endif
  nSize = SG_augmObjCommon -> nSize;
  xSize = SG_augmObjCommon -> pSize;
  nSizeTest = RF_fobservationSize;
  stackMembershipVectors(nSize,
                         &RF_bootMembershipFlag[treeID],
                         &RF_oobMembershipFlag[treeID],
                         &RF_bootMembershipCount[treeID],
                         &RF_ibgMembershipIndex[treeID],
                         &RF_oobMembershipIndex[treeID]);
  stackFactorInSitu(treeID,
                    RF_rFactorCount,
                    RF_xFactorCount,
                    RF_maxFactorLevel,
                    RF_rFactorSize,
                    RF_xFactorSize,
                    RF_factorList);
  RF_nodeMembership[treeID] = (NodeBase **) new_vvector(1, nSize, NRUTIL_NPTR);
  RF_tTermMembership[treeID] = (TerminalBase **) new_vvector(1, nSize, NRUTIL_TPTR);
  RF_leafLinkedObjHead[treeID] = RF_leafLinkedObjTail[treeID] = makeLeafLinkedObj();
  root = makeNode(xSize);
  root -> nSize = nSize;
  rootBase = (NodeBase *) root;
  RF_root[treeID] = rootBase;
  rootBase -> parent = NULL;
  rootBase -> nodeID = 1;
  for (k = 1; k <= rootBase -> xSize; k++) {
    rootBase -> permissible[k] = TRUE;
  }
  rootBase -> allMembrSizeAlloc = rootBase -> allMembrSize = nSize;
  rootBase -> allMembrIndx = uivector(1, rootBase -> allMembrSizeAlloc);
  for (i = 1; i <= nSize; i++) {
    rootBase -> allMembrIndx[i] = i;
  }
  SG_bsfOrder = 0;
  if ( (RF_opt & OPT_BOOT_TYP1) || (RF_opt & OPT_BOOT_TYP2)) {
    bootMembrSize = RF_bootstrapSize;
  }
  else {
    bootMembrSize = nSize;
  }
  bootMembrIndx  = uivector(1, bootMembrSize);
  bootResult = bootstrap (mode,
                          treeID,
                          rootBase,  
                          rootBase -> allMembrIndx,
                          rootBase -> allMembrSize,
                          bootMembrSize,
                          RF_bootstrapIn,
                          RF_subjSize,
                          RF_subjWeight,
                          RF_subjWeightType,
                          RF_subjWeightSorted,
                          RF_subjWeightDensitySize,
                          nSize,
                          bootMembrIndx,
                          RF_bootMembershipFlag,
                          RF_oobMembershipFlag,
                          RF_bootMembershipCount,
                          RF_oobSize,
                          RF_ibgSize,
                          RF_ibgMembershipIndex,
                          RF_oobMembershipIndex,
                          SG_BOOT_CT_ptr);
  if (bootResult) {
    if (mode == RF_GROW) {
      rootBase -> repMembrIndx = bootMembrIndx;
      rootBase -> repMembrSize = rootBase -> repMembrSizeAlloc = bootMembrSize;
      SG_yHatAbsolute[treeID] = dvector(1, nSize);
      root -> yHatAbsolute = SG_yHatAbsolute[treeID];
      growTreeLOT(treeID, root);
    }
    else {
      rootBase -> repMembrIndx = bootMembrIndx;
      rootBase -> repMembrSize = rootBase -> repMembrSizeAlloc = bootMembrSize;
      SG_yHatAbsolute[treeID] = dvector(1, nSize);
      root -> yHatAbsolute = SG_yHatAbsolute[treeID];
      root -> augm = getAugmentationObjGeneric(SG_augmObjCommon, treeID, root);
      restoreTreeLOT(treeID, root, SG_offsetTree);
      if (mode == RF_PRED) {
        SG_yHatAbsoluteTest[treeID] = dvector(1, nSizeTest);
        root -> testMembrSize = root -> testMembrSizeAlloc = RF_fobservationSize;
        root -> testMembrIndx = RF_fidentityMembershipIndex;
        getTestMembershipLOT(treeID, root);
      }
    }
  }
  else {
  }
  if (bootResult) {
      RF_nodeCount[treeID]  = (RF_tLeafCount[treeID] << 1) - 1;
      processEnsembleInSitu(mode, treeID);
  }
  else {
    RF_nodeCount[treeID] = 1;
    RF_tLeafCount[treeID] = 1;
  }
  if (mode == RF_GROW) {
    if (RF_opt & OPT_TREE) {
      offsetTree = offsetCT = offsetID_rmbr = offsetID_ombr = 0;
      stackTreeObjectsPtrOnly(mode, treeID);
      saveTree(treeID, rootBase, & offsetTree, & offsetCT, & offsetID_rmbr, & offsetID_ombr);
    }
  }
  if (bootResult) {
    rootBase -> repMembrIndx = NULL;
    rootBase -> repMembrSize = rootBase -> repMembrSizeAlloc = 0;
    if (mode == RF_GROW) {
      free_dvector(SG_yHatAbsolute[treeID], 1, nSize);
    }
    else {
      free_dvector(SG_yHatAbsolute[treeID], 1, nSize);
      if (mode == RF_PRED) {
        root -> testMembrIndx = NULL;
        root -> testMembrSize = root -> testMembrSizeAlloc = 0;
        free_dvector(SG_yHatAbsoluteTest[treeID], 1, nSizeTest);
      }
    }
  }
  free_uivector(bootMembrIndx, 1, bootMembrSize);
  free_new_vvector(RF_nodeMembership[treeID], 1, nSize, NRUTIL_NPTR);
  unstackFactorInSitu(treeID,
                      RF_rFactorCount,
                      RF_xFactorCount,
                      RF_maxFactorLevel,
                      RF_factorList);
  unstackMembershipVectors(nSize,
                           RF_bootMembershipFlag[treeID],
                           RF_oobMembershipFlag[treeID],
                           RF_bootMembershipCount[treeID],
                           RF_ibgMembershipIndex[treeID],
                           RF_oobMembershipIndex[treeID]);
  freeTree(treeID, rootBase);
}
void freeTree(uint treeID, NodeBase *parent) {
  if (parent != NULL) {
    if ((parent -> left != NULL) && (parent -> right != NULL)) {
      freeTree(treeID, ((NodeBase *) parent) -> left);
      freeTree(treeID, ((NodeBase *) parent) -> right);
    }
    freeNode((Node *) parent);
  }
}
char growTreeLOT (uint treeID, Node *root) {
  Node     *parent;
  NodeBase *parentBase;
  Node     *left;
  NodeBase *leftBase;
  Node     *right;
  NodeBase *rightBase;
  uint *allMembrIndx;
  uint  allMembrSize;
  double  *yArray;
  double *yHatAbsolute;
  char  splitResult;
  char  result;
  GreedyObj *greedyHead, *greedyMembr;
  GreedyObj *greedyBest;
  double dropBest, dropCurrent;
  LatOptTreeObj *lotObj;
  double *eRiskOOB;
  double delta;
  char   updateFlag;
  char   growFlag;
  uint i;
  RF_tLeafCount[treeID] = 1;
  greedyHead = makeGreedyObj(NULL, NULL);
  result = FALSE;
  greedyHead -> head = greedyHead;
  greedyMembr = makeGreedyObj(root, greedyHead);
  greedyHead -> fwdLink = greedyMembr;
  greedyMembr -> bakLink = greedyHead;
  root -> augm = getAugmentationObjGeneric(SG_augmObjCommon, treeID, root);
  root -> yHatAbsoluteLasso = dvector(1, root -> nSize);
  root -> yHatAbsoluteCart  = dvector(1, root -> nSize);
  parent = root;
  parentBase = (NodeBase *) root;
  uint hcut = parent -> augm -> common -> hcut;
  allMembrIndx = parentBase -> allMembrIndx;
  allMembrSize = parentBase -> allMembrSize;
  if (hcut == 0) {
    initializeBetaAndPredictedCart(treeID, root);
    root -> hcut = 0;
    root -> outcome = OUTC_CART;
    for (i = 1; i <= allMembrSize; i++) {
      root -> yHatAbsolute[allMembrIndx[i]] = root -> yHatAbsoluteCart[allMembrIndx[i]];
    }
  }
  else {
    initializeBetaAndPredicted(treeID, root);
    if (!RF_nativeIsNaN(root -> eRisk)) {
      root -> hcut = hcut;
      root -> outcome = OUTC_LASSO;
      for (i = 1; i <= allMembrSize; i++) {
        root -> yHatAbsolute[allMembrIndx[i]] = root -> yHatAbsoluteLasso[allMembrIndx[i]];
      }
    }
    else {
      free_dvector(root -> beta, 1, root -> augm -> common -> hcutCnt + 1);
      root -> beta = NULL;
      initializeBetaAndPredictedCart(treeID, root);
      root -> hcut = 0;
      root -> outcome = OUTC_CART;
      for (i = 1; i <= allMembrSize; i++) {
        root -> yHatAbsolute[allMembrIndx[i]] = root -> yHatAbsoluteCart[allMembrIndx[i]];
      }
    }
  }
  lotObj = makeLatOptTreeObj(SG_lotLag);
  if ( (hcut >= 1) &&
       !(root -> hcut >= 1) &&
       !(SG_optLocal & ((SG_OPT_SWTCH_TWO) | (SG_OPT_SWTCH_THREE) | (SG_OPT_SWTCH_FOUR) | (SG_OPT_SWTCH_FIVE) | (SG_OPT_SWTCH_SIX))) ) {
    growFlag = FALSE;
  }
  else {
    growFlag = TRUE;
  }
  while (growFlag) {
    greedyMembr = greedyHead -> fwdLink;
    while (greedyMembr != NULL) {
      if ((greedyMembr -> leafFlag == FALSE) && (greedyMembr -> splitInfoDerived == NULL) && (greedyMembr -> splitInfoDerivedCart == NULL)) {
        if (lotObj -> treeSize < (uint) SG_lotSize) {
          splitResult = getBestSplit(treeID, lotObj, greedyMembr);
          if (splitResult) {
          }
          else {
            parent = greedyMembr -> parent;
            parentBase = (NodeBase*) parent;
            parentBase -> splitFlag = FALSE;
            parent -> splitInfoDerived = NULL;
            greedyMembr -> leafFlag = TRUE;
          }
        }
        else {
        }
      }
      else {
      }
      greedyMembr = greedyMembr -> fwdLink;
    }  
    greedyBest = NULL;
    dropBest = RF_nativeNaN;
    if (lotObj -> treeSize < (uint) SG_lotSize) {
      greedyMembr = greedyHead -> fwdLink;
      while (greedyMembr != NULL) {
        if (greedyMembr -> leafFlag == FALSE) {
          if ((greedyMembr -> splitInfoDerived != NULL) || (greedyMembr -> splitInfoDerivedCart != NULL)) {
            if (RF_nativeIsNaN(greedyMembr -> eRiskLoss)) {
              RF_nativeError("\nRF-SRC:  *** ERROR *** ");
              RF_nativeError("\nRF-SRC:  Greedy member has abnormal undefined risk loss:  %10x", greedyMembr);
              RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
              RF_nativeExit();
            }
            if (greedyBest == NULL) {
              greedyBest = greedyMembr;
              dropBest = greedyMembr -> eRiskLoss;
            }
            else {
              dropCurrent = greedyMembr -> eRiskLoss;
              delta = dropCurrent - dropBest;
              if (delta > 0.0) {
                updateFlag = TRUE;
              }
              else if (delta < 0.0) {
                updateFlag = FALSE;
              }
              else {
                if (ran1B(treeID) <= 0.5) {
                  updateFlag = TRUE;
                }
                else {
                  updateFlag = FALSE;
                }
              }
              if (updateFlag) {
                greedyBest = greedyMembr;
                dropBest = dropCurrent;
              }
              else {
              }
            }
          }
          else {
              RF_nativeError("\nRF-SRC:  *** ERROR *** ");
              RF_nativeError("\nRF-SRC:  Greedy member is naked and not a leaf:  %10x", greedyMembr);
              RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
              RF_nativeExit();
          }
        }
        else {
        }
        greedyMembr = greedyMembr -> fwdLink;
      }  
    }  
    if (greedyBest != NULL) {
      result = forkAndUpdate(treeID,
                             greedyBest,
                             &RF_tLeafCount[treeID],
                             RF_nodeMembership[treeID]);
      if (result == TRUE) {
        parent = greedyBest -> parent;
        left   = (Node *)  (((NodeBase *) parent) -> left);
        right  = (Node *)  (((NodeBase *) parent) -> right);
        leftBase   = (NodeBase *) left;
        rightBase  = (NodeBase *) right;
        (lotObj -> treeSize) ++;
        if (RF_opt & OPT_EMPR_RISK) {        
          double parentRisk, leftRisk, rightRisk;
          if (parent -> outcome == OUTC_LASSO) {
            parentRisk = parent -> eRisk;
          }
          else {
            parentRisk = parent -> eRiskCart;
          }
          if (left -> outcome == OUTC_LASSO) {
            leftRisk = left -> eRisk;
          }
          else {
            leftRisk = left -> eRiskCart;
          }
          if (right -> outcome == OUTC_LASSO) {
            rightRisk = right -> eRisk;
          }
          else {
            rightRisk = right -> eRiskCart;
          }
          if (RF_opt & OPT_EMPR_RISK) {          
            SG_emprRSKptr[treeID][lotObj -> treeSize] = SG_emprRSKptr[treeID][(lotObj -> treeSize) - 1] 
              - parentRisk + leftRisk + rightRisk;
          }
          yArray = parent -> augm -> common -> yArray;
          yHatAbsolute = parent -> yHatAbsolute;
          allMembrIndx = leftBase -> allMembrIndx;
          allMembrSize = leftBase -> allMembrSize;
          eRiskOOB = & (left -> eRiskOOB);
          *eRiskOOB = 0.0;
          for (i = 1; i <= allMembrSize; i++) {
            if (RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {
              *eRiskOOB += pow (yArray[allMembrIndx[i]] - yHatAbsolute[allMembrIndx[i]], 2.0);
            }
          }
          allMembrIndx = rightBase -> allMembrIndx;
          allMembrSize = rightBase -> allMembrSize;
          eRiskOOB = & (right -> eRiskOOB);
          *eRiskOOB = 0.0;
          for (i = 1; i <= allMembrSize; i++) {
            if (RF_oobMembershipFlag[treeID][allMembrIndx[i]]) {
              *eRiskOOB += pow (yArray[allMembrIndx[i]] - yHatAbsolute[allMembrIndx[i]], 2.0);
            }
          }
          if (RF_opt & OPT_EMPR_RISK) {
            SG_oobEmprRSKptr[treeID][lotObj -> treeSize] = SG_oobEmprRSKptr[treeID][(lotObj -> treeSize) - 1] 
              - (parent -> eRiskOOB) + (left -> eRiskOOB) + (right -> eRiskOOB);
          }
          insertRisk(treeID, lotObj, SG_oobEmprRSKptr[treeID][lotObj -> treeSize]);
        }
        GreedyObj *greedyNakedLeft  = makeGreedyObj(left,  greedyHead);
        GreedyObj *greedyNakedRight = makeGreedyObj(right,  greedyHead);
        (greedyBest -> bakLink) -> fwdLink = greedyNakedLeft;
        greedyNakedLeft -> bakLink = greedyBest -> bakLink;
        greedyNakedLeft -> fwdLink = greedyNakedRight;
        greedyNakedRight -> bakLink = greedyNakedLeft;
        greedyNakedRight -> fwdLink = greedyBest -> fwdLink;
        if (greedyBest -> fwdLink != NULL) {
          (greedyBest -> fwdLink) -> bakLink = greedyNakedRight;
        }
        if ((greedyBest -> splitInfoDerived != NULL)  || (greedyBest -> splitInfoDerivedCart != NULL)) {
          RF_nativeError("\nRF-SRC:  *** ERROR *** ");
          RF_nativeError("\nRF-SRC:  Error in deletion of best greedy object from list, that represents the split actual node.");
          RF_nativeError("\nRF-SRC:  The split information object is non-null:  %10x %10x", greedyBest -> splitInfoDerived, greedyBest -> splitInfoDerivedCart);
          RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
          RF_nativeExit();          
        }
        freeGreedyObj(greedyBest);
      }
      else {
        RF_nativeError("\nRF-SRC:  *** ERROR *** ");
        RF_nativeError("\nRF-SRC:  forkAndUpdate(%10d) failed.", treeID);
        RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
        RF_nativeExit();
      }
    }  
    else {
      growFlag = FALSE;
    }
    if (lotObj -> strikeout >= SG_lotStrikeout) {
      growFlag = FALSE;
    }
  }  
  greedyMembr = greedyHead -> fwdLink;
  while (greedyMembr != NULL) {
    parent = greedyMembr -> parent;
    parentBase = (NodeBase*) parent;
    RF_leafLinkedObjTail[treeID] = makeAndSpliceLeafLinkedObj(RF_leafLinkedObjTail[treeID]);
    RF_leafLinkedObjTail[treeID] -> nodePtr = parentBase;
    RF_leafLinkedObjTail[treeID] -> termPtr = (TerminalBase *) makeTerminal();
    parentBase -> mate = RF_leafLinkedObjTail[treeID] -> termPtr;
    (RF_leafLinkedObjTail[treeID] -> termPtr) -> mate = parentBase;
    RF_leafLinkedObjTail[treeID] -> nodeID = (RF_leafLinkedObjTail[treeID] -> termPtr) -> nodeID = parentBase -> nodeID;
    for (i = 1; i <= parentBase -> allMembrSize; i++) {
      RF_tTermMembership[treeID][parentBase -> allMembrIndx[i]] = RF_leafLinkedObjTail[treeID] -> termPtr;
    }
    if (RF_optHigh & OPT_MEMB_USER) {
      for (i = 1; i <= parentBase -> allMembrSize; i++) {
        SG_MEMB_ID_ptr[treeID][parentBase -> allMembrIndx[i]] = parentBase -> nodeID;
      }
    }
    initTerminalBase(RF_leafLinkedObjTail[treeID] -> termPtr,
                     0, 
                     0, 
                     0, 
                     RF_rNonFactorCount,   
                     RF_rNonFactorIndex,   
                     RF_rFactorCount,      
                     RF_rFactorIndex,      
                     RF_rFactorSize);      
    calculateAllTerminalNodeOutcomes(RF_GROW, treeID, RF_leafLinkedObjTail[treeID] -> termPtr);
    greedyMembr = greedyMembr -> fwdLink;
  }
  if (RF_opt & OPT_EMPR_RISK) {        
    for (i = 1; i <= lotObj -> treeSize; i++) {
      SG_emprRSKptr[treeID][i] = SG_emprRSKptr[treeID][i] / RF_bootstrapSize;
      if (RF_oobSize[treeID] > 0) {
        SG_oobEmprRSKptr[treeID][i] = SG_oobEmprRSKptr[treeID][i] / RF_oobSize[treeID];
      }
    }
  }
  freeGreedyObjList(greedyHead);
  freeLatOptTreeObj(lotObj);
  return result;
}
char forkAndUpdate (uint treeID, GreedyObj *greedyMembr, uint *leafCount, NodeBase **nodeMembership) {
  Node *parent, *left, *right;
  NodeBase *parentBase, *leftBase, *rightBase;
  uint *allMembrIndx;
  uint  allMembrSize;
  double **xArray;
  uint i, k;
  left = right = NULL;
  parent = greedyMembr -> parent;
  parentBase = (NodeBase*) parent;
  AugmentationObjCommon *augmObjCommon = parent -> augm -> common;
  if (greedyMembr -> bestSplitType == REGR_CDL) {
    left = greedyMembr -> left;
    right = greedyMembr -> right;
    parent -> splitInfoDerived = greedyMembr -> splitInfoDerived;
    if (greedyMembr -> splitInfoDerivedCart != NULL) {
      freeSplitInfoDerived(greedyMembr -> splitInfoDerivedCart, augmObjCommon);
      freeNode(greedyMembr -> leftCart);
      freeNode(greedyMembr -> rightCart);
    }
  }
  else if (greedyMembr -> bestSplitType == REGR_CRT) {
    left = greedyMembr -> leftCart;
    right = greedyMembr -> rightCart;
    parent -> splitInfoDerived = greedyMembr -> splitInfoDerivedCart;
    if (greedyMembr -> splitInfoDerived != NULL) {
      freeSplitInfoDerived(greedyMembr -> splitInfoDerived, augmObjCommon);
      freeNode(greedyMembr -> left);
      freeNode(greedyMembr -> right);
    }
  }
  else {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Greedy member split type not found:  %10x is %10d", greedyMembr, greedyMembr -> bestSplitType);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  leftBase  = (NodeBase*) left;
  rightBase = (NodeBase*) right;
  setParent(leftBase, parentBase);
  setParent(rightBase, parentBase);
  setLeftDaughter(leftBase, parentBase);
  setRightDaughter(rightBase, parentBase);
  leftBase -> splitFlag = rightBase -> splitFlag = TRUE;
  parentBase -> splitFlag = FALSE;
  greedyMembr -> left = greedyMembr -> right = NULL;
  greedyMembr -> leftCart = greedyMembr -> rightCart = NULL;
  greedyMembr -> splitInfoDerived = NULL;
  greedyMembr -> splitInfoDerivedCart = NULL;
  (*leafCount) ++;
  ((parentBase -> left) -> nodeID) = (parentBase -> nodeID);
  ((parentBase -> left) -> depth) = (parentBase -> depth) + 1;
  ((parentBase -> right) -> nodeID) = *leafCount;
  ((parentBase -> right) -> depth) = (parentBase -> depth) + 1;
  allMembrIndx = parentBase -> allMembrIndx;
  allMembrSize = parentBase -> allMembrSize; 
  uint *leftAllMembrIndx, *rghtAllMembrIndx;
  uint  leftAllMembrSize,  rghtAllMembrSize;
  leftAllMembrSize = rghtAllMembrSize = 0;
  leftBase  -> allMembrIndx = leftAllMembrIndx  = uivector(1, allMembrSize);
  rightBase -> allMembrIndx = rghtAllMembrIndx  = uivector(1, allMembrSize);
  leftBase  -> allMembrSizeAlloc = allMembrSize;
  rightBase -> allMembrSizeAlloc = allMembrSize;
  double yStar = parent -> splitInfoDerived -> yStar;
  uint hcutCnt = parent -> splitInfoDerived -> hcutCnt;
  double *beta = parent -> splitInfoDerived -> beta;
  char *indicator = ((SplitInfoMax *) (parent -> splitInfoDerived)) -> indicator;
  double *yHatAbsolute = parent -> yHatAbsolute;
  double predicted;
  char  polarity;
  parent -> splitInfoDerived -> bsf = (++SG_bsfOrder);
  xArray = augmObjCommon -> xArray;
  left -> yHatAbsolute  = yHatAbsolute;
  right -> yHatAbsolute = yHatAbsolute;
  if (left -> outcome == OUTC_LASSO) {
    left -> hcut = parent -> hcut;
    for (i = 1; i <= leftBase -> repMembrSize; i++) {
      yHatAbsolute[leftBase -> repMembrIndx[i]] = left -> yHatAbsoluteLasso[leftBase -> repMembrIndx[i]];
    }
  }
  else {
    left -> hcut = 0;
    for (i = 1; i <= leftBase -> repMembrSize; i++) {
      yHatAbsolute[leftBase -> repMembrIndx[i]] = left -> yHatAbsoluteCart[leftBase -> repMembrIndx[i]];
    }
  }
  if (right -> outcome == OUTC_LASSO) {
    right -> hcut = parent -> hcut;
    for (i = 1; i <= rightBase -> repMembrSize; i++) {
      yHatAbsolute[rightBase -> repMembrIndx[i]] = right -> yHatAbsoluteLasso[rightBase -> repMembrIndx[i]];
    }
  }
  else {
    right -> hcut = 0;
    for (i = 1; i <= rightBase -> repMembrSize; i++) {
      yHatAbsolute[rightBase -> repMembrIndx[i]] = right -> yHatAbsoluteCart[rightBase -> repMembrIndx[i]];
    }
  }        
  for (i = 1; i <= allMembrSize; i++) {
    if (indicator[allMembrIndx[i]] == NEITHER) {
      if (greedyMembr -> bestSplitType == REGR_CRT) {
        predicted = beta[1] + xArray[((SplitInfoMax *) (parent -> splitInfoDerived)) -> splitParameter][allMembrIndx[i]];
      }
      else {
        predicted = beta[1];
        for (k = 1; k <= hcutCnt; k++) {
          predicted = predicted + (beta[k+1] * xArray[k][allMembrIndx[i]]);
        }
      }
      if (predicted <= yStar) {
        polarity = LEFT;
        leftAllMembrIndx[++leftAllMembrSize] = allMembrIndx[i];
        nodeMembership[allMembrIndx[i]] = leftBase;
      }
      else {
        polarity = RIGHT;
        rghtAllMembrIndx[++rghtAllMembrSize] = allMembrIndx[i];
        nodeMembership[allMembrIndx[i]] = rightBase;
      }
      if (polarity == LEFT) {
        if (left -> outcome == OUTC_LASSO) {
          predicted = left -> beta[1];
          for (k = 1; k <= hcutCnt; k++) {
            predicted = predicted + ((left -> beta[k+1]) * xArray[k][allMembrIndx[i]]);
          }
        }
        else {
          predicted = left -> mean;
        }
      }
      else {
        if (right -> outcome == OUTC_LASSO) {
          predicted = right -> beta[1];
          for (k = 1; k <= hcutCnt; k++) {
            predicted = predicted + ((right -> beta[k+1]) * xArray[k][allMembrIndx[i]]);
          }
        }
        else {
          predicted = right -> mean;
        }
      }
      yHatAbsolute[allMembrIndx[i]] = predicted;
    }  
    else {
      if (indicator[allMembrIndx[i]] == LEFT) {
        leftAllMembrIndx[++leftAllMembrSize] = allMembrIndx[i];
        nodeMembership[allMembrIndx[i]] = leftBase;
      }
      else {
        rghtAllMembrIndx[++rghtAllMembrSize] = allMembrIndx[i];
        nodeMembership[allMembrIndx[i]] = rightBase;
      }
    }
  }  
  leftBase  -> allMembrSize = leftAllMembrSize;
  rightBase -> allMembrSize = rghtAllMembrSize;
  left  -> yHatAbsolute = parent -> yHatAbsolute;
  right -> yHatAbsolute = parent -> yHatAbsolute;
  if (greedyMembr -> bestSplitType == REGR_CDL) {
    free_dvector(left -> yHatAbsoluteLasso,  1, augmObjCommon -> nSize);
    left -> yHatAbsoluteLasso = NULL;
    free_dvector(right -> yHatAbsoluteLasso, 1, augmObjCommon -> nSize);
    right -> yHatAbsoluteLasso = NULL;
  }
  free_dvector(left -> yHatAbsoluteCart,  1, augmObjCommon -> nSize);
  left -> yHatAbsoluteCart = NULL;
  free_dvector(right -> yHatAbsoluteCart, 1, augmObjCommon -> nSize);
  right -> yHatAbsoluteCart = NULL;
  if ((leftBase -> repMembrSize == 0) || (rightBase -> repMembrSize == 0)) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Left or Right Daughter of size zero:  (%10d, %10d)", leftBase -> repMembrSize, rightBase -> repMembrSize);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  return TRUE;
}
void saveTree(uint treeID, NodeBase *parent, ulong *offset, ulong *offsetCT, ulong *offsetID_rmbr, ulong *offsetID_ombr) {
  double *beta;
  uint hcutCnt;
  uint jj;
  (*offset) ++;
  parent -> brnodeID = *offset;
  SG_prnodeID_ptr[treeID][*offset] = parent -> pnodeID;
  SG_treeID_ptr[treeID][*offset] = treeID;
  SG_nodeID_ptr[treeID][*offset] = parent -> nodeID;
  SG_nodeSZ_ptr[treeID][*offset] = parent -> repMembrSize;
  SG_nodeStat_ptr[treeID][*offset] = ((Node *) parent) -> eRiskOOB;
  hcutCnt = SG_augmObjCommon -> hcutCnt;
  if (((Node *) parent) -> splitInfoDerived == NULL) {
    beta = ((Node *) parent) -> beta;
    if (((Node *) parent) -> hcut >= 1) {
      SG_betaZ_ptr[treeID][*offset] = beta[1];
      for (jj = 1; jj <= hcutCnt; jj++) {
        SG_betaP_ptr[treeID][jj][*offset] = beta[1 + jj];
      }
      SG_yBar_ptr[treeID][*offset] = RF_nativeNaN;
    }
    else {
      if ((*offset) == 1) {
      }
      else {
        SG_betaZ_ptr[treeID][*offset] = ((Node *) (parent -> parent)) -> splitInfoDerived -> beta[1];
        for (jj = 1; jj <= hcutCnt; jj++) {
          SG_betaP_ptr[treeID][jj][*offset] = ((Node *) (parent -> parent)) -> splitInfoDerived -> beta[1 + jj];
        }
      }
      SG_yBar_ptr[treeID][*offset] = ((Node *) parent) -> mean;      
    }
    SG_yStar_ptr[treeID][*offset] = RF_nativeNaN;
    SG_bsf_ptr[treeID][*offset] = 0;
  }
  else {
    double *beta = ((Node *) parent) -> splitInfoDerived -> beta;
    if (((Node *) parent) -> hcut >= 1) {
      SG_yBar_ptr[treeID][*offset]  = RF_nativeNaN;
      SG_yStar_ptr[treeID][*offset] = ((Node *) parent) -> splitInfoDerived -> yStar;    
    }
    else {
      SG_yBar_ptr[treeID][*offset]  = ((Node *) parent) -> splitInfoDerived -> yBar;
      SG_yStar_ptr[treeID][*offset] = RF_nativeNaN;
    }
    SG_betaZ_ptr[treeID][*offset] = beta[1];
    for (jj = 1; jj <= hcutCnt; jj++) {    
      SG_betaP_ptr[treeID][jj][*offset] = beta[1 + jj];
    }
    SG_bsf_ptr[treeID][*offset] = ((Node *) parent) -> splitInfoDerived -> bsf;
  }
  if (((parent -> left) != NULL) && ((parent -> right) != NULL)) {
    parent -> left -> pnodeID = *offset;
    parent -> right -> pnodeID = *offset;
    saveTree(treeID, parent ->  left, offset, offsetCT, offsetID_rmbr, offsetID_ombr);
    saveTree(treeID, parent -> right, offset, offsetCT, offsetID_rmbr, offsetID_ombr);
    SG_brnodeID_ptr[treeID][parent -> brnodeID] = (parent -> right) -> brnodeID;
  }
  else {
    SG_brnodeID_ptr[treeID][parent -> brnodeID] = 0;
    (*offsetCT) ++;
    SG_rmbrTNodeCT_ptr[treeID][*offsetCT] = ((Terminal *) (parent -> mate)) -> ibgMembrCount;
    SG_ombrTNodeCT_ptr[treeID][*offsetCT] = ((Terminal *) (parent -> mate)) -> oobMembrCount;
    for (jj = 1; jj <= ((Terminal *) (parent -> mate)) -> ibgMembrCount; jj++) {
      (*offsetID_rmbr) ++;
      SG_rmbrTNodeID_ptr[treeID][*offsetID_rmbr] = parent -> repMembrIndx[jj];
    }
    for (jj = 1; jj <= ((Terminal *) (parent -> mate)) -> oobMembrCount; jj++) {
      (*offsetID_ombr) ++;
      SG_ombrTNodeID_ptr[treeID][*offsetID_ombr] = ((Terminal *) (parent -> mate)) -> oobMembrIndx[jj];
    }
  }
}
void restoreTreeLOT(uint treeID, Node *parent, ulong *offsetTree) {
  NodeBase *parentBase;
  ulong *offset;
  double *beta;
  uint hcutCnt;
  uint jj;
  offset = & offsetTree[treeID];
  if (treeID != SG_treeID_[*offset]) {
    RF_nativeError("\nRF-SRC:  Diagnostic Trace of Tree Record:  \n");
    RF_nativeError("\nRF-SRC:      treeID     nodeID ");
    RF_nativeError("\nRF-SRC:  %10d %10d \n", SG_treeID_[*offset], SG_nodeID_[*offset]);
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Invalid forest input record in tree:  %10d", treeID);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  parentBase = (NodeBase *) parent;
  if (parentBase -> parent != NULL) {
    parentBase -> depth = (parentBase -> parent) -> depth + 1;
  }
  parentBase -> left  = NULL;
  parentBase -> right = NULL;
  parentBase -> splitFlag = FALSE;
  parentBase -> nodeID = SG_nodeID_[*offset];
  parentBase -> repMembrSize = SG_nodeSZ_[*offset];
  AugmentationObjCommon *augmObjCommon = SG_augmObjCommon;
  hcutCnt = augmObjCommon -> hcutCnt;    
  if (SG_brnodeID_[*offset] != 0) {
    parent -> splitInfoDerived = makeSplitInfoDerived(augmObjCommon);
    parent -> splitInfoDerived -> hcutCnt = hcutCnt;
    beta = parent -> splitInfoDerived -> beta = dvector(1, hcutCnt + 1);
    beta[1] = SG_betaZ_[*offset];
    for (jj = 1; jj <= hcutCnt; jj++) {
      beta[jj + 1] = SG_betaP_in[*offset][jj];
    }
    if (RF_nativeIsNaN(SG_yStar_[*offset]) && (!RF_nativeIsNaN(SG_yBar_[*offset]))) {
      parent -> splitInfoDerived -> hcut = 0;
      parent -> splitInfoDerived -> yStar = 0.0;
      parent -> hcut = parent -> splitInfoDerived -> hcut;
    }
    else if ((!RF_nativeIsNaN(SG_yStar_[*offset])) && RF_nativeIsNaN(SG_yBar_[*offset])) {
      parent -> splitInfoDerived -> hcut = augmObjCommon -> hcut;
      parent -> splitInfoDerived -> yStar = SG_yStar_[*offset];
      parent -> hcut = parent -> splitInfoDerived -> hcut;
    }
    else {
      RF_nativeError("\nRF-SRC:  *** ERROR *** ");
      RF_nativeError("\nRF-SRC:  Internal node is neither a cart or lasso node:  %10x %10d", parent, parentBase -> nodeID);
      RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
      RF_nativeExit();
    }
  }
  else {
    parent -> splitInfoDerived = NULL;
    parent -> mean = SG_yBar_[*offset];
    if (RF_nativeIsNaN(parent -> mean)) {
      parent -> hcut = augmObjCommon -> hcut;
      beta = parent -> beta = dvector(1, hcutCnt + 1);
      beta[1] = SG_betaZ_[*offset];
      for (jj = 1; jj <= hcutCnt; jj++) {
        beta[jj + 1] = SG_betaP_in[*offset][jj];
      }
    }
    else {
      parent -> hcut = 0;
      parent -> beta = NULL;
    }
    RF_leafLinkedObjTail[treeID] = makeAndSpliceLeafLinkedObj(RF_leafLinkedObjTail[treeID]);
    RF_leafLinkedObjTail[treeID] -> nodePtr = parent;
    Terminal *termPtr = makeTerminal();
    TerminalBase *termBasePtr = (TerminalBase *) termPtr;
    RF_leafLinkedObjTail[treeID] -> termPtr = termBasePtr;
    initTerminalBase(termBasePtr,
                     0, 
                     0, 
                     0, 
                     RF_rNonFactorCount,   
                     RF_rNonFactorIndex,   
                     RF_rFactorCount,      
                     RF_rFactorIndex,      
                     RF_rFactorSize);      
    termPtr -> oobMembrCount = SG_ombrTNodeCT_[SG_offsetCT[treeID]];
    termPtr -> ibgMembrCount = SG_rmbrTNodeCT_[SG_offsetCT[treeID]];
    SG_offsetCT[treeID] ++;
    termPtr -> ibgMembrIndx = SG_rmbrTNodeID_ + SG_offsetID_rmbr[treeID];
    SG_offsetID_rmbr[treeID] += termPtr -> ibgMembrCount;
    termPtr -> oobMembrIndx = SG_ombrTNodeID_ + SG_offsetID_ombr[treeID];
    SG_offsetID_ombr[treeID] += termPtr -> oobMembrCount;
    parentBase -> mate = termBasePtr;
    termBasePtr -> mate = parentBase;
    RF_leafLinkedObjTail[treeID] -> nodeID = (RF_leafLinkedObjTail[treeID] -> termPtr) -> nodeID = parentBase -> nodeID;
    restoreTerminalNodeOutcomesCDL(treeID, (Terminal *) (RF_leafLinkedObjTail[treeID] -> termPtr));
  }
  (*offset) ++;
  if (parent -> splitInfoDerived != NULL) {
    Node *left = makeNode(augmObjCommon -> pSize);
    left  -> nSize = augmObjCommon -> nSize;
    NodeBase *leftBase   = (NodeBase *) left;
    setParent(leftBase, parentBase);
    setLeftDaughter(leftBase, parentBase);
    left  -> yHatAbsolute = parent -> yHatAbsolute;
    left -> augm = getAugmentationObjGeneric(SG_augmObjCommon, treeID, left);
    restoreTreeLOT(treeID, left, offsetTree);
    Node *right = makeNode(augmObjCommon -> pSize);
    right -> nSize = augmObjCommon -> nSize;
    NodeBase *rightBase   = (NodeBase *) right;
    setParent(rightBase, parentBase);
    setRightDaughter(rightBase, parentBase);
    right -> yHatAbsolute = parent -> yHatAbsolute;
    right -> augm = getAugmentationObjGeneric(SG_augmObjCommon, treeID, right);
    restoreTreeLOT(treeID, right, offsetTree);
  }
}
void getTestMembershipLOT(uint treeID, Node *parent) {
  NodeBase *parentBase;
  NodeBase *leftBase;
  NodeBase *rightBase;
  Node *left;
  Node *right;
  double **fxArray;
  double *beta;
  double yStar;
  double predicted;
  uint hcutCnt;
  uint testMembrSize, leftTestMembrSize, rghtTestMembrSize;
  uint *testMembrIndx, *leftTestMembrIndx, *rghtTestMembrIndx;
  uint i, k;
  parentBase = (NodeBase *) parent;
  testMembrIndx = parent -> testMembrIndx;
  testMembrSize = parent -> testMembrSize;
  hcutCnt = parent -> augm -> common -> hcutCnt;
  fxArray = parent -> augm -> common -> fxArray;
  leftTestMembrSize = rghtTestMembrSize = 0;
  if (parent -> splitInfoDerived != NULL) {
    leftBase  = parentBase -> left;
    rightBase = parentBase -> right;
    left  = (Node *) leftBase;
    right = (Node *) rightBase;
    beta = parent -> splitInfoDerived -> beta;
    yStar = parent -> splitInfoDerived -> yStar;
    left  -> testMembrIndx = leftTestMembrIndx   = uivector(1, testMembrSize);
    right -> testMembrIndx = rghtTestMembrIndx  = uivector(1, testMembrSize);
    left  -> testMembrSizeAlloc = testMembrSize;
    right -> testMembrSizeAlloc = testMembrSize;
    if (parent -> hcut == 0) {
      for (i = 1; i <= testMembrSize; i++) {
        predicted = beta[1];
        for (k = 1; k <= parentBase -> xSize; k++) {
          predicted = predicted + (beta[k+1] * fxArray[k][testMembrIndx[i]]);
        }
        if (predicted <= yStar) {
          leftTestMembrIndx[++leftTestMembrSize] = testMembrIndx[i];
        }
        else {
          rghtTestMembrIndx[++rghtTestMembrSize] = testMembrIndx[i];
        }
      }
    }
    else {
      for (i = 1; i <= testMembrSize; i++) {
        predicted = beta[1];
        for (k = 1; k <= hcutCnt; k++) {
          predicted = predicted + (beta[k+1] * fxArray[k][testMembrIndx[i]]);
        }
        if (predicted <= yStar) {
          leftTestMembrIndx[++leftTestMembrSize] = testMembrIndx[i];
        }
        else {
          rghtTestMembrIndx[++rghtTestMembrSize] = testMembrIndx[i];
        }
      }
    }
    left -> testMembrSize = leftTestMembrSize;
    right -> testMembrSize = rghtTestMembrSize;
  }
  else {
    beta = parent -> beta;
    if (parent -> hcut == 0) {
      predicted = parent -> mean;
      for (i = 1; i <= testMembrSize; i++) {
        SG_yHatAbsoluteTest[treeID][testMembrIndx[i]] = predicted;
      }
    }
    else {
      for (i = 1; i <= testMembrSize; i++) {
        predicted = beta[1];
        for (k = 1; k <= hcutCnt; k++) {
          predicted = predicted + (beta[k+1] * fxArray[k][testMembrIndx[i]]);
        }
        SG_yHatAbsoluteTest[treeID][testMembrIndx[i]] = predicted;
      }
    }
  }
  if (parent -> splitInfoDerived != NULL) {
    if (leftTestMembrSize > 0) {
      getTestMembershipLOT(treeID, (Node*) (((NodeBase*) parent) -> left));
    }
    if (rghtTestMembrSize > 0) {
      getTestMembershipLOT(treeID, (Node*) (((NodeBase*) parent) -> right));
    }
  }
}
