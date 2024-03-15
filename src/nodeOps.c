
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "nodeOps.h"
#include "node.h"
#include "shared/nrutil.h"
#include "shared/nodeBaseOps.h"
#include "augmentationOpsCommon.h"
#include "augmentationOpsCustom.h"
#include "augmentationOpsSimple.h"
#include "augmentationOps.h"
#include "splitInfoDerived.h"
void *makeNodeDerived(uint xSize) {
  Node *parent = (Node*) gblock((size_t) sizeof(Node));
  initNodeBase((NodeBase*) parent, xSize);
  parent -> nSize = 0;
  parent -> augm = NULL;
  parent -> eRisk     = RF_nativeNaN;
  parent -> beta  = NULL;
  parent -> yHatAbsolute = NULL;
  parent -> yHatAbsoluteLasso = NULL;
  parent -> yHatAbsoluteCart = NULL;
  parent -> eRiskCart  = RF_nativeNaN;
  parent -> mean = RF_nativeNaN;
  parent -> hcut = 0;
  parent -> outcome = 0;
  parent -> splitInfoDerived = NULL;
  parent -> testMembrSizeAlloc = 0;
  parent -> testMembrSize = 0;
  parent -> testMembrIndx = NULL;
  return parent;
}
void freeNodeDerived(void *parent) {
  if (((Node*) parent) -> beta != NULL) {
    free_dvector(((Node*) parent) -> beta, 1, ((Node*) parent) -> augm -> common -> hcutCnt + 1);
    ((Node*) parent) -> beta = NULL;
  }
  if (((Node*) parent) -> splitInfoDerived != NULL) {
    freeSplitInfoDerived(((Node*) parent) -> splitInfoDerived, ((Node*) parent) -> augm -> common);
  }
  if (((Node*) parent) -> yHatAbsoluteCart != NULL) {
    free_dvector(((Node*) parent) -> yHatAbsoluteCart, 1, ((Node*) parent) -> nSize);
    ((Node*) parent) -> yHatAbsoluteCart = NULL;
  }
  if (((Node*) parent) -> yHatAbsoluteLasso != NULL) {
    free_dvector(((Node*) parent) -> yHatAbsoluteLasso, 1, ((Node*) parent) -> nSize);
    ((Node*) parent) -> yHatAbsoluteLasso = NULL;
  }
  if (((Node*) parent) -> augm != NULL) {
    freeAugmentationObj(((Node*) parent) -> augm);
  }
  if (((Node*) parent) -> testMembrSizeAlloc > 0) {
    if (((Node*) parent) -> testMembrIndx != NULL) {
      free_uivector(((Node*) parent) -> testMembrIndx, 1, ((Node*) parent) -> testMembrSizeAlloc);
      ((Node*) parent) -> testMembrIndx = NULL;
    }
  }
  deinitNodeBase((NodeBase*) parent);
  free_gblock(parent, (size_t) sizeof(Node));
}
