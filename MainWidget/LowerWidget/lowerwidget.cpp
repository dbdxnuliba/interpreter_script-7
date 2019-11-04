#include "lowerwidget.h"
#include <QHeaderView>
#include <QMessageBox>
#include "MainWidget/commoncontrols.h"
#include <QDebug>

LowerWidget::LowerWidget(QWidget *parent) : QWidget(parent)
{
    save_button = new SaveButton(this);
    quick_button_ = new QuickButton(this);
    scale_w_ = 1;
    scale_h_ = 1;

    //设置背景为白色
    QPalette pale;
    pale.setColor(QPalette::Window, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pale);

    Init();
}

void LowerWidget::Init()
{
    quick_button_->setStyleSheet("background-color: rgb(255, 255, 255)");
}

void LowerWidget::resizeEvent(QResizeEvent *)
{
    quick_button_->resize(91*scale_w_, 45*scale_h_);
    quick_button_->move(this->width() - quick_button_->width(), 3);
    save_button->resize(this->width() - quick_button_->width(), this->height());
    save_button->move(0, 0);
}

void LowerWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    resizeEvent(nullptr);

    save_button->SetScale(scale_w, scale_h);
}

SaveButton::SaveButton(QWidget *parent) : QWidget(parent)
{
    hotedit_ = new QPushButton(QIcon(":/image/Image/MainWidget/hotedit.png"), "HotEdit", this);
    int_out_ = new QPushButton(QIcon(":/image/Image/MainWidget/int_out.png"), "I/O", this);
    manual_control_ = new QPushButton(QIcon(":/image/Image/MainWidget/manual_control.png"), tr("手动操纵"), this);
    auto_produce_ = new QPushButton(QIcon(":/image/Image/MainWidget/auto_produce.png"), tr("自动生产窗口"), this);
    program_Editor_ = new QPushButton(QIcon(":/image/Image/MainWidget/program_Editor.png"), tr("程序编辑器"), this);
    program_data_ = new QPushButton(QIcon(":/image/Image/MainWidget/program_data.png"), tr("程序数据"), this);
    backup_recovery_ = new QPushButton(QIcon(":/image/Image/MainWidget/backup_recovery.png"), tr("备份与恢复"), this);
    calibration_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibration.png"), tr("校准"), this);
    control_panel_ = new QPushButton(QIcon(":/image/Image/MainWidget/control_panel.png"), tr("控制面板"), this);
    file_manager_ = new QPushButton(QIcon(":/image/Image/MainWidget/file_manager.png"), tr("资源管理器"), this);
    system_info_ = new QPushButton(QIcon(":/image/Image/MainWidget/system_info.png"), tr("系统信息"), this);

    scale_w_ = 1;
    scale_h_ = 1;
}

void SaveButton::resizeEvent(QResizeEvent *)
{
    hotedit_->resize(90*scale_w_, 41*scale_h_);
    int_out_->resize(hotedit_->size());
    manual_control_->resize(int_out_->size());
    auto_produce_->resize(manual_control_->size());
    program_Editor_->resize(auto_produce_->size());
    program_data_->resize(program_Editor_->size());
    backup_recovery_->resize(program_data_->size());
    calibration_->resize(backup_recovery_->size());
    control_panel_->resize(calibration_->size());
    file_manager_->resize(control_panel_->size());
    system_info_->resize(file_manager_->size());

    ShowButton();
}

void SaveButton::ShowButton()
{
    hotedit_->hide();
    int_out_->hide();
    manual_control_->hide();
    auto_produce_->hide();
    program_Editor_->hide();
    program_data_->hide();
    backup_recovery_->hide();
    calibration_->hide();
    control_panel_->hide();
    file_manager_->hide();
    system_info_->hide();

    for(int i = 0; i < show_order_.size(); i++)
    {
        QPushButton *button = show_order_.at(i);
        button->show();

        if(i == 0)
        {
            button->move(2*scale_w_, 3*scale_h_);
        }
        else
        {
            button->move(show_order_.at(i-1)->x() + show_order_.at(i-1)->width() + 1*scale_w_, show_order_.at(i-1)->y());
        }
    }
}

bool SaveButton::IsExist(QPushButton *button)
{
    for(int i = 0; i < show_order_.size(); i++)
        if(show_order_.at(i) == button)
            return true;
    return false;
}

bool SaveButton::AddButton(QPushButton *button)
{
    if(show_order_.size() >= 6) //最多只能打开六个页面
        return false;
    show_order_.append(button);
    ShowButton();
    return true;
}

void SaveButton::RemButton(QPushButton *button)
{
    show_order_.removeAll(button);
    ShowButton();
}

int SaveButton::GetSize()
{
    return show_order_.size();
}

void SaveButton::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    resizeEvent(nullptr);
}

QuickButton::QuickButton(QWidget *parent) : QPushButton(parent)
{
    QFrame *frame = new QFrame(this);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(frame);
    main_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(main_layout);

    frame->setFrameStyle(QFrame::Box);

    QVBoxLayout *layout = new QVBoxLayout(frame);
    tips_unit_label_ = new QLabel(frame);

    QHBoxLayout *tips_layout = new QHBoxLayout;
    tips_action_label_ = new QLabel(frame);
    tips_increment_label_ = new QLabel(frame);
    tips_layout->addWidget(tips_action_label_);
    tips_layout->addWidget(tips_increment_label_);
    tips_layout->setSpacing(0);

    layout->addWidget(tips_unit_label_);
    layout->addLayout(tips_layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    frame->setLayout(layout);

    tips_unit_label_->setAlignment(Qt::AlignCenter);

    tips_unit_label_->setText("ROB_1");

    connect(TeachState::instance().data(), &TeachState::ActionMod, this, &QuickButton::ActionMod);
    connect(TeachState::instance().data(), &TeachState::Incrent, this, &QuickButton::Incrent);
}

void QuickButton::ActionMod(ActionMode mod)
{
    QString qss = "border-image: url(";
    switch(mod)
    {
    case ActionMode::Axis1_3:
    {
        qss.append(":/image/Image/MainWidget/lower_week1_3.png");
        break;
    }
    case ActionMode::Axis4_6:
    {
        qss.append(":/image/Image/MainWidget/lower_week_4_6.png");
        break;
    }
    case ActionMode::Linear:
    {
        qss.append(":/image/Image/MainWidget/linear_mod.png");
        break;
    }
    case ActionMode::Relocate:
    {
        qss.append(":/image/Image/MainWidget/relocation_mod.png");
        break;
    }
    }
    qss.append(")");
    tips_action_label_->setStyleSheet(qss);
}

void QuickButton::Incrent(Increment ict)
{
    QString qss = "border-image: url(";
    switch(ict)
    {
    case Increment::None:
    {
        qss.append(":/image/Image/MainWidget/increment_nullptr.png");
        break;
    }
    case Increment::Small:
    {
        qss.append(":/image/Image/MainWidget/increment_small.png");
        break;
    }
    case Increment::Centre:
    {
        qss.append(":/image/Image/MainWidget/increment_in.png");
        break;
    }
    case Increment::Large:
    {
        qss.append(":/image/Image/MainWidget/increment_large.png");
        break;
    }
    case Increment::User:
    {
        qss.append(":/image/Image/MainWidget/increment_user.png");
        break;
    }
    }
    qss.append(")");
    tips_increment_label_->setStyleSheet(qss);
}

QuickWidget::QuickWidget(QWidget *parent) : QFrame(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addStretch();
    layout->addWidget(&unit_button_);
    layout->addWidget(&increment_button_);
    layout->addWidget(&run_mod_button_);
    layout->addWidget(&step_mod_button_);
    layout->addWidget(&speed_button_);
    layout->addWidget(&task_button_);
    layout->addStretch();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    unit_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    increment_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    run_mod_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    step_mod_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    speed_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    task_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    unit_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/unit_button.png)");
    increment_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/increment_button.png)");
    run_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/run_mod_button.png)");
    step_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_button.png)");
    speed_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/speed_button.png)");
    task_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/taskq_button.png)");

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);

    this->setWindowFlags(Qt::Popup);

    ConnectInit();
}

void QuickWidget::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void QuickWidget::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void QuickWidget::Incrent(Increment ict)
{
    QString qss = "border-image: url(";
    switch(ict)
    {
    case Increment::None:
    {
        qss.append(":/image/Image/MainWidget/increment_nullptr.png");
        break;
    }
    case Increment::Small:
    {
        qss.append(":/image/Image/MainWidget/increment_small.png");
        break;
    }
    case Increment::Centre:
    {
        qss.append(":/image/Image/MainWidget/increment_in.png");
        break;
    }
    case Increment::Large:
    {
        qss.append(":/image/Image/MainWidget/increment_large.png");
        break;
    }
    case Increment::User:
    {
        qss.append(":/image/Image/MainWidget/increment_user.png");
        break;
    }
    }
    qss.append(")");
    increment_button_.setStyleSheet(qss);
}

void QuickWidget::ConnectInit()
{
    connect(TeachState::instance().data(), &TeachState::Incrent, this, &QuickWidget::Incrent);
}

TaskQuickWidget::TaskQuickWidget(QWidget *parent) : QuickBase(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tips_label_ = new QLabel(tr("要\"停止\"和\"启动\"的任务"), this);

    QHBoxLayout *tp_layout = new QHBoxLayout;

    QVBoxLayout *task_layout = new QVBoxLayout;
    tips1_label_ = new QLabel(tr("任务"), this);
    button_ = new QCheckBox("T_ROB1", this);
    task_layout->addWidget(tips1_label_);
    task_layout->addWidget(button_);

    QVBoxLayout *tips_layout = new QVBoxLayout;
    tips2_label_ = new QLabel("I AE", this);
    label_ = new QLabel("Normal", this);
    tips_layout->addWidget(tips2_label_);
    tips_layout->addWidget(label_);

    tp_layout->addLayout(task_layout);
    tp_layout->addLayout(tips_layout);

    layout->addWidget(tips_label_);
    layout->addLayout(tp_layout);
    layout->addStretch();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(layout);

    tips_label_->setAlignment(Qt::AlignCenter);
    tips1_label_->setFrameShape(QFrame::Box);
    tips2_label_->setFrameShape(QFrame::Box);
    tips1_label_->setStyleSheet("background-color: rgb(192, 192, 192)");
    tips2_label_->setStyleSheet("background-color: rgb(192, 192, 192)");
}

SpeedWidget::SpeedWidget(QWidget *parent) : QuickBase(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    speed_ = 5;

    tips_label_ = new QLabel(tr("速度"), this);
    speed_label_ = new QLabel(QString::number(speed_) + "%", this);

    QHBoxLayout *hb1_layout = new QHBoxLayout;
    cut1_button_ = new QPushButton(this);
    add1_button_ = new QPushButton(this);
    hb1_layout->addStretch();
    hb1_layout->addWidget(cut1_button_);
    hb1_layout->addWidget(add1_button_);
    hb1_layout->addStretch();

    QHBoxLayout *hb5_layout = new QHBoxLayout;
    cut5_button_ = new QPushButton(this);
    add5_button_ = new QPushButton(this);
    hb5_layout->addStretch();
    hb5_layout->addWidget(cut5_button_);
    hb5_layout->addWidget(add5_button_);
    hb5_layout->addStretch();

    QHBoxLayout *set_layout = new QHBoxLayout;
    set0_button_ = new QPushButton(this);
    set25_button_ = new QPushButton(this);
    set50_button_ = new QPushButton(this);
    set100_button_ = new QPushButton(this);
    set_layout->addWidget(set0_button_);
    set_layout->addWidget(set25_button_);
    set_layout->addWidget(set50_button_);
    set_layout->addWidget(set100_button_);

    layout->addWidget(tips_label_);
    layout->addStretch();
    layout->addWidget(speed_label_);
    layout->addStretch();
    layout->addLayout(hb1_layout);
    layout->addLayout(hb5_layout);
    layout->addLayout(set_layout);

    this->setLayout(layout);

    speed_label_->setAlignment(Qt::AlignCenter);
    cut1_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cut5_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    add1_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    add5_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    set0_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    set25_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    set50_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    set100_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    cut1_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/cut1.png)");
    cut5_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/cut5.png)");
    add1_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/add1.png)");
    add5_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/add5.png)");
    set0_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/set0.png)");
    set25_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/set25.png)");
    set50_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/set50.png)");
    set100_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/set100.png)");

    connect(cut1_button_, &QPushButton::clicked,
            [=]()
    {
        if(speed_ == 0)
            return;

        speed_--;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });

    connect(cut5_button_, &QPushButton::clicked,
            [=]()
    {
        if(speed_ - 5 < 0)
            return;

        speed_ -= 5;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });

    connect(add1_button_, &QPushButton::clicked,
            [=]()
    {
        if(speed_ == 100)
            return;

        speed_++;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });

    connect(add5_button_, &QPushButton::clicked,
            [=]()
    {
        if(speed_ + 5 > 100)
            return;

        speed_ += 5;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });

    connect(set0_button_, &QPushButton::clicked,
            [=]()
    {
        speed_ = 0;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });

    connect(set25_button_, &QPushButton::clicked,
            [=]()
    {
        speed_ = 25;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });

    connect(set50_button_, &QPushButton::clicked,
            [=]()
    {
        speed_ = 50;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });

    connect(set100_button_, &QPushButton::clicked,
            [=]()
    {
        speed_ = 100;
        speed_label_->setText(QString::number(speed_) + "%");

        emit SpeedChange(speed_);
    });
}

double SpeedWidget::GetSpeed()
{
    double x = speed_;
    double y = 100;

    return x/y;
}

StepModWidget::StepModWidget(QWidget *parent) : QuickBase(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tips_label_ = new QLabel(tr("步进模式"), this);

    QHBoxLayout *join_layout = new QHBoxLayout;
    join_button_ = new QPushButton(this);
    join_layout->addStretch();
    join_layout->addWidget(join_button_);
    join_layout->addStretch();
    join_label_ = new QLabel(tr("步进入"), this);

    QHBoxLayout *out_layout = new QHBoxLayout;
    out_button_ = new QPushButton(this);
    out_layout->addStretch();
    out_layout->addWidget(out_button_);
    out_layout->addStretch();
    out_label_ = new QLabel(tr("步进出"), this);

    QHBoxLayout *skip_layout = new QHBoxLayout;
    skip_button_ = new QPushButton(this);
    skip_layout->addStretch();
    skip_layout->addWidget(skip_button_);
    skip_layout->addStretch();
    skip_label_ = new QLabel(tr("跳过"), this);

    QHBoxLayout *lower_layout = new QHBoxLayout;
    lower_button_ = new QPushButton(this);
    lower_layout->addStretch();
    lower_layout->addWidget(lower_button_);
    lower_layout->addStretch();
    lower_label_ = new QLabel(tr("下一移动指令"), this);

    layout->addWidget(tips_label_);
    layout->addLayout(join_layout);
    layout->addWidget(join_label_);
    layout->addLayout(out_layout);
    layout->addWidget(out_label_);
    layout->addLayout(skip_layout);
    layout->addWidget(skip_label_);
    layout->addLayout(lower_layout);
    layout->addWidget(lower_label_);

    this->setLayout(layout);

    join_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_join.png)");
    out_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_out.png)");
    skip_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_skip.png)");
    lower_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_lower.png)");

    join_label_->setAlignment(Qt::AlignCenter);
    out_label_->setAlignment(Qt::AlignCenter);
    skip_label_->setAlignment(Qt::AlignCenter);
    lower_label_->setAlignment(Qt::AlignCenter);

    join_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    out_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    skip_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lower_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

RunModWidget::RunModWidget(QWidget *parent) : QuickBase(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tips_label_ = new QLabel(tr("运行模式"), this);

    QHBoxLayout *one_layout = new QHBoxLayout;
    one_button_ = new QPushButton(this);
    one_layout->addStretch();
    one_layout->addWidget(one_button_);
    one_layout->addStretch();
    one_label_ = new QLabel(tr("单周"), this);

    QHBoxLayout *series_layout = new QHBoxLayout;
    series_button_ = new QPushButton(this);
    series_layout->addStretch();
    series_layout->addWidget(series_button_);
    series_layout->addStretch();
    series_label_ = new QLabel(tr("连续"), this);

    layout->addWidget(tips_label_);
    layout->addLayout(one_layout);
    layout->addWidget(one_label_);
    layout->addLayout(series_layout);
    layout->addWidget(series_label_);

    this->setLayout(layout);

    one_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/one_run_mod_button.png)");
    series_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/series_run_mod_button.png)");

    one_label_->setAlignment(Qt::AlignCenter);
    series_label_->setAlignment(Qt::AlignCenter);

    one_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    series_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QuickBase::QuickBase(QWidget *parent) : QFrame(parent)
{
    this->setWindowFlags(Qt::Popup);
    this->setFrameShape(QFrame::Box);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
}

void QuickBase::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void QuickBase::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

IncrementWidget::IncrementWidget(QWidget *parent) : QuickBase (parent)
{
    auto *layout = new QHBoxLayout(this);

    right_widget_ = new QWidget(this);
    auto *right_layout = new QVBoxLayout(right_widget_);
    tips_label_ = new QLabel(tr("增量"), right_widget_);
    none_button_ = new ImageButton(tr("无"), ":/image/Image/MainWidget/increment_nullptr.png", right_widget_);
    small_button_ = new ImageButton(tr("小"), ":/image/Image/MainWidget/increment_small.png", right_widget_);
    centre_button_ = new ImageButton(tr("中"), ":/image/Image/MainWidget/increment_in.png", right_widget_);
    large_button_ = new ImageButton(tr("大"), ":/image/Image/MainWidget/increment_large.png", right_widget_);
    user_button_ = new ImageButton(tr("用户模块"), ":/image/Image/MainWidget/increment_user.png", right_widget_);
    show_val_ = new QPushButton(tr(" << 显示值"), right_widget_);
    none_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    small_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centre_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    large_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    user_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_layout->addWidget(tips_label_);
    right_layout->addWidget(none_button_);
    right_layout->addWidget(small_button_);
    right_layout->addWidget(centre_button_);
    right_layout->addWidget(large_button_);
    right_layout->addWidget(user_button_);
    right_layout->addWidget(show_val_);
    right_layout->setStretchFactor(tips_label_, 1);
    right_layout->setStretchFactor(none_button_, 2);
    right_layout->setStretchFactor(small_button_, 2);
    right_layout->setStretchFactor(centre_button_, 2);
    right_layout->setStretchFactor(large_button_, 2);
    right_layout->setStretchFactor(user_button_, 2);
    right_layout->setStretchFactor(show_val_, 1);

    left_widget_ = new QWidget(this);
    auto *left_layout = new QVBoxLayout(left_widget_);

    tips1_label_ = new QLabel(tr("机械单元: ROB_1"), left_widget_);
    table_widget = new QTableWidget(left_widget_);
    key_widget_ = new QFrame(left_widget_);
    auto *key_layout = new QHBoxLayout(key_widget_);
    auto *tips_layout = new QVBoxLayout;
    tips_min_label_ = new QLabel(tr("最小"), key_widget_);
    min_label_ = new QLabel(key_widget_);
    tips_max_label_ = new QLabel(tr("最大"), key_widget_);
    max_label_ = new QLabel(key_widget_);
    tips_layout->addWidget(tips_min_label_);
    tips_layout->addWidget(min_label_);
    tips_layout->addWidget(tips_max_label_);
    tips_layout->addWidget(max_label_);
    num_key_ = new NumKey(key_widget_);
    key_layout->addLayout(tips_layout);
    key_layout->addWidget(num_key_);
    key_layout->setContentsMargins(0, 0, 0, 0);
    key_layout->setStretchFactor(tips_layout, 1);
    key_layout->setStretchFactor(num_key_, 2);
    num_edit_ = new QLineEdit;
    num_edit_->hide();

    left_layout->addWidget(tips1_label_);
    left_layout->addWidget(table_widget);
    left_layout->addWidget(key_widget_);
    left_layout->setStretchFactor(tips1_label_, 1);
    left_layout->setStretchFactor(table_widget, 6);
    left_layout->setStretchFactor(key_widget_, 12);
    left_layout->setContentsMargins(0, 0, 0, 0);
    left_layout->setSpacing(0);

    layout->addWidget(left_widget_);
    layout->addWidget(right_widget_);
    layout->setStretchFactor(right_widget_, 2);
    layout->setStretchFactor(left_widget_, 3);

    left_widget_->hide();

    none_button_->SetIsSelect(true);

    num_key_->GetFEButton()->hide();

    tips_min_label_->setAlignment(Qt::AlignLeft);
    min_label_->setAlignment(Qt::AlignLeft);
    tips_max_label_->setAlignment(Qt::AlignLeft);
    max_label_->setAlignment(Qt::AlignLeft);

    table_widget->setRowCount(3);
    table_widget->setColumnCount(3);
    table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_widget->verticalHeader()->hide();

    old_item_ = nullptr;

    this->setMinimumSize(200, 350); //设置窗口最小大小

    QStringList heads;
    heads << tr("增量") << tr("值" )<< "";
    table_widget->setHorizontalHeaderLabels(heads);

    ConnectInit();
    SetTableData();
}

void IncrementWidget::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;
    if(!item->isSelected())
        return;

    auto *it1 = table_widget->item(0, 1);
    auto *it2 = table_widget->item(1, 1);
    auto *it3 = table_widget->item(2, 1);

    if(item == it1)
    {
        min_label_->setText("0.0");
        max_label_->setText("0.0025");
    }
    else if(item == it2)
    {
        min_label_->setText("0.0");
        max_label_->setText("5.0");
    }
    else if(item == it3)
    {
        min_label_->setText("0.0");
        max_label_->setText("0.009");
    }
    else
    {
        min_label_->clear();
        max_label_->clear();
    }

    if(item == it1 || item == it2 || item == it3)
    {
        int row = table_widget->row(item);

        if(num_edit_.isNull())
        {
            num_edit_ = new QLineEdit(this);
            num_edit_->hide();
        }
        else if(old_item_ == item)
        {
            return;
        }
        else
        {
            int old_row = table_widget->row(old_item_);
            table_widget->removeCellWidget(old_row, 1);
            num_edit_ = new QLineEdit(this);
        }

        old_item_ = item;

        num_edit_->setText(item->text());
        table_widget->setCellWidget(row, 1, num_edit_);

        num_key_->Init(num_edit_);
    }
    else
    {
        return;
    }
}

void IncrementWidget::NumDefine()
{
    if(num_edit_.isNull() || old_item_ == nullptr)
        return;

    auto text = num_edit_->text();

    bool isdouble;
    double val = text.toDouble(&isdouble);
    int row = table_widget->row(old_item_);
    ActionMode mod;

    table_widget->removeCellWidget(row, 1);

    if(!isdouble)
    {
        QMessageBox::warning(this, tr("错误"), tr("请输入浮点数"));

        num_edit_ = new QLineEdit(this);
        num_edit_->setText(text);

        table_widget->setCellWidget(row, 1, num_edit_);
        num_key_->Init(num_edit_);

        return;
    }
    if(row == 0)
        mod = ActionMode::Axis1_3;
    else if(row == 1)
        mod = ActionMode::Linear;
    else
        mod = ActionMode::Relocate;

    if(!TeachState::instance()->SetIncrVal(mod, val))
    {
        QMessageBox::warning(this, tr("错误"), tr("请按照要求输入大小"));

        num_edit_ = new QLineEdit(this);
        num_edit_->setText(text);

        table_widget->setCellWidget(row, 1, num_edit_);
        num_key_->Init(num_edit_);

        return;
    }

    old_item_->setText(text);

    min_label_->clear();
    max_label_->clear();
}

void IncrementWidget::NumCancel()
{
    if(num_edit_.isNull() || old_item_ == nullptr)
        return;

    table_widget->removeCellWidget(table_widget->row(old_item_), 1);

    min_label_->clear();
    max_label_->clear();
}

void IncrementWidget::ConnectInit()
{
    connect(none_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetIncrement(Increment::None);
        SetTableData(0);
    });

    connect(small_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetIncrement(Increment::Small);
        SetTableData(1);
    });

    connect(centre_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetIncrement(Increment::Centre);
        SetTableData(2);
    });

    connect(large_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetIncrement(Increment::Large);
        SetTableData(3);
    });

    connect(user_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetIncrement(Increment::User);
        SetTableData(4);
    });

    connect(show_val_, &QPushButton::clicked,
            [=]()
    {
        if(left_widget_->isHidden())
        {
            left_widget_->show();

            show_val_->setText(tr("隐藏值 >> "));

            emit ShowWidgetSize(570, 384);
        }
        else
        {
            left_widget_->hide();

            show_val_->setText(tr(" << 显示值"));

            emit ShowWidgetSize(250, 384);
        }
    });

    connect(this, &IncrementWidget::ShowWindow,
            [=]()
    {
        if(!left_widget_->isHidden())
        {
            emit ShowWidgetSize(570, 384);
        }
        else
        {
            emit ShowWidgetSize(250, 384);
        }
    });

    connect(TeachState::instance().data(), &TeachState::Incrent,
            [=](Increment inct)
    {
        switch(inct)
        {
        case Increment::None:
        {
            Clicked(0);
            break;
        }
        case Increment::Small:
        {
            Clicked(1);
            break;
        }
        case Increment::Centre:
        {
            Clicked(2);
            break;
        }
        case Increment::Large:
        {
            Clicked(3);
            break;
        }
        case Increment::User:
        {
            Clicked(4);
            break;
        }
        }
    });

    connect(table_widget, &QTableWidget::itemClicked, this, &IncrementWidget::ItemClicked);

    connect(num_key_, &NumKey::DefineButton, this, &IncrementWidget::NumDefine);

    connect(num_key_, &NumKey::CancelButton, this, &IncrementWidget::NumCancel);
}

void IncrementWidget::Clicked(int button)
{
    switch(button)
    {
    case 0:
    {
        none_button_->SetIsSelect(true);
        small_button_->SetIsSelect(false);
        centre_button_->SetIsSelect(false);
        large_button_->SetIsSelect(false);
        user_button_->SetIsSelect(false);

        break;
    }
    case 1:
    {
        none_button_->SetIsSelect(false);
        small_button_->SetIsSelect(true);
        centre_button_->SetIsSelect(false);
        large_button_->SetIsSelect(false);
        user_button_->SetIsSelect(false);

        break;
    }
    case 2:
    {
        none_button_->SetIsSelect(false);
        small_button_->SetIsSelect(false);
        centre_button_->SetIsSelect(true);
        large_button_->SetIsSelect(false);
        user_button_->SetIsSelect(false);

        break;
    }
    case 3:
    {
        none_button_->SetIsSelect(false);
        small_button_->SetIsSelect(false);
        centre_button_->SetIsSelect(false);
        large_button_->SetIsSelect(true);
        user_button_->SetIsSelect(false);

        break;
    }
    case 4:
    {
        none_button_->SetIsSelect(false);
        small_button_->SetIsSelect(false);
        centre_button_->SetIsSelect(false);
        large_button_->SetIsSelect(false);
        user_button_->SetIsSelect(true);

        break;
    }
    }

    if(!left_widget_->isHidden())
        SetTableData(button);
}

void IncrementWidget::SetTableData(int type)
{
    table_widget->clearContents();
    table_widget->setRowCount(3);
    table_widget->setColumnCount(3);

    auto *item0_0 = new QTableWidgetItem(tr("轴"));
    auto *item1_0 = new QTableWidgetItem(tr("线性"));
    auto *item2_0 = new QTableWidgetItem(tr("重定向"));

    auto *item0_2 = new QTableWidgetItem("(rad)");
    auto *item1_2 = new QTableWidgetItem("(mm)");
    auto *item2_2 = new QTableWidgetItem("(red)");

    item0_0->setFlags(Qt::NoItemFlags);
    item1_0->setFlags(Qt::NoItemFlags);
    item2_0->setFlags(Qt::NoItemFlags);
    item0_2->setFlags(Qt::NoItemFlags);
    item1_2->setFlags(Qt::NoItemFlags);
    item2_2->setFlags(Qt::NoItemFlags);

    table_widget->setItem(0, 0, item0_0);
    table_widget->setItem(1, 0, item1_0);
    table_widget->setItem(2, 0, item2_0);
    table_widget->setItem(0, 2, item0_2);
    table_widget->setItem(1, 2, item1_2);
    table_widget->setItem(2, 2, item2_2);

    auto *item0_1 = new QTableWidgetItem;
    auto *item1_1 = new QTableWidgetItem;
    auto *item2_1 = new QTableWidgetItem;

    item0_1->setText(QString::number(TeachState::instance()->GetIncrVar(ActionMode::Axis1_3)));
    item1_1->setText(QString::number(TeachState::instance()->GetIncrVar(ActionMode::Linear)));
    item2_1->setText(QString::number(TeachState::instance()->GetIncrVar(ActionMode::Relocate)));

    switch(type)
    {
    case 0:
    {
        item0_1->setFlags(Qt::NoItemFlags);
        item1_1->setFlags(Qt::NoItemFlags);
        item2_1->setFlags(Qt::NoItemFlags);

        tips_min_label_->hide();
        min_label_->hide();
        tips_max_label_->hide();
        max_label_->hide();
        num_key_->hide();
        break;
    }
    case 1:
    {
        item0_1->setFlags(Qt::NoItemFlags);
        item1_1->setFlags(Qt::NoItemFlags);
        item2_1->setFlags(Qt::NoItemFlags);

        tips_min_label_->hide();
        min_label_->hide();
        tips_max_label_->hide();
        max_label_->hide();
        num_key_->hide();
        break;
    }
    case 2:
    {
        item0_1->setFlags(Qt::NoItemFlags);
        item1_1->setFlags(Qt::NoItemFlags);
        item2_1->setFlags(Qt::NoItemFlags);

        tips_min_label_->hide();
        min_label_->hide();
        tips_max_label_->hide();
        max_label_->hide();
        num_key_->hide();
        break;
    }
    case 3:
    {
        item0_1->setFlags(Qt::NoItemFlags);
        item1_1->setFlags(Qt::NoItemFlags);
        item2_1->setFlags(Qt::NoItemFlags);

        tips_min_label_->hide();
        min_label_->hide();
        tips_max_label_->hide();
        max_label_->hide();
        num_key_->hide();
        break;
    }
    case 4:
    {
        tips_min_label_->show();
        min_label_->show();
        tips_max_label_->show();
        max_label_->show();
        num_key_->show();
        break;
    }
    }

    table_widget->setItem(0, 1, item0_1);
    table_widget->setItem(1, 1, item1_1);
    table_widget->setItem(2, 1, item2_1);
}

UnitQuickWidget::UnitQuickWidget(QWidget *parent) : QuickBase (parent)
{
    auto *layout = new QVBoxLayout(this);
    stack_widget_ = new QStackedWidget(this);

    layout->addWidget(stack_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    min_widget_ = new UnitMinWidget(this);
    max_widget_ = new UnitMaxWidget(this);

    stack_widget_->addWidget(min_widget_);
    stack_widget_->addWidget(max_widget_);

    ConnectInit();
}

void UnitQuickWidget::SetScale(double scale_w, double scale_h)
{
    min_widget_->SetScale(scale_w, scale_h);
    max_widget_->SetScale(scale_w, scale_h);
}

void UnitQuickWidget::HideWidget()
{
    min_widget_->Init();
}

void UnitQuickWidget::ShowWidget()
{
    if(stack_widget_->currentIndex() == 1)
        emit ModSize(570, 384);
}

void UnitQuickWidget::ConnectInit()
{
    connect(min_widget_, &UnitMinWidget::Change,
            [=]()
    {
        stack_widget_->setCurrentIndex(1);

        emit ModSize(570, 384);

        max_widget_->Init();
    });

    connect(max_widget_, &UnitMaxWidget::Change,
            [=]()
    {
        stack_widget_->setCurrentIndex(0);

        emit ModSize(250, 384);

        min_widget_->Init();
    });

    connect(this, &UnitQuickWidget::HideWindow, this, &UnitQuickWidget::HideWidget);

    connect(this, &UnitQuickWidget::ShowWindow, this, &UnitQuickWidget::ShowWidget);
}

UnitMinWidget::UnitMinWidget(QWidget *parent) : QWidget (parent)
{
    ico_label_ = new QLabel(this);

    fm_widget_ = new QFrame(this);
    auto *fm_layout = new QVBoxLayout(fm_widget_);
    title_label_ = new QLabel("ROB_1", fm_widget_);
    auto *label_layout = new QHBoxLayout;
    action_button = new QPushButton(fm_widget_);
    cordss_button = new QPushButton(fm_widget_);
    label_layout->addWidget(action_button);
    label_layout->addWidget(cordss_button);
    tool_button_ = new QPushButton(QIcon(":/image/Image/MainWidget/unit_tool_button.png"), TeachState::instance()->GetCurrentTool(), fm_widget_);
    work_button_ = new QPushButton(QIcon(":/image/Image/MainWidget/unit_work_button.png"), TeachState::instance()->GetCurrentWork(), fm_widget_);
    action_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cordss_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    action_button->setFlat(true);
    cordss_button->setFlat(true);
    tool_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    work_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    fm_layout->addStretch();
    fm_layout->addWidget(title_label_);
    fm_layout->addLayout(label_layout);
    fm_layout->addWidget(tool_button_);
    fm_layout->addWidget(work_button_);
    fm_layout->setStretchFactor(title_label_, 1);
    fm_layout->setStretchFactor(label_layout, 2);
    fm_layout->setStretchFactor(tool_button_, 2);
    fm_layout->setStretchFactor(work_button_, 2);
    fm_widget_->setFrameShape(QFrame::Box);

    show_left_ = new QPushButton(tr(" << 显示详情"), this);

    tool_widget_ = new QWidget(this);
    auto *tool_layout = new QGridLayout(tool_widget_);
    tclose_button_ = new QPushButton(tool_widget_);
    wek1_button_ = new ImageButton("1-3", ":/image/Image/MainWidget/week1_3_mod.bmp", tool_widget_);
    wek4_button_ = new ImageButton("4-6", ":/image/Image/MainWidget/week_4_6_mod.png", tool_widget_);
    line_button_ = new ImageButton(tr("线性"), ":/image/Image/MainWidget/linear_mod.png", tool_widget_);
    relo_button_ = new ImageButton(tr("重定位"), ":/image/Image/MainWidget/relocation_mod.png", tool_widget_);
    tclose_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wek1_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wek4_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    line_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    relo_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tool_layout->addWidget(tclose_button_, 0, 3);
    tool_layout->addWidget(wek1_button_, 1, 0);
    tool_layout->addWidget(wek4_button_, 1, 1);
    tool_layout->addWidget(line_button_, 1, 2);
    tool_layout->addWidget(relo_button_, 1, 3);

    work_widget_ = new QWidget(this);
    auto *work_layout = new QGridLayout(work_widget_);
    wclose_button_ = new QPushButton(work_widget_);
    wear_button_ = new ImageButton(tr("大地坐标"), ":/image/Image/MainWidget/earth_coordinate.png", work_widget_);
    wbas_button_ = new ImageButton(tr("基坐标"), ":/image/Image/MainWidget/base_coordinate.png", work_widget_);
    wtoo_button_ = new ImageButton(tr("工具"), ":/image/Image/MainWidget/tool_coordinate.png", work_widget_);
    wwor_button_ = new ImageButton(tr("工件坐标"), ":/image/Image/MainWidget/work_coordinate.png", work_widget_);
    wclose_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wear_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wbas_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wtoo_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wwor_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    work_layout->addWidget(wclose_button_, 0, 3);
    work_layout->addWidget(wear_button_, 1, 0);
    work_layout->addWidget(wbas_button_, 1, 1);
    work_layout->addWidget(wtoo_button_, 1, 2);
    work_layout->addWidget(wwor_button_, 1, 3);

    tool_widget_->hide();
    work_widget_->hide();
    tclose_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/close_button.png)");
    wclose_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/close_button.png)");
    action_button->setStyleSheet("border-image: url(:/image/Image/MainWidget/week1_3_mod.bmp)");
    cordss_button->setStyleSheet("border-image: url(:/image/Image/MainWidget/none.png)");
    ico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/unit_button.png)");
    title_label_->setAlignment(Qt::AlignCenter);
    ico_label_->setFrameShape(QFrame::Box);

    select_cordas_ = new SelectCordas(this);
    select_cordas_->hide();

    scale_h_ = scale_w_ = 1;
    parent_point = dynamic_cast<UnitQuickWidget*>(parent);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    tool_widget_->setAutoFillBackground(true);
    work_widget_->setAutoFillBackground(true);
    this->setPalette(p);
    tool_widget_->setPalette(p);
    work_widget_->setPalette(p);

    ConnectInit();
}

void UnitMinWidget::resizeEvent(QResizeEvent *)
{
    ico_label_->resize(72*scale_w_, 66*scale_h_);
    fm_widget_->resize(139*scale_w_, 228*scale_h_);
    show_left_->resize(204*scale_w_, 40*scale_h_);

    fm_widget_->move(58*scale_w_, 44*scale_h_);
    ico_label_->move(92*scale_w_, 5*scale_h_);
    show_left_->move(37*scale_w_, 336*scale_h_);

    if(!tool_widget_->isHidden())
    {
        tool_widget_->resize(364*scale_w_, 194*scale_h_);
        tool_widget_->move(0, 95*scale_h_);
    }
    if(!work_widget_->isHidden())
    {
        work_widget_->resize(364*scale_w_, 194*scale_h_);
        work_widget_->move(0, 95*scale_h_);
    }
    if(!select_cordas_->isHidden())
    {
        select_cordas_->resize(254*scale_w_, 237*scale_h_);
        select_cordas_->move(5*scale_w_, 102*scale_h_);
    }
}

void UnitMinWidget::Init()
{
    if(!tool_widget_->isHidden())
        tool_widget_->hide();

    if(!select_cordas_->isHidden())
        select_cordas_->hide();
}

void UnitMinWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

void UnitMinWidget::ActionClicked()
{
    emit parent_point->ModSize(368, 384);

    tool_widget_->show();
    this->resizeEvent(nullptr);

    auto mod = TeachState::instance()->GetActionMode();
    switch (mod)
    {
    case ActionMode::Axis1_3:
    {
        wek1_button_->SetIsSelect(true);
        wek4_button_->SetIsSelect(false);
        line_button_->SetIsSelect(false);
        relo_button_->SetIsSelect(false);
        break;
    }
    case ActionMode::Axis4_6:
    {
        wek1_button_->SetIsSelect(false);
        wek4_button_->SetIsSelect(true);
        line_button_->SetIsSelect(false);
        relo_button_->SetIsSelect(false);
        break;
    }
    case ActionMode::Linear:
    {
        wek1_button_->SetIsSelect(false);
        wek4_button_->SetIsSelect(false);
        line_button_->SetIsSelect(true);
        relo_button_->SetIsSelect(false);
        break;
    }
    case ActionMode::Relocate:
    {
        wek1_button_->SetIsSelect(false);
        wek4_button_->SetIsSelect(false);
        line_button_->SetIsSelect(false);
        relo_button_->SetIsSelect(true);
        break;
    }
    }
}

void UnitMinWidget::CordssClicked()
{
    auto mod = TeachState::instance()->GetActionMode();
    if(mod == ActionMode::Axis1_3 || ActionMode::Axis4_6 == mod)
        return;
    emit parent_point->ModSize(368, 384);

    work_widget_->show();
    this->resizeEvent(nullptr);

    auto coor = TeachState::instance()->GetCoordinSys();
    switch(coor)
    {
    case CoordinSys::Earth:
    {
        wear_button_->SetIsSelect(true);
        wbas_button_->SetIsSelect(false);
        wtoo_button_->SetIsSelect(false);
        wwor_button_->SetIsSelect(false);
        break;
    }
    case CoordinSys::Base:
    {
        wear_button_->SetIsSelect(false);
        wbas_button_->SetIsSelect(true);
        wtoo_button_->SetIsSelect(false);
        wwor_button_->SetIsSelect(false);
        break;
    }
    case CoordinSys::Tool:
    {
        wear_button_->SetIsSelect(false);
        wbas_button_->SetIsSelect(false);
        wtoo_button_->SetIsSelect(true);
        wwor_button_->SetIsSelect(false);
        break;
    }
    case CoordinSys::Work:
    {
        wear_button_->SetIsSelect(false);
        wbas_button_->SetIsSelect(false);
        wtoo_button_->SetIsSelect(false);
        wwor_button_->SetIsSelect(true);
        break;
    }
    }
}

void UnitMinWidget::ShowDetailsClicked()
{
    emit Change();
}

void UnitMinWidget::ToolClicked()
{
    emit parent_point->ModSize(289, 384);

    select_cordas_->show();
    resizeEvent(nullptr);

    select_cordas_->Init("tooldata");
}

void UnitMinWidget::WorkClicked()
{
    emit parent_point->ModSize(289, 384);

    select_cordas_->show();
    resizeEvent(nullptr);

    select_cordas_->Init("wobjdata");
}

void UnitMinWidget::ConnectInit()
{
    connect(TeachState::instance().data(), &TeachState::ActionMod,
            [=](ActionMode mod)
    {
        QString actqss = "border-image: url(";
        QString corqss = actqss;

        switch(mod)
        {
        case ActionMode::Axis1_3:
        {
            actqss.append(":/image/Image/MainWidget/week1_3_mod.bmp");
            corqss.append(":/image/Image/MainWidget/none.png");
            break;
        }
        case ActionMode::Axis4_6:
        {
            actqss.append(":/image/Image/MainWidget/week_4_6_mod.png");
            corqss.append(":/image/Image/MainWidget/none.png");
            break;
        }
        case ActionMode::Linear:
        {
            actqss.append(":/image/Image/MainWidget/linear_mod.png");
            corqss.append(":/image/Image/MainWidget/tool_coordinate.png");
            break;
        }
        case ActionMode::Relocate:
        {
            actqss.append(":/image/Image/MainWidget/relocation_mod.png");
            corqss.append(":/image/Image/MainWidget/work_coordinate.png");
            break;
        }
        }

        actqss.append(")");
        corqss.append(")");

        action_button->setStyleSheet(actqss);
        cordss_button->setStyleSheet(corqss);
    });

    connect(TeachState::instance().data(), &TeachState::CoordSys,
            [=](CoordinSys mod)
    {
        auto act_mod = TeachState::instance()->GetActionMode();
        if(act_mod == ActionMode::Axis1_3 || act_mod == ActionMode::Axis4_6)
        {
            cordss_button->setStyleSheet("border-image: url(:/image/Image/MainWidget/none.png)");
            return;
        }

        QString corqss = "border-image: url(";

        switch(mod)
        {
        case CoordinSys::Earth:
        {
            corqss.append(":/image/Image/MainWidget/earth_coordinate.png");
            break;
        }
        case CoordinSys::Base:
        {
            corqss.append(":/image/Image/MainWidget/base_coordinate.png");
            break;
        }
        case CoordinSys::Tool:
        {
            corqss.append(":/image/Image/MainWidget/tool_coordinate.png");
            break;
        }
        case CoordinSys::Work:
        {
            corqss.append(":/image/Image/MainWidget/work_coordinate.png");
            break;
        }
        }

        corqss.append(")");

        cordss_button->setStyleSheet(corqss);
    });

    connect(TeachState::instance().data(), &TeachState::CordTool,
            [=](QString name)
    {
        tool_button_->setText(name);
    });

    connect(TeachState::instance().data(), &TeachState::CordWork,
            [=](QString name)
    {
        work_button_->setText(name);
    });

    connect(tclose_button_, &QPushButton::clicked,
            [=]()
    {
        tool_widget_->hide();

        emit parent_point->ModSize(250, 384);
    });

    connect(wek1_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetActionMode(ActionMode::Axis1_3);

        tclose_button_->click();
    });

    connect(wek4_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetActionMode(ActionMode::Axis4_6);

        tclose_button_->click();
    });

    connect(line_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetActionMode(ActionMode::Linear);

        tclose_button_->click();
    });

    connect(relo_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetActionMode(ActionMode::Relocate);

        tclose_button_->click();
    });

    connect(wclose_button_, &QPushButton::clicked,
            [=]()
    {
        work_widget_->hide();

        emit parent_point->ModSize(250, 384);
    });

    connect(wear_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Earth);

        wclose_button_->click();
    });

    connect(wbas_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Base);

        wclose_button_->click();
    });

    connect(wtoo_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Tool);

        wclose_button_->click();
    });

    connect(wwor_button_, &ImageButton::clicked,
            [=]()
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Work);

        wclose_button_->click();
    });

    connect(select_cordas_, &SelectCordas::Close,
            [=]()
    {
        select_cordas_->hide();

        emit parent_point->ModSize(250, 384);
    });

    connect(action_button, &QPushButton::clicked, this, &UnitMinWidget::ActionClicked);

    connect(cordss_button, &QPushButton::clicked, this, &UnitMinWidget::CordssClicked);

    connect(show_left_, &QPushButton::clicked, this, &UnitMinWidget::ShowDetailsClicked);

    connect(tool_button_, &QPushButton::clicked, this, &UnitMinWidget::ToolClicked);

    connect(work_button_, &QPushButton::clicked, this, &UnitMinWidget::WorkClicked);
}

UnitMaxWidget::UnitMaxWidget(QWidget *parent) : QWidget (parent)
{
    ico_label_ = new QLabel(this);

    fm_widget_ = new QFrame(this);
    auto *fm_layout = new QVBoxLayout(fm_widget_);
    title_label_ = new QLabel("ROB_1", fm_widget_);
    tool_button_ = new QPushButton(QIcon(":/image/Image/MainWidget/unit_tool_button.png"), TeachState::instance()->GetCurrentTool(), fm_widget_);
    work_button_ = new QPushButton(QIcon(":/image/Image/MainWidget/unit_work_button.png"), TeachState::instance()->GetCurrentWork(), fm_widget_);
    tool_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    work_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    fm_layout->addStretch();
    fm_layout->addWidget(title_label_);
    fm_layout->addWidget(tool_button_);
    fm_layout->addWidget(work_button_);
    fm_layout->setStretchFactor(title_label_, 1);
    fm_layout->setStretchFactor(tool_button_, 2);
    fm_layout->setStretchFactor(work_button_, 2);
    fm_widget_->setFrameShape(QFrame::Box);

    scale_h_ = scale_w_ = 1;
    parent_point = dynamic_cast<UnitQuickWidget*>(parent);

    lower_widget_ = new UnitMaxLower(&scale_w_, &scale_h_,this);

    select_cordas_ = new SelectCordas(this);
    select_cordas_->hide();

    ico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/unit_button.png)");
    title_label_->setAlignment(Qt::AlignCenter);
    ico_label_->setFrameShape(QFrame::Box);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);

    ConnectInit();
}

void UnitMaxWidget::resizeEvent(QResizeEvent *)
{
    ico_label_->resize(72*scale_w_, 66*scale_h_);
    fm_widget_->resize(139*scale_w_, 150*scale_h_);
    lower_widget_->resize(560*scale_w_, 184*scale_h_);

    fm_widget_->move(57*scale_w_, 43*scale_h_);
    ico_label_->move(92*scale_w_, 5*scale_h_);
    lower_widget_->move(5*scale_w_, 190*scale_h_);

    if(!select_cordas_->isHidden())
    {
        select_cordas_->resize(259*scale_w_, 236*scale_h_);
        select_cordas_->move(152*scale_w_, 101*scale_h_);
    }
}

void UnitMaxWidget::Init()
{
    if(!select_cordas_->isHidden())
        select_cordas_->hide();
}

void UnitMaxWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

void UnitMaxWidget::ToolClicked()
{
    select_cordas_->show();
    resizeEvent(nullptr);

    select_cordas_->Init("tooldata");
}

void UnitMaxWidget::WorkClicked()
{
    select_cordas_->show();
    resizeEvent(nullptr);

    select_cordas_->Init("wobjdata");
}

void UnitMaxWidget::ImageClicked()
{
    auto *button = dynamic_cast<ImageButton*>(sender());
    if(button == nullptr)
        return;

    button->SetIsSelect(false);

    auto *wear_button_ = lower_widget_->wear_button_;
    auto *wbas_button_ = lower_widget_->wbas_button_;
    auto *wtoo_button_ = lower_widget_->wtoo_button_;
    auto *wwor_button_ = lower_widget_->wwor_button_;
    auto *wek1_button_ = lower_widget_->wek1_button_;
    auto *wek4_button_ = lower_widget_->wek4_button_;
    auto *line_button_ = lower_widget_->line_button_;
    auto *relo_button_ = lower_widget_->relo_button_;

    if(button == wear_button_)
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Earth);
    }
    else if(button == wbas_button_)
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Base);
    }
    else if(button == wtoo_button_)
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Tool);
    }
    else if(button == wwor_button_)
    {
        TeachState::instance()->SetCoordinSys(CoordinSys::Work);
    }
    else if(button == wek1_button_)
    {
        TeachState::instance()->SetActionMode(ActionMode::Axis1_3);
    }
    else if(button == wek4_button_)
    {
        TeachState::instance()->SetActionMode(ActionMode::Axis4_6);
    }
    else if(button == line_button_)
    {
        TeachState::instance()->SetActionMode(ActionMode::Linear);
    }
    else if(button == relo_button_)
    {
        TeachState::instance()->SetActionMode(ActionMode::Relocate);
    }
}

void UnitMaxWidget::ConnectInit()
{
    connect(select_cordas_, &SelectCordas::Close,
            [=]()
    {
        select_cordas_->hide();
    });

    connect(TeachState::instance().data(), &TeachState::CordTool,
            [=](QString name)
    {
        tool_button_->setText(name);
    });

    connect(TeachState::instance().data(), &TeachState::CordWork,
            [=](QString name)
    {
        work_button_->setText(name);
    });

    connect(TeachState::instance().data(), &TeachState::Incrent,
            [=](Increment ict)
    {
        QString qss = "border-image: url(";
        switch(ict)
        {
        case Increment::None:
        {
            qss.append(":/image/Image/MainWidget/increment_nullptr.png");
            break;
        }
        case Increment::Small:
        {
            qss.append(":/image/Image/MainWidget/increment_small.png");
            break;
        }
        case Increment::Centre:
        {
            qss.append(":/image/Image/MainWidget/increment_in.png");
            break;
        }
        case Increment::Large:
        {
            qss.append(":/image/Image/MainWidget/increment_large.png");
            break;
        }
        case Increment::User:
        {
            qss.append(":/image/Image/MainWidget/increment_user.png");
            break;
        }
        }

        qss.append(")");

        lower_widget_->incremen_label_->setStyleSheet(qss);
    });

    connect(TeachState::instance().data(), &TeachState::CoordSys,
            [=](CoordinSys cos)
    {
        switch (cos)
        {
        case CoordinSys::Earth:
        {
            lower_widget_->wear_button_->SetIsSelect(true);
            lower_widget_->wbas_button_->SetIsSelect(false);
            lower_widget_->wtoo_button_->SetIsSelect(false);
            lower_widget_->wwor_button_->SetIsSelect(false);
            break;
        }
        case CoordinSys::Base:
        {
            lower_widget_->wear_button_->SetIsSelect(false);
            lower_widget_->wbas_button_->SetIsSelect(true);
            lower_widget_->wtoo_button_->SetIsSelect(false);
            lower_widget_->wwor_button_->SetIsSelect(false);
            break;
        }
        case CoordinSys::Tool:
        {
            lower_widget_->wear_button_->SetIsSelect(false);
            lower_widget_->wbas_button_->SetIsSelect(false);
            lower_widget_->wtoo_button_->SetIsSelect(true);
            lower_widget_->wwor_button_->SetIsSelect(false);
            break;
        }
        case CoordinSys::Work:
        {
            lower_widget_->wear_button_->SetIsSelect(false);
            lower_widget_->wbas_button_->SetIsSelect(false);
            lower_widget_->wtoo_button_->SetIsSelect(false);
            lower_widget_->wwor_button_->SetIsSelect(true);
            break;
        }
        }
    });

    connect(TeachState::instance().data(), &TeachState::ActionMod,
            [=](ActionMode mod)
    {
        switch (mod)
        {
        case ActionMode::Axis1_3:
        {
            lower_widget_->wek1_button_->SetIsSelect(true);
            lower_widget_->wek4_button_->SetIsSelect(false);
            lower_widget_->line_button_->SetIsSelect(false);
            lower_widget_->relo_button_->SetIsSelect(false);

            lower_widget_->wbas_button_->setEnabled(false);
            lower_widget_->wear_button_->setEnabled(false);
            lower_widget_->wtoo_button_->setEnabled(false);
            lower_widget_->wwor_button_->setEnabled(false);
            lower_widget_->wbas_button_->SetIsSelect(false);
            lower_widget_->wear_button_->SetIsSelect(false);
            lower_widget_->wtoo_button_->SetIsSelect(false);
            lower_widget_->wwor_button_->SetIsSelect(false);
            break;
        }
        case ActionMode::Axis4_6:
        {
            lower_widget_->wek1_button_->SetIsSelect(false);
            lower_widget_->wek4_button_->SetIsSelect(true);
            lower_widget_->line_button_->SetIsSelect(false);
            lower_widget_->relo_button_->SetIsSelect(false);

            lower_widget_->wbas_button_->setEnabled(false);
            lower_widget_->wear_button_->setEnabled(false);
            lower_widget_->wtoo_button_->setEnabled(false);
            lower_widget_->wwor_button_->setEnabled(false);
            lower_widget_->wbas_button_->SetIsSelect(false);
            lower_widget_->wear_button_->SetIsSelect(false);
            lower_widget_->wtoo_button_->SetIsSelect(false);
            lower_widget_->wwor_button_->SetIsSelect(false);
            break;
        }
        case ActionMode::Linear:
        {
            lower_widget_->wek1_button_->SetIsSelect(false);
            lower_widget_->wek4_button_->SetIsSelect(false);
            lower_widget_->line_button_->SetIsSelect(true);
            lower_widget_->relo_button_->SetIsSelect(false);

            lower_widget_->wbas_button_->setEnabled(true);
            lower_widget_->wear_button_->setEnabled(true);
            lower_widget_->wtoo_button_->setEnabled(true);
            lower_widget_->wwor_button_->setEnabled(true);
            break;
        }
        case ActionMode::Relocate:
        {
            lower_widget_->wek1_button_->SetIsSelect(false);
            lower_widget_->wek4_button_->SetIsSelect(false);
            lower_widget_->line_button_->SetIsSelect(false);
            lower_widget_->relo_button_->SetIsSelect(true);

            lower_widget_->wbas_button_->setEnabled(true);
            lower_widget_->wear_button_->setEnabled(true);
            lower_widget_->wtoo_button_->setEnabled(true);
            lower_widget_->wwor_button_->setEnabled(true);
            break;
        }
        }
    });

    connect(lower_widget_->incremen_label_, &QPushButton::clicked,
            [=]()
    {
        auto inl = TeachState::instance()->GetIncrement();

        if(inl == Increment::None)
            TeachState::instance()->SetIncrement(Increment::Small);
        else
            TeachState::instance()->SetIncrement(Increment::None);
    });

    connect(lower_widget_->hide_left_, &QPushButton::clicked,
            [=]()
    {
        emit Change();
    });

    connect(lower_widget_->wear_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(lower_widget_->wbas_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(lower_widget_->wtoo_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(lower_widget_->wwor_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(lower_widget_->wek1_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(lower_widget_->wek4_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(lower_widget_->line_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(lower_widget_->relo_button_, &ImageButton::clicked, this, &UnitMaxWidget::ImageClicked);

    connect(tool_button_, &QPushButton::clicked, this, &UnitMaxWidget::ToolClicked);

    connect(work_button_, &QPushButton::clicked, this, &UnitMaxWidget::WorkClicked);
}

SelectCordas::SelectCordas(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *curr_layout = new QHBoxLayout;
    curr_group_ = new QGroupBox(this);
    auto *group_layout = new QVBoxLayout;
    curr_label_ = new QLabel(this);
    group_layout->addWidget(curr_label_);
    curr_group_->setLayout(group_layout);
    group_layout->setContentsMargins(0, 0, 0, 0);
    group_layout->setSpacing(0);

    close_button_ = new QPushButton(this);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    curr_layout->addWidget(curr_group_);
    curr_layout->addWidget(close_button_);
    curr_layout->setContentsMargins(0, 0, 0, 0);
    curr_layout->setSpacing(0);
    curr_layout->setStretchFactor(curr_group_, 5);
    curr_layout->setStretchFactor(close_button_, 1);

    table_box_ = new QTableWidget(this);

    layout->addLayout(curr_layout);
    layout->addWidget(table_box_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(curr_layout, 1);
    layout->setStretchFactor(table_box_, 5);

    table_box_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //设置表格列宽自动随窗口大小进行调整
    table_box_->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置禁止编辑

    table_box_->setShowGrid(false); //隐藏网格线
    table_box_->horizontalHeader()->hide(); //隐藏列表头
    table_box_->verticalHeader()->hide(); //隐藏行表头

    table_box_->setFrameShape(QFrame::Box);

    close_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/close_button.png)");

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);

    ConnectInit();
}

void SelectCordas::Init(QString type)
{
    if(type == "tooldata")
    {
        curr_group_->setTitle(tr("选择当前工具"));
        curr_label_->setText(TeachState::instance()->GetCurrentTool());
        type_ = 0;
    }
    else
    {
        curr_group_->setTitle(tr("选择当前工件"));
        curr_label_->setText(TeachState::instance()->GetCurrentWork());
        type_ = 1;
    }

    auto names = VarInfo::instance()->GetTypeVar(type);
    table_box_->clear();
    table_box_->setRowCount(names.size());
    table_box_->setColumnCount(1);

    for(int i = 0; i < names.size(); i++)
    {
        auto &name = names[i].first;

        table_box_->setItem(i, 0, new QTableWidgetItem(name));
    }
}

void SelectCordas::ItemClicked(QTableWidgetItem *item)
{
    if(type_ == 0) //tooldata
    {
        try {
            TeachState::instance()->SetCurrentTool(item->text());
            close_button_->click();
        } catch (ABBException) {
            QString str = QString("%1's \"tload.mass\"" + tr("含有无效数据！纠正数据后重试")).arg(item->text());
            QMessageBox::warning(this, tr("错误"), str);
        }
    }
    else if(type_ == 1) //wobjdata
    {
        TeachState::instance()->SetCurrentWork(item->text());
        close_button_->click();
    }
}

void SelectCordas::ConnectInit()
{
    connect(close_button_, &QPushButton::clicked,
            [=]()
    {
        emit Close();
    });

    connect(table_box_, &QTableWidget::itemClicked, this, &SelectCordas::ItemClicked);
}

UnitMaxLower::UnitMaxLower(double *scale_w, double *scale_h, QWidget *parent) : QFrame (parent), scale_w_(scale_w), scale_h_(scale_h)
{
    speed_widget_ = new QFrame(this);
    spico_label_ = new QLabel(speed_widget_);
    sp_label_ = new QLabel(speed_widget_);
    add_button_ = new QPushButton(speed_widget_);
    red_button_ = new QPushButton(speed_widget_);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QGridLayout(button_widget_);
    wear_button_ = new ImageButton(tr("大地坐标"), ":/image/Image/MainWidget/earth_coordinate.png", button_widget_);
    wbas_button_ = new ImageButton(tr("基坐标"), ":/image/Image/MainWidget/base_coordinate.png", button_widget_);
    wtoo_button_ = new ImageButton(tr("工具"), ":/image/Image/MainWidget/tool_coordinate.png", button_widget_);
    wwor_button_ = new ImageButton(tr("工件坐标"), ":/image/Image/MainWidget/work_coordinate.png", button_widget_);
    wek1_button_ = new ImageButton("1-3", ":/image/Image/MainWidget/week1_3_mod.bmp", button_widget_);
    wek4_button_ = new ImageButton("4-6", ":/image/Image/MainWidget/week_4_6_mod.png", button_widget_);
    line_button_ = new ImageButton(tr("线性"), ":/image/Image/MainWidget/linear_mod.png", button_widget_);
    relo_button_ = new ImageButton(tr("重定位"), ":/image/Image/MainWidget/relocation_mod.png", button_widget_);
    wek1_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wek4_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    line_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    relo_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wear_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wbas_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wtoo_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wwor_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_layout->addWidget(wear_button_, 0, 0);
    button_layout->addWidget(wbas_button_, 0, 1);
    button_layout->addWidget(wtoo_button_, 0, 2);
    button_layout->addWidget(wwor_button_, 0, 3);
    button_layout->addWidget(wek1_button_, 1, 0);
    button_layout->addWidget(wek4_button_, 1, 1);
    button_layout->addWidget(line_button_, 1, 2);
    button_layout->addWidget(relo_button_, 1, 3);
    button_layout->setContentsMargins(0, 0, 0, 0);

    incremen_label_ = new QPushButton(this);

    hide_left_ = new QPushButton(tr("隐藏细节 >> "), this);

    speed_ = 100;

    spico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/manual_control.png)");
    sp_label_->setText(QString::number(speed_) + " %");
    add_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/add.png)");
    red_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/cut.png)");
    incremen_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/increment_small.png)");
    incremen_label_->setFlat(true);
    speed_widget_->setFrameShape(QFrame::Box);

    this->setFrameShape(QFrame::Box);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);

    ConnectInit();
}

void UnitMaxLower::resizeEvent(QResizeEvent *)
{
    double &scalew = *scale_w_;
    double &scaleh = *scale_h_;

    speed_widget_->resize(139*scalew, 115*scaleh);
    spico_label_->resize(45*scalew, 29*scaleh);
    sp_label_->resize(50*scalew, 20*scaleh);
    add_button_->resize(44*scalew, 44*scaleh);
    red_button_->resize(add_button_->size());
    button_widget_->resize(278*scalew, 135*scaleh);
    incremen_label_->resize(44*scalew, 44*scaleh);
    hide_left_->resize(204*scalew, 40*scaleh);

    speed_widget_->move(scalew, scaleh);
    spico_label_->move(38*scalew, scaleh);
    sp_label_->move(42*scalew, 33*scaleh);
    add_button_->move(9*scalew, 58*scaleh);
    red_button_->move(78*scalew, 55*scaleh);
    button_widget_->move(272*scalew, 3*scaleh);
    incremen_label_->move(16*scalew, 125*scaleh);
    hide_left_->move(347*scalew, 140*scaleh);
}

void UnitMaxLower::ConnectInit()
{
    connect(red_button_, &QPushButton::clicked,
            [=]()
    {
        if(speed_ - 10 < 10)
            return;
        speed_ -= 10;
        sp_label_->setText(QString::number(speed_) + " %");
    });

    connect(add_button_, &QPushButton::clicked,
            [=]()
    {
        if(speed_ >= 100)
            return ;
        speed_ += 10;
        sp_label_->setText(QString::number(speed_) + " %");
    });
}
