#ifndef PROGRAMDATA_H
#define PROGRAMDATA_H

#include <QWidget>
#include <QRadioButton>

#include "../../windowobject.h"
#include "../../listboxobject.h"
#include "../../comboboxobject.h"
#include "../../commoncontrols.h"

namespace robotTeachABB
{
    class ProgramData; //程序数据窗口
    class ShowVarInfo; //用于显示变量
    class DataEditWindow; //变量编辑器
    class ModifyRanges; //修改范围窗口
}

enum class VarShowType:char{Genre_User, Genre_Sys, Mod, Rout};

class ModifyRanges : public QFrame
{
    Q_OBJECT
public:
    ModifyRanges(QWidget *parent = nullptr);
    void Refresh(); //刷新
    QString GetTask();
    QString GetMod();
    QString GetRout();
    QString GetRange();

signals:
    void BuiltIn(); //内置
    void CurrTask(); //当前执行与任务
    void Mod(QString mod_name); //模块
    void Rout(QString rout_name); //例行程序

private:
    QLabel *tips_label_;
    QRadioButton *built_in_; //仅限内置数据
    QRadioButton *current_; //当前执行
    QRadioButton *task_; //任务
    QComboBox *task_box_;
    QRadioButton *mod_; //模块
    QComboBox *mod_box_;
    QRadioButton *rout_; //例行程序
    QComboBox *rout_box_;
    QLabel *tips2_label_;
}; //修改范围窗口

class DataEditWindow : public VarEditWindow
{
    Q_OBJECT
public:
    DataEditWindow(QWidget *parent = nullptr);
    void Init(QString name, QString val, QString type);

private:
    QLabel *frame_name_label_;
    QLabel *frame_var_label_;
    QLabel *frame_type_label_;

    QLabel *data_name_;
    QLabel *data_var_;
    QLabel *data_type_;

    QString name_;
    QString var_;
    QString type_;

public:
    QPushButton *revoke_button_; //撤销
    QPushButton *determine_button_; //确定
    QPushButton *cancel_button_; //取消
}; //变量编辑器

class ShowVarInfo : public QWidget
{
    Q_OBJECT
public:
    ShowVarInfo(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void SetModi(ModifyRanges *modi);
    CreateNewVar *GetCreateWidget();
    void Init(QString &name, VarShowType vartype = VarShowType::Genre_Sys, QString str = QString());

signals:
    void VarButtonClicked(QString name, QString var, QString type);
    void ShowCreateNewVar(QString type); //显示新建变量

public slots:
    void SetScale(double scale_w, double scale_h);
    void Refresh(); //刷新
    void ItemDoubleClicked(QTableWidgetItem *item);
    void Clicked(QString name, QString var, QString type);
    void Filter(QString text); //对内容进行过滤

private slots:
    void CreateVar(); //新建变量

private:
    ModifyRanges *modi_;
    CreateNewVar *create_new_var_;
    VarShowType type_;
    QString str_;

    QLabel *tips_data_label_;
    QLabel *tips_filter_label_;
    QLabel *tips_range_label_;
    TableBox *table_box_;
    QWidget *button_widget_;
    ComboBoxObject *filter_button_;
    QPushButton *create_button_;
    ComboBoxObject *edit_button_;
    QPushButton *refresh_button_;

    QAction *delete_act_; //删除
    QAction *state_act_; //更改声明
    QAction *val_act_; //更改值
    QAction *copy_act_; //复制
    QAction *define_act_; //定义

    QString var_name_;
    QString filter_name_;
    QList<QPair<QString, VarDataInfo>> list_;

    double scale_w_, scale_h_;

public:
    QPushButton *return_button_;

    FilterKey *filter_key_; //过滤器键盘
}; //用于显示变量

class ProgramData : public WindowObject
{
    Q_OBJECT
public:
    explicit ProgramData(QWidget *parent = nullptr, int type = 5);
    void resizeEvent(QResizeEvent *);

signals:

public slots:
    void SetScale(double scale_w, double scale_h);
    void ButtonClicked(QString name);
    void HideWidget();
    void ShowMainWidget();

    void VarButtonClicked(QString name, QString var, QString type);
    void ItemClicked(QTableWidgetItem *item);

private:
    void SetDataTypeBox(QStringList list);

private:
    bool isshowall_; //是否显示所有变量
    QString range_text_;
    int type_; //所选泽的类型

    QLabel *tips_label_;
    QLabel *range_label_; //范围标签

    QWidget *show_main_widget_;
    DataEditWindow *show_edit_;

    QAction *all_an_; //全部
    QAction *used_an_; //已用

    ModifyRanges *modify_ranges_; //修改范围窗口

public:
    QPushButton *range_button_; //更改范围按钮
    QPushButton *show_data_button_; //显示数据按钮
    ComboBoxObject *view_button_; //视图按钮

    TableBox *data_type_box_; //数据显示控件
    ShowVarInfo *show_varinfo_; //显示变量
};

#endif // PROGRAMDATA_H
