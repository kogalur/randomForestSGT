// *** THIS FILE IS AUTO GENERATED. DO NOT EDIT IT ***
#ifndef RF_EXTERNAL_H
#define RF_EXTERNAL_H

#include "shared/globalCore.h"
#include "descriptor.h"
#include "augmentation.h"
#include "node.h"
extern double   *SG_betaOut_;
extern double  **SG_betaOutPtr;
extern double   *SG_lambda_;
extern uint     *SG_lambdaCount_;
extern uint     *SG_convergenceCount_;
extern double    *SG_yHatOut_;
extern double   **SG_yHatOutPtr;
extern uint      *SG_lambdaMinIndx_;
extern uint      *SG_lambda1SEIndx_;
extern double    *SG_lambdaMeanErr_;
extern double    *SG_lambdaCVSD_;
extern double   *SG_cpuTime_;
extern double   *SG_emprRSK_;
extern double   *SG_oobEmprRSK_;
extern uint     *SG_MEMB_ID_;
extern uint     *SG_BOOT_CT_;
extern uint     **SG_MEMB_ID_ptr;
extern uint     **SG_BOOT_CT_ptr;
extern double   *SG_cdl_oobEnsembleRGR_;
extern double   *SG_cdl_fullEnsembleRGR_;
extern double  **SG_cdl_oobEnsembleRGRptr;
extern double  **SG_cdl_fullEnsembleRGRptr;
extern double  **SG_cdl_oobEnsembleRGRnum;
extern double  **SG_cdl_fullEnsembleRGRnum;
extern double **SG_yHatAbsolute;
extern double **SG_yHatAbsoluteTest;
extern uint   RF_totalNodeCount;
extern uint  *SG_treeID_;
extern uint  *SG_nodeID_;
extern uint  *SG_nodeSZ_;
extern uint  *SG_brnodeID_;
extern double   *SG_nodeStat_;
extern uint     *SG_bsf_;
extern double  *SG_yBar_;
extern double  *SG_yStar_;
extern double  *SG_betaZ_;
extern double  *SG_betaP_;
extern uint    *SG_rmbrTNodeCT_;
extern uint    *SG_ombrTNodeCT_;
extern uint    *SG_rmbrTNodeID_;
extern uint    *SG_ombrTNodeID_;
extern uint    *SG_ambrTNodeID_;
extern double **SG_betaP_in;
extern uint **SG_treeID_ptr;
extern uint **SG_nodeID_ptr;
extern uint **SG_nodeSZ_ptr;
extern uint **SG_brnodeID_ptr;
extern double   **SG_nodeStat_ptr;
extern uint     **SG_bsf_ptr;
extern double  **SG_yBar_ptr;
extern double  **SG_yStar_ptr;
extern double  **SG_betaZ_ptr;
extern double ***SG_betaP_ptr;
extern double **SG_BETA_P_ptr;
extern uint   **SG_rmbrTNodeCT_ptr;
extern uint   **SG_ombrTNodeCT_ptr;
extern uint   **SG_rmbrTNodeID_ptr;
extern uint   **SG_ombrTNodeID_ptr;
extern uint   **SG_ambrTNodeID_ptr;
extern uint     SG_xSizeAugm;
extern double **SG_observationInAugm;
extern double **SG_fobservationInAugm;
extern char SG_threadV;
extern uint     SG_optLocal;
extern double  *SG_lambdaGrid;
extern uint     SG_lambdaGridSize;
extern double   SG_lambdaMinRatio;
extern double   SG_lambdaEpsilon;
extern double   SG_convergThreshold;
extern uint     SG_maxIter;
extern char     SG_efficiency;
extern double  *SG_weight;
extern uint     SG_nfolds;
extern uint     SG_bsfOrder;
extern double (*updateBetaGeneric) (uint,
                             uint,
                             double,
                             double*,
                             double**,
                             double*);
extern double (*updateGradientResidual) (uint,
                                  uint,
                                  uint,
                                  double,
                                  double,
                                  double,
                                  double**,
                                  double**,
                                  double*);
extern AugmentationObj* ((*getAugmentationObjGeneric) (AugmentationObjCommon *objCommon, uint treeID, Node *parent));
extern AugmentationObjCommon* ((*getAugmentationObjCommonGeneric) (uint hcut,
                                                            uint n,
                                                            uint p,
                                                            double **xArrayIn,
                                                            uint pAugm,
                                                            double **xArrayInAugm,
                                                            char *permIn,
                                                            double *yArrayIn));
extern void (*freeAugmentationObjCommonGeneric) (AugmentationObjCommon *obj);
extern void (*getAugmentationObjCommonGenericTest) (AugmentationObjCommon *obj, uint fn, double **fxArrayIn, double **fxArrayInAugm, double *fyArrayIn);
extern void (*freeAugmentationObjCommonGenericTestOnly) (AugmentationObjCommon *obj);
extern uint    SG_splitRule;
extern uint    SG_hcut;
extern uint    SG_lotSize;
extern uint    SG_lotLag;
extern uint    SG_lotStrikeout;
extern uint    SG_lotCutMax2;
extern uint    SG_lotCutMax3;
extern uint    SG_lotCutMax4;
extern double   **SG_emprRSKptr;
extern double   **SG_oobEmprRSKptr;
extern uint     **SG_OMBR_ID_ptr;
extern uint     **SG_TN_OCNT_ptr;
extern ulong    *SG_offsetTree;
extern ulong    *SG_offsetCT;
extern ulong    *SG_offsetID_rmbr;
extern ulong    *SG_offsetID_ombr;
extern AugmentationObjCommon *SG_augmObjCommon;
extern char (*getVariance) (uint, uint*, double*, double*, double*);
extern DescriptorObj *SG_headDO;
#ifdef _OPENMP
extern omp_lock_t   SG_lockDO;
#endif

#endif
