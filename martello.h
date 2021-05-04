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

#ifndef MARTELLO_H
#define MARTELLO_H

#include <QApplication>
#include <QThread>
#include <QtGui>
#include <QtNetwork>
#include "tellostruct.h"
#include "tellokeepalive.h"
//#include "tellocmdthread.h"
//#include "tellosttthread.h"
//#include "tellovidthread.h"

class Martello : public QMainWindow {
 Q_OBJECT
 public:
  Martello(QApplication *app,QWidget *parent=0) : QMainWindow(parent) {
   application=app;
   guiWidth=800; guiHeight=600; guiX=500; guiY=80;

   // Setup tello structure for command, state and video threads
   tello.connected=tello.cmdQuit=tello.sttQuit=tello.vidQuit=\
   tello.cmdTakeOff=tello.cmdLand=tello.cmdEmergency=tello.cmdStop=false;
   tello.cmd=Tello::cmdNULL;
   tello.cmdParam1=tello.cmdParam2=tello.cmdParam3=tello.cmdParam4=0;
   tello.ip="192.168.10.1";

   //Setup command, state and video threads
   telloKeepAlive=new TelloKeepAlive(this,&tello,&telloKAMutex,
                                          &telloKATActive);
   connect(telloKeepAlive,SIGNAL(signalTelloConnected()),
           this,SLOT(slotTelloConnected()));
   connect(telloKeepAlive,SIGNAL(signalTelloInfoUpdate()),
           this,SLOT(slotTelloInfoUpdate()));

   // *** GUI ***
   setGeometry(guiX,guiY,guiWidth,guiHeight);
   setFixedSize(guiWidth,guiHeight);

   // *** STATUSBAR ***
   guiStatusBar=new QStatusBar(this);
   guiStatusBar->setGeometry(0,height()-20,width(),20);
   guiStatusBar->show(); setStatusBar(guiStatusBar);

   // *** MENUBAR ***
   menuBar=new QMenuBar(this); menuBar->setFixedWidth(width());
   QMenu *sysMenu=new QMenu("&System",menuBar);
   QMenu *netMenu=new QMenu("&Network",menuBar);
   menuBar->addMenu(sysMenu);
   menuBar->addMenu(netMenu); setMenuBar(menuBar);

   // System Menu
   aboutAction=new QAction("&About..",this);
   aboutAction->setStatusTip("About Octopus-STIM-Client..");
   connect(aboutAction,SIGNAL(triggered()),this,SLOT(slotAbout()));
   quitAction=new QAction("&Quit",this);
   quitAction->setStatusTip("Quit Octopus-STIM-Client");
   connect(quitAction,SIGNAL(triggered()),this,SLOT(slotQuit()));

   // Network Menu
   changeWIFIAction=new QAction("&Change WIFI Settings..",this);
   changeWIFIAction->setStatusTip("Set New SSID and password for Tello..");
   connect(changeWIFIAction,SIGNAL(triggered()),
           this,SLOT(slotChangeWIFI()));

   // Add actions to menus
   sysMenu->addAction(aboutAction); sysMenu->addSeparator();
   sysMenu->addAction(quitAction);
   netMenu->addAction(changeWIFIAction);

   statusLabel=new QLabel(this);
   statusLabel->setGeometry(guiWidth-70,guiHeight-50,60,20);
   speedLabel=new QLabel(this);
   speedLabel->setGeometry(20,guiHeight-50,90,20);
   batteryLabel=new QLabel(this);
   batteryLabel->setGeometry(120,guiHeight-50,90,20);
   timeLabel=new QLabel(this);
   timeLabel->setGeometry(220,guiHeight-50,90,20);
   wifiLabel=new QLabel(this);
   wifiLabel->setGeometry(320,guiHeight-50,90,20);
   sdkLabel=new QLabel(this);
   sdkLabel->setGeometry(420,guiHeight-50,90,20);
   snLabel=new QLabel(this);
   snLabel->setGeometry(520,guiHeight-50,90,20);

   // Acticate loops of all via flag
   telloKATActive=true;
   //telloKeepAlive->start(QThread::HighestPriority);
   telloKeepAlive->start();

   setWindowTitle(
    "MarTELLO v0.9.0 - (c) GPL 2021 Barkin Ilhan - barkin@unrlabs.org");
  }

 protected:
  void keyPressEvent(QKeyEvent *e) {
   if (!e->isAutoRepeat()) {
         if (e->key()==Qt::Key_W) { qDebug() << "W"; tello.cmd=Tello::cmdForward; }
    else if (e->key()==Qt::Key_S) { qDebug() << "S"; tello.cmd=Tello::cmdBackward; }
    else if (e->key()==Qt::Key_A) { qDebug() << "A"; tello.cmd=Tello::cmdLeft; }
    else if (e->key()==Qt::Key_D) { qDebug() << "D"; tello.cmd=Tello::cmdRight; }

    else if (e->key()==Qt::Key_I) { qDebug() << "I"; tello.cmd=Tello::cmdUp; }
    else if (e->key()==Qt::Key_K) { qDebug() << "K"; tello.cmd=Tello::cmdDown; }
    else if (e->key()==Qt::Key_J) { qDebug() << "J"; tello.cmd=Tello::cmdYawL; }
    else if (e->key()==Qt::Key_L) { qDebug() << "L"; tello.cmd=Tello::cmdYawR; }

    else if (e->key()==Qt::Key_1) { qDebug() << "1"; tello.cmdTakeOff=true; }
    else if (e->key()==Qt::Key_2) { qDebug() << "2"; tello.cmdLand=true; }
    else if (e->key()==Qt::Key_0) { qDebug() << "0"; tello.cmdEmergency=true; }
   }
   e->ignore();
  }

  void keyReleaseEvent(QKeyEvent *e) {
//   if (!e->isAutoRepeat()) {
   tello.cmd=Tello::cmdNULL;
   e->ignore();
//   }
  }
  
 private slots:
  void slotAbout() {
   QMessageBox::about(this,"About MarTELLO",
                           "Ryze Tello Brain-Drone Interface Client\n"
                           "(c) 2021 Barkin Ilhan (barkin@unrlabs.org)\n"
                           "This is free software coming with\n"
                           "ABSOLUTELY NO WARRANTY; You are welcome\n"
                         "to redistribute it under conditions of GPL v3.\n");
  }

  void slotQuit() {
   qDebug("MarTELLO: Exiting...");
   //cmd.=false;
   tello.cmdQuit=tello.sttQuit=tello.vidQuit=true;
   telloKATActive=false;
   while (!(telloKeepAlive->isFinished()));
            //telloCmdThread->isFinished() &&
   application->exit(0);
  }

  void slotChangeWIFI() {
	  ;
  }

  void slotTelloConnected() {
   if (tello.connected) statusLabel->setText("CONNECTED");
   else statusLabel->setText("");
  }
  void slotTelloInfoUpdate() {
   speedLabel->setText(QString("Speed: "). \
     append(QString::number(tello.speed).toAscii().data()).append("cm/s"));
   batteryLabel->setText(QString("Battery: ").\
     append(QString::number(tello.battery).toAscii().data()).append("%"));
   timeLabel->setText(QString("F.Time: ").\
     append(QString::number(tello.time)).append("s"));
   wifiLabel->setText(QString("SNR: ").\
     append(QString::number(tello.wifi)).append("dB"));
   sdkLabel->setText(QString("SDKver: ").\
     append(QString::number(tello.sdk)));
//   snLabel->setText(QString("").append(tello.sn));
//   qDebug("Result: %d %d %d %d %d",\
//   tello.speed,tello.battery,tello.time,tello.wifi,tello.sdk);
  }
 
 private:
  QApplication *application;

  // Threads
  QThread *telloKeepAlive;
  QMutex telloKAMutex;
  TelloStruct tello;
  bool telloKATActive;

  // GUI
  int guiX,guiY,guiWidth,guiHeight;

  QMenuBar *menuBar;
  QAction *aboutAction,*quitAction, \
	  *changeWIFIAction;

  QLabel *statusLabel;

  QStatusBar *guiStatusBar;
  QLabel *speedLabel,*batteryLabel,*timeLabel,*wifiLabel,*sdkLabel,*snLabel;
};

#endif

//   connect(telloCmdSocket,SIGNAL(connected()),
//           this,SLOT(slotTelloCmdConnected()));
//   connect(telloCmdSocket,SIGNAL(disconnected()),
//           this,SLOT(slotTelloCmdDisconnected()));
//
//   connect(telloSttSocket,SIGNAL(connected()),
//           this,SLOT(slotTelloSttConnected()));
//   connect(telloSttSocket,SIGNAL(disconnected()),
//           this,SLOT(slotTelloSttDisconnected()));
//
//   connect(telloVidSocket,SIGNAL(connected()),
//           this,SLOT(slotTelloVidConnected()));
//   connect(telloVidSocket,SIGNAL(disconnected()),
//           this,SLOT(slotTelloVidDisconnected()));

//   connect(telloCmdThread,SIGNAL(signalTelloCmdConnect()),
//           this,SLOT(slotTelloCmdConnect()));
//   connect(telloCmdThread,SIGNAL(signalTelloCmdDisconnect()),
//           this,SLOT(slotTelloCmdDisconnect()));
//   connect(telloCmdThread,SIGNAL(signalTelloCmdSend(QByteArray)),
//           this,SLOT(slotTelloCmdSend(QByteArray)));

//   connect(telloSttThread,SIGNAL(signalTelloSttConnect()),
//           this,SLOT(slotTelloSttConnect()));
//   connect(telloSttThread,SIGNAL(signalTelloSttDisconnect()),
//           this,SLOT(slotTelloSttDisconnect()));
//   connect(telloSttThread,SIGNAL(signalTelloSttSend(QByteArray)),
//           this,SLOT(slotTelloSttSend(QByteArray)));

//   connect(telloVidThread,SIGNAL(signalTelloVidConnect()),
//           this,SLOT(slotTelloVidConnect()));
//   connect(telloVidThread,SIGNAL(signalTelloVidDisconnect()),
//           this,SLOT(slotTelloVidDisconnect()));
//   connect(telloVidThread,SIGNAL(signalTelloVidSend(QByteArray)),
//           this,SLOT(slotTelloVidSend(QByteArray)));

  // ***** TELLO COMM SIGNALS FROM THREADS
 
  // CMD
//  void slotTelloCmdConnect() {
//   qDebug("MarTELLO: slotTelloCmdConnect()");
//   telloCmdSocket->connectToHost(tello.ip,8889);
//  }
//  void slotTelloCmdConnected() {
//   qDebug("MarTELLO: slotTelloCmdConnected()");
//   if (telloCmdSocket->state()==QAbstractSocket::ConnectedState &&
//       telloSttSocket->state()==QAbstractSocket::ConnectedState &&
//       telloVidSocket->state()==QAbstractSocket::ConnectedState)
//    statusLabel->setText("CONNECTED");
//   tello.startComm=true;
//  }
//  void slotTelloCmdDisconnect() {
//   qDebug("MarTELLO: slotTelloCmdDisconnect()");
//   telloCmdSocket->disconnectFromHost();
//  }
//  void slotTelloCmdDisconnected() {
//   qDebug("MarTELLO: slotTelloCmdDisconnected()");
//   statusLabel->setText("");
//  }
//  void slotTelloCmdSend(QByteArray c) {
//   qDebug("%s",QString("MarTELLO: slotTelloCmdSend()") \
//		       .append(c).toAscii().data());
//   if (telloCmdSocket->write(c.data())==-1)
//    qDebug("MarTELLO: ERROR !! during data writing to socket.");
//   while (!telloCmdSocket->bytesAvailable());
//   QByteArray ba=telloCmdSocket->readAll();
//   qDebug("%s: %s",QString(c).toAscii().data(),
//		   QString(ba).toAscii().data());
//  }

  // STT
//  void slotTelloSttConnect() {
//   qDebug("MarTELLO: slotTelloSttConnect()");
//   telloSttSocket->connectToHost(tello.ip,8890);
//  }
//  void slotTelloSttConnected() {
//   qDebug("MarTELLO: slotTelloSttConnected()");
//  }
//  void slotTelloSttDisconnect() {
//   qDebug("MarTELLO: slotTelloSttDisconnect()");
//   telloSttSocket->disconnectFromHost();
//  }
//  void slotTelloSttDisconnected() {
//   qDebug("MarTELLO: slotTelloSttDisconnected()");
//  }
//  void slotTelloSttSend(QByteArray c) {
//	  ;
//  }

  // VID
//  void slotTelloVidConnect() {
//   qDebug("MarTELLO: slotTelloVidConnect()");
//   telloVidSocket->connectToHost(tello.ip,11111);
//  }
//  void slotTelloVidConnected() {
//   qDebug("MarTELLO: slotTelloVidConnected()");
//  }
//  void slotTelloVidDisconnect() {
//   qDebug("MarTELLO: slotTelloVidDisconnect()");
//   telloVidSocket->disconnectFromHost();
//  }
//  void slotTelloVidDisconnected() {
//   qDebug("MarTELLO: slotTelloVidDisconnected()");
//  }
//  void slotTelloVidSend(QByteArray c) {
//	  ;
//  }
