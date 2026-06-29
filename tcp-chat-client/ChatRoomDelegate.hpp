#ifndef QT_TCP_CHAT_CHATROOMDELEGATE_HPP
#define QT_TCP_CHAT_CHATROOMDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QPainter>

enum Roles {
    UserIdRole = Qt::UserRole + 1,
    RoomIdRole = Qt::UserRole + 2,
    RoomTypeRole = Qt::UserRole + 3,
    UnreadRole = Qt::UserRole + 4,
    OfflineRole = Qt::UserRole + 5,
};


class ChatRoomDelegate : public QStyledItemDelegate
{

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    void paintUnreadBadge(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paintGreyScale(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};




#endif //QT_TCP_CHAT_CHATROOMDELEGATE_HPP
