#ifndef CDL_INFO_H
#define CDL_INFO_H
typedef struct cdlInfo CDLInfo;
struct cdlInfo {
  uint p;
  uint lambdaCnt;
  uint    *convgCnt;
  double  *lambda;
  double **beta;
  double **yHat;
  uint yHatCnt;
  char valid;
  uint lambdaTargetIndx;
  uint lambda1SEIndx;
  double *lambdaMeanErr;
  double *lambdaCVSD;
};
#endif
