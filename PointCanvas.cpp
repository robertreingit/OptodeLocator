#include "PointCanvas.h"

#include "pointmodel.h"

#include <QString>
#include <QMessageBox>
#include <QTime>

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextMapper.h>
#include <vtkScaledTextActor.h>
#include <vtkTextProperty.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>

#define VTK_CREATE(type,name) \
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

PointCanvas::PointCanvas(QWidget *parent) :
    QVTKWidget(parent), model(0)
{
    m_ren = vtkRenderer::New();
    GetRenderWindow()->AddRenderer( m_ren );
	qDebug("PointCanvas::c'tor");
}

PointCanvas::~PointCanvas()
{
    QHash<QString,Location>::iterator iter = actorHash.begin();

    while( iter != actorHash.end() ) {
        iter.value().point->Delete();
        if ( iter.value().label ) iter.value().label->Delete();
        iter++;
    }

    m_ren->Delete();
	qDebug("PointCanvas::d'tor");
}

void PointCanvas::addPoint(const QVector<double>& position, const QString& label )
{
    // Storage object
    Location loc;

    // Create point
    VTK_CREATE(vtkSphereSource,point);
    point->SetCenter( position[0], position[1], position[2] );
    point->SetThetaResolution( 100 );
    point->SetPhiResolution( 100 );
    VTK_CREATE(vtkPolyDataMapper,mapper);
    mapper->SetInputConnection( point->GetOutputPort() );
    loc.point = vtkActor::New();
    loc.point->SetMapper( mapper );
    if ( label.isEmpty() )
        loc.point->GetProperty()->SetColor( 0.0, 0.0, 1.0 );
    else
        if ( label.contains( QString("D")) )
            loc.point->GetProperty()->SetColor( 0.0, 1.0, 0.0 );
        else if (label.contains( QString("S")) )
            loc.point->GetProperty()->SetColor( 1.0, 0.0, 0.0 );
            else
            loc.point->GetProperty()->SetColor( 0.0, 0.0, 1.0 );
    m_ren->AddActor( loc.point );


    VTK_CREATE( vtkVectorText, text );
    if ( label.isEmpty() ) {
        text->SetText( QTime::currentTime().toString( "hmsz").toLocal8Bit() );
    }
    else
        text->SetText( label.toLocal8Bit() );
    VTK_CREATE(vtkPolyDataMapper, textMapper );
    textMapper->SetInputConnection( text->GetOutputPort() );

    loc.label = vtkFollower::New();
    loc.label->SetMapper( textMapper );
    loc.label->SetScale( 0.8, 0.8, 0.8 );
    loc.label->GetProperty()->SetColor( 1.0, 1.0, 1.0 );
    loc.label->SetCamera( m_ren->GetActiveCamera() );
    loc.label->SetPosition( position[0], position[1] + .5, position[2] );
    m_ren->AddActor( loc.label );

    m_ren->ResetCamera();
    GetRenderWindow()->GetInteractor()->Render();

    actorHash.insert( label, loc );
}

void PointCanvas::deletePoint( const QString& key )
{
    if ( !actorHash.isEmpty() ) {

        QHash<QString,Location>::iterator iter = actorHash.find( key );
        if ( iter == actorHash.end() )
            return;
        Location loc = iter.value();

        m_ren->RemoveActor( loc.point );
        loc.point->Delete();
        if ( loc.label ) {
            m_ren->RemoveActor( loc.label );
            loc.label->Delete();
        }
        actorHash.erase( iter );

        m_ren->ResetCamera();
        GetRenderWindow()->GetInteractor()->Render();
    }
}

void PointCanvas::clearPoints()
{

    if ( !actorHash.isEmpty() ) {

        QHash<QString,Location>::iterator iter = actorHash.begin();
        while( iter != actorHash.end() ) {


            m_ren->RemoveActor( iter.value().point );
            iter.value().point->Delete();

            if ( iter.value().label ) {
                m_ren->RemoveActor( iter.value().label );
                iter.value().label->Delete();
            }
            iter++;
        }

        actorHash.clear();

        m_ren->ResetCamera();
        GetRenderWindow()->GetInteractor()->Render();
    }
}

void PointCanvas::setModel(PointModel* _model )
{
    model = _model;
    connect( model, SIGNAL(pointAdded(const QVector<double>&,const QString&)),
             this, SLOT(addPoint(const QVector<double>&, const QString&)) );
    connect( model, SIGNAL(pointsCleared()),
             this, SLOT(clearPoints()) );
}
