#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_GLSLTextEdit_textChanged();
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
    ui->GLSLTextEdit->setPlainText(userCode);
}
