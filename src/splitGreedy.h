#ifndef RF_SPLIT_GREEDY_H
#define RF_SPLIT_GREEDY_H
#include "greedyInfo.h"
char getBestSplit(uint treeID, LatOptTreeObj *lotObj, GreedyObj *greedyMembr);
char virtuallySplitNodeGreedyLasso(uint       treeID,
                                  GreedyObj *greedyMembr,
                                  char   *localSplitIndicator,
                                  uint   *localSplitIndx,
                                  double *localSplitValue,
                                  uint   *leftSize);
void initializeBetaAndPredicted(uint treeID, Node *parent);
double splitRuleGreedyOne(uint treeID, uint repMembrSize, double *yHat);
double splitRuleGreedyTwo(uint treeID, uint repMembrSize, double *yHat);
#endif
