/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <fstream>
#include <iostream>

#include "MainWindow.h"
#include "moc_MainWindow.cpp"
#include "ui_MainWindow.h"
#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include "QGVSubGraph.h"
#include <QTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QtPrintSupport/QPrinter>


using namespace std;

class DefaultCallback : public rfsm::StateCallback {
public:
    virtual void entry() {}

    virtual void doo() {}

    virtual void exit() {}
} defaultCallback;



MyStateMachine::MyStateMachine(MainWindow* mainWnd)
    : rfsm::StateMachine(true) {
    mainWindow = mainWnd;
    runPeriod = 500;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(execute()));
}

MyStateMachine::~MyStateMachine() {
    delete timer;
}


void MyStateMachine::execute() {
    mainWindow->updateEventQueue();
    run();
}

void MyStateMachine::start() {
    timer->start(runPeriod);
}

void MyStateMachine::stop() {
    timer->stop();
}


void MyStateMachine::onPreStep() {
    stateName = getCurrentState();
    if(stateName != "<none>") {
        QGVNode* node = mainWindow->getNode(stateName);
        if(node == NULL)
            node = mainWindow->getNode(stateName+".initial");
        Q_ASSERT(node != NULL);
        node->setActive(false);
        node->update();
    }
}


void MyStateMachine::onPostStep() {
    if(stateName != getCurrentState()) {
        string msg = "Transited from <"+ stateName + "> to <"+ getCurrentState() + ">";
        QTime qt = QTime::currentTime();
        QTreeWidgetItem* item;
        QStringList message;
        message.clear();
        message.append(qt.toString());
        message.append(msg.c_str());
        item = new QTreeWidgetItem(mainWindow->ui->nodesTreeWidgetLog, message);
    }

    if(getCurrentState() != "<none>") {
        QGVNode* node = mainWindow->getNode(getCurrentState());
        if(node == NULL)
            node = mainWindow->getNode(getCurrentState()+".initial");
        Q_ASSERT(node != NULL);
        node->setActive(true);
        node->update();
    }
}



/************************************************/
/* MainWindow                                   */
/************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), scene(NULL), rfsm(this), machineMode(UNLOADED)
{
    ui->setupUi(this);

    // initialize the scene
    initScene();

    connect(ui->actionQuit, SIGNAL(triggered()),this,SLOT(onQuit()));
    connect(ui->action_LoadrFSM, SIGNAL(triggered()),this,SLOT(onLoadrFSM()));
    connect(ui->pushButtonSendEvent, SIGNAL(clicked()),this, SLOT(onSendEvent()));
    connect(ui->actionChangeRunPeriod, SIGNAL(triggered()),this,SLOT(onChangeRunPeriod()));

    connect(ui->actionDebugStart, SIGNAL(triggered()),this,SLOT(onDebugStartrFSM()));
    connect(ui->actionDebugStep, SIGNAL(triggered()),this,SLOT(onDebugSteprFSM()));
    connect(ui->actionDebugReset, SIGNAL(triggered()),this,SLOT(onDebugResetrFSM()));

    connect(ui->actionRunStart, SIGNAL(triggered()),this,SLOT(onRunStartrFSM()));
    connect(ui->actionRunStop, SIGNAL(triggered()),this,SLOT(onRunStoprFSM()));
    connect(ui->actionRunPause, SIGNAL(triggered()),this,SLOT(onRunPauserFSM()));

    connect(ui->actionOrthogonal, SIGNAL(triggered()),this,SLOT(onLayoutOrthogonal()));
    connect(ui->actionCurved, SIGNAL(triggered()),this,SLOT(onLayoutCurved()));
    connect(ui->actionPolyline, SIGNAL(triggered()),this,SLOT(onLayoutPolyline()));
    connect(ui->actionLine, SIGNAL(triggered()),this,SLOT(onLayoutLine()));
    connect(ui->actionExport_scene, SIGNAL(triggered()),this,SLOT(onExportScene()));

    layoutStyle = "polyline";
    ui->actionOrthogonal->setChecked(true);
    layoutSubgraph = true;
    ui->actionSubgraph->setChecked(true);

    ui->action_Save_project->setEnabled(false);
    ui->action_LoadrFSM->setEnabled(true);
    ui->actionDocumentaion->setEnabled(false);
    ui->actionDryrun->setChecked(true);
    ui->actionExport_scene->setEnabled(false);

    switchMachineMode(UNLOADED);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initScene() {
    if(scene) {
        scene->clear();
        delete scene;
    }
    scene = new QGVScene("rFSM", this);
    ui->graphicsView->setBackgroundBrush(QBrush(QColor("#2e3e56"), Qt::SolidPattern));
    ui->graphicsView->setScene(scene);
    connect(scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
    connect(scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
    connect(scene, SIGNAL(edgeContextMenu(QGVEdge*)), SLOT(edgeContextMenu(QGVEdge*)));
}


QGVSubGraph * MainWindow::getParent(const std::string& name) {
     std::map<std::string,QGVSubGraph*>::iterator it;
     for(it = sceneSubGraphMap.begin(); it!=sceneSubGraphMap.end(); it++ ) {
         std::string sgname = (*it).first;
         if(name.find(sgname+".") != std::string::npos)
             return sceneSubGraphMap[sgname];
     }
     return NULL;
}


QGVNode* MainWindow::getNode(const std::string& name) {
    std::map<std::string,QGVNode*>::iterator it;
    it = sceneNodeMap.find(name);
    if(it != sceneNodeMap.end())
        return (*it).second;
    return NULL;
}

QGVSubGraph* MainWindow::getSubGraph(const std::string& name) {
    std::map<std::string,QGVSubGraph*>::iterator it;
    it = sceneSubGraphMap.find(name);
    if(it != sceneSubGraphMap.end())
        return (*it).second;
    return NULL;
}


void MainWindow::drawStateMachine() {
    initScene();
    sceneNodeMap.clear();
    sceneSubGraphMap.clear();

    scene->setGraphAttribute("splines", layoutStyle.c_str()); //curved, polyline, line. ortho
    scene->setGraphAttribute("remincross", "true");
    scene->setGraphAttribute("rankdir", "TD");
    scene->setGraphAttribute("bgcolor", "#2e3e56");
    //scene->setGraphAttribute("concentrate", "true"); //Error !
    scene->setGraphAttribute("nodesep", "0.7");
    scene->setGraphAttribute("ranksep", "0.7");
    //scene->setNodeAttribute("shape", "box");
    scene->setNodeAttribute("style", "filled");
    scene->setNodeAttribute("fillcolor", "gray");
    scene->setNodeAttribute("height", "1.0");
    scene->setEdgeAttribute("minlen", "2.0");
    //scene->setEdgeAttribute("dir", "both");

    // adding composit states
    const rfsm::StateGraph& graph = rfsm.getStateGraph();
    for(int i=0; i<graph.states.size(); i++) {
        if(graph.states[i].type == "composit") {
            QGVSubGraph *sgraph = scene->addSubGraph(graph.states[i].name.c_str());
            sgraph->setAttribute("shape", "box");
            sgraph->setAttribute("label", graph.states[i].name.c_str());
            sgraph->setAttribute("fillcolor", "#edad56");
            sgraph->setAttribute("color", "#edad56");
            sceneSubGraphMap[graph.states[i].name] = sgraph;
        }
    }


    // adding single states
    //const rfsm::StateGraph& graph = rfsm.getStateGraph();
    for(int i=0; i<graph.states.size(); i++) {
        QGVNode *node;
        if(graph.states[i].type != "composit") {
            QGVSubGraph* sgraph =  getParent(graph.states[i].name);
            if(sgraph != NULL)
                node = sgraph->addNode(graph.states[i].name.c_str());
            else
                node = scene->addNode(graph.states[i].name.c_str());

            if(graph.states[i].type == "connector") {
                node->setAttribute("shape", "circle");
                node->setAttribute("height", "0.1");
                node->setAttribute("fixedsize", "true");
                node->setAttribute("label", "");
            }
            else {
                node->setAttribute("shape", "box");
                node->setAttribute("label", graph.states[i].name.c_str());
            }
            // use this for error : #FA8072
            node->setAttribute("fillcolor", "#edad56");
            node->setAttribute("color", "#edad56");
            sceneNodeMap[graph.states[i].name] = node;
        }
    }

    // adding transitions
    for(int i=0; i<graph.transitions.size(); i++) {
        std::string events;
        for(int e=0; e<graph.transitions[i].events.size();e++) {
            std::string ev = graph.transitions[i].events[e];
            if (ev.find("e_done@") != string::npos)
                ev = "e_done";
            events = events +  ((events.size()) ?  ", " + ev : ev);
        }

        //std::cout<<"\t"<<graph.transitions[i].source<<" -> "<<graph.transitions[i].target<<"("<<events<<")"<<std::endl;

        QGVNode* from = getNode(graph.transitions[i].source);
        if(from == NULL) {
            from = getNode(graph.transitions[i].source+".initial");
        }
        QGVNode* to = getNode(graph.transitions[i].target);
        Q_ASSERT(from != NULL);
        Q_ASSERT(to != NULL);
        QGVEdge* gve = scene->addEdge(from, to, events.c_str());
        gve->setAttribute("color", "white");
        //gve->setAttribute("style", "dashed");
    }    

    //node->setIcon(QImage(":/icons/resources/Gnome-System-Run-64.png"));

    //Layout scene
    scene->applyLayout();
    //Fit in view
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    //QGVSubGraph *ssgraph = sgraph->addSubGraph("SUB2");
    //ssgraph->setAttribute("label", "DESK");
    //scene->addEdge(snode1, ssgraph->addNode("PC0155"), "S10");
}

bool MainWindow::loadrFSM(const std::string filename) {
    QDir path = QFileInfo(filename.c_str()).absoluteDir();
    rfsm.addLuaPackagePath((path.absolutePath()+"/?.lua").toStdString());
    QDir::setCurrent(path.absolutePath());

    if(!rfsm.load(filename)) {
        QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr(string("Cannot load " + filename).c_str()));
        return false;
    }

    rfsm.enablePreStepHook();
    rfsm.enablePostStepHook();

    QStringList ql;
    const std::vector<std::string>& events = rfsm.getEventsList();
    for (int i=0; i<events.size(); i++) {
        if(events[i] != "e_init_fsm")
            ql.push_back(events[i].c_str());
    }
    ql.sort();
    ui->comboBoxEvents->clear();
    ui->comboBoxEvents->addItems(ql);
    updateEventQueue();
    drawStateMachine();
    switchMachineMode(IDLE);    
    return true;
}

void MainWindow::onLoadrFSM() {
    QString filters("rFSM LUA files (*.lua);;All files (*.*)");
    QString defaultFilter("rFSM state machine (*.lua)");
    QString filename = QFileDialog::getOpenFileName(0, "Load rFSM state machine",
                                                    QDir::homePath(),
                                                    filters, &defaultFilter);
    if(filename.size() == 0)
        return;
    loadrFSM(filename.toStdString());
}

void MainWindow::onDebugStartrFSM() {
    if(machineMode != DEBUG && ui->actionDryrun->isChecked()) {
        //rfsm.setStateCallback("Configure", defaultCallback);
        const rfsm::StateGraph& graph = rfsm.getStateGraph();
        for(int i=0; i<graph.states.size(); i++) {
            //std::cout<<graph.states[i].name<<", "<<graph.states[i].type<<std::endl;
            if(graph.states[i].type != "connector")
                rfsm.setStateCallback(graph.states[i].name, defaultCallback);
        }
    }
    switchMachineMode(DEBUG);
    rfsm.run();
    updateEventQueue();
}

void MainWindow::onDebugSteprFSM() {
    if(machineMode != DEBUG && ui->actionDryrun->isChecked()) {
        //rfsm.setStateCallback("Configure", defaultCallback);
        const rfsm::StateGraph& graph = rfsm.getStateGraph();
        for(int i=0; i<graph.states.size(); i++) {
            //std::cout<<graph.states[i].name<<", "<<graph.states[i].type<<std::endl;
            if(graph.states[i].type != "connector")
                rfsm.setStateCallback(graph.states[i].name, defaultCallback);
        }
    }
    switchMachineMode(DEBUG);
    rfsm.step();
    updateEventQueue();
}

void MainWindow::onDebugResetrFSM() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Reset", "Reseting the state machine.\n Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;
    switchMachineMode(IDLE);
    initScene();
    std::string filename = rfsm.getFileName();
    rfsm.close();
    loadrFSM(filename);
}

void MainWindow::onRunStartrFSM() {
    rfsm.stop();
    initScene();
    std::string filename = rfsm.getFileName();
    rfsm.close();
    if( loadrFSM(filename) ) {
        switchMachineMode(RUN);
        rfsm.start();
    }
}

void MainWindow::onRunStoprFSM() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Stop", "Stopping the state machine.\n Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    switchMachineMode(IDLE);
    rfsm.stop();
    initScene();
    std::string filename = rfsm.getFileName();
    rfsm.close();
    loadrFSM(filename);
}

void MainWindow::onRunPauserFSM() {
    switchMachineMode(PAUSE);
    rfsm.stop();
}


void MainWindow::onSendEvent() {
    rfsm.sendEvent(ui->comboBoxEvents->currentText().toStdString());
    updateEventQueue();
}


void MainWindow::onChangeRunPeriod() {
    bool ok;
    QInputDialog* inputDialog = new QInputDialog();
    inputDialog->setOptions(QInputDialog::NoButtons);

    int period =  inputDialog->getInt(NULL ,"Change RFSM run period",
                                          "Period (ms):", rfsm.runPeriod, 0, 2147483647, 100, &ok);

     if (ok && (period > 0)) {
        rfsm.runPeriod = period;
     }
}

void MainWindow::updateEventQueue() {
    std::vector<std::string> equeue;
    rfsm.getEventQueue(equeue);
    ui->nodesTreeWidgetEvent->clear();
    QTime qt = QTime::currentTime();
    for(int i=0; i<equeue.size(); i++) {
        QTreeWidgetItem* item;
        QStringList event;
        event.clear();
        event.append(qt.toString());
        event.append(equeue[i].c_str());
        item = new QTreeWidgetItem( ui->nodesTreeWidgetEvent, event);
    }
}

void MainWindow::edgeContextMenu(QGVEdge* edge) {

}

void MainWindow::nodeContextMenu(QGVNode *node)
{

}

void MainWindow::nodeDoubleClick(QGVNode *node)
{

}

void MainWindow::onLayoutOrthogonal() {
    ui->actionPolyline->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "ortho";
    drawStateMachine();
}

void MainWindow::onLayoutPolyline() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "polyline";
    drawStateMachine();
}

void MainWindow::onLayoutLine() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "line";
    drawStateMachine();
}

void MainWindow::onLayoutCurved() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionLine->setChecked(false);
    layoutStyle = "curved";
    drawStateMachine();
}

void MainWindow::onQuit() {
    if(machineMode != IDLE && machineMode != UNLOADED) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Quit", "State machine is running.\n Do you want to stop it and exit?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }
    rfsm.stop();
    rfsm.close();
    MainWindow::close();
}

void MainWindow::onExportScene() {
    QString filters("Image files (*.png);;All files (*.*)");
    QString defaultFilter("Image file (*.png)");
    QString filename = QFileDialog::getSaveFileName(0, "Export scene",
                                                    QDir::homePath(),
                                                    filters, &defaultFilter);
    if(filename.size() == 0)
        return;

    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
    image.fill(QColor("#2e3e56"));
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter);
    if(!image.save(filename))
        return;
}


void MainWindow::switchMachineMode(MachineMode mode) {
    machineMode = mode;
    switch (machineMode) {
    case UNLOADED:
        // debug
        ui->actionDebugStart->setEnabled(false);
        ui->actionDebugReset->setEnabled(false);
        ui->actionDebugStep->setEnabled(false);
        //run
        ui->actionRunStart->setEnabled(false);
        ui->actionRunStop->setEnabled(false);
        ui->actionRunPause->setEnabled(false);
        ui->actionChangeRunPeriod->setEnabled(true);
        break;
    case IDLE:
        ui->actionExport_scene->setEnabled(true);
        // debug
        ui->actionDebugStart->setEnabled(true);
        ui->actionDebugReset->setEnabled(false);
        ui->actionDebugStep->setEnabled(true);
        //run
        ui->actionRunStart->setEnabled(true);
        ui->actionRunStop->setEnabled(false);
        ui->actionRunPause->setEnabled(false);
        ui->actionChangeRunPeriod->setEnabled(true);
        break;
    case DEBUG:
        // debug
        ui->actionDebugStart->setEnabled(true);
        ui->actionDebugReset->setEnabled(true);
        ui->actionDebugStep->setEnabled(true);
        //run
        ui->actionRunStart->setEnabled(false);
        ui->actionRunStop->setEnabled(false);
        ui->actionRunPause->setEnabled(false);
        ui->actionChangeRunPeriod->setEnabled(false);
        break;
    case RUN:
        // debug
        ui->actionDebugStart->setEnabled(false);
        ui->actionDebugReset->setEnabled(false);
        ui->actionDebugStep->setEnabled(false);
        //run
        ui->actionRunStart->setEnabled(false);
        ui->actionRunStop->setEnabled(true);
        ui->actionRunPause->setEnabled(true);
        ui->actionChangeRunPeriod->setEnabled(false);
        break;
    case PAUSE:
        // debug
        ui->actionDebugStart->setEnabled(false);
        ui->actionDebugReset->setEnabled(false);
        ui->actionDebugStep->setEnabled(false);
        //run
        ui->actionRunStart->setEnabled(true);
        ui->actionRunStop->setEnabled(true);
        ui->actionRunPause->setEnabled(false);
        ui->actionChangeRunPeriod->setEnabled(false);
        break;
    default:
        break;
    }
}
