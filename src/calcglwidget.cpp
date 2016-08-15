#include "calcglwidget.h"

#include <QOpenGLShaderProgram>

calcGLWidget::calcGLWidget(QWidget *parent):
    QOpenGLWidget(parent), resultVec(1)
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

void calcGLWidget::setVectorLength(int vectorLength) {
    resultVec.resize(vectorLength);
    update();
}

void calcGLWidget::setResultType(const QString& resultType) {
    this->resultType = resultType;

    update();
}

void calcGLWidget::setResultBase(const QString& resultBase) {
    this->resultBase = resultBase.toInt();

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

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnable(GL_RASTERIZER_DISCARD);
}

void calcGLWidget::paintGL() {
    if(resultVec.empty())
        return;

    if(!userShader->compileSourceCode(userCode)) {
        emit receivedError(userShader->log());
        return;
    }

    if(!userProgram->link()) {
        emit receivedError(userProgram->log());
    }

    userProgram->bind();

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, resultBuf);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float)*resultVec.size(), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, resultBuf);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, resultVec.size());
    glEndTransformFeedback();
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, resultBuf);
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(T32)*resultVec.size(), resultVec.data());
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    userProgram->release();

    QString resultText;

    if(resultType == "float") {
        for(auto res : resultVec){
            resultText += QString::number(res.f) + "\n";
        }
    }else if(resultBase < 2 || resultBase > 36) {
        resultText = "Unsupported base";
    } else if(resultType == "int") {
        for(auto res : resultVec){
            resultText += QString::number(res.i, resultBase) + "\n";
        }
    }else if(resultType == "uint") {
        for(auto res : resultVec){
            resultText += QString::number(res.u, resultBase) + "\n";
        }
    }else{
        resultText = "Unknown type is selected";
    }
    emit calculated(resultText);
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

