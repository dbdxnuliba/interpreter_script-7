#ifndef HARDWARE_H
#define HARDWARE_H

#include "teachstate.h"
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QBitmap>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QMouseEvent>
#include <QPushButton>
#include <QFrame>

namespace robotTeachABB
{
    class Hardware; //硬件主窗口，使用软件模拟出的真实示教器的硬件部分
    class HardwareButton; //操控按钮
    class ElectMachWindow; //控制电机开关
}

class ElectMachWindow : public QFrame
{
    Q_OBJECT
public:
    ElectMachWindow(QWidget *parent);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ConnectInit();
    void TimerOut();
    void ButtonClicked();
    void AutoRadio();
    void ManuRadio();
    void MaxmRadio();

private:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    QPushButton *button_;
    QRadioButton *auto_radio_; //自动
    QRadioButton *manual_radio; //手动
    QRadioButton *max_manual_radio_; //全速手动
    QRadioButton *old_radio_; //上一次模式，用于切换模式失败时避免选中

    QTimer *timer_;

    double scale_w_;
    double scale_h_;
}; //控制电机开关

class HardwareButton : public QPushButton
{
    Q_OBJECT
public:
    HardwareButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *button);
    void mouseReleaseEvent(QMouseEvent *button);

signals:
    void Press();
    void Release();

public:
    static bool issend_; //为真时代表已向服务端发送操控指令
}; //操控按钮

class ProgKeysButton : public QPushButton
{
    Q_OBJECT
public:
    ProgKeysButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void setPORX(int px);
    int GetPORX();

signals:
    void Press();
    void Release();

private:
    int px_;
};

class Hardware : public QWidget
{
    Q_OBJECT
public:
    explicit Hardware(QWidget *parent = nullptr, int model = 0);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ShowElectMach();

    void StopClicked(bool stop);
    void EnabledClicked(bool ebl);

private:
    void ButtonInit(); //对各个按钮进行初始化

private:
    int model_; //示教器界面类型
    QFrame *fm_; //用于显示背景图片
    double scale_w_; //等比例宽
    double scale_h_; //等比例高

public:
    QPushButton *zoom_widget_; //缩放
    QPushButton *show_widget_; //显示
    QPushButton *hide_widget_; //隐藏
    QPushButton *minimize_widget_; //最小化
    QPushButton *close_widget_; //关闭
    QPushButton *switch_model_; //切换示教器类型
    QPushButton *emergency_stop_; //紧急停止
    QPushButton *enabled_;
    QPushButton *hold_to_run_;
    HardwareButton *upper_; //上
    HardwareButton *lower_; //下
    HardwareButton *left_; //左
    HardwareButton *right_; //右
    QPushButton *left_upper_; //左上
    QPushButton *left_lower_; //左下
    QPushButton *right_upper_; //右上
    QPushButton *right_lower_; //右下
    HardwareButton *levo_; //左旋
    HardwareButton *cum_sole_; //右旋
    ProgKeysButton *custom_key1_1_; //自定义按键一类
    ProgKeysButton *custom_key2_1_;
    ProgKeysButton *custom_key3_1_;
    ProgKeysButton *custom_key4_1_;
    QPushButton *mechanical_unit_; //选择机械模式
    QPushButton *linear_relocate_; //选择线性与重定位运动
    QPushButton *select_axis_; //选择轴
    QPushButton *cut_spike_; //切换增量
    ProgKeysButton *custom_key1_2_; //自定义按键二类
    ProgKeysButton *custom_key2_2_;
    ProgKeysButton *custom_key3_2_;
    ProgKeysButton *custom_key4_2_;
    QPushButton *start_execution_1_; //开始执行一类
    QPushButton *stop_execution_1_; //停止执行一类
    QPushButton *step_left_1_; //左步进一类
    QPushButton *right_left_1_; //右步进一类
    QPushButton *start_execution_2_; //开始执行二类
    QPushButton *stop_execution_2_; //停止执行二类
    QPushButton *step_left_2_; //左步进二类
    QPushButton *right_left_2_; //右步进二类
    QPushButton *mode_select_1_; //模式选择一类
    QPushButton *mode_select_2_; //模式选择二类

    ElectMachWindow *elect_mach_; //控制电机状态
};

#endif // HARDWARE_H
