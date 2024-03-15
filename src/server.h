#ifndef RF_SERVER_H
#define RF_SERVER_H
#define SG_TCP_PORT 6666
#define SG_TCP_BACKLOG 32
#define SG_TCP_TIMEOUT 15
#define SG_TCP_POLLING 50
#define SG_TCP_CTRL_NAK   0x00 
#define SG_TCP_CTRL_ACK   0x01 
#define SG_TCP_CTRL_QRY   0x02 
#define SG_TCP_CTRL_NSZ   0x03 
#define SG_TCP_CTRL_EOF   0x04 
#define SG_TCP_CTRL_EOD   0x05 
#define SG_TCP_CTRL_STP   0x06 
#define SG_SOCK_STATE_OPN  0x11
#define SG_SOCK_STATE_LEN  0x12
#define SG_SOCK_STATE_DTA  0x13
void server(uint port, time_t userTimeout, time_t polling, uint xSize, uint pSize, DescriptorObj *headDO);
int serverSend(int clientSockFD, char *record, int length);
int flushReadBuffer(int clientSockFD, DescriptorObj *currDO, char *closeConn);
extern void predictForestRT();
#endif