#ifndef QT_TCP_CHAT_CHATROOMDELEGATE_HPP
#define QT_TCP_CHAT_CHATROOMDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QPainter>

/**
 * @brief Custom room-list roles consumed by the room delegate and application.
 */
enum Roles {
    UserIdRole = Qt::UserRole + 1,
    RoomIdRole = Qt::UserRole + 2,
    RoomTypeRole = Qt::UserRole + 3,
    UnreadRole = Qt::UserRole + 4,
    OfflineRole = Qt::UserRole + 5,
};


/**
 * @brief Delegate that paints room-list rows and unread badges.
 */
class ChatRoomDelegate : public QStyledItemDelegate
{

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    /**
     * @brief Paints a room-list item.
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    /**
     * @brief Paints the unread badge for an item when its role is set.
     */
    void paintUnreadBadge(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //void paintGreyScale(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};




#endif //QT_TCP_CHAT_CHATROOMDELEGATE_HPP
