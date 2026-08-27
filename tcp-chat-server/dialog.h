#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "Application.hpp"
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

/**
 * @brief Server control dialog for starting and stopping the chat server.
 */
class Dialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Creates the server control dialog.
     */
    explicit Dialog(QWidget *parent = nullptr);

    /**
     * @brief Destroys the generated UI.
     */
    ~Dialog() override;
    //void connectToController(Application& application);

private slots:

    /**
     * @brief Starts listening for client connections.
     */
    void on_btnStart_clicked();

    /**
     * @brief Stops the server and closes active client connections.
     */
    void on_btnStop_clicked();

private:

    /**
     * @brief Initializes UI state and signal connections.
     */
    void init();

    /**
     * @brief Updates UI state when the connected client count changes.
     */
    void clientChanged();

    /**
     * @brief Enables controls appropriate for a running server.
     */
    void enableBtnsStarted();

    /**
     * @brief Enables controls appropriate for a stopped server.
     */
    void enableBtnsStopped();

    Ui::Dialog *ui;
    Application m_application;
    //Server m_server;
};
#endif // DIALOG_H
