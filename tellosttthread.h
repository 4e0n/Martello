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
  }

  virtual void run() {
   qDebug("State Thread: Started..");
   QUdpSocket socket; QString cmdResult;
   while (*active) {
    if (tello->connected) {
     qDebug("State Thread: Connected..");
     if (socket.state()==QAbstractSocket::UnconnectedState) {
      socket.bind(QHostAddress(tello->ip),8890);
      socket.connectToHost(tello->ip,8890);
      while (socket.state()!=QAbstractSocket::ConnectedState);
     }

//     int height=(telloCmd(&socket,"h")).toInt();
//     int batLevel=(telloCmd(&socket,"battery")).toInt();
//     int baro=(telloCmd(&socket,"baro")).toInt();
//     qDebug("Height: %d, Bat: %d, Baro: %d",height,batLevel,baro);

     msleep(1000);

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

 private:
  QString telloCmd(QUdpSocket* socket,QByteArray c) {
   QString result; result="(null)";
   qDebug("%s",QString("MarTELLO: slotTelloCmdSend()") \
		       .append(c).toAscii().data());
   if (socket->write(c.data())==-1)
    qDebug("MarTELLO: (telloCmd) ERROR !! during data writing to socket.");
   while (!socket->bytesAvailable());
   QByteArray ba=socket->readAll(); result=QString(ba);
   qDebug("%s: %s",QString(c).toAscii().data(),
		   result.toAscii().data());
   return result;
  }

  QObject *parent; TelloStruct *tello; QMutex *mutex; bool *active;
};

#endif
