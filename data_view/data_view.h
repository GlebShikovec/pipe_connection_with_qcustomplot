#ifndef DATA_VIEW_H
#define DATA_VIEW_H

#include <QtWidgets/QMainWindow>
#include "ui_data_view.h"

class data_view : public QMainWindow
{
	Q_OBJECT
		HANDLE pipe;

public:
	data_view(QWidget *parent = 0);
	~data_view();

private:
	Ui::data_viewClass ui;
	float readPipe();

private slots:
	void viewDataSlot();
};

#endif // DATA_VIEW_H
