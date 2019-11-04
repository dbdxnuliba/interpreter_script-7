#include "alltasks.h"
#include <QGridLayout>
#include <QApplication>
#include <QEvent>
#include <QDebug>

AllTasks::AllTasks(QWidget *parent) : QFrame(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    QGridLayout *fun_layout = new QGridLayout;
    QListWidget *lt = new QListWidget(this);

    decorate_interface_ = new QWidget(this);
    decorate_label_ = new QLabel(tr("全部任务"), decorate_interface_);
    function_interface_ = new QWidget(this);
    button_interface_= new QWidget(this);
    close_button_ = new StatePushButton(tr("清除"), this);
    log_button_ = new StatePushButton(tr("不显示日志"), this);
    task_button_ = new StatePushButton(tr("不显示任务名"), this);

    layout->addWidget(decorate_interface_, 0, 0, 1, 3);
    layout->addWidget(function_interface_, 1, 0, 8, 3);
    layout->addWidget(button_interface_, 9, 0, 1, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    close_button_->setStyleSheet("background-color: rgb(255, 255, 255)"); //将背景设置为白色
    close_button_->setFlat(true);
    log_button_->setStyleSheet("background-color: rgb(255, 255, 255)");
    log_button_->setFlat(true);
    task_button_->setStyleSheet("background-color: rgb(255, 255, 255)");
    task_button_->setFlat(true);
    decorate_label_->setAlignment(Qt::AlignCenter); //设置文字以居中显示

    QPalette palette(this->palette());
    palette.setColor(QPalette::Window, QColor(192, 192, 192)); //设置背景为灰色
    button_interface_->setAutoFillBackground(true);
    button_interface_->setPalette(palette);
    decorate_label_->setAutoFillBackground(true);
    decorate_label_->setPalette(palette);
    palette.setColor(QPalette::Window, QColor(255, 255, 255)); //设置背景为白色
    decorate_interface_->setAutoFillBackground(true);
    decorate_interface_->setPalette(palette);

    fun_layout->addWidget(lt);
    function_interface_->setLayout(fun_layout);
    fun_layout->setContentsMargins(0, 0, 0, 0);

    scale_w_ = 1;
    scale_h_ = 1;

    this->setLayout(layout);
    this->setWindowFlags(Qt::Popup); //将窗口属性设置为弹出式，类似于菜单一样
    this->setFrameShape(QFrame::Box);

    ConnectInit();
}

void AllTasks::resizeEvent(QResizeEvent *)
{
    close_button_->resize(126*scale_w_, 41*scale_h_);
    close_button_->move(0, button_interface_->y());

    log_button_->resize(126*scale_w_, 41*scale_h_);
    log_button_->move(close_button_->width(), button_interface_->y());

    task_button_->resize(150*scale_w_, 41*scale_h_);
    task_button_->move(log_button_->x() + log_button_->width(), button_interface_->y());

    decorate_label_->resize(80*scale_w_, 32*scale_h_);
    decorate_label_->move(48*scale_w_, decorate_interface_->height()-decorate_label_->height());
}

void AllTasks::ConnectInit()
{
}

void AllTasks::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

StatePushButton::StatePushButton(QWidget *parent) : QPushButton(parent)
{
    state_ = true;
}

StatePushButton::StatePushButton(QString name, QWidget *parent) : QPushButton(parent)
{
    state_ = true;
    this->setText(name);
}
