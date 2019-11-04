#ifndef MANUALCONTROL_H
#define MANUALCONTROL_H

#include <QWidget>
#include <QTimer>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>

#include "teachstate.h"
#include "MainWidget/listboxobject.h"
#include "MainWidget/comboboxobject.h"
#include "../../windowobject.h"
#include "../../stackedwidobject.h"
#include "../../commoncontrols.h"

#include "RobotClient/robotClient.h"

namespace robotTeachABB
{
    class ManuMainWidget; //stack_widget中的主窗口
    class ManualControl; //手动操纵窗口
    class LeftButton; //重写的点击属性并修改框中的显示值的按钮，使得文字左对齐显示
    class AttributeBox; //点击属性并修改框
    class PositionBox; //位置框
    class JoystickBox; //操纵杆方向框
    class ActionAttr; //动作模式属性选择窗口
    class UcsAttr; //坐标系属性选择窗口
    class LeverLockAttr; //操作杆锁定界面
    class IncrementAttr; //增量界面
    class AttributeBase; //属性窗口基类
    class VarEditInterface; //坐标、载荷等变量编辑界面
    class FormatPosition; //位置格式界面
    class ReviseUcs; //修改坐标值窗口
    class ToolErrorWindow; //标定工具坐标完成时显示的误差
    class UnitWindow; //机械单元窗口
}

class UnitWindow : public QWidget
{
    Q_OBJECT
public:
    UnitWindow(QWidget *parent = nullptr);

signals:
    void Define();
    void Cancel();

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *current_label_;
    QLabel *tips2_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;
}; //机械单元窗口

class ToolErrorWindow : public QWidget
{
    Q_OBJECT
public:
    ToolErrorWindow(QWidget *parent = nullptr);
    void HideTitle();
    void Init(QString tool_name, int way, double error, QSharedPointer<rclib::Terminal> tm);

signals:
    void CQuit();
    void Quit(QSharedPointer<rclib::Terminal> tm);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void Defint();
    void Cancel();

private:
    QSharedPointer<rclib::Terminal> terminal_point_;

    QWidget *title_widget_;
    QLabel *ico_label_;
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips_label_;
    QLabel *tips1_label_;
    QLabel *tool_name_label_;
    QLabel *tips2_label_;

    QFrame *table_widget_;
    QLabel *tips3_label_;
    QLabel *tips4_label_;
    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;
}; //标定工具坐标完成时显示的误差

class ReviseUcs : public ThreeSectionWindow
{
    Q_OBJECT
public:
    ReviseUcs(QWidget *parent = nullptr);
    virtual ~ReviseUcs();
    bool Init(QString var_name, QString tool_select = QString());

signals:
    void ModQuit(); //修改结束

private:
    void ConnectInit();

public slots:
    void Reset(); //重置
    void SetScale(double scale_w, double scale_h);

private slots:
    void RePosClicked(); //修改位置
    void DefinClicked(); //确定按钮

    void ItemClicked(QTableWidgetItem *item);
    void ToolErrorQuit(QSharedPointer<rclib::Terminal> tm); //误差窗口确定

private:
    bool woto; //真为wobjdata工件坐标，假为tooldata工具坐标
    int tab_curr_; //当前所选位置

    QList<rclib::Joints> joints_pos_;
    rclib::Joints joint_origin_;
    rclib::Joints jointx_;
    rclib::Joints jointz_;

    rclib::Terminal point_origin_;
    rclib::Terminal pointx_;
    rclib::Terminal pointy_;

    QLabel *tips_label_;
    QLabel *tips1_label_;
    QLabel *tips2_label_;
    QLabel *tips3_label_;
    QLabel *tips4_label_;
    QLabel *way_label_;
    QComboBox *way_box_; //方法
    QLabel *num_label_;
    QComboBox *num_box_; //点数

    TableBox *table_box_;

    QAction *reset_;

    ComboBoxObject *position_button_; //位置
    QPushButton *re_pos_button_; //修改位置
    QPushButton *define_button_;

    ToolErrorWindow *tool_error_window_;

public:
    QPushButton *cancel_button_;
}; //修改坐标值窗口

class FormatPosition : public QWidget
{
    Q_OBJECT
public:
    FormatPosition(QWidget *parent = nullptr);
    void Init(int pos, int direc, int angle, int unit);

signals:
    void DefineClicked(int pos, int direc, int angle, int unit);
    void CancelClicked();

private:
    QLabel *pos_label_;
    QComboBox *pos_box_; //位置显示方式
    QLabel *direc_label_;
    QComboBox *direc_box_; //方向格式
    QLabel *angle_label_;
    QComboBox *angle_box_; //角度格式
    QLabel *unit_label_;
    QComboBox *unit_box_; //角度单位

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;
}; //位置格式界面

class VarEditInterface : public ThreeSectionWindow
{
    Q_OBJECT
public:
    VarEditInterface(QWidget *parent = nullptr);
    CreateNewVar *GetCreate();
    void Init(QString data_type, QString select_str, QString var_range = QString(), bool genre = true);

private:
    void ConnectInit();

signals:
    void DefineButton(QString data_type, QString select_str);
    void CancelButton();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ShowCreateWidget(); //显示创建与修改变量界面
    void RemoveVar(); //删除变量
    void ShowReviseWidget(); //显示修改坐标值窗口
    void ShowModVarWindow(); //显示更改值窗口

private:
    CreateNewVar *create_widget_; //新建变量窗口
    ReviseUcs *revise_ucs_; //修改坐标值窗口
    ModVarWindow *mod_var_window_; //更改值窗口

    QLabel *tips_label_;
    QLabel *tips1_label_;
    QLabel *select_label_; //当前选择

    TableBox *table_box_;

    QPushButton *create_button_; //新建
    ComboBoxObject *edit_button_; //编辑
    QPushButton *define_button_; //确定
    QPushButton *cancel_button_; //取消

    QAction *change_val_; //更改值
    QAction *change_state_; //更改声明
    QAction *copy_; //复制
    QAction *del_; //删除
    QAction *defi_; //定义

private:
    QString data_type_; //数据类型
    QString var_range_;
    QSharedPointer<VarFormData> var_form_pointer_;

    bool genre_;
}; //坐标、载荷等变量编辑界面

class AttributeBase : public QWidget
{
    Q_OBJECT
public:
    AttributeBase(QWidget *parent = nullptr);
    QString GetSelectText(); //获取所选中文本

protected:
    QWidget *head_widget_;
    QFrame *core_Widget_;
    QWidget *button_widget_;

protected:
    QLabel *opt_label_; //显示已选择项
    QLabel *tips_label_;
    QLabel *tips_opt_label_; //提示选择标签
}; //属性窗口基类

class ActionAttr : public AttributeBase
{
    Q_OBJECT
public:
    ActionAttr(QWidget *parent = nullptr);
    virtual void Init(int val); //0为轴1-3, 1为轴4-6， 2为线性， 3为重定位
    virtual void Init(QString text);
    QHBoxLayout *GetImageLayout(); //为了使动作模式窗口更好看

private:
    void ConnectInit();

protected:
    virtual void SetSelect(ImageButton *button); //设置选中项

protected:
    QHBoxLayout *image_layout_;

public:
    ImageButton *week1_button_; //轴1-3
    ImageButton *week4_button_;
    ImageButton *linear_button_; //线性
    ImageButton *relocation_button_; //重定位
    QPushButton *define_button_; //确定
    QPushButton *cancel_button_; //取消
}; //动作模式属性选择窗口

class UcsAttr : public ActionAttr
{
    Q_OBJECT
public:
    UcsAttr(QWidget *parent = nullptr);
}; //坐标系属性选择窗口

class LeverLockAttr : public ActionAttr
{
    Q_OBJECT
public:
    LeverLockAttr(QWidget *parent = nullptr);
    void LeverInit(QString text); //操纵杆界面专属初始化函数，禁用基类初始化函数

private:
    void SetSelect(ImageButton *button);
}; //操作杆锁定界面

class IncrementAttr : public ActionAttr
{
    Q_OBJECT
public:
    IncrementAttr(QWidget *parent = nullptr);
    void Init(QString text);

private:
    void SetSelect(ImageButton *button);

public:
    ImageButton *user_button_; //用户
}; //增量界面

class LeftButton : public QPushButton
{
    Q_OBJECT
public:
    LeftButton(QWidget *parent = nullptr);
    LeftButton(QString name, QWidget *parent = nullptr);
};

class AttributeBox : public QGroupBox
{
    Q_OBJECT
public:
    AttributeBox(QWidget *parent = nullptr);

public:
    LeftButton *unit_button_; //机械单元按钮
    LeftButton *unitv_button_; //机械单元值按钮

    LeftButton *accuracy_button_; //绝对精度按钮
    LeftButton *accuracyv_button_; //绝对精度值按钮

    LeftButton *action_button_; //动作模式按钮
    LeftButton *actionv_button_; //动作模式值按钮

    LeftButton *ucs_button_; //坐标系按钮
    LeftButton *ucsv_button_; //坐标系值按钮

    LeftButton *tool_button_; //工具坐标按钮
    LeftButton *toolv_button_; //工具坐标值按钮

    LeftButton *work_button_; //工件坐标按钮
    LeftButton *workv_button_; //工件坐标值按钮

    LeftButton *payload_button_; //有效载荷按钮
    LeftButton *payloadv_button_; //有效载荷值按钮

    LeftButton *lever_locking_button_; //操纵杆锁定按钮
    LeftButton *lever_lockingv_button_; //操纵杆锁定值按钮

    LeftButton *increment_button_; //增量按钮
    LeftButton *incrementv_button_; //增量值按钮
};

class PositionBox : public QGroupBox
{
    Q_OBJECT
public:
    PositionBox(QWidget *parent = nullptr);

signals:
    void ShowFormat(int pos,int direc, int angle, int unit);

public slots:
    void SetFormat(int pos,int direc, int angle, int unit);
    void SetType(int type);

private slots:
    void Timer();

private:
    QTimer *timer_;

    QLabel *pos1_label_; //轴
    QLabel *pos2_label_;
    QLabel *pos3_label_;
    QLabel *pos4_label_;
    QLabel *pos5_label_;
    QLabel *pos6_label_;
    QLabel *pos7_label_;

    QLabel *posv1_label_; //轴位置
    QLabel *posv2_label_;
    QLabel *posv3_label_;
    QLabel *posv4_label_;
    QLabel *posv5_label_;
    QLabel *posv6_label_;
    QLabel *posv7_label_;
    QPushButton *format_button_; //位置格式

    int type_; //类型，0为点动,1为重定位与线性
    int pos_; //位置
    int direc_; //方向
    int angle_; //角度
    int unit_; //角度单位
};

class JoystickBox : public QGroupBox
{
    Q_OBJECT
public:
    JoystickBox(QWidget *parent = nullptr);
    void SetButton(int mod); //为按钮设置图片和文字 0为1-3， 1为4-6， 2为XYZ
    void SetLock(bool lock1, bool lock2, bool lock3); //设置锁定项
    void SetOneLock(int num, bool lock); //单独设置锁定项
    bool GetLock(int num); //获取某一个是否被锁定

private:
    ImageButton *button1_;
    ImageButton *button2_;
    ImageButton *button3_;
    int mod_;
    bool lock1_;
    bool lock2_;
    bool lock3_;
};

class ManuMainWidget : public QWidget
{
    Q_OBJECT
public:
    ManuMainWidget(QWidget *parent = nullptr);

public:
    AttributeBox *attribute_box_; //点击属性并修改框
    PositionBox *pos_box_; //位置框
    JoystickBox *joystick_box_; //操纵杆方向框
};

class ManualControl : public WindowObject
{
    Q_OBJECT
public:
    explicit ManualControl(QWidget *parent = nullptr, int type = 2);
    void resizeEvent(QResizeEvent*);
    CreateNewVar *GetCreateNewVar();

private:
    void ConnectInit();

public slots:
    void SetScale(double scale_w, double scale_h);

    void HideControl(); //隐藏控件
    void ShowControl(); //显示控件

    void ShowUnitWidget(); //显示机械单元窗口
    void ShowActionWidget(); //显示动作模式窗口
    void ShowUcsWidget(); //显示坐标系窗口
    void ShowLeverWidget(); //显示操控杆锁定界面
    void ShowIncrementWidget(); //显示增量窗口
    void ShowVarEditWidget(); //显示坐标等变量编辑窗口

private:
    StackedWidObject *stack_widget_;
    FormatPosition *format_pos_;

public:
    UnitWindow *unit_widget_; //机械单元窗口
    ActionAttr *action_widget_; //动作模式窗口
    UcsAttr *ucs_widget_; //坐标系窗口
    LeverLockAttr *lever_widget_; //操纵杆锁定窗口
    IncrementAttr *increment_widget_; //增量窗口
    VarEditInterface *var_edit_widget_; //坐标、载荷编辑窗口

public:
    ManuMainWidget *manu_widget_;
    QPushButton *aim_button_; //对准
    QPushButton *go_to_button_; //转到
    QPushButton *start_button_; //启动
};

#endif // MANUALCONTROL_H
