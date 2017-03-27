/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <fstream>
#include <iostream>
#include <algorithm>

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
#include <QMutexLocker>

using namespace std;

class DefaultCallback : public rfsm::StateCallback {
public:
    virtual void entry() {}

    virtual void doo() {}

    virtual void exit() {}
} defaultCallback;



MyStateMachine::MyStateMachine(MainWindow* mainWnd)
    : rfsm::StateMachine(true), stopped(false) {
    mainWindow = mainWnd;
    runPeriod = 500;

    qRegisterMetaType<std::vector<std::string> >("std::vector<std::string>");
    qRegisterMetaType<std::string>("std::string");

    thread = new QThread();    
    this->moveToThread(thread);

    connect(thread, SIGNAL(finished()), this, SLOT(onThreadFinished()));
    connect(thread, SIGNAL(started()), this, SLOT(onThreadStarted()));
}

MyStateMachine::~MyStateMachine() {
    if(thread)
        delete thread;
}


void MyStateMachine::execute() {
    if(stopped) {
        thread->quit();
        return;
    }
    mutex.lock();    
    std::vector<std::string> equeue;
    getEventQueue(equeue);
    emit updateEventQueue(equeue);
    run();
    mutex.unlock();
}

void MyStateMachine::onThreadFinished() {
    emit fsmStopped();
    timer->stop();
    if(timer)
        delete timer;
}

void MyStateMachine::onThreadStarted(){
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(execute()));
    timer->start(runPeriod);
}

void MyStateMachine::start() {
    stopped = false;
    mutex.lock();
    thread->start();
    mutex.unlock();
}

void MyStateMachine::stop() {
    stopped = true;
    return;
}

void MyStateMachine::close() {
    stateName.clear();
    StateMachine::close();
}


void MyStateMachine::onPostStep() {
    emit postStep(stateName, getCurrentState());
    stateName = getCurrentState();
}


void MyStateMachine::onWarning(const string message) {
    emit warning(message);
}

void MyStateMachine::onError(const string message) {
    emit error(message, getCurrentState());
}

void MyStateMachine::onInfo(const string message) {
    emit info(message);
}



/************************************************/
/* MainWindow                                   */
/************************************************/
MainWindow::MainWindow(QCommandLineParser *prsr, QWidget *parent) :
    parser(prsr), QMainWindow(parent),
    ui(new Ui::MainWindow), scene(NULL), rfsm(this), machineMode(UNLOADED)
{
    ui->setupUi(this);

    // initialize the scene
    initScene();

    watcher = new QFileSystemWatcher(this);


    connect(watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(onFileChanged(const QString &)));
    connect(ui->actionQuit, SIGNAL(triggered()),this,SLOT(onQuit()));
    connect(ui->actionAbout, SIGNAL(triggered()),this,SLOT(onAbout()));
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
    connect(ui->actionSourceCode, SIGNAL(triggered()),this,SLOT(onSourceCode()));

    // rfsm signals
    connect(&rfsm, SIGNAL(updateEventQueue(const std::vector<std::string>)),this, SLOT(onUpdateEventQueue(const std::vector<std::string>)));
    connect(&rfsm, SIGNAL(error(const std::string, const std::string)),this, SLOT(onError(const std::string, const std::string)));
    connect(&rfsm, SIGNAL(warning(const std::string)),this, SLOT(onWarning(std::string)));
    connect(&rfsm, SIGNAL(info(const std::string)),this, SLOT(onInfo(std::string)));
    connect(&rfsm, SIGNAL(postStep(const std::string, const std::string)),this, SLOT(onPostStep(const std::string, const std::string)));
    connect(&rfsm, SIGNAL(fsmStopped()),this, SLOT(onFsmStopped()));

    sourceWindow = new SourceEditorWindow(this);
    sourceWindow->setWindowModality(Qt::ApplicationModal);
    connect(sourceWindow, SIGNAL(sourceCodeSaved()), SLOT(onSourceCodeSaved()));

    layoutStyle = "spline";
    ui->actionCurved->setChecked(true);
    ui->action_Save_project->setEnabled(false);
    ui->action_LoadrFSM->setEnabled(true);
    ui->actionDocumentaion->setEnabled(false);
    ui->actionDryrun->setChecked(true);
    ui->actionExport_scene->setEnabled(false);
    ui->nodesTreeWidgetLog->sortByColumn(0, Qt::AscendingOrder); // column/order to sort by
    ui->nodesTreeWidgetLog->setSortingEnabled(true);             // should cause sort on add
//    ui->nodesTreeWidgetLog->setAutoScroll(true);
    ui->nodesTreeWidgetEvent->sortByColumn(0, Qt::AscendingOrder); // column/order to sort by
    ui->nodesTreeWidgetEvent->setSortingEnabled(true);             // should cause sort on add
//    ui->nodesTreeWidgetEvent->setAutoScroll(true);

    switchMachineMode(UNLOADED);
    if(parser->value("period").size())
        rfsm.runPeriod = parser->value("period").toInt();

    // loading and running state machine (if set via command line param)
    // is done in MainWindow::showEvent()

    rfsm.runPeriod = settings.value("run-period", 500).toInt();
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


std::string MainWindow::getPureStateName(const std::string& name) {
    QStringList pieces = QString(name.c_str()).split(".");
    return pieces[pieces.size()-1].toStdString();
}

QGVSubGraph * MainWindow::getParent(const std::string& name) {
    QStringList pieces = QString(name.c_str()).split(".");
    std::string lastNodeName = pieces[pieces.size()-1].toStdString();
    if(pieces.size() <2) // no parent
        return NULL;
    std::string parentName = name.substr(0, name.find(lastNodeName)-1);
    //std::cout<<"getParent of "<<lastNodeName <<" out of "<<name << " is "<<parentName<<std::endl;
    return sceneSubGraphMap[parentName];
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

    scene->setGraphAttribute("splines", layoutStyle.c_str()); //spline, polyline, line. ortho
    scene->setGraphAttribute("remincross", "true");
    scene->setGraphAttribute("rankdir", "TD");
    scene->setGraphAttribute("bgcolor", "#2e3e56");
    //scene->setGraphAttribute("concentrate", "true"); //Error !
    scene->setGraphAttribute("nodesep", "0.7");
    scene->setGraphAttribute("ranksep", "0.4");
    //scene->setGraphAttribute("sep", "0.4");
    //scene->setNodeAttribute("shape", "box");
    scene->setNodeAttribute("style", "filled");
    scene->setNodeAttribute("fillcolor", "gray");
    scene->setNodeAttribute("height", "1.0");
    scene->setEdgeAttribute("minlen", "2.0");
    //scene->setEdgeAttribute("dir", "both");

    // adding composit states
    const rfsm::StateGraph& graph = rfsm.getStateGraph();
    for(size_t i=0; i<graph.states.size(); i++) {
        if(graph.states[i].type == "composit") {
            //std::cout<<graph.states[i].name<<std::endl;
            QGVSubGraph *sgraph;
            QGVSubGraph *sgraphParent = getParent(graph.states[i].name);
            if(sgraphParent != NULL)
                sgraph = sgraphParent->addSubGraph(graph.states[i].name.c_str());
            else
                sgraph = scene->addSubGraph(graph.states[i].name.c_str());
            sgraph->setAttribute("shape", "box");
            sgraph->setAttribute("label", getPureStateName(graph.states[i].name).c_str());
            sgraph->setAttribute("fillcolor", "#2e3e56");
            sgraph->setAttribute("style", "filled");
            sgraph->setAttribute("color", "#edad56");
            sceneSubGraphMap[graph.states[i].name] = sgraph;
            // adding end node
            std::string endNodeName = graph.states[i].name + ".end";
            QGVNode * node = sgraph->addNode(endNodeName.c_str());
            node->setAttribute("shape", "circle");
            node->setAttribute("height", "0.1");
            node->setAttribute("fixedsize", "true");
            node->setAttribute("label", "");
            node->setAttribute("fillcolor", "#edad56");
            node->setAttribute("color", "#edad56");
            node->setAttribute("node_type", "end");
            sceneNodeMap[endNodeName] = node;
        }
    }


    // adding single states
    for(size_t i=0; i<graph.states.size(); i++) {
        QGVNode *node;
        if(graph.states[i].type != "composit") {
            QGVSubGraph* sgraph = getParent(graph.states[i].name);
            if(sgraph != NULL)
                node = sgraph->addNode(graph.states[i].name.c_str());
            else
                node = scene->addNode(graph.states[i].name.c_str());

            if(graph.states[i].type == "connector") {
                node->setAttribute("shape", "circle");
                node->setAttribute("height", "0.1");
                node->setAttribute("fixedsize", "true");
                node->setAttribute("label", "");
                node->setAttribute("fillcolor", "#edad56");
            }
            else {
                node->setAttribute("shape", "box");                
                node->setAttribute("label", getPureStateName(graph.states[i].name).c_str());
                node->setAttribute("fillcolor", "#2e3e56");
                node->setAttribute("entry", graph.states[i].entry.fileName.c_str());
                node->setAttribute("doo", graph.states[i].doo.fileName.c_str());
                node->setAttribute("exit", graph.states[i].exit.fileName.c_str());                
            }
            // use this for error : #FA8072
            node->setAttribute("color", "#edad56");
            node->setAttribute("labelfontcolor", "#edad56");
            sceneNodeMap[graph.states[i].name] = node;
        }
    }

    // adding transitions
    for(size_t i=0; i<graph.transitions.size(); i++) {
        std::string events;
        for(size_t e=0; e<graph.transitions[i].events.size();e++) {
            std::string ev = graph.transitions[i].events[e];
            if (ev.find("e_done@") != string::npos)
                ev = "e_done";
            events = events +  ((events.size()) ?  ", " + ev : ev);
        }

        //std::cout<<"\t"<<graph.transitions[i].source<<" -> "<<graph.transitions[i].target<<"("<<events<<")"<<std::endl;
        QGVNode* from = getNode(graph.transitions[i].source);
        if(from == NULL) {
            from = getNode(graph.transitions[i].source+".end");
        }
        QGVNode* to = getNode(graph.transitions[i].target);
        Q_ASSERT(from != NULL);
        Q_ASSERT(to != NULL);
        QGVEdge* gve = scene->addEdge(from, to, events.c_str());
        gve->setAttribute("color", "white");
        //gve->setAttribute("ltail", graph.transitions[i].source.c_str());
        //gve->setAttribute("lhead", graph.transitions[i].target.c_str());
        //gve->setAttribute("style", "dashed");
    }    

    //node->setIcon(QImage(":/icons/resources/Gnome-System-Run-64.png"));
    //Layout scene
    scene->applyLayout();
    //Fit in view
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

bool MainWindow::loadrFSM(const std::string filename) {

    ui->nodesTreeWidgetLog->clear();

    // loading the source code
    QString source;
    if(!loadrFSMSourceCode(filename, source))
        return false;

    sourceWindow->setSourceCode(source);

    // setting lua extra paths
    QDir path = QFileInfo(filename.c_str()).absoluteDir();
    rfsm.addLuaPackagePath((path.absolutePath()+"/?.lua").toStdString());
    QDir::setCurrent(path.absolutePath());

    // loading the state machine using rfsm
    if(!rfsm.load(filename)) {
        QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr(string("Cannot load " + filename).c_str()));        
        sourceWindow->show();
        return false;
    }

    // enablng hooks
    rfsm.enablePostStepHook();
    rfsm.catchPrintOutput();

    // adding events to gui event list
    QStringList ql;
    const std::vector<std::string>& events = rfsm.getEventsList();
    for (size_t i=0; i<events.size(); i++) {
        if(events[i] != "e_init_fsm")
            ql.push_back(events[i].c_str());
    }
    ql.sort();
    ui->comboBoxEvents->clear();
    ui->comboBoxEvents->addItems(ql);
    std::vector<std::string> equeue;
    rfsm.getEventQueue(equeue);
    onUpdateEventQueue(equeue);

    // drawing state machine
    drawStateMachine();
    switchMachineMode(IDLE);    
    showStatusBarMessage(("Loaded " + filename).c_str());
    watcher->addPath(filename.c_str());
    return true;
}

void MainWindow::onLoadrFSM() {
    QString filters("rFSM state machine (*.lua);;All files (*.*)");
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
        const rfsm::StateGraph& graph = rfsm.getStateGraph();
        for(size_t i=0; i<graph.states.size(); i++) {
            if(graph.states[i].type != "connector")
                rfsm.setStateCallback(graph.states[i].name, defaultCallback);
        }
    }
    switchMachineMode(DEBUG);
    rfsm.run();
    std::vector<std::string> equeue;
    rfsm.getEventQueue(equeue);
    onUpdateEventQueue(equeue);
    showStatusBarMessage(("Debugging " +  std::string(((ui->actionDryrun->isChecked()) ? "(Dry Run) ..." : "..."))).c_str());
}

void MainWindow::onDebugSteprFSM() {
    if(machineMode != DEBUG && ui->actionDryrun->isChecked()) {
        //rfsm.setStateCallback("Configure", defaultCallback);
        const rfsm::StateGraph& graph = rfsm.getStateGraph();
        for(size_t i=0; i<graph.states.size(); i++) {
            //std::cout<<graph.states[i].name<<", "<<graph.states[i].type<<std::endl;
            if(graph.states[i].type != "connector")
                rfsm.setStateCallback(graph.states[i].name, defaultCallback);
        }
    }
    switchMachineMode(DEBUG);
    rfsm.step();
    std::vector<std::string> equeue;
    rfsm.getEventQueue(equeue);
    onUpdateEventQueue(equeue);
    showStatusBarMessage(("Debugging " +  std::string(((ui->actionDryrun->isChecked()) ? "(Dry Run) ..." : "..."))).c_str());
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
    if(machineMode == PAUSE) {
        rfsm.start();
        switchMachineMode(RUN);     
    }
    else {
        //rfsm.stop();
        initScene();
        std::string filename = rfsm.getFileName();
        rfsm.close();
        if( loadrFSM(filename) ) {            
            rfsm.start();
            switchMachineMode(RUN);
        }
    }
    showStatusBarMessage("Running...", Qt::darkGreen);
}

void MainWindow::onRunStoprFSM() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Stop", "Stopping the state machine.\n Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;
	if(machineMode == PAUSE)
		switchMachineMode(IDLE);
	else {
		switchMachineMode(ASK_STOP);
		rfsm.stop();
	}
}

void MainWindow::onFsmStopped() {
    if(machineMode == ASK_STOP)
        switchMachineMode(IDLE);
	else if(machineMode == ASK_PAUSE)
		switchMachineMode(PAUSE);
}

void MainWindow::onRunPauserFSM() {    
	switchMachineMode(ASK_PAUSE);
    rfsm.stop();    
    showStatusBarMessage("Paused!", Qt::darkYellow);
}


void MainWindow::onSendEvent() {
    rfsm.sendEvent(ui->comboBoxEvents->currentText().toStdString());
    std::vector<std::string> equeue;
    rfsm.getEventQueue(equeue);
    onUpdateEventQueue(equeue);
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

void MainWindow::onUpdateEventQueue(const std::vector<string> equeue) {
    ui->nodesTreeWidgetEvent->clear();    
    QTime qt = QTime::currentTime();
    for(size_t i=0; i<equeue.size(); i++) {
        QTreeWidgetItem* item;
        QStringList event;
        event.clear();
        event.append(qt.toString());
        event.append(equeue[i].c_str());
        item = new QTreeWidgetItem( ui->nodesTreeWidgetEvent, event);
        ui->nodesTreeWidgetEvent->scrollToBottom();
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
    layoutStyle = "spline";
    drawStateMachine();
}

void MainWindow::onQuit() {
    MainWindow::close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(machineMode != IDLE && machineMode != UNLOADED) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Quit", "State machine is running.\n Do you want to stop it and exit?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            event->ignore();
            return;
        }
    }
    // save setting
    saveSetting();
    // close state machine
    rfsm.stop();
    rfsm.close();
    QMainWindow::closeEvent(event);
}


void MainWindow::onAbout() {
    QMessageBox::about(this, "rFSM Gui (version 1.0.0)",
                       "A graphical tool for running and debuging rFSM state machines\n\nAuthors:\n\t-Ali Paikan <ali.paikan@iit.it>\n\t-Nicolò Genesio <nicolo.genesio@iit.it>");
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

void MainWindow::onSourceCode() {

    QString source;
    if(!loadrFSMSourceCode(rfsm.getFileName(), source))
        return;
    sourceWindow->setSourceCode(source);
    sourceWindow->setReadOnly(false);
    sourceWindow->show();    
}

void MainWindow::onSourceCodeSaved() {
    std::string filename = rfsm.getFileName();
    QFile file(filename.c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr(string("Cannot open " + filename).c_str()));
        return;
    }
    watcher->removePath(filename.c_str());
    if(file.write(sourceWindow->getSourceCode().toUtf8()) == -1)
        QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr(string("Cannot save to " + filename).c_str()));
    file.close();
    initScene();
    rfsm.close();
    loadrFSM(filename);   
}

void MainWindow::showEvent(QShowEvent *ev) {
    QMainWindow::showEvent(ev);
    if(parser->value("rfsm").size()) {
       bool ok = loadrFSM(parser->value("rfsm").toStdString());
       if (ok && parser->isSet("run")) {
           QTimer::singleShot(10, this, SLOT(onRunStartrFSM()));
       }
    }
}

bool MainWindow::loadrFSMSourceCode(const std::string filename, QString& source) {
    QFile file(filename.c_str());
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr(string("Cannot open " + rfsm.getFileName()).c_str()));
        return false;
    }
    source = file.readAll();
    file.close();
    return true;
}

void MainWindow::onFileChanged(const QString &path){

    watcher->addPath(path);
    if(machineMode==IDLE)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "File changed", "Lua file changed.\nDo you want to reload the state machine?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
        rfsm.close();
        if(sourceWindow->isActiveWindow()) {
            QString source;
            if(loadrFSMSourceCode(path.toStdString(), source))
                sourceWindow->setSourceCode(source);
        }
        else
            loadrFSM(path.toStdString());
    }

}

void MainWindow::onWarning(const string message) {
    QTime qt = QTime::currentTime();
    QTreeWidgetItem* item;
    QStringList qmessage;
    qmessage.append(qt.toString());
    qmessage.append(message.c_str());
    item = new QTreeWidgetItem(ui->nodesTreeWidgetLog, qmessage);
    ui->nodesTreeWidgetLog->scrollToBottom();
    QBrush b( QColor("#FFFACD") );
    item->setBackground( 0, b);
    item->setBackground( 1, b);
}


void MainWindow::onInfo(const string message) {
    QTime qt = QTime::currentTime();
    QTreeWidgetItem* item;
    QStringList qmessage;
    qmessage.append(qt.toString());
    qmessage.append(message.c_str());
    item = new QTreeWidgetItem(ui->nodesTreeWidgetLog, qmessage);
    if( message.find("INFO:") != string::npos){
        QBrush b( QColor("#bbff99") );
        item->setBackground( 0, b);
        item->setBackground( 1, b);
    }
    ui->nodesTreeWidgetLog->scrollToBottom();
}

void MainWindow::onError(const string message, const string currentState) {
    QTime qt = QTime::currentTime();
    QTreeWidgetItem* item;
    QStringList qmessage;
    qmessage.append(qt.toString());
    qmessage.append(message.c_str());
    item = new QTreeWidgetItem(ui->nodesTreeWidgetLog, qmessage);
    ui->nodesTreeWidgetLog->scrollToBottom();
    QBrush b( QColor("#FA8072") );
    item->setBackground( 0, b);
    item->setBackground( 1, b);
    // setting node/subgrph error mode
    QGVNode* node = getNode(currentState);
    if(node) {
        node->setError(message);
        node->update();
    }
    else {
        QGVSubGraph* sgv = getSubGraph(currentState);
        if(sgv) {
            sgv->setError(message);
            sgv->update();
        }
    }

    if(sourceWindow->isVisible())
        return;

    if(machineMode == MainWindow::RUN)
        switchMachineMode(MainWindow::PAUSE);
    rfsm.stop();
    showStatusBarMessage("Error occured! (paused)", Qt::darkRed);

    // TODO : Fix the copy graph!
    QString filename = rfsm.getFileName().c_str(); // check if this should be moved to signal param
    int line = 0;
    rfsm::StateGraph graph = rfsm.getStateGraph();
    rfsm::StateGraph::State st;
    st.name = currentState;
    std::vector<rfsm::StateGraph::State>::iterator itr;
    itr = std::find(graph.states.begin(), graph.states.end(), st);
    if(itr != graph.states.end()) {
        if(message.find("ENTRY") == 0)
            filename = (*itr).entry.fileName.c_str();
        else if(message.find("DOO") == 0)
            filename = (*itr).doo.fileName.c_str();
        else if(message.find("EXIT") == 0)
            filename = (*itr).exit.fileName.c_str();
    }

    if(rfsm.getFileName() != filename.toStdString()) {
        QString source;
        loadrFSMSourceCode(filename.toStdString(), source);
        sourceWindow->setSourceCode(source);
    }

    filename = QFileInfo(filename).fileName();
    QRegularExpression re(filename + ":(.*):");
    QRegularExpressionMatch match = re.match(message.c_str(), 1);
    if (match.hasMatch()) {
        QStringList strlist =  match.captured(0).split(":");
        if(strlist.size() > 1)
            line = strlist[1].toInt() - 1;
    }

    sourceWindow->setErrorMessage(message.c_str(), line);
    sourceWindow->setReadOnly((machineMode != MainWindow::IDLE)
                                          && (machineMode != MainWindow::UNLOADED));
    sourceWindow->show();
}

void MainWindow::onPostStep(const std::string prevState, const std::string currentState) {
    if(prevState != currentState) {
        string msg = "Transited from <"+ prevState + "> to <"+ currentState + ">";
        QTime qt = QTime::currentTime();
        QTreeWidgetItem* item;
        QStringList message;
        message.clear();
        message.append(qt.toString());
        message.append(msg.c_str());
        item = new QTreeWidgetItem(ui->nodesTreeWidgetLog, message);
        ui->nodesTreeWidgetLog->scrollToBottom();
        //////////////////////////////////////////////////////
        setNodeActiveMode(prevState, false);
        setNodeActiveMode(currentState, true);
    }
}

void MainWindow::setNodeActiveMode(const std::string &name, bool mode) {
    if(!name.size() || (name == "<none>"))
        return;

    QStringList pieces = QString(name.c_str()).split( "." );
    std::string subName;
    for(int i=0; i<pieces.size(); i++) {
        subName = subName + pieces[i].toStdString();
        QGVSubGraph* sgraph = getParent(subName);
        if(sgraph != NULL) {
            sgraph->setActive(mode);
            sgraph->update();
        }
        subName += ".";
    }
    QGVNode* node = getNode(name);
    if(node == NULL)
        node = getNode(name+".initial");
    Q_ASSERT(node != NULL);
    node->setActive(mode);
    node->update();
    if(mode)
        ui->graphicsView->centerOn(node);
}


void MainWindow::showStatusBarMessage(const QString& message,
                                      QColor color) {
    ui->statusBar->showMessage(message);
    QPalette palette;
    palette.setColor( QPalette::WindowText, color );
    statusBar()->setPalette(palette);
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
        ui->actionSourceCode->setEnabled(false);
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
        ui->actionSourceCode->setEnabled(true);
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
        ui->actionSourceCode->setEnabled(false);
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
        ui->actionSourceCode->setEnabled(false);
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
        ui->actionSourceCode->setEnabled(false);
        break;
    default:
        break;
    }
}

void MainWindow::saveSetting() {
    settings.setValue("run-period", rfsm.runPeriod);
}
