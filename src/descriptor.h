#ifndef RF_DESCRIPTOR_H
#define RF_DESCRIPTOR_H
#define SG_TCP_BUFFER_SIZE 0xFFFF
#define SG_DESC_CLOSED     0x20
#define SG_DESC_OPENED     0x21
#define SG_DESC_PREREAD    0x22
#define SG_DESC_READING    0x23
#define SG_DESC_POSTREAD   0x24
#define SG_DESC_PREDICTING 0x25
#define SG_DESC_WRITING    0x26
typedef struct descriptorObj DescriptorObj;
struct descriptorObj {
  int descID;
  struct descriptorObj *fwdLink;
  struct descriptorObj *bakLink;
  char *record;
  uint16_t len;
  uint nSizeAlloc;
  uint nSize;
  uint *nIndx;
  uint nSizeRead;
  uint mSize;
  uint *mIndx;
  uint xSize;
  uint pSize;
  uint augmSize;
  double  *yHat;
  double **fxArrayIn;
  double **fxArrayInAugm;
  double  *fyArrayIn;
  char userState;
  char socketState;
};
DescriptorObj *makeDescriptorObj(uint xSize, uint pSize, int descID, char sockState);
void setDescriptorObj(DescriptorObj *obj, uint nSizeAlloc, char respInFlag);
void resetDescriptorObj(DescriptorObj *obj, char socketState);
void freeDescriptorObj(DescriptorObj *obj);
uint unlinkUnheldDescriptors(DescriptorObj *headDO, DescriptorObj **tailDO, char force);
#endif
