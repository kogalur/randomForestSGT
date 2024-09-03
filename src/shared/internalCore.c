#include "globalCore.h"
#include "snpAuxiliaryInfo.h"
#include "leafLink.h"
#include "factor.h"
#include "terminalBase.h"
#include "nodeBase.h"
/*
JNIEnv    *RF_java_env;
jobject    RF_java_obj;
jclass     RF_java_cls;
jclass     RF_java_except_cls;
jclass     RF_java_hshmap_cls;
jmethodID  RF_java_hshmap_constr;
jmethodID  RF_java_hshmap_put;
jobject    RF_java_hshmap_obj;
jclass     RF_java_ens_cls;
jmethodID  RF_java_ens_mid;
jmethodID  RF_java_mid_log;
jmethodID  RF_java_mid_logError;
jmethodID  RF_java_mid_logExit;
NAT1DInfo **RF_nat1DInfoList;
NAT2DInfo **RF_nat2DInfoList;
NativeEnsembleInfo **RF_nativeEnsembleInfoList;
uint       RF_nat1DInfoListSize;
uint       RF_nat2DInfoListSize;
uint       RF_nativeEnsembleInfoListSize;
jobject    RF_rLevelsJNIE;
jobject    RF_xLevelsJNIE;
*/

SEXP RF_sexpVector[2];
SEXP      RF_rLevelsSEXP;
SEXP      RF_xLevelsSEXP;

SNPAuxiliaryInfo **RF_snpAuxiliaryInfoList;
SNPAuxiliaryInfo **RF_incomingAuxiliaryInfoList;
AuxiliaryDimensionConstants *RF_auxDimConsts;
uint   RF_userTraceFlag;
time_t RF_userTimeStart;
time_t RF_userTimeSplit;  
uint      RF_opt;
uint      RF_optHigh;
float (*ran1A) (uint);
void  (*randomSetChain) (uint, int);
int   (*randomGetChain) (uint);
float (*ran1B) (uint);
void  (*randomSetChain2) (uint, int);
int   (*randomGetChain2) (uint);
float (*ran1D) (uint);
void  (*randomSetChain3) (uint, int);
int   (*randomGetChain3) (uint);
void *(*makeNode) (uint xSize);
void  (*freeNode) (void *parent);
void *(*makeTerminal) ();
void  (*freeTerminal) (void *parent);
void (*assignAllTerminalNodeOutcomes) (char mode, uint treeID, TerminalBase *term);
void (*calculateAllTerminalNodeOutcomes) (char mode, uint treeID, TerminalBase *term);
double   *RF_quantile;
uint      RF_quantileSize;
double    RF_qEpsilon;
uint      RF_inv_2qEpsilon;
uint      RF_observationSize;
double ***RF_response;
ulong    *RF_restoreTreeOffset;
uint     *RF_treeID_;
uint     *RF_nodeID_;
uint     *RF_nodeSZ_;
uint      *RF_hcDim_;
int      **RF_parmID_;
double   **RF_contPT_;
uint     **RF_mwcpSZ_;
ulong   **RF_restoreMWCPoffset;
uint     **RF_mwcpPT_;
uint  **RF_AMBR_ID_ptr;
uint  **RF_TN_ACNT_ptr;
uint  **RF_MEMB_ID_ptr;
uint RF_stackCount;
uint RF_nativeIndex;
LeafLinkedObj **RF_leafLinkedObjTail;
LeafLinkedObj **RF_leafLinkedObjHead;
TerminalBase ***RF_tTermMembership;
TerminalBase ***RF_ftTermMembership;
uint      RF_ySize;
uint      RF_xSize;
char     *RF_rType;
char     *RF_xType;
uint     *RF_xLevelsCnt;
uint     *RF_rTarget;
uint      RF_rTargetCount;
uint      RF_timeIndex;
uint      RF_statusIndex;
uint      RF_rFactorCount;
uint     *RF_rFactorMap;
uint     *RF_rFactorIndex;
uint     *RF_rFactorSize;
uint      RF_xFactorCount;
uint     *RF_xFactorMap;
uint     *RF_xFactorIndex;
uint     *RF_xFactorSize;
Factor ***RF_factorList;
NodeBase    **RF_root;
NodeBase   ***RF_nodeMembership;
NodeBase   ***RF_fnodeMembership;
NodeBase   ***RF_pNodeMembership;
NodeBase   ***RF_pNodeList;
TerminalBase   ***RF_pTermMembership;
TerminalBase   ***RF_tTermList;
TerminalBase   ***RF_pTermList;
uint      RF_ntree;
int       RF_numThreads;
uint     *RF_brnodeID_;
uint     **RF_fsrecID_;
double   *RF_timeInterest;
uint      RF_timeInterestSize;
uint      RF_ytry;
double   *RF_xWeightStat;
double   *RF_xWeight;
double   *RF_yWeight;
uint      RF_subjSize;
double   *RF_subjWeight;
uint      RF_bootstrapSize;
uint    **RF_bootstrapIn;
uint    **RF_bootMembershipIndex;
uint      RF_splitRule;
uint      RF_nsplit;
uint      RF_nodeSize;
uint      RF_mtry;
double   *RF_masterTime;
uint      RF_masterTimeSize;
uint      RF_sortedTimeInterestSize;
uint     *RF_startMasterTimeIndexIn;
uint     *RF_masterTimeIndexIn;
uint     *RF_masterToInterestTimeMap;
uint     *RF_yIndex;
uint      RF_ySizeProxy;
uint     *RF_yIndexZero;
uint      RF_yIndexZeroSize;
uint      RF_frSize;
uint      RF_ptnCount;
double  **RF_fresponseIn;
double  **RF_responseIn;
double  **RF_observationIn;
uint      RF_fobservationSize;
double  **RF_fobservationIn;
uint     *RF_rTargetFactor;
uint      RF_rTargetFactorCount;
uint      RF_rTargetNonFactorCount;
uint     *RF_rTargetNonFactor;
uint      RF_rNonFactorCount;
uint     *RF_rNonFactorMap;
uint     *RF_rNonFactorIndex;
uint      RF_xNonFactorCount;
uint     *RF_xNonFactorMap;
uint     *RF_xNonFactorIndex;
uint      RF_rMaxFactorLevel;
uint      RF_xMaxFactorLevel;
uint      RF_maxFactorLevel;
uint      RF_startTimeIndex;
uint      RF_crWeightSize;
double   *RF_crWeight;
uint     *RF_mRecordMap;
uint     *RF_fmRecordMap;
uint      RF_mRecordSize;
uint      RF_fmRecordSize;
int     **RF_mpSign;
int     **RF_fmpSign;
uint     *RF_subjIn;
uint     *RF_xLevelsMax;
uint    **RF_xLevels;
uint     *RF_holdBLKptr;
uint     *RF_xtType;
uint     *RF_stType;
uint    *RF_oobSize;
uint    *RF_ibgSize;
uint    *RF_nodeCount;
uint    *RF_pLeafCount;
uint    *RF_maxDepth;
LeafLinkedObj ***RF_hTermMembership;
uint    **RF_bootMembershipIndex;
uint     *RF_identityMembershipIndex;
uint     *RF_fidentityMembershipIndex;
uint      RF_identityMembershipIndexSize;
uint      RF_fidentityMembershipIndexSize;
char    **RF_bootMembershipFlag;
uint    **RF_bootMembershipCount;
char    **RF_oobMembershipFlag;
uint    **RF_ibgMembershipIndex;
uint    **RF_oobMembershipIndex;
uint     *RF_getTree;
uint     *RF_orderedTreeIndex;
uint     *RF_serialTreeIndex;
uint      RF_serialTreeID;
uint      RF_serialBlockID;
uint      RF_perfBlockCount;
uint      RF_perfBlock;
uint      RF_ensembleUpdateCount;
uint     *RF_nodeCountSyth;
uint     *RF_getTreeIndex;
uint      RF_getTreeCount;
uint      RF_subjWeightType;
uint     *RF_subjWeightSorted;
uint      RF_subjWeightDensitySize;
uint      RF_eventTypeSize;
uint      RF_feventTypeSize;
uint      RF_mStatusSize;
uint     *RF_eventType;
uint     *RF_eventTypeIndex;
uint     *RF_eIndividualSize;
uint    **RF_eIndividualIn;
uint     *RF_subjSlot;
uint     *RF_subjSlotCount;
uint    **RF_subjList;
uint     *RF_caseMap;
uint     *RF_subjMap;
uint      RF_subjCount;
uint      *RF_classLevelSize;
uint     **RF_classLevel;
uint     **RF_classLevelIndex;
uint      RF_intrPredictorSize;
uint     *RF_intrPredictor;
char     *RF_importanceFlag;   
uint  **RF_RMBR_ID_ptr;
uint  **RF_TN_RCNT_ptr;
uint     *RF_RMBR_ID_;
uint     *RF_AMBR_ID_;
uint     *RF_TN_RCNT_;
uint     *RF_TN_ACNT_;
double   *RF_TN_SURV_;
double   *RF_TN_MORT_;
double   *RF_TN_NLSN_;
double   *RF_TN_CSHZ_;
double   *RF_TN_CIFN_;
double   *RF_TN_KHZF_;
double   *RF_TN_REGR_;
uint     *RF_TN_CLAS_;
double  ***RF_TN_SURV_ptr;
double  ***RF_TN_MORT_ptr;
double  ***RF_TN_NLSN_ptr;
double ****RF_TN_CSHZ_ptr;
double ****RF_TN_CIFN_ptr;
double  ***RF_TN_KHZF_ptr;
double  ***RF_TN_REGR_ptr;
uint   ****RF_TN_CLAS_ptr;
uint  **RF_BOOT_CT_ptr;
uint   **RF_mwcpCT;
double   *RF_spltST_;
uint     *RF_dpthST_;
ulong     RF_totalTermCount;
uint     *RF_restoreTreeID;
ulong     RF_totalNodeCount_;
uint     *RF_tLeafCount_;
uint     *RF_rLevelsMax;
uint     *RF_rLevelsCnt;
uint    **RF_rLevels;
double   *RF_rFactorThreshold;
uint     *RF_rFactorMinority;
uint     *RF_rFactorMajority;
char     *RF_rFactorMinorityFlag;
uint RF_incomingStackCount;
int      *RF_seed_;
double ***RF_observation;
double ***RF_fobservation;
double  **RF_status;
double  **RF_time;
double  **RF_startTime;
double ***RF_response;
double ***RF_fresponse;
char      RF_mStatusFlag;
char      RF_mTimeFlag;
char      RF_mResponseFlag;
char      RF_mPredictorFlag;
uint    **RF_startMasterTimeIndex;
uint    **RF_masterTimeIndex;
double   *RF_oobEnsembleCLS_;
double   *RF_fullEnsembleCLS_;
double ***RF_oobEnsembleCLSptr;
double ***RF_fullEnsembleCLSptr;
double ***RF_oobEnsembleCLSnum;
double ***RF_fullEnsembleCLSnum;
double   *RF_oobEnsembleRGR_;
double   *RF_fullEnsembleRGR_;
double  **RF_oobEnsembleRGRptr;
double  **RF_fullEnsembleRGRptr;
double  **RF_oobEnsembleRGRnum;
double  **RF_fullEnsembleRGRnum;
double   *RF_oobEnsembleDen;
double   *RF_fullEnsembleDen;
double   *RF_perfCLS_;
double   *RF_perfRGR_;
double  ***RF_perfCLSptr;
double   **RF_perfRGRptr;
uint     *RF_tLeafCount;
int      *RF_seed;
uint     *RF_optLoGrow_;
uint      RF_optLoGrow;
#ifdef _OPENMP
omp_lock_t ***RF_lockCLSoens;
omp_lock_t ***RF_lockCLSfens;
omp_lock_t   *RF_lockDENoens;
omp_lock_t   *RF_lockDENfens;
omp_lock_t    RF_lockPerf;
omp_lock_t    RF_lockEnsbUpdtCount;
#endif
