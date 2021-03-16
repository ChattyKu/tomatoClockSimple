#ifndef DIALOGDETAIL_H
#define DIALOGDETAIL_H

#include <QDialog>
#include <QPoint>

namespace Ui {
class DialogDetail;
}

class DialogDetail : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDetail(QWidget *parent = nullptr);
    ~DialogDetail();
    QString getDetail();
    void DetailExec();
    void ResumeExec();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogDetail *ui;
    QPoint pos;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // DIALOGDETAIL_H
