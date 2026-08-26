#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>
#include <QStringList>
#include <QStringListModel>
#include <QInputDialog>
#include <QShortcut>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>

#include "../common/Packet.hpp"
#include "ChatMessageDelegate.hpp"
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

    void removeUser(const LogoutNotificationPacket &logoutNotificationPacket);

    void enableUser(const LoginNotificationPacket &loginNotificationPacket);
    void disableUser(const LogoutNotificationPacket &logoutNotificationPacket);
    //void addUnreadStatus(QStandardItem* item);

private slots:

    //void on_btnDisconnect_clicked();
    void on_btnSend_clicked();
    void on_roomView_clicked(const QModelIndex &index);
    void onRoomAcquired(const RoomInfoPacket& roomInfoPacket);
    void onRoomStatusChanged();
    void onItemMoved(QStandardItem* item);
    void on_btnExport_clicked();
    void onHistorySaved();
    void onRoomDeleted(const RoomDeletedPacket& roomDeletedPacket);
    void showRoomViewContextMenu(const QPoint &position);
    void showSelectedGroupMembers();
    void on_actionNew_Group_triggered();
    void on_actionDelete_Group_triggered();

private:

    void updateGUIAtSwitch(const QModelIndex& index, std::shared_ptr<ChatRoom> chatRoom);
    void addZoomInOut();
    void disableAllBtns();
    void updateChatRoomLabel(const QModelIndex *userIndex);
    void setConnectedBtnStates();
    void setDisconnectedBtnStates();
    void printLoginMessage(const LoginSuccessPacket &loginSuccessPacket);
    void requestLoginInfo();
    void clearChatBoxBg();
    void setChatBoxBg(const QString& filepath);
    void clearStatusbarOnTimer(size_t seconds);
    bool isGroupsCategoryIndex(const QModelIndex &index) const;
    bool isChatGroupIndex(const QModelIndex &index) const;
    void showGroupMembers(const QModelIndex &index);
    void hideGroupMembers();

    Ui::MainWindow *ui;
    Application* m_application;
    ChatMessageDelegate* m_chatMessageDelegate;
    QStandardItemModel m_groupMemberModel;
};
#endif // MAINWINDOW_H
