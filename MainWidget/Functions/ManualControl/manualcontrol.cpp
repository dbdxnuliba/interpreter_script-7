#include "manualcontrol.h"
#include "MainWidget/Functions/ProgramEditor/programeditordata.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>

using namespace rclib;

ManualControl::ManualControl(QWidget *parent, int type) : WindowObject(type, parent)
{
    stack_widget_ = new StackedWidObject(this);
    manu_widget_ = new ManuMainWidget(this);
    aim_button_ = new QPushButton(tr("对准..."), button_widget_);
    go_to_button_ = new QPushButton(tr("转到..."), button_widget_);
    start_button_ = new QPushButton(tr("启动..."), button_widget_);

    aim_button_->setFlat(true);
    go_to_button_->setFlat(true);
    start_button_->setFlat(true);

    stack_widget_->SetMainWidget(manu_widget_);

    unit_widget_ = new UnitWindow(this);
    action_widget_ = new ActionAttr(this);
    ucs_widget_ = new UcsAttr(this);
    lever_widget_ = new LeverLockAttr(this);
    increment_widget_ = new IncrementAttr(this);
    var_edit_widget_ = new VarEditInterface(this);
    format_pos_ = new FormatPosition(this);

    stack_widget_->AddWidget(unit_widget_, "unit_widget");
    stack_widget_->AddWidget(action_widget_, "action_widget");
    stack_widget_->AddWidget(ucs_widget_, "ucs_widget");
    stack_widget_->AddWidget(lever_widget_, "lever_widget");
    stack_widget_->AddWidget(increment_widget_, "increment_widget");
    stack_widget_->AddWidget(var_edit_widget_, "var_edit_widget");
    stack_widget_->AddWidget(format_pos_, "format_pos");

    action_widget_->GetImageLayout()->addStretch();
    AttributeBox *att_box = manu_widget_->attribute_box_;
    att_box->unitv_button_->setText("ROB_1");
    att_box->actionv_button_->setText(action_widget_->GetSelectText());
    att_box->ucsv_button_->setText(ucs_widget_->GetSelectText());
    att_box->ucs_button_->setEnabled(false);
    att_box->ucsv_button_->setEnabled(false);
    att_box->lever_lockingv_button_->setText(lever_widget_->GetSelectText());
    att_box->incrementv_button_->setText(increment_widget_->GetSelectText());
    att_box->toolv_button_->setText(TeachState::instance()->GetCurrentTool());
    att_box->workv_button_->setText(TeachState::instance()->GetCurrentWork());
    att_box->payloadv_button_->setText("load0");

    ConnectInit();
}

void ManualControl::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    if(stack_widget_->GetName() != "main")
    {
        main_widget_->resize(this->width(), this->height() - GetIcoLabel()->height());
        main_widget_->move(0, GetUnderLabel()->y() + GetUnderLabel()->height());
    }
    else
    {
        WindowObject::resizeEvent(nullptr);

        aim_button_->resize(126*ScaleW(), button_widget_->height());
        go_to_button_->resize(aim_button_->size());
        start_button_->resize(go_to_button_->size());

        aim_button_->move(0, 0);
        go_to_button_->move(aim_button_->x() + aim_button_->width(), aim_button_->y());
        start_button_->move(go_to_button_->x() + go_to_button_->width(), go_to_button_->y());
    }

    stack_widget_->resize(main_widget_->size());
    stack_widget_->move(main_widget_->pos());
}

CreateNewVar *ManualControl::GetCreateNewVar()
{
    return var_edit_widget_->GetCreate();
}

void ManualControl::ConnectInit()
{
    AttributeBox *att_box = manu_widget_->attribute_box_;

    connect(att_box->unit_button_, &LeftButton::clicked, this, &ManualControl::ShowUnitWidget);

    connect(att_box->unitv_button_, &LeftButton::clicked, this, &ManualControl::ShowUnitWidget);

    connect(unit_widget_, &UnitWindow::Define,
            [=]()
    {
        unit_widget_->hide();

        ShowControl();
    });

    connect(unit_widget_, &UnitWindow::Cancel,
            [=]()
    {
        unit_widget_->hide();

        ShowControl();
    });

    connect(att_box->action_button_, &LeftButton::clicked, this, &ManualControl::ShowActionWidget);

    connect(att_box->actionv_button_, &LeftButton::clicked, this, &ManualControl::ShowActionWidget);

    connect(action_widget_->cancel_button_, &QPushButton::clicked, this, &ManualControl::ShowControl);

    connect(action_widget_->define_button_, &QPushButton::clicked,
            [=]()
    {
        ActionMode mod;
        auto text = action_widget_->GetSelectText();

        if(text == action_widget_->week1_button_->GetText())
            mod = ActionMode::Axis1_3;
        else if(text == action_widget_->week4_button_->GetText())
            mod = ActionMode::Axis4_6;
        else if(text == action_widget_->linear_button_->GetText())
            mod = ActionMode::Linear;
        else
            mod = ActionMode::Relocate;

        TeachState::instance()->SetActionMode(mod);

        ShowControl();
    });

    connect(TeachState::instance().data(), &TeachState::ActionMod,
            [=](ActionMode mod)
    {
        switch(mod)
        {
        case ActionMode::Axis1_3:
        {
            att_box->actionv_button_->setText(action_widget_->week1_button_->GetText());
            manu_widget_->pos_box_->SetType(0);
            manu_widget_->joystick_box_->SetButton(0);
            att_box->ucs_button_->setEnabled(false);
            att_box->ucsv_button_->setEnabled(false);
            break;
        }
        case ActionMode::Axis4_6:
        {
            att_box->actionv_button_->setText(action_widget_->week4_button_->GetText());
            manu_widget_->pos_box_->SetType(0);
            manu_widget_->joystick_box_->SetButton(1);
            att_box->ucs_button_->setEnabled(false);
            att_box->ucsv_button_->setEnabled(false);
            break;
        }
        case ActionMode::Linear:
        {
            att_box->actionv_button_->setText(action_widget_->linear_button_->GetText());
            manu_widget_->pos_box_->SetType(1);
            manu_widget_->joystick_box_->SetButton(2);
            att_box->ucs_button_->setEnabled(true);
            att_box->ucsv_button_->setEnabled(true);
            break;
        }
        case ActionMode::Relocate:
        {
            att_box->actionv_button_->setText(action_widget_->relocation_button_->GetText());
            manu_widget_->pos_box_->SetType(1);
            manu_widget_->joystick_box_->SetButton(2);
            att_box->ucs_button_->setEnabled(true);
            att_box->ucsv_button_->setEnabled(true);
            break;
        }
        }
    });

    connect(action_widget_->week1_button_, &ImageButton::clicked,
            [=]()
    {
        att_box->ucs_button_->setEnabled(false);
        att_box->ucsv_button_->setEnabled(false);

        manu_widget_->joystick_box_->SetButton(0);
    });

    connect(action_widget_->week4_button_, &ImageButton::clicked,
            [=]()
    {
        att_box->ucs_button_->setEnabled(false);
        att_box->ucsv_button_->setEnabled(false);

        manu_widget_->joystick_box_->SetButton(1);
    });

    connect(action_widget_->linear_button_, &ImageButton::clicked,
            [=]()
    {
        att_box->ucs_button_->setEnabled(true);
        att_box->ucsv_button_->setEnabled(true);

        att_box->ucsv_button_->setText(ucs_widget_->relocation_button_->GetText());

        manu_widget_->joystick_box_->SetButton(2);
    });

    connect(action_widget_->relocation_button_, &ImageButton::clicked,
            [=]()
    {
        att_box->ucs_button_->setEnabled(true);
        att_box->ucsv_button_->setEnabled(true);

        att_box->ucsv_button_->setText(ucs_widget_->linear_button_->GetText());

        manu_widget_->joystick_box_->SetButton(3);
    });

    connect(att_box->ucs_button_, &LeftButton::clicked, this, &ManualControl::ShowUcsWidget);

    connect(att_box->ucsv_button_, &LeftButton::clicked, this, &ManualControl::ShowUcsWidget);

    connect(ucs_widget_->cancel_button_, &QPushButton::clicked, this, &ManualControl::ShowControl);

    connect(ucs_widget_->define_button_, &QPushButton::clicked,
            [=]()
    {
        CoordinSys cors;
        auto text = ucs_widget_->GetSelectText();

        if(text == ucs_widget_->week1_button_->GetText())
            cors = CoordinSys::Earth;
        else if(text == ucs_widget_->week4_button_->GetText())
            cors = CoordinSys::Base;
        else if(text == ucs_widget_->linear_button_->GetText())
            cors = CoordinSys::Tool;
        else
            cors = CoordinSys::Work;

        TeachState::instance()->SetCoordinSys(cors);

        ShowControl();
    });

    connect(TeachState::instance().data(), &TeachState::CoordSys,
            [=](CoordinSys cors)
    {
        switch(cors)
        {
        case CoordinSys::Earth:
        {
            att_box->ucsv_button_->setText(ucs_widget_->week1_button_->GetText());
            break;
        }
        case CoordinSys::Base:
        {
            att_box->ucsv_button_->setText(ucs_widget_->week4_button_->GetText());
            break;
        }
        case CoordinSys::Tool:
        {
            att_box->ucsv_button_->setText(ucs_widget_->linear_button_->GetText());
            break;
        }
        case CoordinSys::Work:
        {
            att_box->ucsv_button_->setText(ucs_widget_->relocation_button_->GetText());
            break;
        }
        }
    });

    connect(att_box->lever_lockingv_button_, &LeftButton::clicked, this, &ManualControl::ShowLeverWidget);

    connect(att_box->lever_locking_button_, &LeftButton::clicked, this, &ManualControl::ShowLeverWidget);

    connect(lever_widget_->cancel_button_, &QPushButton::clicked, this, &ManualControl::ShowControl);

    connect(lever_widget_->define_button_, &QPushButton::clicked,
            [=]()
    {
        QString str_lock = lever_widget_->GetSelectText();

        att_box->lever_lockingv_button_->setText(str_lock);

        bool lock1 = false;
        bool lock2 = false;
        bool lock3 = false;

        if(str_lock != lever_widget_->week1_button_->GetText())
        {
            QStringList list = str_lock.split("&&");

            for(auto str : list)
            {
                if(str == lever_widget_->week4_button_->GetText())
                    lock2 = true;
                else if(str == lever_widget_->linear_button_->GetText())
                    lock1 = true;
                else if(str == lever_widget_->relocation_button_->GetText())
                    lock3 = true;
            }
        }

        manu_widget_->joystick_box_->SetLock(lock1, lock2, lock3);

        ShowControl();
    });

    connect(att_box->increment_button_, &LeftButton::clicked, this, &ManualControl::ShowIncrementWidget);

    connect(att_box->incrementv_button_, &LeftButton::clicked, this, &ManualControl::ShowIncrementWidget);

    connect(increment_widget_->cancel_button_, &QPushButton::clicked, this, &ManualControl::ShowControl);

    connect(increment_widget_->define_button_, &QPushButton::clicked,
            [=]()
    {
        auto str = increment_widget_->GetSelectText();

        Increment incrt;
        if(str == increment_widget_->week1_button_->GetText())
            incrt = Increment::None;
        else if(str == increment_widget_->week4_button_->GetText())
            incrt = Increment::Small;
        else if(str == increment_widget_->linear_button_->GetText())
            incrt = Increment::Centre;
        else if(str == increment_widget_->relocation_button_->GetText())
            incrt = Increment::Large;
        else
            incrt = Increment::User;

        TeachState::instance()->SetIncrement(incrt);

        ShowControl();
    });

    connect(TeachState::instance().data(), &TeachState::Incrent,
            [=](Increment incrt)
    {
        switch(incrt)
        {
        case Increment::None:
        {
            att_box->incrementv_button_->setText(increment_widget_->week1_button_->GetText());
            break;
        }
        case Increment::Small:
        {
            att_box->incrementv_button_->setText(increment_widget_->week4_button_->GetText());
            break;
        }
        case Increment::Centre:
        {
            att_box->incrementv_button_->setText(increment_widget_->linear_button_->GetText());
            break;
        }
        case Increment::Large:
        {
            att_box->incrementv_button_->setText(increment_widget_->relocation_button_->GetText());
            break;
        }
        case Increment::User:
        {
            att_box->incrementv_button_->setText(increment_widget_->user_button_->GetText());
        }
        }
    });

    connect(att_box->work_button_, &LeftButton::clicked, this, &ManualControl::ShowVarEditWidget);

    connect(att_box->workv_button_, &LeftButton::clicked, this, &ManualControl::ShowVarEditWidget);

    connect(att_box->tool_button_, &LeftButton::clicked, this, &ManualControl::ShowVarEditWidget);

    connect(att_box->toolv_button_, &LeftButton::clicked, this, &ManualControl::ShowVarEditWidget);

    connect(att_box->payload_button_, &LeftButton::clicked, this, &ManualControl::ShowVarEditWidget);

    connect(att_box->payloadv_button_, &LeftButton::clicked, this, &ManualControl::ShowVarEditWidget);

    connect(var_edit_widget_, &VarEditInterface::CancelButton, this, &ManualControl::ShowControl);

    connect(var_edit_widget_, &VarEditInterface::DefineButton,
            [=](QString data_type, QString select_str)
    {
        if(data_type == "loaddata")
        {
            TeachState::instance()->SetCurrentLoad(select_str);
        }
        else if(data_type == "tooldata")
        {
            try {
                if(!TeachState::instance()->SetCurrentTool(select_str))
                {
                    QMessageBox::warning(this, tr("错误"), tr("该坐标系不存在"));
                    return;
                }
            } catch (ABBException) {
                QString str = QString("%1's \"tload.mass\"" + tr("含有无效数据！纠正数据后重试")).arg(select_str);
                QMessageBox::warning(this, "错误", str);
            }
        }
        else if(data_type == "wobjdata")
        {
            if(!TeachState::instance()->SetCurrentWork(select_str))
            {
                QMessageBox::warning(this, tr("错误"), tr("该坐标系不存在"));
                return;
            }
        }
        else
        {
            return;
        }

        ShowControl();
    });

    connect(TeachState::instance().data(), &TeachState::CordTool,
            [=](QString name)
    {
        att_box->toolv_button_->setText(name);
    });

    connect(TeachState::instance().data(), &TeachState::CordWork,
            [=](QString name)
    {
        att_box->workv_button_->setText(name);
    });

    connect(TeachState::instance().data(), &TeachState::CordLoad,
            [=](QString name)
    {
        att_box->payloadv_button_->setText(name);
    });

    connect(manu_widget_->pos_box_, &PositionBox::ShowFormat,
            [=](int pos, int direc, int angle, int unit)
    {
        format_pos_->Init(pos, direc, angle, unit);

        stack_widget_->Change("format_pos");

        this->HideControl();
    });

    connect(format_pos_, &FormatPosition::DefineClicked,
            [=](int pos, int direc, int angle, int unit)
    {
        this->ShowControl();
        manu_widget_->pos_box_->SetFormat(pos, direc, angle, unit);

        stack_widget_->Change();
    });

    connect(format_pos_, &FormatPosition::CancelClicked,
            [=]()
    {
        this->ShowControl();

        stack_widget_->Change();
    });
}

void ManualControl::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
    var_edit_widget_->SetScale(scale_w, scale_h);
    unit_widget_->SetScale(scale_w, scale_h);
}

void ManualControl::HideControl()
{
    button_widget_->hide();
    this->resizeEvent(nullptr);
}

void ManualControl::ShowControl()
{
    stack_widget_->Change(); //跳转回主界面

    button_widget_->show();

    resizeEvent(nullptr);
}

void ManualControl::ShowUnitWidget()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, "错误", "当前运行模式下禁止该操作");
        return;
    }

    stack_widget_->Change("unit_widget");

    HideControl();
}

void ManualControl::ShowActionWidget()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }

    action_widget_->Init(manu_widget_->attribute_box_->actionv_button_->text());

    stack_widget_->Change("action_widget");

    HideControl();
}

void ManualControl::ShowUcsWidget()
{
    if(!TeachState::instance()->GetControlState())
    {
        QMessageBox::warning(this, tr("错误"), tr("无控制权"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("错误"), tr("程序正在运行"));
        return;
    }
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("错误"), tr("当前运行模式下禁止该操作"));
        return;
    }

    ucs_widget_->Init(manu_widget_->attribute_box_->ucsv_button_->text());

    stack_widget_->Change("ucs_widget");

    HideControl();
}

void ManualControl::ShowLeverWidget()
{
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("错误"), tr("程序正在运行"));
        return;
    }
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("错误"), tr("当前运行模式下禁止该操作"));
        return;
    }

    lever_widget_->LeverInit(manu_widget_->attribute_box_->lever_lockingv_button_->text());

    stack_widget_->Change("lever_widget");

    HideControl();
}

void ManualControl::ShowIncrementWidget()
{
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("错误"), tr("程序正在运行"));
        return;
    }
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("错误"), tr("当前运行模式下禁止该操作"));
        return;
    }

    increment_widget_->Init(manu_widget_->attribute_box_->incrementv_button_->text());

    stack_widget_->Change("increment_widget");

    HideControl();
}

void ManualControl::ShowVarEditWidget()
{
    if(!TeachState::instance()->GetControlState())
    {
        QMessageBox::warning(this, tr("错误"), tr("无控制权"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("错误"), tr("程序正在运行"));
        return;
    }
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("错误"), tr("当前运行模式下禁止该操作"));
        return;
    }

    LeftButton *button = dynamic_cast<LeftButton*>(sender());
    if(button == nullptr)
        return;

    AttributeBox *box = manu_widget_->attribute_box_;

    if(button == box->tool_button_ || button == box->toolv_button_) //工具坐标
    {
        var_edit_widget_->Init("tooldata", box->toolv_button_->text());
    }
    else if(button == box->work_button_ || button == box->workv_button_) //工件坐标
    {
        var_edit_widget_->Init("wobjdata", box->workv_button_->text());
    }
    else if(button == box->payload_button_ || button == box->payloadv_button_) //有效载荷
    {
        var_edit_widget_->Init("loaddata", box->payloadv_button_->text());
    }
    else
    {
        return;
    }

    stack_widget_->Change("var_edit_widget");

    HideControl();
}

AttributeBox::AttributeBox(QWidget *parent) : QGroupBox(parent)
{
    QString qss = "QGroupBox {border-color: rgb(156, 156, 156);border-width: 1px;border-style: solid;margin-top: 0.5ex;}";
    qss += "QGroupBox::title {subcontrol-origin: margin;subcontrol-position: top left;left: 10px;margin-left: 2px;padding: 0  0px;}";
    this->setStyleSheet(qss);
    this->setTitle(tr("点击属性并修改"));

    QGridLayout *layout = new QGridLayout(this);

    QHBoxLayout *uni_layout = new QHBoxLayout;
    unit_button_ = new LeftButton(tr("机械单元:"), this);
    unitv_button_ = new LeftButton(this);
    unit_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //使按钮之间的间隔消失
    unitv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    uni_layout->addWidget(unit_button_);
    uni_layout->addWidget(unitv_button_);

    QHBoxLayout *acc_layout = new QHBoxLayout;
    accuracy_button_ = new LeftButton(tr("绝对精度:"), this);
    accuracyv_button_ = new LeftButton(this);
    accuracy_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    accuracyv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    acc_layout->addWidget(accuracy_button_);
    acc_layout->addWidget(accuracyv_button_);

    QHBoxLayout *act_layout = new QHBoxLayout;
    action_button_ = new LeftButton(tr("动作模式:"), this);
    actionv_button_ = new LeftButton(this);
    action_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    actionv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    act_layout->addWidget(action_button_);
    act_layout->addWidget(actionv_button_);

    QHBoxLayout *ucs_layout = new QHBoxLayout;
    ucs_button_ = new LeftButton(tr("坐标系:"), this);
    ucsv_button_ = new LeftButton(this);
    ucs_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ucsv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ucs_layout->addWidget(ucs_button_);
    ucs_layout->addWidget(ucsv_button_);

    QHBoxLayout *too_layout = new QHBoxLayout;
    tool_button_ = new LeftButton(tr("工具坐标:"), this);
    toolv_button_ = new LeftButton(this);
    tool_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    too_layout->addWidget(tool_button_);
    too_layout->addWidget(toolv_button_);

    QHBoxLayout *wor_layout = new QHBoxLayout;
    work_button_ = new LeftButton(tr("工件坐标:"), this);
    workv_button_ = new LeftButton(this);
    work_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    workv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wor_layout->addWidget(work_button_);
    wor_layout->addWidget(workv_button_);

    QHBoxLayout *pay_layout = new QHBoxLayout;
    payload_button_ = new LeftButton(tr("有效载荷:"), this);
    payloadv_button_ = new LeftButton(this);
    payload_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    payloadv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pay_layout->addWidget(payload_button_);
    pay_layout->addWidget(payloadv_button_);

    QHBoxLayout *lev_layout = new QHBoxLayout;
    lever_locking_button_ = new LeftButton(tr("操纵杆锁定:"), this);
    lever_lockingv_button_ = new LeftButton(this);
    lever_locking_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lever_lockingv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lev_layout->addWidget(lever_locking_button_);
    lev_layout->addWidget(lever_lockingv_button_);

    QHBoxLayout *inc_layout = new QHBoxLayout;
    increment_button_ = new LeftButton(tr("增量:"), this);
    incrementv_button_ = new LeftButton(this);
    increment_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    incrementv_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    inc_layout->addWidget(increment_button_);
    inc_layout->addWidget(incrementv_button_);

    layout->addLayout(uni_layout, 0, 0);
    layout->addLayout(acc_layout, 1, 0);
    layout->addLayout(act_layout, 2, 0);
    layout->addLayout(ucs_layout, 3, 0);
    layout->addLayout(too_layout, 4, 0);
    layout->addLayout(wor_layout, 5, 0);
    layout->addLayout(pay_layout, 6, 0);
    layout->addLayout(lev_layout, 7, 0);
    layout->addLayout(inc_layout, 8, 0);
    layout->setSpacing(0);

    this->setLayout(layout);

    accuracy_button_->setEnabled(false);
    accuracyv_button_->setText("Off");
    accuracyv_button_->setEnabled(false);
}

LeftButton::LeftButton(QWidget *parent) : QPushButton(parent)
{
    this->setFlat(true);
    this->setStyleSheet("text-align: left");
}

LeftButton::LeftButton(QString name, QWidget *parent) : QPushButton(parent)
{
    this->setFlat(true);
    this->setText(name);
    this->setStyleSheet("text-align: left");
}

PositionBox::PositionBox(QWidget *parent) : QGroupBox(parent)
{
    QString qss = "QGroupBox {border-color: rgb(156, 156, 156);border-width: 1px;border-style: solid;margin-top: 0.5ex;}";
    qss += "QGroupBox::title {subcontrol-origin: margin;subcontrol-position: top left;left: 10px;margin-left: 2px;padding: 0  0px;}";
    this->setStyleSheet(qss);
    this->setTitle(tr("位置"));

    QGridLayout *layout = new QGridLayout(this);

    QHBoxLayout *p1_layout = new QHBoxLayout;
    pos1_label_ = new QLabel("1:", this);
    posv1_label_ = new QLabel(this);
    p1_layout->addWidget(pos1_label_);
    p1_layout->addWidget(posv1_label_);

    QHBoxLayout *p2_layout = new QHBoxLayout;
    pos2_label_ = new QLabel("2:", this);
    posv2_label_ = new QLabel(this);
    p2_layout->addWidget(pos2_label_);
    p2_layout->addWidget(posv2_label_);

    QHBoxLayout *p3_layout = new QHBoxLayout;
    pos3_label_ = new QLabel("3:", this);
    posv3_label_ = new QLabel(this);
    p3_layout->addWidget(pos3_label_);
    p3_layout->addWidget(posv3_label_);

    QHBoxLayout *p4_layout = new QHBoxLayout;
    pos4_label_ = new QLabel("4:", this);
    posv4_label_ = new QLabel(this);
    p4_layout->addWidget(pos4_label_);
    p4_layout->addWidget(posv4_label_);

    QHBoxLayout *p5_layout = new QHBoxLayout;
    pos5_label_ = new QLabel("5:", this);
    posv5_label_ = new QLabel(this);
    p5_layout->addWidget(pos5_label_);
    p5_layout->addWidget(posv5_label_);

    QHBoxLayout *p6_layout = new QHBoxLayout;
    pos6_label_ = new QLabel("6:", this);
    posv6_label_ = new QLabel(this);
    p6_layout->addWidget(pos6_label_);
    p6_layout->addWidget(posv6_label_);

    QHBoxLayout *p7_layout = new QHBoxLayout;
    pos7_label_ = new QLabel(this);
    posv7_label_ = new QLabel(this);
    p7_layout->addWidget(pos7_label_);
    p7_layout->addWidget(posv7_label_);

    QLabel *decorate_label_ = new QLabel(this);

    format_button_ = new QPushButton(tr("位置格式"), this);

    layout->addLayout(p1_layout, 0, 0);
    layout->addLayout(p2_layout, 1, 0);
    layout->addLayout(p3_layout, 2, 0);
    layout->addLayout(p4_layout, 3, 0);
    layout->addLayout(p5_layout, 4, 0);
    layout->addLayout(p6_layout, 5, 0);
    layout->addLayout(p7_layout, 6, 0);
    layout->addWidget(decorate_label_, 7, 0);
    layout->addWidget(format_button_, 8, 0);
    layout->setSpacing(0);

    this->setLayout(layout);

    pos_ = 2;
    direc_ = 0;
    angle_ = 0;
    unit_ = 0;

    pos7_label_->hide();
    posv7_label_->hide();

    timer_ = new QTimer(this);

    connect(timer_, &QTimer::timeout, this, &PositionBox::Timer);

    timer_->start(100);

    connect(format_button_, &QPushButton::clicked,
            [=]()
    {
        emit ShowFormat(pos_, direc_, angle_, unit_);
    });
}

void PositionBox::SetFormat(int pos, int direc, int angle, int unit)
{
    pos_ = pos;
    direc_ = direc;
    angle_ = angle;
    unit_ = unit;

    if(type_ == 1)
    {
        pos1_label_->setText("X:");
        pos2_label_->setText("Y:");
        pos3_label_->setText("Z:");
        if(direc_ == 0)
        {
            pos7_label_->show();
            posv7_label_->show();

            pos4_label_->setText("q1:");
            pos5_label_->setText("q2:");
            pos6_label_->setText("q3:");
            pos7_label_->setText("q4:");
        }
        else
        {
            pos7_label_->hide();
            posv7_label_->hide();

            pos4_label_->setText("EZ:");
            pos5_label_->setText("EY:");
            pos6_label_->setText("EX:");
        }
    }
}

void PositionBox::SetType(int type)
{
    type_ = type;

    if(type == 0)
    {
        pos1_label_->setText("1:");
        pos2_label_->setText("2:");
        pos3_label_->setText("3:");
        pos4_label_->setText("4:");
        pos5_label_->setText("5:");
        pos6_label_->setText("6:");
        if(!pos7_label_->isHidden())
        {
            pos7_label_->hide();
            posv7_label_->hide();
        }
    }
    else
    {
        pos1_label_->setText("X:");
        pos2_label_->setText("Y:");
        pos3_label_->setText("Z:");
        if(direc_ == 0)
        {
            pos7_label_->show();
            posv7_label_->show();

            pos4_label_->setText("q1:");
            pos5_label_->setText("q2:");
            pos6_label_->setText("q3:");
            pos7_label_->setText("q4:");
        }
        else
        {
            pos7_label_->hide();
            posv7_label_->hide();

            pos4_label_->setText("EZ:");
            pos5_label_->setText("EY:");
            pos6_label_->setText("EX:");
        }
    }
}

void PositionBox::Timer()
{
    auto motion = RobotClient::initance()->getRobotMotion();

    if(type_ == 0) //点动
    {
        Joints position = motion.getCurrentJointPosition();

        double pos1 = position.getValue(JOINT_1);
        double pos2 = position.getValue(JOINT_2);
        double pos3 = position.getValue(JOINT_3);
        double pos4 = position.getValue(JOINT_4);
        double pos5 = position.getValue(JOINT_5);
        double pos6 = position.getValue(JOINT_6);

        QString str;
        if(unit_ == 0)
        {
            str = " °";
        }
        else
        {
            str = tr("  弧度");

            pos1 = Convert::AngleToRadian(pos1);
            pos2 = Convert::AngleToRadian(pos2);
            pos3 = Convert::AngleToRadian(pos3);
            pos4 = Convert::AngleToRadian(pos4);
            pos5 = Convert::AngleToRadian(pos5);
            pos6 = Convert::AngleToRadian(pos6);
        }

        posv1_label_->setText(QString::number(pos1) + str); //度数
        posv2_label_->setText(QString::number(pos2) + str);
        posv3_label_->setText(QString::number(pos3) + str);
        posv4_label_->setText(QString::number(pos4) + str);
        posv5_label_->setText(QString::number(pos5) + str);
        posv6_label_->setText(QString::number(pos6) + str);
    }
    else //重定位与线性
    {
        pos1_label_->setText("X:");
        pos2_label_->setText("Y:");
        pos3_label_->setText("Z:");

        Terminal tal= motion.getCurrentTerminal();
        double pos1 = tal.getValue(TERMINAL_X);
        double pos2 = tal.getValue(TERMINAL_Y);
        double pos3 = tal.getValue(TERMINAL_Z);
        double pos4 = tal.getValue(TERMINAL_A);
        double pos5 = tal.getValue(TERMINAL_B);
        double pos6 = tal.getValue(TERMINAL_C);

        QString str;
        if(direc_ == 0) //四元数
        {
            Euler er(pos4, pos5, pos6);

            auto qn = Convert::EulerToQuaternion(er);
            double pos7 = 0;

            pos4 = qn.w;
            pos5 = qn.x;
            pos6 = qn.y;
            pos7 = qn.z;

            posv7_label_->setText(QString::number(pos7));
        }
        else if(direc_ == 1) //欧拉角
        {
            if(unit_ == 0) //度
            {
                str = " °";
            }
            else //弧度
            {
                str = " " + tr("弧度");

                pos4 = Convert::AngleToRadian(pos4);
                pos5 = Convert::AngleToRadian(pos5);
                pos6 = Convert::AngleToRadian(pos6);
            }
        }

        posv1_label_->setText(QString::number(pos1) + " mm"); //xyz位置与欧拉角
        posv2_label_->setText(QString::number(pos2) + " mm");
        posv3_label_->setText(QString::number(pos3) + " mm");
        posv4_label_->setText(QString::number(pos4) + str);
        posv5_label_->setText(QString::number(pos5) + str);
        posv6_label_->setText(QString::number(pos6) + str);
    }
}

JoystickBox::JoystickBox(QWidget *parent) : QGroupBox(parent)
{
    QString qss = "QGroupBox {border-color: rgb(156, 156, 156);border-width: 1px;border-style: solid;margin-top: 0.5ex;}";
    qss += "QGroupBox::title {subcontrol-origin: margin;subcontrol-position: top left;left: 10px;margin-left: 2px;padding: 0  0px;}";
    this->setStyleSheet(qss);
    this->setTitle(tr("操纵杆方向"));

    QHBoxLayout *layout = new QHBoxLayout(this);

    button1_ = new ImageButton(this);
    button2_ = new ImageButton(this);
    button3_ = new ImageButton(this);
    button1_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button2_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button3_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button1_->setEnabled(false);
    button2_->setEnabled(false);
    button3_->setEnabled(false);

    layout->addStretch();
    layout->addWidget(button1_);
    layout->addWidget(button2_);
    layout->addWidget(button3_);
    layout->addStretch();

    this->setLayout(layout);

    lock1_ = false;
    lock2_ = false;
    lock3_ = false;
    mod_ = 0; //默认为1-3轴
    SetButton(mod_);
}

void JoystickBox::SetButton(int mod)
{
    mod_ = mod;
    if(mod == 0)
    {
        button1_->SetText("2");
        button2_->SetText("1");
        button3_->SetText("3");

        button1_->SetImage(":/image/Image/MainWidget/joystick_2_5.png");
        button2_->SetImage(":/image/Image/MainWidget/joystick_1.png");
        button3_->SetImage(":/image/Image/MainWidget/joystick_3.png");
    }
    else if(mod == 1)
    {
        button1_->SetText("5");
        button2_->SetText("4");
        button3_->SetText("6");

        button1_->SetImage(":/image/Image/MainWidget/joystick_2_5.png");
        button2_->SetImage(":/image/Image/MainWidget/joystick_1.png");
        button3_->SetImage(":/image/Image/MainWidget/joystick_3.png");
    }
    else
    {
        button1_->SetText("X");
        button2_->SetText("Y");
        button3_->SetText("Z");

        button1_->SetImage(":/image/Image/MainWidget/joystick_x.png");
        button2_->SetImage(":/image/Image/MainWidget/joystick_y.png");
        button3_->SetImage(":/image/Image/MainWidget/joystick_z.png");
    }

    if(lock1_)
    {
        button1_->SetImage(":/image/Image/MainWidget/joyst1_lock.png");
    }

    if(lock2_)
    {
        button2_->SetImage(":/image/Image/MainWidget/joyst2_lock.png");
    }

    if(lock3_)
    {
        button3_->SetImage(":/image/Image/MainWidget/joyst3_lock.png");
    }
}

void JoystickBox::SetLock(bool lock1, bool lock2, bool lock3)
{
    lock1_ = lock1;
    lock2_ = lock2;
    lock3_ = lock3;

    SetButton(mod_);
}

void JoystickBox::SetOneLock(int num, bool lock)
{
    if(num == 1)
        lock1_ = lock;
    else if(num == 2)
        lock2_ = lock;
    else
        lock3_ = lock;

    SetButton(mod_);
}

bool JoystickBox::GetLock(int num)
{
    if(num == 1)
        return lock1_;
    else if(num == 2)
        return lock2_;
    else
        return lock3_;
}

ManuMainWidget::ManuMainWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    attribute_box_ = new AttributeBox(this);

    QVBoxLayout *vb_layout = new QVBoxLayout;
    pos_box_ = new PositionBox(this);
    joystick_box_ = new JoystickBox(this);
    vb_layout->addWidget(pos_box_);
    vb_layout->addWidget(joystick_box_);
    vb_layout->setStretchFactor(pos_box_, 5);
    vb_layout->setStretchFactor(joystick_box_, 2);

    layout->addWidget(attribute_box_);
    layout->addLayout(vb_layout);

    this->setLayout(layout);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}

AttributeBase::AttributeBase(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    head_widget_ = new QWidget(this);
    core_Widget_ = new QFrame(this);
    button_widget_ = new QWidget(this);
    layout->addWidget(head_widget_);
    layout->addWidget(core_Widget_);
    layout->addWidget(button_widget_);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStretchFactor(head_widget_, 2);
    layout->setStretchFactor(core_Widget_, 4);
    layout->setStretchFactor(button_widget_, 1);
    this->setLayout(layout);
    core_Widget_->setFrameShape(QFrame::Box);

    QVBoxLayout *head_layout = new QVBoxLayout(head_widget_);

    QHBoxLayout *tp_layout = new QHBoxLayout;
    opt_label_ = new QLabel(head_widget_);
    tips_opt_label_ = new QLabel(tr("当前选择:"), head_widget_);
    tp_layout->addWidget(tips_opt_label_);
    tp_layout->addWidget(opt_label_);

    tips_label_ = new QLabel(head_widget_);
    head_layout->addLayout(tp_layout);
    head_layout->addWidget(tips_label_);
    head_widget_->setLayout(head_layout);

    QPalette p;
    p.setColor(QPalette::Window, QColor(156, 156, 156));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
}

ActionAttr::ActionAttr(QWidget *parent) : AttributeBase(parent)
{
    tips_label_->setText(tr("选择动作模式。"));

    QVBoxLayout *core_layout = new QVBoxLayout(core_Widget_);

    image_layout_ = new QHBoxLayout;
    week1_button_ = new ImageButton(tr("轴1-3"), ":/image/Image/MainWidget/week1_3_mod.bmp", core_Widget_);
    week4_button_ = new ImageButton(tr("轴4-6"), ":/image/Image/MainWidget/week_4_6_mod.png", core_Widget_);
    linear_button_ = new ImageButton(tr("线性"), ":/image/Image/MainWidget/linear_mod.png", core_Widget_);
    relocation_button_ = new ImageButton(tr("重定位"), ":/image/Image/MainWidget/relocation_mod.png", core_Widget_);
    week1_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    week4_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    linear_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    relocation_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    image_layout_->addWidget(week1_button_);
    image_layout_->addWidget(week4_button_);
    image_layout_->addWidget(linear_button_);
    image_layout_->addWidget(relocation_button_);

    core_layout->addStretch();
    core_layout->addLayout(image_layout_);
    core_layout->addStretch();
    core_Widget_->setLayout(core_layout);

    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setSpacing(0);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_widget_->setLayout(button_layout);

    ConnectInit();
    SetSelect(week1_button_);
}

void ActionAttr::Init(int val)
{
    switch (val)
    {
    case 0:
    {
        SetSelect(week1_button_);

        break;
    }
    case 1:
    {
        SetSelect(week4_button_);

        break;
    }
    case 2:
    {
        SetSelect(linear_button_);

        break;
    }
    case 3:
    {
        SetSelect(relocation_button_);

        break;
    }
    default:
        break;
    }
}

void ActionAttr::Init(QString text)
{
    int val;

    if(text == week1_button_->GetText())
        val = 0;
    else if(text == week4_button_->GetText())
        val = 1;
    else if(text == linear_button_->GetText())
        val = 2;
    else
        val = 3;

    Init(val);
}

QHBoxLayout *ActionAttr::GetImageLayout()
{
    return image_layout_;
}

QString AttributeBase::GetSelectText()
{
    return opt_label_->text();
}

void ActionAttr::SetSelect(ImageButton *button)
{
    if(button == week1_button_)
    {
        week1_button_->SetIsSelect(true);

        week4_button_->SetIsSelect(false);
        linear_button_->SetIsSelect(false);
        relocation_button_->SetIsSelect(false);

        opt_label_->setText(week1_button_->GetText());
    }
    else if(button == week4_button_)
    {
        week4_button_->SetIsSelect(true);

        week1_button_->SetIsSelect(false);
        linear_button_->SetIsSelect(false);
        relocation_button_->SetIsSelect(false);

        opt_label_->setText(week4_button_->GetText());
    }
    else if(button == linear_button_)
    {
        linear_button_->SetIsSelect(true);

        week1_button_->SetIsSelect(false);
        week4_button_->SetIsSelect(false);
        relocation_button_->SetIsSelect(false);

        opt_label_->setText(linear_button_->GetText());
    }
    else
    {
        relocation_button_->SetIsSelect(true);

        week1_button_->SetIsSelect(false);
        week4_button_->SetIsSelect(false);
        linear_button_->SetIsSelect(false);

        opt_label_->setText(relocation_button_->GetText());
    }
}

void ActionAttr::ConnectInit()
{
    connect(week1_button_, &ImageButton::clicked,
            [=]()
    {
        SetSelect(week1_button_);
    });

    connect(week4_button_, &ImageButton::clicked,
            [=]()
    {
        SetSelect(week4_button_);
    });

    connect(linear_button_, &ImageButton::clicked,
            [=]()
    {
        SetSelect(linear_button_);
    });

    connect(relocation_button_, &ImageButton::clicked,
            [=]()
    {
        SetSelect(relocation_button_);
    });
}

UcsAttr::UcsAttr(QWidget *parent) : ActionAttr(parent)
{
    tips_label_->setText(tr("选择坐标系"));

    week1_button_->SetButton(tr("大地坐标系"), ":/image/Image/MainWidget/earth_coordinate.png");
    week4_button_->SetButton(tr("基坐标"), ":/image/Image/MainWidget/base_coordinate.png");
    linear_button_->SetButton(tr("工具"), ":/image/Image/MainWidget/tool_coordinate.png");
    relocation_button_->SetButton(tr("工件坐标"), ":/image/Image/MainWidget/work_coordinate.png");

    image_layout_->addStretch();

    SetSelect(week1_button_);
}

LeverLockAttr::LeverLockAttr(QWidget *parent) : ActionAttr(parent)
{
    tips_label_->setText(tr("选择需要锁定的操纵杆方向。"));

    week1_button_->SetButton(tr("无"), ":/image/Image/MainWidget/null_lock.png");
    week4_button_->SetButton(tr("水平方向"), ":/image/Image/MainWidget/level_lock.png");
    linear_button_->SetButton(tr("垂直方向"), ":/image/Image/MainWidget/vertical_lock.png");
    relocation_button_->SetButton(tr("旋转"), ":/image/Image/MainWidget/rotate_lock.png");

    image_layout_->addStretch();

    SetSelect(week1_button_);
}

void LeverLockAttr::LeverInit(QString text)
{
    if(text == week1_button_->GetText())
    {
        ActionAttr::Init(text);
        week1_button_->SetLeverSelect(true);
        return;
    }

    QStringList list = text.split("&&");

    opt_label_->setText(text);
    week1_button_->SetIsSelect(false);

    for(auto str : list)
    {
        if(str == week4_button_->GetText())
        {
            week4_button_->SetIsSelect(true);
            week4_button_->SetLeverSelect(true);
        }
        else if(str == linear_button_->GetText())
        {
            linear_button_->SetIsSelect(true);
            linear_button_->SetLeverSelect(true);
        }
        else if(str == relocation_button_->GetText())
        {
            relocation_button_->SetIsSelect(true);
            relocation_button_->SetLeverSelect(true);
        }
    }
}

void LeverLockAttr::SetSelect(ImageButton *button)
{
    if(button == week1_button_)
    {
        week1_button_->SetIsSelect(true);
        week1_button_->SetLeverSelect(true);

        week4_button_->SetIsSelect(false);
        week4_button_->SetLeverSelect(false);
        linear_button_->SetIsSelect(false);
        linear_button_->SetLeverSelect(false);
        relocation_button_->SetIsSelect(false);
        relocation_button_->SetLeverSelect(false);

        opt_label_->setText(week1_button_->GetText());

        return;
    }

    if(button == week4_button_)
    {
        if(week4_button_->GetLeverSelect())
        {
            week4_button_->SetIsSelect(false);
            week4_button_->SetLeverSelect(false);
        }
        else
        {
            week4_button_->SetIsSelect(true);
            week4_button_->SetLeverSelect(true);

            if(week1_button_->GetIsSelect())
            {
                week1_button_->SetIsSelect(false);
                week1_button_->SetLeverSelect(false);
            }
        }
    }

    if(button == linear_button_)
    {
        if(linear_button_->GetLeverSelect())
        {
            linear_button_->SetIsSelect(false);
            linear_button_->SetLeverSelect(false);
        }
        else
        {
            linear_button_->SetIsSelect(true);
            linear_button_->SetLeverSelect(true);

            if(week1_button_->GetIsSelect())
            {
                week1_button_->SetIsSelect(false);
                week1_button_->SetLeverSelect(false);
            }
        }
    }

    if(button == relocation_button_)
    {
        if(relocation_button_->GetLeverSelect())
        {
            relocation_button_->SetIsSelect(false);
            relocation_button_->SetLeverSelect(false);
        }
        else
        {
            relocation_button_->SetIsSelect(true);
            relocation_button_->SetLeverSelect(true);

            if(week1_button_->GetIsSelect())
            {
                week1_button_->SetIsSelect(false);
                week1_button_->SetLeverSelect(false);
            }
        }
    }

    QString str;

    if(week4_button_->GetIsSelect())
    {
        if(str.isEmpty())
            str.append(QString("%1").arg(week4_button_->GetText()));
        else
            str.append(QString("&&%1").arg(week4_button_->GetText()));
    }

    if(linear_button_->GetIsSelect())
    {
        if(str.isEmpty())
            str.append(QString("%1").arg(linear_button_->GetText()));
        else
            str.append(QString("&&%1").arg(linear_button_->GetText()));
    }

    if(relocation_button_->GetIsSelect())
    {
        if(str.isEmpty())
            str.append(QString("%1").arg(relocation_button_->GetText()));
        else
            str.append(QString("&&%1").arg(relocation_button_->GetText()));
    }

    if(str.isEmpty())
    {
        week1_button_->SetIsSelect(true);

        opt_label_->setText(week1_button_->GetText());
    }
    else
    {
        opt_label_->setText(str);
    }
}

IncrementAttr::IncrementAttr(QWidget *parent) : ActionAttr(parent)
{
    opt_label_->setText(tr("选择增量模式。"));

    user_button_ = new ImageButton(this);
    user_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    image_layout_->addWidget(user_button_);

    week1_button_->SetButton(tr("无"), ":/image/Image/MainWidget/increment_nullptr.png");
    week4_button_->SetButton(tr("小"), ":/image/Image/MainWidget/increment_small.png");
    linear_button_->SetButton(tr("中"), ":/image/Image/MainWidget/increment_in.png");
    relocation_button_->SetButton(tr("大"), ":/image/Image/MainWidget/increment_large.png");
    user_button_->SetButton(tr("用户"), ":/image/Image/MainWidget/increment_user.png");

    SetSelect(week1_button_);

    connect(user_button_, &ImageButton::clicked,
            [=]()
    {
        SetSelect(user_button_);
    });
}

void IncrementAttr::Init(QString text)
{
    if(text == user_button_->GetText())
    {
        this->SetSelect(user_button_);
    }
    else
    {
        ActionAttr::Init(text);
    }
}

void IncrementAttr::SetSelect(ImageButton *button)
{
    if(button == user_button_)
    {
        user_button_->SetIsSelect(true);

        week1_button_->SetIsSelect(false);
        week4_button_->SetIsSelect(false);
        linear_button_->SetIsSelect(false);
        relocation_button_->SetIsSelect(false);

        opt_label_->setText(user_button_->GetText());
    }
    else
    {
        user_button_->SetIsSelect(false);

        ActionAttr::SetSelect(button);
    }
}

VarEditInterface::VarEditInterface(QWidget *parent) : ThreeSectionWindow(parent)
{
    QGridLayout *upper_layout = new QGridLayout(upper_widget_);

    tips_label_ = new QLabel(tr("当前选择:"), upper_widget_);
    select_label_ = new QLabel(upper_widget_);
    tips1_label_ = new QLabel(tr("从列表中选择一个项目"), upper_widget_);
    upper_layout->addWidget(tips_label_, 0, 0);
    upper_layout->addWidget(select_label_, 0, 1);
    upper_layout->addWidget(tips1_label_, 1, 0);
    upper_layout->setContentsMargins(0, 0, 0, 0);
    upper_widget_->setLayout(upper_layout);

    QVBoxLayout *in_layout = new QVBoxLayout(in_widget_);

    table_box_ = new TableBox(1, 3, this);

    in_layout->addWidget(table_box_);
    in_layout->setContentsMargins(0, 0, 0, 0);
    in_widget_->setLayout(in_layout);

    QHBoxLayout *button_layout = new QHBoxLayout(lower_widget_);
    create_button_ = new QPushButton(tr("新建"), lower_widget_);
    edit_button_ = new ComboBoxObject(tr("编辑"), lower_widget_);
    define_button_ = new QPushButton(tr("确定"), lower_widget_);
    cancel_button_ = new QPushButton(tr("取消"), lower_widget_);
    create_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_layout->addWidget(create_button_);
    button_layout->addWidget(edit_button_);
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    lower_widget_->setLayout(button_layout);

    create_button_->setFlat(true);
    edit_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);

    create_widget_ = new CreateNewVar(this);
    create_widget_->hide();

    revise_ucs_ = new ReviseUcs(this);
    revise_ucs_->hide();

    mod_var_window_ = new ModVarWindow(this);
    mod_var_window_->HideTitle();
    mod_var_window_->hide();

    change_val_ = new QAction(tr("更改值"), this);
    change_state_ = new QAction(tr("更改声明"), this);
    copy_ = new QAction(tr("复制"), this);
    del_ = new QAction(tr("删除"), this);
    defi_ = new QAction(tr("定义"), this);

    edit_button_->AppAction(change_val_);
    if(/* DISABLES CODE */ (0))
    {
        edit_button_->AppAction(change_state_);
        edit_button_->AppAction(copy_);
    }
    edit_button_->AppAction(del_);
    edit_button_->AppSeparator();
    edit_button_->AppAction(defi_);

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    lower_widget_->setAutoFillBackground(true);
    lower_widget_->setPalette(p);
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    create_widget_->setAutoFillBackground(true);
    create_widget_->setPalette(p);
    revise_ucs_->setAutoFillBackground(true);
    revise_ucs_->setPalette(p);

    ConnectInit();
}

CreateNewVar *VarEditInterface::GetCreate()
{
    return create_widget_;
}

void VarEditInterface::Init(QString data_type, QString select_str, QString var_range, bool genre)
{
    select_label_->setText(select_str);
    data_type_ = data_type;
    var_range_ = var_range;
    genre_ = genre;

    if(select_label_->text().isEmpty())
        define_button_->setEnabled(false);
    else if(!define_button_->isEnabled())
        define_button_->setEnabled(true);

    table_box_->clearContents();
    table_box_->SetRomn(1, 3);

    if(data_type == "loaddata")
    {
        QStringList head_name;
        head_name << tr("有效载荷名称") << tr("模块") << tr("范围");
        table_box_->setHorizontalHeaderLabels(head_name);
    }
    else if(data_type == "wobjdata")
    {
        QStringList head_name;
        head_name << tr("工件名称") << tr("模块") << tr("范围");
        table_box_->setHorizontalHeaderLabels(head_name);
    }
    else if(data_type == "tooldata")
    {
        QStringList head_name;
        head_name << tr("工具名称") << tr("模块") << tr("范围");
        table_box_->setHorizontalHeaderLabels(head_name);
    }

    auto var_list = VarInfo::instance()->GetTypeVar(data_type);

    int x = 0, i = 0;
    int num = 0;
    for(auto &var : var_list)
    {
        if(var.second.genre != genre)
            continue;
        if(!var.second.dim.isEmpty())
                continue;
        if(var_range.isEmpty()) //为空默认显示全局和任务
        {
            if(var.second.action_scope == "all" || var.second.action_scope == "task")
            {
                QString range;
                if(var.second.action_scope == "all")
                    range = tr("全局");
                else
                    range = tr("任务");

                if(var.first == select_str)
                    x = i;

                auto mod_name = "RAPID/T_ROB1/" + var.second.mod;
                table_box_->AddText(num, 0, var.first);
                table_box_->AddText(num, 1, mod_name);
                table_box_->AddText(num++, 2, range);
                i++;
                continue;
            }
        }
        else
        {
            if(var.second.action_scope != var_range)
            {
                continue;
            }
            else
            {
                QString range;
                if(var.second.action_scope == "all")
                    range = tr("全局");
                else if(var.second.action_scope == "task")
                    range = tr("任务");
                else if(var.second.action_scope == "local")
                    range = tr("本地");

                if(var.first == select_str)
                    x = i;

                auto mod_name = "RAPID/T_ROB1/" + var.second.mod;
                table_box_->AddText(num, 0, var.first);
                table_box_->AddText(num, 1, mod_name);
                table_box_->AddText(num++, 2, range);
                i++;
                continue;
            }
        }
    }

    table_box_->setCurrentCell(x, 0);

}

void VarEditInterface::ConnectInit()
{
    connect(cancel_button_, &QPushButton::clicked,
            [=]()
    {
        emit CancelButton();
    });

    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        if(select_label_->text().isEmpty())
        {
            QMessageBox::warning(this, tr("提示"), tr("请选择"));
            return;
        }
        auto &info = VarInfo::instance()->GetUcsVarInfo(select_label_->text());
        auto point = Convert::DataTypeToVarFormData(data_type_, info.var.toString());

        if(data_type_ == "tooldata") //工具
        {
            if(point->subnode.at(2)->subnode.at(0)->code == "-1")
            {
                QString str = QString("%1's \"tload.mass\"" + tr("含有无效数据！纠正数据后重试")).arg(select_label_->text());
                QMessageBox::warning(this, tr("错误"), str);
                return;
            }
        }

        emit DefineButton(data_type_, select_label_->text());
    });

    connect(table_box_, &TableBox::itemClicked,
            [=](QTableWidgetItem *item)
    {
        if(item == nullptr)
            return;
        item = table_box_->item(table_box_->row(item), 0);
        auto name = item->text();

        select_label_->setText(name);
        if(!define_button_->isEnabled())
            define_button_->setEnabled(true);
    });

    connect(create_widget_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        create_widget_->hide();
    });

    connect(create_widget_, &CreateNewVar::CreateVar,
            [=]()
    {
        create_widget_->hide();
        this->Init(data_type_, select_label_->text(), var_range_, genre_);
        table_box_->clearSelection();
    });

    connect(edit_button_, &ComboBoxObject::LowerSig,
            [=]()
    {
        QString var = select_label_->text();
        if(var == "tool0" || var == "load0" || var == "wobj0" || !genre_ || select_label_->text().isEmpty() || table_box_->currentRow() == -1)
        {
            change_val_->setEnabled(false);
            change_state_->setEnabled(false);
            copy_->setEnabled(false);
            del_->setEnabled(false);
            defi_->setEnabled(false);
        }
        else
        {
            if(!change_val_->isEnabled())
            {
                change_val_->setEnabled(true);
                change_state_->setEnabled(true);
                copy_->setEnabled(true);
                del_->setEnabled(true);
                defi_->setEnabled(true);
            }
        }
    });

    connect(revise_ucs_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        revise_ucs_->hide();
    });

    connect(revise_ucs_, &ReviseUcs::ModQuit,
            [=]()
    {
        revise_ucs_->hide();
    });

    connect(mod_var_window_, &ModVarWindow::Defineed,
            [=]()
    {
        QList<double> xyz;
        Euler er;
        auto qn = er.GetQuaternion();

        auto frame = RobotClient::initance()->getRobotFrame();

        if(data_type_ == "wobjdata")
        {
            xyz = Convert::XYZCodeToDoubles(var_form_pointer_->subnode.at(3)->subnode.at(0)->code);
            qn.SetData(var_form_pointer_->subnode.at(3)->subnode.at(1)->code);
            er = Convert::QuaternionToEuler(qn);

            Terminal ter(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

            auto tm = frame.getWorkFrame(select_label_->text().toStdString());

            if(ter != tm)
            {
                frame.modifyWorkFrame(select_label_->text().toStdString(), ter);
                RobotClient::initance()->setRobotFrame(frame);
            }
        }
        else {
            xyz = Convert::XYZCodeToDoubles(var_form_pointer_->subnode.at(1)->subnode.at(0)->code);
            qn.SetData(var_form_pointer_->subnode.at(1)->subnode.at(1)->code);
            er = Convert::QuaternionToEuler(qn);

            Terminal ter(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

            auto tm = frame.getToolFrame(select_label_->text().toStdString());

            if(ter != tm)
            {
                frame.modifyWorkFrame(select_label_->text().toStdString(), ter);
                RobotClient::initance()->setRobotFrame(frame);
            }
        }

        mod_var_window_->hide();
        table_box_->clearSelection();
        var_form_pointer_.clear();
    });

    connect(mod_var_window_, &ModVarWindow::Cancel,
            [=]()
    {
        mod_var_window_->hide();
        var_form_pointer_.clear();
    });

    connect(create_button_, &QPushButton::clicked, this, &VarEditInterface::ShowCreateWidget);

    connect(change_state_, &QAction::triggered, this, &VarEditInterface::ShowCreateWidget);

    connect(del_, &QAction::triggered, this, &VarEditInterface::RemoveVar);

    connect(defi_, &QAction::triggered, this, &VarEditInterface::ShowReviseWidget);

    connect(change_val_, &QAction::triggered, this, &VarEditInterface::ShowModVarWindow);
}

void VarEditInterface::SetScale(double scale_w, double scale_h)
{
    if(!create_widget_->isHidden())
    {
        create_widget_->resize(this->size());
        create_widget_->move(0, 0);
    }
    if(!revise_ucs_->isHidden())
    {
        revise_ucs_->resize(this->size());
        revise_ucs_->move(0, 0);
    }

    table_box_->SetScale(scale_w, scale_h);
    create_widget_->SetScale(scale_w, scale_h);
    revise_ucs_->SetScale(scale_w, scale_h);
}

void VarEditInterface::ShowCreateWidget()
{
    QPushButton *butt = dynamic_cast<QPushButton*>(sender());
    if(butt == nullptr)
    {
        QAction *chate = dynamic_cast<QAction*>(sender());
        if(chate == nullptr)
            return;
    }

    QStringList list = EditData::instance()->GetModName(false);
    if(list.isEmpty())
        return;

    create_widget_->Init(data_type_, list.at(0));

    if(butt == create_button_) //新建
    {
        create_widget_->dimen_box_->show();
        create_widget_->dimen_edit_->show();
        create_widget_->dimen_label_->show();
        create_widget_->dimen_key_button_->show();
    }
    else //更改声明
    {
        create_widget_->name_edit_->setText(select_label_->text());
        create_widget_->dimen_box_->hide();
        create_widget_->dimen_edit_->hide();
        create_widget_->dimen_label_->hide();
        create_widget_->dimen_key_button_->hide();
    }

    create_widget_->resize(this->size());
    create_widget_->move(0, 0);
    create_widget_->show();
}

void VarEditInterface::RemoveVar()
{
    if(!genre_)
    {
        QMessageBox::warning(this, tr("提示"), tr("内置变量无法被删除"));
        return;
    }
    else if(QMessageBox::question(this, tr("询问"), tr("确定要删除该变量吗？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;

    QString var_name = select_label_->text();

    if(!VarInfo::instance()->RemoveVar(var_name, data_type_))
    {
        QMessageBox::about(this, tr("提示"), tr("删除失败"));
        return;
    }
    else
    {
        int curr = table_box_->currentRow();
        if(curr < 0)
            return;
        RobotFrame frame =  RobotClient::initance()->getRobotFrame();
        if(data_type_ == "wobjdata")
        {
            frame.deleteWorkFrame(var_name.toStdString());
        }
        else if(data_type_ == "tooldata")
        {
            frame.deleteToolFrame(var_name.toStdString());
        }
        RobotClient::initance()->setRobotFrame(frame);
        table_box_->removeRow(curr);
        select_label_->clear();
        table_box_->clearSelection();
    }
}

void VarEditInterface::ShowReviseWidget()
{
    revise_ucs_->resize(this->size());
    revise_ucs_->move(0, 0);
    if(data_type_ == "wobjdata")
    {
        StackedWidObject *stack = dynamic_cast<StackedWidObject*>(this->parentWidget());
        if(stack == nullptr)
        {
            QMessageBox::warning(this, tr("提示"), tr("出现错误"));
            return;
        }
        ManualControl *mau = dynamic_cast<ManualControl*>(stack->parentWidget());
        if(mau == nullptr)
        {
            QMessageBox::warning(this, tr("提示"), tr("出现错误"));
            return;
        }
        QString sel = mau->manu_widget_->attribute_box_->toolv_button_->text();

        revise_ucs_->Init(select_label_->text(), sel);
    }
    else if(data_type_ == "tooldata")
    {
        revise_ucs_->Init(select_label_->text());
    }
    revise_ucs_->show();
}

void VarEditInterface::ShowModVarWindow()
{
    mod_var_window_->resize(this->size());
    mod_var_window_->move(0, 0);

    auto var_info = VarInfo::instance()->GetUcsVarInfo(select_label_->text());

    var_form_pointer_ = Convert::DataTypeToVarFormData(var_info.data_type, var_info.var.toString());

    mod_var_window_->Init(var_form_pointer_, select_label_->text(), var_info.mod, var_info.action_scope);

    mod_var_window_->show();
}

FormatPosition::FormatPosition(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *w_layout = new QVBoxLayout(widget);

    pos_label_ = new QLabel(tr("位置显示方式:"), this);
    pos_box_ = new QComboBox(this);
    direc_label_ = new QLabel(tr("方向格式:"), this);
    direc_box_ = new QComboBox(this);
    angle_label_ = new QLabel(tr("角度格式:"), this);
    angle_box_ = new QComboBox(this);
    unit_label_ = new QLabel(tr("角度单位:"), this);
    unit_box_ = new QComboBox(this);

    w_layout->addWidget(pos_label_);
    w_layout->addWidget(pos_box_);
    w_layout->addWidget(direc_label_);
    w_layout->addWidget(direc_box_);
    w_layout->addWidget(angle_label_);
    w_layout->addWidget(angle_box_);
    w_layout->addWidget(unit_label_);
    w_layout->addWidget(unit_box_);

    button_widget_ = new QWidget(this);
    QHBoxLayout *butt_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    butt_layout->addStretch();
    butt_layout->addStretch();
    butt_layout->addStretch();
    butt_layout->addWidget(define_button_);
    butt_layout->addWidget(cancel_button_);
    button_widget_->setLayout(butt_layout);

    layout->addWidget(widget);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(widget, 6);
    layout->setStretchFactor(button_widget_, 1);

    this->setLayout(layout);

    pos_box_->addItem(tr("大地坐标"));
    pos_box_->addItem(tr("基坐标"));
    pos_box_->addItem(tr("工件坐标"));

    direc_box_->addItem(tr("四个一组"));
    direc_box_->addItem(tr("欧拉角"));

    angle_box_->addItem(tr("角度"));

    unit_box_->addItem(tr("度数"));
    unit_box_->addItem(tr("弧度"));

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        int pos = pos_box_->currentIndex();
        int direc = direc_box_->currentIndex();
        int angle = angle_box_->currentIndex();
        int unit = unit_box_->currentIndex();

        emit DefineClicked(pos, direc, angle, unit);
    });
    connect(cancel_button_, &QPushButton::clicked,
            [=]()
    {
        emit CancelClicked();
    });
}

void FormatPosition::Init(int pos, int direc, int angle, int unit)
{
    pos_box_->setCurrentIndex(pos);
    direc_box_->setCurrentIndex(direc);
    angle_box_->setCurrentIndex(angle);
    unit_box_->setCurrentIndex(unit);
}

ReviseUcs::ReviseUcs(QWidget *parent) : ThreeSectionWindow(parent)
{
    QVBoxLayout *upper_layout = new QVBoxLayout(upper_widget_);

    tips_label_ = new QLabel(upper_widget_);

    QHBoxLayout *tips_layout = new QHBoxLayout;
    tips1_label_ = new QLabel(upper_widget_);
    tips2_label_ = new QLabel(upper_widget_);
    tips3_label_ = new QLabel(upper_widget_);
    tips_layout->addWidget(tips1_label_);
    tips_layout->addWidget(tips2_label_);
    tips_layout->addWidget(tips3_label_);

    tips4_label_ = new QLabel(tr("选择一种方法，修改位置后点击确定"), upper_widget_);

    QHBoxLayout *box_layout = new QHBoxLayout;
    way_label_ = new QLabel(tr("方法："), upper_widget_);
    way_box_ =new QComboBox(upper_widget_);
    num_label_ = new QLabel(tr("点数："), upper_widget_);
    num_box_ = new QComboBox(upper_widget_);
    box_layout->addWidget(way_label_);
    box_layout->addWidget(way_box_);
    box_layout->addWidget(num_label_);
    box_layout->addWidget(num_box_);

    upper_layout->addWidget(tips_label_);
    upper_layout->addLayout(tips_layout);
    upper_layout->addWidget(tips4_label_);
    upper_layout->addLayout(box_layout);

    upper_widget_->setLayout(upper_layout);

    QVBoxLayout *main_layout = new QVBoxLayout(in_widget_);

    table_box_ = new TableBox(1, 2, this);

    main_layout->addWidget(table_box_);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    in_widget_->setLayout(main_layout);

    QHBoxLayout *button_layout = new QHBoxLayout(lower_widget_);
    position_button_ = new ComboBoxObject(tr("位置"), lower_widget_);
    re_pos_button_ = new QPushButton(tr("修改位置"), lower_widget_);
    define_button_ = new QPushButton(tr("确定"), lower_widget_);
    cancel_button_ = new QPushButton(tr("取消"), lower_widget_);
    position_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    re_pos_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_layout->addWidget(position_button_);
    button_layout->addStretch();
    button_layout->addWidget(re_pos_button_);
    button_layout->addWidget(re_pos_button_);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    lower_widget_->setLayout(button_layout);

    way_label_->setAlignment(Qt::AlignCenter);
    num_label_->setAlignment(Qt::AlignCenter);

    position_button_->setFlat(true);
    re_pos_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);

    way_box_->addItem("Tcp");
    way_box_->addItem("Tcp + Z");
    way_box_->addItem("Tcp + Z, X");

    QStringList list;
    list << "3" << "4" << "5" << "6" << "7" << "8" << "9";
    num_box_->addItems(list);

    reset_ = new QAction(tr("全部重置"), this);
    position_button_->AppAction(reset_);

    tool_error_window_ = new ToolErrorWindow(this);
    tool_error_window_->HideTitle();
    tool_error_window_->hide();

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(1, 2);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("点") << tr("状态"));

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    lower_widget_->setAutoFillBackground(true);
    lower_widget_->setPalette(p);

    ConnectInit();
}

ReviseUcs::~ReviseUcs()
{
}

bool ReviseUcs::Init(QString var_name, QString tool_select)
{
    auto var_info = VarInfo::instance()->GetVarInfo(var_name);
    if(var_info.size() != 1)
        return false;

    if(!tool_error_window_->isHidden())
        tool_error_window_->hide();

    tips2_label_->setText(var_name);

    QString type = var_info.at(0).data_type;
    if(type == "wobjdata") //工件
    {
        woto = true;

        tips_label_->setText(tr("工件坐标定义"));
        tips1_label_->setText(tr("工件坐标："));
        tips3_label_->show();
        tips3_label_->setText(tr("活动工具：") + tool_select);

        way_label_->setText(tr("用户方法："));
        num_label_->setText(tr("目标方法"));
        QStringList list;
        list << "No" << "3";
        way_box_->clear();
        num_box_->clear();
        way_box_->addItems(list);
        num_box_->addItems(list);

        way_box_->setCurrentIndex(0);
        num_box_->setCurrentIndex(0);
    }
    else //工具
    {
        woto = false;

        tips_label_->setText(tr("工具坐标定义"));
        tips1_label_->setText(tr("工具坐标："));
        tips3_label_->hide();

        way_label_->setText(tr("方法："));
        num_label_->setText(tr("方法"));
        QStringList list;
        list << "3" << "4" << "5" << "6" << "7" << "8" << "9";
        way_box_->clear();
        num_box_->clear();
        way_box_->addItem("Tcp");
        way_box_->addItem("Tcp + Z");
        way_box_->addItem("Tcp + Z, X");
        num_box_->addItems(list);

        way_box_->setCurrentIndex(0);
        num_box_->setCurrentIndex(1);
    }

    this->Reset();
    return true;
}

void ReviseUcs::ConnectInit()
{
    connect(tool_error_window_, &ToolErrorWindow::CQuit,
            [=]()
    {
        tool_error_window_->hide();
    });

    connect(tool_error_window_, &ToolErrorWindow::Quit, this, &ReviseUcs::ToolErrorQuit);

    connect(way_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ReviseUcs::Reset);

    connect(num_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ReviseUcs::Reset);

    connect(re_pos_button_, &QPushButton::clicked, this, &ReviseUcs::RePosClicked);

    connect(define_button_, &QPushButton::clicked, this, &ReviseUcs::DefinClicked);

    connect(reset_, &QAction::triggered, this, &ReviseUcs::Reset);

    connect(table_box_, &TableBox::itemClicked, this, &ReviseUcs::ItemClicked);
}

void ReviseUcs::Reset()
{
    table_box_->clearContents();
    table_box_->SetRomn(0, 2);
    joints_pos_.clear();

    int way = way_box_->currentIndex();
    int num = num_box_->currentIndex();

    if(woto)
    {
        int row = table_box_->GetRow();
        if(way == 1)
        {
            table_box_->AddText(row, 0, tr("用户点  X 1"));
            table_box_->AddText(row++, 1, "-");

            table_box_->AddText(row, 0, tr("用户点  X 2"));
            table_box_->AddText(row++, 1, "-");

            table_box_->AddText(row, 0, tr("用户点  Y 1"));
            table_box_->AddText(row++, 1, "-");
        }
        if(num == 1)
        {
            table_box_->AddText(row, 0, tr("目标点 X 1"));
            table_box_->AddText(row++, 1, "-");

            table_box_->AddText(row, 0, tr("目标点 X 2"));
            table_box_->AddText(row++, 1, "-");

            table_box_->AddText(row, 0, tr("目标点 Y 1"));
            table_box_->AddText(row++, 1, "-");
        }

        if(way == 1 && num == 1)
        {
            tab_curr_ = -1;
        }
        else
        {
            tab_curr_ = 0;
            table_box_->setCurrentCell(tab_curr_, 1);
        }
    }
    else
    {
        num = num_box_->currentText().toInt();

        for(int i = 0; i < num; i++)
        {
            table_box_->AddText(i, 0, tr("点") + QString::number(i));
            table_box_->AddText(i, 1, "-");
            joints_pos_.append(Joints());
        }

        if(way == 1)
        {
            table_box_->AddText(num, 0, tr("延伸器点 Z"));
            table_box_->AddText(num, 1, "-");
        }
        else if(way == 2)
        {
            table_box_->AddText(num, 0, tr("延伸器点 X"));
            table_box_->AddText(num++, 1, "-");

            table_box_->AddText(num, 0, tr("延伸器点 Z"));
            table_box_->AddText(num, 1, "-");
        }

        tab_curr_ = 0;
        table_box_->setCurrentCell(tab_curr_, 1);
        joint_origin_ = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
    }
}

void ReviseUcs::SetScale(double scale_w, double scale_h)
{
    if(!tool_error_window_->isHidden())
    {
        tool_error_window_->resize(this->size());
        tool_error_window_->move(0, 0);
    }
    tool_error_window_->SetScale(scale_w, scale_h);
    table_box_->SetScale(scale_w, scale_h);
}

void ReviseUcs::RePosClicked()
{
    int way = way_box_->currentIndex();

    if(woto)
    {
        if(tab_curr_ == -1)
            return;
        if(tab_curr_ == 0)
        {
            point_origin_ = RobotClient::initance()->getRobotMotion().getCurrentTerminal();
        }
        else if(tab_curr_ == 1)
        {
            pointx_ = RobotClient::initance()->getRobotMotion().getCurrentTerminal();
        }
        else if(tab_curr_ == 2)
        {
            pointy_ = RobotClient::initance()->getRobotMotion().getCurrentTerminal();
        }
        auto *item = table_box_->item(tab_curr_, 1);
        item->setText(tr("已修改"));
    }
    else
    {
        if(way == 0)
        {
            joints_pos_[tab_curr_] = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
            auto *item = table_box_->item(tab_curr_, 1);
            item->setText(tr("已修改"));
        }
        else if(way == 1)
        {
            if(tab_curr_ == table_box_->GetRow() - 1)
            {
                jointz_ = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
            }
            else
            {
                joints_pos_[tab_curr_] = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
            }
            auto *item = table_box_->item(tab_curr_, 1);
            item->setText(tr("已修改"));
        }
        else if(way == 2)
        {
            if(tab_curr_ == table_box_->GetRow() - 2)
            {
                jointx_ = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
            }
            else if(tab_curr_ == table_box_->GetRow() - 1)
            {
                jointz_ = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
            }
            else
            {
                joints_pos_[tab_curr_] = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
            }
            auto *item = table_box_->item(tab_curr_, 1);
            item->setText(tr("已修改"));
        }
    }

    if(tab_curr_ + 1 < table_box_->GetRow())
    {
        table_box_->setCurrentCell(++tab_curr_, 1);
    }
}

void ReviseUcs::DefinClicked()
{
    for(int i = 0; i < table_box_->GetRow(); i++)
    {
        auto *item = table_box_->item(i, 1);
        if(item->text() == "-")
        {
            QMessageBox::warning(this, tr("提示"), tr("您还有未标定完的点"));
            return;
        }
    }

    QSharedPointer<Terminal> tm(new Terminal);

    if(woto) //工件坐标
    {
        if(point_origin_ == pointx_ || point_origin_ == pointy_ || pointx_ == pointy_)
        {
            QMessageBox::warning(this, tr("警告"), tr("无法通过当前位置校准。"));
            return;
        }

        RobotClient::initance()->calibrateUSRF(point_origin_, pointx_, pointy_, *tm);

        RobotFrame frame = RobotClient::initance()->getRobotFrame();
        int x = frame.modifyWorkFrame(tips2_label_->text().toStdString(), *tm);
        if(x != 0)
        {
            QMessageBox::warning(this, tr("提示"), tr("修改坐标失败"));
            return;
        }
        RobotClient::initance()->setRobotFrame(frame);
    }
    else //工具坐标
    {
        int way = way_box_->currentIndex();
        double error;

        for(int i = 0; i < joints_pos_.size(); i++)
        {
            auto &pos = joints_pos_[i];

            for(int b = i + 1; b < joints_pos_.size(); b++)
            {
                auto &posb = joints_pos_[b];
                if(pos == posb)
                {
                    QMessageBox::warning(this, tr("警告"), tr("无法通过当前位置校准。"));
                    return;
                }
            }
            if(way != 0)
            {
                if(pos == joint_origin_ || pos == jointx_ || pos == jointz_)
                {
                    QMessageBox::warning(this, tr("警告"), tr("无法通过当前位置校准。"));
                    return;
                }
            }
        }

        if(way == 0)
            error = RobotClient::initance()->calibrateTCP(joints_pos_.toVector().toStdVector(), *tm);
        else if(way == 1)
            error = RobotClient::initance()->calibrateTCFZ(joints_pos_.toVector().toStdVector(), joint_origin_, jointz_, *tm);
        else
            error = RobotClient::initance()->calibrateTCFX(joints_pos_.toVector().toStdVector(), joint_origin_, jointx_, jointz_, *tm);

        tool_error_window_->resize(this->size());
        tool_error_window_->move(0, 0);
        tool_error_window_->Init(tips2_label_->text(), way, error, tm);
        tool_error_window_->show();

        return;
    }

    double x, y, z;
    Euler er;
    x = tm->getValue(TERMINAL_X);
    y = tm->getValue(TERMINAL_Y);
    z = tm->getValue(TERMINAL_Z);
    er.roll = tm->getValue(TERMINAL_A);
    er.pitch = tm->getValue(TERMINAL_B);
    er.yaw = tm->getValue(TERMINAL_C);

    QString varcode = Convert::PositDoubleToPositCode(x, y, z, er);

    auto &info = VarInfo::instance()->GetUcsVarInfo(tips2_label_->text());

    auto var_ptr = Convert::DataTypeToVarFormData(info.data_type, info.var.toString());
    var_ptr->subnode.at(1)->code = varcode;
    var_ptr->subnode.at(1)->LastUpdate();
    var_ptr->Update();

    info.SetVar(var_ptr);

    emit ModQuit();
}

void ReviseUcs::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    tab_curr_ = table_box_->row(item);
}

void ReviseUcs::ToolErrorQuit(QSharedPointer<Terminal> tm)
{
    double x, y, z;
    Euler er;
    x = tm->getValue(TERMINAL_X);
    y = tm->getValue(TERMINAL_Y);
    z = tm->getValue(TERMINAL_Z);
    er.roll = tm->getValue(TERMINAL_A);
    er.pitch = tm->getValue(TERMINAL_B);
    er.yaw = tm->getValue(TERMINAL_C);

    QString varcode = Convert::PositDoubleToPositCode(x, y, z, er);

    auto &info = VarInfo::instance()->GetUcsVarInfo(tips2_label_->text());

    auto var_ptr = Convert::DataTypeToVarFormData(info.data_type, info.var.toString());
    var_ptr->subnode.at(1)->code = varcode;
    var_ptr->subnode.at(1)->LastUpdate();
    var_ptr->Update();

    info.SetVar(var_ptr);

    emit ModQuit();
}

ToolErrorWindow::ToolErrorWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_widget_ = new QWidget(this);
    auto *title_layout = new QHBoxLayout(title_widget_);
    ico_label_ = new QLabel(title_widget_);
    title_label_ = new QLabel(tr("程序数据 - tooldata - 定义 - 工具坐标定义"), title_widget_);
    title_layout->addWidget(ico_label_);
    title_layout->addWidget(title_label_);
    title_layout->setContentsMargins(0, 0, 0, 0);
    title_layout->setSpacing(0);
    title_layout->setStretchFactor(ico_label_, 1);
    title_layout->setStretchFactor(title_label_, 15);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips_label_ = new QLabel(tr("计算结果"), head_widget_);
    tips1_label_ = new QLabel(tr("工具坐标:"), head_widget_);
    tool_name_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel(tr("点击确定确认结果，或点击取消重新定义源数据"), head_widget_);
    head_layout->addWidget(tips_label_, 0, 0);
    head_layout->addWidget(tips1_label_, 1, 0);
    head_layout->addWidget(tool_name_label_, 1, 1);
    head_layout->addWidget(tips2_label_, 2, 0);

    table_widget_ = new QFrame(this);
    auto *table_layout = new QVBoxLayout(table_widget_);

    auto *label_layout = new QHBoxLayout;
    tips3_label_ = new QLabel(table_widget_);
    tips4_label_ = new QLabel(table_widget_);
    label_layout->addWidget(tips3_label_);
    label_layout->addWidget(tips4_label_);
    label_layout->setContentsMargins(0, 0, 0, 0);
    label_layout->setSpacing(0);

    table_box_ = new TableBox(table_widget_);
    table_layout->addLayout(label_layout);
    table_layout->addWidget(table_box_);
    table_layout->setContentsMargins(0, 0, 0, 0);
    table_layout->setSpacing(0);
    table_layout->setStretchFactor(label_layout, 1);
    table_layout->setStretchFactor(table_box_, 6);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确认"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(title_widget_);
    layout->addWidget(head_widget_);
    layout->addWidget(table_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_widget_, 1);
    layout->setStretchFactor(head_widget_, 5);
    layout->setStretchFactor(table_widget_, 12);
    layout->setStretchFactor(button_widget_, 2);

    ico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/program_data.png)");
    head_widget_->setFrameShape(QFrame::Box);
    tips3_label_->setFrameShape(QFrame::Box);
    tips4_label_->setFrameShape(QFrame::Box);

    table_box_->SetRomn(7, 2);

    table_box_->AddText(0, 0, tr("方法"));
    table_box_->AddText(1, 0, tr("最大误差"));
    table_box_->AddText(2, 0, tr("最小误差"));
    table_box_->AddText(3, 0, tr("平均误差"));
    table_box_->AddText(4, 0, "X");
    table_box_->AddText(5, 0, "Y");
    table_box_->AddText(6, 0, "Z");

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    tips3_label_->setAutoFillBackground(true);
    tips4_label_->setAutoFillBackground(true);
    button_widget_->setAutoFillBackground(true);
    tips3_label_->setPalette(p);
    tips4_label_->setPalette(p);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked, this, &ToolErrorWindow::Defint);
    connect(cancel_button_, &QPushButton::clicked, this, &ToolErrorWindow::Cancel);
}

void ToolErrorWindow::HideTitle()
{
    title_widget_->hide();
}

void ToolErrorWindow::Init(QString tool_name, int way, double error, QSharedPointer<Terminal> tm)
{
    tool_name_label_->setText(tool_name);
    terminal_point_ = tm;

    if(way == 0)
        table_box_->AddText(0, 1, "TCP");
    else if(way == 1)
        table_box_->AddText(0, 1, "Tcp + Z");
    else
        table_box_->AddText(0, 1, "Tcp + Z, X");

    auto str_error = QString("%1 %2").arg(error).arg(tr("毫米"));
    table_box_->AddText(1, 1, str_error);
    table_box_->AddText(2, 1, str_error);
    table_box_->AddText(3, 1, str_error);

    auto str_x = QString("%1 %2").arg(tm->getValue(TERMINAL_X)).arg(tr("毫米"));
    auto str_y = QString("%1 %2").arg(tm->getValue(TERMINAL_Y)).arg(tr("毫米"));
    auto str_z = QString("%1 %2").arg(tm->getValue(TERMINAL_Z)).arg(tr("毫米"));
    table_box_->AddText(4, 1, str_x);
    table_box_->AddText(5, 1, str_y);
    table_box_->AddText(6, 1, str_z);

    for(int i = 0; i < table_box_->GetRow(); i++)
    {
        auto *item = table_box_->item(i, 0);
        item->setFlags(Qt::NoItemFlags);
        auto *item2 = table_box_->item(i, 1);
        item2->setFlags(Qt::NoItemFlags);
    }
}

void ToolErrorWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
}

void ToolErrorWindow::Defint()
{
    auto frame = RobotClient::initance()->getRobotFrame();
    int x = frame.modifyToolFrame(tool_name_label_->text().toStdString(), *terminal_point_);

    if(x != 0)
    {
        QMessageBox::warning(this, tr("提示"), tr("修改坐标失败"));
        return;
    }
    RobotClient::initance()->setRobotFrame(frame);

    emit Quit(terminal_point_);
}

void ToolErrorWindow::Cancel()
{
    terminal_point_.reset();

    emit CQuit();
}

UnitWindow::UnitWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips1_label_ = new QLabel(tr("当前选择:"), head_widget_);
    current_label_ = new QLabel("ROB_1", head_widget_);
    tips2_label_ = new QLabel(tr("选择要更改其状态的机械单元。"), head_widget_);
    head_layout->addWidget(tips1_label_, 0, 0);
    head_layout->addWidget(current_label_, 0, 1);
    head_layout->addWidget(tips2_label_, 1, 0);

    table_box_ = new TableBox(1, 2, this);

    button_widget_ = new QWidget(this);
    auto *bt_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addWidget(define_button_);
    bt_layout->addWidget(cancel_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setStretchFactor(head_widget_, 3);
    layout->setStretchFactor(table_box_, 7);
    layout->setStretchFactor(button_widget_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(1, 2);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("机械单元") << tr("状态"));

    table_box_->AddText(0, 0, "ROB_1");
    table_box_->AddText(0, 1, tr("已启动"));
    table_box_->setCurrentCell(0, 0);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked, this, &UnitWindow::Define);
    connect(cancel_button_, &QPushButton::clicked, this, &UnitWindow::Cancel);
}

void UnitWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
}
