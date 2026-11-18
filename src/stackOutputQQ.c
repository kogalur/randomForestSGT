
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "stackOutput.h"
#include "terminal.h"
#include "shared/nrutil.h"
#include "shared/error.h"
#include "shared/nativeUtil.h"
#include "sexpOutgoing.h"
#include "shared/stackAuxiliaryInfo.h"
void stackTNQualitativeObjectsOutput(char mode) {
  ulong localSize;
  uint treeID;
  if (RF_optHigh & OPT_TERM_OUTG) {
    localSize = RF_totalTermCount;
    SG_rmbrTNodeCT_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_RMBR_TN_CT, localSize, 0, SG_sexpStringOutgoing, NULL, 1,
      (const AuxiliaryDimension[]) {
        [1] = {RF_AUX_DIM_FIXED, localSize}
      });
    SG_ombrTNodeCT_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_OMBR_TN_CT, localSize, 0, SG_sexpStringOutgoing, NULL, 1,
      (const AuxiliaryDimension[]) {
        [1] = {RF_AUX_DIM_FIXED, localSize}
      });
    SG_rmbrTNodeCT_ --;
    SG_ombrTNodeCT_ --;
    localSize = (ulong) RF_ntree * RF_bootstrapSize;
    SG_rmbrTNodeID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_RMBR_TN_ID,  localSize, 0, SG_sexpStringOutgoing, NULL, 1,
      (const AuxiliaryDimension[]) {
        [1] = {RF_AUX_DIM_FIXED, localSize}
      });
    localSize = 0;
    for (treeID = 1; treeID <= RF_ntree; treeID++) {
      localSize += RF_oobSize[treeID];
    }
    if (localSize > 0) {
      SG_ombrTNodeID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_OMBR_TN_ID,  localSize, 0, SG_sexpStringOutgoing, NULL, 1,
        (const AuxiliaryDimension[]) {
          [1] = {RF_AUX_DIM_FIXED, localSize}
        });
      SG_ombrTNodeID_ --;
    }
    else {
      localSize = 0;
      for (uint treeID = 1; treeID <= RF_ntree; treeID++) {
        localSize += RF_tLeafCount[treeID];
      }
      SG_ombrTNodeID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_OMBR_TN_ID,  localSize, 0, SG_sexpStringOutgoing, NULL, 1,
        (const AuxiliaryDimension[]) {
          [1] = {RF_AUX_DIM_FIXED, localSize}
        });
      SG_ombrTNodeID_ --;
    }
    localSize = (ulong) RF_ntree * RF_observationSize;
    SG_ambrTNodeID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_AMBR_TN_ID,  localSize, 0, SG_sexpStringOutgoing, &SG_ambrTNodeID_ptr, 2,
      (const AuxiliaryDimension[]) {
        [1] = {RF_AUX_DIM_FIXED, RF_ntree},
        [2] = {RF_AUX_DIM_FIXED, RF_observationSize}
      });
    SG_rmbrTNodeID_ --;
    SG_ambrTNodeID_ --;
  }
}
void stackTNQualitativeObjectsOutputTest(char mode) {
  ulong localSize;
  if (RF_optHigh & OPT_TERM_OUTG) {
    localSize = (ulong) RF_ntree * RF_fobservationSize;
    SG_ambrTNodeID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_AMBR_TN_ID,  localSize, 0, SG_sexpStringOutgoing, &SG_ambrTNodeID_ptr, 2,
      (const AuxiliaryDimension[]) {
        [1] = {RF_AUX_DIM_FIXED, RF_ntree},
        [2] = {RF_AUX_DIM_FIXED, RF_fobservationSize}
      });
    SG_ambrTNodeID_ --;
  }
}
void writeTNQualitativeObjectsOutput(char mode) {
  LeafLinkedObj **leafLinkedObjMap;
  LeafLinkedObj  *leafLinkedPtr;
  Terminal *termPtr;
  uint      nodeID;
  uint      genMembrSize;
  uint     *genMembrIndx;
  ulong offset, offsetCT, offsetID_rmbr, offsetID_ombr;
  uint treeID;
  uint i, k;
  if (RF_optHigh & OPT_TERM_OUTG) {
    offset = 0;
    offsetCT = 0;
    offsetID_rmbr = offsetID_ombr = 0;
    for (treeID = 1; treeID <= RF_ntree; treeID++) {
      leafLinkedObjMap = (LeafLinkedObj **) new_vvector(1, RF_tLeafCount[treeID], NRUTIL_LEAFPTR2);
      leafLinkedPtr = RF_leafLinkedObjHead[treeID] -> fwdLink;
      while (leafLinkedPtr != NULL) {
        leafLinkedObjMap[leafLinkedPtr -> nodeID] = leafLinkedPtr;
        leafLinkedPtr = leafLinkedPtr -> fwdLink;
      }
      for (k = 1; k <= RF_nodeCount[treeID]; k++) {
        offset ++;
        if (SG_brnodeID_[offset] == 0) {
          nodeID = SG_nodeID_[offset];
          termPtr = (Terminal *) (leafLinkedObjMap[nodeID] -> termPtr);
          genMembrSize = termPtr -> oobMembrCount;
          genMembrIndx = termPtr -> oobMembrIndx;
          for (i = 1; i <= genMembrSize; i++) {
            SG_ambrTNodeID_ptr[treeID][genMembrIndx[i]] = (uint) offset;
          }
          genMembrSize = termPtr -> ibgMembrCount;
          genMembrIndx = termPtr -> ibgMembrIndx;
          for (i = 1; i <= genMembrSize; i++) {
            SG_ambrTNodeID_ptr[treeID][genMembrIndx[i]] = (uint) offset;
          }
        }
      }
      for (k = 1; k <= RF_tLeafCount[treeID]; k++) {
        offsetCT ++;
        SG_rmbrTNodeCT_[offsetCT]  = SG_rmbrTNodeCT_ptr[treeID][k];
        SG_ombrTNodeCT_[offsetCT]  = SG_ombrTNodeCT_ptr[treeID][k];
      }
      for (k = 1; k <= RF_bootstrapSize; k++) {
        offsetID_rmbr ++;
        SG_rmbrTNodeID_[offsetID_rmbr]  = SG_rmbrTNodeID_ptr[treeID][k];
      }
      for (k = 1; k <= RF_oobSize[treeID]; k++) {
        offsetID_ombr ++;
        SG_ombrTNodeID_[offsetID_ombr]  = SG_ombrTNodeID_ptr[treeID][k];
      }
      free_new_vvector(leafLinkedObjMap, 1, RF_tLeafCount[treeID], NRUTIL_LEAFPTR2);
    }
    if (offsetCT != RF_totalTermCount) {
      RF_nativeError("\nRF-SRC:  *** ERROR *** ");
      RF_nativeError("\nRF-SRC:  Inconsistent total terminal count encountered during writing of forest topology.");
      RF_nativeError("\nRF-SRC:  Offset versus total was:  %20llu, %20llu", (unsigned long long) offsetCT, (unsigned long long) RF_totalTermCount);
      RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
      RF_nativeExit();
    }
  }     
}
void writeTNQualitativeObjectsOutputTest(char mode) {
  LeafLinkedObj **leafLinkedObjMap;
  LeafLinkedObj  *leafLinkedPtr;
  Terminal *termPtr;
  uint      nodeID;
  uint      genMembrSize;
  uint     *genMembrIndx;
  ulong offset;
  uint treeID;
  uint i, k;
  if (RF_optHigh & OPT_TERM_OUTG) {
    offset = 0;
    for (treeID = 1; treeID <= RF_ntree; treeID++) {
      if (RF_getTree != NULL && RF_getTree[treeID] == 0) {
        offset += RF_nodeCount[treeID];
        continue;
      }
      leafLinkedObjMap = (LeafLinkedObj **) new_vvector(1, RF_tLeafCount[treeID], NRUTIL_LEAFPTR2);
      leafLinkedPtr = RF_leafLinkedObjHead[treeID] -> fwdLink;
      while (leafLinkedPtr != NULL) {
        leafLinkedObjMap[leafLinkedPtr -> nodeID] = leafLinkedPtr;
        leafLinkedPtr = leafLinkedPtr -> fwdLink;
      }
      for (k = 1; k <= RF_nodeCount[treeID]; k++) {
        offset ++;
        if (SG_brnodeID_[offset] == 0) {
          nodeID = SG_nodeID_[offset];
          termPtr = (Terminal *) (leafLinkedObjMap[nodeID] -> termPtr);
          genMembrSize = termPtr -> tstMembrCount;
          genMembrIndx = termPtr -> tstMembrIndx;
          for (i = 1; i <= genMembrSize; i++) {
            SG_ambrTNodeID_ptr[treeID][genMembrIndx[i]] = (uint) offset;
          }
        }
      }
      free_new_vvector(leafLinkedObjMap, 1, RF_tLeafCount[treeID], NRUTIL_LEAFPTR2);
    }
  }     
}
void stackTNQualitativeObjectsForestPtr(char mode) {
  uint treeID;
  if (RF_optHigh & OPT_TERM_OUTG) {
    SG_rmbrTNodeCT_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
    SG_ombrTNodeCT_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
    for (treeID = 1; treeID <= RF_ntree; treeID++) {
      SG_rmbrTNodeCT_ptr[treeID] = NULL;
      SG_ombrTNodeCT_ptr[treeID] = NULL;
    }
    SG_rmbrTNodeID_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
    SG_ombrTNodeID_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
    for (treeID = 1; treeID <= RF_ntree; treeID++) {
      SG_rmbrTNodeID_ptr[treeID] = NULL;
      SG_ombrTNodeID_ptr[treeID] = NULL;
    }
  }
}
void stackTNQualitativeObjectsTreePtr(char mode, uint treeID) {
  uint termCount;
  uint rmbrCount, ombrCount;
  if (RF_optHigh & OPT_TERM_OUTG) {  
    termCount = RF_tLeafCount[treeID];
    SG_rmbrTNodeCT_ptr[treeID] = uivector(1, termCount);
    SG_ombrTNodeCT_ptr[treeID] = uivector(1, termCount);
    rmbrCount = RF_bootstrapSize;
    ombrCount = RF_oobSize[treeID];
    SG_rmbrTNodeID_ptr[treeID] = uivector(1, rmbrCount);
    SG_ombrTNodeID_ptr[treeID] = uivector(1, ombrCount + 1);  
  }
}
void unstackTNQualitativeObjectsForestPtr(char mode) {
  uint treeID;
  uint termCount;
  uint rmbrCount, ombrCount;
  if (RF_optHigh & OPT_TERM_OUTG) {
    for (treeID = 1; treeID <= RF_ntree; treeID++) {
      termCount = RF_tLeafCount[treeID];  
      rmbrCount = RF_bootstrapSize;
      ombrCount = RF_oobSize[treeID];
      free_uivector(SG_rmbrTNodeCT_ptr[treeID], 1, termCount);
      free_uivector(SG_ombrTNodeCT_ptr[treeID], 1, termCount);
      free_uivector(SG_rmbrTNodeID_ptr[treeID], 1, rmbrCount);
      free_uivector(SG_ombrTNodeID_ptr[treeID], 1, ombrCount + 1);  
    }
    free_new_vvector(SG_rmbrTNodeCT_ptr, 1, RF_ntree, NRUTIL_UPTR);
    free_new_vvector(SG_ombrTNodeCT_ptr, 1, RF_ntree, NRUTIL_UPTR);
    free_new_vvector(SG_rmbrTNodeID_ptr, 1, RF_ntree, NRUTIL_UPTR);
    free_new_vvector(SG_ombrTNodeID_ptr, 1, RF_ntree, NRUTIL_UPTR);
  }
}
