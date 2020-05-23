#pragma once
#include <QtWidgets/QWidget>
#include <QtOpenGL>
#include <qimage.h>
#include <qmutex.h>
#include <qcompilerdetection.h>
#include <qopenglfunctions_4_3_core.h>
#pragma comment (lib, "opengl32.lib")

class ShaderWidget : public QOpenGLWidget
{
private:
	QOpenGLShaderProgram m_program;
	GLfloat* vert_data;
	int vert_data_location;

	QOpenGLFunctions_4_3_Core *functions;
	GLuint ssbo = 0;
	struct Sphere
	{
		QVector3D position;
		float radius;
		QVector3D color;
		int material_idx;

	};
protected:
	void initializeGL() override;
	void resizeGL(int nWidth, int nHeight) override;
	void paintGL() override;
public:
	ShaderWidget(QWidget *parent = 0);
	~ShaderWidget();
};
