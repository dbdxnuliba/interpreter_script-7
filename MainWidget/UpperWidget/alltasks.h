#ifndef ALLTASKS_H
#define ALLTASKS_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QWidget>
#include <QLabel>
#include <QBitmap>
#include <QPushButton>
#include <QListWidget>

namespace robotTeachABB
{
    class StatePushButton; //重写的按钮类，添加了状态属性
    class AllTasks; //所有任务窗口
}

class StatePushButton : public QPushButton
{
    Q_OBJECT
public:
    StatePushButton(QWidget *parent = nullptr);
    StatePushButton(QString name, QWidget *parent = nullptr);

public:
    bool state_; //状态
};

class AllTasks : public QFrame
{
    Q_OBJECT
public:
    explicit AllTasks(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void ConnectInit();

signals:

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QWidget *decorate_interface_; //装饰界面
    QLabel *decorate_label_; //装饰性标签
    QWidget *function_interface_; //功能显示界面
    QWidget *button_interface_; //按钮界面
    double scale_w_;
    double scale_h_;

public:
    StatePushButton *close_button_; //清除按钮
    StatePushButton *log_button_; //显示或不显示日志
    StatePushButton *task_button_; //显示或不显示任务
};

#endif // ALLTASKS_H
