#include "functions.h"
#include <QGridLayout>
#include <QDebug>

FunctionsMenu::FunctionsMenu(QWidget *parent) : QFrame(parent)
{
    hotedit_ = new IcoButton("HotEdit", 0, this);
    int_out_ = new IcoButton(tr("输入输出"), 1, this);
    manual_control_ = new IcoButton(tr("手动操纵"), 2, this);
    auto_produce_ = new IcoButton(tr("自动生产窗口"), 3, this);
    program_Editor_ = new IcoButton(tr("程序编辑器"), 4, this);
    program_data_ = new IcoButton(tr("程序数据"), 5, this);
    move_zero_ = new IcoButton(tr("传感器回零"), 100, this);

    backup_recovery_ = new IcoButton(tr("备份与恢复"), 6, this);
    calibration_ = new IcoButton(tr("校准"), 7, this);
    control_panel_ = new IcoButton(tr("控制面板"), 8, this);
    event_log_ = new IcoButton(tr("事件日志"), 9, this);
    file_manager_ = new IcoButton(tr("FlexPendant资源管理器"), 10, this);
    system_info_ = new IcoButton(tr("系统信息"), 11, this);

    cancellation_ = new IcoButton(tr("注销"), 12, this);
    reboot_ = new IcoButton(tr("重新启动"), 13, this);

    scale_w_ = 1;
    scale_h_ = 1;

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    this->setWindowFlags(Qt::Popup);
    this->setFrameShape(QFrame::Box);
}

void FunctionsMenu::resizeEvent(QResizeEvent *)
{
    double &scalew = scale_w_;
    double &scaleh = scale_h_;

    hotedit_->resize(270*scalew, 40*scaleh);
    int_out_->resize(hotedit_->size());
    manual_control_->resize(int_out_->size());
    auto_produce_->resize(manual_control_->size());
    program_Editor_->resize(auto_produce_->size());
    program_data_->resize(program_Editor_->size());
    move_zero_->resize(program_data_->size());
    backup_recovery_->resize(program_data_->size());
    calibration_->resize(backup_recovery_->size());
    control_panel_->resize(calibration_->size());
    event_log_->resize(control_panel_->size());
    file_manager_->resize(event_log_->size());
    system_info_->resize(file_manager_->size());
    cancellation_->resize(system_info_->size());
    reboot_->resize(cancellation_->size());

    hotedit_->move(18*scalew, 16*scaleh);
    int_out_->move(hotedit_->x(), hotedit_->y() + hotedit_->height());
    manual_control_->move(int_out_->x(), int_out_->y() + int_out_->height());
    auto_produce_->move(manual_control_->x(), manual_control_->y() + manual_control_->height());
    program_Editor_->move(auto_produce_->x(), auto_produce_->y() + auto_produce_->height());
    program_data_->move(program_Editor_->x(), program_Editor_->y() + program_Editor_->height());
    move_zero_->move(program_data_->x(), program_data_->y() + program_data_->height());
    backup_recovery_->move(hotedit_->x() + hotedit_->width(), hotedit_->y());
    calibration_->move(backup_recovery_->x(), backup_recovery_->y() + backup_recovery_->height());
    control_panel_->move(calibration_->x(), calibration_->y() + calibration_->height());
    event_log_->move(control_panel_->x(), control_panel_->y() + control_panel_->height());
    file_manager_->move(event_log_->x(), event_log_->y() + event_log_->height());
    system_info_->move(file_manager_->x(), file_manager_->y() + file_manager_->height());
    cancellation_->move(program_data_->x(), program_data_->y() + program_data_->height() + 79*scaleh);
    reboot_->move(system_info_->x(), system_info_->y() + system_info_->height() + 79*scaleh);
}

void FunctionsMenu::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void FunctionsMenu::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void FunctionsMenu::SetScale(double scale_w, double scale_h) //270*40
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

IcoButton::IcoButton(QString name, int type, QWidget *parent) : QPushButton(parent)
{
    ico_label_ = new QLabel(this);
    title_label_ = new QLabel(name, this);

    scale_w_ = 1;
    scale_h_ = 1;

    Init(type);

    this->setStyleSheet("border-color: rgb(0, 139, 139)");
    this->setFlat(true);
}

void IcoButton::resizeEvent(QResizeEvent *)
{
    double &scalew = scale_w_;

    ico_label_->resize(40*scalew, this->height());
    title_label_->resize(this->width() - ico_label_->width(), this->height());

    ico_label_->move(0, 0);
    title_label_->move(ico_label_->x() + ico_label_->width(), ico_label_->y());
}

void IcoButton::Init(int type)
{
    QPixmap pixmap;

    switch(type)
    {
    case 0: pixmap.load(":/image/Image/MainWidget/hotedit.png"); break; //hotedit按钮
    case 1: pixmap.load(":/image/Image/MainWidget/int_out.png"); break; //输入输出按钮
    case 2: pixmap.load(":/image/Image/MainWidget/manual_control.png"); break; //手动操纵按钮
    case 3: pixmap.load(":/image/Image/MainWidget/auto_produce.png"); break; //自动生产按钮
    case 4: pixmap.load(":/image/Image/MainWidget/program_Editor.png"); break; //程序编辑器按钮
    case 5: pixmap.load(":/image/Image/MainWidget/program_data.png"); break; //程序数据按钮
    case 6: pixmap.load(":/image/Image/MainWidget/backup_recovery.png"); break; //备份与恢复按钮
    case 7: pixmap.load(":/image/Image/MainWidget/calibration.png"); break; //校准按钮
    case 8: pixmap.load(":/image/Image/MainWidget/control_panel.png"); break; //控制面板按钮
    case 9: pixmap.load(":/image/Image/MainWidget/event_log.png"); break; //事件日志按钮
    case 10: pixmap.load(":/image/Image/MainWidget/file_manager.png"); break; //FlexPendant资源管理器按钮
    case 11: pixmap.load(":/image/Image/MainWidget/system_info.png"); break; //系统信息按钮
    case 12: pixmap.load(":/image/Image/MainWidget/cancellation.png"); break; //注销按钮
    case 13: pixmap.load(":/image/Image/MainWidget/reboot.png"); break; //重新启动按钮
    default: return;
    }

    ico_label_->setPixmap(pixmap);
}

void IcoButton::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}
