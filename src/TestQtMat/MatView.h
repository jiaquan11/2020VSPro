#pragma once
#include <QOpenGLWidget>
class MatView : public QOpenGLWidget
{
	Q_OBJECT

public:
	MatView(QWidget* p);
	~MatView();

	void paintEvent(QPaintEvent *e);
};

