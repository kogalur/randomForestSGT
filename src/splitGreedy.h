#ifndef RF_SPLIT_GREEDY_H
#define RF_SPLIT_GREEDY_H
#include "greedyInfo.h"
#include "cdlInfo.h"
GreedyObj *makeGreedyObj(Node *parent, GreedyObj *head);
void freeGreedyObj(GreedyObj *gObj);
void freeGreedyObjList(GreedyObj *gObj);
double standardVector(uint       treeID,
                      char       standardFlag,
                      GreedyObj *greedyMembr,
                      double    *response,
                      uint      *repMembrIndx,
                      uint       repMembrSize);
double getNegLogLikelihood(uint    treeID,
                           double *response,
                           uint    maxLevel,
                           char    selectFlag,
                           char   *membershipFlag,
                           uint   *repMembrIndx,
                           uint    repMembrSize,
                           uint   *allMembrIndx,
                           uint    allMembrSize);
LatOptTreeObj *makeLatOptTreeObj(uint lag);
void freeLatOptTreeObj(LatOptTreeObj *lotObj);
void insertRisk(uint treeID, LatOptTreeObj *obj, double value);
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
