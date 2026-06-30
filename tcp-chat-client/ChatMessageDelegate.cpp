//
// Created by Houcai Li on 29.06.26.
//

#include "ChatMessageDelegate.hpp"


QSize ChatMessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

    constexpr int messagePadding = 10;
    constexpr int margin = 12;

    const int maxBubbleWidth = option.rect.width() * 0.7;

    QTextDocument doc;
    doc.setDefaultFont(option.font);
    doc.setPlainText(index.data(ChatModel::MsgRole).toString());
    //doc.setTextWidth(maxBubbleWidth - 2 * messagePadding);
    qreal idealWidth = doc.idealWidth();
    qreal contentWidth = std::min(idealWidth, static_cast<qreal>(maxBubbleWidth - 2 * messagePadding));
    doc.setTextWidth(contentWidth);

    const QSizeF textSize = doc.size();

    return QSize(
        option.rect.width(),
        textSize.height() + 2 * messagePadding + 2 * margin);
}

void ChatMessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    constexpr int padding = 10;
    constexpr int margin = 12;
    const int maxBubbleWidth = option.rect.width() * 0.7f;
    QString message = index.data(ChatModel::MsgRole).toString();
    QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setDefaultFont(option.font);
    doc.setPlainText(message);

    // this approach is broken: it computes width using QFrontMetrics and QTextDocument for the height
    // and these class don't compute the layout identically.
    // QFontMetrics fm(option.font);
    // // Get the width of the message as a line
    // int naturalWidth = fm.horizontalAdvance(message);
    // // content width is the maximum width of the text in the chat bubble
    // int contentWidth = std::min( naturalWidth, maxBubbleWidth - 2 * padding);

    const QString timestamp = index.data(ChatModel::TimestampRole).toDateTime().toString("HH:mm");
    QFont timeFont = option.font;
    timeFont.setPointSizeF(timeFont.pointSizeF() * 0.85);
    QFontMetrics timeFm(timeFont);
    QSize timeSize = timeFm.size(Qt::TextSingleLine, timestamp);

    // set the width of the bubble
    qreal idealWidth = doc.idealWidth();
    qreal contentWidth = std::min(idealWidth, static_cast<qreal>(maxBubbleWidth - 2 * padding));
    doc.setTextWidth(contentWidth);

    QSizeF textSize = doc.size();
    int bubbleWidth = timeSize.width() > contentWidth ?  contentWidth + timeSize.width() + padding: contentWidth + 2 * padding;
    int bubbleHeight = textSize.height() + timeSize.height() + 2 *  padding - 2;

    const bool outgoing = index.data(ChatModel::OutGoingRole).toBool();
    qDebug() << "message:" << message;
    qDebug() << "contentWidth:" << contentWidth;
    qDebug() << "textSize:" << textSize;
    qDebug() << "option.rect.width()" << option.rect.width();
    qDebug() << "maxBubbleWidth" << maxBubbleWidth;

    // draw the bubble (rectangle)
    QColor color;
    QRect bubbleRect;
    if (outgoing) {
        // Right aligned
        bubbleRect = QRect(
            option.rect.right() - bubbleWidth - margin,
            option.rect.top() + margin,
            bubbleWidth,
            bubbleHeight);
        color = QColor(220, 248, 198);
    } else {
        // Left aligned
        bubbleRect = QRect(
            option.rect.left() + margin,
            option.rect.top() + margin,
            bubbleWidth,
            bubbleHeight);
        color = QColor(Qt::white);
    }

    painter->save();
    painter->setBrush(color);
    painter->setPen(Qt::lightGray);
    painter->drawRoundedRect(bubbleRect, 10, 10);
    painter->restore();

    // draw the message inside the bubble
    painter->save();
    painter->setPen(Qt::black);
    painter->translate(bubbleRect.topLeft() + QPoint(padding, padding));
    doc.drawContents(painter);
    painter->restore();

    // draw timestamp
    painter->save();
    painter->setFont(timeFont);
    painter->setPen(Qt::gray);

    QPoint timePos(
        bubbleRect.right() - 5 - timeSize.width(),
        bubbleRect.bottom() - 5);

    painter->drawText(timePos, timestamp);
    painter->restore();

    // draw sender icon

    //QStyledItemDelegate::paint(painter, option, index);
}

QSizeF ChatMessageDelegate::layoutText(const QString &text, const QFont &font, qreal maxWidth) {
    QTextLayout layout(text, font);

    layout.beginLayout();

    qreal width = 0;
    qreal height = 0;

    while (true) {
        QTextLine line = layout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(maxWidth);

        line.setPosition(QPointF(0, height));

        width = std::max(width, line.naturalTextWidth());
        height += line.height();
    }

    layout.endLayout();

    return QSizeF(width, height);
}
