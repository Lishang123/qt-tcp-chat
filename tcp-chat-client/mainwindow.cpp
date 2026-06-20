#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_application(application)
{
    ui->setupUi(this);

    for (int i = 0; i < ui->horizontalLayout->count(); ++i) {
        QLayoutItem* item = ui->horizontalLayout->itemAt(i);

        if (QWidget* w = item->widget()) {
            w->hide();
        }
    }
    //connect signals and slots
    connect(ui->textMsg, &QLineEdit::returnPressed, this, &MainWindow::on_btnSend_clicked);

    //connect(&m_application->getClient(), &Client::connecloggedInted, this, &MainWindow::onClientConnected);
    connect(&m_application->getClient(), &Client::disconnected, this, &MainWindow::onClientDisconnected);
    connect(&m_application->getClient(), &Client::notifyUserLogin, this, &MainWindow::addUser);
    connect(&m_application->getClient(), &Client::notifyUserLogout, this, &MainWindow::removeUser);

    connect(&m_application->getClient(), &Client::messageReceived, this, &MainWindow::onMessageReceived);
    connect(&m_application->getClient(), &Client::errorOccured, this, &MainWindow::onError);


    //enable buttons
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(true);
    ui->btnSend->setEnabled(true);

    //set string list model
    ui->chatbox->setModel(&m_application->getChatModel());
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
    m_application->addUser(loginNotificationPacket.userId, loginNotificationPacket.username);
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
    printMessage(chatMessagePacket);
}

void MainWindow::printMessage(const ChatMessagePacket &messagePacket) {
    m_application->addChatMessage(messagePacket.senderName + ":  " + messagePacket.message);
    ui->chatbox->scrollToBottom();
}

void MainWindow::printMessage(const LoginSuccessPacket &loginSuccessPacket) {
    m_application->addChatMessage(loginSuccessPacket.username + ",  " + loginSuccessPacket.welcomeMsg);
    ui->chatbox->scrollToBottom();
}

void MainWindow::disableAllBtns()
{
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);
}

void MainWindow::setConnectedBtnStates()
{
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(true);
    ui->btnSend->setEnabled(true);
}

void MainWindow::setDisconnectedBtnStates()
{
    ui->btnConnect->setEnabled(true);
    ui->btnDisconnect->setEnabled(false);
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


