/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGVScene.h>
#include <QStringList>
#include <QStringListModel>
#include <QTreeWidgetItem>
#include <QThread>
#include <QTimer>
#include <rfsm.h>
#include <map>


namespace Ui {
class MainWindow;
}


class MainWindow;

class MyStateMachine : public QObject, public rfsm::StateMachine {
      Q_OBJECT
public:
    MyStateMachine(MainWindow* mainWnd);
    virtual ~MyStateMachine();

public slots:
    void execute();

public:
    void start();
    void stop();
    virtual void onPreStep();
    virtual void onPostStep();

public:
    int runPeriod;

private:
    MainWindow* mainWindow;
    QTimer *timer;
    std::string stateName;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    enum MachineMode {
        UNLOADED,
        IDLE,
        RUN,
        PAUSE,
        DEBUG
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QGVNode* getNode(const std::string& name);
    QGVSubGraph* getSubGraph(const std::string& name);
    void updateEventQueue();

private:
    void initScene();    
    void drawStateMachine();
    QGVSubGraph * getParent(const std::string& name );
    void switchMachineMode(MachineMode mode);
    bool loadrFSM(const std::string filename);

//    void onNodeContextMenuProccess(QGVNode *node, YarpvizVertex* vertex);
//    void onNodeContextMenuPort(QGVNode *node, YarpvizVertex* vertex);
//    void updateNodeWidgetItems();

private slots:
    void nodeContextMenu(QGVNode* node);
    void edgeContextMenu(QGVEdge* edge);
    void nodeDoubleClick(QGVNode* node);
    void onLayoutOrthogonal();
    void onLayoutPolyline();
    void onLayoutLine();
    void onLayoutCurved();
    void onExportScene();
    void onLoadrFSM();
    void onDebugStartrFSM();
    void onDebugSteprFSM();
    void onDebugResetrFSM();
    void onRunStartrFSM();
    void onRunStoprFSM();
    void onRunPauserFSM();
    void onSendEvent();
    void onChangeRunPeriod();
    void onQuit();

public:    
    Ui::MainWindow *ui;
    std::map<std::string, QGVNode*> sceneNodeMap;
    std::map<std::string, QGVSubGraph*> sceneSubGraphMap;
    QGVScene *scene;

private:
    MyStateMachine rfsm;
    MachineMode machineMode;    
    std::string layoutStyle;
    bool layoutSubgraph;
    QTreeWidgetItem *moduleParentItem;
    QTreeWidgetItem *portParentItem;
};

#endif // MAINWINDOW_H
