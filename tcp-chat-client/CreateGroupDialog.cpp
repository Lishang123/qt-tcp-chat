#include "CreateGroupDialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

CreateGroupDialog::CreateGroupDialog(const QList<Application::Contact> &contacts, QWidget *parent)
    : QDialog(parent),
      m_groupNameEdit(new QLineEdit(this)),
      m_contactList(new QListWidget(this)),
      m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this)) {
    setWindowTitle(tr("Create Group"));

    m_groupNameEdit->setObjectName("groupNameEdit");
    m_groupNameEdit->setPlaceholderText(tr("Group name"));

    m_contactList->setObjectName("contactList");
    m_contactList->setSelectionMode(QAbstractItemView::NoSelection);
    for (const auto &contact : contacts) {
        auto *item = new QListWidgetItem(contact.username, m_contactList);
        item->setData(Qt::UserRole, contact.userId);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        if (!contact.isOnline) {
            item->setText(contact.username + tr(" [Offline]"));
        }
    }
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Group name"), this));
    layout->addWidget(m_groupNameEdit);
    layout->addWidget(new QLabel(tr("Members"), this));
    layout->addWidget(m_contactList);
    layout->addWidget(m_buttonBox);

    connect(m_groupNameEdit, &QLineEdit::textChanged, this, &CreateGroupDialog::updateAcceptButton);
    connect(m_contactList, &QListWidget::itemChanged, this, &CreateGroupDialog::updateAcceptButton);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &CreateGroupDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &CreateGroupDialog::reject);

    updateAcceptButton();
}

QString CreateGroupDialog::groupName() const {
    return m_groupNameEdit->text().trimmed();
}

QList<QUuid> CreateGroupDialog::selectedUserIds() const {
    QList<QUuid> userIds;
    for (int row = 0; row < m_contactList->count(); ++row) {
        const QListWidgetItem *item = m_contactList->item(row);
        if (item->checkState() == Qt::Checked) {
            userIds.append(item->data(Qt::UserRole).toUuid());
        }
    }
    return userIds;
}

void CreateGroupDialog::updateAcceptButton() {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!groupName().isEmpty() && !selectedUserIds().isEmpty());
}
