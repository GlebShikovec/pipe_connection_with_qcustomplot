#include "data_view.h"
#include <QtWidgets/QApplication>



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	data_view w;
	w.show();
	return a.exec();
}
