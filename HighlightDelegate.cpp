#include "HighlightDelegate.h"

#include <QBrush>
#include <QPainter>

HighlightDelegate::HighlightDelegate(QObject *parent) :
    QItemDelegate(parent)
{
	qDebug("HighlightDelegate c'tor");
}

HighlightDelegate::~HighlightDelegate()
{
	qDebug("HighlightDelegate d'tor");
}

QSize HighlightDelegate::sizeHint(
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    return QSize(150,50);
}

void HighlightDelegate::paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    QString data = index.model()->data(index, Qt::DisplayRole).toString();
    if ( data.isEmpty() )
        return;

    if ( index.column() == 0 &&
         index.model()->rowCount()-1 == index.row() )
    {
        QBrush brush(Qt::SolidPattern);
        brush.setColor( Qt::red );
        painter->fillRect( option.rect, brush );
    }
    drawDisplay( painter, option, option.rect, data );
}
