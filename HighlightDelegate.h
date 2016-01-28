#ifndef HIGHLIGHTDELEGATE_H
#define HIGHLIGHTDELEGATE_H

#include <QItemDelegate>
#include <QSize>

class HighlightDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit HighlightDelegate(QObject *parent = 0);
    ~HighlightDelegate();
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
};

#endif // HIGHLIGHTDELEGATE_H
