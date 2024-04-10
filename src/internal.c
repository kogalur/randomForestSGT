#include "shared/globalCore.h"
#include "descriptor.h"
#include "augmentation.h"
#include "node.h"
double   *SG_betaOut_;
double  **SG_betaOutPtr;
double   *SG_lambda_;
uint     *SG_lambdaCount_;
uint     *SG_convergenceCount_;
double    *SG_yHatOut_;
double   **SG_yHatOutPtr;
uint      *SG_lambdaMinIndx_;
uint      *SG_lambda1SEIndx_;
double    *SG_lambdaMeanErr_;
double    *SG_lambdaCVSD_;
double   *SG_cpuTime_;
double   *SG_emprRSK_;
double   *SG_oobEmprRSK_;
uint     *SG_MEMB_ID_;
uint     *SG_BOOT_CT_;
uint     **SG_MEMB_ID_ptr;
uint     **SG_BOOT_CT_ptr;
double   *SG_cdl_oobEnsembleRGR_;
double   *SG_cdl_fullEnsembleRGR_;
double  **SG_cdl_oobEnsembleRGRptr;
double  **SG_cdl_fullEnsembleRGRptr;
double  **SG_cdl_oobEnsembleRGRnum;
double  **SG_cdl_fullEnsembleRGRnum;
double **SG_yHatAbsolute;
double **SG_yHatAbsoluteTest;
uint   RF_totalNodeCount;
uint  *SG_treeID_;
uint  *SG_nodeID_;
uint  *SG_nodeSZ_;
uint  *SG_brnodeID_;
uint  *SG_prnodeID_;
double   *SG_nodeStat_;
uint     *SG_bsf_;
double  *SG_yBar_;
double  *SG_yStar_;
double  *SG_betaZ_;
double  *SG_betaP_;
uint    *SG_rmbrTNodeCT_;
uint    *SG_ombrTNodeCT_;
uint    *SG_rmbrTNodeID_;
uint    *SG_ombrTNodeID_;
uint    *SG_ambrTNodeID_;
double **SG_betaP_in;
uint **SG_treeID_ptr;
uint **SG_nodeID_ptr;
uint **SG_nodeSZ_ptr;
uint **SG_brnodeID_ptr;
uint **SG_prnodeID_ptr;
double   **SG_nodeStat_ptr;
uint     **SG_bsf_ptr;
double  **SG_yBar_ptr;
double  **SG_yStar_ptr;
double  **SG_betaZ_ptr;
double ***SG_betaP_ptr;
double **SG_BETA_P_ptr;
uint   **SG_rmbrTNodeCT_ptr;
uint   **SG_ombrTNodeCT_ptr;
uint   **SG_rmbrTNodeID_ptr;
uint   **SG_ombrTNodeID_ptr;
uint   **SG_ambrTNodeID_ptr;
uint     SG_xSizeAugm;
double **SG_observationInAugm;
double **SG_fobservationInAugm;
char SG_threadV;
uint     SG_optLocal;
double  *SG_lambdaGrid;
uint     SG_lambdaGridSize;
double   SG_lambdaMinRatio;
double   SG_lambdaEpsilon;
double   SG_convergThreshold;
uint     SG_maxIter;
char     SG_efficiency;
double  *SG_weight;
uint     SG_nfolds;
uint     SG_bsfOrder;
double (*updateBetaGeneric) (uint,
                             uint,
                             double,
                             double*,
                             double**,
                             double*);
double (*updateGradientResidual) (uint,
                                  uint,
                                  uint,
                                  double,
                                  double,
                                  double,
                                  double**,
                                  double**,
                                  double*);
AugmentationObj* ((*getAugmentationObjGeneric) (AugmentationObjCommon *objCommon, uint treeID, Node *parent));
AugmentationObjCommon* ((*getAugmentationObjCommonGeneric) (uint hcut,
                                                            uint n,
                                                            uint p,
                                                            double **xArrayIn,
                                                            uint pAugm,
                                                            double **xArrayInAugm,
                                                            char *permIn,
                                                            double *yArrayIn));
void (*freeAugmentationObjCommonGeneric) (AugmentationObjCommon *obj);
void (*getAugmentationObjCommonGenericTest) (AugmentationObjCommon *obj, uint fn, double **fxArrayIn, double **fxArrayInAugm, double *fyArrayIn);
void (*freeAugmentationObjCommonGenericTestOnly) (AugmentationObjCommon *obj);
uint    SG_splitRule;
uint    SG_hcut;
uint    SG_lotSize;
uint    SG_lotLag;
uint    SG_lotStrikeout;
uint    SG_lotCutMax2;
uint    SG_lotCutMax3;
uint    SG_lotCutMax4;
double   **SG_emprRSKptr;
double   **SG_oobEmprRSKptr;
uint     **SG_OMBR_ID_ptr;
uint     **SG_TN_OCNT_ptr;
ulong    *SG_offsetTree;
ulong    *SG_offsetCT;
ulong    *SG_offsetID_rmbr;
ulong    *SG_offsetID_ombr;
AugmentationObjCommon *SG_augmObjCommon;
char (*getVariance) (uint, uint*, double*, double*, double*);
DescriptorObj *SG_headDO;
uint SG_tcpPort;
uint SG_tcpTimeOut;
#ifdef _OPENMP
omp_lock_t   SG_lockDO;
#endif
