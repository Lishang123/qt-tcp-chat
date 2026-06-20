#include "loginform.h"


#include "ui_loginform.h"

LoginForm::LoginForm(Application* application, QWidget *parent)
    :m_application(application),
    QDialog(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    connect(&m_application->getClient(), &Client::connected, this, &LoginForm::onClientConnected);
    connect(&m_application->getClient(), &Client::loggedIn, this, &LoginForm::onClientLoggedIn);
    connect(&m_application->getClient(), &Client::errorOccured, this, &LoginForm::onError);

}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::onClientConnected() {
    qInfo() << Q_FUNC_INFO;
    requestLoginInfo();
}

void LoginForm::onClientLoggedIn(const LoginSuccessPacket &loginSuccessPacket) {
    Q_UNUSED(loginSuccessPacket);
    accept();
}

void LoginForm::on_btnConnect_clicked()
{
    qInfo() << Q_FUNC_INFO << "connect clicked";
    // cast the port to quint16 for connectToHost call
    // TCP and UDP port numbers are defined by the Internet protocol as:
    // 0 - 65535, which is an unsigned 16-bit integer.
    quint16 port = static_cast<quint16>(ui->spboxPort->value());
    //disable all buttons so that the user doesn't click anything in between
    ui->btnConnect->setDisabled(true);
    m_application->connectToServer(ui->textServer->text(), port);
}


void LoginForm::onError(const QString &errorMessage) {
    ui->btnConnect->setDisabled(false);
    QMessageBox::critical(this, "Error", errorMessage);
}

void LoginForm::requestLoginInfo() {
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

