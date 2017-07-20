#include "glwidget.h"
#include <QDebug>
#include <QString>

#include <cmath>
#include <unistd.h>

#include <iostream>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>

#include "input.h"

#include <iostream>
#include <random>

std::random_device rd;

pathAttr path;
float knot[1011];


float randAngle (float min, float max) {
    float num = min + (max - min) * rd() / rd.max();

    return num;
}

QVector3D rotate (float theta, float phi) {
    float x = cos (theta) * sin (phi);
    float y = sin (theta) * sin (phi);
    float z = cos (phi);

    return QVector3D (x, y, z);
}

void GLWidget::startPath (pathAttr *path, int numPoints) {

    float a1 = randAngle(0, 2 * 3.14);
    float a2 = randAngle(0, 2 * 3.14);
    path->path[0].value = QVector3D (0,0,0);
    path->head = 1;
    path->tail = 0;
    path->vectors = 2;
    path->m = numPoints;
    int head = path->head;
    path->path[head].theta = a1;
    path->path[head].phi = a2;
    path->path[(head+1)%path->m].pTheta = a1;
    path->path[(head+1)%path->m].pPhi = a2;
    path->path[head].value = rotate (a1, a2) + path->path[0].value;
    path->path[(head+1)%path->m].pValue = path->path[head].value;
}

void GLWidget::calcVec (pathAttr *path) {
    float a1 = randAngle(0, 3.14);
    float a2 = randAngle(0, 3.14);
    if (2 * a1 > 3.14)
        a1 += 3.14;
    if (2 * a2 > 3.14)
        a2 += 3.14;


    path->head = (path->head + 1) % path->m;
    int head = path->head;
    path->path[head].theta = a1 + path->path[path->head].pTheta;
    if (path->path[head].theta > 2 * 3.14)
        path->path[head].theta -= 3.14;
    path->path[head].phi = a2 + path->path[head].pPhi;
    if (path->path[head].phi > 2 * 3.14)
        path->path[head].phi -= 3.14;

    QVector3D newValue = path->path[head].pValue + snakeAttr.spacement * rotate (path->path[head].theta, path->path[head].phi);
    QVector3D normal;
    int reflected = 0;
    if (newValue.x() > 1) {
        normal = QVector3D (-1, 0 , 0);
        reflected = 1;
    }
    else if (newValue.x() < -1) {
        normal = QVector3D (1, 0 , 0);
        reflected = 1;
    }
    else if (newValue.y() > 1) {
        normal = QVector3D (0, -1 , 0);
        reflected = 1;
    }
    else if (newValue.y() < -1) {
        normal = QVector3D (0, 1 , 0);
        reflected = 1;
    }
    else if (newValue.z() > 1) {
        normal = QVector3D (0, 0 , -1);
        reflected = 1;
    }
    else if (newValue.z() < -1) {
        normal = QVector3D (0, 0 , 1);
        reflected = 1;
    }

    if (reflected) {
        newValue = newValue - 2*(newValue.dotProduct(normal , newValue.normalized()))*normal;
        QVector3D v1 = QVector3D (newValue.x(), 0, newValue.z()).normalized();
        QVector3D v2 = QVector3D (newValue.x(), newValue.y(), 0).normalized();
        if (path->path[head].theta <= 3.14)
            path->path[head].theta = 3.14 - v1.dotProduct(v1, normal);
        else
            path->path[head].theta = 3.14 + v1.dotProduct(v1, normal);
        if (path->path[head].phi <= 3.14)
            path->path[head].phi = 3.14 - v2.dotProduct(v2, normal);
        else
            path->path[head].phi = 3.14 + v2.dotProduct(v2, normal);
    }

    path->path[(head+1)%path->m].pTheta =  path->path[head].theta;
    path->path[(head+1)%path->m].pPhi =  path->path[head].phi;
    path->path[head].value = newValue;
    path->path[(head+1)%path->m].pValue = path->path[head].value;

}

void GLWidget::updatePath (pathAttr *path) {

    if (path->vectors == path->m) {
        calcVec (path);
        path->tail = (path->tail + 1) % path->m;
    }


    while (path->vectors < path->m) {
        calcVec (path);
        path->vectors++;
    }
}

void createKnots (pathAttr *path, int d) {
    int j = 0;
    for (int i = 0; i < d + 1 + path->m;++i)
        knot[i] = j++;
}

float spline (int k, int d, float t) {
    if (d == 0) {
        if (knot[k] <= t && t < knot[k+1])
            return 1;
        return 0;
    }
    float a = (t - knot[k])/(knot[k+d] - knot[k]);
    float b = (knot[k+d+1] - t)/(knot[k+d+1] - knot[k+1]);
    return a * spline (k,d-1,t) + b * spline(k+1,d-1,t);
}

QVector3D curve (float d, pathAttr *path, float t) {
    QVector3D val = QVector3D(0,0,0);
    int tail = path->tail;
    for (int i = 0; i < path->m; ++i) {
        val += spline (i,d, t) * path->path[tail].value;
        tail = (tail + 1) % path->m;
    }
    return val;
}

void GLWidget::createDiscs (QVector3D *verts, pathAttr path) {
    std::vector<QVector3D> curvePoints;
    std::vector<QVector3D> normCurvePoints;
    std::vector<QVector3D> vertices;
    for (float i = 1; i < path.m - snakeAttr.curveOrder + 2; i += snakeAttr.step)
        curvePoints.push_back (curve (snakeAttr.curveOrder, &path, i));

    float step = 2*3.14 / 32;
    float r = snakeAttr.radius;
    float phi = 3.14/2;
    for (int i = 0; (uint) i < curvePoints.size()-1; ++i) {
        for (float theta = 0; theta <= 2*3.14; theta += step) {
            QQuaternion q1, q3, q2, q4;
            q1 = QQuaternion (cos (0), QVector3D (0,1,0)*sin(0));
            q2 = QQuaternion (cos (0), QVector3D (1,0,0)*sin(0));
            q3 = QQuaternion (cos (0), QVector3D (0,1,0)*sin(0));
            q4 = QQuaternion (cos (0), QVector3D (1,0,0)*sin(0));

            vertices.push_back(q2.rotatedVector (q1.rotatedVector (curvePoints[i] + QVector3D(r*cos(theta)*sin(phi), r*sin(theta)*sin(phi), cos(phi)))));
            vertices.push_back(q4.rotatedVector (q3.rotatedVector(curvePoints[i+1] + QVector3D(r*cos(theta)*sin(phi), r*sin(theta)*sin(phi), cos(phi)))));
            vertices.push_back(q4.rotatedVector (q3.rotatedVector(curvePoints[i+1] + QVector3D(r*cos(theta+step)*sin(phi), r*sin(theta+step)*sin(phi), cos(phi)))));
            vertices.push_back(q4.rotatedVector (q3.rotatedVector(curvePoints[i+1] + QVector3D(r*cos(theta+step)*sin(phi), r*sin(theta+step)*sin(phi), cos(phi)))));
            vertices.push_back(q2.rotatedVector (q1.rotatedVector (curvePoints[i] + QVector3D(r*cos(theta+step)*sin(phi), r*sin(theta+step)*sin(phi), cos(phi)))));
            vertices.push_back(q2.rotatedVector (q1.rotatedVector(curvePoints[i] + QVector3D(r*cos(theta)*sin(phi), r*sin(theta)*sin(phi), cos(phi)))));
        }

    }
    for (float theta = 0; theta <= 2*3.14; theta += step) {
        vertices.push_back(curvePoints[0]);
        vertices.push_back(curvePoints[0] + QVector3D(r*cos(theta)*sin(phi), r*sin(theta)*sin(phi), cos(phi)));
        vertices.push_back(curvePoints[0] + QVector3D(r*cos(theta+step)*sin(phi), r*sin(theta+step)*sin(phi), cos(phi)));
    }

    for (float theta = 0; theta <= 2*3.14; theta += step) {
        int i = curvePoints.size() - 1;
        vertices.push_back(curvePoints[i]);
        vertices.push_back(curvePoints[i] + QVector3D(r*cos(theta)*sin(phi), r*sin(theta)*sin(phi), cos(phi)));
        vertices.push_back(curvePoints[i] + QVector3D(r*cos(theta+step)*sin(phi), r*sin(theta+step)*sin(phi), cos(phi)));
    }

    verts = (QVector3D*) malloc (vertices.size() * sizeof(QVector3D));
    for (int i = 0; (uint) i < vertices.size();++i)
        verts[i] = vertices[i];

    m_object.bind();
    m_vertex.bind();

    vertexCount_ = vertices.size();
    sg_vertexes_ = verts;
    normalCount_ = vertices.size();
    sg_normals_ = verts;
    m_vertex.allocate(sg_vertexes_, vertexCount_*sizeof(QVector3D));
    m_normal.allocate(sg_normals_, normalCount_*sizeof(QVector3D));

    m_object.release();
    m_vertex.release();

}


QOpenGLTexture *texture;

GLWidget::GLWidget(int step, QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(0) {
  step_ = step;
}

GLWidget::~GLWidget() {
    cleanup();
}

QSize GLWidget::minimumSizeHint() const {
    return QSize(200, 200);
}

QSize GLWidget::sizeHint() const {
    return QSize(400, 400);
}

void GLWidget::cleanup() {
    makeCurrent();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

void GLWidget::connectUpdate() {
  m_connection = connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
  update();
}

void GLWidget::disconnectUpdate() {
  disconnect(m_connection);
}

QVector3D *verts;
void GLWidget::initializeGL() {

  startPath(&path, numPoints);
  updatePath(&path);

  snakeAttr.step = 0.1;
  createKnots(&path, snakeAttr.curveOrder);

  createDiscs(verts, path);

  vLightPosition = QVector3D (0.0f, 4.0f, 5.0f);

  m_camera.translate(QVector3D(0.0f, 0.0f, 2.0f));
  // Put the object a little to the front of the camera (the camera is looking
  // at (0.0, 0.0, -1.0))
  actual = factor = -5;
  m_transform.translate(0.0f, 0.0f, -5.0f);
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();
  connectUpdate();
  printContextInformation();
  // Set global information
  glEnable(GL_DEPTH_TEST);
  glDepthRange(0,1);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

  // Application-specific initialization
  {
    // Create Shader (Do not release until VAO is created)
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/BP_interp_sd.vert");
    shader = new QOpenGLShader(QOpenGLShader::Fragment);
    shader->compileSourceFile(":/shaders/BP_interp_sd.frag");
    m_program->addShader(shader);
    m_program->link();
    m_program->bind();

    // Cache Uniform Locations
    u_modelToWorld = m_program->uniformLocation("modelToWorld");
    u_worldToCamera = m_program->uniformLocation("worldToCamera");
    u_cameraToView = m_program->uniformLocation("cameraToView");
    u_lightPosition = m_program->uniformLocation("lightPosition");
    u_difuseColor= m_program->uniformLocation("diffuseProduct");
    u_ambientColor= m_program->uniformLocation("ambientProduct");
    u_specularColor= m_program->uniformLocation("specularProduct");
    u_shininess= m_program->uniformLocation("shininess");
    m_normalAttr = m_program->attributeLocation("vNormal");

    // Create Buffer (Do not release until VAO is created)
    m_vertex.create();
    m_vertex.bind();
    m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertex.allocate(sg_vertexes_, vertexCount_*sizeof(QVector3D));

    // Create Vertex Array Object
    m_object.create();
    m_object.bind();

    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3,0);
    m_program->enableAttributeArray(0);

    // Release (unbind) all
    m_object.release();

    m_object.bind();
    m_normal.create();
    m_normal.bind();
    m_normal.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_normal.allocate(sg_normals_, normalCount_*sizeof(QVector3D));

    m_program->setAttributeBuffer(1, GL_FLOAT, 0, 3,0);
    m_program->enableAttributeArray(1);

    m_object.release();

    m_vertex.release();
    m_normal.release();
    m_program->release();
  }
}

void GLWidget::update() {
  Input::update();
  if (Input::buttonPressed(Qt::LeftButton)) {
    static const float rotSpeed   = 0.75f;
    m_transform.rotate(rotSpeed * Input::mouseDelta().x(), Camera3D::LocalUp);
    m_transform.rotate(rotSpeed * Input::mouseDelta().y(), Camera3D::LocalRight);
  }
  // Schedule a redraw
  QOpenGLWidget::update();
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (restart) {
      restart = false;
      startPath(&path, numPoints);
      createKnots(&path, snakeAttr.curveOrder);
  }
  if (!paused){
      free (verts);
      updatePath(&path);
      createDiscs(verts, path);
  }

  m_program->bind();

  m_program->setUniformValue(u_worldToCamera, m_camera.toMatrix());

  m_program->setUniformValue(u_lightPosition, vLightPosition.toVector4D());
  m_program->setUniformValue(u_cameraToView, m_projection);
  m_program->setUniformValue(u_difuseColor, difusiveColor);
  m_program->setUniformValue(u_ambientColor, ambientColor);
  m_program->setUniformValue(u_specularColor, specularColor);
  m_program->setUniformValue(u_shininess, shininess);

  {
    m_object.bind();
    m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());

    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
    m_object.release();
  }
  m_program->release();
}

void GLWidget::resizeGL(int w, int h) {
  m_projection.setToIdentity();
  m_projection.perspective(30.0f, w / float(h), 1.0f, 1000.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  Input::registerMousePress(event->button());
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  Input::registerMouseRelease(event->button());
}

void GLWidget::printContextInformation() {
  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << "Context valid: " << context()->isValid();
  qDebug() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
  qDebug() << "OpenGl information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
  qDebug() << "                    RENDERER:    " << (const char*)glGetString(GL_RENDERER);
  qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
  qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
}
