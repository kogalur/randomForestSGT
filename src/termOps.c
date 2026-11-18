
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "termOps.h"
#include "node.h"
#include "augmentationOpsCommon.h"
#include "augmentationOpsCustom.h"
#include "augmentationOpsSimple.h"
#include "augmentationOps.h"
#include "cdlRegr.h"
#include "cdlUtil.h"
#include "cdlInfo.h"
#include "regressionDerived.h"
#include "shared/termBaseOps.h"
#include "shared/nrutil.h"
#include "terminal.h"
#include "shared/error.h"
void *makeTerminalDerived(void) {
  Terminal *parent = (Terminal*) gblock((size_t) sizeof(Terminal));
  preInitTerminalBase((TerminalBase *) parent);
  parent -> mean = 0.0;
  parent -> repMembrCount = 0;
  parent -> oobMembrCount = 0;
  parent -> ibgMembrCount = 0;
  parent -> tstMembrCount = 0;
  parent -> allMembrCount = 0;
  parent -> repMembrIndx  = NULL;
  parent -> ibgMembrIndx  = NULL;
  parent -> oobMembrIndx  = NULL;
  parent -> tstMembrIndx  = NULL;
  return parent;
}
void freeTerminalDerived(void *parent) {
  deinitTerminalBase((TerminalBase*) parent);
  if (((Terminal *) parent) -> allMembrCount > 0) {
    free_uivector(((Terminal *) parent) -> oobMembrIndx,  1, ((Terminal *) parent) -> allMembrCount);
    free_uivector(((Terminal *) parent) -> repMembrIndx,  1, ((Terminal *) parent) -> repMembrCount);
    free_uivector(((Terminal *) parent) -> ibgMembrIndx,  1, ((Terminal *) parent) -> repMembrCount);
  }
  if (((Terminal *) parent) -> tstMembrCount > 0) {
    free_uivector(((Terminal *) parent) -> tstMembrIndx,  1, ((Terminal *) parent) -> tstMembrCount);
  }
  free_gblock(parent, (size_t) sizeof(Terminal));
}
void restoreTerminalNodeOutcomesCDL(uint treeID, Terminal *parent) {
if ((RF_opt & OPT_PERF) ||
    (RF_opt & OPT_OENS) ||
    (RF_opt & OPT_IENS) ||
    (RF_opt & OPT_FENS)) {
  if (RF_rFactorCount > 0) {
  }
  if (RF_rNonFactorCount > 0) {
    restoreMeanResponseCDL(treeID, parent);
  }
 }
}
