#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //connect signals and slots
    connect(ui->textMsg, &QLineEdit::returnPressed, this, &MainWindow::on_btnSend_clicked);
    connect(&m_socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &MainWindow::OnReadyRead);
    //Qt5:
    //connect(&m_socket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&MainWindow::error);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &MainWindow::error);

    //enable buttons
    ui->btnConnect->setEnabled(true);
    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(true);

    //set string list model
    m_model.setStringList(m_list);
    ui->chatbox->setModel(&m_model);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnConnect_clicked()
{
    qInfo() << "connect clicked";
    //this should not happen
    if(m_socket.isOpen()) m_socket.close();

    // cast the port to quint16 for connectToHost call
    // TCP and UDP port numbers are defined by the Internet protocol as:
    // 0 - 65535, which is an unsigned 16-bit integer.
    quint16 port = static_cast<quint16>(ui->spboxPort->value());

    //disable all buttons so that the user doesn't click anything in between
    disableAllBtns();

    //asynchronous call to connect to the host
    m_socket.connectToHost(ui->textServer->text(), port);

    // check if connected, force synchronous waiting.
    // unnecessary since the socket will emit error if connection failed.
    // if(!m_socket.waitForConnected(3000)){
    //     on_btnDisconnect_clicked();
    //     QMessageBox::critical(this, "Error", "Cannot connect to the server!");
    //     return;
    // }
}


void MainWindow::on_btnDisconnect_clicked()
{
    qInfo() << "disconnect clicked";
    disableAllBtns();
    m_socket.disconnectFromHost();
}


void MainWindow::on_btnSend_clicked()
{
    qInfo() << "send clicked";
    sendMessage(ui->textMsg->text());
    ui->textMsg->clear();
}



void MainWindow::onConnected()
{
    qInfo() << "Connected!";
    setConnectedBtnStates();

    // TODO: implement authentication!
    sendLoginRequest(requestLoginInfo());

}



void MainWindow::onDisconnected()
{
    qInfo() << "Disconnected!";
    setDisconnectedBtnStates();
    setWindowTitle("Chat Client");
}



void MainWindow::OnReadyRead()
{
    qInfo() << "ready read!";
    QByteArray data = m_socket.readAll();
    QDataStream stream(&data, QIODevice::ReadOnly);
    PacketType packetType;
    stream >> packetType;
    qInfo() << "packet type:" << packetType;
    switch (packetType) {
        case PacketType::LoginFail: {
            LoginFailedPacket loginFailedPacket;
            stream >> loginFailedPacket;
            QMessageBox::critical(this, "Error", loginFailedPacket.errorMsg);
            break;
        }
        case PacketType::LoginSuccess: {
            setWindowTitle("Chat Client " + m_name);
            LoginSuccessPacket loginSuccessPacket;
            stream >> loginSuccessPacket;
            printMessage(loginSuccessPacket);
            break;
        }
        case PacketType::ChatMessage: {
            ChatMessagePacket messagePacket;
            stream >> messagePacket;
            printMessage(messagePacket);
            break;
        }
        default:
            qCritical() << Q_FUNC_INFO << "\tUnknown packet type:" << packetType;
            break;
    }
}

void MainWindow::printMessage(const ChatMessagePacket &messagePacket) {
    m_list.append((messagePacket.senderName + ":  " + messagePacket.message));
    m_model.setStringList(m_list);
    ui->chatbox->scrollToBottom();
}

void MainWindow::printMessage(const LoginSuccessPacket &loginSuccessPacket) {
    m_list.append((loginSuccessPacket.username + ",  " + loginSuccessPacket.welcomeMsg));
    m_model.setStringList(m_list);
    ui->chatbox->scrollToBottom();
}

void MainWindow::error(QAbstractSocket::SocketError error)
{
    //Ignore this error since we already handled it.
    if (error == QAbstractSocket::RemoteHostClosedError)
        return;

    setDisconnectedBtnStates();
    QString errorMsg = m_socket.errorString();
    QMessageBox::critical(this, "Error", errorMsg);
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


LoginRequestPacket MainWindow::requestLoginInfo() {

    while (true) {
        m_name = QInputDialog::getText(
            this, //parent widget
        "Name", //title
        "What is your name?",//label text inside of the dialog
        QLineEdit::EchoMode::Normal, //show typed text normally
        m_name // The pre-filled text inside the input field.
        );

        if (m_name.isEmpty()){
            QMessageBox::critical(this, "Error", "Please enter a valid name!");
            continue;
        }
        return LoginRequestPacket{m_name, 0, "password"};
    }
}


void MainWindow::sendLoginRequest(const LoginRequestPacket &loginRequestPacket) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << PacketType::LoginRequest;
    out << loginRequestPacket;
    if (!m_socket.write(data)) {
        qCritical() << Q_FUNC_INFO << "\tCannot send login reqeust:\t" << data << m_socket.errorString() ;
    }
}


void MainWindow::sendMessage(const QString &message) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << PacketType::ChatMessage;
    out << ChatMessagePacket{m_name, "", message};
    if (!m_socket.write(data)) {
        qCritical() << "cannot send message: " << data << m_socket.errorString();
    };
}


