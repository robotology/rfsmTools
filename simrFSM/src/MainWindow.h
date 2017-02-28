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

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initScene();
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

private:
    rfsm::StateMachine rfsm;
    Ui::MainWindow *ui;
    QGVScene *scene;
    QStringList messages;
    QStringListModel stringModel;
    std::string layoutStyle;
    bool layoutSubgraph;
    QTreeWidgetItem *moduleParentItem;
    QTreeWidgetItem *portParentItem;
};

#endif // MAINWINDOW_H
