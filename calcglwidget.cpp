#include "calcglwidget.h"

#include <QOpenGLShaderProgram>

calcGLWidget::calcGLWidget(QWidget *parent):
    QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 3);
    setFormat(format);
}

void calcGLWidget::calculate(const QString& userCode)
{
    this->userCode = userCode;
    update();
}

void calcGLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &calcGLWidget::cleanup);

    initializeOpenGLFunctions();
    userShader = new QOpenGLShader(QOpenGLShader::Vertex);
    const char* vsrc =
        "#version 430\n"
        "out float result;\n"
        "void main() {\n"
        "   result = 777.0;\n"
        "}\n";
    if(!userShader->compileSourceCode(vsrc)) {
        emit receivedError(userShader->log());
    }

    userProgram = new QOpenGLShaderProgram;
    userProgram->addShader(userShader);
#if 1
    const char* varyings[] = {"result"};
    glTransformFeedbackVaryings
    (
        userProgram->programId(), 1, varyings, GL_INTERLEAVED_ATTRIBS
    );
#endif
    if(!userProgram->link()) {
        emit receivedError(userProgram->log());
    }

    glGenBuffers(1, &resultBuf);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, resultBuf);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnable(GL_RASTERIZER_DISCARD);
}

void calcGLWidget::paintGL() {
    if(!userShader->compileSourceCode(userCode)) {
        emit receivedError(userShader->log());
        return;
    }

    if(!userProgram->link()) {
        emit receivedError(userProgram->log());
    }

    userProgram->bind();

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, resultBuf);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, 1);
    glEndTransformFeedback();
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

    float result;

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, resultBuf);
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(result), &result);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    userProgram->release();

    emit calculated(QString::number(result));
    emit receivedError("Calculation completed without errors");
}

void calcGLWidget::cleanup() {
    makeCurrent();

    delete userProgram;
    userProgram = 0;
    delete userShader;
    userShader = 0;

    glDeleteBuffers(1, &resultBuf);
    resultBuf = 0;

    doneCurrent();
}
