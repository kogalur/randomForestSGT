
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "cdlRegr.h"
#include "cdlUtil.h"
#include "shared/nrutil.h"
#include "shared/error.h"
#include "shared/nativeUtil.h"
#include "stackOutput.h"
#include "cdlInfo.h"
#include "cdlUtil.h"
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
                               double **observationIn) {
  uint      xSize;
  uint     *nIndx;
  char     *xPerm;
  double   *beta;
  double    *weightNorm;
  double   *yObserved;
  double   *yRecast;
  double   *yRecastStandard;
  double  **xRecast;
  double  **xRecastStandard;
  double  **xObserved;
  double  *xWeightedMean;
  double   yWeightedMean;
  double  *xStdDev;
  double   yStdDev;
  double  *innerProductXY;
  double **innerProductXX;
  double   *gradientVector, *residualVector;
  double   *gradientResidualVector;
  char  *masterActiveX;
  char  *previouslyActiveX;
  char  *currentlyActiveX;
  double *lambdaGrid;
  uint    lambdaGridSizeThinned;
  double  result;
  CDLInfo *cdlObj;
  uint i, j, k, jj;
  yObserved = responseIn;
  xObserved = observationIn;
  if (nIndxIn == NULL) {
    nIndx = uivector(1, nIndxSize);
    for (uint k = 1; k <= nIndxSize; k++) {
      nIndx[k] = k;
    }
  }
  else {
    nIndx = nIndxIn;
  }
  if (xPermIn == NULL) {
    xPerm = cvector(1, pSize);
    xSize = pSize;
    for (uint j = 1; j <= pSize; j++) {
      xPerm[j] = TRUE;
    }
  }
  else {
    xPerm = xPermIn;
    xSize = 0;
    for (j = 1; j <= pSize; j++) {
      if (xPerm[j] == TRUE) {
        xSize ++;
      }
      else {
      }
    }
  }
  weightNorm = dvector(1, nIndxSize);
  if (weight == NULL) {
    for (k = 1; k <= nIndxSize; k++) {
      weightNorm[k] = 1.0 / nIndxSize;
    }
  }
  else {
    result = 0.0;
    for (k = 1; k <= nIndxSize; k++) {
      result += weight[nIndx[k]];
    }
    for (k = 1; k <= nIndxSize; k++) {
      weightNorm[k] = weight[nIndx[k]] / result;
    }
  }
  yWeightedMean = 0.0;
  for (k = 1; k <= nIndxSize; k++) {
    yWeightedMean += yObserved[nIndx[k]] * weightNorm[k];
  }
  yRecast         = dvector(1, nIndxSize);
  yRecastStandard = dvector(1, nIndxSize);
  for (k = 1; k <= nIndxSize; k++) {
    yRecast[k] = sqrt(weightNorm[k]) * (yObserved[nIndx[k]] - yWeightedMean);
  }
  yStdDev = 0.0;
  for (k = 1; k <= nIndxSize; k++) {
    yStdDev += yRecast[k] * yRecast[k];
  }
  yStdDev = sqrt(yStdDev);
  if (yStdDev > EPSILON) {
    for (k = 1; k <= nIndxSize; k++) {
      yRecastStandard[k] = yRecast[k] / yStdDev;
    }
  }
  if ( (yStdDev > EPSILON) && (xSize > 0) )  {
    beta = dvector(1, xSize);
    masterActiveX     = cvector(1, xSize);
    previouslyActiveX = cvector(1, xSize);
    currentlyActiveX  = cvector(1, xSize);
    innerProductXY = dvector(1, xSize);
    xRecast         = (double **) new_vvector(1, xSize, NRUTIL_DPTR);
    xRecastStandard = (double **) new_vvector(1, xSize, NRUTIL_DPTR);
    for (j = 1; j <= xSize; j++) {
      xRecast[j]         = dvector(1, nIndxSize);
      xRecastStandard[j] = dvector(1, nIndxSize);
    }
    xWeightedMean = dvector(1, xSize);
    xStdDev       = dvector(1, xSize);
    jj = 0;
    for (j = 1; j <= pSize; j++) {
      if (xPerm[j] == TRUE) {
        jj++;
        xWeightedMean[jj] = 0.0;
        for (k = 1; k <= nIndxSize; k++) {
          xWeightedMean[jj] += xObserved[j][nIndx[k]] * weightNorm[k];
        }
      }
      else {
      }
    }
    jj = 0;
    for (j = 1; j <= pSize; j++) {      
      if (xPerm[j] == TRUE) {
        jj++;
        for (k = 1; k <= nIndxSize; k++) {
          xRecast[jj][k] = sqrt(weightNorm[k]) * (xObserved[j][nIndx[k]] - xWeightedMean[jj]);
        }
      }
      else {
      }
    }
    for (j = 1; j <= xSize; j++) {  
      xStdDev[j] = 0.0;
      for (k = 1; k <= nIndxSize; k++) {
        xStdDev[j] += xRecast[j][k] * xRecast[j][k];
      }
      xStdDev[j] = sqrt(xStdDev[j]);
      if (xStdDev[j] > EPSILON) {
        for (k = 1; k <= nIndxSize; k++) {
          xRecastStandard[j][k] = xRecast[j][k] / xStdDev[j];
        }
      }
      else {
        for (k = 1; k <= nIndxSize; k++) {
          xRecastStandard[j][k] = 0.0;
        }
        xStdDev[j] = 1.0;
      }
    }
    initializeXY(xSize,
                 nIndxSize,
                 xRecastStandard,
                 yRecastStandard,
                 NULL,
                 0,
                 innerProductXY);
    if (!TRUE) {
      allocInitLambdaFirstPrinciples(xSize,
                                     nIndxSize,
                                     observationIn,
                                     responseIn,
                                     weightNorm,
                                     lambdaUser,
                                     lambdaGridSize,
                                     lambdaMinRatio,
                                     lambdaEpsilon,
                                     & lambdaGrid,
                                     & lambdaGridSizeThinned);
    }
    else {
      allocInitLambda(xSize,
                      lambdaUser,
                      lambdaGridSize,
                      lambdaMinRatio,
                      lambdaEpsilon,
                      innerProductXY,
                      & lambdaGrid,
                      & lambdaGridSizeThinned);
    }
    cdlObj = makeCDLInfo(pSize, lambdaGridSizeThinned);
    for (j = 1; j <= xSize; j++) {
      beta[j] = 0.0;
    }
    for (j = 1; j <= xSize; j++) {
      previouslyActiveX[j] = currentlyActiveX[j] = FALSE;
    }
    if (efficiency == TRUE) {
      gradientVector = dvector(1, xSize);
      gradientResidualVector = gradientVector;
      residualVector = NULL;
      innerProductXX = (double **) new_vvector(1, xSize, NRUTIL_DPTR);
      for (j = 1; j <= xSize; j++) {  
        innerProductXX[j] = dvector(1, xSize);
      }
      for (j = 1; j <= xSize; j++) {
        masterActiveX[j] = FALSE;
      }
      initializeGradient(xSize,
                         NULL,             
                         innerProductXY,   
                         NULL,             
                         gradientVector);
      updateBetaGeneric = updateBetaEfficient;
      updateGradientResidual = updateGradient;
    }
    else {
      residualVector = dvector(1, nIndxSize);
      gradientResidualVector = residualVector;
      gradientVector = NULL;
      innerProductXX = NULL;
      for (j = 1; j <= xSize; j++) {
        masterActiveX[j] = TRUE;
      }
      initializeResidual(nIndxSize,
                         xSize,
                         NULL,             
                         yRecastStandard,  
                         NULL,             
                         residualVector);
      updateBetaGeneric = updateBetaNaive;
      updateGradientResidual = updateResidual;
    }
    for (i = 1; i <= lambdaGridSizeThinned; i++) {
      cdlObj -> convgCnt[i] = 
        cdlFast(xSize,
                nIndxSize,
                lambdaGrid[i],
                maxIter,
                convergThreshold,
                xRecastStandard,
                masterActiveX,           
                previouslyActiveX,       
                currentlyActiveX,        
                innerProductXX,          
                gradientResidualVector,  
                beta);                   
      cdlObj -> lambda[i] = lambdaGrid[i];
      jj = 0;
      for (j = 1; j <= pSize; j++) {
        if (xPerm[j] == TRUE) {
          jj++;
          cdlObj -> beta[i][j+1] = (beta[jj] * yStdDev / xStdDev[jj]);
        }
        else {
          cdlObj -> beta[i][j+1] = 0.0;
        }
      }
      jj = 0;
      cdlObj -> beta[i][1] = yWeightedMean;
      for (j = 1; j <= pSize; j++) {      
        if (xPerm[j] == TRUE) {
          jj++;
          cdlObj -> beta[i][1] -= (xWeightedMean[jj] * cdlObj -> beta[i][j+1]);
        }
        else {
        }
      }
    }
    cdlObj -> valid = FALSE;
    jj = 0;
    while ((cdlObj -> valid == FALSE) && (jj < pSize)) {
      jj ++;
      if (fabs(cdlObj -> beta[lambdaGridSizeThinned][jj + 1]) > EPSILON) {
        cdlObj -> valid = TRUE;
      }
    }
    if (efficiency == TRUE) {
      free_dvector(gradientVector, 1, xSize);
      for (j = 1; j <= xSize; j++) {
        free_dvector(innerProductXX[j], 1, xSize);
      }
      free_new_vvector(innerProductXX, 1, xSize, NRUTIL_DPTR);
    }
    else {
      free_dvector(residualVector, 1, nIndxSize);
    }
    for (j = 1; j <= xSize; j++) {
      free_dvector(xRecast[j], 1, nIndxSize);
      free_dvector(xRecastStandard[j], 1, nIndxSize);
    }
    free_new_vvector(xRecast, 1, xSize, NRUTIL_DPTR);
    free_new_vvector(xRecastStandard, 1, xSize, NRUTIL_DPTR);
    free_dvector(xWeightedMean, 1, xSize);
    free_dvector(xStdDev,       1, xSize);
    deallocLambda(lambdaUser,
                  lambdaGridSize,
                  lambdaGrid);
    free_dvector(innerProductXY, 1, xSize);
    free_cvector(masterActiveX,     1, xSize);
    free_cvector(previouslyActiveX, 1, xSize);
    free_cvector(currentlyActiveX,  1, xSize);
    free_dvector(beta, 1, xSize);
  }
  else {
    lambdaGridSizeThinned = 1;
    cdlObj = makeCDLInfo(pSize, lambdaGridSizeThinned);
    cdlObj -> convgCnt[1] = 0;
    cdlObj -> lambda[lambdaGridSizeThinned] = 0.0;
    cdlObj -> beta[lambdaGridSizeThinned][1] = yWeightedMean;
    for (j = 1; j <= pSize; j++) {
      cdlObj -> beta[lambdaGridSizeThinned][j+1] = 0.0;
    }
    cdlObj -> valid = FALSE;
  }
  cdlObj -> lambdaTargetIndx = lambdaGridSizeThinned;
  free_dvector(yRecast,   1, nIndxSize);
  free_dvector(yRecastStandard, 1, nIndxSize);
  free_dvector(weightNorm, 1, nIndxSize);
  if (nIndxIn == NULL) {
    free_uivector(nIndx, 1, nIndxSize);
  }
  if (xPermIn == NULL) {
    free_cvector(xPerm, 1, pSize);
  }
  return cdlObj;
}
