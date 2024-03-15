#ifndef RF_SPLIT_REGR_H
#define RF_SPLIT_REGR_H
#include "greedyInfo.h"
#include "shared/sampling.h"
#include "shared/splitInfo.h"
char virtuallySplitNodeGreedyCart (uint       treeID,
                                   GreedyObj *greedyMembr,
                                   char   *localSplitIndicator,
                                   uint   *localSplitIndx,
                                   double *localSplitValue,
                                   uint   *leftSize);
void initializeBetaAndPredictedCart(uint treeID, Node *parent);
DistributionObj *stackRandomCovariatesSimple(uint treeID, NodeBase *parent, uint mtry);
void unstackRandomCovariatesSimple(uint treeID, DistributionObj *obj);
char selectRandomCovariatesSimple(uint      treeID,
                                  NodeBase *parent,
                                  DistributionObj *obj,
                                  uint      mtry,
                                  char     *factorFlag,
                                  uint     *xvarID,
                                  uint     *xvarCount);
uint stackAndConstructSplitVectorGenericPhase1 (uint       treeID,
                                                NodeBase  *parent,
                                                uint       covariate,
                                                double    *observation, ...);
uint stackAndConstructSplitVectorGenericPhase2 (uint         treeID,
                                                NodeBase    *parent,
                                                uint         covariate,
                                                double      *splitVector,
                                                uint         vectorSize,
                                                char         factorFlag,
                                                char        *deterministicSplitFlag,
                                                uint        *mwcpSizeAbsolute,
                                                void       **splitVectorPtr);
void unstackSplitVectorGeneric(uint       treeID,
                               NodeBase  *parent,
                               uint       splitLength,
                               char       factorFlag,
                               uint       splitVectorSize,
                               uint       mwcpSizeAbsolute,
                               char       deterministicSplitFlag,
                               void      *splitVectorPtr,
                               uint      *indxx);
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
                        uint *currentMembrIter);
char updateMaximumSplitCart(uint    treeID,
                            Node   *parent,
                            double  delta,
                            uint    covariate,
                            uint    index,
                            char    factorFlag,
                            uint    mwcpSizeAbsolute,
                            void   *splitVectorPtr,
                            SplitInfoMax *infoMax);
#endif
