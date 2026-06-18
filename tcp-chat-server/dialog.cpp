#include "dialog.h"
#include "./ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("TCP Chat Server");
    init();

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::init()
{
    connect(&m_server, &Server::clientChanged, this, &Dialog::clientChanged);
    enableBtnsStopped();
}

void Dialog::clientChanged()
{
    ui->lblStatus->setText(QString("%0 clients connected").arg(m_server.getClientsCount()));
}

void Dialog::enableBtnsStarted()
{
    ui->txtWelcomeMsg->setReadOnly(true);
    ui->sbPort->setEnabled(false);
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
}

void Dialog::enableBtnsStopped()
{
    ui->txtWelcomeMsg->setReadOnly(false);
    ui->sbPort->setEnabled(true);
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
}


void Dialog::on_btnStart_clicked()
{
    quint16 port = static_cast<quint16>(ui->sbPort->value());
    m_server.setWelcome_msg(ui->txtWelcomeMsg->text());
    if (!m_server.listen(QHostAddress::Any, port))
    {
        QMessageBox::critical(this, "Error", m_server.errorString());
        return;
    }
    ui->lblStatus->setText("Listening...");
    enableBtnsStarted();
}


void Dialog::on_btnStop_clicked()
{
    m_server.close();
    ui->lblStatus->setText("Closed");
    enableBtnsStopped();
}

