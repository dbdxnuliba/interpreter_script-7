#ifndef PROGRAMEDITOR_H
#define PROGRAMEDITOR_H

#include <QWidget>

#include "progeditmain.h"

namespace robotTeachABB
{
    class ProgramEditor; //程序编辑器功能
}

class ProgramEditor : public WindowObject
{
    Q_OBJECT
public:
    explicit ProgramEditor(QWidget *parent = nullptr, int type = 4);
    void resizeEvent(QResizeEvent *);
    void HideWidget(); //隐藏图标和标题等控件

signals:

public slots:
    void SetScale(double scale_w, double scale_h);
    void ShowWidget(); //显示图标和标题等控件
    void ShowRoutine(); //根据模块界面所选项显示例行程序窗口

    void ShowInsertExpres(QString data, QString type, bool lerg, bool rep); //显示插入变量窗口
    void ShowAddFunc(); //显示添加函数窗口

private:
    void Init(); //初始化，从文件中读取出模块、例行程序、变量等信息
    void ConnectInit();
    void ReadRoutine(); //根据模块所选项打开文件读取分析出例行程序
    void ChangeRoutine(); //根据例行程序界面所选项切换代码框所显示位置

private:
    StackedWidObject *stack_widget_;

    InsertExpresWindow *insert_expres_; //插入表达式窗口
    ChangeDataType *change_datatype_; //更改数据类型窗口
    CreateNewVar *create_newvar_; //新建变量窗口
    AddFunc *add_func_; //添加函数窗口

public:
    ProgEditMain *edit_main_; //主窗口
    TaskOrder *task_order_; //任务
    Modular *modular_; //模块
    Routine *routine_; //例行程序
};

#endif // PROGRAMEDITOR_H
