
#ifndef TELLO_STRUCT_H
#define TELLO_STRUCT_H

struct TelloStruct {
 bool pollConnection,startComm;

 bool cmdForward,cmdBackward,cmdLeft,cmdRight;
 bool cmdUp,cmdDown,cmdCCW,cmdCW;
 bool cmdFlip,cmdGo;

 bool cmdTakeOff,cmdLand,cmdEmergency,cmdStop;

 int cmdParam1,cmdParam2,cmdParam3,cmdParam4;
};

#endif
