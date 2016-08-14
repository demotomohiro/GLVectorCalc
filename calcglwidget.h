#ifndef CALCGLWIDGET_H
#define CALCGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>
#include <QVector>

QT_FORWARD_DECLARE_CLASS(QOpenGLShader)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class calcGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    calcGLWidget(QWidget *parent = Q_NULLPTR);

signals:
    void calculated(const QString& result);
    void receivedError(const QString& errMsg);

public slots:
    void calculate(const QString& userCode);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private slots:
    void cleanup();
    void setVectorLength(int vectorLength);

private:
    QOpenGLShader*          userShader;
    QOpenGLShaderProgram*   userProgram;
    GLuint                  resultBuf       = 0;
    QString                 userCode;
    QVector<float>          resultVec;
};

#endif // CALCGLWIDGET_H
