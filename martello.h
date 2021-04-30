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
#include "tellocmdthread.h"
#include "tellosttthread.h"
#include "tellovidthread.h"

class Martello : public QMainWindow {
 Q_OBJECT
 public:
  Martello(QApplication *app,QWidget *parent=0) : QMainWindow(parent) {
   application=app;
   guiWidth=800; guiHeight=600; guiX=40; guiY=40;

   // Setup tello structure for command, state and video threads
   tello.startComm= \
   tello.cmdForward=tello.cmdBackward=tello.cmdLeft=tello.cmdRight= \
   tello.cmdUp=tello.cmdDown=tello.cmdCCW=tello.cmdCW= \
   tello.cmdFlip=tello.cmdGo= \
   tello.cmdTakeOff=tello.cmdLand=tello.cmdEmergency=tello.cmdStop=false;
   tello.cmdParam1=tello.cmdParam2=tello.cmdParam3=tello.cmdParam4=0;
   tello.ip="192.168.10.1";

   // Setup sockets
   telloCmdSocket=new QUdpSocket(this);
   telloCmdSocket->bind(QHostAddress(tello.ip),8889);
   telloSttSocket=new QUdpSocket(this);
   telloSttSocket->bind(QHostAddress(tello.ip),8890);
   telloVidSocket=new QUdpSocket(this);
   telloVidSocket->bind(QHostAddress(tello.ip),11111);

   connect(telloCmdSocket,SIGNAL(connected()),
           this,SLOT(slotTelloCmdConnected()));
   connect(telloCmdSocket,SIGNAL(disconnected()),
           this,SLOT(slotTelloCmdDisconnected()));

   connect(telloSttSocket,SIGNAL(connected()),
           this,SLOT(slotTelloSttConnected()));
   connect(telloSttSocket,SIGNAL(disconnected()),
           this,SLOT(slotTelloSttDisconnected()));

   connect(telloVidSocket,SIGNAL(connected()),
           this,SLOT(slotTelloVidConnected()));
   connect(telloVidSocket,SIGNAL(disconnected()),
           this,SLOT(slotTelloVidDisconnected()));

   //Setup command, state and video threads
   telloKeepAlive=new TelloKeepAlive(this,&tello,&telloKAMutex,
                                          &telloKATActive);
   telloCmdThread=new TelloCmdThread(this,&tello,&telloCmdMutex,
                                          &telloCmdTActive,telloCmdSocket);
   telloSttThread=new TelloSttThread(this,&tello,&telloSttMutex,
                                          &telloSttTActive,telloSttSocket);
   telloVidThread=new TelloVidThread(this,&tello,&telloVidMutex,
                                          &telloVidTActive,telloVidSocket);

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

   connect(telloKeepAlive,SIGNAL(signalTelloAlive()),
           this,SLOT(slotTelloAlive()));
   connect(telloKeepAlive,SIGNAL(signalTelloDead()),
           this,SLOT(slotTelloDead()));

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

//   upButton=new QPushButton("Up",this);
//   upButton->setGeometry(200,100,40,40); upButton->show();
//   connect(upButton,SIGNAL(clicked()),this,SLOT(slotUp()));
   statusLabel=new QLabel(this);
   statusLabel->setGeometry(guiWidth-70,guiHeight-50,60,20);

   // Acticate loops of all via flag
   telloKATActive=telloCmdTActive=telloSttTActive=telloVidTActive=true;
   //telloCmdThread->start(QThread::HighestPriority);
   telloKeepAlive->start();
   telloCmdThread->start(); telloSttThread->start(); telloVidThread->start();

   setWindowTitle(
    "MarTELLO v0.9.0 - (c) GPL 2021 Barkin Ilhan - barkin@unrlabs.org");
  }

 protected:
  void keyPressEvent(QKeyEvent *e) {
   if (!e->isAutoRepeat()) {
         if (e->key()==Qt::Key_W) { qDebug() << "W"; tello.cmdForward=true; }
    else if (e->key()==Qt::Key_S) { qDebug() << "S"; tello.cmdBackward=true; }
    else if (e->key()==Qt::Key_A) { qDebug() << "A"; tello.cmdLeft=true; }
    else if (e->key()==Qt::Key_D) { qDebug() << "D"; tello.cmdRight=true; }

    else if (e->key()==Qt::Key_I) { qDebug() << "I"; tello.cmdUp=true; }
    else if (e->key()==Qt::Key_K) { qDebug() << "K"; tello.cmdDown=true; }
    else if (e->key()==Qt::Key_J) { qDebug() << "J"; tello.cmdCCW=true; }
    else if (e->key()==Qt::Key_L) { qDebug() << "L"; tello.cmdCW=true; }

    else if (e->key()==Qt::Key_1) { qDebug() << "1"; tello.cmdTakeOff=true; }
    else if (e->key()==Qt::Key_2) { qDebug() << "2"; tello.cmdLand=true; }
    else if (e->key()==Qt::Key_0) { qDebug() << "0"; tello.cmdEmergency=true; }
   }
   e->ignore();
  }

  void keyReleaseEvent(QKeyEvent *e) {
//   if (!e->isAutoRepeat()) {
        if (e->key()==Qt::Key_W) { qDebug() << "W"; tello.cmdForward=false; }
   else if (e->key()==Qt::Key_S) { qDebug() << "S"; tello.cmdBackward=false; }
   else if (e->key()==Qt::Key_A) { qDebug() << "A"; tello.cmdLeft=false; }
   else if (e->key()==Qt::Key_D) { qDebug() << "D"; tello.cmdRight=false; }

   else if (e->key()==Qt::Key_I) { qDebug() << "I"; tello.cmdUp=false; }
   else if (e->key()==Qt::Key_K) { qDebug() << "K"; tello.cmdDown=false; }
   else if (e->key()==Qt::Key_J) { qDebug() << "J"; tello.cmdCCW=false; }
   else if (e->key()==Qt::Key_L) { qDebug() << "L"; tello.cmdCW=false; }

   else if (e->key()==Qt::Key_1) { qDebug() << "1"; tello.cmdTakeOff=false; }
   else if (e->key()==Qt::Key_2) { qDebug() << "2"; tello.cmdLand=false; }
   else if (e->key()==Qt::Key_0) { qDebug() << "0"; tello.cmdEmergency=false; }
   e->ignore();
//   }
  }
  
 private slots:
  void slotAbout() {
   QMessageBox::about(this,"About MarTELLO",
                           "Ryze Tello Brain-Drone Interface Client\n"
                           "(c) 2020 Barkin Ilhan (barkin@unrlabs.org)\n"
                           "This is free software coming with\n"
                           "ABSOLUTELY NO WARRANTY; You are welcome\n"
                         "to redistribute it under conditions of GPL v3.\n");
  }

  void slotQuit() {
   qDebug("MarTELLO: Exiting...");
   //cmd.=false;
   telloVidSocket->disconnectFromHost(); telloVidTActive=false;
   telloSttSocket->disconnectFromHost(); telloSttTActive=false;
   telloCmdSocket->disconnectFromHost(); telloCmdTActive=false;
   telloKATActive=false;
   while (!(telloKeepAlive->isFinished() &&
            telloCmdThread->isFinished() &&
	    telloSttThread->isFinished() &&
	    telloVidThread->isFinished()));
   if ((telloCmdSocket->state()==QAbstractSocket::UnconnectedState ||
        telloCmdSocket->waitForDisconnected(1000)) &&
       (telloSttSocket->state()==QAbstractSocket::UnconnectedState ||
        telloSttSocket->waitForDisconnected(1000)) &&
       (telloVidSocket->state()==QAbstractSocket::UnconnectedState ||
        telloVidSocket->waitForDisconnected(1000)))
    application->exit(0);
   else { qDebug("MarTELLO: Rude exit!"); application->exit(0); }
  }

  void slotChangeWIFI() {
	  ;
  }

  // ***** TELLO COMM SIGNALS FROM THREADS
 
  // CMD
  void slotTelloCmdConnect() {
   qDebug("MarTELLO: slotTelloCmdConnect()");
//   telloCmdSocket->connectToHost(tello.ip,8889);
  }
  void slotTelloCmdConnected() {
   qDebug("MarTELLO: slotTelloCmdConnected()");
   if (telloCmdSocket->state()==QAbstractSocket::ConnectedState &&
       telloSttSocket->state()==QAbstractSocket::ConnectedState &&
       telloVidSocket->state()==QAbstractSocket::ConnectedState)
    statusLabel->setText("CONNECTED");
//   tello.startComm=true;
  }
  void slotTelloCmdDisconnect() {
   qDebug("MarTELLO: slotTelloCmdDisconnect()");
   telloCmdSocket->disconnectFromHost();
  }
  void slotTelloCmdDisconnected() {
   qDebug("MarTELLO: slotTelloCmdDisconnected()");
   statusLabel->setText("");
  }
  void slotTelloCmdSend(QByteArray c) {
   qDebug("%s",QString("MarTELLO: slotTelloCmdSend()") \
		       .append(c).toAscii().data());
   if (telloCmdSocket->write(c.data())==-1)
    qDebug("MarTELLO: ERROR !! during data writing to socket.");
   while (!telloCmdSocket->bytesAvailable());
   QByteArray ba=telloCmdSocket->readAll();
   qDebug("%s: %s",QString(c).toAscii().data(),
		   QString(ba).toAscii().data());
  }

  // STT
  void slotTelloSttConnect() {
   qDebug("MarTELLO: slotTelloSttConnect()");
//   telloSttSocket->connectToHost(tello.ip,8890);
  }
  void slotTelloSttConnected() {
   qDebug("MarTELLO: slotTelloSttConnected()");
  }
  void slotTelloSttDisconnect() {
   qDebug("MarTELLO: slotTelloSttDisconnect()");
//   telloSttSocket->disconnectFromHost();
  }
  void slotTelloSttDisconnected() {
   qDebug("MarTELLO: slotTelloSttDisconnected()");
  }
  void slotTelloSttSend(QByteArray c) {
	  ;
  }

  // VID
  void slotTelloVidConnect() {
   qDebug("MarTELLO: slotTelloVidConnect()");
//   telloVidSocket->connectToHost(tello.ip,11111);
  }
  void slotTelloVidConnected() {
//   qDebug("MarTELLO: slotTelloVidConnected()");
  }
  void slotTelloVidDisconnect() {
   qDebug("MarTELLO: slotTelloVidDisconnect()");
//   telloVidSocket->disconnectFromHost();
  }
  void slotTelloVidDisconnected() {
   qDebug("MarTELLO: slotTelloVidDisconnected()");
  }
  void slotTelloVidSend(QByteArray c) {
	  ;
  }

  void slotTelloAlive() {
//   statusLabel->setText("CONNECTED");
   telloCmdSocket->connectToHost(tello.ip,8889);
   telloSttSocket->connectToHost(tello.ip,8890);
   telloVidSocket->connectToHost(tello.ip,11111);
//   tello.startComm=true;
  }
  void slotTelloDead() {
   statusLabel->setText("");
  }
 
 private:
  QApplication *application;

  // Threads
  QThread *telloKeepAlive,*telloCmdThread,*telloSttThread,*telloVidThread;
  QMutex telloKAMutex,telloCmdMutex,telloSttMutex,telloVidMutex;
  TelloStruct tello;
  bool telloKATActive,telloCmdTActive,telloSttTActive,telloVidTActive;

  QUdpSocket *telloCmdSocket,*telloSttSocket,*telloVidSocket;

  // GUI
  int guiX,guiY,guiWidth,guiHeight;

  QMenuBar *menuBar;
  QAction *aboutAction,*quitAction, \
	  *changeWIFIAction;

  QLabel *statusLabel;
//  QPushButton *upButton,*downButton,*leftButton,*rightButton;

  QStatusBar *guiStatusBar;
};

#endif
