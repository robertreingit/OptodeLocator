#include "pointmodel.h"

#include <QStandardItem>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QList>

PointModel::PointModel(QObject *parent) :
    QStandardItemModel(parent), iCurrentPoint(-1)
{
    labelList = new QStringList();
    initModel();
	qDebug("PointModel::c'tor");
}

PointModel::~PointModel()
{
    delete labelList;
	qDebug("PointModel::d'tor");
}

void PointModel::addPoint(const QVector<double> & point)
{

    for (int i = 1; i < 4; ++i)
        setItem( iCurrentPoint, i, new QStandardItem( QString::number( point[i-1]) ) );

    emit pointAdded( point, item(iCurrentPoint,0)->data( Qt::DisplayRole).toString() );

    prepareNextPoint();

}


void PointModel::setSpecs(const QStringList & labels )
{
    *labelList = labels;
    initModel();
}

void PointModel::initModel()
{
    clear();
    emit pointsCleared();

    setColumnCount( 4 );
    QStringList header;
    header << "Label" << "X" << "Y" << "Z";
    setHorizontalHeaderLabels( header );
    iCurrentPoint = -1;
    prepareNextPoint();
}

void PointModel::prepareNextPoint()
{
    ++iCurrentPoint;
    QString currentLabel;
    qDebug("Item in list: %d", labelList->count() );
    qDebug("iCurrentPoint: %d", iCurrentPoint );
    if ( iCurrentPoint >= labelList->count() || labelList->isEmpty() )
        currentLabel = QString( "P%1" ).arg( iCurrentPoint );
    else
        currentLabel = labelList->at( iCurrentPoint );

    QStandardItem *newLabelItem = new QStandardItem(currentLabel);
    QFont font = newLabelItem->font();
    font.setPointSize( 30 );
    font.setBold( true );
    newLabelItem->setFont( font );

    setItem( iCurrentPoint, 0, newLabelItem );
}

QVector<QString> PointModel::dataToString() const
{
    QVector<QString> data;
    if ( rowCount() > 1 ) {     // As one item is always present
        for (int row = 0; row < rowCount()-1; ++row )
            data.push_back( QString( "%1\t%2\t%3\t%4")
                            .arg( item(row,0)->text() )
                            .arg( item(row,1)->text() )
                            .arg( item(row,2)->text() )
                            .arg( item(row,3)->text() ) );
    }
    // Returns an empty vector if no model data is found.
    return data;
}

void PointModel::clearPoints()
{
    initModel();
}
