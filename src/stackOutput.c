
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
void stackCDLOutput(uint       lambdaCountIn,
                    uint       p,
                    uint      *lambdaCount,
                    double   **lambda,
                    uint     **convergenceCount,
                    double  ***beta) {
  uint i;
  *lambdaCount = lambdaCountIn;
  *lambda = dvector(1, lambdaCountIn);
  *convergenceCount = uivector(1, lambdaCountIn);
  *beta = (double **) new_vvector(1, lambdaCountIn, NRUTIL_DPTR);
  for (i = 1; i <= lambdaCountIn; i++) {
    (*beta)[i] = dvector(1, p + 1);
  }
}
void unstackCDLOutput(uint      lambdaCountIn,
                      uint      p,
                      double   *lambda,
                      uint     *convergenceCount,
                      double  **beta) {
  uint i;
  free_dvector(lambda, 1, lambdaCountIn);
  free_uivector(convergenceCount, 1, lambdaCountIn);
  for (i = 1; i <= lambdaCountIn; i++) {
    free_dvector(beta[i], 1, p + 1);
  }
  free_new_vvector(beta, 1, lambdaCountIn, NRUTIL_DPTR);
}
void stackCoordinateDescentOutput(CDLInfo  *obj,
                                  uint    **lambdaCount,
                                  double  **lambda,
                                  uint    **convergenceCount,
                                  double   **beta,
                                  double  ***betaPtr,
                                  double   **yHat,
                                  double  ***yHatPtr,
                                  uint     **lambdaMinIndx,
                                  uint     **lambda1SEIndx,
                                  double   **lambdaMeanErr,
                                  double   **lambdaCVSD) {
  uint lambdaGridSizeThinned;
  uint yHatCnt;
  uint localRecordCount;
  lambdaGridSizeThinned = obj -> lambdaCnt;
  yHatCnt = obj -> yHatCnt;
  *lambdaCount = (uint*) stackAndProtect(RF_auxDimConsts,
                                         RF_GROW,
                                         &RF_nativeIndex,
                                         NATIVE_TYPE_INTEGER,
                                         SG_LAMBDA_CT,
                                         1,
                                         0,
                                         SG_sexpStringOutgoingCDL,
                                         NULL,
                                         1,
                                         1);
  (*lambdaCount) --;
  localRecordCount = lambdaGridSizeThinned;
  *lambda = (double*) stackAndProtect(RF_auxDimConsts,
                                      RF_GROW,
                                      &RF_nativeIndex,
                                      NATIVE_TYPE_NUMERIC,
                                      SG_LAMBDA,
                                      localRecordCount,
                                      0,
                                      SG_sexpStringOutgoingCDL,
                                      NULL,
                                      1,
                                      localRecordCount);
  (*lambda) --;
  localRecordCount = lambdaGridSizeThinned;
  *convergenceCount = (uint*) stackAndProtect(RF_auxDimConsts,
                                              RF_GROW,
                                              &RF_nativeIndex,
                                              NATIVE_TYPE_INTEGER,
                                              SG_CONVG_CT,
                                              localRecordCount,
                                              0,
                                              SG_sexpStringOutgoingCDL,
                                              NULL,
                                              1,
                                              localRecordCount);
  (*convergenceCount) --;
  localRecordCount = lambdaGridSizeThinned * (RF_xSize + 1);
  *beta = (double*) stackAndProtect(RF_auxDimConsts,
                                    RF_GROW,
                                    &RF_nativeIndex,
                                    NATIVE_TYPE_NUMERIC,
                                    SG_BETA,
                                    localRecordCount,
                                    0,
                                    SG_sexpStringOutgoingCDL,
                                    betaPtr,
                                    2,
                                    lambdaGridSizeThinned,
                                    RF_xSize + 1);
  if (yHatCnt > 0) {
    localRecordCount = lambdaGridSizeThinned * yHatCnt;
    *yHat = (double*) stackAndProtect(RF_auxDimConsts,
                                      RF_GROW,
                                      &RF_nativeIndex,
                                      NATIVE_TYPE_NUMERIC,
                                      SG_YHAT,
                                      localRecordCount,
                                      0,
                                      SG_sexpStringOutgoingCDL,
                                      yHatPtr,
                                      2,
                                      lambdaGridSizeThinned,
                                      yHatCnt);
    *lambdaMinIndx = (uint*) stackAndProtect(RF_auxDimConsts,
                                             RF_GROW,
                                             &RF_nativeIndex,
                                             NATIVE_TYPE_INTEGER,
                                             SG_LAMBDA_MIN,
                                             1,
                                             0,
                                             SG_sexpStringOutgoingCDL,
                                             NULL,
                                             1,
                                             1);
    (*lambdaMinIndx) --;
    *lambda1SEIndx = (uint*) stackAndProtect(RF_auxDimConsts,
                                             RF_GROW,
                                             &RF_nativeIndex,
                                             NATIVE_TYPE_INTEGER,
                                             SG_LAMBDA_1SE,
                                             1,
                                             0,
                                             SG_sexpStringOutgoingCDL,
                                             NULL,
                                             1,
                                             1);
    (*lambda1SEIndx) --;
    localRecordCount = lambdaGridSizeThinned;
    *lambdaMeanErr = (double*) stackAndProtect(RF_auxDimConsts,
                                               RF_GROW,
                                               &RF_nativeIndex,
                                               NATIVE_TYPE_NUMERIC,
                                               SG_LAMBDA_ERR,
                                               localRecordCount,
                                               0,
                                               SG_sexpStringOutgoingCDL,
                                               NULL,
                                               1,
                                               localRecordCount);
    (*lambdaMeanErr) --;
    localRecordCount = lambdaGridSizeThinned;
    *lambdaCVSD = (double*) stackAndProtect(RF_auxDimConsts,
                                            RF_GROW,
                                            &RF_nativeIndex,
                                            NATIVE_TYPE_NUMERIC,
                                            SG_LAMBDA_CVSD,
                                            localRecordCount,
                                            0,
                                            SG_sexpStringOutgoingCDL,
                                            NULL,
                                            1,
                                            localRecordCount);
    (*lambdaCVSD) --;
  }
}
void stackDefinedOutputObjects(char mode) {
  uint  sexpIdentity;
  uint localSize;
  uint  obsSize;
  double **ensembleDen;
  double   **ensembleCLS;
  double ****ensembleCLSptr;
  double ****ensembleCLSnum;
  double   **ensembleRGR;
  double  ***ensembleRGRptr;
  double  ***ensembleRGRnum;
  char oobFlag, fullFlag;
  uint i, j, k;
  sexpIdentity = 0;
  if ((TRUE) || (mode == RF_GROW)) {
    if (RF_opt & OPT_LEAF) {
      RF_stackCount += 1;
    }
    if ((RF_opt & OPT_IENS) || (RF_opt & OPT_FENS)) {
      if (RF_rTargetFactorCount > 0) {
        RF_stackCount += 1;
      }
      if (RF_rTargetNonFactorCount > 0) {
        RF_stackCount += 1;
      }
    }
    if (RF_opt & OPT_OENS) {
      if (RF_rTargetFactorCount > 0) {
        RF_stackCount += 1;
      }
      if (RF_rTargetNonFactorCount > 0) {
        RF_stackCount += 1;
      }
    }
    if (RF_opt & OPT_PERF) {
      if (RF_rTargetFactorCount > 0) {
        RF_stackCount += 1;
      }
      if (RF_rTargetNonFactorCount > 0) {
        RF_stackCount += 1;
      }
    }
    if (RF_opt & OPT_TREE) {
      if (RF_opt & OPT_SEED) {
        RF_stackCount += 1;
      }
      if ((SG_hcut == 0) || (SG_hcut >= 1)) {
        RF_stackCount += 1;
        RF_stackCount += 14;
      }
    }
    if (RF_opt & OPT_EMPR_RISK) {
      RF_stackCount += 2;
    }
    if (RF_optHigh & OPT_MEMB_USER) {
      RF_stackCount += 2;
    }
    obsSize = RF_observationSize;
  }  
  RF_stackCount +=1;
  initProtect(RF_stackCount);
  stackAuxiliaryInfoList(&RF_snpAuxiliaryInfoList, RF_stackCount);
  oobFlag = fullFlag = FALSE;
  if ((RF_opt & OPT_OENS) || (RF_opt & OPT_IENS) || (RF_opt & OPT_FENS)) {
    SG_yHatAbsolute = (double **) new_vvector(1, RF_ntree, NRUTIL_DPTR);
    if (mode == RF_PRED) {
      SG_yHatAbsoluteTest = (double **) new_vvector(1, RF_ntree, NRUTIL_DPTR);
    }
    switch (mode) {
    case RF_PRED:
      if (RF_opt & OPT_FENS) {
        fullFlag = TRUE;
      }
      break;
    default:
      if (RF_opt & OPT_OENS) {
        oobFlag = TRUE;
      }
      if (RF_opt & OPT_IENS) {
        fullFlag = TRUE;
      }
      break;
    }
    while ((oobFlag == TRUE) || (fullFlag == TRUE)) {
      ensembleDen    = NULL;
      ensembleCLS    = NULL;
      ensembleCLSptr = NULL;
      ensembleCLSnum = NULL;
      ensembleRGR    = NULL;
      ensembleRGRptr = NULL;
      ensembleRGRnum = NULL;
      if (oobFlag == TRUE) {
        obsSize = RF_observationSize;        
        ensembleDen     = &RF_oobEnsembleDen;
        ensembleCLS    = &RF_oobEnsembleCLS_;
        ensembleCLSptr = &RF_oobEnsembleCLSptr;
        ensembleCLSnum = &RF_oobEnsembleCLSnum;
        ensembleRGR    = &SG_cdl_oobEnsembleRGR_;
        ensembleRGRptr = &SG_cdl_oobEnsembleRGRptr;
        ensembleRGRnum = &SG_cdl_oobEnsembleRGRnum;
      }
      else {
        obsSize = (mode == RF_PRED) ? RF_fobservationSize : RF_observationSize;
        ensembleDen    = &RF_fullEnsembleDen;
        ensembleCLS    = &RF_fullEnsembleCLS_;
        ensembleCLSptr = &RF_fullEnsembleCLSptr;
        ensembleCLSnum = &RF_fullEnsembleCLSnum;
        ensembleRGR    = &SG_cdl_fullEnsembleRGR_;
        ensembleRGRptr = &SG_cdl_fullEnsembleRGRptr;
        ensembleRGRnum = &SG_cdl_fullEnsembleRGRnum;
      }
      *ensembleDen = dvector(1, obsSize);
      for (i = 1; i <= obsSize; i++) {
        (*ensembleDen)[i] = 0;
      }
      if (RF_rTargetFactorCount > 0) {
        (oobFlag == TRUE) ? (sexpIdentity = SG_OCLS_ID) : ((fullFlag == TRUE) ? sexpIdentity = SG_ACLS_ID: TRUE);
        localSize = 0;
        for (j = 1; j <= RF_rTargetFactorCount; j++) {
          for (k = 1; k <= RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]; k++) {
            localSize += obsSize;
          }
        }
        *ensembleCLS = (double*) stackAndProtect(RF_auxDimConsts,
                                                 mode,
                                                 &RF_nativeIndex,
                                                 NATIVE_TYPE_NUMERIC,
                                                 sexpIdentity,
                                                 localSize,
                                                 0,
                                                 SG_sexpStringOutgoing,
                                                 ensembleCLSptr,
                                                 3,
                                                 RF_rTargetFactorCount,
                                                 0,
                                                 obsSize);
        *ensembleCLSnum = (double ***) new_vvector(1, RF_rTargetFactorCount, NRUTIL_DPTR2);
        localSize = 0;
        for (j = 1; j <= RF_rTargetFactorCount; j++) {
          (*ensembleCLSnum)[j] = (double **) new_vvector(1, RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]], NRUTIL_DPTR);
          for (k = 1; k <= RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]; k++) {
            (*ensembleCLSnum)[j][k]  = dvector(1, obsSize);
            localSize += obsSize;
            for (i = 1; i <= obsSize; i++) {
              (*ensembleCLSnum)[j][k][i] = 0.0;
            }
          }
        }
      }
      if (RF_rTargetNonFactorCount > 0) {
        (oobFlag == TRUE) ? (sexpIdentity = SG_ORGR_ID) : ((fullFlag == TRUE) ? sexpIdentity = SG_ARGR_ID: TRUE);
        localSize = RF_rTargetNonFactorCount * obsSize;
        *ensembleRGR = (double*) stackAndProtect(RF_auxDimConsts,
                                                 mode,
                                                 &RF_nativeIndex,
                                                 NATIVE_TYPE_NUMERIC,
                                                 sexpIdentity,
                                                 localSize,
                                                 0,
                                                 SG_sexpStringOutgoing,
                                                 ensembleRGRptr,
                                                 2,
                                                 RF_rTargetNonFactorCount,
                                                 obsSize);
        (*ensembleRGRnum) = (double **) new_vvector(1, RF_rTargetNonFactorCount, NRUTIL_DPTR);
        for (j = 1; j <= RF_rTargetNonFactorCount; j++) {
          (*ensembleRGRnum)[j] = dvector(1, obsSize);
          for (i = 1; i <= obsSize; i++) {
            (*ensembleRGRnum)[j][i] = 0.0;
          }
        }
      }
      if (oobFlag == TRUE) {
        oobFlag = FALSE;
      }
      else {
        fullFlag = FALSE;
      }
    }  
  }
  if (RF_opt & OPT_PERF) {
    if (RF_rTargetFactorCount > 0) {
      localSize = 0;
      for (j = 1; j <= RF_rTargetFactorCount; j++) {
        for (k = 1; k <= 1 + RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]; k++) {
          localSize += RF_ntree;
        }
      }
      RF_perfCLS_ = (double*) stackAndProtect(RF_auxDimConsts,
                                              mode,
                                              &RF_nativeIndex,
                                              NATIVE_TYPE_NUMERIC,
                                              SG_ER_CLAS,
                                              localSize,
                                              RF_nativeNaN,
                                              SG_sexpStringOutgoing,
                                              &RF_perfCLSptr,
                                              3,
                                              RF_ntree,
                                              RF_rTargetFactorCount,
                                              -1);
    }
    if (RF_rTargetNonFactorCount > 0) {
      localSize = RF_ntree * RF_rTargetNonFactorCount;
      RF_perfRGR_ = (double*) stackAndProtect(RF_auxDimConsts,
                                              mode,
                                              &RF_nativeIndex,
                                              NATIVE_TYPE_NUMERIC,
                                              SG_ER_REGR,
                                              localSize,
                                              RF_nativeNaN,
                                              SG_sexpStringOutgoing,
                                              &RF_perfRGRptr,
                                              2,
                                              RF_ntree,
                                              RF_rTargetNonFactorCount);
    }
  }  
  if (RF_opt & OPT_LEAF) {
    localSize = RF_ntree;
    RF_tLeafCount_ = (uint*) stackAndProtect(RF_auxDimConsts,
                                             mode,
                                             &RF_nativeIndex,
                                             NATIVE_TYPE_INTEGER,
                                             SG_LEAF_CT,
                                             localSize,
                                             0,
                                             SG_sexpStringOutgoing,
                                             NULL,
                                             1,
                                             localSize);
    RF_tLeafCount_ --;
    if (mode == RF_GROW) {
      RF_tLeafCount = RF_tLeafCount_;
    }
    else {
      for (i = 1; i <= RF_ntree; i++) {
        RF_tLeafCount_[i] = RF_tLeafCount[i];
      }
    }
  }
  if (RF_opt & OPT_SEED) {
    localSize = RF_ntree;
    RF_seed_ = (int*) stackAndProtect(RF_auxDimConsts,
                                      mode,
                                      &RF_nativeIndex,
                                      NATIVE_TYPE_INTEGER,
                                      SG_SEED_ID,
                                      localSize,
                                      0,
                                      SG_sexpStringOutgoing,
                                      NULL,
                                      1,
                                      localSize);
    RF_seed_ --;
    for (i = 1; i <= RF_ntree; i++) {
      RF_seed_[i] = -1;
    }
  }
  if (RF_opt & OPT_TREE) {
    RF_optLoGrow_ = (uint*) stackAndProtect(RF_auxDimConsts,
                                            mode,
                                            &RF_nativeIndex,
                                            NATIVE_TYPE_INTEGER,
                                            SG_OPT_LO_GROW,
                                            1,
                                            0,
                                            SG_sexpStringOutgoing,
                                            NULL,
                                            1,
                                            1);
    RF_optLoGrow_ --;
    RF_optLoGrow_[1] = RF_optLoGrow = RF_opt;
  }
  if (RF_opt & OPT_EMPR_RISK) {
    localSize = RF_ntree * SG_lotSize;
    SG_emprRSK_      = (double*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_NUMERIC, SG_EMP_RSK,  localSize, RF_nativeNaN, SG_sexpStringOutgoing, &SG_emprRSKptr,    2, RF_ntree, SG_lotSize);
    SG_oobEmprRSK_   = (double*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_NUMERIC, SG_OEMP_RSK, localSize, RF_nativeNaN, SG_sexpStringOutgoing, &SG_oobEmprRSKptr, 2, RF_ntree, SG_lotSize);
  }
  if (RF_optHigh & OPT_MEMB_USER) {
    localSize = RF_ntree * obsSize;
    SG_MEMB_ID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_MEMB_ID, localSize, 0, SG_sexpStringOutgoing, &SG_MEMB_ID_ptr, 2, RF_ntree, obsSize);
    localSize = RF_ntree * obsSize;
    SG_BOOT_CT_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_BOOT_CT, localSize, 0, SG_sexpStringOutgoing, &SG_BOOT_CT_ptr, 2, RF_ntree, obsSize);
  }
  SG_cpuTime_ = (double*) stackAndProtect(RF_auxDimConsts,
                                          mode,
                                          &RF_nativeIndex,
                                          NATIVE_TYPE_NUMERIC,
                                          SG_CPU_TIME,
                                          1,
                                          0,
                                          SG_sexpStringOutgoing,
                                          NULL,
                                          1,
                                          1);
  SG_cpuTime_ --;
}
void unstackDefinedOutputObjects(char mode) {
  uint obsSize;
  char oobFlag, fullFlag;
  double **ensembleDen;
  double ****ensembleCLSnum;
  double  ***ensembleRGRnum;
  uint j, k;
  oobFlag = fullFlag = FALSE;
  if ((RF_opt & OPT_OENS) || (RF_opt & OPT_IENS) || (RF_opt & OPT_FENS)) {
    free_new_vvector(SG_yHatAbsolute, 1, RF_ntree, NRUTIL_DPTR);
    if (mode == RF_PRED) {
      free_new_vvector(SG_yHatAbsoluteTest, 1, RF_ntree, NRUTIL_DPTR);
    }
    switch (mode) {
    case RF_PRED:
      if (RF_opt & OPT_FENS) {
        fullFlag = TRUE;
      }
      break;
    default:
      if (RF_opt & OPT_OENS) {
        oobFlag = TRUE;
      }
      if (RF_opt & OPT_IENS) {
        fullFlag = TRUE;
      }
      break;
    }
    while ((oobFlag == TRUE) || (fullFlag == TRUE)) {
      if (oobFlag == TRUE) {
        obsSize = RF_observationSize;
        ensembleDen    = &RF_oobEnsembleDen;
        ensembleCLSnum = &RF_oobEnsembleCLSnum;
        ensembleRGRnum = &SG_cdl_oobEnsembleRGRnum;
      }
      else {
        obsSize = (mode == RF_PRED) ? RF_fobservationSize : RF_observationSize;
        ensembleDen    = &RF_fullEnsembleDen;
        ensembleCLSnum = &RF_fullEnsembleCLSnum;
        ensembleRGRnum = &SG_cdl_fullEnsembleRGRnum;
      }
      free_dvector(*ensembleDen, 1, obsSize);
      if (RF_rTargetFactorCount > 0) {
        for (j = 1; j <= RF_rTargetFactorCount; j++) {
          for (k = 1; k <= RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]]; k++) {
            free_dvector((*ensembleCLSnum)[j][k], 1, obsSize);
          }
          free_new_vvector((*ensembleCLSnum)[j], 1, RF_rFactorSize[RF_rFactorMap[RF_rTargetFactor[j]]], NRUTIL_DPTR);
        }
        free_new_vvector(*ensembleCLSnum, 1, RF_rTargetFactorCount, NRUTIL_DPTR2);
      }
      if (RF_rTargetNonFactorCount > 0) {
        for (j = 1; j <= RF_rTargetNonFactorCount; j++) {
          free_dvector((*ensembleRGRnum)[j], 1, obsSize);
        }
        free_new_vvector((*ensembleRGRnum), 1, RF_rTargetNonFactorCount, NRUTIL_DPTR);        
      }
      if (oobFlag == TRUE) {
        oobFlag = FALSE;
      }
      else {
        fullFlag = FALSE;
      }
    }  
  }
}
void stackForestObjectsPtrOnly(char mode) {
  uint i, j;
      SG_treeID_ptr     = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      SG_nodeID_ptr     = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      SG_nodeSZ_ptr     = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      SG_brnodeID_ptr   = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      SG_yBar_ptr     = (double **)  new_vvector(1, RF_ntree, NRUTIL_DPTR);
      SG_yStar_ptr     = (double **)  new_vvector(1, RF_ntree, NRUTIL_DPTR);
      SG_betaZ_ptr     = (double **)  new_vvector(1, RF_ntree, NRUTIL_DPTR);
      SG_betaP_ptr     = (double ***) new_vvector(1, RF_ntree, NRUTIL_DPTR2);
      SG_nodeStat_ptr   = (double **) new_vvector(1, RF_ntree, NRUTIL_DPTR);
      SG_bsf_ptr   = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      if ((SG_hcut == 0) || (SG_hcut >= 1)) {
        for (i = 1; i <= RF_ntree; i++) {
          SG_betaP_ptr[i]  = (double **) new_vvector(1, SG_augmObjCommon -> hcutCnt, NRUTIL_DPTR);
        }
        for (i = 1; i <= RF_ntree; i++) {
          SG_yBar_ptr[i]  = NULL;
          SG_yStar_ptr[i]  = NULL;
          SG_betaZ_ptr[i] = NULL;
          for (j = 1; j <= SG_augmObjCommon -> hcutCnt; j++) {
            SG_betaP_ptr[i][j]  = NULL;
          }
        }
      }
      SG_rmbrTNodeCT_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      SG_ombrTNodeCT_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      for (i = 1; i <= RF_ntree; i++) {
        SG_rmbrTNodeCT_ptr[i] = NULL;
        SG_ombrTNodeCT_ptr[i] = NULL;
      }
      SG_rmbrTNodeID_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      SG_ombrTNodeID_ptr = (uint **) new_vvector(1, RF_ntree, NRUTIL_UPTR);
      for (i = 1; i <= RF_ntree; i++) {
        SG_rmbrTNodeID_ptr[i] = NULL;
        SG_ombrTNodeID_ptr[i] = NULL;
      }
}
void unstackForestObjectsPtrOnly(char mode) {
  uint treeID;
  uint i;
      for (treeID = 1; treeID <= RF_ntree; treeID++) {
        unstackTreeObjectsPtrOnly(treeID);
      }
      if ((SG_hcut == 0) || (SG_hcut >= 1)) {
        for (i = 1; i <= RF_ntree; i++) {
          free_new_vvector(SG_betaP_ptr[i], 1, SG_augmObjCommon -> hcutCnt, NRUTIL_DPTR);
        }
      }
      free_new_vvector(SG_treeID_ptr,     1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_nodeID_ptr,     1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_nodeSZ_ptr,     1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_brnodeID_ptr,   1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_nodeStat_ptr,   1, RF_ntree, NRUTIL_DPTR);
      free_new_vvector(SG_bsf_ptr,        1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_yBar_ptr,    1,  RF_ntree, NRUTIL_DPTR);
      free_new_vvector(SG_yStar_ptr,   1,  RF_ntree, NRUTIL_DPTR);      
      free_new_vvector(SG_betaZ_ptr,   1,  RF_ntree, NRUTIL_DPTR);
      free_new_vvector(SG_betaP_ptr,   1,  RF_ntree, NRUTIL_DPTR2);
      free_new_vvector(SG_rmbrTNodeCT_ptr, 1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_ombrTNodeCT_ptr, 1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_rmbrTNodeID_ptr, 1, RF_ntree, NRUTIL_UPTR);
      free_new_vvector(SG_ombrTNodeID_ptr, 1, RF_ntree, NRUTIL_UPTR);
}
void stackTreeObjectsPtrOnly(char mode, uint treeID) {
  uint nodeCount, termCount;
  uint rmbrCount, ombrCount;
  uint j;
    nodeCount = RF_nodeCount[treeID];
    termCount = RF_tLeafCount[treeID];
    rmbrCount = RF_bootstrapSize;
    ombrCount = RF_oobSize[treeID];
    SG_treeID_ptr[treeID]     = uivector(1, nodeCount);
    SG_nodeID_ptr[treeID]     = uivector(1, nodeCount);
    SG_nodeSZ_ptr[treeID]     = uivector(1, nodeCount);
    SG_brnodeID_ptr[treeID]   = uivector(1, nodeCount);
    SG_nodeStat_ptr[treeID]   =   dvector(1, nodeCount);
    SG_bsf_ptr[treeID]        =  uivector(1, nodeCount);
    if ((SG_hcut == 0) || (SG_hcut >= 1)) {
      SG_yBar_ptr[treeID]  = dvector(1, nodeCount);
      SG_yStar_ptr[treeID] = dvector(1, nodeCount);        
      SG_betaZ_ptr[treeID] = dvector(1, nodeCount);
      for (j = 1; j <= SG_augmObjCommon -> hcutCnt; j++) {
        SG_betaP_ptr[treeID][j] = dvector(1, nodeCount);
      }
    }
    SG_rmbrTNodeCT_ptr[treeID] = uivector(1, termCount);
    SG_ombrTNodeCT_ptr[treeID] = uivector(1, termCount);
    SG_rmbrTNodeID_ptr[treeID] = uivector(1, rmbrCount);
    SG_ombrTNodeID_ptr[treeID] = uivector(1, ombrCount + 1);  
}
void unstackTreeObjectsPtrOnly(uint treeID) {
  uint nodeCount, termCount;
  uint rmbrCount, ombrCount;
  uint j;
  nodeCount = RF_nodeCount[treeID];
  termCount = RF_tLeafCount[treeID];  
  rmbrCount = RF_bootstrapSize;
  ombrCount = RF_oobSize[treeID];
  free_uivector(SG_treeID_ptr[treeID],     1, nodeCount);
  free_uivector(SG_nodeID_ptr[treeID],     1, nodeCount);
  free_uivector(SG_nodeSZ_ptr[treeID],     1, nodeCount);
  free_uivector(SG_brnodeID_ptr[treeID],   1, nodeCount);
  free_dvector(SG_nodeStat_ptr[treeID],    1, nodeCount);
  free_uivector(SG_bsf_ptr[treeID],        1, nodeCount);
  if ((SG_hcut == 0) || (SG_hcut >= 1)) {
    free_dvector(SG_yBar_ptr[treeID],  1, nodeCount);
    free_dvector(SG_yStar_ptr[treeID], 1, nodeCount);    
    free_dvector(SG_betaZ_ptr[treeID], 1, nodeCount);
    for (j = 1; j <= SG_augmObjCommon -> hcutCnt; j++) {
      free_dvector(SG_betaP_ptr[treeID][j], 1, nodeCount);
    }
  }
  free_uivector(SG_rmbrTNodeCT_ptr[treeID], 1, termCount);
  free_uivector(SG_ombrTNodeCT_ptr[treeID], 1, termCount);
  free_uivector(SG_rmbrTNodeID_ptr[treeID], 1, rmbrCount);
  free_uivector(SG_ombrTNodeID_ptr[treeID], 1, ombrCount + 1);  
}
void stackForestObjectsOutput(char mode) {
  uint totalNodeCount;
  uint localSize;
  uint treeID;
      RF_totalNodeCount = 0;
      RF_totalTermCount = 0;
      for (treeID = 1; treeID <= RF_ntree; treeID++) {
        RF_totalNodeCount += RF_nodeCount[treeID];
        RF_totalTermCount += RF_tLeafCount[treeID];
      }
      totalNodeCount = RF_totalNodeCount;
      SG_treeID_   =   (uint*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_TREE_ID, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_nodeID_   =   (uint*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_NODE_ID, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_nodeSZ_   =   (uint*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_NODE_SZ, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_brnodeID_ =   (uint*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_BR_NODE_ID, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_treeID_   --;
      SG_nodeID_   --;
      SG_nodeSZ_   --;
      SG_brnodeID_ --;
      SG_nodeStat_ = (double*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_NUMERIC, SG_NODE_STAT, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_nodeStat_ --;
      SG_bsf_ = (uint*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_BSF_ORD, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_bsf_ --;
      SG_betaZ_    = (double*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_NUMERIC, SG_BETA_Z, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_betaZ_    --;
      SG_yBar_    = (double*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_NUMERIC, SG_YBAR, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_yBar_    --;
      SG_yStar_    = (double*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_NUMERIC, SG_YSTAR, totalNodeCount, 0, SG_sexpStringOutgoing, NULL, 1, totalNodeCount);
      SG_yStar_    --;
      localSize = SG_augmObjCommon -> hcutCnt * totalNodeCount;
      SG_betaP_ =   (double*)  stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_NUMERIC, SG_BETA_P, localSize, 0, SG_sexpStringOutgoing, &SG_BETA_P_ptr, 2, SG_augmObjCommon -> hcutCnt, totalNodeCount);
      localSize = RF_totalTermCount;
      SG_rmbrTNodeCT_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_RMBR_TN_CT, localSize, 0, SG_sexpStringOutgoing, NULL, 1, localSize);
      SG_ombrTNodeCT_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_OMBR_TN_CT, localSize, 0, SG_sexpStringOutgoing, NULL, 1, localSize);
      SG_rmbrTNodeCT_ --;
      SG_ombrTNodeCT_ --;
      localSize = (ulong) RF_ntree * RF_bootstrapSize;
      SG_rmbrTNodeID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_RMBR_TN_ID,  localSize, 0, SG_sexpStringOutgoing, NULL, 1, localSize);
      localSize = 0;
      for (treeID = 1; treeID <= RF_ntree; treeID++) {
        localSize += RF_oobSize[treeID];
      }
      SG_ombrTNodeID_ = (uint*) stackAndProtect(RF_auxDimConsts, mode, &RF_nativeIndex, NATIVE_TYPE_INTEGER, SG_OMBR_TN_ID,  localSize, 0, SG_sexpStringOutgoing, NULL, 1, localSize);
      SG_rmbrTNodeID_ --;
      SG_ombrTNodeID_ --;
}
void writeForestObjectsOutput(char mode) {
  uint offset, offsetCT, offsetID_rmbr, offsetID_ombr;
  uint treeID;
  uint j, k;
      offset = 0;
      offsetCT = 0;
      offsetID_rmbr = offsetID_ombr = 0;
      for (treeID = 1; treeID <= RF_ntree; treeID++) {
        for (k = 1; k <= RF_nodeCount[treeID]; k++) {
          offset ++;
          SG_treeID_[offset]   = SG_treeID_ptr[treeID][k];
          SG_nodeID_[offset]   = SG_nodeID_ptr[treeID][k];
          SG_nodeSZ_[offset]   = SG_nodeSZ_ptr[treeID][k];
          SG_brnodeID_[offset] = SG_brnodeID_ptr[treeID][k];
          SG_nodeStat_[offset] = SG_nodeStat_ptr[treeID][k];
          SG_bsf_[offset]      = SG_bsf_ptr[treeID][k];          
          SG_yBar_[offset]     = SG_yBar_ptr[treeID][k];
          SG_yStar_[offset]    = SG_yStar_ptr[treeID][k];          
          SG_betaZ_[offset]    = SG_betaZ_ptr[treeID][k];
          for (j = 1; j <= SG_augmObjCommon -> hcutCnt; j++) {
            SG_BETA_P_ptr[j][offset] = SG_betaP_ptr[treeID][j][k];
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
      }
      if (offset != RF_totalNodeCount) {
        RF_nativeError("\nRF-SRC:  *** ERROR *** ");
        RF_nativeError("\nRF-SRC:  Inconsistent total node count encountered during writing of forest topology.");
        RF_nativeError("\nRF-SRC:  Offset versus total was:  %20lu, %20lu", offset, RF_totalNodeCount);
        RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
        RF_nativeExit();
      }
      if (offsetCT != RF_totalTermCount) {
        RF_nativeError("\nRF-SRC:  *** ERROR *** ");
        RF_nativeError("\nRF-SRC:  Inconsistent total terminal count encountered during writing of forest topology.");
        RF_nativeError("\nRF-SRC:  Offset versus total was:  %20lu, %20lu", offsetCT, RF_totalTermCount);
        RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
        RF_nativeExit();
      }
}
void stackForestObjectsAuxOnlySGT(void) {
  uint previousTreeID;
  uint i, b;
  ulong offset;
  RF_restoreTreeID = uivector(1, RF_ntree);
  SG_offsetTree = ulvector(1, RF_ntree);
  SG_offsetCT   = ulvector(1, RF_ntree);
  SG_offsetID_rmbr = ulvector(1, RF_ntree);
  SG_offsetID_ombr = ulvector(1, RF_ntree);
  previousTreeID = b = 0;
  for (ulong ui = 1; ui <= RF_totalNodeCount; ui++) {
    if ((SG_treeID_[ui] > 0) && (SG_treeID_[ui] <= RF_ntree)) {
      if (SG_treeID_[ui] != previousTreeID) {
        previousTreeID = RF_restoreTreeID[++b] = SG_treeID_[ui];
        SG_offsetTree[SG_treeID_[ui]] = ui;
      }
      RF_nodeCount[SG_treeID_[ui]] ++;
    }
    else {
      RF_nativeError("\nRF-SRC:  Diagnostic Trace of Tree Record:  \n");
      RF_nativeError("\nRF-SRC:      treeID     nodeID ");
      RF_nativeError("\nRF-SRC:  %10d %10d \n", SG_treeID_[ui], SG_nodeID_[ui]);
      RF_nativeError("\nRF-SRC:  *** ERROR *** ");
      RF_nativeError("\nRF-SRC:  Invalid forest input record at line:  %20lu", ui);
      RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
      RF_nativeExit();
    }
  }
  for (b = 1; b <= RF_ntree; b++) {
    SG_offsetCT[b] = 0;
    if (b == 1) {
      SG_offsetCT[b] = 1;
    }
    else {
      SG_offsetCT[b] = SG_offsetCT[b-1] + RF_tLeafCount[b-1];
    }
  }
  offset = 0;
  for (b = 1; b <= RF_ntree; b++) {
    SG_offsetID_rmbr[b] = 0;
    SG_offsetID_ombr[b] = 0;
    if (b == 1) {
    }
    else {
      for (i = 1; i <= RF_tLeafCount[b-1]; i++) {
        ++offset;
        SG_offsetID_rmbr[b] += SG_rmbrTNodeCT_[offset];
        SG_offsetID_ombr[b] += SG_ombrTNodeCT_[offset];
      }
      SG_offsetID_rmbr[b] += SG_offsetID_rmbr[b-1];
      SG_offsetID_ombr[b] += SG_offsetID_ombr[b-1];
    }
  }
}
void unstackForestObjectsAuxOnlySGT(void) {
  free_uivector(RF_restoreTreeID, 1, RF_ntree);
  free_ulvector(SG_offsetTree, 1, RF_ntree);
  free_ulvector(SG_offsetCT, 1, RF_ntree);
  free_ulvector(SG_offsetID_rmbr, 1, RF_ntree);
  free_ulvector(SG_offsetID_ombr, 1, RF_ntree);
}
