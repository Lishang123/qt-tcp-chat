#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "CreateGroupDialog.hpp"

#include <QMenu>

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
      , m_application(application) {
    ui->setupUi(this);
    ui->groupMemberView->setVisible(false);
    ui->groupMemberView->setFocusPolicy(Qt::NoFocus);
    ui->groupMemberView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->groupMemberView->setIconSize(QSize(26, 26));
    ui->groupMemberView->setModel(&m_groupMemberModel);
    ui->groupMemberView->setItemDelegate(new ChatRoomDelegate(ui->groupMemberView));
    ui->groupMemberView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->chatbox->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->chatbox->setWordWrap(true);
    m_chatMessageDelegate = new ChatMessageDelegate(ui->chatbox);
    ui->chatbox->setItemDelegate(m_chatMessageDelegate);
    clearChatBoxBg();

    ui->roomView->setHeaderHidden(true);
    ui->roomView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->roomView->setIconSize(QSize(26, 26));
    auto font = ui->roomView->font();
    font.setPointSize(16);
    ui->roomView->setFont(font);
    ui->roomView->setItemDelegate(new ChatRoomDelegate(ui->roomView));
    // get rid of the blue bar when the item is selected
    ui->roomView->setFocusPolicy(Qt::NoFocus);
    ui->roomView->setContextMenuPolicy(Qt::CustomContextMenu);


    m_application->setIconSize(ui->roomView->iconSize());
    ui->lblChatbox->setText("");

    //connect signals and slots
    connect(ui->textMsg, &QLineEdit::returnPressed, this, &MainWindow::on_btnSend_clicked);
    connect(ui->roomView, &QWidget::customContextMenuRequested, this, &MainWindow::showRoomViewContextMenu);
    connect(ui->groupMemberView, &QWidget::customContextMenuRequested, this, &MainWindow::showGroupMemberViewContextMenu);

    connect(m_application, &Application::roomStatusChanged, this, &MainWindow::onRoomStatusChanged);
    connect(m_application, &Application::itemMoved, this, &MainWindow::onItemMoved);
    connect(m_application, &Application::historySaved, this, &MainWindow::onHistorySaved);


    connect(&m_application->getClient(), &Client::disconnected, this, &MainWindow::onClientDisconnected);

    connect(&m_application->getClient(), &Client::notifyUserLogin, this, &MainWindow::enableUser);

    connect(&m_application->getClient(), &Client::notifyUserLogout, this, &MainWindow::disableUser);

    connect(&m_application->getClient(), &Client::messageReceived, this, &MainWindow::onMessageReceived);
    connect(&m_application->getClient(), &Client::errorOccured, this, &MainWindow::onError);
    connect(&m_application->getClient(), &Client::roomAcquired, this, &MainWindow::onRoomAcquired);
    connect(&m_application->getClient(), &Client::roomDeleted, this, &MainWindow::onRoomDeleted);


    //enable buttons
    ui->btnSend->setEnabled(false);
    ui->textMsg->setEnabled(false);
    //set room model
    //ui->chatbox->setModel(&m_application->getChatModel());
    addZoomInOut();
    ui->roomView->setModel(&m_application->getRoomListModel());
    ui->roomView->expandAll();

    QString username = m_application->getClient().getUserName();
    setWindowTitle("Chat Client " + username);
}

MainWindow::~MainWindow() {
    delete ui;
}

//
// void MainWindow::on_btnDisconnect_clicked()
// {
//     qInfo() << "disconnect clicked";
//     disableAllBtns();
//     m_application->disconnectFromHost();
// }


void MainWindow::on_btnSend_clicked() {
    qInfo() << "send clicked";
    if (ui->textMsg->text().isEmpty()) return;
    m_application->sendMessage(ui->textMsg->text());
    ui->textMsg->clear();
}

void MainWindow::removeUser(const LogoutNotificationPacket &logoutNotificationPacket) {
    m_application->removeUser(logoutNotificationPacket);
}

void MainWindow::enableUser(const LoginNotificationPacket &loginNotificationPacket) {
    qInfo() << Q_FUNC_INFO;
    bool keepSelection = false;
    auto *sm = ui->roomView->selectionModel();
    if (sm->hasSelection()) {
        QModelIndex selectedIndex = sm->selectedIndexes().first();
        keepSelection = (selectedIndex.data(UserIdRole).toUuid() == loginNotificationPacket.userId);
        if (!keepSelection) {
            qInfo() << Q_FUNC_INFO << "keepSelection: false" << selectedIndex.data(UserIdRole).toUuid() <<
                    loginNotificationPacket.userId;
        }
    }
    auto userItem = m_application->enableUser(loginNotificationPacket);
    updateGroupMemberOnlineStatus(loginNotificationPacket.userId, true);
    if (keepSelection && userItem) {
        auto index = userItem->index();
        updateChatRoomLabel(&index);
        onItemMoved(userItem);
    }
    qInfo() << Q_FUNC_INFO << "finished";
}

void MainWindow::disableUser(const LogoutNotificationPacket &logoutNotificationPacket) {
    qInfo() << Q_FUNC_INFO;
    bool keepSelection = false;
    auto *sm = ui->roomView->selectionModel();
    if (sm->hasSelection()) {
        QModelIndex selectedIndex = sm->selectedIndexes().first();
        keepSelection = (selectedIndex.data(UserIdRole).toUuid() == logoutNotificationPacket.userId);
        if (!keepSelection) {
            qInfo() << Q_FUNC_INFO << "keepSelection: " << keepSelection << selectedIndex.data(UserIdRole).toUuid() <<
                    logoutNotificationPacket.userId;
        }
    }
    auto userItem = m_application->disableUser(logoutNotificationPacket);
    updateGroupMemberOnlineStatus(logoutNotificationPacket.userId, false);
    if (keepSelection && userItem) {
        auto index = userItem->index();
        updateChatRoomLabel(&index);
        onItemMoved(userItem);
    }
}

void MainWindow::onError(const QString &errorMessage) {
    setDisconnectedBtnStates();
    QMessageBox::critical(this, "Error", errorMessage);
}

void MainWindow::onClientDisconnected() {
    qInfo() << Q_FUNC_INFO;
    //setDisconnectedBtnStates();
    qInfo() << QApplication::topLevelWidgets();
    //setWindowTitle("Chat Client: Logged out");
}

void MainWindow::onMessageReceived(const ChatMessagePacket &chatMessagePacket) {
    //create room, update room message
    m_application->processMessage(chatMessagePacket);
    if (chatMessagePacket.roomId == m_application->getCurrentRoomId())
        ui->chatbox->scrollToBottom();
}

void MainWindow::printLoginMessage(const LoginSuccessPacket &loginSuccessPacket) {
    //TODO: print welcome message
    ui->chatbox->scrollToBottom();
}

void MainWindow::disableAllBtns() {
    ui->btnSend->setEnabled(false);
}

void MainWindow::updateChatRoomLabel(const QModelIndex *userIndex) {
    qInfo() << Q_FUNC_INFO;
    // REMEMBER: this way, returned index is always valid in a tree view (0,0, ...)!!!
    // QModelIndex index = ui->roomView->currentIndex();
    // if (index.isValid()) {
    // Always use selection model instead!!
    if (userIndex->data(OfflineRole).toBool()) {
        qInfo() << Q_FUNC_INFO << ", change label and disable buttons";
        auto text = ui->lblChatbox->text();
        if (!text.endsWith("[Offline]"))
            ui->lblChatbox->setText(text + " [Offline]");
        ui->btnSend->setEnabled(false);
        ui->textMsg->setEnabled(false);
        return;
    }
    auto room = m_application->getCurrentRoom();
    if (!room) return;
    ui->lblChatbox->setText(room->getRoomName());
    ui->btnSend->setEnabled(true);
    ui->textMsg->setEnabled(true);
}


void MainWindow::setConnectedBtnStates() {
    ui->btnSend->setEnabled(true);
}

void MainWindow::setDisconnectedBtnStates() {
    ui->btnSend->setEnabled(false);
}

void MainWindow::requestLoginInfo() {
    qInfo() << Q_FUNC_INFO;
    while (true) {
        QString username = QInputDialog::getText(
            this, //parent widget
            "Name", //title
            "What is your name?", //label text inside of the dialog
            QLineEdit::EchoMode::Normal, //show typed text normally
            m_application->getClient().getUserName() // The pre-filled text inside the input field.
        );
        qInfo() << "username: " << username;
        if (username.isEmpty()) {
            QMessageBox::critical(this, "Error", "Please enter a valid name!");
            continue;
        }
        return m_application->sendLoginRequest(LoginRequestPacket{username, 0, "password"});
    }
}

void MainWindow::clearChatBoxBg() {
    ui->chatbox->setStyleSheet(
        "QListView{"
        "background-color: white;"
        "}"
    );
}

void MainWindow::setChatBoxBg(const QString &filepath) {
    ui->chatbox->setStyleSheet(
        "QListView{"
        "background-image: url(:/background/background/botanics.png);"
        "background-attachment: scrolled;"
        "}"
    );
}

void MainWindow::clearStatusbarOnTimer(size_t seconds) {
    QTimer::singleShot(seconds, [this]() {
           ui->statusbar->clearMessage();
        });
}

void MainWindow::on_roomView_clicked(const QModelIndex &index) {
    qInfo() << Q_FUNC_INFO << ", index : " << index << "clicked";
    auto chatRoom = m_application->switchRoom(index);
    if (!chatRoom) {
        //clearChatBoxBg();
        //ui->btnSend->setEnabled(false);
        //ui->textMsg->setEnabled(false);
        return;
    };
    //update the GUI
    updateGUIAtSwitch(index, chatRoom);
}

void MainWindow::onRoomAcquired(const RoomInfoPacket &roomInfoPacket) {
    qInfo() << Q_FUNC_INFO;
    RoomInfo roomInfo = roomInfoPacket.roomInfo;
    QModelIndex switchedIndex;
    std::shared_ptr<ChatRoom> switchedRoom;
    switch (roomInfo.roomType) {
        case RoomType::DirectChat: {
            auto userInfos = roomInfo.userInfos;
            assert(userInfos.count() == 1);
            if (!m_application->setRoomIdOnUser(roomInfo.roomId, userInfos.firstKey(), true)) {
                qCritical() << Q_FUNC_INFO << "Should not happend!";
                return;
            }
            switchedIndex = ui->roomView->currentIndex();
            switchedRoom = m_application->getCurrentRoom();
            break;
        }
        case RoomType::Chatgroup: {
            auto *item = m_application->addChatGroupFromRoomInfo(roomInfo);
            if (!item) {
                return;
            }
            switchedIndex = item->index();
            ui->roomView->setCurrentIndex(switchedIndex);
            switchedRoom = m_application->switchRoom(switchedIndex);
            break;
        }
        case RoomType::Self:
        case RoomType::Public:
        default: {
            qInfo() << Q_FUNC_INFO << "This shouldn't happen!";
            return;
        }
    }

    if (switchedRoom) {
        updateGUIAtSwitch(switchedIndex, switchedRoom);
    }
    qCritical() << Q_FUNC_INFO << "finished.";
}

void MainWindow::onRoomStatusChanged() {
    qInfo() << Q_FUNC_INFO;
    ui->roomView->viewport()->update();
    qInfo() << Q_FUNC_INFO << "finished";
}

void MainWindow::onItemMoved(QStandardItem *item) {
    qInfo() << Q_FUNC_INFO;
    auto sm = ui->roomView->selectionModel();
    // Nothing is selected: no need to change selection
    if (!sm->hasSelection()) return;
    sm->clearSelection();
    ui->roomView->setCurrentIndex(item->index());
    onRoomStatusChanged();
}

void MainWindow::addZoomInOut() {
    auto chatBoxView = ui->chatbox;
    auto zoomIn = new QShortcut(QKeySequence::ZoomIn, chatBoxView);
    auto zoomOut = new QShortcut(QKeySequence::ZoomOut, chatBoxView);
    connect(zoomIn, &QShortcut::activated, [chatBoxView] {
        QFont font = chatBoxView->font();
        font.setPointSize(font.pointSize() + 1);
        chatBoxView->setFont(font);
    });
    connect(zoomOut, &QShortcut::activated, [chatBoxView] {
        QFont f = chatBoxView->font();
        if (f.pointSize() > 6)
            f.setPointSize(f.pointSize() - 1);
        chatBoxView->setFont(f);
    });
}


void MainWindow::on_btnExport_clicked()
{
    // Ask file name and format
    qInfo() << Q_FUNC_INFO;
    const QString format = "html"; // default format
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); // picture folder
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();
    initialPath += tr("/untitled.") + format; //tr: auto translation

    QFileDialog fileDialog(this, tr("Save As"), initialPath);
    // This means the dialog is for saving, not opening.
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    // This allows the user to type a new file name that does not exist yet.
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);

    // Get supported formats
    QString filter =
    tr("HTML (*.html *.htm);;"
       "Text (*.txt);;"
       "JSON (*.json);;"
       "PDF (*.pdf)");
    fileDialog.setNameFilter(filter);
    fileDialog.selectMimeTypeFilter("text/" + format);
    fileDialog.setDefaultSuffix(format);
    if (fileDialog.exec() != QDialog::Accepted)
        return;

    // Get selected filename
    // Even though this is a save dialog and usually only one file is selected, Qt still returns a list
    // so get the first element.
    QList selectedFiles = fileDialog.selectedFiles();
    if (!selectedFiles.empty())
    {
        const QString fileName = selectedFiles.first();
        auto nameFilter = fileDialog.selectedNameFilter();
        Application::ExportFormat chosenFormat;
        if (nameFilter.startsWith("HTML")) chosenFormat = Application::ExportFormat::HTML;
        else if (nameFilter.startsWith("Text")) chosenFormat = Application::ExportFormat::TXT;
        else if (nameFilter.startsWith("JSON")) chosenFormat = Application::ExportFormat::JSON;
        else if (nameFilter.startsWith("PDF")) chosenFormat = Application::ExportFormat::PDF;
        else {
            chosenFormat = Application::ExportFormat::UNKNOWN;
            qCritical() << Q_FUNC_INFO << "unknown format!";
        };
        if (m_application->exportHistory(fileName, chosenFormat)) {
            ui->statusbar->showMessage("Chat exported to " + fileName);
        }
        else {
            ui->statusbar->showMessage("Chat export failed");
        }
        clearStatusbarOnTimer(3000);
    }
}

void MainWindow::onHistorySaved() {
    ui->statusbar->showMessage("Chat history saved locally.");
    clearStatusbarOnTimer(3000);
}

bool MainWindow::isGroupsCategoryIndex(const QModelIndex &index) const
{
    return index.isValid()
        && !index.parent().isValid()
        && index.data(Qt::UserRole).toInt() == 0;
}

bool MainWindow::isChatGroupIndex(const QModelIndex &index) const
{
    return index.isValid()
        && index.parent().isValid()
        && static_cast<RoomType>(index.data(RoomTypeRole).toInt()) == RoomType::Chatgroup;
}

void MainWindow::updateGroupMemberOnlineStatus(const QUuid &userId, bool online)
{
    for (int row = 0; row < m_groupMemberModel.rowCount(); ++row) {
        QStandardItem *item = m_groupMemberModel.item(row);
        if (!item || item->data(UserIdRole).toUuid() != userId) {
            continue;
        }

        const QString offlineSuffix = tr(" [Offline]");
        QString username = item->text();
        if (username.endsWith(offlineSuffix)) {
            username.chop(offlineSuffix.size());
        }

        item->setText(online ? username : username + offlineSuffix);
        item->setData(!online, OfflineRole);
        ui->groupMemberView->viewport()->update();
        return;
    }
}


void MainWindow::on_groupMemberView_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const QUuid userId = index.data(UserIdRole).toUuid();
    if (userId.isNull()) {
        qCritical() << Q_FUNC_INFO << "group member item has no user id";
        return;
    }
    auto userItem = m_application->getUserItem(userId);
    if (userItem) {
        auto chatRoom = m_application->switchRoom(userItem->index());
        if (chatRoom) {
            updateGUIAtSwitch(userItem->index(), chatRoom);
        }
        return;
    }
    qCritical() << Q_FUNC_INFO << "user item not found:" << userId;
}

void MainWindow::showGroupMemberViewContextMenu(const QPoint &position) {
    const QModelIndex index = ui->groupMemberView->indexAt(position);
    if (!index.isValid()) {
        return;
    }

    QMenu menu(this);
    menu.addAction(tr("Direct Message"), this, [this, index]() {
        on_groupMemberView_doubleClicked(index);
    });
    menu.exec(ui->groupMemberView->viewport()->mapToGlobal(position));
}

void MainWindow::showRoomViewContextMenu(const QPoint &position)
{
    const QModelIndex index = ui->roomView->indexAt(position);
    QMenu menu(this);
    if (isGroupsCategoryIndex(index)) {
        menu.addAction(ui->actionNew_Group);
    }
    else if (isChatGroupIndex(index)) {
        ui->roomView->setCurrentIndex(index);
        menu.addAction(tr("Show Members"), this, &MainWindow::showSelectedGroupMembers);
        menu.addAction(ui->actionDelete_Group);
    }
    else {
        return;
    }

    menu.exec(ui->roomView->viewport()->mapToGlobal(position));
}

void MainWindow::showSelectedGroupMembers()
{
    const QModelIndex index = ui->roomView->currentIndex();
    if (!isChatGroupIndex(index)) {
        return;
    }
    on_roomView_clicked(index);
    showGroupMembers(index);
}

void MainWindow::showGroupMembers(const QModelIndex &index)
{
    if (!isChatGroupIndex(index)) {
        hideGroupMembers();
        return;
    }

    m_groupMemberModel.clear();
    const auto members = m_application->getRoomMembers(index.data(RoomIdRole).toUuid());
    for (const auto &[userId, userInfo] : members.asKeyValueRange()) {
        QString text = userInfo.username;
        if (!userInfo.isOnline) {
            text += tr(" [Offline]");
        }
        auto *item = new QStandardItem(QIcon(":/icons/icons/mann-avatar.png"), text);
        item->setData(userId, UserIdRole);
        item->setData(!userInfo.isOnline, OfflineRole);
        m_groupMemberModel.appendRow(item);
    }
    ui->groupMemberView->setVisible(true);
}

void MainWindow::hideGroupMembers()
{
    m_groupMemberModel.clear();
    ui->groupMemberView->setVisible(false);
}

void MainWindow::on_actionNew_Group_triggered()
{
    CreateGroupDialog dialog(m_application->getContacts(), this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    if (!m_application->sendChatGroupRequest(dialog.groupName(), dialog.selectedUserIds())) {
        QMessageBox::warning(this, tr("Create Group"), tr("Enter a group name and select at least one contact."));
    }
}


void MainWindow::on_actionDelete_Group_triggered()
{
    const QModelIndex index = ui->roomView->currentIndex();
    if (!index.isValid() || static_cast<RoomType>(index.data(RoomTypeRole).toInt()) != RoomType::Chatgroup) {
        QMessageBox::information(this, tr("Delete Group"), tr("Select a group to delete."));
        return;
    }

    const QString roomName = index.data(Qt::DisplayRole).toString();
    const auto result = QMessageBox::question(
        this,
        tr("Delete Group"),
        tr("Delete group \"%1\"?").arg(roomName));
    if (result != QMessageBox::Yes) {
        return;
    }

    if (!m_application->sendDeleteGroupRequest(index.data(RoomIdRole).toUuid())) {
        QMessageBox::warning(this, tr("Delete Group"), tr("Cannot delete the selected group."));
    }
}

void MainWindow::updateGUIAtSwitch(const QModelIndex &index, std::shared_ptr<ChatRoom> chatRoom) {
    if (!chatRoom) {
        return;
    }
    auto roomType = chatRoom->getRoomType();
    bool showSenderName = (roomType == RoomType::Public || roomType == RoomType::Chatgroup);
    m_chatMessageDelegate->setShowSender(showSenderName);
    if (ui->groupMemberView->isVisible()) {
        if (roomType == RoomType::Chatgroup) {
            showGroupMembers(index);
        }
        else {
            hideGroupMembers();
        }
    }
    setChatBoxBg({});
    // room label, buttons/fields
    ui->lblChatbox->setText(chatRoom->getRoomName());
    updateChatRoomLabel(&index);
    // chat box
    ui->chatbox->setModel(m_application->getChatModel());

    // debug info
    auto *sm = ui->roomView->selectionModel();
    if (sm->hasSelection()) {
        QModelIndex index = sm->selectedIndexes().first();
        qDebug() << Q_FUNC_INFO << index.data(UserIdRole).toString();
    }
}

void MainWindow::onRoomDeleted(const RoomDeletedPacket &roomDeletedPacket)
{
    const bool deletedCurrentRoom = roomDeletedPacket.roomId == m_application->getCurrentRoomId();
    if (!m_application->removeChatGroup(roomDeletedPacket.roomId)) {
        return;
    }
    if (deletedCurrentRoom) {
        ui->chatbox->setModel(nullptr);
        ui->lblChatbox->clear();
        ui->btnSend->setEnabled(false);
        ui->textMsg->setEnabled(false);
    }
}
