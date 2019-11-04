#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QMouseEvent>

#include "Hardware/hardware.h"
#include "MainWidget/mainwidget.h"

namespace robotTeachABB
{
    class Widget; //示教器主窗口，负责提供一个窗口用来显示mainwidget软件主窗口和hardware硬件主窗口，同时也是这两个主窗口之间数据通信的桥梁
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);

signals:
    void SetScale(double scale_w, double scale_h);
    void RunProgram();
    void LeftStepRunProgram();
    void RightStepRunProgram();
    void ProgramStart();

public slots:
    void Adapt(); //根据屏幕宽高进行自适应

private slots:
    //操控机器人
    void HardwareEnabled(); //使能按钮
    void Stop(); //停止操控
    void HardwareUpper(); //上
    void HardwareLower(); //下
    void HardwareLeft(); //左
    void HardwareRight(); //右
    void HardwareLevo(); //左旋
    void HardwareCumSole(); //右旋

    //快捷键
    void PK1Clicked();
    void PK2Clicked();
    void PK3Clicked();
    void PK4Clicked();
    void PKRelease();

    //单步
    void LeftStep();
    void RightStep();

private:
    void SaveData();  //保存数据，清除临时文件夹
    void ConnectInit(); //初始化信号与槽
    void Run(bool isok, QSharedPointer<QString> code); //执行程序
    void LeftStepRun(bool isok, QSharedPointer<QString> code); //单步执行程序
    void RightStepRun(bool isok, QSharedPointer<QString> code);

    void ProgramRun(); //程序运行
    void ProgramStop(); //程序停止
    void ProgramLeftStepRun(); //单步
    void ProgramRightStepRun(); //单步

private:
    QPoint point_;
    Hardware *hardware_;
    MainWidget *main_widget_;
    bool ismove_; //用于判断是否可以移动，为真时可移动，当窗口处于最大化时为假否则为真
    bool issteprun_; //是否为单步运行
    int width_; //保存主窗口与硬件窗口同时存在时的宽度用于恢复宽度
    double scale_w_; //等比例宽
    double scale_h_; //等比例高
};

#endif // WIDGET_H
