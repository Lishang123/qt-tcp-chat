#ifndef QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP
#define QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextDocument>
#include <QTextLayout>
#include "ChatModel.hpp"

/**
 * @brief Delegate that paints chat messages, bubbles, and separators.
 */
class ChatMessageDelegate : public QStyledItemDelegate {

    using QStyledItemDelegate::QStyledItemDelegate;

    /**
     * @brief Cached measurements for one message bubble.
     */
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
    /**
     * @brief Returns the required row size for a chat item.
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /**
     * @brief Paints the chat item for the given model index.
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /**
     * @brief Paints a message item.
     */
    void drawMessage(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /**
     * @brief Paints a date separator item.
     */
    void drawDateSeparator(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /**
     * @brief Measures wrapped text for the given font and maximum width.
     */
    QSizeF layoutText(const QString &text, const QFont &font, qreal maxWidth) ;

    /**
     * @brief Controls whether message bubbles include sender names.
     */
    void setShowSender(bool showSender) {
        m_showSender = showSender;
    }

private:
    /**
     * @brief Calculates text and bubble geometry for a message item.
     */
    BubbleLayout calculateBubbleLayout(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /**
     * @brief Creates a QTextDocument configured for message bubble rendering.
     */
    QTextDocument createTextDocument(const QString &message, const QFont font, qreal maxBubbleWidth, int padding) const;
    bool m_showSender = false;
};



#endif //QT_TCP_CHAT_CHATMESSAGEDELEGATE_HPP
