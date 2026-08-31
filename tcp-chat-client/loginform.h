#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QInputDialog>
#include "Application.hpp"

namespace Ui {
class LoginForm;
}

/**
 * @brief Dialog that connects to the server and submits login information.
 */
class LoginForm : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Creates the login dialog bound to the client application facade.
     */
    explicit LoginForm(Application* application, QWidget *parent=nullptr);

    /**
     * @brief Destroys the generated UI.
     */
    ~LoginForm();
    

private slots:
    /**
     * @brief Starts the server connection flow.
     */
    void on_btnConnect_clicked();

    /**
     * @brief Requests credentials once the socket is connected.
     */
    void onClientConnected();

    /**
     * @brief Completes login and accepts the dialog.
     */
    void onClientLoggedIn(const LoginSuccessPacket& loginSuccessPacket);

    /**
     * @brief Displays a connection or login error.
     */
    void onError(const QString &errorMessage);

private:
    /**
     * @brief Collects login information from the UI and sends it.
     */
    bool requestLoginInfo();

    Ui::LoginForm *ui;
    Application *m_application;
};

#endif // LOGINFORM_H
