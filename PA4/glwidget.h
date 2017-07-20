#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QString>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

#include "camera3d.h"
#include "transform3d.h"

class pathVector {
public:
    float theta, phi;
    float pTheta, pPhi;
    QVector3D value, pValue;
};

class pathAttr {
public:
    int head, tail;
    int m, vectors;
    pathVector path[1000];
};

class drawAttr {
public:
    int curveOrder;
    float step;
    float radius;
    float spacement;
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(int step, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void startPath (pathAttr *path, int numPoints);
    void updatePath (pathAttr *path);
    void calcVec (pathAttr *path);
    void createDiscs (QVector3D *verts, pathAttr path);
    void connectUpdate();
    void disconnectUpdate();

    QVector3D difusiveColor;
    QVector3D ambientColor;
    QVector3D specularColor;
    float shininess;
    int paused = 0;
    bool restart = false;
    int numPoints;
    drawAttr snakeAttr;

    QOpenGLShaderProgram *m_program;
    QOpenGLShader *shader;

public slots:
    void cleanup();

protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


protected slots:
  void update();

private:
    void printContextInformation();

    QOpenGLBuffer m_vertex;
    QOpenGLBuffer m_normal;

    QOpenGLVertexArrayObject m_object;

    // Shader Information
    int u_modelToWorld;
    int u_cameraToView;
    int u_worldToCamera;
    int u_lightPosition;
    int u_difuseColor;
    int u_ambientColor;
    int u_specularColor;
    int u_shadding;

    Camera3D m_camera;
    QMatrix4x4 m_projection;
    Transform3D m_transform;

    QVector3D vLightPosition;

    int actual;
    int factor;
    int step_;
    int indiceSize_, normalIndiceSize_;
    int vertexCount_, normalCount_;
    int m_normalAttr;
    int u_shininess;
    QVector3D color_;
    QVector3D *vertices, *sg_vertexes_, *sg_normals_;

    QMetaObject::Connection m_connection;

};

#endif
