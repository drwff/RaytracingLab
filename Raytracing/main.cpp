#include "ShaderWidget.h"
#include <QtWidgets/QApplication>
#pragma comment (lib, "opengl32.lib")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ShaderWidget w;
	w.show();
	return a.exec();
}
