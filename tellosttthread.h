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

#ifndef TELLO_STT_THREAD_H
#define TELLO_STT_THREAD_H

#include <QThread>
#include <QMutex>
#include <QtNetwork>
#include "tellostruct.h"

class TelloSttThread : public QThread {
 Q_OBJECT
 public:
  TelloSttThread(QObject* p,TelloStruct *ts,QMutex *m,bool *t) : QThread(p) {
   parent=p; tello=ts; mutex=m; active=t;
   socket=new QUdpSocket(this);
   socket->bind(QHostAddress("192.168.10.1"),8890);
  }

  virtual void run() {
   qDebug("State Thread activated..");
   while (*active) {
//    if (tello->startComm) {
//     tello->startComm=false;
//     socket->connectToHost("192.168.10.1",8889);
//     qDebug("Thread: Connecting to Tello..");
//     while(socket->state()!=QAbstractSocket::ConnectedState);
//     telloCmdSend(QByteArray("command"));
//     connected=true;
//     qDebug("CmdThread: Connected..");
    if (tello->connected) {
     if (tello->cmdUp) {
      qDebug("Thread: Up..");
      telloCmdSend(QByteArray("up ").append(QByteArray::number(20)));
     } else if (tello->cmdDown) {
      qDebug("Thread: Down..");
      telloCmdSend(QByteArray("down ").append(QByteArray::number(20)));
     } else if (tello->cmdLeft) {
      qDebug("Thread: Left..");
      telloCmdSend(QByteArray("left ").append(QByteArray::number(20)));
     } else if (tello->cmdRight) {
      qDebug("Thread: Right..");
      telloCmdSend(QByteArray("right ").append(QByteArray::number(20)));
     } else if (tello->cmdForward) {
      qDebug("Thread: Forward..");
      telloCmdSend(QByteArray("forward ").append(QByteArray::number(20)));
     } else if (tello->cmdBackward) {
      qDebug("Thread: Backward..");
      telloCmdSend(QByteArray("back ").append(QByteArray::number(20)));
     } else if (tello->cmdCCW) {
      qDebug("Thread: Rotate left..");
      telloCmdSend(QByteArray("ccw ").append(QByteArray::number(1)));
     } else if (tello->cmdCW) {
      qDebug("Thread: Rotate right..");
      telloCmdSend(QByteArray("cw ").append(QByteArray::number(1)));
     } else if (tello->cmdFlip) {
      tello->cmdFlip=false; char dir;
      switch (tello->cmdParam1) {
       case 0: dir='f'; qDebug("Thread: Flip forward.."); break;
       case 1: dir='b'; qDebug("Thread: Flip backward.."); break;
       case 2: dir='l'; qDebug("Thread: Flip left.."); break;
       case 3: dir='r'; qDebug("Thread: Flip right.."); break;
      };
      qDebug("%s",QByteArray("flip ").append(dir).data());
      telloCmdSend(QByteArray("flip ").append(dir));
     } else if (tello->cmdGo) {
      tello->cmdGo=false;
      telloCmdSend(QByteArray("go ") \
		      .append(QByteArray::number(tello->cmdParam1)) \
                      .append(QByteArray::number(tello->cmdParam2)) \
                      .append(QByteArray::number(tello->cmdParam3)) \
                      .append(QByteArray::number(tello->cmdParam4)));
     } else if (tello->cmdTakeOff) {
      tello->cmdTakeOff=false;
      qDebug("Thread: Taking off..");
      telloCmdSend(QByteArray("takeoff"));
     } else if (tello->cmdLand) {
      tello->cmdLand=false;
      qDebug("Thread: Landing..");
      telloCmdSend(QByteArray("land"));
     } else if (tello->cmdEmergency) {
      tello->cmdEmergency=false;
      qDebug("Thread: Motors shut down..");
      telloCmdSend(QByteArray("emergency"));
     } else if (tello->cmdStop) {
      tello->cmdStop=false;
      telloCmdSend(QByteArray("stop"));
     }
    }
   } // while

   qDebug("State Thread stopping..");
  }
//      mutex->lock();
//      mutex->unlock();
//      emit sendData();
// signals:
//  void sendData();

  void telloCmdSend(QByteArray c) {
   if (socket->write(c.data())==-1)
    qDebug("ERROR: telloCmdSend.");
   while (!socket->bytesAvailable());
   QByteArray ba=socket->readAll();
   qDebug("%s: %s",QString(c).toAscii().data(),
		   QString(ba).toAscii().data());
  }

 private:
  QObject *parent; TelloStruct *tello; QMutex *mutex; bool *active;
  QUdpSocket *socket;
};

#endif

//   telloCmdSend(QByteArray("stop"));
//  void telloCmdSend(QByteArray cmd) {
//   //if (socket->writeDatagram(cmd,
//	                             QHostAddress("192.168.10.1"),8889)==-1)
//   if (socket->write(cmd.data())==-1)
//    qDebug("ERROR: telloCmdSend.");
//   while (!socket->bytesAvailable());
//   QByteArray ba=socket->readAll();
//   qDebug("%s: %s",QString(cmd).toAscii().data(),
//		     QString(ba).toAscii().data());
//  }
