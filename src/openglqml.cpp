#include "openglqml.h"
//#include <QTime>
//#include <QDebug>

#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1

QImage OpenGLQmlRenderer::m_Image = QImage();
bool OpenGLQmlRenderer::isPainting = false;
//OpenGLQmlRenderer* OpenGLQmlRenderer::mPtr_instance = nullptr;

OpenGLQmlRenderer::OpenGLQmlRenderer()
    : m_t(0), m_indexBuff(QOpenGLBuffer::IndexBuffer)
{
    mPtr_texture = nullptr;


    m_arrayBuff.create();
    m_indexBuff.create();

    initGL();
}

OpenGLQmlRenderer::~OpenGLQmlRenderer()
{
    mPtr_texture->destroy();
    delete  mPtr_texture;

    delete mPtr_program;

//    delete mPtr_window;

    m_arrayBuff.destroy();
    m_arrayBuff.destroy();
}


void OpenGLQmlRenderer::setT(qreal t)
{
     m_t = t;
}

void OpenGLQmlRenderer::setViewportSize(const QSize &size)
{
     m_viewportSize = size;
}

QSize OpenGLQmlRenderer::getViewportSize() const
{
    return m_viewportSize;
}

void OpenGLQmlRenderer::setWindow(QQuickWindow *window)
{
    mPtr_window = window;
}

void OpenGLQmlRenderer::setLastPos(const QPoint tmp)
{
    m_lastPos = tmp;
}

void OpenGLQmlRenderer::setCurrPos(const QPoint tmp)
{
    m_currPos = tmp;
    calRot();
    m_lastPos = m_currPos;
}

void OpenGLQmlRenderer::initGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

	initTexture();
    initVertex();
    initShader();
    initMVPMatrix();
}

void OpenGLQmlRenderer::initShader()
{
    mPtr_program = new QOpenGLShaderProgram();

    mPtr_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                                "uniform highp mat4 matrix;"
                                                "attribute highp vec3 vertices;"
                                                "attribute highp vec2 texcoords;"
                                                "varying highp vec2 coords;"
                                                "void main() {"
                                                "    gl_Position = matrix * vec4(vertices, 1.0);"
                                                "    coords = texcoords;"
                                                "}");

    mPtr_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                                "uniform highp sampler2D texture;"
                                                "varying highp vec2 coords;"
                                                "void main() {"
                                                "gl_FragColor = texture2D(texture, coords);"
                                                "}");

    mPtr_program->bindAttributeLocation("vertices", POSITION_LOCATION);
    mPtr_program->bindAttributeLocation("texcoods", TEX_COORD_LOCATION);
    mPtr_program->link();
}

void OpenGLQmlRenderer::initTexture()
{
	isPainting = true;

	QDir dir(QCoreApplication::applicationDirPath());
	m_Image = QImage(dir.absoluteFilePath("image/bg.jpg"));

	mPtr_texture = new QOpenGLTexture(m_Image, QOpenGLTexture::MipMapGeneration::DontGenerateMipMaps);

	isPainting = false;
}

void OpenGLQmlRenderer::initVertex()
{
    float x, y, z, xz;
    float s, t;

    float sectorStep = 2*M_PI / SECTOR_COUNT;
    float stackStep = M_PI / STACK_COUNT;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= STACK_COUNT; i++)
    {
        stackAngle = M_PI/2 - i*stackStep;
        xz = cosf(stackAngle);
        y    = sinf(stackAngle);

        for(int j = 0; j <= SECTOR_COUNT; j++)
        {
            sectorAngle = j * sectorStep;

            z = xz * cosf(sectorAngle);
            x = xz * sinf(sectorAngle);

            mArr_vertex.push_back(x);
            mArr_vertex.push_back(y);
            mArr_vertex.push_back(z);


            s = (float)j / SECTOR_COUNT;
            t = (float)i / STACK_COUNT;
            mArr_vertex.push_back(s);
            mArr_vertex.push_back(t);
        }
    }

    unsigned int k1, k2;
    for(unsigned int i = 0; i < STACK_COUNT; i++)
    {
        k1 = i * (SECTOR_COUNT + 1);
        k2 = k1 + SECTOR_COUNT + 1; // (i+1) * (SECTOR_COUNT+1)

        for(int j = 0; j < SECTOR_COUNT; j++, k1++, k2++)
        {
            if(i != 0)
            {
                mArr_indice.push_back(k1);
                mArr_indice.push_back(k2);
                mArr_indice.push_back(k1+1);
            }

            if(i != (STACK_COUNT-1))
            {
                mArr_indice.push_back(k1+1);
                mArr_indice.push_back(k2);
                mArr_indice.push_back(k2+1);
            }

        }
    }


    m_arrayBuff.bind();
    m_arrayBuff.allocate(mArr_vertex.data(), mArr_vertex.size()*sizeof(float));

    m_indexBuff.bind();
    m_indexBuff.allocate(mArr_indice.data(), mArr_indice.size()*sizeof(int));

}

void OpenGLQmlRenderer::initMVPMatrix()
{
    m_desViewPos.setX(0.0f);    m_desViewPos.setY(0.0f);   m_desViewPos.setZ(-1.0f);
    QMatrix4x4 view;
    view.lookAt(QVector3D(0, 0, 0), m_desViewPos, QVector3D(0, 1, 0));
    m_projection.setToIdentity();
    m_projection.perspective(FOV, m_viewportSize.width()/m_viewportSize.height(), 0.2f, 2.0f);

    m_MVP = m_projection * view;

    m_anglePhi = 0;
    m_angleTheta = M_PI;
}

void OpenGLQmlRenderer::updateMVPMatrix()
{
    QMatrix4x4 view;
    view.lookAt(QVector3D(0, 0, 0), m_desViewPos, QVector3D(0, 1, 0));
    m_MVP = m_projection * view;
}

void OpenGLQmlRenderer::calRot()
{
    float monitorCenterY = m_viewportSize.height()/2;
    float monitorCenerX = m_viewportSize.width()/2;

    float tmp;

    tmp = (float)(m_lastPos.y()-monitorCenterY)/m_viewportSize.height()*sqrt(3);
    float lastPos_Phi_Des = atan(tmp/0.5);

    tmp = (float)(m_lastPos.x()-monitorCenerX)/m_viewportSize.height()*sqrt(3);
    float lastPos_Theta_Des = atan(tmp/0.5);

    tmp = (float)(m_currPos.y()-monitorCenterY)/m_viewportSize.height()*sqrt(3);
    float currPos_Phi = atan(tmp/0.5) + m_anglePhi;

    tmp = (float)(m_currPos.x()-monitorCenerX)/m_viewportSize.height()*sqrt(3);
    float currPos_Theta = atan(tmp/0.5) + m_angleTheta;

    float newAnglePhi = currPos_Phi - lastPos_Phi_Des;
    if(newAnglePhi > M_PI/3 || newAnglePhi < -M_PI/3)
        return;
    float newAngleTheta = currPos_Theta - lastPos_Theta_Des;

    m_desViewPos.setY(sinf(newAnglePhi));
    m_desViewPos.setZ(cosf(newAnglePhi)*cosf(newAngleTheta));
    m_desViewPos.setX(cosf(newAnglePhi)*sinf(newAngleTheta));

    m_anglePhi = newAnglePhi;
    m_angleTheta = newAngleTheta;

    updateMVPMatrix();
}

void OpenGLQmlRenderer::paint()
{
	isPainting = true;

	mPtr_texture->destroy();
	mPtr_texture->create();
	mPtr_texture->setMaximumAnisotropy(16);
	mPtr_texture->setData(m_Image.mirrored(true, false));

    mPtr_program->bind();

    mPtr_program->setUniformValue("texture", 0);

    m_arrayBuff.bind();
    m_indexBuff.bind();

    int offset = 0;

    mPtr_program->enableAttributeArray(POSITION_LOCATION);
    mPtr_program->setAttributeBuffer(POSITION_LOCATION, GL_FLOAT, offset, 3, 5*sizeof(float));

    offset += 3*sizeof(float);

    mPtr_program->enableAttributeArray(TEX_COORD_LOCATION);
    mPtr_program->setAttributeBuffer(TEX_COORD_LOCATION, GL_FLOAT, offset, 2, 5*sizeof(float));

    mPtr_program->setUniformValue("matrix", m_MVP);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    mPtr_texture->bind();

    glDrawElements(GL_TRIANGLES, mArr_indice.size(), GL_UNSIGNED_INT, 0);

    mPtr_program->disableAttributeArray(POSITION_LOCATION);
    mPtr_program->disableAttributeArray(TEX_COORD_LOCATION);
    mPtr_program->release();

    mPtr_window->resetOpenGLState();

	isPainting = false;
}

void OpenGLQmlRenderer::setTexture(const QImage tmp)
{
	if(isPainting)
		return;
    m_Image = tmp;
}

void OpenGLQml::sync()
{
    if (!mPtr_renderer)
    {
        mPtr_renderer = new OpenGLQmlRenderer();
        connect(window(), &QQuickWindow::beforeRendering, mPtr_renderer, &OpenGLQmlRenderer::paint, Qt::DirectConnection);
    }

    mPtr_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    mPtr_renderer->setT(m_t);
    mPtr_renderer->setWindow(window());
}

OpenGLQml::OpenGLQml()
    : m_t(0)
    , mPtr_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &OpenGLQml::handleWindowChanged, Qt::DirectConnection);
    setAcceptedMouseButtons(Qt::AllButtons);
//    setAcceptTouchEvents(Qt::TouchPointMoved);
//    setAcceptTouchEvents(Qt::TouchPointPressed);
	//    setAcceptTouchEvents(Qt::TouchPointReleased);
}

void OpenGLQml::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void OpenGLQml::handleWindowChanged(QQuickWindow *win)
{
    if (win)
    {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &OpenGLQml::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &OpenGLQml::cleanup, Qt::DirectConnection);

        win->setClearBeforeRendering(false);
    }
}

void OpenGLQml::mousePressEvent(QMouseEvent *event)
{
    mPtr_renderer->setLastPos(event->pos());
}

void OpenGLQml::mouseMoveEvent(QMouseEvent *event)
{
    mPtr_renderer->setCurrPos(event->pos());
}

void OpenGLQml::mouseReleaseEvent(QMouseEvent *event)
{

}

void OpenGLQml::cleanup()
{
    if (mPtr_renderer)
    {
        delete mPtr_renderer;
        mPtr_renderer = nullptr;
    }
}
