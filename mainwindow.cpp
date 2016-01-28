#include "mainwindow.h"

#include "pointmodel.h"
#include "PointCanvas.h"
#include "HighlightDelegate.h"
#include "LibertyController.h"

#include <QCoreApplication>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QStringList>
#include <QIcon>
#include <QToolBar>
#include <QMessageBox>
#include <QString>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    view = new QTableView();
    model = new PointModel();
    vtkview = new PointCanvas();
    vtkview->setModel( model );
	highlightdel = new HighlightDelegate();

    view->setModel( model );
    view->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
    view->verticalHeader()->setResizeMode( QHeaderView::Stretch );
    view->setSortingEnabled( false );
    view->setItemDelegateForColumn( 0, highlightdel );
    connect( model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             view, SLOT(resizeRowsToContents()) );
    connect( model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             view, SLOT(scrollToBottom()) );

    centralwidget = new LibertyController();
    QGridLayout *layout = new QGridLayout( centralwidget );
    layout->addWidget( view, 0, 0, 1, 2 );
    layout->addWidget( vtkview, 0, 2 );

    setCentralWidget( centralwidget );

    createActions();
    createMenus();
    createToolbars();
	connectObjects();

    resize(600,400);

	(void*)statusBar();
}

MainWindow::~MainWindow()
{
	qDebug("Cleaning up MainWindow.");
	delete highlightdel;
	delete model;
	qDebug("MainWindow::~MainWindow finished.");
}


void MainWindow::createActions()
{
    // File actions
    loadSpecsAct = new QAction( tr("Load Specs"), this );
    loadSpecsAct->setShortcut( tr("Ctrl+O") );
    connect( loadSpecsAct, SIGNAL(triggered()),
             this, SLOT(loadSpecs()) );

    saveDataAct = new QAction( tr("Save data"), this );
    saveDataAct->setShortcut( tr("Ctrl+S") );
    connect( saveDataAct, SIGNAL(triggered()),
             this, SLOT(saveData()) );

    quitAct = new QAction( tr("Exit"), this );
    quitAct->setShortcut( tr("Ctrl+Q") );
    connect( quitAct, SIGNAL(triggered()),
             qApp, SLOT(quit()) );

    addPointAct = new QAction( tr("Add Point"), this );
    addPointAct->setShortcut( tr("Ctrl+A") );
    connect( addPointAct, SIGNAL(triggered()),
             this, SLOT(createNewPoint()) );

    clearPtsAct = new QAction( tr("Clear Points"), this );
    clearPtsAct->setShortcut( tr("Ctrl+D") );
    connect( clearPtsAct, SIGNAL(triggered()),
             model, SLOT(clearPoints()) );

    // Liberty actions
    connectAct = new QAction( tr("Connect"), this );
    connectAct->setIcon( QIcon(":/ICON/connected.png"));
    connect( connectAct, SIGNAL(triggered()),
             centralwidget, SLOT(connect()) );

    disconnectAct = new QAction( tr("Disconnect"), this );
    disconnectAct->setIcon( QIcon(":/ICON/disconnected.png"));
    connect( disconnectAct, SIGNAL(triggered()),
             centralwidget, SLOT(disconnect()) );

    configAct = new QAction( tr("Prepare Liberty"), this );
    configAct->setIcon( QIcon(":/ICON/configure.png") );
	configAct->setEnabled( false );
    connect( configAct, SIGNAL(triggered()),
             centralwidget, SLOT(configure()) );

    startAct = new QAction( tr("Start"), this );
    startAct->setIcon( QIcon(":/ICON/start.png") );
	startAct->setEnabled( false );
    connect( startAct, SIGNAL(triggered()),
             centralwidget, SLOT(start()) );

    stopAct = new QAction( tr("Stop"), this );
    stopAct->setIcon( QIcon(":/ICON/stop.png") );
	stopAct->setEnabled( false );
    connect( stopAct, SIGNAL(triggered()),
             centralwidget, SLOT(stop()) );

    resetAct = new QAction( tr("Reset"), this );
    connect( resetAct, SIGNAL(triggered()),
             centralwidget, SLOT(reset()) );

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu( tr("File") );
    fileMenu->addAction( loadSpecsAct );
    fileMenu->addAction( saveDataAct );
    fileMenu->addAction( addPointAct );
    fileMenu->addAction( clearPtsAct );
    fileMenu->addAction( quitAct );

    libertyMenu = menuBar()->addMenu( tr("Liberty") );
    libertyMenu->addAction( connectAct );
    libertyMenu->addAction( configAct );
    libertyMenu->addAction( startAct );
    libertyMenu->addAction( stopAct );
    libertyMenu->addAction( disconnectAct );
    libertyMenu->addSeparator();
    libertyMenu->addAction( resetAct );
}

void MainWindow::createNewPoint()
{
    QVector<double> point;
    point << (qrand()%40)/sqrt(2.0) << (qrand()%20)/sqrt(2.0) << (qrand()%20)/sqrt(2.0);
    model->addPoint( point );

}

void MainWindow::loadSpecs()
{
    QString fileName, line;
    QStringList labels;
    fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open specs file."),
                "",
                "Text files (*.txt)");
    if ( !fileName.isEmpty() ) {
        QFile file(fileName);
        if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
            QTextStream in(&file);
            while ( !(line = in.readLine()).isNull() )
                labels << line;
        }
        if ( labels.count() > 0 )
            model->setSpecs( labels );
    }
}

void MainWindow::saveData()
{
    QVector<QString> data = model->dataToString();
    if ( data.isEmpty() )
        return;

    QString fileName;
    fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save Data"),
                "",
                "Text files (*.txt)");
    if ( !fileName.isEmpty() ) {
        QFile file(fileName);
        if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
            QTextStream out(&file);
            foreach( QString line, data )
                out << line  << endl;
        }
    }
}

void MainWindow::createToolbars()
{
    libertyToolbar = addToolBar( tr("Liberty") );
    libertyToolbar->addAction( connectAct );
    libertyToolbar->addAction( configAct );
    libertyToolbar->addAction( startAct );
    libertyToolbar->addAction( stopAct );
    libertyToolbar->addAction( disconnectAct );
}

void MainWindow::errorMsg( QString msg )
{
	statusBar()->showMessage( msg );
}

void MainWindow::connectObjects()
{
	connect( centralwidget, SIGNAL(errorMsg(QString)),
		this, SLOT(errorMsg(QString)));

	connect( centralwidget, SIGNAL(newPoint(const QVector<double>& )),
		model, SLOT(addPoint( const QVector<double>& )) );

	connect( centralwidget, SIGNAL(connected(bool)),
		this, SLOT(connected(bool)) );

	connect( centralwidget, SIGNAL(connected(bool)),
		configAct, SLOT(setEnabled(bool)) );

	connect( centralwidget, SIGNAL(configured(bool)),
		startAct, SLOT(setEnabled(bool)) );

	connect( centralwidget, SIGNAL(configured(bool)),
		stopAct, SLOT(setEnabled(bool)) );
}

void MainWindow::connected(bool signal)
{
	statusBar()->showMessage( tr("Connected to Liberty"), 3000 );
	//configAct->setEnabled( signal );
}
