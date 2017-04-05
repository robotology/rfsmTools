/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "newrfsmdialog.h"
#include "ui_newrfsmdialog.h"
#include "iostream"


NewRFSMDialog::NewRFSMDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewRFSMDialog)
{
    ui->setupUi(this);

    ui->filenameEdit->setPlaceholderText(QDir::currentPath());
    ui->descriptionEdit->setPlaceholderText("Created using rFSMGui");
    QString authors=qgetenv("USERNAME");
    if(!authors.size())
        authors=qgetenv("USER");
    ui->authorsEdit->setPlaceholderText(authors);
    ui->versionEdit->setPlaceholderText("1.0.0");
    connect(ui->browseButton, SIGNAL(clicked()),this, SLOT(onBrowseFile()));
    connect(ui->filenameEdit,SIGNAL(textChanged(QString)), this, SLOT(onFileNameEditChanged(QString)));
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
}

NewRFSMDialog::~NewRFSMDialog()
{
    delete ui;
}

void NewRFSMDialog::onBrowseFile()
{
    QString filters("rFSM state machine (*.lua);;All files (*.*)");
    QString defaultFilter("rFSM state machine (*.lua)");
    QString filename = QFileDialog::getSaveFileName(0, "Load rFSM state machine",
                                                    QDir::currentPath(),
                                                    filters, &defaultFilter);
    if(filename.size() == 0)
        return;
    ui->filenameEdit->setText(filename);

}

void NewRFSMDialog::onFileNameEditChanged(QString text)
{
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( text.trimmed().size() );

}

QString NewRFSMDialog::getFileName()
{
    if(!ui->filenameEdit->text().trimmed().size())
        return NULL;
    else
        return ui->filenameEdit->text();

}
QString NewRFSMDialog::getDescription()
{
    if(!ui->descriptionEdit->text().trimmed().size())
        return ui->descriptionEdit->placeholderText();
    else
        return ui->descriptionEdit->text();

}
QString NewRFSMDialog::getAuthors()
{
    if(!ui->authorsEdit->text().trimmed().size())
        return ui->authorsEdit->placeholderText();
    else
        return ui->authorsEdit->text();

}
QString NewRFSMDialog::getVersion()
{
    if(!ui->versionEdit->text().trimmed().size())
        return ui->versionEdit->placeholderText();
    else
        return ui->versionEdit->text();

}

