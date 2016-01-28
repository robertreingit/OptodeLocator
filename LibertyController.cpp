#include "LibertyController.h"

#include <windows.h>
#include <tchar.h>
#include "PDI.h"

#include "DataStructDefs.h"

#include <string>
#include <Dense>
#include <QVector>

LibertyController::LibertyController(QWidget *parent) :
    QWidget(parent),
		m_captureReady( false ),
		m_configured( false )
{
	m_CPDI = new CPDIdev();
	qDebug("LibertyController::c'tor");
}

LibertyController::~LibertyController()
{
	if (m_CPDI) {
		if ( m_CPDI->CnxReady() )
			this->disconnect();
		delete m_CPDI;
	}
	qDebug("LibertyController::d'tor");
}

//////////////////////////////////////////////////////////////////////////
/// Tries to establish connection to Polhemus via USB only.
//////////////////////////////////////////////////////////////////////////
void LibertyController::connect()
{
    qDebug("LibertyController::connect()");
	if ( m_CPDI->ConnectUSB() ) {
		qDebug("Connection successfull!");
		emit connected(true);
	}
	else {
		qDebug("Connection unsuccessfull!");
		emit errorMsg( msgToQString() );
	}
    
}

void LibertyController::disconnect()
{
    qDebug("LibertyController::disconnect()");
	if ( m_captureReady )
		stop();

	if ( m_CPDI->Disconnect() )
		emit connected(false);
	else errorMsg( msgToQString() );
}

//////////////////////////////////////////////////////////////////////////
/// Configure Polhemus Station
/// Hard-coded for my purposes (see individual parameters). Change accordingly ...
//////////////////////////////////////////////////////////////////////////
void LibertyController::configure()
{
    qDebug("LibertyController::configure()");
	if ( !m_CPDI->CnxReady() ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// set to binary mode
	if ( !m_CPDI->SetBinary( TRUE ) ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// Use half frequency
	if ( !m_CPDI->SetFrameRate( PI_FRATE_HALF ) ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// configure to use metric system
	if ( !m_CPDI->SetMetric( TRUE ) ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// configure to stylus mode
	if ( !m_CPDI->SetSStylusMode( 1, PI_STYMODE_POINT ) ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// reset frame count
	if ( !m_CPDI->ResetFrameCount() ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// reset align frames
	if ( !m_CPDI->ResetSAlignment( -1 ) ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// enable station and first two sensors
	if ( !m_CPDI->EnableStation( -1, TRUE ) ) {
		emit errorMsg( msgToQString() );
		return;
	}
	if ( !m_CPDI->EnableSensor( 1, TRUE ) ) {
		emit errorMsg( msgToQString() );
		return;
	}
	if ( !m_CPDI->EnableSensor( 2, TRUE ) ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// Configure filter
	CPDIfilter filter;
	filter.m_fSensitivity	= 0.0f;
	filter.m_fLowValue		= 0.5f;
	filter.m_fHighValue		= 0.5f;
	filter.m_fMaxTransRate	= 0.0f;
	if ( !m_CPDI->SetPosFilter( filter ) || !m_CPDI->SetAttFilter( filter ) ) {
		emit errorMsg( msgToQString() );
		return;
	}

	// configure hemisphere
	PDI3vec hemisphere;
	hemisphere[0] = 0.0f;
	hemisphere[1] = 0.0f;
	hemisphere[2] = -1.0f;
	if ( !m_CPDI->SetSHemisphere( -1, hemisphere ) ) {
		emit errorMsg( msgToQString() );
		return;
	}
	
	// configure result data structure
	CPDImdat mdat;	
	mdat.Append( PDI_MODATA_POS );
	mdat.Append( PDI_MODATA_STYLUS );
	if ( !m_CPDI->SetSDataList( 1, mdat ) ) {
		emit errorMsg( msgToQString() );
		return;
	}
	mdat.Empty();
	mdat.Append( PDI_MODATA_POS );
	mdat.Append( PDI_MODATA_ORI );
	if ( !m_CPDI->SetSDataList( 2, mdat ) ) {
		emit errorMsg( msgToQString() );
		return;
	}
    emit configured(true);
	qDebug("Configuration successfull");
}

//////////////////////////////////////////////////////////////////////////
/// Start waiting for data.
//////////////////////////////////////////////////////////////////////////
void LibertyController::start()
{
    qDebug("LibertyController::start()");
	
	if ( m_captureReady )
	return;

	if ( m_CPDI->StartStylusPno( this->winId() ) )
		emit started(true);
	else
		emit errorMsg( msgToQString() );
}

void LibertyController::stop()
{
    qDebug("LibertyController::stop()");

	if ( m_CPDI->StopStylusPno() )
		emit started(false);
	else
		emit errorMsg( msgToQString() );
}

//////////////////////////////////////////////////////////////////////////
/// Data event occurred, calling parseData() functions.
//////////////////////////////////////////////////////////////////////////
bool LibertyController::winEvent( MSG* message, long* result )
{	
	switch ( message->message ) {
	case WM_PI_RAWDATA_READY:
		qDebug("Caught signal");
		parseData();
		return true;

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Parses new data frame.
/// Works only according to configuration (see above). Check
/// Polhemus docs for more details.
//////////////////////////////////////////////////////////////////////////
void LibertyController::parseData()
{
	PBYTE pBuffer;
	DWORD dwSize;
	DWORD pos = 0;
	StylusDataFrame *dfStylus;
	MarkerDataFrame *dfMarker;
	int prec = 4;

	qDebug("Processing data");

	// Retrieve data
	m_CPDI->LastPnoPtr( pBuffer, dwSize );

	dfStylus = (StylusDataFrame*)pBuffer;
	dfMarker = (MarkerDataFrame*)(pBuffer+8+dfStylus->hrd.length);

	// Convert from local to global points
	using namespace Eigen;

	Vector3f stylus_pos;
	Vector3f marker_pos;
	Vector3f marker_att;
	Vector3f stylus_local;
	Matrix3f rotation;

	stylus_pos << dfStylus->xPos, dfStylus->yPos, dfStylus->zPos;
	marker_pos << dfMarker->xPos, dfMarker->yPos, dfMarker->zPos;
	marker_att << dfMarker->azimuth, dfMarker->elevation, dfMarker->roll;
	marker_att *= M_PI/-180.0f;	
	rotation = 
		  AngleAxisf(marker_att[2], Vector3f::UnitX())
		* AngleAxisf(marker_att[1], Vector3f::UnitY())
		* AngleAxisf(marker_att[0], Vector3f::UnitZ());
	stylus_local = rotation * (stylus_pos-marker_pos);
	// end: Convert from local to global points

	QVector<double> point;
	point << stylus_local[0] << stylus_local[1] << stylus_local[2];
	emit newPoint( point );

}

QString LibertyController::msgToQString()
{
	LPCTSTR msg = m_CPDI->GetLastResultStr();
	return QString::fromUtf16( (const ushort*) msg );
}

void LibertyController::reset()
{
	qDebug("LibertyController::reset()");
    
	if ( m_captureReady )
		stop();

	if ( m_CPDI->CnxReady() )
		disconnect();

	emit resetted();
}