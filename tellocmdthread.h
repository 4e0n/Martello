/*
MarTELLO - Brain-Drone Interface for Ryze Tello Drones
   Copyright (C) 2021 Barkin Ilhan

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If no:t, see <https://www.gnu.org/licenses/>.

 Contact info:
 E-Mail:  barkin@unrlabs.org
 Website: http://icon.unrlabs.org/staff/barkin/
 Repo:    https://github.com/4e0n/
*/

#ifndef TELLO_CMD_THREAD_H
#define TELLO_CMD_THREAD_H

#include <QThread>
#include <QMutex>
#include <QtNetwork>
#include "tellostruct.h"

class TelloCmdThread : public QThread {
 Q_OBJECT
 public:
  TelloCmdThread(QObject* p,TelloStruct *ts,QMutex *m,bool *t) : QThread(p) {
   parent=p; tello=ts; mutex=m; active=t;
  }

  virtual void run() {
   qDebug("Command Thread: Started..");
   QUdpSocket socket; QTime timer; QString cmdResult;
   timer.start();
   while (*active) {
    if (tello->startComm) {
     if (socket.state()==QAbstractSocket::UnconnectedState) {
      //socket.bind(QHostAddress(tello->ip),8889);
      socket.connectToHost(tello->ip,8889);
      if (socket.waitForConnected(1000)) {
       tello->startComm=false; tello->connected=true;
       emit signalTelloConnected();
       cmdResult=telloCmd(&socket,"command");
       qDebug("%s",cmdResult.toAscii().data());
       qDebug("Command Thread: Comm. started.");
      }
     }
    }

    if (tello->connected && timer.elapsed()>=1000) { // Read current info
     tello->speed=QString(telloCmd(&socket,"speed?")).toInt();
     tello->batLevel=QString(telloCmd(&socket,"battery?")).toInt();
     tello->time=QString(telloCmd(&socket,"time?")).toInt();
     tello->snr=QString(telloCmd(&socket,"wifi?")).toInt();
     tello->sdkVer=QString(telloCmd(&socket,"sdk?")).toInt();
//     tello->sn=QString(telloCmd(&socket,"sn?"));
     timer.restart();
     qDebug("Result: %d %d %d %d %d",
		     tello->speed,tello->batLevel,tello->time,\
		     tello->snr,tello->sdkVer);
    }

    if (tello->cmdQuit) {
     socket.disconnectFromHost();
     while (socket.state()!=QAbstractSocket::UnconnectedState);
     *active=false;
     qDebug("Command Thread: Quitting.");
    }
   } // while

   qDebug("Command Thread: Stopped..");
  }

 signals:
  void signalTelloConnected();

 private:
  QByteArray telloCmd(QUdpSocket* socket,QByteArray c) {
   QByteArray result; result="(null)";
   if (socket->write(c.data())==-1)
    qDebug("MarTELLO: (telloCmd) ERROR !! during data writing to socket.");
   while (!socket->bytesAvailable());
   result=socket->readAll();
   //qDebug("COMMAND (%s) : RESULT (%s)",QString(c).toAscii().data(),
//		                       QString(result).toAscii().data());
   return result;
  }

  QObject *parent; TelloStruct *tello; QMutex *mutex; bool *active;
};

#endif
//   telloSttSocket=new QUdpSocket(this);
//   telloSttSocket->bind(QHostAddress(tello.ip),8890);
//   telloVidSocket=new QUdpSocket(this);
//   telloVidSocket->bind(QHostAddress(tello.ip),11111);

//    if (socket.state()==QAbstractSocket::ConnectedState) {
//     qDebug("Command socket connected..");
//    } else {
//     qDebug("Command socket disconnected..");
//     if (tello->startComm) {
//      qDebug("Thread: Started Comm..");
//      tello->startComm=false;
// emit signalTelloCmdSend(QByteArray("command"));
//     } else if (tello->cmdUp) {
//      qDebug("Thread: Up..");
// emit signalTelloCmdSend(QByteArray("up ").append(QByteArray::number(20)));
//     } else if (tello->cmdDown) {
//      qDebug("Thread: Down..");
// emit signalTelloCmdSend(QByteArray("down ").append(QByteArray::number(20)));
//     } else if (tello->cmdLeft) {
//      qDebug("Thread: Left..");
// emit signalTelloCmdSend(QByteArray("left ").append(QByteArray::number(20)));
//     } else if (tello->cmdRight) {
//      qDebug("Thread: Right..");
// emit signalTelloCmdSend(QByteArray("right ").append(QByteArray::number(20)));
//     } else if (tello->cmdForward) {
//      qDebug("Thread: Forward..");
// emit signalTelloCmdSend(QByteArray("forward ").append(QByteArray::number(20)));
//     } else if (tello->cmdBackward) {
//      qDebug("Thread: Backward..");
// emit signalTelloCmdSend(QByteArray("back ").append(QByteArray::number(20)));
//     } else if (tello->cmdCCW) {
//      qDebug("Thread: Rotate left..");
// emit signalTelloCmdSend(QByteArray("ccw ").append(QByteArray::number(1)));
//     } else if (tello->cmdCW) {
//      qDebug("Thread: Rotate right..");
// emit signalTelloCmdSend(QByteArray("cw ").append(QByteArray::number(1)));
//     } else if (tello->cmdFlip) {
//      tello->cmdFlip=false; char dir;
//      switch (tello->cmdParam1) {
//       case 0: dir='f'; qDebug("Thread: Flip forward.."); break;
//       case 1: dir='b'; qDebug("Thread: Flip backward.."); break;
//       case 2: dir='l'; qDebug("Thread: Flip left.."); break;
//       case 3: dir='r'; qDebug("Thread: Flip right.."); break;
//      };
//      qDebug("%s",QByteArray("flip ").append(dir).data());
// emit signalTelloCmdSend(QByteArray("flip ").append(dir));
//     } else if (tello->cmdGo) {
//      tello->cmdGo=false;
// emit signalTelloCmdSend(QByteArray("go ") \
//                        .append(QByteArray::number(tello->cmdParam1)) \
//                        .append(QByteArray::number(tello->cmdParam2)) \
//                        .append(QByteArray::number(tello->cmdParam3)) \
//                        .append(QByteArray::number(tello->cmdParam4)));
//     } else if (tello->cmdTakeOff) {
//      tello->cmdTakeOff=false;
//      qDebug("Thread: Taking off..");
// emit signalTelloCmdSend(QByteArray("takeoff"));
//     } else if (tello->cmdLand) {
//      tello->cmdLand=false;
//      qDebug("Thread: Landing..");
// emit signalTelloCmdSend(QByteArray("land"));
//     } else if (tello->cmdEmergency) {
//      tello->cmdEmergency=false;
//      qDebug("Thread: Motors shut down..");
// emit signalTelloCmdSend(QByteArray("emergency"));
//     } else if (tello->cmdStop) {
//      tello->cmdStop=false;
// emit signalTelloCmdSend(QByteArray("stop"));
//     }
//    }
