#include "SourceEditorWindow.h"
#include "ui_SourceEditorWindow.h"
#include <QMessageBox>
#include <QShortcut>
#include <QCloseEvent>

SourceEditorWindow::SourceEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SourceEditorWindow)
{    
    ui->setupUi(this);

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    ui->textEdit->setFont(font);

    highlighter = new Highlighter(ui->textEdit->document());    

    connect(ui->action_Save, SIGNAL(triggered()),this,SLOT(onSave()));
    connect(ui->action_Close, SIGNAL(triggered()),this,SLOT(onClose()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(onSave()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(onClose()));
}

SourceEditorWindow::~SourceEditorWindow()
{
    delete ui;
}

void SourceEditorWindow::setSourceCode(const QString& sourceCode){
    SourceEditorWindow::sourceCode = sourceCode;
    ui->textEdit->setPlainText(sourceCode);
}

void SourceEditorWindow::closeEvent(QCloseEvent *event)
{
    onClose();
    QWidget::closeEvent(event);
}

void SourceEditorWindow::onClose() {

    if((sourceCode != ui->textEdit->toPlainText())) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Close", "The source code has been changed.\n Do you want to save it?",
                                      QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes) {
            onSave();
        }
    }
    SourceEditorWindow::close();
}


void SourceEditorWindow::onSave() {
    sourceCode = ui->textEdit->toPlainText();
    emit sourceCodeSaved();
}


QString& SourceEditorWindow::getSourceCode() {
    return sourceCode;
}

