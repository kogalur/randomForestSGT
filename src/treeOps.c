
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
#include "cdlRegr.h"
#include "stackOutputQQ.h"
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
static void checkRestoreMembership(uint treeID, uint nSize, uint bootMembrSize, uint *bootMembrIndx);
static char restorePredictReplicates(uint treeID, NodeBase *root, uint nSize, uint bootMembrSize);
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
      for (i = 1; i <= nSize; i++) {
        RF_nodeMembership[treeID][i] = rootBase;
      }
      rootBase -> repMembrIndx = bootMembrIndx;
      rootBase -> repMembrSize = rootBase -> repMembrSizeAlloc = bootMembrSize;
      SG_yHatAbsolute[treeID] = dvector(1, nSize);
      root -> yHatAbsolute = SG_yHatAbsolute[treeID];
      if (RF_oobSize[treeID] > 0) {
        rootBase -> oobMembrSizeAlloc = rootBase -> oobMembrSize = RF_oobSize[treeID];
        rootBase -> oobMembrIndx = uivector(1, rootBase -> oobMembrSizeAlloc);
        for (i = 1; i <= RF_oobSize[treeID]; i++) {
          root -> yHatAbsolute[RF_oobMembershipIndex[treeID][i]] = RF_nativeNaN;
          rootBase -> oobMembrIndx[i] = RF_oobMembershipIndex[treeID][i];
        }
      }
      growTreeLOT(treeID, root);
    }
    else {
      rootBase -> repMembrIndx = bootMembrIndx;
      rootBase -> repMembrSize = rootBase -> repMembrSizeAlloc = bootMembrSize;
      SG_yHatAbsolute[treeID] = dvector(1, nSize);
      root -> yHatAbsolute = SG_yHatAbsolute[treeID];
      root -> augm = getAugmentationObjGeneric(SG_augmObjCommon, treeID, root);
      restoreTreeLOT(treeID, root, SG_offsetTree);
      if (mode == RF_REST) {
        LeafLinkedObj *leaf = RF_leafLinkedObjHead[treeID] -> fwdLink;
        while (leaf != NULL) {
          Terminal *term = (Terminal *) leaf -> termPtr;
          for (i = 1; i <= term -> repMembrCount; i++) {
            uint observationID = term -> repMembrIndx[i];
            RF_tTermMembership[treeID][observationID] = leaf -> termPtr;
            if (RF_optHigh & OPT_MEMB_USER) {
              SG_MEMB_ID_ptr[treeID][observationID] = leaf -> nodeID;
            }
          }
          for (i = 1; i <= term -> oobMembrCount; i++) {
            uint observationID = term -> oobMembrIndx[i];
            RF_tTermMembership[treeID][observationID] = leaf -> termPtr;
            if (RF_optHigh & OPT_MEMB_USER) {
              SG_MEMB_ID_ptr[treeID][observationID] = leaf -> nodeID;
            }
          }
          leaf = leaf -> fwdLink;
        }
      }
      if (mode == RF_PRED) {
        SG_yHatAbsoluteTest[treeID] = dvector(1, nSizeTest);
        root -> tstMembrSize = root -> tstMembrSizeAlloc = RF_fobservationSize;
        root -> tstMembrIndx = RF_fidentityMembershipIndex;
        if ((SG_optLocal & SG_OPT_SWTCH_NINE) &&
            !restorePredictReplicates(treeID, rootBase, nSize, bootMembrSize)) {
          RF_nativeError("\nRF-SGT:  *** ERROR *** ");
          RF_nativeError("\nRF-SGT: Unable to reconstruct centered prediction memberships for tree %u.", treeID);
          RF_nativeExit();
        }
        getTestMembershipLOT(treeID, root);
        if (RF_optHigh & OPT_MEMB_USER) {
          LeafLinkedObj *leaf = RF_leafLinkedObjHead[treeID] -> fwdLink;
          while (leaf != NULL) {
            Terminal *term = (Terminal *) leaf -> termPtr;
            for (i = 1; i <= term -> tstMembrCount; i++) {
              SG_MEMB_ID_ptr[treeID][term -> tstMembrIndx[i]] = leaf -> nodeID;
            }
            leaf = leaf -> fwdLink;
          }
        }
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
      stackTNQualitativeObjectsTreePtr(mode, treeID);
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
        root -> tstMembrIndx = NULL;
        root -> tstMembrSize = root -> tstMembrSizeAlloc = 0;
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
  Node     *right;
  uint *allMembrIndx, *oobMembrIndx;
  uint  allMembrSize,  oobMembrSize;
  double *yHatAbsolute;
  char  splitResult;
  char  result;
  GreedyObj *greedyHead, *greedyMembr;
  GreedyObj *greedyBest;
  double dropBest, dropCurrent;
  LatOptTreeObj *lotObj;
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
  lotObj = makeLatOptTreeObj(SG_lotLag, SG_lotStrikeout);
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
        parentBase = (NodeBase*) parent;
        left   = (Node *)  (((NodeBase *) parent) -> left);
        right  = (Node *)  (((NodeBase *) parent) -> right);
        (lotObj -> treeSize) ++;
        if (FALSE) {
          if ( ((NodeBase *) parent) -> depth == 0 ) {
            if (RF_opt & OPT_EMPR_RISK) {
              SG_emprRSKptr[treeID][(lotObj -> treeSize) - 1] = parent -> eRiskCart;            
            }
            parent -> outcome = OUTC_CART;                   
            left -> hcut = 0;
            right -> hcut = 0;
            left -> outcome = OUTC_CART;
            right -> outcome = OUTC_CART;
          }
        }
        if (RF_opt & OPT_EMPR_RISK) {
          double parentRisk, leftRisk, rightRisk;
          double parentRiskOOB, leftRiskOOB, rightRiskOOB;
          if (parent -> outcome == OUTC_LASSO) {
            parentRisk = parent -> eRisk;
            parentRiskOOB = parent -> eRiskOOB;
          }
          else {
            parentRisk = parent -> eRiskCart;
            parentRiskOOB = parent -> eRiskCartOOB;
          }
          if (left -> outcome == OUTC_LASSO) {
            leftRisk = left -> eRisk;
            leftRiskOOB = left -> eRiskOOB;
          }
          else {
            leftRisk = left -> eRiskCart;
            leftRiskOOB = left -> eRiskCartOOB;
          }
          if (right -> outcome == OUTC_LASSO) {
            rightRisk = right -> eRisk;
            rightRiskOOB = right -> eRiskOOB;
          }
          else {
            rightRisk = right -> eRiskCart;
            rightRiskOOB = right -> eRiskCartOOB;
          }
          SG_emprRSKptr[treeID][lotObj -> treeSize] = SG_emprRSKptr[treeID][(lotObj -> treeSize) - 1] 
            - parentRisk + leftRisk + rightRisk;
          SG_oobEmprRSKptr[treeID][lotObj -> treeSize] = SG_oobEmprRSKptr[treeID][(lotObj -> treeSize) - 1] 
            - parentRiskOOB + leftRiskOOB + rightRiskOOB;
          SG_cart_emprRSKptr[treeID][lotObj -> treeSize] = SG_cart_emprRSKptr[treeID][(lotObj -> treeSize) - 1] 
            - (parent -> eRiskCart) + (left -> eRiskCart) + (right -> eRiskCart);
          SG_cart_oobEmprRSKptr[treeID][lotObj -> treeSize] = SG_cart_oobEmprRSKptr[treeID][(lotObj -> treeSize) - 1] 
            - (parent -> eRiskCartOOB) + (left -> eRiskCartOOB) + (right -> eRiskCartOOB);
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
    if (lotObj -> lag > 0) {
      if (lotObj -> strikeout >= SG_lotStrikeout) {
        growFlag = FALSE;
      }
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
                     0, 
                     RF_rNonFactorCount,   
                     RF_rNonFactorIndex,   
                     RF_rFactorCount,      
                     RF_rFactorIndex,      
                     RF_rFactorSize);      
    if (parentBase -> depth == 0) {
      yHatAbsolute = parent -> yHatAbsolute;
      oobMembrSize = parentBase -> oobMembrSize;
      oobMembrIndx = parentBase -> oobMembrIndx;
      if ((root -> hcut > 0) &&
          !(SG_optLocal & SG_OPT_SWTCH_NINE)) {
        double *beta = parent -> beta;
        double **xArray = parent -> augm -> common -> xArray;
        uint  hcutCnt = parent -> augm -> common -> hcutCnt;    
        double predicted;
        uint jj;
        for (i = 1; i <= oobMembrSize; i++) {
          predicted = beta[1];
          for (jj = 1; jj <= hcutCnt; jj++) {
            predicted = predicted + (beta[jj+1] * xArray[jj][oobMembrIndx[i]]);
          }
          yHatAbsolute[oobMembrIndx[i]] = predicted;
        }
      }
      else if (root -> hcut == 0) {
        for (i = 1; i <= oobMembrSize; i++) {
          yHatAbsolute[oobMembrIndx[i]] = parent-> mean;
        }
      }
    }
    calculateAllTerminalNodeOutcomes(RF_GROW, treeID, RF_leafLinkedObjTail[treeID] -> termPtr);
    greedyMembr = greedyMembr -> fwdLink;
  }
  if (RF_opt & OPT_EMPR_RISK) {        
    for (i = 1; i <= lotObj -> treeSize; i++) {
      SG_emprRSKptr[treeID][i] = SG_emprRSKptr[treeID][i] / RF_bootstrapSize;
      if (RF_oobSize[treeID] > 0) {
        SG_oobEmprRSKptr[treeID][i] = SG_oobEmprRSKptr[treeID][i] / RF_oobSize[treeID];
      }
      SG_cart_emprRSKptr[treeID][i] = SG_cart_emprRSKptr[treeID][i] / RF_bootstrapSize;
      if (RF_oobSize[treeID] > 0) {
        SG_cart_oobEmprRSKptr[treeID][i] = SG_cart_oobEmprRSKptr[treeID][i] / RF_oobSize[treeID];
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
      else if (SG_optLocal & SG_OPT_SWTCH_NINE) {
        predicted = yHatAbsolute[allMembrIndx[i]];
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
          if (SG_optLocal & SG_OPT_SWTCH_NINE) {
            predicted = left -> yHatAbsoluteLasso[allMembrIndx[i]];
          }
          else {
            predicted = left -> beta[1];
            for (k = 1; k <= hcutCnt; k++) {
              predicted = predicted + ((left -> beta[k+1]) * xArray[k][allMembrIndx[i]]);
            }
          }
        }
        else {
          predicted = left -> mean;
        }
      }
      else {
        if (right -> outcome == OUTC_LASSO) {
          if (SG_optLocal & SG_OPT_SWTCH_NINE) {
            predicted = right -> yHatAbsoluteLasso[allMembrIndx[i]];
          }
          else {
            predicted = right -> beta[1];
            for (k = 1; k <= hcutCnt; k++) {
              predicted = predicted + ((right -> beta[k+1]) * xArray[k][allMembrIndx[i]]);
            }
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
    if (RF_nativeIsNaN(((Node *) parent) -> splitInfoDerived -> yBar)) {
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
    SG_rmbrTNodeCT_ptr[treeID][*offsetCT] = parent -> repMembrSize;
    SG_ombrTNodeCT_ptr[treeID][*offsetCT] = ((Terminal *) (parent -> mate)) -> oobMembrCount;
    for (jj = 1; jj <= parent -> repMembrSize; jj++) {
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
                     0, 
                     RF_rNonFactorCount,   
                     RF_rNonFactorIndex,   
                     RF_rFactorCount,      
                     RF_rFactorIndex,      
                     RF_rFactorSize);      
    termPtr -> oobMembrCount = SG_ombrTNodeCT_[SG_offsetCT[treeID]];
    termPtr -> repMembrCount = SG_rmbrTNodeCT_[SG_offsetCT[treeID]];
    SG_offsetCT[treeID] ++;
    termPtr -> repMembrIndx = SG_rmbrTNodeID_ + SG_offsetID_rmbr[treeID];
    SG_offsetID_rmbr[treeID] += termPtr -> repMembrCount;
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
static char restorePredictReplicates(uint treeID, NodeBase *root, uint nSize, uint bootMembrSize) {
  NodeBase *node;
  LeafLinkedObj *leaf;
  Terminal *term;
  uint i, observationID;
  NodeBase **membership = RF_nodeMembership[treeID];
  for (i = 1; i <= nSize; i++) {
    membership[i] = NULL;
  }
  for (leaf = RF_leafLinkedObjHead[treeID] -> fwdLink;
       leaf != NULL; leaf = leaf -> fwdLink) {
    node = (NodeBase *) leaf -> nodePtr;
    term = (Terminal *) leaf -> termPtr;
    for (i = 1; i <= term -> repMembrCount; i++) {
      observationID = term -> repMembrIndx[i];
      membership[observationID] = node;
    }
    for (; node != root; node = node -> parent) {
      if (node -> repMembrIndx == NULL) {      
        node -> repMembrSizeAlloc = node -> repMembrSize;
        node -> repMembrIndx = uivector(1, node -> repMembrSizeAlloc);
        node -> repMembrSize = 0;
      }
    }
  }
  for (i = 1; i <= bootMembrSize; i++) {
    observationID = root -> repMembrIndx[i];
    for (node = membership[observationID]; node != root;
         node = node -> parent) {
      node -> repMembrIndx[++node -> repMembrSize] = observationID;
    }
  }
  for (leaf = RF_leafLinkedObjHead[treeID] -> fwdLink;
       leaf != NULL; leaf = leaf -> fwdLink) {
    node = (NodeBase *) leaf -> nodePtr;
    term = (Terminal *) leaf -> termPtr;
    if (node -> repMembrSize != term -> repMembrCount) {
      return FALSE;
    }
    for (i = 1; i <= term -> repMembrCount; i++) {
      if (node -> repMembrIndx[i] != term -> repMembrIndx[i]) {
        return FALSE;
      }
    }
    for (; node != root; node = node -> parent) {
      if (node -> repMembrSize != node -> repMembrSizeAlloc) {
        return FALSE;
      }
    }
  }
  return TRUE;
}
void getTestMembershipLOT(uint treeID, Node *parent) {
  NodeBase *parentBase;
  NodeBase *leftBase;
  NodeBase *rightBase;
  Node *left;
  Node *right;
  Terminal *termPtr;
  TerminalBase *termBasePtr;
  double **fxArray;
  double *beta;
  double *nodeCenter = NULL;
  double yStar;
  double predicted;
  uint hcutCnt;
  uint tstMembrSize, leftTestMembrSize, rghtTestMembrSize;
  uint *tstMembrIndx, *leftTestMembrIndx, *rghtTestMembrIndx;
  uint i, k;
  parentBase = (NodeBase *) parent;
  tstMembrIndx = parent -> tstMembrIndx;
  tstMembrSize = parent -> tstMembrSize;
  hcutCnt = parent -> augm -> common -> hcutCnt;
  fxArray = parent -> augm -> common -> fxArray;
  if ((SG_optLocal & SG_OPT_SWTCH_NINE) && parent -> hcut > 0 &&
      (parent -> splitInfoDerived != NULL ||
       !(SG_optLocal & SG_OPT_NODE_RESTRICT))) {
    if (SG_fbasisSource == NULL || parentBase -> repMembrSize == 0 ||
        parentBase -> repMembrIndx == NULL) {
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: Missing centered prediction inputs for tree %u, node %u.",
                     treeID, parentBase -> nodeID);
      RF_nativeExit();
    }
    nodeCenter = dvector(1, SG_basisSourceSize);
    if (!computeNodeCenter(parentBase -> repMembrSize, parentBase -> repMembrIndx,
                           SG_weight, nodeCenter)) {
      free_dvector(nodeCenter, 1, SG_basisSourceSize);
      RF_nativeError("\nRF-SGT:  *** ERROR *** ");
      RF_nativeError("\nRF-SGT: Unable to reconstruct center for tree %u, node %u.",
                     treeID, parentBase -> nodeID);
      RF_nativeExit();
    }
  }
  leftTestMembrSize = rghtTestMembrSize = 0;
  if (parent -> splitInfoDerived != NULL) {
    leftBase  = parentBase -> left;
    rightBase = parentBase -> right;
    left  = (Node *) leftBase;
    right = (Node *) rightBase;
    beta = parent -> splitInfoDerived -> beta;
    yStar = parent -> splitInfoDerived -> yStar;
    left  -> tstMembrIndx = leftTestMembrIndx   = uivector(1, tstMembrSize);
    right -> tstMembrIndx = rghtTestMembrIndx  = uivector(1, tstMembrSize);
    left  -> tstMembrSizeAlloc = tstMembrSize;
    right -> tstMembrSizeAlloc = tstMembrSize;
    if (parent -> hcut == 0) {
      for (i = 1; i <= tstMembrSize; i++) {
        predicted = beta[1];
        for (k = 1; k <= parentBase -> xSize; k++) {
          predicted = predicted + (beta[k+1] * fxArray[k][tstMembrIndx[i]]);
        }
        if (predicted <= yStar) {
          leftTestMembrIndx[++leftTestMembrSize] = tstMembrIndx[i];
        }
        else {
          rghtTestMembrIndx[++rghtTestMembrSize] = tstMembrIndx[i];
        }
      }
    }
    else {
      for (i = 1; i <= tstMembrSize; i++) {
        predicted = beta[1];
        for (k = 1; k <= hcutCnt; k++) {
          if (nodeCenter == NULL) {
            predicted = predicted + (beta[k+1] * fxArray[k][tstMembrIndx[i]]);
          }
          else if (fabs(beta[k+1]) > 0.0) {
            predicted = predicted + (beta[k+1] *
              evaluateNodeBasisTerm(SG_fbasisSource, nodeCenter, tstMembrIndx[i], k));
          }
        }
        if (predicted <= yStar) {
          leftTestMembrIndx[++leftTestMembrSize] = tstMembrIndx[i];
        }
        else {
          rghtTestMembrIndx[++rghtTestMembrSize] = tstMembrIndx[i];
        }
      }
    }
    left -> tstMembrSize = leftTestMembrSize;
    right -> tstMembrSize = rghtTestMembrSize;
  }
  else {
    beta = parent -> beta;
    if (parent -> hcut == 0) {
      predicted = parent -> mean;
      for (i = 1; i <= tstMembrSize; i++) {
        SG_yHatAbsoluteTest[treeID][tstMembrIndx[i]] = predicted;
      }
    }
    else if (SG_optLocal & SG_OPT_NODE_RESTRICT) {
      termPtr = (Terminal *) parentBase -> mate;
      if (SG_fbasisSource == NULL || termPtr -> repMembrCount == 0 ||
          termPtr -> repMembrIndx == NULL ||
          parent -> augm -> common -> yArray == NULL) {
        RF_nativeError("\nRF-SGT:  *** ERROR *** ");
        RF_nativeError("\nRF-SGT: Missing restricted prediction inputs for tree %u, node %u.",
                       treeID, parentBase -> nodeID);
        RF_nativeExit();
      }
      NodeSupport support;
      support.lower = dvector(1, SG_basisSourceSize);
      support.upper = dvector(1, SG_basisSourceSize);
      nodeCenter = dvector(1, SG_basisSourceSize);
      if (!computeNodeCenterAndSupport(termPtr -> repMembrCount,
                                       termPtr -> repMembrIndx, SG_weight,
                                       parent -> augm -> common -> yArray,
                                       nodeCenter, &support)) {
        free_dvector(support.lower, 1, SG_basisSourceSize);
        free_dvector(support.upper, 1, SG_basisSourceSize);
        free_dvector(nodeCenter, 1, SG_basisSourceSize);
        RF_nativeError("\nRF-SGT:  *** ERROR *** ");
        RF_nativeError("\nRF-SGT: Unable to reconstruct support for tree %u, node %u.",
                       treeID, parentBase -> nodeID);
        RF_nativeExit();
      }
      double *scratch = dvector(1, SG_basisSourceSize);
      for (i = 1; i <= tstMembrSize; i++) {
        SG_yHatAbsoluteTest[treeID][tstMembrIndx[i]] =
          evaluateRestrictedNodePrediction(SG_fbasisSource, nodeCenter, &support,
                                           beta, tstMembrIndx[i], scratch);
      }
      free_dvector(scratch, 1, SG_basisSourceSize);
      free_dvector(support.lower, 1, SG_basisSourceSize);
      free_dvector(support.upper, 1, SG_basisSourceSize);
    }
    else {
      for (i = 1; i <= tstMembrSize; i++) {
        predicted = beta[1];
        for (k = 1; k <= hcutCnt; k++) {
          if (nodeCenter == NULL) {
            predicted = predicted + (beta[k+1] * fxArray[k][tstMembrIndx[i]]);
          }
          else if (fabs(beta[k+1]) > 0.0) {
            predicted = predicted + (beta[k+1] *
              evaluateNodeBasisTerm(SG_fbasisSource, nodeCenter, tstMembrIndx[i], k));
          }
        }
        SG_yHatAbsoluteTest[treeID][tstMembrIndx[i]] = predicted;
      }
    }
    termBasePtr = parentBase -> mate;
    termPtr = (Terminal *) termBasePtr;
    termPtr -> tstMembrCount = parent -> tstMembrSize;
    termPtr -> tstMembrIndx = uivector(1, termPtr -> tstMembrCount);
    for (i = 1; i <= termPtr -> tstMembrCount; i++) {
      termPtr -> tstMembrIndx[i] = parent -> tstMembrIndx[i];
    }
  }
  if (nodeCenter != NULL) {
    free_dvector(nodeCenter, 1, SG_basisSourceSize);
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
static void checkRestoreMembership(uint treeID,
                            uint nSize,
                            uint bootMembrSize,
                            uint *bootMembrIndx) {
  uint *bootstrapCount = uivector(1, nSize);
  uint *terminalCount = uivector(1, nSize);
  uint kind, i, ii, membershipSize;
  uint *membershipIndex;
  const char *label;
  LeafLinkedObj *leaf;
  Terminal *term;
  char valid = TRUE;
  for (kind = 0; kind < 2; kind++) {
    label = (kind == 0) ? "replicated" : "OOB";
    for (i = 1; i <= nSize; i++) {
      bootstrapCount[i] = terminalCount[i] = 0;
    }
    membershipSize = (kind == 0) ? bootMembrSize : RF_oobSize[treeID];
    membershipIndex = (kind == 0) ? bootMembrIndx : RF_oobMembershipIndex[treeID];
    for (i = 1; i <= membershipSize; i++) {
      ii = membershipIndex[i];
      if (ii == 0 || ii > nSize) {
        RF_nativeError("\nRF-SGT: Restore %s membership check failed for tree %u:"
                       " bootstrap index %u contains invalid observation %u.\n",
                       label, treeID, i, ii);
        valid = FALSE;
        goto cleanup;
      }
      bootstrapCount[ii]++;
    }
    for (leaf = RF_leafLinkedObjHead[treeID] -> fwdLink;
         leaf != NULL; leaf = leaf -> fwdLink) {
      term = (Terminal *) leaf -> termPtr;
      membershipSize = (kind == 0) ? term -> repMembrCount : term -> oobMembrCount;
      membershipIndex = (kind == 0) ? term -> repMembrIndx : term -> oobMembrIndx;
      for (i = 1; i <= membershipSize; i++) {
        ii = membershipIndex[i];
        if (ii == 0 || ii > nSize) {
          RF_nativeError("\nRF-SGT: Restore %s membership check failed for tree %u,"
                         " node %u: terminal index %u contains invalid observation %u.\n",
                         label, treeID, leaf -> nodeID, i, ii);
          valid = FALSE;
          goto cleanup;
        }
        terminalCount[ii]++;
      }
    }
    for (i = 1; i <= nSize; i++) {
      if (bootstrapCount[i] != terminalCount[i]) {
        RF_nativeError("\nRF-SGT: Restore %s membership mismatch for tree %u,"
                       " observation %u: bootstrap count=%u, terminal count=%u.\n",
                       label, treeID, i, bootstrapCount[i], terminalCount[i]);
        valid = FALSE;
        goto cleanup;
      }
      if (kind == 1 && terminalCount[i] > 1) {
        RF_nativeError("\nRF-SGT: Duplicate OOB membership for tree %u,"
                       " observation %u: count=%u.\n",
                       treeID, i, terminalCount[i]);
        valid = FALSE;
        goto cleanup;
      }
    }
  }
cleanup:
  free_uivector(terminalCount, 1, nSize);
  free_uivector(bootstrapCount, 1, nSize);
  if (valid) {
    RF_nativePrint("\nRF-SGT: Restore membership check passed for tree %u:"
                   " replicated=%u, OOB=%u.\n",
                   treeID, bootMembrSize, RF_oobSize[treeID]);
  }
  else {
    RF_nativeExit();
  }
}
