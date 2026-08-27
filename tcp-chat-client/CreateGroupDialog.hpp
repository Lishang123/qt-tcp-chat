#ifndef QT_TCP_CHAT_CREATEGROUPDIALOG_HPP
#define QT_TCP_CHAT_CREATEGROUPDIALOG_HPP

#include <QDialog>
#include <QList>
#include <QUuid>

#include "Application.hpp"

class QDialogButtonBox;
class QLineEdit;
class QListWidget;

/**
 * @brief Dialog for naming a chat group and selecting its members.
 */
class CreateGroupDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Builds the dialog from the available contact list.
     */
    explicit CreateGroupDialog(const QList<Application::Contact> &contacts, QWidget *parent = nullptr);

    /**
     * @brief Returns the trimmed group name entered by the user.
     */
    QString groupName() const;

    /**
     * @brief Returns the ids of checked contacts.
     */
    QList<QUuid> selectedUserIds() const;

private slots:
    /**
     * @brief Enables the accept button only when the form is valid.
     */
    void updateAcceptButton();

private:
    QLineEdit *m_groupNameEdit;
    QListWidget *m_contactList;
    QDialogButtonBox *m_buttonBox;
};

#endif // QT_TCP_CHAT_CREATEGROUPDIALOG_HPP
