#ifndef UPPERWIDGET_H
#define UPPERWIDGET_H

#include <QWidget>
#include <QPushButton>

#include "alltasks.h"
#include "eventlog.h"

namespace robotTeachABB
{
    class UpperWidget; //上栏窗口，是mainwidget功能主窗口的一个子窗口，此窗口存放着菜单窗口、任务窗口、事件日志窗口的入口
    class StateButton; //状态栏按钮
}

class StateButton : public QFrame
{
    Q_OBJECT
public:
    StateButton(QWidget *parent = nullptr);

public slots:
    void SetMod(QString mod); //设置模式
    void SetState(QString state); //设置电机状态
    void SetRunState(bool state); //设置运行状态
    void SetSpeed(int speed); //设置速度

signals:
    void clicked();

private:
    void mousePressEvent(QMouseEvent *event);
    void Init();

private:
    QLabel *mod_label_; //模式，手动还是自动
    QLabel *state_label_; //电机状态
    QLabel *image_label_; //图片
    QLabel *tips_label_; //提示
    QLabel *run_state_label_; //运行状态
    QLabel *speed_label_; //速度
};

class UpperWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UpperWidget(QWidget *parent = nullptr);
    void ButtonInit();

signals:

public slots:
    void SetScale(double scale_w, double scale_h);

public:
    QPushButton *menu_button_; //菜单按钮
    QPushButton *task_button_; //任务按钮
    StateButton *state_button_; //状态栏按钮
    QPushButton *close_button_; //关闭按钮
};

#endif // UPPERWIDGET_H
