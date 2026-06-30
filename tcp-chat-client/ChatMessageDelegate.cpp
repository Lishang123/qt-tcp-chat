//
// Created by Houcai Li on 29.06.26.
//

#include "ChatMessageDelegate.hpp"


QSize ChatMessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

    constexpr int messagePadding = 10;
    constexpr int margin = 12;
    constexpr int dateItemPadding = 4;

    // for date separators
    if (index.data(ChatModel::ItemTypeRole) == ChatItemType::DateSep)
    {
        QFont font = option.font;
        font.setPointSizeF(font.pointSizeF() * 0.9f);
        QFontMetrics fm(font);
        return QSize(option.rect.width(), fm.height() + 2* dateItemPadding);
    }

    // size hint for chat messages
    auto bubbleLayout = calculateBubbleLayout(option, index);

    QTextDocument doc;
    doc.setDefaultFont(bubbleLayout.messageFont);
    doc.setPlainText(index.data(ChatModel::MsgRole).toString());
    doc.setTextWidth(bubbleLayout.contentWidth);

    return QSize(
        option.rect.width(),
        bubbleLayout.bubbleHeight + messagePadding + margin);
}

void ChatMessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.data(ChatModel::ItemTypeRole) == ChatItemType::Message)
        drawMessage(painter, option, index);
    if (index.data(ChatModel::ItemTypeRole) == ChatItemType::DateSep)
        drawDateSeparator(painter, option, index);
}

void ChatMessageDelegate::drawMessage(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    constexpr int verticalPaddingSender = 20;
    constexpr int verticalPaddingMessage = 10;
    constexpr int horizontalPadding = 5;
    constexpr int margin = 12;

    // this approach is broken: it computes width using QFrontMetrics and QTextDocument for the height
    // and these class don't compute the layout identically.
    // QFontMetrics fm(option.font);
    // // Get the width of the message as a line
    // int naturalWidth = fm.horizontalAdvance(message);
    // // content width is the maximum width of the text in the chat bubble
    // int contentWidth = std::min( naturalWidth, maxBubbleWidth - 2 * padding);

    auto bubbleLayout = calculateBubbleLayout(option, index);
    QString message = index.data(ChatModel::MsgRole).toString();

    QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setDefaultFont(bubbleLayout.messageFont);
    doc.setPlainText(message);
    doc.setTextWidth(bubbleLayout.contentWidth);

    const bool outgoing = index.data(ChatModel::OutGoingRole).toBool();

    // draw the bubble (rectangle)
    QColor color;
    QRect bubbleRect;
    if (outgoing) {
        // Right aligned
        bubbleRect = QRect(
            option.rect.right() - bubbleLayout.bubbleWidth - margin,
            option.rect.top() + margin,
            bubbleLayout.bubbleWidth,
            bubbleLayout.bubbleHeight);
        color = QColor(220, 248, 198);
    } else {
        // Left aligned
        bubbleRect = QRect(
            option.rect.left() + margin,
            option.rect.top() + margin,
            bubbleLayout.bubbleWidth,
            bubbleLayout.bubbleHeight);
        color = QColor(Qt::white);
    }

    painter->save();
    painter->setBrush(color);
    painter->setPen(Qt::lightGray);
    painter->drawRoundedRect(bubbleRect, 10, 10);
    painter->restore();

    // draw senderName
    if (m_showSender) {
        const QString senderName = index.data(ChatModel::SenderRole).toString();
        QPoint senderPos(
        bubbleRect.left() + horizontalPadding,
        bubbleRect.top() + verticalPaddingSender);
        painter->save();
        painter->setFont(bubbleLayout.senderFont);
        if (outgoing) {
            painter->setPen(Qt::darkGreen);
        }
        else painter->setPen(Qt::darkMagenta);
        painter->drawText(senderPos, senderName);
        painter->restore();
    }

    // draw the message inside the bubble
    painter->save();
    painter->setPen(Qt::black);
    painter->translate(bubbleRect.topLeft() + QPoint(horizontalPadding, verticalPaddingMessage+bubbleLayout.senderSize.height()));
    doc.drawContents(painter);
    painter->restore();

    // draw timestamp
    painter->save();
    painter->setFont(bubbleLayout.timeFont);
    painter->setPen(Qt::gray);

    QPoint timePos(
        bubbleRect.right() - 5 - bubbleLayout.timeSize.width(),
        bubbleRect.bottom() - 5);

    const QString timestamp = index.data(ChatModel::TimestampRole).toDateTime().toString("HH:mm");
    painter->drawText(timePos, timestamp);
    painter->restore();

    // draw sender icon

    //QStyledItemDelegate::paint(painter, option, index);
}

void ChatMessageDelegate::drawDateSeparator(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    constexpr int padding = 10;
    const QString date = index.data(ChatModel::TextRole).toString();
    qInfo() << Q_FUNC_INFO << "date: " << date;
    QFont dateFont = option.font;
    dateFont.setPointSizeF(dateFont.pointSizeF() * 0.9f);
    QFontMetrics timeFm(dateFont);
    QSize dateSize = timeFm.size(Qt::TextSingleLine, date);

    QRect dateRect(
        (option.rect.width() - dateSize.width()) / 2,
        option.rect.top() + 4,
        dateSize.width() + 1.5f * padding,
        dateSize.height() + 1 * padding);

    painter->save();
    painter->setBrush(Qt::transparent);
    painter->setPen(Qt::lightGray);
    painter->drawRoundedRect(dateRect, 10, 10);
    painter->restore();

    // draw the message inside the bubble
    QPoint datePos(
            dateRect.left() +  0.5f * padding ,
            dateRect.bottom() - 0.5f * padding);
    painter->save();
    painter->setPen(Qt::darkGray);
    painter->drawText( datePos ,date);
    painter->restore();
}

ChatMessageDelegate::BubbleLayout ChatMessageDelegate::calculateBubbleLayout(const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    BubbleLayout res;

    // calculate time size
    const QString timestamp = index.data(ChatModel::TimestampRole).toDateTime().toString("HH:mm");
    res.timeFont = option.font;
    res.timeFont.setPointSizeF(res.timeFont.pointSizeF() * 0.85);
    QFontMetrics timeFm(res.timeFont);
    res.timeSize = timeFm.size(Qt::TextSingleLine, timestamp);

    // calculate sender size
    if (m_showSender) {
        const QString senderName = index.data(ChatModel::SenderRole).toString();
        res.senderFont = option.font;
        res.senderFont.setBold(true);
        QFontMetrics senderFm(res.senderFont);
        res.senderSize = senderFm.size(Qt::TextSingleLine, senderName);
    }
    else {
        res.senderSize = QSize(0, 0);
    }

    // calculate message size

    constexpr int padding = 10;
    constexpr int margin = 12;

    const int maxBubbleWidth = option.rect.width() * 0.7f;
    QString message = index.data(ChatModel::MsgRole).toString();

    res.messageFont = option.font;
    QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setDefaultFont(res.messageFont);
    doc.setPlainText(message);
    // set the width of the bubble
    qreal idealWidth = doc.idealWidth();
    res.contentWidth = std::min(idealWidth, static_cast<qreal>(maxBubbleWidth - 2 * padding));
    // doc.setTextWidth(contentWidth);
    auto messageSize = doc.size();

    qDebug() << "message:" << message;
    qDebug() << "contentWidth:" << res.contentWidth;
    qDebug() << "textSize:" << messageSize;
    qDebug() << "option.rect.width()" << option.rect.width();
    qDebug() << "maxBubbleWidth" << maxBubbleWidth;

    //calculate bubble size
    if (res.timeSize.width() > res.contentWidth || res.senderSize.width() > res.contentWidth) {
        res.bubbleWidth = std::max(res.timeSize.width(), res.senderSize.width()) + padding;
    }
    else {
        res.bubbleWidth = res.contentWidth + 2*padding;
    }
    res.bubbleHeight = messageSize.height() + res.timeSize.height() + res.senderSize.height() + 2 *  padding - 2;
    return res;
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
