#ifndef RF_CDL_REGR_H
#define RF_CDL_REGR_H
#include "cdlInfo.h"
typedef struct nodeSupport {
  double *lower;
  double *upper;
  double yLower;
  double yUpper;
} NodeSupport;
char computeNodeCenter(uint memberSize, const uint *memberIndex,
                       const double *weight, double *center);
char computeNodeCenterAndSupport(uint memberSize, const uint *memberIndex,
                                 const double *weight, const double *response,
                                 double *center, NodeSupport *support);
double evaluateNodeBasisTerm(double **source, const double *center,
                             uint observationID, uint termID);
double evaluateRestrictedNodePrediction(double **source, const double *center,
                                         const NodeSupport *support,
                                         const double *beta, uint observationID,
                                         double *scratch);
double **makeNodeBasis(const double *center,
                       uint repMembrSize, const uint *repMembrIndx,
                       uint oobMembrSize, const uint *oobMembrIndx);
void freeNodeBasis(double **basis);
CDLInfo *coordinateDescentRegr(double  *lambdaUser,
                               uint     lambdaGridSize,
                               double   lambdaMinRatio,
                               double   lambdaEpsilon,
                               double   convergThreshold,
                               uint     maxIter,
                               char     efficiency,
                               double  *weight,
                               uint     nIndxSize,
                               uint     pSize,
                               uint    *nIndxIn,
                               char    *xPermIn,
                               uint     nfolds,
                               double  *responseIn,
                               double **observationIn);
#endif
