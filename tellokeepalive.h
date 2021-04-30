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

  virtual void run() {
   qDebug("Keepalive Thread: Activated..");
   QProcess p; //p.setStandardOutputFile("/dev/null");
   QString pingCmd=QString("ping -c 1 -w 1 -q ").append(tello->ip);
   qDebug("%s",pingCmd.toAscii().data());
   while (*active) {
    p.start(pingCmd); p.waitForFinished();
    if (p.exitCode()==0) { emit signalTelloAlive(); msleep(500); }
    else { emit signalTelloDead(); msleep(500); }
   } // while
   qDebug("Keepalive Thread: Stopping..");
  }

 signals:
  void signalTelloAlive();
  void signalTelloDead();

 private:
  QObject *parent; TelloStruct *tello; QMutex *mutex; bool *active;
};

#endif
    //exitCode=QProcess::execute("ping -c 1 -w 1 -q 192.168.10.1");
		   // 2>&1 >/dev/null
