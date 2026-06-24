//
// Created by Houcai Li on 24.06.26.
//

#include "ChatRoomDelegate.hpp"

#include <QApplication>

void ChatRoomDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // Draw the item normally
    QStyledItemDelegate::paint(painter, option, index);

    // Check unread status
    bool unread = index.data(UnreadRole).toBool();
    if (!unread)
        return;

    qInfo() << Q_FUNC_INFO << ", printing unread item";

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
