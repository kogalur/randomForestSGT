#ifndef RF_CDL_REGR_H
#define RF_CDL_REGR_H
#include "cdlInfo.h"
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
