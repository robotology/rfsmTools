/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

//#include <valgrind/callgrind.h>

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
#include <QtPrintSupport/QPrinter>

/*
#include <yarp/os/Random.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
*/


using namespace std;
//using namespace yarp::os;

class DefaultCallback : public rfsm::StateCallback {
public:
    virtual void entry() {}

    virtual void doo() {}

    virtual void exit() {}
} defaultCallback;



MyStateMachine::MyStateMachine(MainWindow* mainWnd)
    : rfsm::StateMachine(true) {
    mainWindow = mainWnd;
}

MyStateMachine::~MyStateMachine() { }


void MyStateMachine::onPreStep() {
    string msg = "onPreStep(): current state:" + getCurrentState();
    QTime qt = QTime::currentTime();
    QTreeWidgetItem* item;
    QStringList message;
    message.clear();
    message.append(qt.toString());
    message.append(msg.c_str());
    item = new QTreeWidgetItem(mainWindow->ui->nodesTreeWidgetLog, message);
    if(getCurrentState() != "<none>") {
        QGVNode* node = mainWindow->getNode(getCurrentState());
        if(node == NULL)
            node = mainWindow->getNode(getCurrentState()+".initial");
        Q_ASSERT(node != NULL);
        node->setActive(false);
        node->update();
    }
}


void MyStateMachine::onPostStep() {
    string msg = "onPostStep(): current state:" + getCurrentState();
    QTime qt = QTime::currentTime();
    QTreeWidgetItem* item;
    QStringList message;
    message.clear();
    message.append(qt.toString());
    message.append(msg.c_str());
    item = new QTreeWidgetItem( mainWindow->ui->nodesTreeWidgetLog, message);
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
    ui(new Ui::MainWindow), scene(NULL), rfsm(this)
{
    ui->setupUi(this);
//    stringModel.setStringList(messages);
//    ui->messageView->setModel(&stringModel);
//    ui->messageView->setVisible(false);

    // initialize the scene
    initScene();

    connect(ui->action_LoadrFSM, SIGNAL(triggered()),this,SLOT(onLoadrFSM()));
    connect(ui->actionRun, SIGNAL(triggered()),this,SLOT(onRunrFSM()));
    connect(ui->actionStep, SIGNAL(triggered()),this,SLOT(onSteprFSM()));
    connect(ui->pushButtonSendEvent, SIGNAL(clicked()),this, SLOT(onSendEvent()));

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

//    moduleParentItem = new QTreeWidgetItem( ui->nodesTreeWidget,  QStringList("Modules"));
//    portParentItem = new QTreeWidgetItem( ui->nodesTreeWidget,  QStringList("Ports"));
//    moduleParentItem->setIcon(0, QIcon(":icons/resources/module.svg"));
//    portParentItem->setIcon(0, QIcon(":icons/resources/port.svg"));

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

void MainWindow::onLoadrFSM() {
    QString filters("rFSM LUA files (*.lua);;All files (*.*)");
    QString defaultFilter("rFSM state machine (*.lua)");
    QString filename = QFileDialog::getOpenFileName(0, "Load rFSM state machine",
                                                    QDir::homePath(),
                                                    filters, &defaultFilter);
    if(filename.size() == 0)
        return;

    QDir path = QFileInfo(filename).absoluteDir();
    cout<<"current dir"<<path.absolutePath().toStdString();
    rfsm.addLuaPackagePath((path.absolutePath()+"/?.lua").toStdString());
    QDir::setCurrent(path.absolutePath());

    if(!rfsm.load(filename.toStdString())) {
        QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr(string("Cannot load " + filename.toStdString()).c_str()));
        return;
    }

    if(ui->actionDryrun->isChecked()) {
        //rfsm.setStateCallback("Configure", defaultCallback);
        const rfsm::StateGraph& graph = rfsm.getStateGraph();
        for(int i=0; i<graph.states.size(); i++) {
            //std::cout<<graph.states[i].name<<", "<<graph.states[i].type<<std::endl;
            if(graph.states[i].type != "connector")
                rfsm.setStateCallback(graph.states[i].name, defaultCallback);
        }
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
    ui->comboBoxEvents->addItems(ql);
    updateEventQueue();
    drawStateMachine();
}

void MainWindow::onRunrFSM() {
    rfsm.run();
    updateEventQueue();
}

void MainWindow::onSteprFSM() {
    rfsm.step();
    updateEventQueue();
}

void MainWindow::onSendEvent() {
    rfsm.sendEvent(ui->comboBoxEvents->currentText().toStdString());
    updateEventQueue();
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
