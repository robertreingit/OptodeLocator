#ifndef POINTCANVAS_H
#define POINTCANVAS_H

#include <DefsHeader.h>

#include <QVTKWidget.h>
#include <QString>
#include <QHash>

class vtkActor;
class vtkFollower;
class vtkRenderer;
class PointModel;

//////////////////////////////////////////////////////////////////////////
/// Helper struct
//////////////////////////////////////////////////////////////////////////
struct Location
{
    vtkActor *point;
    vtkFollower *label;
};

//////////////////////////////////////////////////////////////////////////
/// Canvas to draw points in 3D.
/// Uses 
class PointCanvas : public QVTKWidget
{
    Q_OBJECT

public:
    explicit PointCanvas(QWidget *parent = 0);
    ~PointCanvas();
    
    void setModel( PointModel* );

signals:
    
public slots: 
    void clearPoints();
    void addPoint( const QVector<double>& position, const QString& label );
    void deletePoint( const QString& key );

private:
    QHash<QString,Location> actorHash;
    vtkRenderer *m_ren;
    PointModel *model;
    
};

#endif // POINTCANVAS_H
