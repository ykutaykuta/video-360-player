#ifndef OPENGLQML_H
#define OPENGLQML_H

#define STACK_COUNT 100
#define SECTOR_COUNT 100
#define FOV 120

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <math.h>
#include <QQuickItem>
#include <vector>
#include <QOpenGLBuffer>
#include <QVector3D>
//#include <QVector2D>
#include <QQuickWindow>
#include <QOpenGLContext>
#include <QImage>
#include <QDir>


class OpenGLQmlRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLQmlRenderer();
    ~OpenGLQmlRenderer();

    void setT(qreal t);
    void setViewportSize(const QSize &size);
    QSize getViewportSize() const;
    void setWindow(QQuickWindow *window);

    void setLastPos(const QPoint tmp);
    void setCurrPos(const QPoint tmp);
private:
    void initGL();
    void initShader();
    void initTexture();
    void initVertex();
    void initMVPMatrix();
    void updateMVPMatrix();
    void calRot();

public slots:
    void paint();
public:
	static void setTexture(const QImage tmp);

private:
    std::vector<float> mArr_vertex;
    std::vector<unsigned int> mArr_indice;
    QOpenGLBuffer m_arrayBuff;
    QOpenGLBuffer m_indexBuff;
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram *mPtr_program;
    QQuickWindow *mPtr_window;
    QMatrix4x4 m_MVP;
    QOpenGLTexture *mPtr_texture;
    QPoint m_lastPos;
    QPoint m_currPos;
    float m_anglePhi;   // angle to plane Oxz (radian)
    float m_angleTheta; // angle to postive axis Oz (radian)

    QVector3D m_desViewPos;
    QMatrix4x4 m_projection;
    static QImage m_Image;
	static bool isPainting;
//    static OpenGLQmlRenderer *mPtr_instance;
};

class OpenGLQml : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    OpenGLQml();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t;
    OpenGLQmlRenderer *mPtr_renderer;
    QMatrix4x4 m;

    // QQuickItem interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // OPENGLQML_H
