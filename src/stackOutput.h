#ifndef RF_STACK_OUTPUT_H
#define RF_STACK_OUTPUT_H
#include "cdlInfo.h"
void stackCDLOutput(uint       lambdaCountIn,
                    uint       p,
                    uint      *lambdaCount,
                    double   **lambda,
                    uint     **convergenceCount,
                    double  ***beta);
void unstackCDLOutput(uint      lambdaCountIn,
                      uint      p,
                      double   *lambda,
                      uint     *convergenceCount,
                      double  **beta);
void stackCoordinateDescentOutput(CDLInfo   *obj,
                                  uint    **lambdaCount,
                                  double  **lambda,
                                  uint    **convergenceCount,
                                  double   **beta,
                                  double  ***betaPtr,
                                  double   **yHat,
                                  double  ***yHatPtr,
                                  uint     **lambdaMinIndx,
                                  uint     **lambda1SEIndxMin,
                                  uint     **lambda1SEIndxMax,
                                  double   **lambdaMeanErr,
                                  double   **lambdaCVSD);
void stackDefinedOutputObjects(char mode);
void unstackDefinedOutputObjects(char mode);
void stackForestObjectsPtrOnly(char mode);
void stackTreeObjectsPtrOnly(char mode, uint treeID);
void unstackTreeObjectsPtrOnly(uint treeID);
void unstackForestObjectsPtrOnly(char mode);
void stackForestObjectsOutput(char mode);
void writeForestObjectsOutput(char mode);
void stackForestObjectsAuxOnlySGT(void);
void unstackForestObjectsAuxOnlySGT(void);
#endif
