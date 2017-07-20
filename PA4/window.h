#ifndef WINDOW_H
#define WINDOW_H

#include <QLabel>
#include <QWidget>
#include <QString>
#include <QCheckBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QRadioButton>

QT_BEGIN_NAMESPACE
class QSlider;
class QPushButton;
QT_END_NAMESPACE

class GLWidget;
class MainWindow;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(MainWindow *mw, int step, int shininess);
    void connectWidgetUpdate();
    void disconnectWidgetUpdate();
    void load(int indSize, int vertCount, uint *inds, QVector3D *verts, QVector3D *norms, QVector3D *tex);
    void loadTexture (QString file);

    // Ambient Color Sliders
    QSlider *redAmbientSlider;
    QSlider *greenAmbientSlider;
    QSlider *blueAmbientSlider;
    QLabel *redAmbientLabel;
    QLabel *greenAmbientLabel;
    QLabel *blueAmbientLabel;

private slots:
    void valueBMP(int);
    void valueDSP(int);
    void valueCTRL(int);
    void valueORD(int);

    void valueRA(int);
    void valueGA(int);
    void valueBA(int);

    void valueRD(int);
    void valueGD(int);
    void valueBD(int);

    void valueRS(int);
    void valueGS(int);
    void valueBS(int);

    void valueSh(int);

    void handlePauseButton(bool);
    void handleRestartButton(bool);

private:
    QSlider *createSlider();
    QSlider *createShininessSlider();

    int step_;
    int shininess_;
    GLWidget *glWidget;
    MainWindow *mainWindow;

    // Sliders
    QSlider *shininessSlider;
    QSlider *ambientColorSlider;
    QSlider *specularColorSlider;
    QSlider *diffusiveColorSlider;


    // Diffusive Color Sliders
    QSlider *redDiffusiveSlider;
    QSlider *greenDiffusiveSlider;
    QSlider *blueDiffusiveSlider;
    QLabel *redDiffusiveLabel;
    QLabel *greenDiffusiveLabel;
    QLabel *blueDiffusiveLabel;


    // Specular Color Sliders
    QSlider *redSpecularSlider;
    QSlider *greenSpecularSlider;
    QSlider *blueSpecularSlider;
    QLabel *redSpecularLabel;
    QLabel *greenSpecularLabel;
    QLabel *blueSpecularLabel;

    QSlider *bumpSlider;
    QLabel *bumpLabel;

    QSlider *displacementSlider;
    QLabel *displacementLabel;

    QSlider *controlSlider;
    QLabel *controlLabel;

    QSlider *orderSlider;
    QLabel *orderLabel;

    // Labels
    QLabel *shininessLabel;
    QLabel *ambientColorLabel;
    QLabel *specularColorLabel;
    QLabel *diffusiveColorLabel;

    QPushButton *createPauseButton();
    QPushButton *createRestartButton();
    QWidget* createShininess();
    QWidget* createBumpness();
    QWidget* createControl();
    QWidget* createOrder();
    QWidget* createDisplacement();
    QWidget* createAmbientColors();
    QWidget* createSpecularColors();
    QWidget* createDiffusiveColors();
    QTabWidget* createColorSliders();
    QTabWidget* createBumpSlider();
    QTabWidget* createShininessSliders();
    QTabWidget* createControlSlider();
    QTabWidget* createOrderSlider();
    QTabWidget* createDisplacementSlider();
};

#endif
