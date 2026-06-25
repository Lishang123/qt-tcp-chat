#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_application(application)
{
    ui->setupUi(this);
    ui->chatbox->setEditTriggers(QAbstractItemView::NoEditTriggers);


    ui->roomView->setHeaderHidden(true);
    ui->roomView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->roomView->setIconSize(QSize(26, 26));
    auto font = ui->roomView->font();
    font.setPointSize(16);
    ui->roomView->setFont(font);
    ui->roomView->setItemDelegate(new ChatRoomDelegate(ui->roomView));
    ui->roomView->setFocusPolicy(Qt::NoFocus);


    m_application->setIconSize(ui->roomView->iconSize());
    ui->lblChatbox->setText("");

    //connect signals and slots
    connect(ui->textMsg, &QLineEdit::returnPressed, this, &MainWindow::on_btnSend_clicked);

    connect(m_application, &Application::roomStatusChanged, this, &MainWindow::onRoomStatusChanged);
    connect(m_application, &Application::itemMoved, this, &MainWindow::onItemMoved);

    connect(&m_application->getClient(), &Client::disconnected, this, &MainWindow::onClientDisconnected);

    connect(&m_application->getClient(), &Client::notifyUserLogin, this, &MainWindow::enableUser);
    // connect(&m_application->getClient(), &Client::notifyUserLogout, this, &MainWindow::removeUser);
    connect(&m_application->getClient(), &Client::notifyUserLogout, this, &MainWindow::disableUser);

    connect(&m_application->getClient(), &Client::messageReceived, this, &MainWindow::onMessageReceived);
    connect(&m_application->getClient(), &Client::errorOccured, this, &MainWindow::onError);
    connect(&m_application->getClient(), &Client::roomAcquired, this, &MainWindow::onRoomAcquired);


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

MainWindow::~MainWindow()
{
    delete ui;
}

//
// void MainWindow::on_btnDisconnect_clicked()
// {
//     qInfo() << "disconnect clicked";
//     disableAllBtns();
//     m_application->disconnectFromHost();
// }


void MainWindow::on_btnSend_clicked()
{
    qInfo() << "send clicked";
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
            qInfo() << Q_FUNC_INFO << "keepSelection: false" << selectedIndex.data(UserIdRole).toUuid() << loginNotificationPacket.userId;
        }
    }
    auto userItem = m_application->enableUser(loginNotificationPacket);
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
            qInfo() << Q_FUNC_INFO << "keepSelection: " << keepSelection << selectedIndex.data(UserIdRole).toUuid() << logoutNotificationPacket.userId;
        }
    }
    auto userItem = m_application->disableUser(logoutNotificationPacket);
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

void MainWindow::onMessageReceived(const ChatMessagePacket& chatMessagePacket) {
    //create room, update room message
    m_application->processMessage(chatMessagePacket);
    if (chatMessagePacket.roomId == m_application->getCurrentRoomId())
        ui->chatbox->scrollToBottom();
    //TODO: move user to the top!
}

void MainWindow::printLoginMessage(const LoginSuccessPacket &loginSuccessPacket) {
    //TODO: print welcome message
    ui->chatbox->scrollToBottom();
}

void MainWindow::disableAllBtns()
{
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


void MainWindow::setConnectedBtnStates()
{
    ui->btnSend->setEnabled(true);
}

void MainWindow::setDisconnectedBtnStates()
{
    ui->btnSend->setEnabled(false);
}

void MainWindow::requestLoginInfo() {
    qInfo() << Q_FUNC_INFO;
    while (true) {
        QString username = QInputDialog::getText(
            this, //parent widget
        "Name", //title
        "What is your name?",//label text inside of the dialog
        QLineEdit::EchoMode::Normal, //show typed text normally
        m_application->getClient().getUserName() // The pre-filled text inside the input field.
        );
        qInfo() << "username: " << username;
        if (username.isEmpty()){
            QMessageBox::critical(this, "Error", "Please enter a valid name!");
            continue;
        }
        return m_application->sendLoginRequest(LoginRequestPacket{username, 0, "password"});
    }
}

void MainWindow::on_roomView_clicked(const QModelIndex &index)
{
    qInfo() << Q_FUNC_INFO << ", index : " << index << "clicked";
    auto chatRoom = m_application->switchRoom(index);
    if (!chatRoom) return;
    //update the GUI
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

void MainWindow::onRoomAcquired(const RoomInfoPacket &roomInfoPacket) {
    qInfo() << Q_FUNC_INFO;
    RoomInfo roomInfo = roomInfoPacket.roomInfo;
    switch (roomInfo.roomType) {
        case RoomType::DirectChat: {
            auto userInfos = roomInfo.userInfos;
            assert(userInfos.count() == 1);
            if (!m_application->setRoomIdOnUser(roomInfo.roomId, userInfos.firstKey(), true)) {
                qCritical() << Q_FUNC_INFO << "Should not happend!";
                return;
            }
            break;
        }
        case RoomType::Chatgroup: {
            qCritical() << Q_FUNC_INFO << "handle chatGroup here!";
            break;
        }
        case RoomType::Self:
        case RoomType::Public:
        default: {
            qInfo() << Q_FUNC_INFO << "This shouldn't happen!";
        }
    }
    qCritical() << Q_FUNC_INFO << "Setting chat model...";
    ui->chatbox->setModel(m_application->getChatModel());
    QModelIndex index = ui->roomView->currentIndex();
    if (index.isValid()) {
        QString text = index.data(Qt::DisplayRole).toString();
        ui->lblChatbox->setText(text);
    }
    ui->btnSend->setEnabled(true);
    ui->textMsg->setEnabled(true);
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

