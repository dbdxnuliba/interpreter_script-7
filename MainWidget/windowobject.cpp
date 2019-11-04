#include "windowobject.h"
#include <QPixmap>

WindowObject::WindowObject(int type, QWidget *parent) : QWidget(parent)
{
    ico_label_ = new QLabel(this);
    title_label_ = new QLabel(this);
    underline_ = new QLabel(this);
    main_widget_ = new QWidget(this);
    button_widget_ = new QWidget(this);

    scale_w_ = 1;
    scale_h_ = 1;
    underline_->setFrameShape(QFrame::HLine);

    Init(type);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(palette);

    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}

void WindowObject::resizeEvent(QResizeEvent *)
{
    ico_label_->resize(33*scale_w_, 26*scale_h_);
    title_label_->resize(this->width() - ico_label_->width(), 26*scale_h_);
    underline_->resize(642*scale_w_, 1*scale_h_);
    button_widget_->resize(642*scale_w_, 43*scale_h_);
    main_widget_->resize(642*scale_w_, this->height() - ico_label_->height() - button_widget_->height());

    ico_label_->move(0, 0);
    title_label_->move(ico_label_->x() + ico_label_->width(), ico_label_->y());
    underline_->move(ico_label_->x(), ico_label_->y() + ico_label_->height());
    main_widget_->move(underline_->x(), underline_->y() + underline_->height());
    button_widget_->move(main_widget_->x(), main_widget_->y() + main_widget_->height());
}

void WindowObject::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

QLabel *WindowObject::GetTitleLabel()
{
    return title_label_;
}

QLabel *WindowObject::GetIcoLabel()
{
    return ico_label_;
}

QLabel *WindowObject::GetUnderLabel()
{
    return underline_;
}

double WindowObject::ScaleW()
{
    return scale_w_;
}

double WindowObject::ScaleH()
{
    return scale_h_;
}

void WindowObject::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void WindowObject::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void WindowObject::Init(int type)
{
    QString image_path;
    switch(type)
    {
    case 0:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/hotedit.png)";  //hotedit按钮
        title_label_->setText("hotedit");
        break;
    }
    case 1:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/int_out.png)"; //输入输出按钮
        title_label_->setText(tr("输入输出"));
        break;
    }
    case 2:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/manual_control.png)"; //手动操纵按钮
        title_label_->setText(tr("手动操纵"));
        break;
    }
    case 3:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/auto_produce.png)"; //自动生产按钮
        title_label_->setText(tr("自动生产窗口"));
        break;
    }
    case 4:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/program_Editor.png)"; //程序编辑器按钮
        title_label_->setText(tr("程序编辑器"));
        break;
    }
    case 5:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/program_data.png)"; //程序数据按钮
        title_label_->setText(tr("程序数据"));
        break;
    }
    case 6:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/backup_recovery.png)"; //备份与恢复按钮
        title_label_->setText(tr("备份与恢复"));
        break; //备份与恢复按钮
    }
    case 7:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/calibration.png)";  //校准按钮
        title_label_->setText(tr("校准"));
        break;
    }
    case 8:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/control_panel.png)";  //控制面板按钮
        title_label_->setText(tr("控制面板"));
        break;
    }
    case 9:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/event_log.png)"; //事件日志按钮
        title_label_->setText(tr("事件日志"));
        break;
    }
    case 10:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/file_manager.png)";//FlexPendant资源管理器按钮
        title_label_->setText(tr("FlexPendant资源管理器"));
        break;
    }
    case 11:
    {
        image_path = "border-image: url(:/image/Image/MainWidget/system_info.png)"; //系统信息按钮
        title_label_->setText(tr("系统信息"));
        break;
    }
    default: //不是以上任何图标，也就是说子类要设置一个特殊的图标
    {
        return;
    }
    }
    ico_label_->setStyleSheet(image_path);
}
