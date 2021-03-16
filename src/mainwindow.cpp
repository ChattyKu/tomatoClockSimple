#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QMouseEvent>
#include <QPoint>

const int g_iClockTime = 25; ///< 计时器时间长度（分钟）
const int g_iMaxPauseSecs = 180; ///< 打扰最长时间 3分钟，超过则时钟被重置

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_tcsCur(TCS_NORMAL), m_pDbMgr(createDbMgr()),
    m_iCurrentTomatoId(-1), m_pWebServer(new WebServer())
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowFlag(Qt::FramelessWindowHint);
    ui->lcdMinTimer->setDecMode();
    ui->lcdMinTimer->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdSecTimer->setDecMode();
    ui->lcdSecTimer->setSegmentStyle(QLCDNumber::Flat);
    m_timer = new QTimer(this);
    m_timerFlash = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateUI()));
    connect(m_timerFlash, SIGNAL(timeout()), this, SLOT(updateUINum()));

    m_menuRight = new QMenu();
    QAction *clo = m_menuRight->addAction("关闭");
    connect(clo, SIGNAL(triggered(void)), this, SLOT(closeWin()));

    m_winDetail = new DialogDetail();

    m_pWebServer->run();

    m_pDBSysCfg = createSystemConfig();
    QPoint pos = {0, 0};
    m_pDBSysCfg->getStartPos(pos);

    if (pos.x() != 0 || pos.y() != 0) {
        this->move(pos);
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}
// 点击按钮
// 1. 开始计数
// 2. 打扰
// 3. 打扰恢复
void MainWindow::on_pushButton_clicked()
{
    switch (m_tcsCur) {
        case TCS_NORMAL:
            startRun();
            break;
        case TCS_RUN:
            pauseRun();
            break;
        case TCS_PAUSE:
            resumeRun();
            break;
        default:
            qDebug() << "state is normal!";
            stopRun(false);
            break;
    }
}

void MainWindow::updateUI()
{
    if (m_bIsPause) {
        ++m_iPauseSecs;
        ui->pushButton->setText("解打扰(" + QString("%1").arg(g_iMaxPauseSecs - m_iPauseSecs) + ")");
        if (m_iPauseSecs >= g_iMaxPauseSecs) {
            stopRun(false);
        }
        return;
    }
    if (m_iSecs > 0) {
        --m_iSecs;
    }
    if (m_iSecs == 0) { ///< 计时器时间到
        stopRun(true);
        timesUp();
    }
    // qDebug() << "timer run:" << m_iSecs << "," << calcTimeStrForMin(m_iSecs);
}

void MainWindow::updateUINum()
{
    if (m_iFlash > 0) {
        --m_iFlash;
    }
    setLCD(m_iSecs);
}

// 产生分秒时间的显示字符串
QString MainWindow::calcTimeStrForMin(int time)
{
    int min = time / 60;
    int sec = time % 60;
    if (m_iFlash % 2) {
        return QString("%01 %02").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
    } else {
        return QString("%01:%02").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
    }

}

void MainWindow::setLCD(int time)
{
    ui->lcdSecTimer->display(time);
    ui->lcdMinTimer->display(calcTimeStrForMin(time));
}

void MainWindow::timesUp()
{
    QMessageBox msg;
    msg.setWindowFlag(Qt::WindowStaysOnTopHint);
    msg.setText("Times up!");
    msg.exec();
    qDebug() << "times up:";
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - pos);
        event->accept();
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    QCursor cur = this->cursor();
    m_menuRight->exec(cur.pos());
}

void MainWindow::startRun()
{
    m_winDetail->DetailExec();
    QString detail = m_winDetail->getDetail();
    m_iCurrentTomatoId = m_pDbMgr->createTomatoRecord(detail);
    m_iSecs = g_iClockTime * 60;
    m_iFlash = m_iSecs * 2;
    m_timer->start(1000);
    m_timerFlash->start(500);
    m_bIsPause = false;
    m_tcsCur = TCS_RUN;
    setLCD(m_iSecs);
    m_iPauseTime = 3;
    ui->pushButton->setText("被打扰(" + QString("%1").arg(m_iPauseTime) + ")");
}

void MainWindow::stopRun(bool normalStop)
{
    m_timer->stop();
    m_timerFlash->stop();
    m_tcsCur = TCS_NORMAL;
    m_bIsPause = false;
    m_iSecs = 0;
    m_iFlash = 0;
    ui->pushButton->setText("开始计时");
    setLCD(m_iSecs);
    m_pDbMgr->updateTomatoState(m_iCurrentTomatoId, normalStop);

    m_iCurrentTomatoId = -1;

}
// 运行状态暂停
void MainWindow::pauseRun()
{
    if (m_iPauseTime <= 1) {
        m_pDbMgr->updateTomatoPause(m_iCurrentTomatoId, 3,
                                    QDateTime::currentDateTime().addSecs(-1).toString("yyyy-MM-dd HH:mm:ss"), "");
        stopRun(false);
        return;
    }
    m_dtPauseBegin = QDateTime::currentDateTime();
    m_bIsPause = true;
    m_tcsCur = TCS_PAUSE;
    m_iPauseSecs = 0;
    ui->pushButton->setText("解打扰(" + QString("%1").arg(g_iMaxPauseSecs - m_iPauseSecs) + ")");
    --m_iPauseTime;
}
// 运行状态恢复
void MainWindow::resumeRun()
{
    m_winDetail->ResumeExec();
    QString str = m_winDetail->getDetail();
    if (m_iCurrentTomatoId > 0) {
        m_pDbMgr->updateTomatoPause(m_iCurrentTomatoId, -m_iPauseTime + 3,
                                    m_dtPauseBegin.toString("yyyy-MM-dd HH:mm:ss"), str);
    }

    m_bIsPause = false;
    m_tcsCur = TCS_RUN;
    ui->pushButton->setText("被打扰(" + QString("%1").arg(m_iPauseTime) + ")");
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_pDBSysCfg->setStartPos(this->frameGeometry().topLeft());
    }
}

void MainWindow::closeWin()
{
    if (m_iCurrentTomatoId > 0 ) {
        stopRun(false);
    }
    close();
}
