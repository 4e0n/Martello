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

#ifndef TELLO_KEEPALIVE_H
#define TELLO_KEEPALIVE_H

#include <QThread>
#include <QMutex>
#include <QtNetwork>
#include "tellostruct.h"

class TelloKeepAlive : public QThread {
 Q_OBJECT
 public:
  TelloKeepAlive(QObject* p,TelloStruct *ts,QMutex *m,bool *t) : QThread(p) {
   parent=p; tello=ts; mutex=m; active=t;
  }

 protected:
  virtual void run() {
   QTime timer1,timer2,timer3,respGrace; bool wasDead,wasAlive; int sba;
   QByteArray cmd; QString cmdResult; QProcess p; QString pingCmd;
   qDebug("Keepalive Thread: Activated..");

   wasDead=true; wasAlive=true; socket=new QUdpSocket();
   p.setStandardOutputFile("/dev/null");
   pingCmd=QString("ping -c 1 -w 1 -q ").append(tello->ip);
   //qDebug("%s",pingCmd.toAscii().data());
 
   timer1.start(); timer2.start(); respGrace.start();
   while (*active) {
    if (timer1.elapsed()>=250) { // KEEP ALIVE & AUTOCONNECT - ITSELF
     p.start(pingCmd); p.waitForFinished();
     if (p.exitCode()==0) { // PING ALIVE
      if (wasDead) { wasDead=false; wasAlive=true;

       // IF UNCONNECTED, CONNECT
       if (socket->state()==QAbstractSocket::UnconnectedState)
        socket->connectToHost(tello->ip,8889);
       while (socket->state()!=QAbstractSocket::ConnectedState);

       // ENTER COMMAND MODE AFTER CONNECTION
       if (socket->write(QByteArray("command").data())==-1)
        qDebug("ERROR !! during data writing to socket->");

       // WAIT FOR OK, WITHOUT ANY ENTRAPMENT INSIDE WHILE LOOP (2seconds max)
       respGrace.restart();
    //while(respGrace.elapsed()<500 && (sba=socket->pendingDatagramSize())<=0);
       while(respGrace.elapsed()<500 && (sba=socket->bytesAvailable())<=0);
       if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
        if (cmdResult=="ok") {
         tello->connected=true;
         emit signalTelloConnected();
        }
       }
      }

     } else { // PING DEAD
      if (wasAlive) { wasAlive=false; wasDead=true;
       tello->connected=false;
       emit signalTelloConnected();
      }
     }
     timer1.restart();

    } // timer1

    if (tello->connected) {

     if (timer2.elapsed()>=250) {
     // MAIN GETINFO LOOP - ONLY IF CONNECTED

     // ***** GET INFO *****

      // CURRENT SPEED
      if (socket->write(QByteArray("speed?").data())==-1)
       qDebug("ERROR !! during data writing to socket->");
      respGrace.restart();
      while(respGrace.elapsed()<500 && (sba=socket->bytesAvailable())<=0);
      if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
       cmdResult=QString(socket->readAll()).toAscii();
       tello->speed=cmdResult.toInt();
      }
  
      // CURRENT BATTERY %
      if (socket->write(QByteArray("battery?").data())==-1)
       qDebug("ERROR !! during data writing to socket->");
      respGrace.restart();
      while(respGrace.elapsed()<500 && (sba=socket->bytesAvailable())<=0);
      if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
       cmdResult=QString(socket->readAll()).toAscii();
       tello->battery=cmdResult.toInt();
      }

      // CURRENT FLIGHT TIME
      if (socket->write(QByteArray("time?").data())==-1)
       qDebug("ERROR !! during data writing to socket->");
      respGrace.restart();
      while(respGrace.elapsed()<500 && (sba=socket->bytesAvailable())<=0);
      if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
       cmdResult=QString(socket->readAll()).toAscii();
       tello->time=cmdResult.toInt();
      }
 
      // CURRENT WIFI SNR LEVEL
      if (socket->write(QByteArray("wifi?").data())==-1)
       qDebug("ERROR !! during data writing to socket->");
      respGrace.restart();
      while(respGrace.elapsed()<500 && (sba=socket->bytesAvailable())<=0);
      if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
       cmdResult=QString(socket->readAll()).toAscii();
       tello->wifi=cmdResult.toInt();
      }

      // SDK VERSION
      //if (socket->write(QByteArray("sdk?").data())==-1)
      // qDebug("ERROR !! during data writing to socket->");
      //respGrace.restart();
      //while(respGrace.elapsed()<500 && (sba=socket->bytesAvailable())<=0);
      //if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
      // cmdResult=QString(socket->readAll()).toAscii();
      // tello->sdk=QString(socket->readAll()).toAscii().toInt();
      //}
      emit signalTelloInfoUpdate();
      timer2.restart();
     } // timer2

     //if (timer3.elapsed()>=100) {
     if (true) {
      // MAIN APPLYCOMMAND LOOP - ONLY IF CONNECTED

      // TAKEOFF, LAND, and EMERGENCY ARE HIGH PRIORITY
      if (tello->cmdTakeOff && !tello->cmdLand && !tello->cmdEmergency) {
       qDebug("TAKEOFF");
       socket->write(QByteArray("takeoff").data());
       respGrace.restart();
       while(respGrace.elapsed()<5000 && (sba=socket->bytesAvailable())<=0);
       if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
       tello->cmdTakeOff=false;
       qDebug("takeoff: %s",cmdResult.toAscii().data()); }
      } else if (!tello->cmdTakeOff && tello->cmdLand && !tello->cmdEmergency) {
       qDebug("LAND");
       socket->write(QByteArray("land").data());
       respGrace.restart();
       while(respGrace.elapsed()<5000 && (sba=socket->bytesAvailable())<=0);
       if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
       tello->cmdLand=false;
       qDebug("land: %s",cmdResult.toAscii().data()); }
      } else if (!tello->cmdTakeOff && !tello->cmdLand && tello->cmdEmergency) {
       qDebug("EMERGENCY");
       socket->write(QByteArray("emergency").data());
       respGrace.restart();
       while(respGrace.elapsed()<5000 && (sba=socket->bytesAvailable())<=0);
       if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
       tello->cmdEmergency=false;
       qDebug("emergency: %s",cmdResult.toAscii().data()); }
      } else if (tello->cmd!=Tello::cmdNULL) {

       // MOVEMENT DUE TO SPECIFIC KEYSTROKES (WSAD,IKJL)
       if (tello->cmd==Tello::cmdForward)
//        qDebug("Forward");
        socket->write(QByteArray("forward 20").data());
       else if (tello->cmd==Tello::cmdBackward)
//        qDebug("Backward");
        socket->write(QByteArray("backward 20").data());
       else if (tello->cmd==Tello::cmdLeft)
//        qDebug("Left");
        socket->write(QByteArray("left 20").data());
       else if (tello->cmd==Tello::cmdRight)
//        qDebug("Right");
        socket->write(QByteArray("right 20").data());
       else if (tello->cmd==Tello::cmdUp)
//        qDebug("Up");
        socket->write(QByteArray("up 20").data());
       else if (tello->cmd==Tello::cmdDown)
//        qDebug("Down");
        socket->write(QByteArray("down 20").data());
       else if (tello->cmd==Tello::cmdYawL)
//        qDebug("YawL");
        socket->write(QByteArray("ccw 10").data());
       else if (tello->cmd==Tello::cmdYawR)
//        qDebug("YawR");
        socket->write(QByteArray("cw 10").data());
       respGrace.restart();
       while(respGrace.elapsed()<1000 && (sba=socket->bytesAvailable())<=0);
       if (sba>0) { cmdResult=QString(socket->readAll()).toAscii();
//        qDebug("Move result: %s",cmdResult.toAscii().data());
       }
      }
     }
     timer3.restart();
    } // timer3
   } // while
   delete socket;
   qDebug("Keepalive Thread: Stopping..");
  }

 signals:
  void signalTelloConnected();
  void signalTelloInfoUpdate();

 private:
  QObject *parent; TelloStruct *tello; QMutex *mutex; bool *active;
  QUdpSocket *socket;
};

#endif
