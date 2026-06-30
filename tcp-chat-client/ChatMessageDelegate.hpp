#ifndef QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP
#define QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextDocument>
#include <QTextLayout>
#include "ChatModel.hpp"

class ChatMessageDelegate : public QStyledItemDelegate {

    using QStyledItemDelegate::QStyledItemDelegate;

public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void drawMessage(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawDateSeparator(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSizeF layoutText(const QString &text, const QFont &font, qreal maxWidth) ;
};



#endif //QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP
