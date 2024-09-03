
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "cdlUtil.h"
#include "shared/nrutil.h"
#include "shared/error.h"
#include "shared/nativeUtil.h"
#include "stackOutput.h"
#include "cdlInfo.h"
void allocInitLambdaFirstPrinciples(uint     xSize,
                                    uint     n,
                                    double **xArray,
                                    double  *yArray,
                                    double  *weightNorm,
                                    double  *lambdaUser,
                                    uint     lambdaGridSize,
                                    double   lambdaMinRatio,
                                    double   lambdaEpsilon,
                                    double **lambdaGrid,
                                    uint    *lambdaGridSizeThinned) {
  double lambdaMax, lambdaMaxLog;
  double lambdaMin, lambdaMinLog;
  double yMean,   *xMean;
  double yStdDev, *xStdDev;
  double delta;
  double result;
  uint i, j, k;
  if (lambdaUser == NULL) {
    xMean   = dvector(1, xSize);
    xStdDev = dvector(1, xSize);
    (*lambdaGrid) = dvector(1, lambdaGridSize);
    (*lambdaGridSizeThinned) = lambdaGridSize;
    yMean = 0.0;
    for (i = 1; i <= n; i++) {
      yMean += yArray[i] * weightNorm[i];
    }
    yStdDev = 0.0;
    for (i = 1; i <= n; i++) {
      yStdDev += weightNorm[i] * pow (yArray[i] - yMean, 2.0);
    }
    yStdDev = sqrt(yStdDev);
    for (j = 1; j <= xSize; j++) {
      xMean[j] = 0.0;
      for (i = 1; i <= n; i++) {
        xMean[j] += xArray[j][i] * weightNorm[i];
      }
      xStdDev[j] = 0.0;
      for (i = 1; i <= n; i++) {
        xStdDev[j] += weightNorm[i] * pow (xArray[j][i] - xMean[j], 2.0);
      }
      xStdDev[j] = sqrt(xStdDev[j]);
    }
      lambdaMax = -1.0;
      for (j = 1; j <= xSize; j++) {
        result = 0.0;
        for (i = 1; i <= n; i++) {
          result += weightNorm[i] * xArray[j][i] * (yArray[i] - yMean);
        }
        result = fabs(result) / (xStdDev[j] * yStdDev);
        if (result > lambdaMax) {
          lambdaMax = result;
        }
      }
      lambdaMin = lambdaMax * lambdaEpsilon;
      lambdaMaxLog = log(lambdaMax);
      lambdaMinLog = log(lambdaMin);
      delta = (lambdaMaxLog - lambdaMinLog) / (lambdaGridSize - 1);
      for (k = 1; k <= lambdaGridSize; k++) {
        (*lambdaGrid)[lambdaGridSize - k + 1] = lambdaMinLog + (delta * (k-1));
        (*lambdaGrid)[lambdaGridSize - k + 1] = exp((*lambdaGrid)[lambdaGridSize - k + 1]);
      }
      i = 0;
      for (k = 1; k <= (*lambdaGridSizeThinned) - 1; k++) {
        if (((*lambdaGrid)[k] / lambdaMax) > lambdaMinRatio) {
          (*lambdaGrid)[++i] = (*lambdaGrid)[k];
        }
      }
      (*lambdaGrid)[++i] = (*lambdaGrid)[(*lambdaGridSizeThinned)];      
      (*lambdaGridSizeThinned) = i;
    free_dvector(xMean,   1, xSize);
    free_dvector(xStdDev, 1, xSize);
  }
  else {
    *lambdaGrid = lambdaUser;
    (*lambdaGridSizeThinned) = lambdaGridSize;
  }
}
void allocInitLambda(uint     xSize,
                     double  *lambdaUser,
                     uint     lambdaGridSize,
                     double   lambdaMinRatio,
                     double   lambdaEpsilon,
                     double  *innerProductXY,
                     double **lambdaGrid,
                     uint    *lambdaGridSizeThinned) {
  double lambdaMax, lambdaMaxLog;
  double lambdaMin, lambdaMinLog;
  double delta;
  double result;
  uint i, j, k;
  if (lambdaUser == NULL) {
    (*lambdaGrid) = dvector(1, lambdaGridSize);
    (*lambdaGridSizeThinned) = lambdaGridSize;
    result = 0.0;
    for (j = 1; j <= xSize; j++) {
      if (result <= fabs(innerProductXY[j])) {
        result = fabs(innerProductXY[j]);
      }
    }
    lambdaMax = result;
    lambdaMin = result * lambdaEpsilon;
    lambdaMaxLog = log(lambdaMax);
    lambdaMinLog = log(lambdaMin);
    delta = (lambdaMaxLog - lambdaMinLog) / (lambdaGridSize - 1);
    for (k = 1; k <= lambdaGridSize; k++) {
      (*lambdaGrid)[lambdaGridSize - k + 1] = lambdaMinLog + (delta * (k-1));
      (*lambdaGrid)[lambdaGridSize - k + 1] = exp((*lambdaGrid)[lambdaGridSize - k + 1]);
    }
    i = 0;
    for (k = 1; k <= (*lambdaGridSizeThinned) - 1; k++) {
      if (((*lambdaGrid)[k] / lambdaMax) > lambdaMinRatio) {
        (*lambdaGrid)[++i] = (*lambdaGrid)[k];
      }
    }
    (*lambdaGrid)[++i] = (*lambdaGrid)[(*lambdaGridSizeThinned)];      
    (*lambdaGridSizeThinned) = i;
  }
  else {
    *lambdaGrid = lambdaUser;
    (*lambdaGridSizeThinned) = lambdaGridSize;
  }
}
void deallocLambda(double  *lambdaUser,
                   uint     lambdaGridSize,
                   double  *lambdaGrid) {
  if (lambdaUser == NULL) {
    free_dvector(lambdaGrid, 1, lambdaGridSize);
  }
  else {
  }
}
double updateBetaFirstPrinciples(uint j,
                                 uint p,
                                 double    lambda,
                                 double   *beta,
                                 double   *innerProductXY,
                                 double  **innerProductXX) {
  double sZLfun;
  double z;
  double partialSum;
  uint k;
  partialSum = 0.0;
  for (k = 1; k <= p; k++) {
    partialSum += (innerProductXX[j][k] * beta[k]);
  }
  z = beta[j] + innerProductXY[j] - partialSum;
  if ((z > SG_EPSILON) && ((fabs(z) - lambda) > SG_EPSILON)) {
    sZLfun = z - lambda;
  }
  else if ((z < (-SG_EPSILON)) && ((fabs(z) - lambda) > SG_EPSILON)) {
    sZLfun = z + lambda;
  }
  else {
    sZLfun = 0.0;
  }
  return sZLfun;
}
double updateBetaEfficient(uint      j,
                           uint      nDummy,
                           double    lambda,
                           double   *beta,
                           double  **xArrayDummy,
                           double   *gradientVector) {
  double sZLfun;
  double z;
  z = beta[j] + gradientVector[j];
  if ((z > SG_EPSILON) && ((fabs(z) - lambda) > SG_EPSILON)) {
    sZLfun = z - lambda;
  }
  else if ((z < (-SG_EPSILON)) && ((fabs(z) - lambda) > SG_EPSILON)) {
    sZLfun = z + lambda;
  }
  else {
    sZLfun = 0.0;
  }
  return sZLfun;
}
double updateBetaNaive(uint      j,
                       uint      n,
                       double    lambda,
                       double   *beta,
                       double  **xArray,
                       double   *residualVector) {
  double sZLfun;
  double z;
  uint i;
  z = beta[j];
  for (i = 1; i <= n; i++) {
    z += (xArray[j][i] * residualVector[i]);
  }
  if ((z > SG_EPSILON) && ((fabs(z) - lambda) > SG_EPSILON)) {
    sZLfun = z - lambda;
  }
  else if ((z < (-SG_EPSILON)) && ((fabs(z) - lambda) > SG_EPSILON)) {
    sZLfun = z + lambda;
  }
  else {
    sZLfun = 0.0;
  }
  return sZLfun;
}
void initializeGradient(uint p,
                        double   *beta,
                        double   *innerProductXY,
                        double  **innerProductXX,
                        double   *gradientVector) {
  double partialSum;
  uint j, k;
  if ((beta == NULL) || (innerProductXX == NULL)) {
    for (j = 1; j <= p; j++) {
      gradientVector[j] = innerProductXY[j];
    }
  }
  else {
    for (j = 1; j <= p; j++) {
      partialSum = 0.0;
      for (k = 1; k <= p; k++) {
        partialSum += (innerProductXX[j][k] * beta[k]);
      }
      gradientVector[j] = innerProductXY[j] - partialSum;
    }
  }
}
void initializeResidual(uint n,
                        uint p,
                        double   *beta,
                        double   *yRecastStandard,
                        double  **xRecastStandard,
                        double   *residualVector) {
  double partialSum;
  uint i, j;
  if ((beta == NULL) || (xRecastStandard == NULL)) {
    for (i = 1; i <= n; i++) {
      residualVector[i] = yRecastStandard[i];
    }
  }
  else {
    for (i = 1; i <= n; i++) {
      partialSum = 0.0;
      for (j = 1; j <= p; j++) {
        partialSum += (xRecastStandard[j][i] * beta[j]);
      }
      residualVector[i] = yRecastStandard[i] - partialSum;
    }
  }
}
void calculateGradient(uint j,
                       uint p,
                       double   *beta,
                       double   *innerProductXY,
                       double  **innerProductXX,
                       double   *gradientVector) {
  double partialSum;
  uint k;
  partialSum = 0.0;
  for (k = 1; k <= p; k++) {
    partialSum += (innerProductXX[j][k] * beta[k]);
  }
  gradientVector[j] = innerProductXY[j] - partialSum;
}
double updateGradient(uint l,
                      uint p,
                      uint nDummy,
                      double    lambda,
                      double    betaOld,
                      double    betaNew,
                      double  **xArrayDummy,
                      double  **innerProductXX,
                      double   *gradientVector) {
  double result;
  uint j;
  for (j = 1; j <= p; j++) {
    gradientVector[j] = gradientVector[j] + (innerProductXX[j][l] * (betaOld - betaNew));
  }
  result = ((betaOld - betaNew) * (betaOld + gradientVector[l])
            + (0.5 * betaNew * betaNew) - (0.5 * betaOld * betaOld)
            + (lambda * (fabs(betaNew) - fabs(betaOld))));
  return result;
}
double updateResidual(uint l,
                      uint pDummy,
                      uint n,
                      double    lambda,
                      double    betaOld,
                      double    betaNew,
                      double  **xArray,
                      double  **innerProductXXDummy,
                      double   *residualVector) {
  double *rvOld;
  double result;
  uint i;
  rvOld = dvector(1, n);
  for (i = 1; i <= n; i++) {
    rvOld[i] = residualVector[i];
    residualVector[i] = residualVector[i] + (xArray[l][i] * (betaOld - betaNew));
  }
  result = 0.0;
  for (i = 1; i <= n; i++) {
    result += pow(residualVector[i], 2.0) - pow(rvOld[i], 2.0);
  }
  result += (lambda * (fabs(betaNew) - fabs(betaOld)));
  free_dvector(rvOld, 1, n);
  return result;
}
void initializeXY(uint p,
                  uint n,
                  double **observation,
                  double  *response,
                  double  *weightDummy,
                  double   weightSumDummy,
                  double  *innerProductXY) {
  double result;
  uint i, j;
  for (j = 1; j <= p; j++) {
    result = 0.0;    
    for (i = 1; i <= n; i++) {
      result += (observation[j][i] * response[i]);
    }
    innerProductXY[j]  = result;
  }
}
void initializeXX(uint p,
                  uint n,
                  double **observation,
                  double  *weight,
                  double   weightSum,
                  double **innerProductXX) {
  double result;
  uint i, j, k;
    for (j = 1; j < p; j++) {  
      for (k = j + 1; k <= p; k++) {
        result = 0.0;
        for (i = 1; i <= n; i++) {
          result += (observation[j][i] * observation[k][i]);
        }
        innerProductXX[j][k] = result;
      }
    }
}
uint cdlFast(uint p,
             uint n,
             double lambdaCurrent,
             uint   maxIter,
             double convergThreshold,
             double  **xArray,
             char     *masterActiveX,
             char     *previouslyActiveX,
             char     *currentlyActiveX,
             double  **innerProductXX,
             double   *gradientResidualVector,
             double   *beta) {
  char runFlag;
  char testFlag;
  uint runCounter;
  uint j;
  runCounter = 0;
  runFlag = TRUE;
  while (runFlag) {
    runCounter++;
    cdlFastComplete(p,
                    n,
                    lambdaCurrent,
                    xArray,
                    masterActiveX,
                    currentlyActiveX,
                    innerProductXX,
                    gradientResidualVector,
                    beta);
    cdlFastActive(p,
                  n,
                  lambdaCurrent,
                  maxIter,
                  convergThreshold,
                  xArray,
                  innerProductXX,
                  currentlyActiveX,
                  gradientResidualVector,
                  beta);
    for (j = 1; j <= p; j++) {
      previouslyActiveX[j] = currentlyActiveX[j];
    }
    cdlFastComplete(p,
                    n,
                    lambdaCurrent,
                    xArray,
                    masterActiveX,
                    currentlyActiveX,
                    innerProductXX,
                    gradientResidualVector,
                    beta);
    testFlag = TRUE;
    for (j = 1; j <= p; j++) {
      if (previouslyActiveX[j] != currentlyActiveX[j]) {
        testFlag = FALSE;
      }
    }
    if (testFlag) {
      runFlag = FALSE;
    }
  }
  return runCounter;
}
void cdlFastComplete(uint p,
                     uint n,
                     double lambdaCurrent,
                     double  **xArray,
                     char     *masterActiveX,
                     char     *currentlyActiveX,
                     double  **innerProductXX,
                     double   *gradientResidualVector,
                     double   *beta) {
  double betaOld, betaNew;
  double result;
  char updateFlag;
  uint i, j, l;
  for (l = 1; l <= p; l++) {
    betaNew = updateBetaGeneric(l,
                                n,
                                lambdaCurrent,
                                beta,
                                xArray,
                                gradientResidualVector);
    betaOld = beta[l];
    beta[l] = betaNew;
    if (fabs(betaNew - betaOld) > SG_EPSILON) {
      updateFlag = TRUE;
    }
    else {
      updateFlag = FALSE;
    }
    if (fabs(betaNew) > SG_EPSILON) {
      currentlyActiveX[l] = TRUE;
    }
    else {
    }
    if ((currentlyActiveX[l] == TRUE) && (masterActiveX[l] == FALSE)) {
      for (j = 1; j <= p; j++) {  
        result = 0.0;
        for (i = 1; i <= n; i++) {
          result += (xArray[l][i] * xArray[j][i]);
        }
        innerProductXX[j][l] = result;
      }
      masterActiveX[l] = TRUE;
    }
    if (updateFlag) {
      updateGradientResidual(l,
                             p,
                             n,
                             lambdaCurrent,
                             betaOld,
                             betaNew,
                             xArray,
                             innerProductXX,
                             gradientResidualVector);
    }
  }
}
void cdlFastActive(uint p,
                   uint n,
                   double lambdaCurrent,
                   uint   maxIter,
                   double convergThreshold,
                   double  **xArray,
                   double  **innerProductXX,
                   char     *currentlyActiveX,
                   double   *gradientResidualVector,
                   double   *beta) {
  double betaOld, betaNew;
  uint activeSet;
  char runFlag;
  char convergFlag;
  uint activeIter;
  double result;
  char updateFlag;
  uint l;
  runFlag = TRUE;
  activeIter = 0;  
  while (runFlag) {
    convergFlag = TRUE;
    activeSet = 0;
    for (l = 1; l <= p; l++) {
      if (currentlyActiveX[l] == TRUE) {
        activeSet ++;
        activeIter ++;
        betaNew = updateBetaGeneric(l,
                                    n,
                                    lambdaCurrent,
                                    beta,
                                    xArray,
                                    gradientResidualVector);
        betaOld = beta[l];
        beta[l] = betaNew;
        if (fabs(betaNew - betaOld) > SG_EPSILON) {
          updateFlag = TRUE;
        }
        else {
          updateFlag = FALSE;
        }
        if (fabs(betaNew) <= SG_EPSILON) {
          currentlyActiveX[l] = FALSE;
        }
        if (updateFlag) {
          result = updateGradientResidual(l,
                                          p,
                                          n,
                                          lambdaCurrent,
                                          betaOld,
                                          betaNew,
                                          xArray,
                                          innerProductXX,
                                          gradientResidualVector);
          if (fabs(result) > convergThreshold * 1.0) {
            convergFlag = FALSE;
          }
        }
      }
    }
    if ((activeIter > maxIter) || (convergFlag) || (activeSet == 0)) {
      runFlag = FALSE;
    }
  }
}
CDLInfo *makeCDLInfo(uint p, uint lambdaCnt) {
  uint i;
  CDLInfo *obj = (CDLInfo*) gblock((size_t) sizeof(CDLInfo));
  obj -> p = p;
  obj -> lambdaCnt = lambdaCnt;
  obj -> lambda   =  dvector(1, lambdaCnt);
  obj -> convgCnt = uivector(1, lambdaCnt);
  obj -> beta = (double **) new_vvector(1, lambdaCnt, NRUTIL_DPTR);
  for (i = 1; i <= lambdaCnt; i++) {
    (obj -> beta)[i] = dvector(1, p + 1);
  }
  obj -> yHat = NULL;
  obj -> yHatCnt = 0;
  obj -> valid = FALSE;
  obj -> lambdaTargetIndx = 0;
  obj -> lambda1SEIndxMin = 0;
  obj -> lambda1SEIndxMax = 0;
  obj -> lambdaMeanErr = NULL;
  obj -> lambdaCVSD = NULL;
  return obj;
}
void freeCDLInfo(CDLInfo *obj) {
  uint i;
  uint p, lambdaCnt;
  p = obj -> p;
  lambdaCnt = obj -> lambdaCnt;
  free_dvector(obj -> lambda, 1, lambdaCnt);
  free_uivector(obj -> convgCnt, 1, lambdaCnt);
  for (i = 1; i <= lambdaCnt; i++) {
    free_dvector((obj -> beta)[i], 1, p + 1);
  }
  free_new_vvector(obj -> beta, 1, lambdaCnt, NRUTIL_DPTR);
  if (obj -> yHatCnt > 0) {
    if (obj -> yHat != NULL) {
      for (i = 1; i <= lambdaCnt; i++) {
        free_dvector(obj -> yHat[i], 1, obj -> yHatCnt);
      }
      free_new_vvector(obj -> yHat, 1, lambdaCnt, NRUTIL_DPTR);
      obj -> yHat = NULL;
      if (obj -> lambdaMeanErr != NULL) {
        free_dvector(obj -> lambdaMeanErr, 1, lambdaCnt);
        obj -> lambdaMeanErr = NULL;
        free_dvector(obj -> lambdaCVSD, 1, lambdaCnt);
        obj -> lambdaCVSD = NULL;
      }
    }
    obj -> yHatCnt = 0;
  }
  free_gblock(obj, (size_t)  sizeof(CDLInfo));
}
double **predictCDL(CDLInfo *obj,
                    uint     tstMembrSize,
                    uint    *tstMembrIndx,
                    double **xArray,
                    uint     lambdaCntMaster,
                    double  *lambdaMaster) {
  uint *alignIndx;
  uint  alignCnt;
  double **beta;
  uint p;
  double resultMin, result;
  double **yHat;
  uint i, j, k;
  if (lambdaMaster == NULL) {
    alignCnt = obj -> lambdaCnt;
    alignIndx = uivector(1, obj -> lambdaCnt);
    for (j = 1; j <= obj -> lambdaCnt; j++) {
      alignIndx[j] = j;
    }
  }
  else {
    alignCnt = lambdaCntMaster;
    alignIndx = uivector(1, lambdaCntMaster);
    for (j = 1; j <= lambdaCntMaster; j++) {
      resultMin = DBL_MAX;
      for (i = 1; i <= obj -> lambdaCnt; i++) {
        result = fabs(obj -> lambda[i] - lambdaMaster[j]);
        if (resultMin > result) {
          resultMin = result;
          alignIndx[j] = i;
        }
      }
    }
  }
  beta = obj -> beta;
  p = obj -> p;
  yHat = new_vvector(1, alignCnt, NRUTIL_DPTR);
  for (j = 1; j <= alignCnt; j++) {
    yHat[j] = dvector(1, tstMembrSize);
  }
  for (j = 1; j <= alignCnt; j++) {
    for (i = 1; i <= tstMembrSize; i++) {
      result = beta[alignIndx[j]][1];
      for (k = 1; k <= p; k++) {
        if (fabs(beta[alignIndx[j]][k+1]) > 0.0) {
          result = result + (beta[alignIndx[j]][k+1] * xArray[k][tstMembrIndx[i]]);
        }
      }
      yHat[j][i] = result;
    }
  }
  free_uivector(alignIndx, 1, alignCnt);
  return yHat;
}
