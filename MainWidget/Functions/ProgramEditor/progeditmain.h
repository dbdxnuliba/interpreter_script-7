#ifndef PROGEDITMAIN_H
#define PROGEDITMAIN_H

#include <QWidget>
#include <QCheckBox>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMouseEvent>

#include "../../textbrowserobject.h"
#include "../../filemangerobject.h"
#include "../../stackedwidobject.h"
#include "../../comboboxobject.h"
#include "../../commoncontrols.h"
#include "../../listboxobject.h"
#include "../../windowobject.h"
#include "programeditordata.h"

namespace robotTeachABB
{
    class DebugWindow; //调试窗口
    class EditWindow; //编辑窗口
    class ShowAddRoutine; //新建例行程序窗口中用于显示代码的窗口
    class ParamWidget; //新建例行程序窗口中的添加参数窗口
    class RoutineWidget; //例行程序的新建、复制、移动、更改声明等窗口
    class LoadModular; //模块窗口中的加载模块窗口，继承于文件资源管理器基类
    class SaveModular; //模块窗口中的另存模块窗口，继承于LoadModular,增加了键盘输入文件名
    class CreateModular; //模块窗口中的新建模块窗口,同时兼做修改模块窗口
    class ProgEditMain; //程序编辑器功能中的主窗口
    class CodeEdit; //重写的文本框类，在原有基础上添加放大缩小左移右移等按钮，并去除了滚动条
    class TaskOrder; //任务与程序窗口
    class TaskFileManger; //任务与程序窗口专用文件资源管理器
    class Modular; //模块窗口
    class Routine; //例行程序窗口
    class AddCodeWindow; //添加指令窗口
    class ACWButton; //添加指令窗口中存放各种指令按钮的窗口
    class InsertExpresWindow; //插入表达式窗口
    class ChangeDataType; //插入表达式窗口中的更改数据类型窗口
    class InsertCodeText; //插入表达式窗口中显示代码的编辑框
    class AddFunc; //添加函数窗口
    class CodeRow; //行数窗口
    class SearchRoutine; //搜索例行程序窗口
    class ShowSystemData; //查看系统数据窗口
    class ChangeIFandFOR; //更改if和for以及test指令窗口
    class ChangeMoveWindow; //更改Move等移动指令
    class MoveOptVarWindow; //ChangeMoveWindow窗口中的可选变量窗口
    class ChangeSelectionWindow; //更改选择窗口
    class ChangeSelectText; //更改选择窗口中的显示代码编辑框
    class PPtoRoutWindow; //PP移至例行程序窗口
}
class CodeEdit;
class ChangeDataType;
class InsertExpresWindow;
class ChangeSelectionWindow;

class PPtoRoutWindow : public QWidget
{
    Q_OBJECT
public:
    PPtoRoutWindow(QWidget *parent = nullptr);
    void Init(QString select, QString filter = QString());

public slots:
    void SetScale(double scale_w, double scale_h);

signals:
    void Cancel();
    void Define(int func);

private slots:
    void ItemClicked(QTableWidgetItem *item);
    void DefineClicked();

private:
    void ConnectInit();

private:
    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *select__label_;
    QLabel *tips_filter_label_;
    QLabel *filter_label_;
    QLabel *tips2_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    ComboBoxObject *filter_button_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;
}; //PP移至例行程序窗口

class ChangeSelectText : public QTextBrowser
{
    Q_OBJECT
public:
    ChangeSelectText(QWidget *parent = nullptr);

private:
    void SetParentPointer(ChangeSelectionWindow *pointer);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    friend class ChangeSelectionWindow;
    QPointer<ChangeSelectionWindow> parent_;
}; //更改选择窗口中的显示代码编辑框

class ChangeSelectionWindow : public QWidget
{
    Q_OBJECT
public:
    ChangeSelectionWindow(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void Init(QString code, QString orig_code, QString mod_name, int select = -1, bool rep = true);

signals:
    void Define(QString code, bool rep);
    void Cancel();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefineClicked();

    void DataBoxItem(QTableWidgetItem *item);
    void FuncBoxItem(QTableWidgetItem *item);

    void ShidFilterWindow();
    void ResetFilter();
    void FilterText(QString text);

    void ShowCreateVarWindow(QString type);
    void HideCreateVarWindow();
    void CreateVar(QString);

    void ShowChangeDataType();
    void HideChangeDataType();
    void ChangeType(QString type);

    void ShowInsertWindow();
    void InsertRetCode(QString code, bool);

private:
    void ConnectInit();
    void UpdateTabBox(QString filter = QString());
    void UpdateTabBox(QString &type, QString filter);
    void DataEditClicked(); //文本框被点击
    void Init(int select);
    void SelectText(int start, int size); //选中文本

    void MoveAnalysis(int select);
    void SetAnalysis();
    void AddAnalysis(int select);
    void PulseDOAnalysis(int select);
    void SetDOAnalysis(int select);
    void WaitDIAnalysis(int select);
    void WaitDOAnalysis(int select);
    void WaitTimeAnalysis();

public:
    CreateNewVar *creat_var_window_; //新建变量窗口

private:
    friend class ChangeSelectText;

    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *current_label_;
    QLabel *tips2_label_;
    QLabel *tips_filter_label_;
    QLabel *filter_label_;

    ChangeSelectText *data_edit_;

    QTabWidget *tab_widget_;
    TableBox *data_box_;
    TableBox *func_box_;

    QWidget *button_widget_;
    ComboBoxObject *filter_button_;
    QPushButton *b123_button_;
    QPushButton *expression_button_;
    ComboBoxObject *edit_button_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    QAction *change_data_type_;
    QAction *opt_var_;
    QAction *abc_key_;

    FilterKey *filter_key_;
    ChangeDataType *change_type_;
    InsertExpresWindow *insert_window_;

private:
    QString mod_name_;
    QString original_code_;
    QString current_type_;

    QStringList di_list_;
    QStringList do_list_;
    QStringList ai_list_;
    QStringList ao_list_;

    QList<QPair<int, int>> select_pos_;
    QMap<QTableWidgetItem*, QString> data_dim_;
    int select_;
    bool rep_; //真为替换假为添加
    bool iscreate_; //是否允许新建变量，真为允许假为不允许
}; //更改选择窗口


class MoveOptVarWindow : public QWidget
{
    Q_OBJECT
public:
    MoveOptVarWindow(QWidget *parent = nullptr);

signals:
    void CloseWindow();

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    void ConnectInit();

private:
    QLabel *head_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *use_button_;
    QPushButton *no_use_button_;
    QPushButton *close_button_;
}; //ChangeMoveWindow窗口中的可选变量窗口

class ChangeMoveWindow : public QWidget
{
    Q_OBJECT
public:
    ChangeMoveWindow(QWidget *parent = nullptr);
    void Init(QString code, QString mod_name, QString rout_name, int row);
    ChangeSelectionWindow *GetChSl();

signals:
    void Cancel();
    void Define(QString code);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefineClicked();
    void ShowOptVar();
    void ItemChicked(QTableWidgetItem *item);
    void ChangeSelectDefine(QString code);

private:
    void ConnectInit();
    void Init(QString code);

private:
    QWidget *head_widget_;
    QLabel *tips1_label_;
    QLabel *current_label_;
    QLabel *tips2_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *opt_var_button_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    MoveOptVarWindow *opt_var_window_;
    ChangeSelectionWindow *change_select_window_;

private:
    QString mod_name_;
    QString code_;
    QString rout_name_;
    int row_;
}; //更改Move等移动指令

class ChangeIFandFOR : public QWidget
{
    Q_OBJECT
public:
    ChangeIFandFOR(QWidget *parent = nullptr);
    void Init(int type, QString code); //0为IF 1为FOR 2为TEST

signals:
    void Cancel();
    void Define(QString code);

public:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefineClicked();
    void Button1Clicked();
    void Button2Clicked();

    void ListItemClicked(QListWidgetItem *item);

private:
    void ConnectInit();

private:
    QLabel *tips_label_;
    ListBoxObject *list_widget_;
    QWidget *button_widget_;
    QPushButton *button1_;
    QPushButton *button2_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

private:
    QMap<QListWidgetItem*, QString> code_map_;
    QStringList code_list_;
    QListWidgetItem *old_item_;
    int type_; //0为IF 1为FOR 2为TEST
    bool is_else_; //是否存在else，真为存在假为不存在
}; //更改if和for以及test指令窗口

class ShowSystemData : public QWidget
{
    Q_OBJECT
public:
    ShowSystemData(QWidget *parent = nullptr);
    void Init();

signals:
    void Cancel();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void Refresh();
    void Define();
    void ItemTableClicked(QTableWidgetItem *item);
    void ModVarCancel();

private:
    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *select_label_;
    QLabel *tips2_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *refresh_button_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    ModVarWindow *mod_var_window_;

private:
    QSharedPointer<VarFormData> var_point_;
}; //查看系统数据窗口

class SearchRoutine : public QWidget
{
    Q_OBJECT
public:
    SearchRoutine(QWidget *parent = nullptr);
    void Init(QString filter = QString());

signals:
    void Close();

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QFrame *table_widget_;
    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *close_button_;
}; //搜索例行程序窗口

class CodeRow : public QTextBrowser
{
    Q_OBJECT
public:
    CodeRow(QWidget *parent = nullptr);
    virtual void clear();
    void AddData(QString data);
    void MvStart(); //将光标移至开始处
    void MvEnd(); //将光标移至末尾处
    void MvPos(int row); //将光标移至row处
    void SetFont(QFont &font); //设置字体
    void Upper(); //向上一格
    void Lower(); //向下一格
    void Uppers(); //向上一页
    void Lowers(); //向下一页
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    TBScrollBar *tbscrollbar_;
}; //行数窗口

class AddFunc : public QFrame
{
    Q_OBJECT
public:
    AddFunc(QWidget *parent = nullptr);
    void Init();

signals:
    void AddFuncClicked(QString code);
    void CancelClicked();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void AddFuncList(QStringList &func_list);
    void ButtonClicked(QTableWidgetItem *item);

private:
    QString func_code_;

    QLabel *title_label_;

    QLabel *tips_label_;
    QLabel *tips2_label_;
    QLabel *filter_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    ComboBoxObject *filter_button_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    InsertExpresWindow *insert_window_;
}; //添加函数窗口

class InsertExpresWindow;

class ChangeDataType : public QWidget
{
    Q_OBJECT
public:
    ChangeDataType(QWidget *parent = nullptr);
    void Init(QString type);
    void Refresh();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ItemClicked(QTableWidgetItem *item);

private:
    void Init();

private:
    TableBox *table_box_;
    QWidget *button_widget_;
    QString type_;
    QStringList name_list_;

signals:
    void ChangeType(QString type);

private:
    ComboBoxObject *view_button_; //视图按钮
    QPushButton *define_button_; //确定按钮

public:
    QPushButton *cancel_button_; //取消按钮
}; //插入表达式窗口中的更改数据类型窗口

struct InsertCodeNode
{
    InsertCodeNode(InsertCodeNode *parent);
    void SetCode(QString text);
    void SetSeat(QList<QWeakPointer<InsertCodeNode>> &select_pos, int st = 0);

    static QSharedPointer<InsertCodeNode> MakePointer(QString text, InsertCodeNode *parent);

    QString code;
    int seat;
    InsertCodeNode *parent;
    QList<QSharedPointer<InsertCodeNode>> nodes;
};

class InsertCodeText : public QTextBrowser
{
    Q_OBJECT
public:
    InsertCodeText(QWidget *parent = nullptr);

private:
    void SetParentPointer(InsertExpresWindow *pointer);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    friend class InsertExpresWindow;
    QPointer<InsertExpresWindow> parent_;
}; //插入表达式窗口中显示代码的编辑框

class InsertExpresWindow : public QWidget
{
    Q_OBJECT
public:
    InsertExpresWindow(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void Init(QString data, QString mod_name, QString type, bool lerg, bool rep);

signals:
    void ShowChangeDataType(); //显示切换数据类型界面
    void ShowCreateNewVar(QString type); //显示新建变量界面
    void ReturnCode(QString code, bool rep); //返回修改后的代码

public slots:
    void SetScale(double scale_w, double scale_h);
    void ChangeDataType(QString type); //数据类型改变完成
    void CreateVared(); //变量新建完成

private slots:
    void DefineClicked();

    void DataBoxItem(QTableWidgetItem *item);
    void FuncBoxItem(QTableWidgetItem *item);

    void ChangeType();

    void Left(); //向左
    void Right(); //向右

    void ShidFilterWindow();
    void ResetFilter();
    void FilterText(QString text);

    void AllData();
    void SelectData();
    void HideKey();
    void GetKeyData(QString text);

    void AddBt(); //加号
    void RedBt(); //减号
    void BakBt(); //添加括号
    void RbaBt(); //删除括号

private:
    void Init(QString type, bool lerg);
    void ConnectInit();
    void UpdateTabBox(QString &type, QString filter = QString());
    void DataEditClicked(); //文本框被点击

private:
    void Analysis(); //分析代码
    bool SelectText(QString text, int num = 1); //选中文本
    void SelectText(int start, int size); //选中文本

public:
    QPushButton *cancel_button_;

private:
    friend class InsertCodeText;
    friend class ChangeSelectionWindow;

    QFrame *title_widget_;
    QLabel *ico_label_;
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips_activity_label_; //活动
    QLabel *activity_label_;
    QLabel *tips_result_label_; //结果
    QLabel *result_label_;
    QLabel *tips_filter_label_; //活动过滤器
    QLabel *filter_label_;
    QLabel *tips_tips_label_; //提示
    QLabel *tips_label_;

    InsertCodeText *data_edit_;

    QTabWidget *tab_widget_;
    TableBox *data_box_;
    TableBox *func_box_;

    QWidget *button_widget_;
    QPushButton *left_button_; //左
    QPushButton *right_button_; //右
    QPushButton *add_button_; //加
    QPushButton *red_button_; //减
    QPushButton *brack_button_; //添加括号
    QPushButton *rebra_button_; //删除括号

    QWidget *lower_widget_;
    ComboBoxObject *filter_button_;
    ComboBoxObject *edit_button_;
    QPushButton *change_type_button_;
    QPushButton *define_button_;

    QAction *option_an_;
    QAction *add_an_; //添加记录
    QAction *del_an_; //删除记录
    QAction *annay_an_; //数组索引
    QAction *all_an_; //全部
    QAction *select_an_; //仅限选定内容

    Keyboard *keyboard_;
    FilterKey *filter_key_;

private:
    QString mod_name_; //模块名
    QStringList di_list_;
    QStringList do_list_;
    QStringList ai_list_;
    QStringList ao_list_;
    QList<QWeakPointer<InsertCodeNode>> select_pos_;
    QSharedPointer<InsertCodeNode> node_point_;
    QWeakPointer<InsertCodeNode> curr_point_;
    QMap<QTableWidgetItem*, QString> data_dim_;
    QStringList add_list_;
    int key_x_;
    int key_y_;
    double separate_; //分隔，比这大的为右侧，比这小的为左侧，此值一般为左侧最后一个值加0.5
    int curr_; //当前选中节点
    bool rep_; //真为替换，假为添加
    bool lerg_; //真为右侧，假为左侧
    bool isall_; //真为全部，假为选定内容
    bool iscreate_; //是否允许新建变量，真为允许假为不允许
}; //插入表达式窗口

class DebugWindow : public QWidget
{
    Q_OBJECT
public:
    DebugWindow(QWidget *parent = nullptr);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

    void SetEnableAll(bool enable);

signals:
    void ShowWindow();
    void HideWindow();

public:
    QPushButton pp_to_main_button_; //PP移至main按钮
    QPushButton pp_to_routine_button_; //PP移至例行程序按钮
    QPushButton curso_to_mp_button_; //光标移至MP按钮
    QPushButton call_routine_button_; //调用例行程序按钮
    QPushButton see_value_button_; //查看值按钮
    QPushButton see_system_button_; //查看系统数据按钮
    QPushButton pp_to_curso_button_; //PP移至光标按钮
    QPushButton curso_topp_button_; //光标移至pp按钮
    QPushButton move_pos_button_; //移至位置按钮
    QPushButton cancel_call_button_; //取消调用例行程序按钮
    QPushButton inspect_button_; //检查程序按钮
    QPushButton search_routine_button_; //搜索例行程序按钮

}; //调试窗口

class EditWindow : public QWidget
{
    Q_OBJECT
public:
    EditWindow(QWidget *parent = nullptr);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

    void SetEnableAll(bool enable);

signals:
    void ShowWindow();
    void HideWindow();

public:
    QPushButton shear_button_; //剪切按钮
    QPushButton copy_button_; //复制按钮
    QPushButton stick_button_; //黏贴按钮
    QPushButton chang_data_button_; //更改选择内容按钮
    QPushButton key_button_; //键盘按钮
    QPushButton chang_button_; //更改为按钮
    QPushButton revoke_button_; //撤销按钮
    QPushButton edit_button_; //编辑按钮
    QPushButton upper_button_; //至顶部按钮
    QPushButton lower_button_; //至底部按钮
    QPushButton upper_stick_button_; //在上面黏贴按钮
    QPushButton del_button_; //删除按钮
    QPushButton image_button_; //镜像按钮
    QPushButton remarks_button_; //备注按钮
    QPushButton redo_button_; //重做按钮
    QPushButton select_button_; //选择一项按钮
}; //编辑窗口

class ShowAddRoutine : public QWidget
{
    Q_OBJECT
public:
    ShowAddRoutine(QWidget *parent = nullptr);
    void SetCode(QString &code);

private:
    QTextBrowser *show_code_;
    QWidget *button_widget_;

public:
    QPushButton *close_button_;
}; //新建例行程序窗口中用于显示代码的窗口

struct ParamInfo
{
    QString type; //类型
    int argno; //序号
    int altno; //列号
    bool ask; //要求
    QString pattern; //模式
    QString dimen; //维数
    bool isadd; //是否可添加共用参数
};
class ParamWidget : public QWidget
{
    Q_OBJECT
public:
    ParamWidget(QWidget *parent = nullptr);
    void SetParam(QString &code);
    QString GetParam();
    void Init();
    void Init(QString code);

signals:
    void clicked();
    void cacclicked();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void GetData(QString data);
    void ParmClicked(QTreeWidgetItem *item, int);
    void UpperButtonClicked();
    void LowerButtonClicked();
    void DeleteButtonClicked();

private:
    void HideControl(); //隐藏主界面控件
    void ShowControl();

    void HideAttr(); //隐藏属性中的控件
    void ShowAttr(); //显示属性中的控件

    void AddParm(); //添加参数
    void AddOptParam(); //添加可选参数
    void AddShareParam(); //添加可选共用参数

private:
    Keyboard *keyboard_;
    QMap<QString, ParamInfo> param_map_;

    QTreeWidget *parm_tree_; //参数
    QWidget *attribute_widget_; //属性
    QWidget *button_widget_;

    ComboBoxObject *add_button_; //添加
    QPushButton *del_button_; //删除
    QPushButton *upper_button_;
    QPushButton *lower_button_;
    QPushButton *define_button_; //确定按钮
    QPushButton *cancel_button_; //取消按钮

    QLabel *name_label_;
    QLabel *type_label_;
    QLabel *argno_label_;
    QLabel *altno_label_;
    QLabel *ask_label_;
    QLabel *pattern_label_;
    QLabel *dimen_label_;
    QPushButton *name_button_;
    QPushButton *type_button_;
    QPushButton *argno_button_;
    QPushButton *altno_button_;
    QPushButton *ask_button_;
    QComboBox *pattern_box_;
    QComboBox *dimen_box_;

    QAction *add_param_; //添加参数 0
    QAction *add_opt_param_; //添加可选参数 1
    QAction *add_share_param_; //添加可选共用参数 2
    int add_type_; //用于决定是哪里进入键盘的 为3时代表修改名字
}; //新建例行程序窗口中的添加参数窗口

class RoutineWidget : public QWidget
{
    Q_OBJECT
public:
    RoutineWidget(bool show_task = true, bool show_tips = true, QWidget *parent = nullptr);
    void SetShowTips(bool show_tips); //设置是否需要显示提示标签
    void SetShowTask(bool show_task); //设置是否需要显示任务标签和任务按钮
    void Init();
    void Init(QString mod_name);
    QPair<RoutineData, QString> GetCode(); //根据已有条件获取例行程序

public slots:
    void SetScale(double scale_w, double scale_h); //因此界面使用了布局管理器所以此处的缩放比例不修改此界面大小，这里只用于兼容键盘的缩放
    void HideControl(); //隐藏主界面中的控件
    void ShowControl(); //显示主界面中的控件，同时还会隐藏已显示的其他附属界面

private:
    void ConnectInit();

private:
    QLabel *name_label_; //名称标签
    QLabel *type_label_; //程序类型标签
    QLabel *param_label_; //参数标签
    QLabel *data_type_label_; //数据类型标签
    QLabel *task_label_; //任务标签
    QLabel *mod_label_; //模块类型标签
    QWidget *button_widget_;
    QWidget *main_widget_;
    QPushButton *result_button_; //结果按钮
    QVBoxLayout *layout_;
    Keyboard *keyboard_;
    bool show_tips_; //是否显示提示标签，真显示
    bool show_task_; //是否显示任务标签和任务下拉框，真显示
    ShowAddRoutine *show_code_; //显示代码窗口
    ParamWidget *param_widget_; //修改参数窗口

public:
    QLabel *tips_label_; //提示标签
    QLineEdit *name_edit_; //名称输入框
    QComboBox *type_box_; //程序类型下拉框
    QLineEdit *param_edit_; //参数输入框
    //QLineEdit *data_type_edit_; //数据类型输入框
    QComboBox *data_type_box_; //数据类型选择框
    QComboBox *task_box_; //任务下栏选择框
    QComboBox *mod_box_; //模块下拉框
    QPushButton *define_button_; //确定按钮
    QPushButton *cancel_button_; //取消按钮
    QPushButton *show_key_button_; //名称编辑框的显示键盘的按钮
    QPushButton *show_param_button_; //显示参数界面的按钮
    QPushButton *show_type_button_; //显示类型界面的按钮
    QCheckBox *local_box_; //本地声明多选框
    QCheckBox *revoke_box_; //撤销处理程序多选框
    QCheckBox *error_box_; //错误处理程序多选框
    QCheckBox *retral_box_; //向后处理程序多选框
}; //例行程序的新建、复制、移动、更改声明等窗口

class LoadModular : public FileMangerObject
{
    Q_OBJECT
public:
    LoadModular(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void InIt();
    QString CurrentText(QString text);
    QString GetModPath();
    void SetFilterBoxText(int row); //设置过滤器所选项，row超出范围时自动为0
    void SetFilterBoxText(QString text); //设置过滤器所选项，text不存在时自动为第0项

public slots:
    void SetScale(double scale_w, double scale_h);

protected slots:
    void ShowKeyboardWidget(); //显示键盘界面
    void HideKeyboardWidget(); //隐藏键盘界面

protected:
    QComboBox *filter_box_; //过滤器
    QLabel *tips_name_label_; //文件名标签
    QString mod_path_; //模块路径
    QWidget *button_widget_;

public:
    QLineEdit *file_edit_; //文件输入框
    QPushButton *define_button_; //确定按钮
    QPushButton *cancel_button_; //取消按钮
}; //模块窗口中的加载模块窗口，继承于文件资源管理器基类

class SaveModular : public LoadModular
{
    Q_OBJECT
public:
    SaveModular(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ShowKeyboardWidget();
    void HideKeyboardWidget();

private:
    void ShowKeyWidget();
    void HideKeyWidget();
    void GetData(QString text);

private:
    QPushButton *key_button_; //显示键盘按钮
    Keyboard *keyboard_; //键盘
}; //模块窗口中的另存模块窗口，继承于LoadModular,增加了键盘输入文件名

class CreateModular : public QWidget
{
    Q_OBJECT
public:
    CreateModular(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent*);
    void Init();

public slots:
    void SetScale(double scale_w, double scale_h);
    QLabel *GetDecoLabel(); //返回装饰标签，当用作更改声明时使用

private slots:
    void GetData(QString data);

private:
    void ShowControl();
    void HideControl();

private:
    double scale_w_, scale_h_;
    QLabel *decorate_label_; //装饰性标签
    QLabel *tips_name_label_; //提示名称标签
    QLabel *tips_type_label_; //提示类型标签
    Keyboard *keyboard_; //键盘
    QPushButton *key_button_; //打开键盘按钮
    QWidget *button_widget_; //存放确定按钮和取消按钮的窗口

public:
    QLineEdit *name_edit_; //名称输入框
    QComboBox *type_box_; //类型选择框
    QPushButton *define_button_; //确定按钮
    QPushButton *cancel_button_; //取消按钮
}; //模块窗口中的新建模块窗口,同时兼做修改模块窗口

class ACWButton : public QWidget
{
    Q_OBJECT
public:
    ACWButton(QString name = QString(), QWidget *parent = nullptr);
    void AddButton(QPushButton *button1, QPushButton *button2 = nullptr); //添加按钮
    void SetNum(QString name); //为窗口设置编号
    void HideButton(); //用于隐藏上一个和下一个按钮
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

signals:
    void UpperButton(QString name);
    void LowerButton(QString name);
    void ShowWindow();
    void HideWindow();

public slots:
    void UpperClicket();
    void LowerClicket();

private:
    QStackedWidget *stack_widget_;
    QVBoxLayout *w_layout_;
    QHBoxLayout *button_layout_;
    int layout_num_;
    int save_btton_num_; //每个窗口已存放按钮数量
    QPushButton *upper_button_;
    QPushButton *lower_button_;
    QString name_; //本窗口名称
}; //添加指令窗口中存放各种指令按钮的窗口

class AddCodeWindow : public QFrame
{
    Q_OBJECT
public:
    AddCodeWindow(QWidget *parent = nullptr);
    void ReadCode(); //从文件夹中读取所有指令
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

    void SetEnableAll(bool enable); //设置所有按钮是否可以被点击

signals:
    void ShowWindow();
    void HideWindow();
    void AddCode(QString code, bool iscode); //向代码显示框添加指令

public slots:

private:
    void ButtonClicked(); //各种指令按钮被按下
    void ChangeWindow(); //用于切换窗口
    void UpperButton(QString name);
    void LowerButton(QString name);

private:
    StackedWidObject *stack_widget_;
    ComboBoxObject *show_type_; //用于显示当前窗口类型
    QMap<QString, int> type_map_; //存放类型的容器
    QMap<QString, QString> *code_map_; //存放指令的容器
    QList<QPushButton*> button_list_; //按钮指针，用于改变按钮状态
    ACWButton *type_widget_; //选择类型窗口
}; //添加指令窗口

class Routine : public WindowObject
{
    Q_OBJECT
public:
    Routine(QWidget *parent = nullptr, int type = 4);
    void resizeEvent(QResizeEvent *);
    void ReadRoutine(QString mod_name, QString filter = QString());

    void CreateRoutine(); //新建程序
    void CopyRoutine(); //复制程序
    void MoveRoutine(); //移动程序
    void ChangeState(); //更改声明
    void RenameRout(QString new_name); //重命名
    void DelRout(); //删除例行程序

signals:
    void RevMod(QString mod_name);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void HideControl(); //隐藏主界面各控件
    void ShowControl(); //显示主界面各控件
    void ShowCreateRoutine(); //显示新建例行程序窗口
    void ShowCopyRoutine();
    void ShowMoveRoutine();
    void ShowChangeState(); //显示更改声明窗口
    void ShowRenameRout(); //显示重命名窗口

private:
    QString mod_name_; //当前显示数据所属模块

    Keyboard *keyboard_; //键盘
    FilterKey *filter_key_; //过滤器

    QLabel *tips_label_;
    QLabel *tips2_label_;

    QAction *create_routine_;
    QAction *copy_routine_;
    QAction *move_routine_;
    QAction *change_state_;
    QAction *rename_routine_;
    QAction *delete_routine_;

    RoutineWidget *create_routine_widget_; //新建例行程序窗口
    RoutineWidget *copy_routine_widget_; //复制例行程序窗口
    RoutineWidget *move_routine_widget_; //移动例行程序窗口
    RoutineWidget *change_state_widget_; //更改声明窗口

public:
    QLabel *filter_label_; //显示过滤标签
    TableBox *table_box_; //显示框
    ComboBoxObject *file_button_; //文件按钮
    ComboBoxObject *filter_button_; //过滤按钮
    QPushButton *show_button_; //显示例行程序按钮
    QPushButton *retreat_button_; //后退按钮
}; //例行程序窗口

class Modular : public WindowObject
{
    Q_OBJECT
public:
    Modular(QWidget *parent = nullptr, int type = 4);
    void resizeEvent(QResizeEvent *);

signals:
    void AddMod(QString name, bool syur); //为例行程序类提供模块
    void RevMod(QString mod_name); //模块被修改
    void ClearFocus(); //清空Routine类的数据

public slots:
    void SetScale(double scale_w, double scale_h);
    void Init(); //初始化
    void ReadMod();
    void HideControl(); //隐藏控件
    void ShowControl(); //显示控件

private slots:
    void ShowCreateMod(); //显示新建模块窗口
    void ShowLoadMod(); //显示加载模块窗口
    void ShowSaveMod(); //显示保存模块窗口
    void ShowChangeState(); //显示更改声明窗口
    void DeleteMod(); //删除模块

private:
    void CreateMod(); //新建模块
    void LoadMod(); //加载模块
    void SaveMod(); //保存模块
    void ChangeState(); //更改声明

private:
    QLabel *tips_label_;

    CreateModular *create_widget_; //新建模块窗口
    LoadModular *load_widget_; //加载模块窗口
    SaveModular *save_widget_; //保存模块窗口
    CreateModular *change_widget_; //更改申明窗口

    QAction *create_mod_; //新建模块
    QAction *load_mod_; //加载模块
    QAction *save_mod_; //另存模块为
    QAction *change_state_; //更改声明
    QAction *del_mod_; //删除模块

public:
    TableBox *table_box_; //显示栏
    ComboBoxObject *file_button_; //文件按钮
    QPushButton *refresh_button_; //刷新按钮
    QPushButton *show_button_; //显示模块按钮
    QPushButton *retreat_button_; //后退按钮
}; //模块窗口

class TaskFileManger : public QWidget
{
    Q_OBJECT
public:
    TaskFileManger(QWidget *parent = nullptr);
    void Init(bool issave, QString path = QString());
    void Init(bool issave, bool crsa);

signals:
    void SaveTask();
    void LoadTask();
    void CancelButton();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void Sort(); //排序
    void ShowKeyboard(bool iscrea); //显示键盘
    void DefineClicked();

private:
    void ConnectInit();
    void FindDirs(QString path); //寻找某个路径下的所有文件夹
    void AddWidget(QStringList &data_list); //为数据框添加数据

private:
    QLabel *tips_label_;
    QLabel *curr_path_label_;

    ImageButton *show_sort_button_; //显示排序按钮
    ImageButton *hide_sort_button_; //隐藏排序按钮

    TableBox *file_box_;

    QLabel *name_label_;
    QLineEdit *dir_edit_;
    QPushButton *key_button_;

    QWidget *button_widget_;
    QPushButton *creator_button_;
    QPushButton *upper_button_;
    QPushButton *home_button_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    Keyboard *key_board_;

private:
    QString root_path_; //根目录
    QString home_path_; //主目录
    QString curr_path_; //当前路径

    QStringList data_list_; //数据

    bool pos_neg_; //用于排序，真为正序假为倒序
    bool iscrea_; //判断是新建文件夹还是修改文件夹名，真为新建
    bool issave_; //是否是保存，真为保存，假为加载
    bool crsa_; //判断是新建任务时保存或者只是另存为，真为新建，假为另存为
}; //任务与程序窗口专用文件资源管理器

class TaskOrder : public WindowObject
{
    Q_OBJECT
public:
    TaskOrder(QWidget *parent = nullptr, int type = 4);
    ~TaskOrder();
    void resizeEvent(QResizeEvent *);
    void ReadTask(); //从文件夹中读取任务

signals:
    void CreateSig(); //新建程序信号
    void DeleteSig(); //删除程序信号
    void LoadTaSig(); //加载任务信号

public slots:
    void SetScale(double scale_w, double scale_h);
    void CreateOrder();
    void RenameOrder();
    void DeleteOrder();
    void GetDate(QString data);
    void HideKey();
    void LoadTask();

private slots:
    void SaveTask();

private:
    QLabel *tips_label_;
    QAction *create_order_; //新建程序
    QAction *load_order_; //加载程序
    QAction *save_order_; //另存程序为
    QAction *rename_order_; //重命名程序
    QAction *delete_order_; //删除程序
    Keyboard *key_board_; //虚拟键盘
    TaskFileManger *file_manger_; //文件资源管理器

public:
    TableBox *table_box_; //显示栏
    ComboBoxObject *file_button_; //文件按钮
    QPushButton *show_button_; //显示模块按钮
    QPushButton *open_button_; //打开按钮
}; //任务与程序窗口

class CodeEdit : public TextBrowserObject
{
    Q_OBJECT
public:
    CodeEdit(QWidget *parent = nullptr);
    ~CodeEdit();
    void resizeEvent(QResizeEvent *e);
    void MvStart(); //将光标移至开始处
    QString GetMod(); //获取当前所显示的模块名
    void ShowAddCode(); //添加指令窗口显示，隐藏放大、缩小等按钮
    void HideAddCode(); //添加指令窗口隐藏，显示放大、缩小等按钮
    int GetRow(); //获取可操控的代码块开始行数
    void SetIsShow(bool isshow); //控制是否显示声明
    bool GetIsShow(); //返回是否显示声明
    void SaveData(); //保存数据，当示教器退出或切换模块时进行
    void SetData(QString mod_name); //设置文本框中的数据
    void Refresh(bool isupdate = false); //刷新所显示数据，如果为真则重置滚动条否则将保持滚动条原有位置
    QPair<QString, int> Convert(int row); //将指定的文本框所显示的行数转换为例行程序名与代码序号
    int Convert(QString code); //将给定代码转化为该行代码在编辑框中首次出现的位置
    int Convert(QString rout_name, int row); //将指定例行程序的指定行转化为该行在编辑框中的位置
    void SetCodeRow(CodeRow *point); //设置行号显示控件的指针
    void SetEditWindow(EditWindow *point); //设置编辑窗口控件的指针
    void SetDebugWindow(DebugWindow *point); //设置调试窗口控件的指针
    void SetCopyStr(QString *point); //设置复制字符串指针，用于单击时进行分析
    void SetSelect(int row); //选中某一行
    int GetStartRow(); //获取选中起始行
    int GetEndRow(); //获取选中终点行

signals:
    void ShowInsertExpres(QString data, QString type, bool lerg, bool rep); //需要显示出插入表达式窗口
    void ShowChangeSelect(QString code); //显示更改选择窗口
    void ShowAddFunc(); //需要显示插入函数窗口
    void DoubleClick(); //双击

public slots:
    void AddCode(QString code); //向文本框中添加指令
    void ChangeShow(int row, bool iskeep = false, int num = 0, int size = 0); //移动到指定行数，iskeep为真时切换时选中，为假切换时不选中
    void RetInsertExpres(QString data, bool rep); //插入表达式窗口的返回值
    void RetAddFunc(QString data); //插入函数窗口的返回值
    void CreateVar(QString mod_name); //新建变量
    void ChangeSelect(QString data); //更改选则窗口的返回值，只有添加

private:
    void ConnInit(); //信号与槽的初始化
    void ButtonIsHide(); //控制按钮是否隐藏或显示
    void mousePressEvent(QMouseEvent *e); //鼠标单击事件
    void mouseDoubleClickEvent(QMouseEvent *); //鼠标双击事件，空事件用于避免鼠标双击选中
    void mouseMoveEvent(QMouseEvent *); //鼠标移动事件，空事件用于防止鼠标拖动选中
    void mouseReleaseEvent(QMouseEvent *); //鼠标释放事件，空事件用于防止处于选中状态时单击导致选中取消
    void ModCodeRow(); //修改行号
    QPair<bool, QPair<QString, QString>> IsCreateVarMove(int row); //从row行向上在一个例行程序中寻找robtarget变量，用于判断是否需要添加变量，同时会搜寻第一次出现的Move指令的速度与区域数据
    QPair<bool, QPair<QString, QString>> IsCreateVarAbsj(int row); //从row行向上在一个例行程序中寻找jointtarget变量，用于判断是否需要添加变量，同时会搜寻第一次出现的Move指令的速度与区域数据

private:
    QPushButton *enlarge_button_; //放大按钮
    QPushButton *narrow_button_; //缩小按钮
    QPushButton *left_button_; //左移按钮
    QPushButton *right_button_; //右移按钮
    QScrollBar *horiz_scroll_bar_; //水平滚动条
    CodeRow *code_row_;
    EditWindow *edit_window_;
    DebugWindow *debug_window_;
    QString *copy_str_;
    int font_size_; //字体大小，初始值取自程序运行大小
    int font_max_size_; //字体最大大小，由初始值与某个数相加产生
    QString mod_name_; //模块名，仅用于保存模块
    QString old_select_; //上一次选中
    int old_position_; //上一次选中位置
    int start_row_; //选中起始行
    int end_row_; //选中终点行，普通语句与start_row相等，IF、FOR等语句时值是相对于start_row
    QString old_code_; //上一次添加的指令
    int row_main_; //分隔数，Main函数所在的行数，比这数小的为变量声明处不可主动添加指令
    bool isshow_; //是否显示声明
}; //重写的文本框类

class ProgEditMain : public QWidget
{
    Q_OBJECT
public:
    explicit ProgEditMain(QWidget *parent = nullptr);
    ~ProgEditMain();
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

signals:
    void Inspt();

public slots:
    void SetScale(double scale_w, double scale_h);
    void StartRun(); //程序开始运行

private:
    void ConnectInit();
    void EditWinConnectInit(); //编辑窗口的信号与槽绑定初始化

    void ShowChangeSelect(QString code, QString orig_code, QString mod_name, int select = -1, bool rep = true);

private slots:
    void Remark(); //备注
    void DelCode(); //删除
    void Copy(); //复制
    void Cut(); //剪切
    void Paste(); //粘贴
    void UpPaste(); //向上粘贴
    void ChangeSelect(); //更改选中内容
    void ModifyPos(); //修改位置

    void ModVar(); //查看值
    void Insp(); //检查程序
    void Search(); //搜索例行程序
    void SystemData(); //查看系统数据
    void PPtoMain(); //程序指针移动的main函数
    void PPtoCursor(); //程序指针移至光标
    void PPtoRout(); //程序指针移至例行程序
    void PPMoveRout(int func);
    void CursortoPP(); //光标移至程序指针

    void UpdatePointer(); //定时更新程序指针

    void KeyData(QString data); //键盘的返回值

    void ChangeIFFor(QString code); //if与for以及TEST指令更改返回值
    void ChangeMove(QString code); //Move等移动指令更改返回值
    void CodeEditShowChangeSelect(QString code);
    void ChangeSelection(QString code, bool rep); //更改选中窗口返回值，rep真为替换 假为添加 

private:
    void Inspect(QSharedPointer<QMap<QString,QPair<QString,int>>> insp); //检查程序

public:
    QPushButton *task_button_; //任务与程序按钮
    QPushButton *module_button_; //模块按钮
    QPushButton *order_button_; //例行程序按钮
    CodeEdit *code_edit_; //显示代码
    ComboBoxObject *add_button_; //添加指令按钮
    ComboBoxObject *edit_button_; //编辑按钮
    ComboBoxObject *debug_button_; //调试按钮
    QPushButton *modify_button_; //修改位置按钮
    QPushButton *state_button_; //隐藏或显示声明按钮

    EditWindow *edit_window_; //编辑窗口
    DebugWindow *debug_window_; //调试窗口

private:
    QSharedPointer<VarFormData> var_point_;
    QSharedPointer<QMap<QString,QPair<QString,int>>> insp_;
    QTimer *update_pointer_;

    Keyboard *keyboard_;
    CodeRow *row_num_; //用于显示行数

    QWidget *button_widget_;
    AddCodeWindow *add_code_window_; //添加指令窗口
    ModVarWindow *mod_var_window_; //修改值窗口
    SearchRoutine *search_window_; //搜索例行程序窗口
    ShowSystemData *data_window_; //查看系统数据窗口

    ChangeIFandFOR *change_if_for_window_; //更改if和for指令窗口
    ChangeMoveWindow *change_move_window_; //更改Move等移动指令窗口
    ChangeSelectionWindow *change_select_window_; //更改指令选中部分窗口

    PPtoRoutWindow *pp_move_rout_window_; //PP移至例行程序窗口

    QString copy_str_; //将被粘贴的内容
    int current_row_; //当前程序指针
    int next_row_; //下一条程序指针
    bool isshow_; //用了控制代码显示框是否显示声明
    bool timer_start_; //定时器是否开启
    bool this_isshow_; //窗口是否显示
}; //程序编辑器功能中的主窗口

#endif // PROGEDITMAIN_H
