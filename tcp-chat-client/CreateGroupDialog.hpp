#ifndef QT_TCP_CHAT_CREATEGROUPDIALOG_HPP
#define QT_TCP_CHAT_CREATEGROUPDIALOG_HPP

#include <QDialog>
#include <QList>
#include <QUuid>

#include "Application.hpp"

class QDialogButtonBox;
class QLineEdit;
class QListWidget;

class CreateGroupDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateGroupDialog(const QList<Application::Contact> &contacts, QWidget *parent = nullptr);

    QString groupName() const;
    QList<QUuid> selectedUserIds() const;

private slots:
    void updateAcceptButton();

private:
    QLineEdit *m_groupNameEdit;
    QListWidget *m_contactList;
    QDialogButtonBox *m_buttonBox;
};

#endif // QT_TCP_CHAT_CREATEGROUPDIALOG_HPP
