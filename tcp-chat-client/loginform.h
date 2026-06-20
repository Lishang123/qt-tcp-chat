#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QInputDialog>
#include "Application.hpp"

namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(Application* application, QWidget *parent=nullptr);
    ~LoginForm();
    

private slots:
    void on_btnConnect_clicked();
    void onClientConnected();
    void onClientLoggedIn(const LoginSuccessPacket& loginSuccessPacket);
    void onError(const QString &errorMessage);

private:
    void requestLoginInfo();

    Ui::LoginForm *ui;
    Application *m_application;
};

#endif // LOGINFORM_H
