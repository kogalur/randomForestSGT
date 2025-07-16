#ifndef RF_GREEDY_INFO_H
#define RF_GREEDY_INFO_H
#include "node.h"
#include "splitInfoDerived.h"
typedef struct greedyObj GreedyObj;
struct greedyObj {
  Node *parent;
  Node *parentCart;
  Node *left;
  Node *right;
  Node *leftCart;
  Node *rightCart;
  GreedyObj *fwdLink;
  GreedyObj *bakLink;
  GreedyObj *head;
  SplitInfoDerived *splitInfoDerived;
  SplitInfoDerived *splitInfoDerivedCart;
  char leafFlag;
  double eRiskLoss;
  char bestSplitType;  
};
typedef struct lotObj LatOptTreeObj;
struct lotObj {
  char tune;
  uint strikeout;
  uint lag;
  double *risk;
  uint size;
  uint firstIn;
  uint lastIn;
  double firstOD;
  uint treeSize;
};
#endif
