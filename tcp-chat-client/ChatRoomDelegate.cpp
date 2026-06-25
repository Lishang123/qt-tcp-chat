#include "ChatRoomDelegate.hpp"

#include <QApplication>

void ChatRoomDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    if (index.data(OfflineRole).toBool())
    {
        //all bits set except the 'State_Enabled' bit.
        opt.state &= ~QStyle::State_Enabled;
        // reverse: set this flag
        // opt.state |= QStyle::State_Enabled;
    }
    // Draw the item
    QApplication::style()->drawControl(
        QStyle::CE_ItemViewItem,
        &opt,
        painter);

    // Check unread status
    bool unread = index.data(UnreadRole).toBool();
    if (!unread)
        return;
    paintUnreadBadge(painter, option, index);
}

void ChatRoomDelegate::paintUnreadBadge(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {

    qInfo() << Q_FUNC_INFO;

    // make a copy of the style option to modify
    QStyleOptionViewItem opt(option);
    // read styling data from the model item and stores it in opt
    initStyleOption(&opt, index);
    // Get the rectangle for icon (decoration) of this item
    QRect iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt);

    int radius = iconRect.width() / 5;
    // center of the eclipse: bottom right
    QPoint center(iconRect.right() - radius, iconRect.bottom() - radius);

    /*
     * `QPainter` has a current state (pen, brush, font, transformations, etc.).
     * `save()` stores the current state so that any changes you make won't affect the rest of the painting.
     */
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::NoPen); // no boarder Qt::black

    // Draw a red dot in the bottom-right corner of the icon with horizontal, and vertical radius
    painter->drawEllipse(center, radius, radius);
    painter->restore();
}



void ChatRoomDelegate::paintGreyScale(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {

    // Check disabled status
    qInfo() << Q_FUNC_INFO;

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    // Obtain the normal icon
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

    // Obtain the pixmap at the appropriate size
    QPixmap pixmap = icon.pixmap(opt.decorationSize);

    // Convert to grayscale
    QImage grayImage = pixmap.toImage().convertToFormat(QImage::Format_Grayscale8);

    // Replace the icon in the style option
    opt.icon = QIcon(QPixmap::fromImage(grayImage));

    // Draw the item
    // FIXME: it doesn't work since paint calls initStyleOption again
    QStyledItemDelegate::paint(painter, opt, index);
}

