#include "data_view.h"
#include <windows.h>

data_view::data_view(QWidget *parent)
	: QMainWindow(parent)
{
	// Open the named pipe.
	pipe = CreateFile(
		L"\\\\.\\pipe\\pipe_plot_data",		// name of the pipe
		GENERIC_READ,						// only need read access
		FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode (read|write)
		NULL,								// security attributes
		OPEN_EXISTING,						// open existing pipe
		FILE_ATTRIBUTE_NORMAL,				// default attributes and flags
		NULL								// no template file
	);

	ui.setupUi(this);

	ui.mainplot->addGraph();
	ui.mainplot->graph(0)->setPen(QPen(QColor(40, 110, 255))); // blue line

	QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
	timeTicker->setTimeFormat("%h:%m:%s");
	ui.mainplot->xAxis->setTicker(timeTicker);
	ui.mainplot->axisRect()->setupFullAxesBox();
	//ui.mainplot->graph(0)->rescaleKeyAxis();
	ui.mainplot->yAxis->setRange(-1.2, 1.2);

	// Make left and bottom axes transfer their ranges to right and top axes.
	connect(ui.mainplot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui.mainplot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui.mainplot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui.mainplot->yAxis2, SLOT(setRange(QCPRange)));

	// Setup a timer that repeatedly calls data_view::viewDataSlot.
	QTimer *dataTimer = new QTimer(this);
	connect(dataTimer, SIGNAL(timeout()), this, SLOT(viewDataSlot()));
	dataTimer->start(10); // 10 ms. Interval 0 means to refresh as fast as possible.
}

data_view::~data_view()
{

}

float data_view::readPipe() {

	// The read operation will block until there is data to read.
	float buffer;
	DWORD numBytesRead = 0;
	BOOL result = ReadFile(
		pipe,			// opened pipe
		&buffer,		// the data from the pipe will be put here
		sizeof(float),  // number of bytes allocated
		&numBytesRead,	// this will store number of bytes actually read
		NULL			// not using overlapped IO
	);
	return buffer;
}

void data_view::viewDataSlot() {

	// Get slice of data.
	float buffer = data_view::readPipe();

	static QTime time(QTime::currentTime());

	// Perform new data point.
	float key = time.elapsed() / 1000.0; // Time elapsed since start of demo, in seconds.
	static float lastPointKey = 0;
	if (key - lastPointKey > 0.010)		  // At most add point every 10 ms.
	{
		// Add point to plot:
		ui.mainplot->graph(0)->addData(key, buffer);
		//ui.mainplot->graph(0)->rescaleValueAxis();
		lastPointKey = key;
	}
	// Make key axis range scroll with the data (at a constant range size of range_size).
	const int range_size = 20;
	ui.mainplot->xAxis->setRange(key, range_size, Qt::AlignRight);
	ui.mainplot->replot();

	// Calculate frames per second and number of data points.
	static float lastFpsKey;
	static int frameCount;
	++frameCount;
	if (key - lastFpsKey > 2) // Average fps over 2 seconds.
	{
		ui.statusBar->showMessage(
			QString("%1 FPS, Total Data points: %2")
			.arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
			.arg(ui.mainplot->graph(0)->data()->size())
			, 0);
		lastFpsKey = key;
		frameCount = 0;
	}
}
