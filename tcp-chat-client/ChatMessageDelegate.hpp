#ifndef QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP
#define QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextDocument>
#include <QTextLayout>
#include "ChatModel.hpp"

class ChatMessageDelegate : public QStyledItemDelegate {

    using QStyledItemDelegate::QStyledItemDelegate;

    struct BubbleLayout
    {
        QFont senderFont;
        QFont timeFont;
        QFont messageFont;

        QSize senderSize;
        qreal contentWidth;
        QSize timeSize;


        int bubbleWidth;
        int bubbleHeight;
    };

public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void drawMessage(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawDateSeparator(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSizeF layoutText(const QString &text, const QFont &font, qreal maxWidth) ;

    void setShowSender(bool showSender) {
        m_showSender = showSender;
    }

private:
    BubbleLayout calculateBubbleLayout(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QTextDocument createTextDocument(const QString &message, const QFont font, qreal maxBubbleWidth, int padding) const;
    bool m_showSender = false;
};



#endif //QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP
