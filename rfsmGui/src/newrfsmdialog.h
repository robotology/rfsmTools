/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */
#ifndef NEWRFSMDIALOG_H
#define NEWRFSMDIALOG_H

#include <QDialog>
#include "qfiledialog.h"

namespace Ui {
class NewRFSMDialog;
}

class NewRFSMDialog : public QDialog
{
    Q_OBJECT
private slots:
    void onBrowseFile();
    void onFileNameEditChanged(QString text);
public:
    explicit NewRFSMDialog(QWidget *parent = 0);
    ~NewRFSMDialog();
    QString getFileName();
    QString getDescription();
    QString getAuthors();
    QString getVersion();

private:
    Ui::NewRFSMDialog *ui;
};

#endif // NEWRFSMDIALOG_H
