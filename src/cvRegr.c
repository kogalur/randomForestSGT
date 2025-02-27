
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "cvRegr.h"
#include "cvUtil.h"
#include "cdlRegr.h"
#include "cdlUtil.h"
#include "cdlInfo.h"
#include "nrutilDerived.h"
#include "shared/nrutil.h"
#include "shared/error.h"
#include "stackOutput.h"
CDLInfo *crossValidationRegr(uint     threadID,
                             double  *lambdaUser,
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
  CDLInfo  *cdrMaster;
  double resultMin;
  uint   lambdaMinIndx;
  uint *nIndx;
  uint nfoldsValidCnt;
  if (nIndxIn == NULL) {
    nIndx = uivector(1, nIndxSize);
    for (uint m = 1; m <= nIndxSize; m++) {
      nIndx[m] = m;
    }
  }
  else {
    nIndx = nIndxIn;
  }
  cdrMaster = coordinateDescentRegr(lambdaUser,
                                    lambdaGridSize,
                                    lambdaMinRatio,
                                    lambdaEpsilon,                           
                                    convergThreshold,
                                    maxIter,
                                    efficiency,
                                    weight,
                                    nIndxSize,
                                    pSize,
                                    nIndx,
                                    xPermIn,
                                    nfolds,
                                    responseIn,
                                    observationIn);
  uint    lambdaCntMaster = cdrMaster -> lambdaCnt;
  double *lambdaMaster    = cdrMaster -> lambda;
  double **yHatMaster = predictCDL(cdrMaster, nIndxSize, nIndx, observationIn, 0, NULL); 
  cdrMaster -> yHat = yHatMaster;
  cdrMaster -> yHatCnt = nIndxSize;
  if (cdrMaster -> valid == FALSE) {
  }
  else if ((nfolds == 0) || (nfolds == 1)) {
  }
  else if (nfolds > nIndxSize) {
    cdrMaster -> valid = FALSE;
  }
  else {
    FoldParms *fpObj = makeFoldParmsObject(threadID, nIndxSize, nfolds);
    Fold **fObjVector = new_vvectorDerived(1, nfolds, NRUTIL_FOLDPTR);
    CDLInfo **cdlObjVector = new_vvectorDerived(1, nfolds, NRUTIL_CDLIPTR);
    uint **ibgIndxIn = new_vvector(1, nfolds, NRUTIL_UPTR);
    uint **oobIndxIn = new_vvector(1, nfolds, NRUTIL_UPTR);
    double **mseMatrix = new_vvector(1, nfolds, NRUTIL_DPTR);
    double *meanError = dvector(1, lambdaCntMaster);
    double *sdError   = dvector(1, lambdaCntMaster);
    nfoldsValidCnt = 0;
#ifdef _OPENMP
#pragma omp parallel for num_threads(RF_numThreads) if (SG_threadV == SG_THREADV_NFOLD)
#endif
    for (uint i = 1; i <= nfolds; i++) {
      uint ibgSizeIn, oobSizeIn;
      uint j, k;
      fObjVector[i] = makeFoldObject(i, fpObj);
      ibgSizeIn = fObjVector[i] -> ibgSize;
      ibgIndxIn[i] = uivector(1, ibgSizeIn);
      oobSizeIn = fObjVector[i] -> oobSize;
      oobIndxIn[i] = uivector(1, oobSizeIn);
      for (k = 1; k <= ibgSizeIn; k++) {
        ibgIndxIn[i][k] = nIndx[ fObjVector[i] -> ibgIndx[k] ];
      }        
      for (k = 1; k <= oobSizeIn; k++) {
        oobIndxIn[i][k] = nIndx[ fObjVector[i] -> oobIndx[k] ];
      }
      cdlObjVector[i] = coordinateDescentRegr(lambdaUser,
                                              lambdaGridSize,
                                              lambdaMinRatio,
                                              lambdaEpsilon,                           
                                              convergThreshold,
                                              maxIter,
                                              efficiency,
                                              weight,
                                              ibgSizeIn,
                                              pSize,
                                              ibgIndxIn[i],
                                              xPermIn,
                                              0,
                                              responseIn,
                                              observationIn);
      if (cdlObjVector[i] -> valid == TRUE) {
        double result;
        double **yHatFold = predictCDL(cdlObjVector[i],
                                       oobSizeIn,
                                       oobIndxIn[i], 
                                       observationIn,
                                       lambdaCntMaster,
                                       lambdaMaster);
        mseMatrix[i] = dvector(1, lambdaCntMaster);
        for (j = 1; j <= lambdaCntMaster; j++) {
          result = 0.0;
          for (k = 1; k <= oobSizeIn; k++) {
            result += pow (responseIn[oobIndxIn[i][k]] - yHatFold[j][k], 2.0);
          }
          result = result / oobSizeIn;
          mseMatrix[i][j] = result;
        }
        for (j = 1; j <= lambdaCntMaster; j++) {
          free_dvector(yHatFold[j], 1, oobSizeIn);
        }
        free_new_vvector(yHatFold, 1, lambdaCntMaster, NRUTIL_DPTR);
      }
      else {
      }
    }  
    uint i, j;
    for (i = 1; i <= nfolds; i++) {
      if (cdlObjVector[i] -> valid == TRUE) {
        nfoldsValidCnt ++;
      }
    }
    if (nfoldsValidCnt > 0) {
      uint l;
      cdrMaster -> lambdaMeanErr = dvector(1, lambdaCntMaster);
      cdrMaster -> lambdaCVSD    = dvector(1, lambdaCntMaster);
      for (j = 1; j <= lambdaCntMaster; j++) {
        meanError[j] = 0.0;
        for (i = 1; i <= nfolds; i++) {
          if (cdlObjVector[i] -> valid == TRUE) {
            meanError[j] += mseMatrix[i][j];
          }
        }
        (cdrMaster -> lambdaMeanErr)[j] = meanError[j] = meanError[j] / nfoldsValidCnt;
      }
      for (j = 1; j <= lambdaCntMaster; j++) {
        sdError[j] = 0.0;
        for (i = 1; i <= nfolds; i++) {
          if (cdlObjVector[i] -> valid == TRUE) {
            sdError[j] += pow (mseMatrix[i][j] - meanError[j], 2.0);
          }
        }
        if (nfoldsValidCnt > 0) {
          sdError[j] = sdError[j] / nfoldsValidCnt;
          sdError[j] = sqrt(sdError[j] / nfoldsValidCnt);
          (cdrMaster -> lambdaCVSD)[j] = sdError[j];
        }
      }
      resultMin = DBL_MAX;
      lambdaMinIndx = 0;
      for (j = 1; j <= lambdaCntMaster; j++) {
        if (resultMin > meanError[j]) {
          resultMin = meanError[j];
          lambdaMinIndx = j;
        }
      }
      for (j = 1; j <= lambdaCntMaster; j++) {
        if (meanError[j] <= meanError[lambdaMinIndx] + sdError[lambdaMinIndx]) {
          cdrMaster -> lambda1SEIndxMin = j;
        }
      }
      for (j = 1; j <= lambdaCntMaster; j++) {
        if (meanError[j] <= meanError[lambdaMinIndx] + sdError[lambdaMinIndx]) {
          cdrMaster -> lambda1SEIndxMax = j;
          j = lambdaCntMaster;
        }
      }
      cdrMaster -> lambdaTargetIndx = lambdaMinIndx;
      cdrMaster -> valid = FALSE;
      l = 0;
      while ((cdrMaster -> valid == FALSE) && (l < pSize)) {
        l ++;
        if (fabs(cdrMaster -> beta[lambdaMinIndx][l + 1]) > EPSILON) {
          cdrMaster -> valid = TRUE;
        }
      }
      if (SG_optLocal & SG_OPT_SWTCH_SEVEN) {
        if (cdrMaster -> valid == FALSE) {
          l = 0;
          while ((cdrMaster -> valid == FALSE) && (l < pSize)) {
            l ++;
            if (fabs(cdrMaster -> beta[cdrMaster -> lambda1SEIndxMin][l + 1]) > EPSILON) {
              cdrMaster -> valid = TRUE;
              cdrMaster -> lambdaTargetIndx = cdrMaster -> lambda1SEIndxMin;
            }
          }
        }
      }
    }  
    else {
      cdrMaster -> valid = FALSE;
    }
    for (i = 1; i <= nfolds; i++) {
      uint ibgSizeIn, oobSizeIn;
      ibgSizeIn = fObjVector[i] -> ibgSize;
      free_uivector(ibgIndxIn[i], 1, ibgSizeIn);
      oobSizeIn = fObjVector[i] -> oobSize;
      free_uivector(oobIndxIn[i], 1, oobSizeIn);
      if (cdlObjVector[i] -> valid == TRUE) {
        free_dvector(mseMatrix[i], 1, lambdaCntMaster);
      }
    }
    free_dvector(meanError, 1, lambdaCntMaster);
    free_dvector(sdError,   1, lambdaCntMaster);
    free_new_vvector(mseMatrix, 1, nfolds, NRUTIL_DPTR);
    free_new_vvector(ibgIndxIn, 1, nfolds, NRUTIL_UPTR);
    free_new_vvector(oobIndxIn, 1, nfolds, NRUTIL_UPTR);
    for (i = 1; i <= nfolds; i++) {       
      freeCDLInfo(cdlObjVector[i]);
    }
    free_new_vvectorDerived(cdlObjVector, 1, nfolds, NRUTIL_FOLDPTR);
    for (i = 1; i <= nfolds; i++) {   
      freeFoldObject(fpObj, fObjVector[i]);
    }
    free_new_vvectorDerived(fObjVector, 1, nfolds, NRUTIL_FOLDPTR);
    freeFoldParmsObject(fpObj);
  }
  if (nIndxIn == NULL) {
    free_uivector(nIndx, 1, nIndxSize);
  }
  else {
  }
  return cdrMaster;
}
