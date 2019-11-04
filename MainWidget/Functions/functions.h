#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include "HotEdit/hotedit.h"
#include "IntOut/intout.h"
#include "FileManager/filemanager.h"
#include "BackupRecovery/backuprecovery.h"
#include "Calibration/calibration.h"
#include "AutoProduce/autoproduce.h"
#include "SystemInfo/systeminfo.h"
#include "ProgramData/programdata.h"
#include "ControlPanel/controlpanel.h"
#include "ManualControl/manualcontrol.h"
#include "ProgramData/programdata.h"
#include "ProgramEditor/programeditor.h"

namespace robotTeachABB
{
    class FunctionsMenu; //功能菜单窗口，类似与开始菜单被点击以后弹出包含各种功能的窗口，是示教器几乎所有功能的启动入口
    class IcoButton; //重写的按钮类，使得按钮左边显示图标右边显示文字
}

class IcoButton : public QPushButton
{
    Q_OBJECT
public:
    IcoButton(QString name, int type, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void Init(int type);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *ico_label_;
    QLabel *title_label_;
    double scale_w_;
    double scale_h_;
};

class FunctionsMenu : public QFrame
{
    Q_OBJECT
public:
    explicit FunctionsMenu(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent*); //发出窗口显示信号
    void hideEvent(QHideEvent*); //发出窗口隐藏信号

signals:
    void ShowWindow(); //当窗口显示时发出此信号，用于改变菜单按钮的颜色
    void HideWindow(); //当窗口隐藏时发出此信号，用于恢复菜单按钮的颜色

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    double scale_w_;
    double scale_h_;

public:
    IcoButton *hotedit_; //hotedit
    IcoButton *int_out_; //输入输出
    IcoButton *manual_control_; //手动操纵
    IcoButton *auto_produce_; //自动生产
    IcoButton *program_Editor_; //程序编辑器
    IcoButton *program_data_; //程序数据
    IcoButton *backup_recovery_; //备份与恢复
    IcoButton *calibration_; //校准
    IcoButton *control_panel_; //控制面板
    IcoButton *event_log_; //事件日志
    IcoButton *file_manager_; //FlexPendant资源管理器
    IcoButton *system_info_; //系统信息
    IcoButton *cancellation_; //注销
    IcoButton *reboot_; //重新启动
    IcoButton *move_zero_; //传感器回零
};

#endif // FUNCTIONS_H
