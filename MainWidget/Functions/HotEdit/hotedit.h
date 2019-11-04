#ifndef HOTEDIT_H
#define HOTEDIT_H

#include <QWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>

#include "../../windowobject.h"
#include "../../comboboxobject.h"
#include "MainWidget/listboxobject.h"
#include "MainWidget/Functions/ProgramEditor/programeditordata.h"

namespace robotTeachABB
{
    class HotEditObject; //HotEdit界面所有类的基类
    class HotEdit; //hotedit功能
    class SetTreeWindow; //设定目标窗口中的树型图
    class SetAdjustWindow; //设定目标窗口中的调节窗口
    class ChoiceWindow; //选定目标窗口
    class ChoiceTableSub; //选定目标窗口中的元素
}

class HotEditObject : public QFrame
{
    Q_OBJECT
public:
    HotEditObject(QWidget *parent = nullptr);

signals:
    void ShowWindow();
    void HideWindow();

private:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
}; //HotEdit界面所有类的基类
class HotEdit;


class ChoiceTableSub : public QWidget
{
    Q_OBJECT
public:
    ChoiceTableSub(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *e);
    void SetName(QString name);
    void SetRedress(QString val);
    static QMap<QString,int> &GetMap();
    static QPointer<ChoiceTableSub> GetPointer();

signals:
    void DelButtonClicked();
    void Clicked();

public slots:
    void UpdateState();

private:
    friend class HotEdit;

    QLabel *name_label_;
    QLabel *redress_label_;
    QPushButton *del_button_;

    static QPointer<ChoiceTableSub> sub_pointer_;
    static QMap<QString,int> save_sub_map_;
}; //选定目标窗口中的元素

class ChoiceWindow : public HotEditObject
{
    Q_OBJECT
public:
    ChoiceWindow(QWidget *parent = nullptr);

private:
    friend class HotEdit;

    QLabel *target_label_;
    QLabel *compensate_label_;
    TableBox *table_box_;
}; //选定目标窗口

class SetAdjustWindow : public HotEditObject
{
    Q_OBJECT
public:
    SetAdjustWindow(QWidget *parent = nullptr);

private slots:
    void ModBoxClicked(int index);
    void AddClicked();
    void DelClicked();

private:
    void ConnectInit();

private:
    friend class HotEdit;

    QLabel *mod_label_;
    QComboBox *mod_box_;

    QLabel *ucs_label_;
    QComboBox *ucs_box_;

    QLabel *insert_label_;
    QComboBox *insert_box_;

    QLabel *delta1_name_label_;
    QPushButton *delta1_del_button_;
    QLabel *delta1_val_label_;
    QPushButton *delta1_add_button_;

    QLabel *delta2_name_label_;
    QPushButton *delta2_del_button_;
    QLabel *delta2_val_label_;
    QPushButton *delta2_add_button_;

    QLabel *delta3_name_label_;
    QPushButton *delta3_del_button_;
    QLabel *delta3_val_label_;
    QPushButton *delta3_add_button_;
}; //设定目标窗口中的调节窗口

class SetTreeWindow : public HotEditObject
{
    Q_OBJECT
public:
    SetTreeWindow(QWidget *parent = nullptr);

private:
    friend class HotEdit;

    QTreeWidget *tree_widget_;
}; //设定目标窗口中的树型图

class HotEdit : public HotEditObject
{
    Q_OBJECT
public:
    explicit HotEdit(QWidget *parent = nullptr);
    ~HotEdit();

signals:
    void SubPointClicked();
    void TAddTree();

private slots:
    void Init();
    void TreeClicked(QTreeWidgetItem *item, int column);
    void SubClicked();
    void SubDelClicked();
    void DefineClicked();
    void AddTreeData(); //将数据加入到树中

private:
    void ConnectInit();
    void ReadData(); //从本地读取数据
    void WriteData(); //将数据写入到本地
    void ThreadAddTree(); //为解决子线程操控ui时的警告而进行的中转


private:
    static QSharedPointer<QMap<QString, HotEditVarInfo>> hot_var_point_;

    QLabel ico_label_;
    QLabel title_label_;

    QGroupBox set_goals_; //设定目标窗口
    SetTreeWindow *set_tree_;
    SetAdjustWindow *set_adjust_;
    QGroupBox choice_goals_; //选定目标窗口
    ChoiceWindow * choice_window_;
    QPushButton *choice_button_; //调节目标按钮

    QFrame button_widget_;
    ComboBoxObject file_button_; //文件按钮
    ComboBoxObject base_button_; //基线按钮
    QPushButton define_button_; //应用按钮
};

#endif // HOTEDIT_H
