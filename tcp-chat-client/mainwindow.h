#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>
#include <QStringList>
#include <QStringListModel>
#include <QInputDialog>
#include <QMessageBox>
#include "../common/Packet.hpp"
#include "Application.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(Application *application, QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:

    void onClientDisconnected();
    void onMessageReceived(const ChatMessagePacket& chatMessagePacket);
    void onError(const QString& errorMessage);
    void addUser(const LoginNotificationPacket &loginNotificationPacket);
    void removeUser(const LogoutNotificationPacket &logoutNotificationPacket);

private slots:

    void on_btnDisconnect_clicked();
    void on_btnSend_clicked();
    void on_roomView_clicked(const QModelIndex &index);
    void onRoomAcquired(const RoomInfoPacket& roomInfoPacket);

private:

    void disableAllBtns();
    void setConnectedBtnStates();
    void setDisconnectedBtnStates();
    void printLoginMessage(const LoginSuccessPacket &loginSuccessPacket);

    void requestLoginInfo();

    Ui::MainWindow *ui;
    Application* m_application;
};
#endif // MAINWINDOW_H
