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
#include <rfsm.h>
#include <map>

/*
class NodeWidgetItem : public QTreeWidgetItem {
public:
    NodeWidgetItem(QTreeWidgetItem* parent, yarp::graph::Vertex* vertex, int type)
        : QTreeWidgetItem(parent, QStringList(vertex->property.find("name").asString().c_str()), type)
    {
        if(dynamic_cast<ProcessVertex*>(vertex)) {
            std::stringstream lable;
            lable << vertex->property.find("name").asString().c_str()
                  << " (" << vertex->property.find("pid").asInt() << ")";
            setText(0, lable.str().c_str());
        }
        checkFlag = false;
        NodeWidgetItem::vertex = vertex;
    }

    void check(bool flag) {
        checkFlag = flag;
        setCheckState( 0, (flag == true) ? Qt::Checked : Qt::Unchecked);
        if(!checkFlag)
            vertex->property.put("hidden", true);
        else
            vertex->property.unput("hidden");
    }

    bool checked() { return checkFlag; }
    yarp::graph::Vertex* getVertex() { return vertex; }

public:
    bool checkFlag;
    yarp::graph::Vertex* vertex;
};
*/

namespace Ui {
class MainWindow;
}


class MainWindow;

class MyStateMachine : public rfsm::StateMachine {
public:
    MyStateMachine(MainWindow* mainWnd);
    virtual ~MyStateMachine();

public:
    virtual void onPreStep();
    virtual void onPostStep();
private:
    MainWindow* mainWindow;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QGVNode* getNode(const std::string& name);
    QGVSubGraph* getSubGraph(const std::string& name);

private:
    void initScene();
    void updateEventQueue();
    void drawStateMachine();
    QGVSubGraph * getParent(const std::string& name );

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
    void onRunrFSM();
    void onSteprFSM();
    void onSendEvent();

public:    
    Ui::MainWindow *ui;
    std::map<std::string, QGVNode*> sceneNodeMap;
    std::map<std::string, QGVSubGraph*> sceneSubGraphMap;
    QGVScene *scene;

private:
    MyStateMachine rfsm;
    std::string layoutStyle;
    bool layoutSubgraph;
    QTreeWidgetItem *moduleParentItem;
    QTreeWidgetItem *portParentItem;
};

#endif // MAINWINDOW_H
