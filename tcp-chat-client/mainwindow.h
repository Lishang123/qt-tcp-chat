#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>
#include <QStringList>
#include <QStringListModel>
#include <QInputDialog>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    void on_btnConnect_clicked();

    void on_btnDisconnect_clicked();

    void on_btnSend_clicked();

    void onConnected();

    void onDisconnected();

    void OnReadyRead();

    void error(QAbstractSocket::SocketError error);

private:

    void disableAllBtns();
    void setConnectedBtnStates();
    void setDisconnectedBtnStates();

    Ui::MainWindow *ui;
    QTcpSocket m_socket;
    QStringList m_list;
    QStringListModel m_model;
    QString m_name;

};
#endif // MAINWINDOW_H
