#ifndef SOURCEEDITORWINDOW_H
#define SOURCEEDITORWINDOW_H

#include <QMainWindow>
#include "highlighter.h"

namespace Ui {
class SourceEditorWindow;
}

class SourceEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SourceEditorWindow(QWidget *parent = 0);
    ~SourceEditorWindow();
    void setSourceCode(const QString& sourceCode);
    QString& getSourceCode();

signals:
    void sourceCodeSaved();

private slots:
    void onSave();
    void onClose();
    void closeEvent(QCloseEvent *event);


private:    
    QString sourceCode;
    Ui::SourceEditorWindow *ui;
    Highlighter *highlighter;

};

#endif // SOURCEEDITORWINDOW_H
