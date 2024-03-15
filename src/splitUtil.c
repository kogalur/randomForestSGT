
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "splitUtil.h"
#include "shared/sampling.h"
#include "shared/nrutil.h"
#include "shared/error.h"
char getPreSplitResult (uint treeID, NodeBase *parent, uint nodeSize, double *response) {
  char result;
  result = TRUE;
  if (result) {
    if (parent -> repMembrSize < (2 * nodeSize)) {
      result = FALSE;
    }
  }
  if (result) {
        result = getVariance(parent -> repMembrSize,
                             parent -> repMembrIndx,
                             response,
                             & parent -> mean,
                             & parent -> variance);
  }
  return result;
}
char getVarianceSinglePass(uint    repMembrSize,
                           uint   *repMembrIndx,
                           double *response,
                           double *mean,
                           double *variance) {
  uint i;
  double meanResultOld, meanResult, varResultOld, varResult;
  char result;
  result = TRUE;
  meanResultOld = response[repMembrIndx[1]];
  varResultOld  = 0.0;
  for (i = 2; i <= repMembrSize; i++) {
    meanResult = meanResultOld + ((response[repMembrIndx[i]] - meanResultOld) / i);
    varResult  = varResultOld + ((response[repMembrIndx[i]] - meanResultOld) * (response[repMembrIndx[i]] - meanResult)); 
    meanResultOld = meanResult;
    varResultOld = varResult;
  }
  if (repMembrSize > 1) {
    varResultOld = varResultOld / (double) (repMembrSize - 1);
    result = ((varResultOld <= EPSILON) ? FALSE : TRUE);
  }
  if (mean     != NULL) *mean     = meanResultOld;
  if (variance != NULL) *variance = varResultOld;
  return result;
}
void getRandomPair (uint treeID, uint relativeFactorSize, uint absoluteFactorSize, double *absoluteLevel, uint *result) {
  uint randomGroupIndex;
  double randomValue;
  uint k;
  if(RF_factorList[treeID][relativeFactorSize] == NULL) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  Factor not allocated for size:  %10d", relativeFactorSize);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  double *cdf = dvector(1, RF_factorList[treeID][relativeFactorSize] -> cardinalGroupCount);
  if (relativeFactorSize <= MAX_EXACT_LEVEL) {
    for (k=1; k <= RF_factorList[treeID][relativeFactorSize] -> cardinalGroupCount; k++) {
      cdf[k] = (double) ((uint*) RF_factorList[treeID][relativeFactorSize] -> cardinalGroupSize)[k];
    }
  }
  else {
    for (k=1; k <= RF_factorList[treeID][relativeFactorSize] -> cardinalGroupCount; k++) {
      cdf[k] = ((double*) RF_factorList[treeID][relativeFactorSize] -> cardinalGroupSize)[k];
    }
  }
  for (k=2; k <= RF_factorList[treeID][relativeFactorSize] -> cardinalGroupCount; k++) {
    cdf[k] += cdf[k-1];
  }
  randomValue = ceil((ran1B(treeID) * cdf[RF_factorList[treeID][relativeFactorSize] -> cardinalGroupCount]));
  randomGroupIndex = 1;
  while (randomValue > cdf[randomGroupIndex]) {
    randomGroupIndex ++;
  }
  free_dvector(cdf, 1, RF_factorList[treeID][relativeFactorSize] -> cardinalGroupCount);
  createRandomBinaryPair(treeID, relativeFactorSize, absoluteFactorSize, randomGroupIndex, absoluteLevel, result);
}
void createRandomBinaryPair(uint    treeID,
                            uint    relativeFactorSize,
                            uint    absoluteFactorSize,
                            uint    groupIndex,
                            double *absoluteLevel,
                            uint   *pair) {
  uint mwcpLevelIdentifier;
  uint mwcpSizeAbsolute;
  uint offset, levelSize, levelIndex;
  uint k;
  levelIndex = 0;  
  mwcpSizeAbsolute = RF_factorList[treeID][absoluteFactorSize] -> mwcpSize;
  uint *levelVector = uivector(1, relativeFactorSize);
  uint *randomLevel = uivector(1, groupIndex);
  for (k = 1; k <= relativeFactorSize; k++) {
    levelVector[k] = k;
  }
  levelSize = relativeFactorSize;
  for (k = 1; k <= groupIndex; k++) {
    randomLevel[k] = sampleUniformlyFromVector(treeID,
                                               ran1B,
                                               levelVector,
                                               levelSize,
                                               &levelIndex);
    levelVector[levelIndex] = levelVector[levelSize];
    levelSize --;
  }
  for (k = 1; k <= groupIndex; k++) {
    randomLevel[k] = (uint) absoluteLevel[randomLevel[k]];
  }
  for (offset = 1; offset <= mwcpSizeAbsolute; offset++) {
    pair[offset] = 0;
  }
  for (k = 1; k <= groupIndex; k++) {
    mwcpLevelIdentifier = (randomLevel[k] >> (3 + ulog2(sizeof(uint)))) + ((randomLevel[k] & (MAX_EXACT_LEVEL - 1)) ? 1 : 0);
    pair[mwcpLevelIdentifier] += upower(2, randomLevel[k] - ((mwcpLevelIdentifier - 1) * MAX_EXACT_LEVEL) - 1 );
  }
  free_uivector(levelVector, 1, relativeFactorSize);
  free_uivector(randomLevel, 1, groupIndex);
}
void convertRelToAbsBinaryPair(uint    treeID,
                               uint    relativeFactorSize,
                               uint    absoluteFactorSize,
                               uint    relativePair,
                               double *absoluteLevel,
                               uint   *pair) {
  uint mwcpLevelIdentifier;
  uint mwcpSizeAbsolute;
  uint coercedAbsoluteLevel;
  uint k, offset;
  mwcpSizeAbsolute = RF_factorList[treeID][absoluteFactorSize] -> mwcpSize;
  for (offset = 1; offset <= mwcpSizeAbsolute; offset++) {
    pair[offset] = 0;
  }
  for (k = 1; k <= relativeFactorSize; k++) {
    if (relativePair & ((uint) 0x01)) {
      coercedAbsoluteLevel = (uint) absoluteLevel[k];
      mwcpLevelIdentifier = (coercedAbsoluteLevel >> (3 + ulog2(sizeof(uint)))) + ((coercedAbsoluteLevel & (MAX_EXACT_LEVEL - 1)) ? 1 : 0);
      pair[mwcpLevelIdentifier] += upower(2, coercedAbsoluteLevel - ((mwcpLevelIdentifier - 1) * MAX_EXACT_LEVEL) - 1 );
    }
    relativePair = relativePair >> 1;
  }
}
