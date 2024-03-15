
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "splitRegr.h"
#include "greedyInfo.h"
#include "splitUtil.h"
#include "augmentationOpsCommon.h"
#include "augmentationOpsCustom.h"
#include "augmentationOpsSimple.h"
#include "augmentationOps.h"
#include "shared/error.h"
#include "shared/sampling.h"
#include "shared/factorOps.h"
#include "shared/splitInfo.h"
#include "shared/nrutil.h"
char virtuallySplitNodeGreedyCart (uint       treeID,
                                   GreedyObj *greedyMembr,
                                   char   *localSplitIndicator,
                                   uint   *localSplitIndx,
                                   double *localSplitValue,
                                   uint   *leftSize) {
  Node     *parent;
  NodeBase *baseParent;
  uint  repMembrSize;
  uint *repMembrIndx;
  uint     xvarID;
  uint     xvarCount;
  double  *splitVector;
  uint     splitVectorSize;
  uint   *indxx;
  double **xArray;
  double  *yArray;
  uint priorMembrIter, currentMembrIter;
  uint rghtSize;
  char daughterFlag;
  uint splitLength;
  void *splitVectorPtr;
  double *observation;
  char factorFlag;
  uint mwcpSizeAbsolute;
  char deterministicSplitFlag;
  char result;
  double delta;
  uint i, j, k;
  parent = greedyMembr -> parent;
  baseParent = (NodeBase *) parent;
  AugmentationObjCommon *augmObjCommon = parent -> augm -> common;  
  repMembrSize = baseParent -> repMembrSize;
  repMembrIndx = baseParent -> repMembrIndx;
  yArray = augmObjCommon -> yArray;
  xArray = augmObjCommon -> xArray;
  splitVector = dvector(1, repMembrSize);
  DistributionObj *distObj = stackRandomCovariatesSimple(treeID, baseParent, RF_mtry);
  double sumLeft, sumRght, sumLeftSqr, sumRghtSqr;
  double sumRghtSave, sumRghtSqrSave;
  double leftTemp1, rghtTemp1;
  sumLeft = sumRght = sumLeftSqr = sumRghtSqr = 0.0;  
  sumRghtSave = sumRghtSqrSave = 0.0;                 
  delta = RF_nativeNaN;                               
  SplitInfoMax *infoMax = (SplitInfoMax *) (greedyMembr -> splitInfoDerivedCart);
  double deltaMax;
  uint   indexMax;
  sumRghtSave = (baseParent -> mean) * repMembrSize;
  xvarCount = 0;
  result = FALSE;
  while (selectRandomCovariatesSimple(treeID,
                                      baseParent,
                                      distObj,
                                      RF_mtry,
                                      & factorFlag,
                                      & xvarID,
                                      & xvarCount)) {
    observation = xArray[xvarID];
    splitVectorSize = stackAndConstructSplitVectorGenericPhase1(treeID,
                                                                baseParent,
                                                                xvarID,
                                                                observation,
                                                                splitVector,
                                                                & indxx);
    if (splitVectorSize >= 2) {
      result = TRUE;
      splitLength = stackAndConstructSplitVectorGenericPhase2(treeID,
                                                              baseParent,
                                                              xvarID,
                                                              splitVector,
                                                              splitVectorSize,
                                                              factorFlag,
                                                              & deterministicSplitFlag,
                                                              & mwcpSizeAbsolute,
                                                              & splitVectorPtr);
      *leftSize = 0;
      priorMembrIter = 0;
      if (factorFlag == FALSE) {
        sumLeft = sumLeftSqr = 0.0;
        sumRght = sumRghtSave;
        for (j = 1; j <= repMembrSize; j++) {
          localSplitIndicator[repMembrIndx[j]] = RIGHT;
        }
      }
      deltaMax =  RF_nativeNaN;
      indexMax =  0;
      for (j = 1; j < splitLength; j++) {
        if (factorFlag == TRUE) {
          priorMembrIter = 0;
          *leftSize = 0;
        }
        virtuallySplitNode(treeID,
                           baseParent,
                           factorFlag,
                           mwcpSizeAbsolute,
                           observation,
                           indxx,
                           priorMembrIter,
                           splitVectorPtr,
                           j,
                           localSplitIndicator,                                 
                           leftSize,
                           & currentMembrIter);
        rghtSize = repMembrSize - (*leftSize);
        if (((*leftSize) != 0) && (rghtSize != 0)) {
          if (factorFlag == TRUE) {
            sumLeft = sumRght = 0.0;
            for (k = 1; k <= repMembrSize; k++) {
              if (localSplitIndicator[ repMembrIndx[k] ] == LEFT) {
                sumLeft += yArray[ repMembrIndx[k] ];
              }
              else {
                sumRght += yArray[ repMembrIndx[k] ];
              }
            } 
          }
          else {
            for (k = priorMembrIter + 1; k < currentMembrIter; k++) {
              sumLeft += yArray[ repMembrIndx[indxx[k]] ];
              sumRght -= yArray[ repMembrIndx[indxx[k]] ];
            }
          }
          leftTemp1 = pow (sumLeft, 2.0) / (*leftSize);
          rghtTemp1 = pow (sumRght, 2.0) / rghtSize;
          delta = leftTemp1 + rghtTemp1;
        }
        else {
          delta = RF_nativeNaN;
        }
        if (!RF_nativeIsNaN(delta)) {
          if(RF_nativeIsNaN(deltaMax)) {
            deltaMax = delta;
            indexMax = j;
          }
          else {
            if ((delta - deltaMax) > EPSILON) {
              deltaMax = delta;
              indexMax = j;
            }
            else {
            }
          }
        }
        else {
        }
        if (factorFlag == FALSE) {
          priorMembrIter = currentMembrIter - 1;
        }
      }  
      updateMaximumSplitCart(treeID,
                             parent,
                             deltaMax,
                             xvarID,
                             indexMax,
                             factorFlag,
                             mwcpSizeAbsolute,
                             splitVectorPtr,
                             infoMax);
      unstackSplitVectorGeneric(treeID,
                                baseParent,
                                splitLength,
                                factorFlag,            
                                splitVectorSize,
                                mwcpSizeAbsolute,
                                deterministicSplitFlag,
                                splitVectorPtr,
                                indxx);
    }
  }  
  unstackRandomCovariatesSimple(treeID, distObj);
  free_dvector(splitVector, 1, repMembrSize);
  if (result) {
    *localSplitIndx = 0;
    *localSplitValue = RF_nativeNaN;
    Node *left  = makeNode(augmObjCommon -> pSize);
    Node *right = makeNode(augmObjCommon -> pSize);
    left  -> nSize = augmObjCommon -> nSize;
    right -> nSize = augmObjCommon -> nSize;
    greedyMembr -> leftCart  = left;
    greedyMembr -> rightCart = right;
    NodeBase *baseLeft  = (NodeBase *) left;
    NodeBase *baseRight = (NodeBase *) right;
    baseLeft ->  repMembrIndx  = uivector(1, repMembrSize);
    baseRight -> repMembrIndx  = uivector(1, repMembrSize);
    baseLeft  -> repMembrSizeAlloc = repMembrSize;
    baseRight -> repMembrSizeAlloc = repMembrSize;  
    *leftSize = 0;
    rghtSize = 0;
    observation = xArray[infoMax -> splitParameter];
    if (infoMax -> splitValueFactSize > 0) {
      for (i = 1; i <= repMembrSize; i++) {
        daughterFlag = splitOnFactor((uint)  observation[repMembrIndx[i]], infoMax -> splitValueFactPtr);
        if (daughterFlag == LEFT) {
          localSplitIndicator[repMembrIndx[i]] = LEFT;
          (*leftSize) ++;
          baseLeft -> repMembrIndx[*leftSize] = repMembrIndx[i];
        }
        else {
          localSplitIndicator[repMembrIndx[i]] = RIGHT;
          rghtSize ++;
          baseRight -> repMembrIndx[rghtSize] = repMembrIndx[i];
        }
      }
    }
    else {
      for (i = 1; i <= repMembrSize; i++) {
        if ( (infoMax -> splitValueCont - observation[repMembrIndx[i]]) >= 0.0) {
          daughterFlag = LEFT;
        }
        else {
          daughterFlag = RIGHT;
        }
        if (daughterFlag == LEFT) {
          localSplitIndicator[repMembrIndx[i]] = LEFT;
          (*leftSize) ++;
          baseLeft -> repMembrIndx[*leftSize] = repMembrIndx[i];
        }
        else {
          localSplitIndicator[repMembrIndx[i]] = RIGHT;
          rghtSize ++;
          baseRight -> repMembrIndx[rghtSize] = repMembrIndx[i];
        }
      }
    }
    baseLeft  -> repMembrSize = *leftSize;
    baseRight -> repMembrSize =  rghtSize;
    for (k = 1; k <= baseParent -> xSize; k++) {
      baseLeft -> permissible[k] = baseRight -> permissible[k] = baseParent -> permissible[k];
    }
    left  -> augm =  getAugmentationObjGeneric(augmObjCommon, treeID, left);
    right -> augm =  getAugmentationObjGeneric(augmObjCommon, treeID, right);
    left -> yHatAbsoluteCart  = dvector(1, augmObjCommon -> nSize);
    right -> yHatAbsoluteCart = dvector(1, augmObjCommon -> nSize);
    initializeBetaAndPredictedCart(treeID, left);
    left -> outcome = OUTC_CART;
    initializeBetaAndPredictedCart(treeID, right);
    right -> outcome = OUTC_CART;
  }
  return result;
}
void initializeBetaAndPredictedCart(uint treeID, Node *parent) {
  double  *yArray;
  uint *repMembrIndx;
  uint  repMembrSize;
  uint  hcutCnt;
  double **beta;
  double  *yHatAbsoluteCart;
  double *eRiskPtr;
  double  localMean;
  uint i, jj;
  hcutCnt = parent -> augm -> common -> hcutCnt;
  yArray = parent -> augm -> common -> yArray;
  repMembrIndx = ((NodeBase *) parent) -> repMembrIndx;
  repMembrSize = ((NodeBase *) parent) -> repMembrSize;
  beta = & (parent -> beta);
  yHatAbsoluteCart = parent -> yHatAbsoluteCart;
  *beta = dvector(1, hcutCnt + 1);
  (*beta)[1] = 0.0;
  for (jj = 1; jj <= hcutCnt; jj++) {
    (*beta)[jj + 1] = 0.0;
  }
  localMean = 0.0;
  for (i = 1; i <= repMembrSize; i++) {
    localMean += yArray[repMembrIndx[i]];
  }
  localMean = localMean / repMembrSize;
  parent -> mean = localMean;
  for (i = 1; i <= repMembrSize; i++) {
    yHatAbsoluteCart[repMembrIndx[i]] = localMean;
  }
  eRiskPtr = & (parent -> eRiskCart);
  *eRiskPtr = 0.0;
  for (i = 1; i <= repMembrSize; i++) {
    *eRiskPtr += pow (yArray[repMembrIndx[i]] - localMean, 2.0);
  }
}
DistributionObj *stackRandomCovariatesSimple(uint treeID, NodeBase *parent, uint mtry) {
  char *permissible;
  uint  xSize;
  uint indexSize;
  DistributionObj *obj = makeDistributionObjRaw();
  xSize = parent -> xSize;
  permissible = parent -> permissible;
  obj -> indexSizeAlloc = xSize;
  obj -> index = uivector(1, xSize);
  indexSize = 0;
  for (uint p = 1; p <= xSize; p++) {
    if (permissible[p]) {
      (obj -> index)[++indexSize] = p;
    }
  }
  obj -> indexSize = indexSize;
  if (indexSize <= mtry) {
    obj -> deterministicFlag = TRUE;
  }
  else {
    obj -> deterministicFlag = FALSE;
  }
  return obj;
}
void unstackRandomCovariatesSimple(uint treeID, DistributionObj *obj) {
  if (obj -> indexSizeAlloc > 0) {
    if (obj -> index != NULL) {
      free_uivector(obj -> index, 1, obj -> indexSizeAlloc);
      obj -> index = NULL;
    }
    obj -> indexSizeAlloc = 0;
  }
  freeDistributionObjRaw(obj);
}
char selectRandomCovariatesSimple(uint      treeID,
                                  NodeBase *parent,
                                  DistributionObj *obj,
                                  uint      mtry,
                                  char     *factorFlag,
                                  uint     *xvarID,
                                  uint     *xvarCount) {
  char found;
  (*xvarID) =  UINT_MAX;
  found = FALSE;
  *factorFlag = FALSE;
  while ( ((*xvarCount) < mtry) && ((*xvarID) != 0) && (found == FALSE)) {
    if (obj -> indexSize > 0) {
      (*xvarCount) ++;
      if (mtry == 1) {
        obj -> slot = (uint) ceil(ran1B(treeID) * ((obj -> indexSize) * 1.0));
        *xvarID = obj -> index[obj -> slot];
        found = TRUE;
      }
      else {
        if (obj -> deterministicFlag) {
          *xvarID = (obj -> index)[obj -> indexSize];
          (obj -> indexSize) --;
          found = TRUE;
        }
        else {
          obj -> slot = (uint) ceil(ran1B(treeID) * ((obj -> indexSize) * 1.0));
          *xvarID = (obj -> index)[obj -> slot];
          (obj -> index)[obj -> slot] = (obj -> index)[obj -> indexSize];
          (obj -> indexSize) --;
          found = TRUE;
        }
      }
      if (RF_xType[*xvarID] == 'C') {
        *factorFlag = TRUE;
      }
    }
    else {
      *xvarID = 0;
    }
  }  
  return found;
}
uint stackAndConstructSplitVectorGenericPhase1 (uint       treeID,
                                                NodeBase  *parent,
                                                uint       covariate,
                                                double    *observation, ...) {
  double *splitVectorTemp;
  uint vectorSize;
  uint i;
  uint  *repMembrIndx = parent -> repMembrIndx;
  uint   repMembrSize = parent -> repMembrSize;
  va_list list;
  va_start(list, observation);
  double *splitVector = va_arg(list, double*);
  uint **indxx = (uint**) va_arg(list, uint**);
  splitVectorTemp = dvector(1, repMembrSize);
  vectorSize = 0;
  for (i = 1; i <= repMembrSize; i++) {
    splitVectorTemp[i] = observation[repMembrIndx[i]];
  }
  if ((repMembrSize) >= 2) {
    (*indxx) = uivector(1, repMembrSize);
    indexx((repMembrSize),
           splitVectorTemp,
           (*indxx));
    splitVector[1] = splitVectorTemp[(*indxx)[1]];
    vectorSize = 1;
    for (i = 2; i <= (repMembrSize); i++) {
      if (splitVectorTemp[(*indxx)[i]] > splitVector[vectorSize]) {
        vectorSize ++;
        splitVector[vectorSize] = splitVectorTemp[(*indxx)[i]];
      }
    }
    if(vectorSize >= 2) {
    }
    else {
      vectorSize = 0;
      if (covariate <= RF_xSize) {
        (parent -> permissible)[covariate] = FALSE;
      }
      free_uivector(*indxx, 1, repMembrSize);
    }
  }
  else {
    vectorSize = 0;
    (parent -> permissible)[covariate] = FALSE;
  }
  free_dvector(splitVectorTemp, 1, repMembrSize);
  return vectorSize;
}
uint stackAndConstructSplitVectorGenericPhase2 (uint         treeID,
                                                NodeBase    *parent,
                                                uint         covariate,
                                                double      *splitVector,
                                                uint         vectorSize,
                                                char         factorFlag,
                                                char        *deterministicSplitFlag,
                                                uint        *mwcpSizeAbsolute,
                                                void       **splitVectorPtr) {
  uint repMembrSize;
  uint  sworIndex;
  uint *sworVector;
  uint  sworVectorSize;
  uint j, j2, k2;
  uint factorSizeAbsolute;
  uint offset;
  uint splitLength;
  uint relativePair;
  repMembrSize = parent -> repMembrSize;
  splitLength = 0;  
  (*splitVectorPtr) = NULL;  
  if (vectorSize < 2) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Split vector is of insufficient size in Stack Phase II allocation:  %10d", vectorSize);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  if (factorFlag) {
    if(RF_factorList[treeID][vectorSize] == NULL) {
      RF_factorList[treeID][vectorSize] = makeFactor(vectorSize, FALSE);
    }
    factorSizeAbsolute = RF_xFactorSize[RF_xFactorMap[covariate]];
    *mwcpSizeAbsolute = RF_factorList[treeID][factorSizeAbsolute] -> mwcpSize;
    if (SG_splitRule == RAND_CRT) {
      splitLength = 1 + 1;
      *deterministicSplitFlag = FALSE;
    }
    else {
      if (RF_nsplit == 0) {
        *deterministicSplitFlag = TRUE;
        if ((RF_factorList[treeID][vectorSize] -> r) > MAX_EXACT_LEVEL) {
          *deterministicSplitFlag = FALSE;
        }
        else {
          if ( *((uint *) RF_factorList[treeID][vectorSize] -> complementaryPairCount) >= repMembrSize ) {
            *deterministicSplitFlag = FALSE;
          }
        }
        if (*deterministicSplitFlag == FALSE) {
          splitLength = repMembrSize + 1;
        }
        else {
          splitLength = *((uint*) RF_factorList[treeID][vectorSize] -> complementaryPairCount) + 1;
        }
      }
      else {
        *deterministicSplitFlag = FALSE;
        if ((RF_factorList[treeID][vectorSize] -> r) <= MAX_EXACT_LEVEL) {
          if (*((uint*) RF_factorList[treeID][vectorSize] -> complementaryPairCount) <= ((RF_nsplit <= repMembrSize) ? RF_nsplit : repMembrSize)) {
            splitLength = *((uint*) RF_factorList[treeID][vectorSize] -> complementaryPairCount) + 1;
            *deterministicSplitFlag = TRUE;
          }
        }
        if (*deterministicSplitFlag == FALSE) {
          splitLength = 1 + ((RF_nsplit <= repMembrSize) ? RF_nsplit : repMembrSize);
        }
      }  
    }  
    (*splitVectorPtr) = uivector(1, splitLength * (*mwcpSizeAbsolute));
    for (offset = 1; offset <= *mwcpSizeAbsolute; offset++) {
      ((uint*) (*splitVectorPtr) + ((splitLength - 1) * (*mwcpSizeAbsolute)))[offset] = 0;
    }
    if (*deterministicSplitFlag) {
      bookFactor(RF_factorList[treeID][vectorSize]);
      j2 = 0;
      for (j = 1; j <= RF_factorList[treeID][vectorSize] -> cardinalGroupCount; j++) {
        for (k2 = 1; k2 <= ((uint*) RF_factorList[treeID][vectorSize] -> cardinalGroupSize)[j]; k2++) {
          ++j2;
          relativePair = (RF_factorList[treeID][vectorSize] -> cardinalGroupBinary)[j][k2];
          convertRelToAbsBinaryPair(treeID,
                                    vectorSize,
                                    factorSizeAbsolute,
                                    relativePair,
                                    splitVector,
                                    (uint*) (*splitVectorPtr) + ((j2 - 1) * (*mwcpSizeAbsolute)));
        }
      }
    }  
    else {
      for (j = 1; j < splitLength; j++) {
        getRandomPair(treeID, vectorSize, factorSizeAbsolute, splitVector, (uint*) (*splitVectorPtr) + ((j - 1) * (*mwcpSizeAbsolute)));
      }
    }
  }  
  else {
    if (SG_splitRule == RAND_CRT) {
      splitLength = 1 + 1;
      *deterministicSplitFlag = FALSE;
    }
    else {
      if(RF_nsplit == 0) {
        splitLength = vectorSize;
        (*splitVectorPtr) = splitVector;
        *deterministicSplitFlag = TRUE;
      }
      else {
        if (vectorSize <= RF_nsplit + 1) {
          splitLength = vectorSize;
          (*splitVectorPtr) = splitVector;
          *deterministicSplitFlag = TRUE;
        }
        else {
          splitLength = RF_nsplit + 1;
          *deterministicSplitFlag = FALSE;
        }
      }  
    }  
    if (*deterministicSplitFlag == FALSE) {
      (*splitVectorPtr) = dvector(1, splitLength);
      ((double*) (*splitVectorPtr))[splitLength] = 0;
      if (SG_splitRule == RAND_CRT) {
        ((double*) (*splitVectorPtr))[1]  = splitVector[(uint) ceil(ran1B(treeID) * ((vectorSize - 1) * 1.0))];
      }
      else {
        sworVector = uivector(1, vectorSize);
        sworVectorSize = vectorSize - 1;
        for (j = 1; j <= sworVectorSize; j++) {
          sworVector[j] = j;
        }
        for (j = 1; j < splitLength; j++) {
          sworIndex = (uint) ceil(ran1B(treeID) * (sworVectorSize * 1.0));
          ((double*) (*splitVectorPtr))[j]  = splitVector[sworVector[sworIndex]];
          sworVector[sworIndex] = sworVector[sworVectorSize];
          sworVectorSize --;
        }
        free_uivector (sworVector, 1, vectorSize);
        qksort(((double*) (*splitVectorPtr)), splitLength-1);
      }
    }
  }  
  return splitLength;
}
void unstackSplitVectorGeneric(uint       treeID,
                               NodeBase  *parent,
                               uint       splitLength,
                               char       factorFlag,
                               uint       splitVectorSize,
                               uint       mwcpSizeAbsolute,
                               char       deterministicSplitFlag,
                               void      *splitVectorPtr,
                               uint      *indxx) {
  if (splitLength > 0) {
    if (factorFlag == TRUE) {
      free_uivector(splitVectorPtr, 1, splitLength * mwcpSizeAbsolute);
      if (deterministicSplitFlag == FALSE) {
        if (splitVectorSize > SAFE_FACTOR_SIZE) {
          unbookFactor(RF_factorList[treeID][splitVectorSize]);
        }
      }
    }
    else {
      if (deterministicSplitFlag == FALSE) {
        free_dvector(splitVectorPtr, 1, splitLength);
      }
    }
    if (indxx != NULL) {
      free_uivector((indxx), 1, parent -> repMembrSize);
    }
  }
}
uint virtuallySplitNode(uint      treeID,
                        NodeBase *parent,
                        char  factorFlag,
                        uint  mwcpSizeAbsolute,
                        double *observation,
                        uint *indxx,
                        uint  priorMembrIter,
                        void *splitVectorPtr,
                        uint  offset,
                        char *localSplitIndicator,
                        uint *leftSize,
                        uint *currentMembrIter) {
  uint *repMembrIndx;
  uint  repMembrSize;
  char daughterFlag;
  char iterFlag;
  iterFlag = TRUE;
  repMembrIndx = parent -> repMembrIndx;
  repMembrSize = parent -> repMembrSize;
  *currentMembrIter = priorMembrIter;
  while (iterFlag) {
    (*currentMembrIter) ++;
    if (factorFlag == TRUE) {
      daughterFlag = splitOnFactor((uint)  observation[    repMembrIndx[*currentMembrIter]     ],
                                   (uint*) splitVectorPtr + ((offset - 1) * mwcpSizeAbsolute));
      localSplitIndicator[     repMembrIndx[*currentMembrIter]   ] = daughterFlag;
      if ((*currentMembrIter) == repMembrSize) {
        iterFlag = FALSE;
      }
    }
    else {
      if ((((double*) splitVectorPtr)[offset] - observation[   repMembrIndx[indxx[*currentMembrIter]]    ]) >= 0.0) {
        daughterFlag = LEFT;
      }
      else {
        daughterFlag = RIGHT;
        iterFlag = FALSE;
      }
      localSplitIndicator[     repMembrIndx[indxx[*currentMembrIter]]   ] = daughterFlag;
    }
    if (daughterFlag == LEFT) {
      (*leftSize) ++;
    }
  }  
  if ((*leftSize == 0) || (*leftSize == repMembrSize)) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Left or Right Daughter of size zero:  (%10d, %10d)", *leftSize, repMembrSize);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  return (*leftSize);
}
char updateMaximumSplitCart(uint    treeID,
                            Node   *parent,
                            double  delta,
                            uint    covariate,
                            uint    index,
                            char    factorFlag,
                            uint    mwcpSizeAbsolute,
                            void   *splitVectorPtr,
                            SplitInfoMax *infoMax) {
  char flag;
  uint k;
  if(RF_nativeIsNaN(delta)) {
    flag = FALSE;
  }
  else {
    if(RF_nativeIsNaN(infoMax -> delta)) {
      flag = TRUE;
    }
    else {
      if ((delta - (infoMax -> delta)) > EPSILON) {
        flag = TRUE;
      }
      else {
        flag = FALSE;
      }
    }
  }
  if (flag) {
    infoMax -> delta = delta;
    infoMax -> splitParameter = covariate;
    if (factorFlag == TRUE) {
      if (infoMax -> splitValueFactSize > 0) {
        if (infoMax -> splitValueFactSize != mwcpSizeAbsolute) {
          free_uivector(infoMax -> splitValueFactPtr, 1, infoMax -> splitValueFactSize);
          infoMax -> splitValueFactSize = mwcpSizeAbsolute;
          infoMax -> splitValueFactPtr = uivector(1, infoMax -> splitValueFactSize);
        }
      }
      else {
        infoMax -> splitValueFactSize = mwcpSizeAbsolute;
        infoMax -> splitValueFactPtr = uivector(1, infoMax -> splitValueFactSize);
      }
      infoMax -> splitValueCont = RF_nativeNaN;
      for (k=1; k <= infoMax -> splitValueFactSize; k++) {
        (infoMax -> splitValueFactPtr)[k] =
          ((uint*) splitVectorPtr + ((index - 1) * (infoMax -> splitValueFactSize)))[k];
      }
    }
    else {
      if (infoMax -> splitValueFactSize > 0) {
        free_uivector(infoMax -> splitValueFactPtr, 1, infoMax -> splitValueFactSize);
        infoMax -> splitValueFactSize = 0;
        infoMax -> splitValueFactPtr = NULL;
      }
      else {
      }
      infoMax -> splitValueCont = ((double*) splitVectorPtr)[index];
    }
  }
  else {
  }
  return flag;
}
