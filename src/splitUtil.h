#ifndef RF_SPLIT_UTIL_H
#define RF_SPLIT_UTIL_H
char getPreSplitResult (uint treeID, NodeBase *parent, uint nodeSize, double *response);
char getVarianceSinglePass(uint    repMembrSize,
                           uint   *repMembrIndx,
                           double *targetResponse,
                           double *mean,
                           double *variance);
void getRandomPair(uint treeID, uint relativeFactorSize, uint absoluteFactorSize, double *absoluteLevel, uint *result);
void createRandomBinaryPair(uint    treeID,
                            uint    relativeFactorSize,
                            uint    absoluteFactorSize,
                            uint    groupSize,
                            double *absolutelevel,
                            uint   *pair);
void convertRelToAbsBinaryPair(uint    treeID,
                               uint    relativeFactorSize,
                               uint    absoluteFactorSize,
                               uint    relativePair,
                               double *absoluteLevel,
                               uint   *pair);
#endif
