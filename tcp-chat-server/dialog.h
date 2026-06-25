#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "Application.hpp"
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog() override;
    //void connectToController(Application& application);

private slots:

    void on_btnStart_clicked();

    void on_btnStop_clicked();

private:

    void init();
    void clientChanged();
    void enableBtnsStarted();
    void enableBtnsStopped();

    Ui::Dialog *ui;
    Application m_application;
    //Server m_server;
};
#endif // DIALOG_H
