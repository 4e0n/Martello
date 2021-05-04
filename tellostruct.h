
#ifndef TELLO_STRUCT_H
#define TELLO_STRUCT_H

namespace Tello {
 const int cmdNULL=0;

 const int cmdForward=101;
 const int cmdBackward=102;
 const int cmdLeft=103;
 const int cmdRight=104;
 const int cmdUp=105;
 const int cmdDown=106;
 const int cmdYawL=107;
 const int cmdYawR=108;

 const int cmdFlipF=111;
 const int cmdFlipB=112;
 const int cmdFlipL=113;
 const int cmdFlipR=114;
 const int cmdFlipFL=115;
 const int cmdFlipFR=116;
 const int cmdFlipBL=117;
 const int cmdFlipBR=118;
};

struct TelloStruct {
 bool connected,cmdTakeOff,cmdLand,cmdEmergency,cmdStop;
 bool cmdQuit,sttQuit,vidQuit;
 int cmd;

 int cmdParam1,cmdParam2,cmdParam3,cmdParam4;
 QString ip;

 int speed,battery,time,wifi,sdk; QString sn;
};

#endif
