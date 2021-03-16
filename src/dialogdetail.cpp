#include "dialogdetail.h"
#include "ui_dialogdetail.h"
#include <QtDebug>
#include <QMouseEvent>

DialogDetail::DialogDetail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDetail)
{
    ui->setupUi(this);


    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowFlag(Qt::FramelessWindowHint);
}

DialogDetail::~DialogDetail()
{
    delete ui;
}

QString DialogDetail::getDetail()
{
    return ui->lineEdit->text();
}

void DialogDetail::DetailExec()
{
    ui->label->setText("目标描述：");
    ui->pushButton->setText("确认目标");
    this->exec();
}

void DialogDetail::ResumeExec()
{
    ui->label->setText("打扰原因：");
    ui->pushButton->setText("确认原因");
    this->exec();
}



void DialogDetail::on_pushButton_clicked()
{
    close();
}

void DialogDetail::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}


void DialogDetail::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - pos);
        event->accept();
    }
}
