#ifndef AUTOPRODUCE_H
#define AUTOPRODUCE_H

#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>

#include "../../windowobject.h"
#include "../../comboboxobject.h"
#include "../ProgramEditor/progeditmain.h"

namespace robotTeachABB
{
    class AutoProduce; //自动生产窗口
    class DebugWidget; //调试窗口
    class AutoCodeEdit; //代码显示框
}

class AutoProduce;

class AutoCodeEdit : public QTextBrowser
{
    Q_OBJECT
public:
    AutoCodeEdit(QWidget *parent = nullptr);
    void SetCodeRow(CodeRow *row);
    void SetData(QString mod_name);
    QString GetMod();
    int Convert(QString rout_name, int row); //将指定例行程序的指定行转化为该行在编辑框中的位置

private:
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    TBScrollBar *tbsbar_;
    CodeRow *code_row_;
    QString mod_name_;
    int row_main_;
}; //代码显示框

class DebugWidget : public QFrame
{
    Q_OBJECT
public:
    DebugWidget(QWidget *parent = nullptr);

signals:
    void ShowWindow();
    void HideWindow();

private:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    friend class AutoProduce;
    QPushButton *modify_pos_; //修改位置
    QPushButton *show_act_point_; //显示动作指针
    QPushButton *show_fun_point_; //显示程序指针
    QPushButton *edit_fun_; //编辑程序
}; //调试窗口

class AutoProduce : public WindowObject
{
    Q_OBJECT
public:
    explicit AutoProduce(QWidget *parent = nullptr, int type = 3);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void SetCodeEdit(CodeEdit *ct); //设置progedit_code_edit_指针

signals:
    void LoadTaSig();
    void ShowEdit(); //显示编辑程序界面，也就是显示程序编辑器界面

public slots:
    void SetScale(double scale_w, double scale_h);
    void StartRun(); //程序开始运行

private slots:
    void UpdatePointer(); //定时更新程序指针
    void PPtoMain();
    void LoadTask(); //加载程序

    void FileLoadTask();

private:
    void ConnectInit();

private:
    QPushButton *loader_button_; //加载程序
    QPushButton *transfer_button_; //PP移至Main
    ComboBoxObject *debug_button_; //调试

    QWidget *brosw_widget_;
    CodeRow *code_row_;
    AutoCodeEdit *code_edit_;
    DebugWidget *debug_widget_; //调试窗口

    TaskFileManger *file_manger_; //任务加载资源管理器

private:
    QTimer *update_pointer_;
    CodeEdit *progedit_code_edit_; //ProgEditMain类的文本框，用于更新code_edit_的数据
    int current_row_; //当前程序指针
    int next_row_; //下一条程序指针
    bool timer_start_;
    bool isshow_;
};

#endif // AUTOPRODUCE_H
