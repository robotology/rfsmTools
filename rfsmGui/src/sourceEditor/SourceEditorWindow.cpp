/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <iostream>

#include "SourceEditorWindow.h"
#include "ui_SourceEditorWindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFontDialog>
#include <QSettings>

SourceEditorWindow::SourceEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SourceEditorWindow)
{    
    ui->setupUi(this);

    QSettings settings;
    QFont font;
    font.setFixedPitch(true);
    font.setFamily(settings.value("editor-font-name", "Courier").toString());
    font.setPointSize(settings.value("editor-font-size", 10).toInt());
    ui->textEdit->setFont(font);
    QFontMetrics metrics(font);
    ui->textEdit->setTabStopWidth(settings.value("editor-tab-size", 4).toInt() * metrics.width(' '));
    ui->action_Save->setEnabled(false);
    highlighter = new Highlighter(ui->textEdit->document());

    connect(ui->action_Save, SIGNAL(triggered()),this,SLOT(onSave()));
    connect(ui->action_Close, SIGNAL(triggered()),this,SLOT(onClose()));
    connect(ui->textEdit, SIGNAL(textChanged()),this,SLOT(onTextChanged()));
    connect(ui->action_Font, SIGNAL(triggered()),this,SLOT(onFontChanged()));

}

SourceEditorWindow::~SourceEditorWindow()
{
    delete ui;
}


void SourceEditorWindow::onTextChanged() {
    ui->action_Save->setEnabled(true);
}


void SourceEditorWindow::setSourceCode(const QString& sourceCode, const std::string filename, bool readOnly){
    SourceEditorWindow::sourceCode = sourceCode;
    SourceEditorWindow::fileName = filename;
    ui->textEdit->clear();
    ui->textEdit->setPlainText(sourceCode);
    showStatusBarMessage("");
    ui->action_Save->setEnabled(false);
    setReadOnly(readOnly);

}

void SourceEditorWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("editor-tab-size", settings.value("editor-tab-size", 4).toInt());
    if(ui->action_Save->isEnabled()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Close", "The source code has been changed.\n Do you want to save it?",
                                      QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes) {
            onSave();
        }
    }

    QWidget::closeEvent(event);
}

void SourceEditorWindow::showEvent(QShowEvent* event){
    showStatusBarMessage(fileName.c_str());
    QWidget::showEvent(event);
}

void SourceEditorWindow::onClose() {

    SourceEditorWindow::close();

}


void SourceEditorWindow::onSave() {
    showStatusBarMessage("Saved!");
    sourceCode = ui->textEdit->toPlainText();
    // clear extra selections
    QList<QTextEdit::ExtraSelection> extraSelections;
    ui->textEdit->setExtraSelections(extraSelections);
    ui->action_Save->setEnabled(false);
    emit sourceCodeSaved();
}


QString& SourceEditorWindow::getSourceCode() {
    return sourceCode;
}

void SourceEditorWindow::showStatusBarMessage(const QString& message,
                          QColor color) {
    ui->statusbar->showMessage(message);
    QPalette palette;
    palette.setColor( QPalette::WindowText, color);
    statusBar()->setPalette(palette);
}

void SourceEditorWindow::goToLine(const int line, bool errored)
{
    QTextEdit::ExtraSelection highlight;
    highlight.cursor = ui->textEdit->textCursor();
    highlight.cursor.setPosition(0);
    highlight.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line);
    ui->textEdit->setTextCursor(highlight.cursor);
    highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    QColor lineColor;
    if(errored)
        lineColor = QColor("#FA8072");
    else
        lineColor = QColor(Qt::yellow).lighter(160);
    highlight.format.setBackground(lineColor);
    QList<QTextEdit::ExtraSelection> extras;
    extras << highlight;
    ui->textEdit->setExtraSelections( extras );

}

void SourceEditorWindow::setErrorMessage(const QString& message, const int line) {
    showStatusBarMessage(message, Qt::darkRed);
    goToLine(line,true);

}

void SourceEditorWindow::setReadOnly(bool flag) {
    ui->textEdit->setReadOnly(flag);
}


std::string SourceEditorWindow::getFileName(){
    return fileName;
}

void SourceEditorWindow::onFontChanged() {
    bool ok;
    QSettings settings;
    QFont font = QFontDialog::getFont( &ok,
                                       QFont(settings.value("editor-font-name", "Courier").toString(),
                                             settings.value("editor-font-size", 10).toInt()),
                                       this, tr("Set Editor font") );
    if(ok) {
        ui->textEdit->setFont(font);
        QFontMetrics metrics(font);
        ui->textEdit->setTabStopWidth(4 * metrics.width(' '));
        settings.setValue("editor-font-name", font.family());
        settings.setValue("editor-font-size", font.pointSize());
    }
}


