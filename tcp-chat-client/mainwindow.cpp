#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_application(application)
{
    ui->setupUi(this);
    ui->roomView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->lblChatbox->setText("");

    //connect signals and slots
    connect(ui->textMsg, &QLineEdit::returnPressed, this, &MainWindow::on_btnSend_clicked);

    //connect(&m_application->getClient(), &Client::connecloggedInted, this, &MainWindow::onClientConnected);
    connect(&m_application->getClient(), &Client::disconnected, this, &MainWindow::onClientDisconnected);
    connect(&m_application->getClient(), &Client::notifyUserLogin, this, &MainWindow::addUser);
    connect(&m_application->getClient(), &Client::notifyUserLogout, this, &MainWindow::removeUser);

    connect(&m_application->getClient(), &Client::messageReceived, this, &MainWindow::onMessageReceived);
    connect(&m_application->getClient(), &Client::errorOccured, this, &MainWindow::onError);
    connect(&m_application->getClient(), &Client::roomAcquired, this, &MainWindow::onRoomAcquired);



    //enable buttons
    ui->btnSend->setEnabled(false);
    ui->textMsg->setEnabled(false);
    //set room model
    //ui->chatbox->setModel(&m_application->getChatModel());
    ui->roomView->setModel(&m_application->getRoomListModel());

    QString username = m_application->getClient().getUserName();
    setWindowTitle("Chat Client " + username);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnDisconnect_clicked()
{
    qInfo() << "disconnect clicked";
    disableAllBtns();
    m_application->disconnectFromHost();
}


void MainWindow::on_btnSend_clicked()
{
    qInfo() << "send clicked";
    m_application->sendMessage(ui->textMsg->text());
    ui->textMsg->clear();
}

void MainWindow::addUser(const LoginNotificationPacket &loginNotificationPacket) {
    m_application->addUser(QUuid(), loginNotificationPacket.userId, loginNotificationPacket.username);
}

void MainWindow::removeUser(const LogoutNotificationPacket &logoutNotificationPacket) {
    m_application->removeUser(logoutNotificationPacket);
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
}

void MainWindow::printLoginMessage(const LoginSuccessPacket &loginSuccessPacket) {
    //TODO: print welcome message
    ui->chatbox->scrollToBottom();
}

void MainWindow::disableAllBtns()
{
    ui->btnSend->setEnabled(false);
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
    ui->lblChatbox->setText(chatRoom->m_room_name());
    ui->chatbox->setModel(m_application->getChatModel());
    ui->btnSend->setEnabled(true);
    ui->textMsg->setEnabled(true);
}

void MainWindow::onRoomAcquired(const RoomInfoPacket &roomInfoPacket) {
    qInfo() << Q_FUNC_INFO;
    RoomInfo roomInfo = roomInfoPacket.roomInfo;
    switch (roomInfo.roomType) {
        case RoomType::DirectChat: {
            auto userIds = roomInfo.userIds;
            assert(userIds.count() == 2);
            if (!m_application->setRoomIdOnUser(roomInfo.roomId, userIds[1], true)) {
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

