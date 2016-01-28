#ifndef POINTMODEL_H
#define POINTMODEL_H

#include <QStandardItemModel>
#include <QVector>
#include <QString>

class QStringList;

class PointModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PointModel(QObject *parent = 0);
    ~PointModel();
    
    void setSpecs( const QStringList& );
    QVector<QString> dataToString() const;

protected:
    void initModel();
    void prepareNextPoint();

signals:
    void pointAdded( const QVector<double>&, const QString& );
    void pointsCleared();
    
public slots:
    void addPoint( const QVector<double>& );
    void clearPoints();

private:
    int iCurrentPoint;

    QStringList *labelList;
};

#endif // POINTMODEL_H
