#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_GLSLTextEdit_textChanged();

    ui->GLSLTextEdit->setFocus();
    ui->GLSLTextEdit->find(QRegExp("result\\s+=[^;]*;"));
    QTextCursor cursor = ui->GLSLTextEdit->textCursor();
    cursor.clearSelection();
    const auto pos = cursor.position();
    cursor.setPosition(pos > 1 ? pos-2 : pos);
    ui->GLSLTextEdit->setTextCursor(cursor);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_GLSLTextEdit_textChanged()
{
    ui->glWidget->calculate(ui->GLSLTextEdit->toPlainText());
}

void MainWindow::on_typeComboBox_currentIndexChanged(const QString &arg1)
{
    QString userCode = ui->GLSLTextEdit->toPlainText();
    userCode.replace(QRegExp("out\\s+\\w+\\s+result\\s*;"), "out " + arg1 + " result;");
    userCode.replace(QRegExp("\\w+\\s+x\\s*=\\s*gl_VertexID\\s*;"), arg1 + " x = gl_VertexID;");
    ui->GLSLTextEdit->setPlainText(userCode);
}
