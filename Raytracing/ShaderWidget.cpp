#include "ShaderWidget.h"

ShaderWidget::ShaderWidget(QWidget * parent):QOpenGLWidget(parent), vert_data_location(0)
{
	vert_data = new GLfloat[12];
	vert_data[0] = -1.0f; vert_data[1] = -1.0f; vert_data[2] = 0.0f;
	vert_data[3] = 1.0f;  vert_data[4] = -1.0f; vert_data[5] = 0.0f;
	vert_data[6] = 1.0f;  vert_data[7] = 1.0f;  vert_data[8] = 0.0f;
	vert_data[9] = -1.0f; vert_data[10] = 1.0f; vert_data[11] = 0.0f;
}

ShaderWidget::~ShaderWidget()
{
	delete[] vert_data;
}

void ShaderWidget::initializeGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	QOpenGLShader vShader(QOpenGLShader::Vertex);
	vShader.compileSourceFile("raytracing.vert");
	QOpenGLShader fShader(QOpenGLShader::Fragment);
	fShader.compileSourceFile("raytracing.frag");
	m_program.addShader(&vShader);
	m_program.addShader(&fShader);
	if (!m_program.link())
	{
		qWarning("Error link programm shader");
		return;
	}
	vert_data_location = m_program.attributeLocation("vertex");// расположение нашего массива вершин в пределах списка параметров шейдерной программы
	qDebug() << QString("Log programm");
	qDebug() << m_program.log();

	if (!m_program.bind())
		qWarning("Error bind programm shader");

	m_program.setUniformValue("camera.position", QVector3D(0.0, 0.0, -10.0));
	m_program.setUniformValue("camera.view", QVector3D(0.0, 0.0, 1.0));
	m_program.setUniformValue("camera.up", QVector3D(0.0, 1.0, 0.0));
	m_program.setUniformValue("camera.side", QVector3D(1.0, 0.0, 0.0));
	m_program.setUniformValue("scale", QVector2D(width(), height()));
	m_program.release();

	std::vector<Sphere> all_spheres;
	all_spheres.push_back(Sphere{ QVector3D(-1,0,-2),2,QVector3D(1.0,0.3,0.6) ,0 });
	all_spheres.push_back(Sphere{ QVector3D(3,0,-3),1,QVector3D(0.0,0.6,5.2) ,0 });
	all_spheres.push_back(Sphere{ QVector3D(-4,0,-4),1,QVector3D(1.0,0.0,1.2) ,0 });

	functions = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>();
	functions->glGenBuffers(1, &ssbo);
	functions->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	functions->glBufferData(GL_SHADER_STORAGE_BUFFER, all_spheres.size() * sizeof(Sphere), all_spheres.data(), GL_DYNAMIC_COPY);
	functions->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
}

void ShaderWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (!m_program.bind())
		return;

	m_program.enableAttributeArray(vert_data_location); //шейдер будет брать данные по этому адресу, здесь происходит включение
	m_program.setAttributeArray(vert_data_location, vert_data, 3);// говорит откуда брать данные, сами данные и размер компоненты
	glDrawArrays(GL_QUADS, 0, 4); //отрисовка примитива, с какой вершины начать и сколько рисовать
	m_program.disableAttributeArray(vert_data_location);
	m_program.release();
}

void ShaderWidget::resizeGL(int nWidth, int nHeight)
{
	glViewport(0, 0, nWidth, nHeight);
	if (!m_program.bind())
		qWarning("Error bind programm shader");
	m_program.setUniformValue("scale", QVector2D(width(), height()));
	m_program.release();

}