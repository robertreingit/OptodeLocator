#ifndef LIBERTYCONTROLLER_H
#define LIBERTYCONTROLLER_H

#include <QWidget>

#include <QVector>
#include <QString>

class CPDIdev;

class LibertyController : public QWidget
{
    Q_OBJECT
public:
    explicit LibertyController(QWidget *parent = 0);
    ~LibertyController();
    
signals:
    void connected(bool);
    void configured(bool);
    void started(bool);
    void resetted();
    void errorMsg( QString );

    void newPoint(const QVector<double>& );
    
public slots:
    void connect();
    void disconnect();
    void configure();
    void start();
    void stop();
    void reset();

protected:
	bool winEvent( MSG*, long*);

private:
	void parseData();
	QString msgToQString();

private:

    CPDIdev* m_CPDI;

	bool m_captureReady;
	bool m_configured;
    
};

#endif // LIBERTYCONTROLLER_H
