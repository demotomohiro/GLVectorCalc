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
    void setVectorLength(int vectorLength);
    void setResultType(const QString& resultType);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private slots:
    void cleanup();

private:
    union T32 {
        float           f;
        int             i;
        unsigned int    u;
    };

    QOpenGLShader*          userShader;
    QOpenGLShaderProgram*   userProgram;
    GLuint                  resultBuf       = 0;
    QString                 userCode;
    QVector<T32>            resultVec;
    QString                 resultType{"float"};
};

#endif // CALCGLWIDGET_H
