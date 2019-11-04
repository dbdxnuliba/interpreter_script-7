#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QBitmap>
#include <QSplitter>
#include <QGridLayout>

#include "LowerWidget/lowerwidget.h"
#include "UpperWidget/upperwidget.h"
#include "Functions/functions.h"

namespace robotTeachABB
{
    class ShowWidgets; //显示功能窗口的一个装饰类，作用是在没有某个功能窗口显示的情况下作为一个控件填充mainwidget主窗口的中间
    class MainWidget; //功能主窗口，和hardware硬件主窗口并列为本示教器的两大最重要的主窗口，是所有软件功能的入口
}

class ShowWidgets : public QWidget
{
    Q_OBJECT
public:
    ShowWidgets(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    QWidget *GetShow();

public slots:
    void InsertWidget(QWidget *widget); //显示某个功能窗口
    void HideWidget(QWidget *widget); //隐藏某个功能窗口

private:
    QFrame *fm_;
    QWidget *widget_; //当前显示的窗口，用于点击自适应按钮后自动调整当前显示的窗口的大小
};

class MainWidget : public QFrame
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

private:
    void WinFunInit(); //功能窗口初始化
    void ConnectInit(); //初始化信号与槽
    void HideWidget(); //隐藏功能窗口，当界面比例要发生改变时调用
    void CloseWindow(); //关闭所显示的功能窗口

signals:
    void SendScale(double scale_w, double scale_h);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void PopEventLog(int row); //在需要的情况下弹出事件日志窗口
    void PopAutoProduce(RunMode mode); //当运行模式切换到自动模式时自动打开自动生产窗口

public:
    UpperWidget *upper_widget_;
    ShowWidgets *show_widgets_;
    LowerWidget *lower_widget_;

private:
    QSplitter *splitter_;
    double scale_w_; //等比例宽
    double scale_h_; //等比例高

public:
    AllTasks *win_all_tasks_; //全部任务窗口
    EventLog *win_event_log_; //事件日志窗口
    FunctionsMenu *win_func_menu_; //功能菜单窗口

public:
    HotEdit *hotedit_;
    FileManager *file_manager_;
    IntOut *input_out_;
    BackupRecovery *backup_recovery_;
    Calibration *calib_;
    AutoProduce *auto_produce_;
    SystemInfo *system_info_;
    ProgramData *program_data_;
    ControlPanel *control_panel_;
    ManualControl *manual_control_;
    ProgramEditor *program_edit_;

    QuickWidget *quick_window_;
    TaskQuickWidget *task_widget_;
    SpeedWidget *speed_widget_;
    StepModWidget *step_mod_widget_;
    RunModWidget *run_mod_widget_;
    IncrementWidget *increment_widget_;
    UnitQuickWidget *unit_quick_widget_;

public slots:
    void ShowEventLog(); //显示日志窗口
    void ShowHotEdit(); //显示hotedit窗口
    void ShowFileManager(); //显示文件资源管理器
    void ShowIntOut(); //显示输入输出窗口
    void ShowBackReco(); //显示备份与恢复窗口
    void ShowCalibration(); //显示校准窗口
    void ShowAutoProduce(); //显示自动生产窗口
    void ShowSystemInfo(); //显示系统信息窗口
    void ShowProgramData(); //显示程序数据窗口
    void ShowControlPanel(); //显示控制面板窗口
    void ShowManualControl(); //显示手动操纵窗口
    void ShowProgramEdit(); //显示程序编辑器窗口
    void ShowQuickWidget(); //显示快捷设置窗口
};

#endif // MAINWIDGET_H
