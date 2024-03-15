#ifndef  RF_SPLIT_INFO_DERIVED_H
#define  RF_SPLIT_INFO_DERIVED_H
#include "shared/splitInfo.h"
typedef struct splitInfoDerived SplitInfoDerived;
struct splitInfoDerived {
  struct splitInfoMax base;
  uint hcut;
  uint hcutCnt;
  double *beta;
  double yStar;
  double yBar;
  uint bsf;
};  
SplitInfoDerived *makeSplitInfoDerived(struct augmentationObjCommon *augmComm);
void freeSplitInfoDerived(SplitInfoDerived *info, struct augmentationObjCommon *augmComm);
#endif
