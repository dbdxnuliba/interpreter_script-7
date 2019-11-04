#ifndef LOWERWIDGET_H
#define LOWERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QVector>
#include <QGroupBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QStackedWidget>
#include <QGridLayout>
#include <QPushButton>

#include "teachstate.h"
#include "MainWidget/commoncontrols.h"

namespace robotTeachABB
{
    class SaveButton; //保存被用来与打开的某一个功能窗口相关联的按钮
    class LowerWidget; //下栏窗口，是mainwidget软件主窗口的一个控件
    class QuickButton; //快捷设置显示按钮
    class QuickWidget; //快捷设置窗口
    class TaskQuickWidget; //快捷设置窗口中的任务窗口
    class SpeedWidget; //快捷设置窗口中的速度窗口
    class StepModWidget; //快捷设置窗口中的步进模式窗口
    class RunModWidget; //快捷设置窗口中的运行模式
    class QuickBase; //快捷设置窗口中所有子窗口的基类
    class IncrementWidget; //快捷设置窗口中的增量窗口
    class UnitQuickWidget; //快捷设置窗口中的快捷单元窗口
    class UnitMinWidget; //快捷单元窗口中的最小化窗口
    class UnitMaxWidget; //快捷单元窗口中的最大化窗口
    class SelectCordas; //选择当前工具或工件坐标
    class UnitMaxLower; //UnitMaxWidget窗口中的下栏窗口
}

class QuickBase : public QFrame
{
    Q_OBJECT
public:
    QuickBase(QWidget *parent = nullptr);

signals:
    void HideWindow();
    void ShowWindow();

private:
    void hideEvent(QHideEvent *);
    void showEvent(QShowEvent *);
}; //快捷设置窗口中所有子窗口的基类


class UnitQuickWidget;
class SelectCordas : public QWidget
{
    Q_OBJECT
public:
    SelectCordas(QWidget *parent = nullptr);
    void Init(QString type);

signals:
    void Close();

private slots:
    void ItemClicked(QTableWidgetItem *item);

private:
    void ConnectInit();

private:
    QGroupBox *curr_group_;
    QLabel *curr_label_;
    QPushButton *close_button_;

    QTableWidget *table_box_;

private:
    int type_;
}; //选择当前工具或工件坐标

class UnitMaxLower : public QFrame
{
    Q_OBJECT
public:
    UnitMaxLower(double *scale_w, double *scale_h, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);

private:
    void ConnectInit();

public:
    QFrame *speed_widget_;
    QLabel *spico_label_;
    QLabel *sp_label_;
    QPushButton *add_button_;
    QPushButton *red_button_;

    QWidget *button_widget_;
    ImageButton *wear_button_;
    ImageButton *wbas_button_;
    ImageButton *wtoo_button_;
    ImageButton *wwor_button_;
    ImageButton *wek1_button_;
    ImageButton *wek4_button_;
    ImageButton *line_button_;
    ImageButton *relo_button_;

    QPushButton *incremen_label_;

    QPushButton *hide_left_;

public:
    double *scale_w_, *scale_h_;
    int speed_;
}; //UnitMaxWidget窗口中的下栏窗口

class UnitMaxWidget : public QWidget
{
    Q_OBJECT
public:
    UnitMaxWidget(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void Init();

signals:
    void Change();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ToolClicked();
    void WorkClicked();
    void ImageClicked();

private:
    void ConnectInit();

private:
    friend class UnitQuickWidget;

    QLabel *ico_label_;

    QFrame *fm_widget_;
    QLabel *title_label_;
    QPushButton *tool_button_;
    QPushButton *work_button_;

    UnitMaxLower *lower_widget_;

    UnitQuickWidget *parent_point;

    SelectCordas *select_cordas_;

private:
    double scale_w_, scale_h_;
}; //快捷单元窗口中的最大化窗口

class UnitMinWidget : public QWidget
{
    Q_OBJECT
public:
    UnitMinWidget(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void Init();

signals:
    void Change();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ActionClicked();
    void CordssClicked();
    void ShowDetailsClicked();
    void ToolClicked();
    void WorkClicked();

private:
    void ConnectInit();

private:
    friend class UnitQuickWidget;

    QLabel *ico_label_;

    QFrame *fm_widget_;
    QLabel *title_label_;
    QPushButton *action_button;
    QPushButton *cordss_button;
    QPushButton *tool_button_;
    QPushButton *work_button_;

    QPushButton *show_left_;

    UnitQuickWidget *parent_point;

    QWidget *tool_widget_;
    QPushButton *tclose_button_;
    ImageButton *wek1_button_;
    ImageButton *wek4_button_;
    ImageButton *line_button_;
    ImageButton *relo_button_;

    QWidget *work_widget_;
    QPushButton *wclose_button_;
    ImageButton *wear_button_;
    ImageButton *wbas_button_;
    ImageButton *wtoo_button_;
    ImageButton *wwor_button_;

    SelectCordas *select_cordas_;

private:
    double scale_w_, scale_h_;
}; //快捷单元窗口中的最小化窗口

class UnitQuickWidget : public QuickBase
{
    Q_OBJECT
public:
    UnitQuickWidget(QWidget *parent = nullptr);

signals:
    void ModSize(int width, int height);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void HideWidget(); //当窗口隐藏时恢复内部的设置
    void ShowWidget(); //同HideWidget

private:
    void ConnectInit();

private:
    QStackedWidget *stack_widget_;

    UnitMinWidget *min_widget_;
    UnitMaxWidget *max_widget_;
}; //快捷设置窗口中的快捷单元窗口

class IncrementWidget : public QuickBase
{
    Q_OBJECT
public:
    IncrementWidget(QWidget *parent = nullptr);

signals:
    void ShowWidgetSize(int width, int height);

private slots:
    void ItemClicked(QTableWidgetItem *item);
    void NumDefine();
    void NumCancel();

private:
    void ConnectInit();
    void Clicked(int button);
    void SetTableData(int type = 0);

private:
    QWidget *right_widget_;
    QLabel *tips_label_;
    ImageButton *none_button_;
    ImageButton *small_button_;
    ImageButton *centre_button_;
    ImageButton *large_button_;
    ImageButton *user_button_;
    QPushButton *show_val_;

    QWidget *left_widget_;
    QLabel *tips1_label_;
    QTableWidget *table_widget;
    QFrame *key_widget_;
    QLabel *tips_min_label_;
    QLabel *min_label_;
    QLabel *tips_max_label_;
    QLabel *max_label_;
    NumKey *num_key_;

    QTableWidgetItem *old_item_;
    QPointer<QLineEdit> num_edit_;
}; //快捷设置窗口中的增量窗口

class RunModWidget : public QuickBase
{
    Q_OBJECT
public:
    RunModWidget(QWidget *parent = nullptr);

public:
    QPushButton *one_button_;
    QLabel *one_label_;
    QPushButton *series_button_;
    QLabel *series_label_;

private:
    QLabel *tips_label_;
}; //快捷设置窗口中的运行模式

class StepModWidget : public QuickBase
{
    Q_OBJECT
public:
    StepModWidget(QWidget *parent = nullptr);

public:
    QPushButton *join_button_;
    QLabel *join_label_;
    QPushButton *out_button_;
    QLabel *out_label_;
    QPushButton *skip_button_;
    QLabel *skip_label_;
    QPushButton *lower_button_;
    QLabel *lower_label_;

private:
    QLabel *tips_label_;
}; //快捷设置窗口中的步进模式窗口

class SpeedWidget : public QuickBase
{
    Q_OBJECT
public:
    SpeedWidget(QWidget *parent = nullptr);
    double GetSpeed(); //获取浮点数形式的速度

signals:
    void SpeedChange(int speed); //速度发生变化

public:
    int speed_; //速度百分比

private:
     QLabel *tips_label_;
     QLabel *speed_label_;
     QPushButton  *cut1_button_; //减1
     QPushButton *add1_button_; //加1
     QPushButton *cut5_button_; //减5
     QPushButton *add5_button_; //加5
     QPushButton *set0_button_;
     QPushButton *set25_button_;
     QPushButton *set50_button_;
     QPushButton *set100_button_;
}; //快捷设置窗口中的速度窗口

class TaskQuickWidget : public QuickBase
{
    Q_OBJECT
public:
    TaskQuickWidget(QWidget *parent = nullptr);

public:
    QCheckBox *button_;
    QLabel *label_;

private:
    QLabel *tips_label_;
    QLabel *tips1_label_;
    QLabel *tips2_label_;
}; //快捷设置窗口中的任务窗口

class QuickWidget : public QFrame
{
    Q_OBJECT
public:
    QuickWidget(QWidget *parent = nullptr);
    void hideEvent(QHideEvent*);
    void showEvent(QShowEvent*);

public slots:
    void Incrent(Increment ict);

private:
    void ConnectInit();

signals:
    void HideWindow();
    void ShowWindow();

public:
    QPushButton increment_button_; //增量
    QPushButton run_mod_button_; //运行模式
    QPushButton speed_button_; //速度
    QPushButton step_mod_button_; //步进模式
    QPushButton task_button_; //任务
    QPushButton unit_button_; //机械单元
}; //快捷设置窗口

class QuickButton : public QPushButton
{
    Q_OBJECT
public:
    QuickButton(QWidget *parent = nullptr);

public slots:
    void ActionMod(ActionMode mod);
    void Incrent(Increment ict);

public:
    QLabel *tips_unit_label_; //机械单元
    QLabel *tips_action_label_; //动作模式
    QLabel *tips_increment_label_; //增量
}; //快捷设置显示按钮

class SaveButton : public QWidget
{
    Q_OBJECT
public:
    SaveButton(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void ShowButton(); //刷新显示，每次添加或删除按钮后都要调用
    bool IsExist(QPushButton *button); //判断是否已经存在
    bool AddButton(QPushButton *button); //添加按钮
    void RemButton(QPushButton *button); //删除按钮
    int GetSize(); //获取总共显示的数量

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QVector<QPushButton*> show_order_; //显示的按钮
    double scale_w_;
    double scale_h_;

public:
    QPushButton *hotedit_; //hotedit
    QPushButton *int_out_; //输入输出
    QPushButton *manual_control_; //手动操纵
    QPushButton *auto_produce_; //自动生产
    QPushButton *program_Editor_; //程序编辑器
    QPushButton *program_data_; //程序数据
    QPushButton *backup_recovery_; //备份与恢复
    QPushButton *calibration_; //校准
    QPushButton *control_panel_; //控制面板
    QPushButton *file_manager_; //FlexPendant资源管理器
    QPushButton *system_info_; //系统信息
};

class LowerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LowerWidget(QWidget *parent = nullptr);
    void Init();
    void resizeEvent(QResizeEvent *);

signals:

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    double scale_w_; //等比例宽
    double scale_h_; //等比例高

public:
    SaveButton *save_button; //以按钮的形式保存打开的功能
    QuickButton *quick_button_; //快捷按钮
};

#endif // LOWERWIDGET_H
