#ifndef  RF_SNP_AUXILIARY_INFO_H
#define  RF_SNP_AUXILIARY_INFO_H
typedef struct snpAuxiliaryInfo SNPAuxiliaryInfo;
struct snpAuxiliaryInfo {
  char type;
  char *identity;
  uint slot;
  ulong linearSize;
  void *snpPtr;
  void *auxiliaryArrayPtr;
  uint dimSize;
  int *dim;
};
typedef struct auxiliaryDimensionConstants AuxiliaryDimensionConstants;
struct auxiliaryDimensionConstants {
  uint *rFactorSize;
  uint *rFactorMap;
  uint *rTargetFactor;
  uint *tLeafCount;
  uint *holdBLKptr;
};
#endif
