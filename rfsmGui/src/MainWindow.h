/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCommandLineParser>
#include <QGVScene.h>
#include <QStringList>
#include <QStringListModel>
#include <QTreeWidgetItem>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QFileSystemWatcher>
#include <rfsm.h>
#include <map>
#include "SourceEditorWindow.h"
#include <QSettings>

namespace Ui {
class MainWindow;
}


class MainWindow;

class MyStateMachine : public QObject, public rfsm::StateMachine {
      Q_OBJECT
public:
    explicit MyStateMachine(MainWindow* mainWnd);
    virtual ~MyStateMachine();

signals:
    void fsmStopped();
    void updateEventQueue(const std::vector<std::string> equeue);
    void warning(const std::string message);
    void info(const std::string message);
    void error(const std::string message, const std::string currentState);
    void postStep(const std::string prevState, const std::string currentState);

public slots:
    void execute();
    void onThreadFinished();
    void onThreadStarted();

public:
    void start();
    void stop();
    void close();    
    virtual void onPostStep();
    virtual void onWarning(const std::string message);
    virtual void onError(const std::string message);
    virtual void onInfo(const std::string message);        

public:
    int runPeriod;

private:
    MainWindow* mainWindow;
    QTimer *timer;
    QThread* thread;
    QMutex mutex;
    std::string stateName;
    bool stopped;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum MachineMode {
        UNLOADED,
        IDLE,
        RUN,
        PAUSE,
        ASK_PAUSE,
        ASK_STOP,
        DEBUG
    };

public:
    explicit MainWindow(QCommandLineParser* prsr, QWidget *parent = 0);
    ~MainWindow();
    QGVNode* getNode(const std::string& name);
    QGVSubGraph * getParent(const std::string& name );
    QGVSubGraph* getSubGraph(const std::string& name);    
    void switchMachineMode(MachineMode mode);
    void showStatusBarMessage(const QString& message,
                              QColor color = Qt::black);
    bool loadrFSMSourceCode(const std::string filename, QString& source);

protected:
    void showEvent(QShowEvent *ev);

private:
    void initScene();    
    void drawStateMachine();    
    bool loadrFSM(const std::string filename);    
    std::string getPureStateName(const std::string& name);
    void saveSetting();    
    void setNodeActiveMode(const std::string &name, bool mode);

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
    void onAbout();
    void onSourceCode();
    void onSourceCodeSaved();
    void onFileChanged(const QString & path);
    void closeEvent(QCloseEvent *event);

    void onUpdateEventQueue(const std::vector<std::string> equeue);
    void onWarning(const std::string message);
    void onInfo(const std::string message);
    void onError(const std::string message, const std::string currentState);
    void onPostStep(const std::string prevState, const std::string currentState);
    void onFsmStopped();

public:    
    Ui::MainWindow *ui;
    std::map<std::string, QGVNode*> sceneNodeMap;
    std::map<std::string, QGVSubGraph*> sceneSubGraphMap;
    QGVScene *scene;
    SourceEditorWindow* sourceWindow;
    MachineMode machineMode;

private:
    QCommandLineParser* parser;
    MyStateMachine rfsm;    
    std::string layoutStyle;    
    QTreeWidgetItem *moduleParentItem;
    QTreeWidgetItem *portParentItem;        
    QFileSystemWatcher* watcher;
    QSettings settings;
};

#endif // MAINWINDOW_H
