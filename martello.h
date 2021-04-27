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
   tello.pollConnection=tello.connected=tello.startComm= \
   tello.cmdForward=tello.cmdBackward=tello.cmdLeft=tello.cmdRight= \
   tello.cmdUp=tello.cmdDown=tello.cmdCCW=tello.cmdCW= \
   tello.cmdFlip=tello.cmdGo= \
   tello.cmdTakeOff=tello.cmdLand=tello.cmdEmergency=tello.cmdStop=false;
   tello.cmdParam1=tello.cmdParam2=tello.cmdParam3=tello.cmdParam4=false;

   //Setup command, state and video threads
   telloCmdTActive=telloSttTActive=telloVidTActive=true;
   telloCmdThread=new TelloCmdThread(this,&tello,&telloCmdMutex,
		                          &telloCmdTActive);
   telloSttThread=new TelloSttThread(this,&tello,&telloSttMutex,
		                          &telloSttTActive);
   telloVidThread=new TelloVidThread(this,&tello,&telloVidMutex,
		                          &telloVidTActive);
   //telloCmdThread->start(QThread::HighestPriority);
   telloCmdThread->start(); telloSttThread->start(); telloVidThread->start();

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
   connectAction=new QAction("&Connect..",this);
   connectAction->setStatusTip("Poll connection to Tello..");
   connect(connectAction,SIGNAL(triggered()),this,SLOT(slotConnect()));

   // Add actions to menus
   sysMenu->addAction(aboutAction); sysMenu->addSeparator();
   sysMenu->addAction(quitAction);
   netMenu->addAction(changeWIFIAction); netMenu->addSeparator();
   netMenu->addAction(connectAction);

//   upButton=new QPushButton("Up",this);
//   upButton->setGeometry(200,100,40,40); upButton->show();
//   connect(upButton,SIGNAL(clicked()),this,SLOT(slotUp()));

   setWindowTitle(
    "MarTELLO v0.9.0 - (c) GPL 2021 Barkin Ilhan - barkin@unrlabs.org");
  }

 protected:
  void keyPressEvent(QKeyEvent *e) {
   if (!e->isAutoRepeat()) {
         if (e->key() == Qt::Key_W) { qDebug() << "W"; tello.cmdForward=true; }
    else if (e->key() == Qt::Key_S) { qDebug() << "S"; tello.cmdBackward=true; }
    else if (e->key() == Qt::Key_A) { qDebug() << "A"; tello.cmdLeft=true; }
    else if (e->key() == Qt::Key_D) { qDebug() << "D"; tello.cmdRight=true; }

    else if (e->key() == Qt::Key_I) { qDebug() << "I"; tello.cmdUp=true; }
    else if (e->key() == Qt::Key_K) { qDebug() << "K"; tello.cmdDown=true; }
    else if (e->key() == Qt::Key_J) { qDebug() << "J"; tello.cmdCCW=true; }
    else if (e->key() == Qt::Key_L) { qDebug() << "L"; tello.cmdCW=true; }

    else if (e->key() == Qt::Key_1) { qDebug() << "1"; tello.cmdTakeOff=true; }
    else if (e->key() == Qt::Key_2) { qDebug() << "2"; tello.cmdLand=true; }
    else if (e->key() == Qt::Key_0) { qDebug() << "0"; tello.cmdEmergency=true; }
    e->ignore();
   }
  }

  void keyReleaseEvent(QKeyEvent *e) {
//   if (!e->isAutoRepeat()) {
         if (e->key() == Qt::Key_W) { qDebug() << "W"; tello.cmdForward=false; }
    else if (e->key() == Qt::Key_S) { qDebug() << "S"; tello.cmdBackward=false; }
    else if (e->key() == Qt::Key_A) { qDebug() << "A"; tello.cmdLeft=false; }
    else if (e->key() == Qt::Key_D) { qDebug() << "D"; tello.cmdRight=false; }

    else if (e->key() == Qt::Key_I) { qDebug() << "I"; tello.cmdUp=false; }
    else if (e->key() == Qt::Key_K) { qDebug() << "K"; tello.cmdDown=false; }
    else if (e->key() == Qt::Key_J) { qDebug() << "J"; tello.cmdCCW=false; }
    else if (e->key() == Qt::Key_L) { qDebug() << "L"; tello.cmdCW=false; }

    else if (e->key() == Qt::Key_1) { qDebug() << "1"; tello.cmdTakeOff=false; }
    else if (e->key() == Qt::Key_2) { qDebug() << "2"; tello.cmdLand=false; }
    else if (e->key() == Qt::Key_0) { qDebug() << "0"; tello.cmdEmergency=false; }
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
   //cmd.=false;
   application->exit(0);
  }

  void slotChangeWIFI() {
	  ;
  }

  void slotConnect() {
   if (tello.pollConnection) {
    connectAction->setText("&Connect.."); tello.pollConnection=false;
   } else {
    connectAction->setText("&Disconnect.."); tello.pollConnection=true;
   }
  }

  void slotStartComm() { tello.startComm=true; }
  void slotStopComm() { ; }

 private:
  QApplication *application;

  // Threads
  QThread *telloCmdThread,*telloSttThread,*telloVidThread;
  QMutex telloCmdMutex,telloSttMutex,telloVidMutex; TelloStruct tello;
  bool telloCmdTActive,telloSttTActive,telloVidTActive;

  // GUI
  int guiX,guiY,guiWidth,guiHeight;

  QMenuBar *menuBar;
  QAction *aboutAction,*quitAction, \
	  *changeWIFIAction,*connectAction;

//  QPushButton *upButton,*downButton,*leftButton,*rightButton;

  QStatusBar *guiStatusBar;
};

#endif
