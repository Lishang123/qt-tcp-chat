#ifndef QT_TCP_CHAT_CHATROOMDELEGATE_HPP
#define QT_TCP_CHAT_CHATROOMDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QPainter>

enum Roles {
    UserIdRole = Qt::UserRole + 1,
    RoomIdRole = Qt::UserRole + 2,
    StatusRole = Qt::UserRole + 3,
    UnreadRole = Qt::UserRole + 4,
};

enum ItemType {
    Category,
    Room,
};


class ChatRoomDelegate : public QStyledItemDelegate
{

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};




#endif //QT_TCP_CHAT_CHATROOMDELEGATE_HPP
