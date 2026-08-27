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

/**
 * @brief Main chat window that binds UI widgets to the application facade.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Creates the main window for an initialized client application.
     */
    explicit MainWindow(Application *application, QWidget *parent = nullptr);

    /**
     * @brief Destroys the generated UI and owned delegates.
     */
    ~MainWindow() override;

public slots:

    /**
     * @brief Updates the UI when the client disconnects from the server.
     */
    void onClientDisconnected();

    /**
     * @brief Adds an incoming message to application state and refreshes the view.
     */
    void onMessageReceived(const ChatMessagePacket& chatMessagePacket);

    /**
     * @brief Displays a network or application error.
     */
    void onError(const QString& errorMessage);

    /**
     * @brief Removes a user from the visible contact list.
     */
    void removeUser(const LogoutNotificationPacket &logoutNotificationPacket);

    /**
     * @brief Marks a user online in the visible contact list.
     */
    void enableUser(const LoginNotificationPacket &loginNotificationPacket);

    /**
     * @brief Marks a user offline in the visible contact list.
     */
    void disableUser(const LogoutNotificationPacket &logoutNotificationPacket);
    //void addUnreadStatus(QStandardItem* item);

private slots:

    //void on_btnDisconnect_clicked();
    /**
     * @brief Sends the current message text.
     */
    void on_btnSend_clicked();

    /**
     * @brief Switches rooms when the room list is clicked.
     */
    void on_roomView_clicked(const QModelIndex &index);

    /**
     * @brief Adds or activates a room after the server grants it.
     */
    void onRoomAcquired(const RoomInfoPacket& roomInfoPacket);

    /**
     * @brief Refreshes room-list painting after room status changes.
     */
    void onRoomStatusChanged();

    /**
     * @brief Handles room-list item movement notifications.
     */
    void onItemMoved(QStandardItem* item);

    /**
     * @brief Prompts for an export path and exports current history.
     */
    void on_btnExport_clicked();

    /**
     * @brief Shows saved-history feedback.
     */
    void onHistorySaved();

    /**
     * @brief Removes a deleted room from the UI.
     */
    void onRoomDeleted(const RoomDeletedPacket& roomDeletedPacket);

    /**
     * @brief Shows the room-list context menu.
     */
    void showRoomViewContextMenu(const QPoint &position);

    /**
     * @brief Shows the group-member context menu.
     */
    void showGroupMemberViewContextMenu(const QPoint &position);

    /**
     * @brief Displays members for the selected group.
     */
    void showSelectedGroupMembers();

    /**
     * @brief Opens the create-group dialog.
     */
    void on_actionNew_Group_triggered();

    /**
     * @brief Requests deletion for the selected group.
     */
    void on_actionDelete_Group_triggered();

    /**
     * @brief Opens direct chat for a double-clicked group member.
     */
    void on_groupMemberView_doubleClicked(const QModelIndex &index);

private:

    /**
     * @brief Updates widgets after switching the active room.
     */
    void updateGUIAtSwitch(const QModelIndex& index, std::shared_ptr<ChatRoom> chatRoom);

    /**
     * @brief Installs chat zoom shortcuts.
     */
    void addZoomInOut();

    /**
     * @brief Disables chat action buttons.
     */
    void disableAllBtns();

    /**
     * @brief Updates the room title from the selected room-list item.
     */
    void updateChatRoomLabel(const QModelIndex *userIndex);

    /**
     * @brief Applies button states for a connected client.
     */
    void setConnectedBtnStates();

    /**
     * @brief Applies button states for a disconnected client.
     */
    void setDisconnectedBtnStates();

    /**
     * @brief Prints the welcome/login message into the chat view.
     */
    void printLoginMessage(const LoginSuccessPacket &loginSuccessPacket);

    /**
     * @brief Opens the login flow.
     */
    void requestLoginInfo();

    /**
     * @brief Removes the chat box background image.
     */
    void clearChatBoxBg();

    /**
     * @brief Sets the chat box background image.
     */
    void setChatBoxBg(const QString& filepath);

    /**
     * @brief Clears status bar text after a delay.
     */
    void clearStatusbarOnTimer(size_t seconds);

    /**
     * @brief Adds the group category control to the room list.
     */
    void addGroupCategoryButton();

    /**
     * @brief Returns whether an index is the groups category row.
     */
    bool isGroupsCategoryIndex(const QModelIndex &index) const;

    /**
     * @brief Returns whether an index points to a chat group row.
     */
    bool isChatGroupIndex(const QModelIndex &index) const;

    /**
     * @brief Populates the group member view for a selected group.
     */
    void showGroupMembers(const QModelIndex &index);

    /**
     * @brief Hides and clears the group member view.
     */
    void hideGroupMembers();

    /**
     * @brief Updates one displayed group member's online status.
     */
    void updateGroupMemberOnlineStatus(const QUuid &userId, bool online);

    Ui::MainWindow *ui;
    Application* m_application;
    ChatMessageDelegate* m_chatMessageDelegate;
    QStandardItemModel m_groupMemberModel;
};
#endif // MAINWINDOW_H
