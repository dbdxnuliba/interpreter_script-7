#include "widget.h"
#include "teachstate.h"
#include <QGridLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QThread>
#include <thread>
#include "RobotClient/robotClient.h"
#include <QDir>
#include <QDebug>

using namespace rclib;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->resize(860, 484);
    hardware_ = new Hardware(this);
    main_widget_ = new MainWidget(this);
    this->setAttribute(Qt::WA_TranslucentBackground); //使窗口透明
    hardware_->resize(220, 484);
    main_widget_->resize(640, 480);
    this->resize(hardware_->width() + main_widget_->width(), hardware_->height()); //调整窗口大小以便正常显示图片
    width_ = this->width();
    scale_w_ = 1;
    scale_h_ = 1;
    ismove_ = true;
    issteprun_ = false;

    ConnectInit();

#ifdef __arm__
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); //隐藏窗口标题栏并且置顶
    hardware_->zoom_widget_->hide(); //隐藏调整大小按钮
    Adapt(); //最大化
#else
    this->setWindowFlags(Qt::FramelessWindowHint); //隐藏窗口标题栏
#endif

    TeachState::instance()->SendStateSignals(); //将当前状态向整个示教器发送一遍，用于初始化
}

Widget::~Widget()
{
    SaveData();

    std::_Exit(0);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(!ismove_)
            return;

        if(hardware_->geometry().contains(this->mapFromGlobal(QCursor::pos()))) //判断当前鼠标点击的位置是否在硬件窗口中，如果不在则不允许移动
        {
            ismove_ = false;
            return;
        }
        point_ = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        if(!ismove_)
            return;

        if(hardware_->geometry().contains(this->mapFromGlobal(QCursor::pos()))) //判断当前鼠标点击的位置是否在硬件窗口中，如果不在则不允许移动
        {
            return;
        }
        move(event->globalPos() - point_);
        event->accept();
    }
}

void Widget::resizeEvent(QResizeEvent *)
{
    if(main_widget_->isHidden())
    {
        hardware_->move(0, 0);
    }
    else
    {
        main_widget_->move(0, 0);
        hardware_->move(main_widget_->width(), 0);
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    if(TeachState::instance()->GetRunState()) //退出时判断程序是否还在运行
    {
        QMessageBox::about(this, tr("提示"), tr("请先使程序停止运行"));
        event->ignore(); //忽略关闭事件
    }
    else
    {
        event->accept();
    }
}

void Widget::Adapt()
{
    if(hardware_->zoom_widget_->text() == " ") //未放大
    {
        hardware_->zoom_widget_->setText("  ");

        double w = QApplication::desktop()->width();
        double h = QApplication::desktop()->height();
        double w_ = this->width();
        double h_ = this->height();
        scale_w_ = w / w_;
        scale_h_ = h / h_;
        ismove_ = false; //设置为不可移动

        emit SetScale(scale_w_, scale_h_);

        this->resize(this->width()*scale_w_, this->height()*scale_h_);
        width_ = width_*static_cast<int>(scale_w_);
        this->showFullScreen(); //全屏显示

        if(main_widget_->isHidden()) //如果功能窗口被隐藏，为了防止bug禁止在最大化状态下显示功能窗口
            hardware_->show_widget_->setEnabled(false);
    }
    else
    {
        hardware_->zoom_widget_->setText(" ");

        scale_w_ = 1;
        scale_h_ = 1;
        ismove_ = true; //设置为可移动

        emit SetScale(scale_w_, scale_h_);

        if(main_widget_->isHidden()) //如果功能窗口已被隐藏
        {
            this->showNormal(); //取消全屏显示
            this->resize(220, 484);
            hardware_->move(0, 0);
            width_ = 860;

            hardware_->show_widget_->setEnabled(true); //恢复显示功能窗口按钮
        }
        else
        {
            this->showNormal();
            this->resize(860, 484);
            main_widget_->move(0, 0);
            hardware_->move(main_widget_->width(), 0);
            width_ = 860;
        }
    }
}

void Widget::HardwareEnabled()
{
    if(!TeachState::instance()->GetEnabled())
    {
        main_widget_->upper_widget_->state_button_->SetState(tr("防护装置停止"));
    }
    else
    {
        main_widget_->upper_widget_->state_button_->SetState(tr("电机开启"));
    }
}

void Widget::Stop()
{
    auto *button = dynamic_cast<HardwareButton*>(sender());
    if(button == nullptr)
        return;

    if(button->issend_)
    {
        if(TeachState::instance()->GetIncrement() == Increment::None) //只有点动才发送点动停止，步进不发送
            RobotClient::initance()->stopJOG();

        button->issend_ = false;
    }
}

void Widget::HardwareUpper()
{
    if(!TeachState::instance()->GetEnabled()) //使能按钮未被按下
        return;

    if(TeachState::instance()->GetRunState() == 2)
    {
        TeachState::instance()->SendLog(EventLogType::TipsPop, tr("操作失败"), tr("当前程序正在运行"));
        return;
    }

    ManualControl *manu = main_widget_->manual_control_;

    if(manu->manu_widget_->joystick_box_->GetLock(1)) //已被锁定
        return;

    double speed =  main_widget_->speed_widget_->GetSpeed(); //速度

    auto mod = TeachState::instance()->GetActionMode();
    if(mod == ActionMode::Axis1_3 || mod == ActionMode::Axis4_6)
    {
        JOINTINDEX joint;
        MOVEDIRECTION move = MOVE_POSITIVE; //加

        if(mod == ActionMode::Axis1_3) //1-3轴
            joint = JOINT_2;
        else
            joint = JOINT_5;

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //关节点动
        {
            RobotClient::initance()->jointJOG(joint, move, speed);
        }
        else //关节步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->jointStep(joint, move, step, speed);
        }
    }
    else if(mod == ActionMode::Linear) //线性
    {
        TERMINALINDEX terminal = TERMINAL_X;
        MOVEDIRECTION dir = MOVE_POSITIVE; //加
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }
    else if(mod == ActionMode::Relocate) //重定位
    {
        TERMINALINDEX terminal = TERMINAL_A;
        MOVEDIRECTION dir = MOVE_POSITIVE; //加
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }

    HardwareButton *button = dynamic_cast<HardwareButton*>(sender());
    if(button == nullptr)
        return;

    button->issend_ = true;
}

void Widget::HardwareLower()
{
    if(!TeachState::instance()->GetEnabled()) //使能按钮未被按下
        return;

    if(TeachState::instance()->GetRunState() == 2)
    {
        TeachState::instance()->SendLog(EventLogType::TipsPop, tr("操作失败"), tr("当前程序正在运行"));
        return;
    }

    ManualControl *manu = main_widget_->manual_control_;

    if(manu->manu_widget_->joystick_box_->GetLock(1)) //已被锁定
        return;

    double speed =  main_widget_->speed_widget_->GetSpeed(); //速度

    auto mod = TeachState::instance()->GetActionMode();
    if(mod == ActionMode::Axis1_3 || mod == ActionMode::Axis4_6)
    {
        JOINTINDEX joint;
        MOVEDIRECTION move = MOVE_NEGATIVE; //减

        if(mod == ActionMode::Axis1_3) //1-3轴
            joint = JOINT_2;
        else
            joint = JOINT_5;

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //关节点动
        {
            RobotClient::initance()->jointJOG(joint, move, speed);
        }
        else //关节步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->jointStep(joint, move, step, speed);
        }
    }
    else if(mod == ActionMode::Linear) //线性
    {
        TERMINALINDEX terminal = TERMINAL_X;
        MOVEDIRECTION dir = MOVE_NEGATIVE; //减
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }
    else if(mod == ActionMode::Relocate) //重定位
    {
        TERMINALINDEX terminal = TERMINAL_A;
        MOVEDIRECTION dir = MOVE_NEGATIVE; //减
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }

    HardwareButton *button = dynamic_cast<HardwareButton*>(sender());
    if(button == nullptr)
        return;

    button->issend_ = true;
}

void Widget::HardwareLeft()
{
    if(!TeachState::instance()->GetEnabled()) //使能按钮未被按下
        return;

    if(TeachState::instance()->GetRunState() == 2)
    {
        TeachState::instance()->SendLog(EventLogType::TipsPop, tr("操作失败"), tr("当前程序正在运行"));
        return;
    }

    ManualControl *manu = main_widget_->manual_control_;

    if(manu->manu_widget_->joystick_box_->GetLock(2)) //已被锁定
        return;

    double speed =  main_widget_->speed_widget_->GetSpeed(); //速度

    auto mod = TeachState::instance()->GetActionMode();
    if(mod == ActionMode::Axis1_3 || mod == ActionMode::Axis4_6)
    {
        JOINTINDEX joint;
        MOVEDIRECTION move = MOVE_POSITIVE; //加

        if(mod == ActionMode::Axis1_3) //1-3轴
            joint = JOINT_1;
        else
            joint = JOINT_4;

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //关节点动
        {
            RobotClient::initance()->jointJOG(joint, move, speed);
        }
        else //关节步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->jointStep(joint, move, step, speed);
        }
    }
    else if(mod == ActionMode::Linear) //线性
    {
        TERMINALINDEX terminal = TERMINAL_Y;
        MOVEDIRECTION dir = MOVE_POSITIVE; //加
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }
    else if(mod == ActionMode::Relocate) //重定位
    {
        TERMINALINDEX terminal = TERMINAL_B;
        MOVEDIRECTION dir = MOVE_POSITIVE; //加
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }

    HardwareButton *button = dynamic_cast<HardwareButton*>(sender());
    if(button == nullptr)
        return;

    button->issend_ = true;
}

void Widget::HardwareRight()
{
    if(!TeachState::instance()->GetEnabled()) //使能按钮未被按下
        return;

    if(TeachState::instance()->GetRunState() == 2)
    {
        TeachState::instance()->SendLog(EventLogType::TipsPop, tr("操作失败"), tr("当前程序正在运行"));
        return;
    }

    ManualControl *manu = main_widget_->manual_control_;

    if(manu->manu_widget_->joystick_box_->GetLock(2)) //已被锁定
        return;

    double speed =  main_widget_->speed_widget_->GetSpeed(); //速度

    auto mod = TeachState::instance()->GetActionMode();
    if(mod == ActionMode::Axis1_3 || mod == ActionMode::Axis4_6)
    {
        JOINTINDEX joint;
        MOVEDIRECTION move = MOVE_NEGATIVE; //减

        if(mod == ActionMode::Axis1_3) //1-3轴
            joint = JOINT_1;
        else
            joint = JOINT_4;

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //关节点动
        {
            RobotClient::initance()->jointJOG(joint, move, speed);
        }
        else //关节步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->jointStep(joint, move, step, speed);
        }
    }
    else if(mod == ActionMode::Linear) //线性
    {
        TERMINALINDEX terminal = TERMINAL_Y;
        MOVEDIRECTION dir = MOVE_NEGATIVE; //减
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }
    else if(mod == ActionMode::Relocate) //重定位
    {
        TERMINALINDEX terminal = TERMINAL_B;
        MOVEDIRECTION dir = MOVE_NEGATIVE; //减
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }

    HardwareButton *button = dynamic_cast<HardwareButton*>(sender());
    if(button == nullptr)
        return;

    button->issend_ = true;
}

void Widget::HardwareLevo()
{
    if(!TeachState::instance()->GetEnabled()) //使能按钮未被按下
        return;

    if(TeachState::instance()->GetRunState() == 2)
    {
        TeachState::instance()->SendLog(EventLogType::TipsPop, tr("操作失败"), tr("当前程序正在运行"));
        return;
    }

    ManualControl *manu = main_widget_->manual_control_;

    if(manu->manu_widget_->joystick_box_->GetLock(3)) //已被锁定
        return;

    double speed =  main_widget_->speed_widget_->GetSpeed(); //速度

    auto mod = TeachState::instance()->GetActionMode();

    if(mod == ActionMode::Axis1_3 || mod == ActionMode::Axis4_6)
    {
        JOINTINDEX joint;
        MOVEDIRECTION move = MOVE_POSITIVE; //加

        if(mod == ActionMode::Axis1_3) //1-3轴
            joint = JOINT_3;
        else
            joint = JOINT_6;

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //关节点动
        {
            RobotClient::initance()->jointJOG(joint, move, speed);
        }
        else //关节步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->jointStep(joint, move, step, speed);
        }
    }
    else if(mod == ActionMode::Linear) //线性
    {
        TERMINALINDEX terminal = TERMINAL_Z;
        MOVEDIRECTION dir = MOVE_POSITIVE; //加
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }
    else if(mod == ActionMode::Relocate) //重定位
    {
        TERMINALINDEX terminal = TERMINAL_C;
        MOVEDIRECTION dir = MOVE_POSITIVE; //加
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }

    HardwareButton *button = dynamic_cast<HardwareButton*>(sender());
    if(button == nullptr)
        return;

    button->issend_ = true;
}

void Widget::HardwareCumSole()
{
    if(!TeachState::instance()->GetEnabled()) //使能按钮未被按下
        return;

    if(TeachState::instance()->GetRunState() == 2)
    {
        TeachState::instance()->SendLog(EventLogType::TipsPop, tr("操作失败"), tr("当前程序正在运行"));
        return;
    }

    ManualControl *manu = main_widget_->manual_control_;

    if(manu->manu_widget_->joystick_box_->GetLock(3)) //已被锁定
        return;

    double speed =  main_widget_->speed_widget_->GetSpeed(); //速度

    auto mod = TeachState::instance()->GetActionMode();

    if(mod == ActionMode::Axis1_3 || mod == ActionMode::Axis4_6)
    {
        JOINTINDEX joint;
        MOVEDIRECTION move = MOVE_NEGATIVE; //减

        if(mod == ActionMode::Axis1_3) //1-3轴
            joint = JOINT_3;
        else
            joint = JOINT_6;

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //关节点动
        {
            RobotClient::initance()->jointJOG(joint, move, speed);
        }
        else //关节步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->jointStep(joint, move, step, speed);
        }
    }
    else if(mod == ActionMode::Linear) //线性
    {
        TERMINALINDEX terminal = TERMINAL_Z;
        MOVEDIRECTION dir = MOVE_NEGATIVE; //减
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }
    else if(mod == ActionMode::Relocate) //重定位
    {
        TERMINALINDEX terminal = TERMINAL_C;
        MOVEDIRECTION dir = MOVE_NEGATIVE; //减
        COORDINATESYSTEM frame;

        auto coor_mod = TeachState::instance()->GetCoordinSys();
        if(coor_mod == CoordinSys::Base || coor_mod == CoordinSys::Earth) //基坐标和大地坐标
        {
            frame = COORDINATE_BASE;
        }
        else if(coor_mod == CoordinSys::Tool) //工具坐标
        {
            frame = COORDINATE_TOOL;
        }
        else //工件坐标
        {
            frame = COORDINATE_WORK;
        }

        auto incr_mod = TeachState::instance()->GetIncrement();
        if(incr_mod == Increment::None) //末端点动
        {
            RobotClient::initance()->terminalJOG(terminal, dir, speed, frame);
        }
        else //末端步进
        {
            auto step = TeachState::instance()->GetIncrVar(mod);
            RobotClient::initance()->terminalStep(terminal, dir, step, speed, frame);
        }
    }

    HardwareButton *button = dynamic_cast<HardwareButton*>(sender());
    if(button == nullptr)
        return;

    button->issend_ = true;
}

void Widget::PK1Clicked()
{
    if(!TeachState::instance()->GetControlState())
        return;
    int type = TeachState::instance()->GetPKType(ProgKeysName::PK1);
    int keys = TeachState::instance()->GetPKKeys(ProgKeysName::PK1);
    int isauto = TeachState::instance()->GetPKAuto(ProgKeysName::PK1);
    auto select = TeachState::instance()->GetPKSelect(ProgKeysName::PK1);
    if(type == 0)
        return;
    if(TeachState::instance()->GetRunMode() == RunMode::AutoMod && isauto == 0)
        return;

    switch(type)
    {
    case 1: //输入
    {
        break;
    }
    case 2: //输出
    {
        auto str = select.mid(2);
        bool isint;
        int port = str.toInt(&isint) - 1;
        if(!isint)
            return;
        PORTINDEX px = PORTINDEX(port);

        switch(keys)
        {
        case 0: //切换
        {
            auto state = RobotClient::initance()->getDigitalOutput(px);
            if(state == SWITCHSTATE::SWITCHOFF)
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            else
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 1: //设为1
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            break;
        }
        case 2: //设为0
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 3: //按下/松开
        {
            auto *button = dynamic_cast<ProgKeysButton*>(sender());
            if(button == nullptr)
                return;
            button->setWindowTitle("0");
            button->setPORX(port);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);

            break;
        }
        case 4: //脉冲
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);

            break;
        }
        }

        break;
    }
    case 3: //系统
    {
        auto pir = EditData::instance()->GetRoutPoint("main", 1);
        if(pir.first == -1 || pir.second == -1)
            return;
        ProgramPointer pr;
        pr.nFunction = pir.first;
        pr.nSentence = pir.second;

        RobotClient::initance()->setPointer(pr);

        break;
    }
    }
}

void Widget::PK2Clicked()
{
    if(!TeachState::instance()->GetControlState())
        return;
    int type = TeachState::instance()->GetPKType(ProgKeysName::PK2);
    int keys = TeachState::instance()->GetPKKeys(ProgKeysName::PK2);
    int isauto = TeachState::instance()->GetPKAuto(ProgKeysName::PK2);
    auto select = TeachState::instance()->GetPKSelect(ProgKeysName::PK2);
    if(type == 0)
        return;
    if(TeachState::instance()->GetRunMode() == RunMode::AutoMod && isauto == 0)
        return;

    switch(type)
    {
    case 1: //输入
    {
        break;
    }
    case 2: //输出
    {
        auto str = select.mid(2);
        bool isint;
        int port = str.toInt(&isint) - 1;
        if(!isint)
            return;
        PORTINDEX px = PORTINDEX(port);

        switch(keys)
        {
        case 0: //切换
        {
            auto state = RobotClient::initance()->getDigitalOutput(px);
            if(state == SWITCHSTATE::SWITCHOFF)
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            else
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 1: //设为1
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            break;
        }
        case 2: //设为0
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 3: //按下/松开
        {
            auto *button = dynamic_cast<ProgKeysButton*>(sender());
            if(button == nullptr)
                return;
            button->setWindowTitle("0");
            button->setPORX(port);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);

            break;
        }
        case 4: //脉冲
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);

            break;
        }
        }

        break;
    }
    case 3: //系统
    {
        auto pir = EditData::instance()->GetRoutPoint("main", 1);
        if(pir.first == -1 || pir.second == -1)
            return;
        ProgramPointer pr;
        pr.nFunction = pir.first;
        pr.nSentence = pir.second;

        RobotClient::initance()->setPointer(pr);

        break;
    }
    }
}

void Widget::PK3Clicked()
{
    if(!TeachState::instance()->GetControlState())
        return;
    int type = TeachState::instance()->GetPKType(ProgKeysName::PK3);
    int keys = TeachState::instance()->GetPKKeys(ProgKeysName::PK3);
    int isauto = TeachState::instance()->GetPKAuto(ProgKeysName::PK3);
    auto select = TeachState::instance()->GetPKSelect(ProgKeysName::PK3);
    if(type == 0)
        return;
    if(TeachState::instance()->GetRunMode() == RunMode::AutoMod && isauto == 0)
        return;

    switch(type)
    {
    case 1: //输入
    {
        break;
    }
    case 2: //输出
    {
        auto str = select.mid(2);
        bool isint;
        int port = str.toInt(&isint) - 1;
        if(!isint)
            return;
        PORTINDEX px = PORTINDEX(port);

        switch(keys)
        {
        case 0: //切换
        {
            auto state = RobotClient::initance()->getDigitalOutput(px);
            if(state == SWITCHSTATE::SWITCHOFF)
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            else
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 1: //设为1
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            break;
        }
        case 2: //设为0
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 3: //按下/松开
        {
            auto *button = dynamic_cast<ProgKeysButton*>(sender());
            if(button == nullptr)
                return;
            button->setWindowTitle("0");
            button->setPORX(port);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);

            break;
        }
        case 4: //脉冲
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);

            break;
        }
        }

        break;
    }
    case 3: //系统
    {
        auto pir = EditData::instance()->GetRoutPoint("main", 1);
        if(pir.first == -1 || pir.second == -1)
            return;
        ProgramPointer pr;
        pr.nFunction = pir.first;
        pr.nSentence = pir.second;

        RobotClient::initance()->setPointer(pr);

        break;
    }
    }
}

void Widget::PK4Clicked()
{
    if(!TeachState::instance()->GetControlState())
        return;
    int type = TeachState::instance()->GetPKType(ProgKeysName::PK4);
    int keys = TeachState::instance()->GetPKKeys(ProgKeysName::PK4);
    int isauto = TeachState::instance()->GetPKAuto(ProgKeysName::PK4);
    auto select = TeachState::instance()->GetPKSelect(ProgKeysName::PK4);
    if(type == 0)
        return;
    if(TeachState::instance()->GetRunMode() == RunMode::AutoMod && isauto == 0)
        return;

    switch(type)
    {
    case 1: //输入
    {
        break;
    }
    case 2: //输出
    {
        auto str = select.mid(2);
        bool isint;
        int port = str.toInt(&isint) - 1;
        if(!isint)
            return;
        PORTINDEX px = PORTINDEX(port);

        switch(keys)
        {
        case 0: //切换
        {
            auto state = RobotClient::initance()->getDigitalOutput(px);
            if(state == SWITCHSTATE::SWITCHOFF)
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            else
                RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 1: //设为1
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            break;
        }
        case 2: //设为0
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
            break;
        }
        case 3: //按下/松开
        {
            auto *button = dynamic_cast<ProgKeysButton*>(sender());
            if(button == nullptr)
                return;
            button->setWindowTitle("0");
            button->setPORX(port);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);

            break;
        }
        case 4: //脉冲
        {
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
            RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);

            break;
        }
        }

        break;
    }
    case 3: //系统
    {
        auto pir = EditData::instance()->GetRoutPoint("main", 1);
        if(pir.first == -1 || pir.second == -1)
            return;
        ProgramPointer pr;
        pr.nFunction = pir.first;
        pr.nSentence = pir.second;

        RobotClient::initance()->setPointer(pr);

        break;
    }
    }
}

void Widget::PKRelease()
{
    auto *button = dynamic_cast<ProgKeysButton*>(sender());
    if(button == nullptr)
        return;

    if(button->windowTitle() == "0")
    {
        button->setWindowTitle("1");

        if(button->GetPORX() == -1)
            return;
        PORTINDEX px = PORTINDEX(button->GetPORX());
        button->setPORX(-1);

        RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
    }
}

void Widget::LeftStep()
{
    if(!TeachState::instance()->GetControlState())
    {
        QMessageBox::warning(this, tr("启动程序"), tr("不允许的操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("无控制权"));
        return;
    }
    if(!TeachState::instance()->GetElect())
    {
        QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("电机关闭"));
        return;
    }
    if(!TeachState::instance()->GetEnabled() && TeachState::instance()->GetRunMode() != RunMode::AutoMod)
    {
        QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("使能关闭"));
        return;
    }
    auto run_state = RobotClient::initance()->getRobotState().getRunState();
    if(run_state == SYSRUNSTATE::SYSRUN_RUN && !issteprun_)
    {
        if(!TeachState::instance()->GetRunState())
            TeachState::instance()->SetRunState(true);
        QMessageBox::warning(this, tr("启动程序"), tr("由于至少在执行一个任务，所以无法更改执行模式"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("程序正在运行"));
        return;
    }
    else
    {
        if(TeachState::instance()->GetRunState())
            TeachState::instance()->SetRunState(false);
    }

    if(TeachState::instance()->GetCodeState()) //只有代码被修改才重新拼接发送给服务端
        EditData::instance()->GetSendCode(std::bind(&Widget::LeftStepRun, this, std::placeholders::_1, std::placeholders::_2));
    else
        this->ProgramLeftStepRun();
}

void Widget::RightStep()
{
    if(!TeachState::instance()->GetControlState())
    {
        QMessageBox::warning(this, tr("启动程序"), tr("不允许的操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("无控制权"));
        return;
    }
    if(!TeachState::instance()->GetElect())
    {
        QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("电机关闭"));
        return;
    }
    if(!TeachState::instance()->GetEnabled() && TeachState::instance()->GetRunMode() != RunMode::AutoMod)
    {
        QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("使能关闭"));
        return;
    }
    auto run_state = RobotClient::initance()->getRobotState().getRunState();
    if(run_state == SYSRUNSTATE::SYSRUN_RUN && !issteprun_)
    {
        if(!TeachState::instance()->GetRunState())
            TeachState::instance()->SetRunState(true);
        QMessageBox::warning(this, tr("启动程序"), tr("由于至少在执行一个任务，所以无法更改执行模式"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("单步失败"), tr("程序正在运行"));
        return;
    }
    else
    {
        if(TeachState::instance()->GetRunState())
            TeachState::instance()->SetRunState(false);
    }

    if(TeachState::instance()->GetCodeState()) //只有代码被修改才重新拼接发送给服务端
        EditData::instance()->GetSendCode(std::bind(&Widget::RightStepRun, this, std::placeholders::_1, std::placeholders::_2));
    else
        this->ProgramRightStepRun();
}

void Widget::SaveData()
{
    delete hardware_;
    delete main_widget_;

    auto dir_path = qApp->applicationDirPath();
    QDir d(dir_path);
    d.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    auto dirs = d.entryList();
    for(auto dir : dirs)
    {
        bool isint;
        dir.toInt(&isint);
        if(!isint && dir != "robotTeachABBFile") //只删除由整数所构成名字的文件夹以及robotTeachABBFile文件夹
            continue;

        auto curr_dir = dir_path + "/" + dir;

        QDir fd(curr_dir);
        fd.setFilter(QDir::Files);
        int count = static_cast<int>(fd.count());
        for(int i = 0; i < count; i++)
            fd.remove(fd[i]); //先删除该文件夹下的所有文件

        fd.rmpath(curr_dir);
    }
}

void Widget::Run(bool isok, QSharedPointer<QString> code)
{
    if(code.isNull() || !isok)
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("所执行的代码可能存在错误"));
        return;
    }

    auto path = qApp->applicationDirPath() + "/123456/";
    QDir dir(path);
    if(!dir.mkpath(path))
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("无本地访问权限"));
        return;
    }

    path.append("code.txt");
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("无本地读写权限"));
        return;
    }

    file.write(code->toUtf8().data());
    file.close();

    RobotClient::initance()->readFunctionABB(path.toUtf8().data()); //发送代码文件
    TeachState::instance()->SetCodeState(false);
    QThread::sleep(1);

    emit RunProgram();
}

void Widget::LeftStepRun(bool isok, QSharedPointer<QString> code)
{
    if(code.isNull() || !isok)
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("所执行的代码可能存在错误"));
        return;
    }

    auto path = qApp->applicationDirPath() + "/123456/";
    QDir dir(path);
    if(!dir.mkpath(path))
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("无本地访问权限"));
        return;
    }

    path.append("code.txt");
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("无本地读写权限"));
        return;
    }

    file.write(code->toUtf8().data());
    file.close();

    RobotClient::initance()->readFunctionABB(path.toUtf8().data()); //发送代码文件
    TeachState::instance()->SetCodeState(false);
    QThread::sleep(1);

    emit LeftStepRunProgram();
}

void Widget::RightStepRun(bool isok, QSharedPointer<QString> code)
{
    if(code.isNull() || !isok)
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("所执行的代码可能存在错误"));
        return;
    }

    auto path = qApp->applicationDirPath() + "/123456/";
    QDir dir(path);
    if(!dir.mkpath(path))
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("无本地访问权限"));
        return;
    }

    path.append("code.txt");
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("程序执行失败"), tr("无本地读写权限"));
        return;
    }

    file.write(code->toUtf8().data());
    file.close();

    RobotClient::initance()->readFunctionABB(path.toUtf8().data()); //发送代码文件
    TeachState::instance()->SetCodeState(false);
    QThread::sleep(1);

    emit RightStepRunProgram();
}

void runThread11()
{
    TeachState::instance()->SetRunState(true);
    RobotClient::initance()->setPlayState(SYSPLAY_PLAY);
    RobotClient::initance()->setRunState(SYSRUN_RUN);
    RobotClient::initance()->m_bRunning = true;
    RobotClient::initance()->setTeach(ABB);
    RobotClient::initance()->runABB();
    if (RobotClient::initance()->m_bConStartFlag)
    {
        RobotClient::initance()->m_bConStartFlag = false;
        RobotClient::initance()->moveEndCon();
        RobotClient::initance()->waitCommandEnd();
    }
    RobotClient::initance()->programStop();
    RobotClient::initance()->m_bRunning = false;
    RobotClient::initance()->setPlayState(SYSPLAY_TEACH);
    TeachState::instance()->SetRunState(false);
}

void Widget::ProgramRun()
{
    issteprun_ = false;
    std::thread t(runThread11);
    t.detach();

    emit ProgramStart();
}

void Widget::ProgramStop()
{
    if(!TeachState::instance()->GetControlState())
    {
        QMessageBox::warning(this, tr("提示"), tr("不允许的操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("停止失败"), tr("无控制权"));
        return;
    }
    auto run_state = RobotClient::initance()->getRobotState().getRunState();
    if(run_state == SYSRUNSTATE::SYSRUN_STOP)
    {
        QMessageBox::warning(this, tr("提示"), tr("不允许的操作"));
        TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("停止失败"), tr("当前已处于停止状态"));
        return;
    }

    RobotClient::initance()->programStop(); //停止程序
    RobotClient::initance()->stop();
}

void steprunThread(bool &issteprun)
{
    RobotClient::initance()->setPlayState(SYSPLAY_PLAY);    //在线模式先运行程序
    RobotClient::initance()->setRunState(SYSRUN_RUN);       //服务端开启运行状态
    RobotClient::initance()->setDebugState(SWITCHON);       //debug模式下调试
    RobotClient::initance()->m_bSingle = true;
    RobotClient::initance()->m_bRunning = true;
    RobotClient::initance()->setTeach(ABB);                 //abb模式解析程序
    RobotClient::initance()->runABB();
    RobotClient::initance()->programStop();                 //停止运行
    RobotClient::initance()->m_bRunning = false;
    RobotClient::initance()->setDebugState(SWITCHOFF);      //还原状态
    RobotClient::initance()->m_bSingle = false;
    RobotClient::initance()->setPlayState(SYSPLAY_TEACH);
    RobotClient::initance()->setTeach(HR);
    issteprun = false;
    TeachState::instance()->SetRunState(false);
}

void Widget::ProgramRightStepRun()
{
    if(!issteprun_)
    {
        std::thread t(std::bind(steprunThread, std::ref(issteprun_)));
        t.detach();
        issteprun_ = true;
    }

    TeachState::instance()->SetRunState(true);
    emit ProgramStart();

    RobotClient::initance()->step();
}

void Widget::ProgramLeftStepRun()
{
    if(!issteprun_)
    {
        std::thread t(std::bind(steprunThread, std::ref(issteprun_)));
        t.detach();
        issteprun_ = true;
    }

    ProgramPointer pr = RobotClient::initance()->getProgramPointer();
    pr.nSentence--;

    TeachState::instance()->SetRunState(true);
    emit ProgramStart();

    auto point = EditData::instance()->GetRoutPoint(pr.nFunction, pr.nSentence);
    if(point.second == -1)
    {
        return;
    }
    auto &rout = EditData::instance()->GetRoutData(point.first);
    auto head = rout.code.at(point.second).section(" ", 0, 0);
    if(head == "WHILE" || head == "FOR")
    {
        return;
    }

    RobotClient::initance()->setPointer(pr);
    RobotClient::initance()->step();
}

void Widget::ConnectInit()
{
    connect(this, &Widget::SetScale, hardware_, &Hardware::SetScale);

    connect(this, &Widget::SetScale, main_widget_, &MainWidget::SetScale);

    connect(this, &Widget::ProgramStart, main_widget_->program_edit_->edit_main_, &ProgEditMain::StartRun);

    connect(this, &Widget::ProgramStart, main_widget_->auto_produce_, &AutoProduce::StartRun);

    connect(hardware_->zoom_widget_, &QPushButton::clicked, this, &Widget::Adapt);

    connect(hardware_->enabled_, &QPushButton::clicked, this, &Widget::HardwareEnabled);

    connect(hardware_->upper_, &HardwareButton::Press, this, &Widget::HardwareLower);

    connect(hardware_->lower_, &HardwareButton::Press, this, &Widget::HardwareUpper);

    connect(hardware_->left_, &HardwareButton::Press, this, &Widget::HardwareRight);

    connect(hardware_->right_, &HardwareButton::Press, this, &Widget::HardwareLeft);

    connect(hardware_->levo_, &HardwareButton::Press, this, &Widget::HardwareLevo);

    connect(hardware_->cum_sole_, &HardwareButton::Press, this, &Widget::HardwareCumSole);

    connect(hardware_->upper_, &HardwareButton::Release, this, &Widget::Stop);

    connect(hardware_->lower_, &HardwareButton::Release, this, &Widget::Stop);

    connect(hardware_->left_, &HardwareButton::Release, this, &Widget::Stop);

    connect(hardware_->right_, &HardwareButton::Release, this, &Widget::Stop);

    connect(hardware_->levo_, &HardwareButton::Release, this, &Widget::Stop);

    connect(hardware_->cum_sole_, &HardwareButton::Release, this, &Widget::Stop);

    connect(hardware_->stop_execution_1_, &QPushButton::clicked, this, &Widget::ProgramStop);

    connect(hardware_->stop_execution_2_, &QPushButton::clicked, this, &Widget::ProgramStop);

    connect(this, &Widget::RunProgram, this, &Widget::ProgramRun);

    connect(hardware_->custom_key1_1_, &ProgKeysButton::Press, this, &Widget::PK1Clicked);

    connect(hardware_->custom_key2_1_, &ProgKeysButton::Press, this, &Widget::PK2Clicked);

    connect(hardware_->custom_key3_1_, &ProgKeysButton::Press, this, &Widget::PK3Clicked);

    connect(hardware_->custom_key4_1_, &ProgKeysButton::Press, this, &Widget::PK4Clicked);

    connect(hardware_->custom_key1_2_, &ProgKeysButton::Press, this, &Widget::PK1Clicked);

    connect(hardware_->custom_key2_2_, &ProgKeysButton::Press, this, &Widget::PK2Clicked);

    connect(hardware_->custom_key3_2_, &ProgKeysButton::Press, this, &Widget::PK3Clicked);

    connect(hardware_->custom_key4_2_, &ProgKeysButton::Press, this, &Widget::PK4Clicked);

    connect(hardware_->custom_key1_1_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->custom_key2_1_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->custom_key3_1_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->custom_key4_1_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->custom_key1_2_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->custom_key2_2_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->custom_key3_2_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->custom_key4_2_, &ProgKeysButton::Release, this, &Widget::PKRelease);

    connect(hardware_->step_left_1_, &QPushButton::clicked, this, &Widget::LeftStep);

    connect(hardware_->step_left_2_, &QPushButton::clicked, this, &Widget::LeftStep);

    connect(hardware_->right_left_1_, &QPushButton::clicked, this, &Widget::RightStep);

    connect(hardware_->right_left_2_, &QPushButton::clicked, this, &Widget::RightStep);

    connect(this, &Widget::LeftStepRunProgram, this, &Widget::ProgramLeftStepRun);

    connect(this, &Widget::RightStepRunProgram, this, &Widget::ProgramRightStepRun);

    connect(hardware_->start_execution_1_, &QPushButton::clicked,
            [=]()
    {
        if(!TeachState::instance()->GetControlState())
        {
            QMessageBox::warning(this, tr("启动程序"), tr("不允许的操作"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("无控制权"));
            return;
        }
        if(!TeachState::instance()->GetElect())
        {
            QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("电机关闭"));
            return;
        }
        if(!TeachState::instance()->GetEnabled() && TeachState::instance()->GetRunMode() != RunMode::AutoMod)
        {
            QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("使能关闭"));
            return;
        }
        auto run_state = RobotClient::initance()->getRobotState().getRunState();
        if(run_state == SYSRUNSTATE::SYSRUN_RUN)
        {
            if(!TeachState::instance()->GetRunState())
                TeachState::instance()->SetRunState(true);
            QMessageBox::warning(this, tr("启动程序"), tr("由于至少在执行一个任务，所以无法更改执行模式"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("程序正在运行"));
            return;
        }
        else
        {
            if(TeachState::instance()->GetRunState())
                TeachState::instance()->SetRunState(false);
        }

        if(TeachState::instance()->GetCodeState()) //只有代码被修改才重新拼接发送给服务端
            EditData::instance()->GetSendCode(std::bind(&Widget::Run, this, std::placeholders::_1, std::placeholders::_2));
        else
            this->ProgramRun();
    });

    connect(hardware_->start_execution_2_, &QPushButton::clicked,
            [=]()
    {
        if(!TeachState::instance()->GetControlState())
        {
            QMessageBox::warning(this, tr("启动程序"), tr("不允许的操作"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("无控制权"));
            return;
        }
        if(!TeachState::instance()->GetElect())
        {
            QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("电机关闭"));
            return;
        }
        if(!TeachState::instance()->GetEnabled() && TeachState::instance()->GetRunMode() != RunMode::AutoMod)
        {
            QMessageBox::warning(this, tr("启动程序"), tr("启动失败。控制器安全访问限制机制拒绝此操作"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("使能关闭"));
            return;
        }
        auto run_state = RobotClient::initance()->getRobotState().getRunState();
        if(run_state == SYSRUNSTATE::SYSRUN_RUN)
        {
            if(!TeachState::instance()->GetRunState())
                TeachState::instance()->SetRunState(true);
            QMessageBox::warning(this, tr("启动程序"), tr("由于至少在执行一个任务，所以无法更改执行模式"));
            TeachState::instance()->SendLog(EventLogType::ErrorPop, tr("运行失败"), tr("程序正在运行"));
            return;
        }
        else
        {
            if(TeachState::instance()->GetRunState())
                TeachState::instance()->SetRunState(false);
        }

        if(TeachState::instance()->GetCodeState())
            EditData::instance()->GetSendCode(std::bind(&Widget::Run, this, std::placeholders::_1, std::placeholders::_2));
        else
            this->ProgramRun();
    });

    connect(hardware_->show_widget_, &QPushButton::clicked,
            [=]()
    {
        main_widget_->show();
        this->resize(width_, hardware_->height());
        this->move(this->x() - (width_ - hardware_->width()), this->y());
        hardware_->move(main_widget_->width(), 0);
    });

    connect(hardware_->hide_widget_, &QPushButton::clicked,
            [=]()
    {
        int width = main_widget_->width();
        main_widget_->hide();
        this->resize(hardware_->width(), hardware_->height());
        hardware_->move(0, 0);
        this->move(this->x() + width, this->y());
    });

    connect(hardware_->minimize_widget_, &QPushButton::clicked, this, &Widget::showMinimized);

    connect(hardware_->close_widget_, &QPushButton::clicked,
            [=]()
    {
        close();
    });

    connect(main_widget_->win_func_menu_->reboot_, &QPushButton::clicked,
            [=]()
    {
        QMessageBox::about(this, tr("提示"), tr("示教器即将关闭，请手动启动"));
        close();
    });

    connect(hardware_->linear_relocate_, &QPushButton::clicked,
            [=]()
    {
        if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
            return;

        auto mod = TeachState::instance()->GetActionMode();
        if(mod == ActionMode::Linear)
            TeachState::instance()->SetActionMode(ActionMode::Relocate);
        else
            TeachState::instance()->SetActionMode(ActionMode::Linear);
    });

    connect(hardware_->select_axis_, &QPushButton::clicked,
            [=]()
    {
        if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
            return;

        auto mod = TeachState::instance()->GetActionMode();
        if(mod == ActionMode::Axis1_3)
            TeachState::instance()->SetActionMode(ActionMode::Axis4_6);
        else
            TeachState::instance()->SetActionMode(ActionMode::Axis1_3);

    });

    connect(hardware_->cut_spike_, &QPushButton::clicked,
            [=]()
    {
        if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
            return;

        auto mod = TeachState::instance()->GetIncrement();
        if(mod == Increment::None)
            TeachState::instance()->SetIncrement(Increment::Small);
        else
            TeachState::instance()->SetIncrement(Increment::None);
    });
}
