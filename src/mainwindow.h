#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "sqldata.h"
#include "dialogdetail.h"

#include <QMainWindow>
#include <QTimer>
#include <QPoint>
#include <QMenu>
#include <QDateTime>

#include "webserver.h"

namespace Ui {
class MainWindow;
}

enum TomatoClockState{TCS_NORMAL, ///< 正常态，开启后就进入
                      TCS_RUN,  ///< 计时态，开始进入始终时间
                      TCS_PAUSE, ///< 打扰态，时钟被中断
                      TCS_MAX}; ///< 最大值，用于判断参数

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void updateUI();
    void updateUINum();
    QString calcTimeStrForMin(int time);
    void setLCD(int time);
    void timesUp();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void closeWin();
private:
    void startRun();
    void stopRun(bool normalStop);
    void pauseRun();
    void resumeRun();
private:
    Ui::MainWindow *ui;
    volatile int m_iSecs; ///< 时长计数
    volatile int m_iFlash; ///< 刷新计数 时长计数的2倍 500ms刷新一次
    QTimer *m_timer;    ///< 时长定时器 1s每次调用
    QTimer *m_timerFlash; ///< 刷新定时器 500ms每次调用
    QPoint pos; ///< 窗口位置
    QMenu *m_menuRight; ///< 关闭菜单
    TomatoClockState m_tcsCur; ///< 当前状态
    bool m_bIsPause; ///< 是否处于被打扰状态
    int m_iPauseTime; ///< 暂停次数
    int m_iPauseSecs; ///< 从暂停开始计数，秒数
    std::unique_ptr<sqlTomatoRecordData> m_pDbMgr; ///< 数据库操作对象
    std::unique_ptr<sqlSystemConfigData> m_pDBSysCfg; ///<
    DialogDetail *m_winDetail; ///< 描述输入对话框
    int m_iCurrentTomatoId; ///< 当前Tomato的ID
    QDateTime m_dtPauseBegin; ///< 当前暂停的开始时间

    std::unique_ptr<WebServer> m_pWebServer;

    // QWidget interface
protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
