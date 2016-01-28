#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTableView;
class PointModel;
class LibertyController;
class QAction;
class QMenu;
class PointCanvas;
class QToolBar;
class HighlightDelegate;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void createNewPoint();
    void loadSpecs();
    void saveData();
	void errorMsg( QString );

	void connected(bool);

protected:
    void createMenus();
    void createActions();
    void createToolbars();
	void connectObjects();

private:
    void populateTable();

private:
    QTableView *view;
    PointModel *model;
    PointCanvas *vtkview;
    LibertyController *centralwidget;
	HighlightDelegate *highlightdel;

    QMenu *fileMenu;
    QAction *loadSpecsAct;
    QAction *saveDataAct;
    QAction *quitAct;

    QAction *addPointAct;
    QAction *clearPtsAct;

    QMenu *libertyMenu;
    QAction *connectAct;
    QAction *disconnectAct;
    QAction *configAct;
    QAction *startAct;
    QAction *stopAct;
    QAction *resetAct;

    QToolBar *libertyToolbar;
};

#endif // MAINWINDOW_H
