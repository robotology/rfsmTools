#ifndef SOURCEEDITORWINDOW_H
#define SOURCEEDITORWINDOW_H

#include <QMainWindow>
#include "highlighter.h"
#include "codeeditor.h"

namespace Ui {
class SourceEditorWindow;
}

class SourceEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SourceEditorWindow(QWidget *parent = 0);
    ~SourceEditorWindow();
    void setSourceCode(const QString& sourceCode, const std::string filename, bool readOnly=false);
    QString& getSourceCode();
    void goToLine(const int line, bool errored=false);
    void setErrorMessage(const QString& message, const int line=0);
    void setReadOnly(bool flag);
    std::string getFileName();

signals:
    void sourceCodeSaved();

private slots:
    void onSave();
    void onClose();
    void closeEvent(QCloseEvent *event);
    void onTextChanged();
    void onFontChanged();

private:
    void showStatusBarMessage(const QString& message,
                              QColor color = Qt::black);
private:
    std::string fileName;
    QString sourceCode;
    Ui::SourceEditorWindow *ui;
    Highlighter *highlighter;


};

#endif // SOURCEEDITORWINDOW_H
