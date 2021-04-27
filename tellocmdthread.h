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
  TelloCmdThread(QObject* p,TelloStruct *ts,QMutex *m,bool *t, \
		 QUdpSocket *s): QThread(p) {
   parent=p; tello=ts; mutex=m; active=t; socket=s;
  }

  virtual void run() {
   qDebug("Command Thread activated..");
   while (*active) {
    if (tello->pollConnection && \
        (socket->state()!=QAbstractSocket::ConnectedState)) {
 emit signalTelloCmdConnect();
    } else if (socket->state()==QAbstractSocket::ConnectedState) {
     if (!tello->pollConnection) {
 emit signalTelloCmdDisconnect();
     } else {
      if (tello->startComm) {
       qDebug("Thread: Started Comm..");
       tello->startComm=false;
 emit signalTelloCmdSend(QByteArray("command"));
      } else if (tello->cmdUp) {
       qDebug("Thread: Up..");
 emit signalTelloCmdSend(QByteArray("up ").append(QByteArray::number(20)));
      } else if (tello->cmdDown) {
       qDebug("Thread: Down..");
 emit signalTelloCmdSend(QByteArray("down ").append(QByteArray::number(20)));
      } else if (tello->cmdLeft) {
       qDebug("Thread: Left..");
 emit signalTelloCmdSend(QByteArray("left ").append(QByteArray::number(20)));
      } else if (tello->cmdRight) {
       qDebug("Thread: Right..");
 emit signalTelloCmdSend(QByteArray("right ").append(QByteArray::number(20)));
      } else if (tello->cmdForward) {
       qDebug("Thread: Forward..");
 emit signalTelloCmdSend(QByteArray("forward ").append(QByteArray::number(20)));
      } else if (tello->cmdBackward) {
       qDebug("Thread: Backward..");
 emit signalTelloCmdSend(QByteArray("back ").append(QByteArray::number(20)));
      } else if (tello->cmdCCW) {
       qDebug("Thread: Rotate left..");
 emit signalTelloCmdSend(QByteArray("ccw ").append(QByteArray::number(1)));
      } else if (tello->cmdCW) {
       qDebug("Thread: Rotate right..");
 emit signalTelloCmdSend(QByteArray("cw ").append(QByteArray::number(1)));
      } else if (tello->cmdFlip) {
       tello->cmdFlip=false; char dir;
       switch (tello->cmdParam1) {
        case 0: dir='f'; qDebug("Thread: Flip forward.."); break;
        case 1: dir='b'; qDebug("Thread: Flip backward.."); break;
        case 2: dir='l'; qDebug("Thread: Flip left.."); break;
        case 3: dir='r'; qDebug("Thread: Flip right.."); break;
       };
       qDebug("%s",QByteArray("flip ").append(dir).data());
 emit signalTelloCmdSend(QByteArray("flip ").append(dir));
      } else if (tello->cmdGo) {
       tello->cmdGo=false;
 emit signalTelloCmdSend(QByteArray("go ") \
                         .append(QByteArray::number(tello->cmdParam1)) \
                         .append(QByteArray::number(tello->cmdParam2)) \
                         .append(QByteArray::number(tello->cmdParam3)) \
                         .append(QByteArray::number(tello->cmdParam4)));
      } else if (tello->cmdTakeOff) {
       tello->cmdTakeOff=false;
       qDebug("Thread: Taking off..");
 emit signalTelloCmdSend(QByteArray("takeoff"));
      } else if (tello->cmdLand) {
       tello->cmdLand=false;
       qDebug("Thread: Landing..");
 emit signalTelloCmdSend(QByteArray("land"));
      } else if (tello->cmdEmergency) {
       tello->cmdEmergency=false;
       qDebug("Thread: Motors shut down..");
 emit signalTelloCmdSend(QByteArray("emergency"));
      } else if (tello->cmdStop) {
       tello->cmdStop=false;
 emit signalTelloCmdSend(QByteArray("stop"));
      }
     }
    }
   } // while

   qDebug("Command Thread stopping..");
  }

 signals:
  void signalTelloCmdConnect();
  void signalTelloCmdDisconnect();
  void signalTelloCmdSend(QByteArray);

 private:
  QObject *parent; TelloStruct *tello; QMutex *mutex; bool *active;
  QUdpSocket *socket;
};

#endif
