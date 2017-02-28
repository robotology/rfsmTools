/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

//#include <valgrind/callgrind.h>

#include <fstream>

#include "MainWindow.h"
#include "moc_MainWindow.cpp"
#include "ui_MainWindow.h"
#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include "QGVSubGraph.h"
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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), scene(NULL)
{
    ui->setupUi(this);
    stringModel.setStringList(messages);
//    ui->messageView->setModel(&stringModel);
//    ui->messageView->setVisible(false);

    // initialize the scene
    initScene();

    /*
    connect(ui->actionProfile_YARP_network, SIGNAL(triggered()),this,SLOT(onProfileYarpNetwork()));
    connect(ui->actionHighlight_Loops, SIGNAL(triggered()),this,SLOT(onHighlightLoops()));
    connect(ui->actionHideConnectionsLable, SIGNAL(triggered()),this,SLOT(onHideConnectionsLable()));
    connect(ui->actionOrthogonal, SIGNAL(triggered()),this,SLOT(onLayoutOrthogonal()));
    connect(ui->actionCurved, SIGNAL(triggered()),this,SLOT(onLayoutCurved()));
    connect(ui->actionPolyline, SIGNAL(triggered()),this,SLOT(onLayoutPolyline()));
    connect(ui->actionLine, SIGNAL(triggered()),this,SLOT(onLayoutLine()));
    connect(ui->actionSubgraph, SIGNAL(triggered()),this,SLOT(onLayoutSubgraph()));
    connect(ui->actionHidePorts, SIGNAL(triggered()),this,SLOT(onHidePorts()));
    connect(ui->nodesTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
            SLOT(onNodesTreeItemClicked(QTreeWidgetItem *, int)));
    connect(ui->actionMessageBox, SIGNAL(triggered()),this,SLOT(onWindowMessageBox()));
    connect(ui->actionItemswindow, SIGNAL(triggered()),this,SLOT(onWindowItem()));
    connect(ui->actionExport_scene, SIGNAL(triggered()),this,SLOT(onExportScene()));
    connect(ui->actionExport_connections_list, SIGNAL(triggered()),this,SLOT(onExportConList()));
    connect(ui->actionConfigure_connections_QOS, SIGNAL(triggered()),this,SLOT(onConfigureConsQos()));
    connect(ui->actionUpdateConnectionQosStatus, SIGNAL(triggered()),this,SLOT(onUpdateQosStatus()));
    connect(ui->actionProfilePortsRate, SIGNAL(triggered()),this,SLOT(onProfilePortsRate()));
    */

    layoutStyle = "ortho";
    ui->actionOrthogonal->setChecked(true);
    layoutSubgraph = true;
    ui->actionSubgraph->setChecked(true);


    ui->action_Save_project->setEnabled(false);
    ui->action_LoadrFSM->setEnabled(true);
    ui->actionDocumentaion->setEnabled(false);

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
    scene = new QGVScene("yarpviz", this);
    ui->graphicsView->setBackgroundBrush(QBrush(QColor("#2e3e56"), Qt::SolidPattern));
    ui->graphicsView->setScene(scene);
    connect(scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
    connect(scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
    connect(scene, SIGNAL(edgeContextMenu(QGVEdge*)), SLOT(edgeContextMenu(QGVEdge*)));
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
//    if(currentGraph)
//        drawGraph(*currentGraph);
}

void MainWindow::onLayoutPolyline() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "polyline";
//    if(currentGraph)
//        drawGraph(*currentGraph);
}

void MainWindow::onLayoutLine() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionCurved->setChecked(false);
    layoutStyle = "line";
//    if(currentGraph)
//        drawGraph(*currentGraph);
}

void MainWindow::onLayoutCurved() {
    ui->actionOrthogonal->setChecked(false);
    ui->actionPolyline->setChecked(false);
    ui->actionLine->setChecked(false);
    layoutStyle = "curved";
    //drawGraph(*currentGraph);
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
//        yError()<<"Cannot save scene to"<<filename.toStdString();

    /*
    QPrinter printer( QPrinter::HighResolution );
    //printer.setPageSize( QPrinter::A4 );
    printer.setOrientation( QPrinter::Landscape );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setOutputFileName( filename ); // file will be created in your build directory (where debug/release directories are)
    QPainter p;
    if(!p.begin(&printer)){
        yError() << "Error!";
        return;
    }
    this->scene->render( &p );
    p.end();
    */
}
