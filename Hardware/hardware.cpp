#include "hardware.h"
#include <QMessageBox>
#include <QDebug>

bool HardwareButton::issend_ = false;

Hardware::Hardware(QWidget *parent, int model) : QWidget(parent)
{
    model_ = model;
    fm_ = new QFrame(this);
    if(model_ == 0) //一类示教器，默认
    {
        fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main1.png)");
    }
    else //二类示教器
    {
        fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main2.png)");
    }

    scale_w_ = 1;
    scale_h_ = 1;

    ButtonInit();

#ifdef __arm__ //在arm平台下编译时将最小化和隐藏显示左边窗口按钮隐藏
    minimize_widget_->hide();
    show_widget_->hide();
    hide_widget_->hide();
#endif

    upper_->setAutoRepeat(true); //设置当按钮被一直按下时一直响应按下事件
    lower_->setAutoRepeat(true);
    left_->setAutoRepeat(true);
    right_->setAutoRepeat(true);
    left_upper_->setAutoRepeat(true);
    left_lower_->setAutoRepeat(true);
    right_upper_->setAutoRepeat(true);
    right_lower_->setAutoRepeat(true);
    levo_->setAutoRepeat(true);
    cum_sole_->setAutoRepeat(true);

    elect_mach_ = new ElectMachWindow(this);

    connect(TeachState::instance().data(), &TeachState::Stop, this, &Hardware::StopClicked);
    connect(TeachState::instance().data(), &TeachState::Enabled, this, &Hardware::EnabledClicked);
}

void Hardware::resizeEvent(QResizeEvent *)
{
    fm_->resize(this->size());
    fm_->move(0, 0);

    zoom_widget_->resize(24*scale_w_, 24*scale_h_);
    zoom_widget_->move(95*scale_w_, 28*scale_h_);

    show_widget_->resize(20*scale_w_, 20*scale_h_);
    show_widget_->move(4*scale_w_, 7*scale_h_);

    hide_widget_->resize(20*scale_w_, 20*scale_h_);
    hide_widget_->move(4*scale_w_, 7*scale_h_);

    minimize_widget_->resize(20*scale_w_, 20*scale_h_);
    minimize_widget_->move(26*scale_w_, 6*scale_h_);

    close_widget_->resize(20*scale_w_, 20*scale_h_);
    close_widget_->move(49*scale_w_, 7*scale_h_);

    switch_model_->resize(15*scale_w_, 15*scale_h_);
    switch_model_->move(5*scale_w_, 28*scale_h_);

    emergency_stop_->resize(56*scale_w_, 70*scale_h_);
    emergency_stop_->move(163*scale_w_, 21*scale_h_);

    enabled_->resize(80*scale_w_, 30*scale_h_);
    enabled_->move(110*scale_w_, 148*scale_h_);

    hold_to_run_->resize(80*scale_w_, 30*scale_h_);
    hold_to_run_->move(110*scale_w_, 307*scale_h_);

    upper_->resize(25*scale_w_, 25*scale_h_);
    upper_->move(137*scale_w_, 191*scale_h_);

    lower_->resize(25*scale_w_, 25*scale_h_);
    lower_->move(137*scale_w_, 272*scale_h_);

    left_->resize(25*scale_w_, 25*scale_h_);
    left_->move(99*scale_w_, 233*scale_h_);

    right_->resize(25*scale_w_, 25*scale_h_);
    right_->move(177*scale_w_, 231*scale_h_);

    left_upper_->resize(25*scale_w_, 25*scale_h_);
    left_upper_->move(108*scale_w_, 200*scale_h_);

    left_lower_->resize(25*scale_w_, 25*scale_h_);
    left_lower_->move(108*scale_w_, 262*scale_h_);

    right_upper_->resize(25*scale_w_, 25*scale_h_);
    right_upper_->move(168*scale_w_, 203*scale_h_);

    right_lower_->resize(25*scale_w_, 25*scale_h_);
    right_lower_->move(168*scale_w_, 260*scale_h_);

    levo_->resize(25*scale_w_, 25*scale_h_);
    levo_->move(126*scale_w_, 230*scale_h_);

    cum_sole_->resize(25*scale_w_, 25*scale_h_);
    cum_sole_->move(152*scale_w_, 230*scale_h_);

    custom_key1_1_->resize(35*scale_w_, 35*scale_h_);
    custom_key1_1_->move(9*scale_w_, 84*scale_h_);

    custom_key2_1_->resize(35*scale_w_, 35*scale_h_);
    custom_key2_1_->move(48*scale_w_, 55*scale_h_);

    custom_key3_1_->resize(35*scale_w_, 35*scale_h_);
    custom_key3_1_->move(92*scale_w_, 76*scale_h_);

    custom_key4_1_->resize(35*scale_w_, 35*scale_h_);
    custom_key4_1_->move(52*scale_w_, 105*scale_h_);

    mechanical_unit_->resize(35*scale_w_, 35*scale_h_);
    mechanical_unit_->move(7*scale_w_, 150*scale_h_);

    linear_relocate_->resize(35*scale_w_, 35*scale_h_);
    linear_relocate_->move(mechanical_unit_->x(), mechanical_unit_->y() + 49*scale_h_);

    select_axis_->resize(35*scale_w_, 35*scale_h_);
    select_axis_->move(linear_relocate_->x(), linear_relocate_->y() + 49*scale_h_);

    cut_spike_->resize(35*scale_w_, 35*scale_h_);
    cut_spike_->move(select_axis_->x(), select_axis_->y() + 49*scale_h_);

    custom_key1_2_->resize(35*scale_w_, 35*scale_h_);
    custom_key1_2_->move(9*scale_w_, 52*scale_h_);

    custom_key2_2_->resize(35*scale_w_, 35*scale_h_);
    custom_key2_2_->move(9*scale_w_, 97*scale_h_);

    custom_key3_2_->resize(35*scale_w_, 35*scale_h_);
    custom_key3_2_->move(9*scale_w_, 142*scale_h_);

    custom_key4_2_->resize(35*scale_w_, 35*scale_h_);
    custom_key4_2_->move(54*scale_w_, 97*scale_h_);

    start_execution_1_->resize(35*scale_w_, 35*scale_h_);
    start_execution_1_->move(53*scale_w_, 341*scale_h_);

    stop_execution_1_->resize(35*scale_w_, 35*scale_h_);
    stop_execution_1_->move(48*scale_w_, 391*scale_h_);

    step_left_1_->resize(35*scale_w_, 35*scale_h_);
    step_left_1_->move(9*scale_w_, 362*scale_h_);

    right_left_1_->resize(35*scale_w_, 35*scale_h_);
    right_left_1_->move(92*scale_w_, 370*scale_h_);

    start_execution_2_->resize(35*scale_w_, 35*scale_h_);
    start_execution_2_->move(9*scale_w_, 307*scale_h_);

    step_left_2_->resize(35*scale_w_, 35*scale_h_);
    step_left_2_->move(start_execution_2_->x(), start_execution_2_->y() + 45*scale_h_);

    stop_execution_2_->resize(35*scale_w_, 35*scale_h_);
    stop_execution_2_->move(step_left_2_->x(), step_left_2_->y() + 45*scale_h_);

    right_left_2_->resize(35*scale_w_, 35*scale_h_);
    right_left_2_->move(step_left_2_->x() + 45*scale_w_, step_left_2_->y());

    mode_select_1_->resize(24*scale_w_, 24*scale_h_);
    mode_select_1_->move(49*scale_w_, 231*scale_h_);

    mode_select_2_->resize(24*scale_w_, 24*scale_h_);
    mode_select_2_->move(4*scale_w_, 230*scale_h_);
}

void Hardware::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
    this->resize(220*scale_w_, 484*scale_h_);

    elect_mach_->SetScale(scale_w, scale_h);
}

void Hardware::ShowElectMach()
{
    QPoint p(this->mapToGlobal(QPoint(0, 0)));

    if(elect_mach_->isHidden())
    {
        elect_mach_->resize(127*scale_w_, 167*scale_h_);
        elect_mach_->move(p.x() + 30*scale_w_, p.y() + this->height()/2);
        elect_mach_->show();
    }
    else
    {
        elect_mach_->hide();
    }
}

void Hardware::StopClicked(bool stop)
{
    if(stop)
    {
        if(model_ == 0)
        {
            model_ = 1;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main_stop1.png)");
        }
        else if(model_ == 2)
        {
            model_ = 3;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main_stop2.png)");
        }
    }
    else
    {
        if(model_ == 1)
        {
            model_ = 0;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main1.png)");
        }
        else if(model_ == 3)
        {
            model_ = 2;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main2.png)");
        }
    }
}

void Hardware::EnabledClicked(bool ebl)
{
    if(ebl)
    {
        enabled_->setStyleSheet("border-image: url(:/image/Image/Hardware/enable_do.png)");
    }
    else
    {
        enabled_->setStyleSheet("border-image: url(:/image/Image/Hardware/enable.png)");
    }
}

void Hardware::ButtonInit()
{
    zoom_widget_ = new QPushButton(" ", this);
    zoom_widget_->setStyleSheet("border-image: url(:/image/Image/Hardware/zoom_widget.png)");
    zoom_widget_->setFlat(true);

    show_widget_ = new QPushButton(this);
    show_widget_->setStyleSheet("border-image: url(:/image/Image/Hardware/show_widget.png)"); //加载图片
    show_widget_->setFlat(true); //使按钮透明从而融入背景

    hide_widget_ = new QPushButton(this);
    hide_widget_->setStyleSheet("border-image: url(:/image/Image/Hardware/hide_widget.png)");
    hide_widget_->setFlat(true);

    show_widget_->hide();
    hide_widget_->show();

    connect(show_widget_, &QPushButton::clicked, [=](){show_widget_->hide(); hide_widget_->show();});
    connect(hide_widget_, &QPushButton::clicked, [=](){hide_widget_->hide(); show_widget_->show();});


    minimize_widget_ = new QPushButton(this);
    minimize_widget_->setStyleSheet("border-image: url(:/image/Image/Hardware/minimize_widget.png)");
    minimize_widget_->setFlat(true);


    close_widget_ = new QPushButton(this);
    close_widget_->setStyleSheet("border-image: url(:/image/Image/Hardware/close_widget.png)");
    close_widget_->setFlat(true);


    switch_model_ = new QPushButton(this);
    switch_model_->setStyleSheet("QPushButton#switch_model_{border-image: url(:/image/Image/Hardware/switch_model.png); border: none}");
    switch_model_->setFlat(true);

    connect(switch_model_, &QPushButton::clicked,
            [=]()
    {
        if(model_ == 0) //为一类示教器时需要将界面切换到二类示教器界面同时还要改变各个按钮的属性
        {
            model_ = 2;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main2.png)");

            custom_key1_1_->hide(); //隐藏一类示教器专属按钮
            custom_key2_1_->hide();
            custom_key3_1_->hide();
            custom_key4_1_->hide();
            mechanical_unit_->hide();
            linear_relocate_->hide();
            select_axis_->hide();
            cut_spike_->hide();
            start_execution_1_->hide();
            stop_execution_1_->hide();
            step_left_1_->hide();
            right_left_1_->hide();
            mode_select_1_->hide();

            custom_key1_2_->show(); //显示二类示教器专属按钮
            custom_key2_2_->show();
            custom_key3_2_->show();
            custom_key4_2_->show();
            start_execution_2_->show();
            step_left_2_->show();
            stop_execution_2_->show();
            right_left_2_->show();
            mode_select_2_->show();
        }
        else if(model_ == 1)
        {
            model_ = 3;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main_stop2.png)");

            custom_key1_1_->hide(); //隐藏一类示教器专属按钮
            custom_key2_1_->hide();
            custom_key3_1_->hide();
            custom_key4_1_->hide();
            mechanical_unit_->hide();
            linear_relocate_->hide();
            select_axis_->hide();
            cut_spike_->hide();
            start_execution_1_->hide();
            stop_execution_1_->hide();
            step_left_1_->hide();
            right_left_1_->hide();
            mode_select_1_->hide();

            custom_key1_2_->show(); //显示二类示教器专属按钮
            custom_key2_2_->show();
            custom_key3_2_->show();
            custom_key4_2_->show();
            start_execution_2_->show();
            step_left_2_->show();
            stop_execution_2_->show();
            right_left_2_->show();
            mode_select_2_->show();
        }
        else if(model_ == 2) //为二类示教器时需要将界面切换到一类示教器界面同时还要改变各个按钮的属性
        {
            model_ = 0;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main1.png)");

            custom_key1_2_->hide(); //隐藏二类示教器专属按钮
            custom_key2_2_->hide();
            custom_key3_2_->hide();
            custom_key4_2_->hide();
            start_execution_2_->hide();
            step_left_2_->hide();
            stop_execution_2_->hide();
            right_left_2_->hide();
            mode_select_2_->hide();

            custom_key1_1_->show(); //显示一类示教器专属按钮
            custom_key2_1_->show();
            custom_key3_1_->show();
            custom_key4_1_->show();
            mechanical_unit_->show();
            linear_relocate_->show();
            select_axis_->show();
            cut_spike_->show();
            start_execution_1_->show();
            stop_execution_1_->show();
            step_left_1_->show();
            right_left_1_->show();
            mode_select_1_->show();
        }
        else if(model_ == 3)
        {
            model_ = 1;
            fm_->setStyleSheet("border-image: url(:/image/Image/Hardware/main_stop1.png)");

            custom_key1_2_->hide(); //隐藏二类示教器专属按钮
            custom_key2_2_->hide();
            custom_key3_2_->hide();
            custom_key4_2_->hide();
            start_execution_2_->hide();
            step_left_2_->hide();
            stop_execution_2_->hide();
            right_left_2_->hide();
            mode_select_2_->hide();

            custom_key1_1_->show(); //显示一类示教器专属按钮
            custom_key2_1_->show();
            custom_key3_1_->show();
            custom_key4_1_->show();
            mechanical_unit_->show();
            linear_relocate_->show();
            select_axis_->show();
            cut_spike_->show();
            start_execution_1_->show();
            stop_execution_1_->show();
            step_left_1_->show();
            right_left_1_->show();
            mode_select_1_->show();
        }

        update();
    });


    emergency_stop_ = new QPushButton(this);
    emergency_stop_->setStyleSheet("border: none"); //去除边框
    emergency_stop_->setFlat(true);

    connect(emergency_stop_, &QPushButton::clicked,
            [=]()
    {
        if(TeachState::instance()->GetStop())
            TeachState::instance()->SetStop(false);
        else
            TeachState::instance()->SetStop(true);
    });


    enabled_ = new QPushButton(this);
    enabled_->setStyleSheet("border-image: url(:/image/Image/Hardware/enabled.png)");
    enabled_->setFlat(true);

    connect(enabled_, &QPushButton::clicked,
            [=]()
    {
        if(TeachState::instance()->GetEnabled())
            TeachState::instance()->SetEnabled(false);
        else
            TeachState::instance()->SetEnabled(true);
    });


    hold_to_run_ = new QPushButton(" ", this);
    hold_to_run_->setStyleSheet("border-image: url(:/image/Image/Hardware/hold_to_run.png)");
    hold_to_run_->setFlat(true);

    connect(hold_to_run_, &QPushButton::clicked,
            [=]()
    {
        if(hold_to_run_->text() == " ")
        {
            hold_to_run_->setText("  ");
            hold_to_run_->setStyleSheet("border-image: url(:/image/Image/Hardware/hold_to_run_do.png)");
        }
        else
        {
            hold_to_run_->setText(" ");
            hold_to_run_->setStyleSheet("border-image: url(:/image/Image/Hardware/hold_to_run.png)");
        }
    });


    upper_ = new HardwareButton(this);
    upper_->setStyleSheet("border-image: url(:/image/Image/Hardware/upper.png)");
    upper_->setFlat(true);


    lower_ = new HardwareButton(this);
    lower_->setStyleSheet("border-image: url(:/image/Image/Hardware/lower.png)");
    lower_->setFlat(true);


    left_ = new HardwareButton(this);
    left_->setStyleSheet("border-image: url(:/image/Image/Hardware/left.png)");
    left_->setFlat(true);


    right_ = new HardwareButton(this);
    right_->setStyleSheet("border-image: url(:/image/Image/Hardware/right.png)");
    right_->setFlat(true);


    left_upper_ = new QPushButton(this);
    left_upper_->setStyleSheet("border-image: url(:/image/Image/Hardware/left_upper.png)");
    left_upper_->setFlat(true);


    left_lower_ = new QPushButton(this);
    left_lower_->setStyleSheet("border-image: url(:/image/Image/Hardware/left_lower.png)");
    left_lower_->setFlat(true);


    right_upper_ = new QPushButton(this);
    right_upper_->setStyleSheet("border-image: url(:/image/Image/Hardware/right_upper.png)");
    right_upper_->setFlat(true);


    right_lower_ = new QPushButton(this);
    right_lower_->setStyleSheet("border-image: url(:/image/Image/Hardware/right_lower.png)");
    right_lower_->setFlat(true);


    levo_ = new HardwareButton(this);
    levo_->setStyleSheet("border-image: url(:/image/Image/Hardware/levo.png)");
    levo_->setFlat(true);


    cum_sole_ = new HardwareButton(this);
    cum_sole_->setStyleSheet("border-image: url(:/image/Image/Hardware/cum_sole.png)");
    cum_sole_->setFlat(true);


    custom_key1_1_ = new ProgKeysButton(this);
    custom_key1_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key1_1.png)");
    custom_key1_1_->setFlat(true);

    custom_key2_1_ = new ProgKeysButton(this);
    custom_key2_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key2_1.png)");
    custom_key2_1_->setFlat(true);

    custom_key3_1_ = new ProgKeysButton(this);
    custom_key3_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key3_1.png)");
    custom_key3_1_->setFlat(true);

    custom_key4_1_ = new ProgKeysButton(this);
    custom_key4_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key4_1.png)");
    custom_key4_1_->setFlat(true);

    mechanical_unit_ = new QPushButton(this);
    mechanical_unit_->setStyleSheet("border-image: url(:/image/Image/Hardware/mechanical_unit.png)");
    mechanical_unit_->setFlat(true);

    linear_relocate_ = new QPushButton(this);
    linear_relocate_->setStyleSheet("border-image: url(:/image/Image/Hardware/linear_relocate.png)");
    linear_relocate_->setFlat(true);

    select_axis_ = new QPushButton(this);
    select_axis_->setStyleSheet("border-image: url(:/image/Image/Hardware/select_axis.png)");
    select_axis_->setFlat(true);

    cut_spike_ = new QPushButton(this);
    cut_spike_->setStyleSheet("border-image: url(:/image/Image/Hardware/cut_spike.png)");
    cut_spike_->setFlat(true);

    custom_key1_2_ = new ProgKeysButton(this);
    custom_key1_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key1_2.png)");
    custom_key1_2_->setFlat(true);

    custom_key2_2_ = new ProgKeysButton(this);
    custom_key2_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key2_2.png)");
    custom_key2_2_->setFlat(true);

    custom_key3_2_ = new ProgKeysButton(this);
    custom_key3_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key3_2.png)");
    custom_key3_2_->setFlat(true);

    custom_key4_2_ = new ProgKeysButton(this);
    custom_key4_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/custom_key4_2.png)");
    custom_key4_2_->setFlat(true);

    start_execution_1_ = new QPushButton(this);
    start_execution_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/start_execution1.png)");
    start_execution_1_->setFlat(true);

    stop_execution_1_ = new QPushButton(this);
    stop_execution_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/stop_execution1.png)");
    stop_execution_1_->setFlat(true);

    step_left_1_ = new QPushButton(this);
    step_left_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/step_left1.png)");
    step_left_1_->setFlat(true);

    right_left_1_ = new QPushButton(this);
    right_left_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/right_left1.png)");
    right_left_1_->setFlat(true);

    start_execution_2_ = new QPushButton(this);
    start_execution_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/start_execution2.png)");
    start_execution_2_->setFlat(true);

    step_left_2_ = new QPushButton(this);
    step_left_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/step_left2.png)");
    step_left_2_->setFlat(true);

    stop_execution_2_ = new QPushButton(this);
    stop_execution_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/stop_execution2.png)");
    stop_execution_2_->setFlat(true);

    right_left_2_ = new QPushButton(this);
    right_left_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/right_left2.png)");
    right_left_2_->setFlat(true);

    mode_select_1_ = new QPushButton(this);
    mode_select_1_->setStyleSheet("border-image: url(:/image/Image/Hardware/mode_select1.png)");
    mode_select_1_->setFlat(true);

    mode_select_2_ = new QPushButton(this);
    mode_select_2_->setStyleSheet("border-image: url(:/image/Image/Hardware/mode_select2.png)");
    mode_select_2_->setFlat(true);

    connect(mode_select_1_, &QPushButton::clicked, this, &Hardware::ShowElectMach);

    connect(mode_select_2_, &QPushButton::clicked, this, &Hardware::ShowElectMach);

    if(model_ != 0 && model_ != 1)
    {
        custom_key1_1_->hide();
        custom_key2_1_->hide();
        custom_key3_1_->hide();
        custom_key4_1_->hide();
        mechanical_unit_->hide();
        linear_relocate_->hide();
        select_axis_->hide();
        cut_spike_->hide();
        start_execution_1_->hide();
        stop_execution_1_->hide();
        step_left_1_->hide();
        right_left_1_->hide();
        mode_select_1_->hide();

        custom_key1_2_->show();
        custom_key2_2_->show();
        custom_key3_2_->show();
        custom_key4_2_->show();
        start_execution_2_->show();
        step_left_2_->show();
        stop_execution_2_->show();
        right_left_2_->show();
        mode_select_2_->show();
    }
    else
    {
        custom_key1_2_->hide();
        custom_key2_2_->hide();
        custom_key3_2_->hide();
        custom_key4_2_->hide();
        start_execution_2_->hide();
        step_left_2_->hide();
        stop_execution_2_->hide();
        right_left_2_->hide();
        mode_select_2_->hide();

        custom_key1_1_->show();
        custom_key2_1_->show();
        custom_key3_1_->show();
        custom_key4_1_->show();
        mechanical_unit_->show();
        linear_relocate_->show();
        select_axis_->show();
        cut_spike_->show();
        start_execution_1_->show();
        stop_execution_1_->show();
        step_left_1_->show();
        right_left_1_->show();
        mode_select_1_->show();
    }
}

HardwareButton::HardwareButton(QWidget *parent) : QPushButton(parent)
{
}

void HardwareButton::mousePressEvent(QMouseEvent *button)
{
    if(button->button() == Qt::LeftButton)
    {
        emit Press();
    }
}

void HardwareButton::mouseReleaseEvent(QMouseEvent *button)
{
    if(button->button() == Qt::LeftButton)
    {
        emit Release();
    }
}

ElectMachWindow::ElectMachWindow(QWidget *parent) : QFrame (parent)
{
    auto url = ".ElectMachWindow{border-image: url(:/image/Image/Hardware/electmach_window_auto.png);}";
    this->setStyleSheet(url);

    scale_w_ = scale_h_ = 1;

    button_ = new QPushButton(this);
    auto_radio_ = new QRadioButton(this);
    manual_radio = new QRadioButton(this);
    max_manual_radio_ = new QRadioButton(this);

    auto qss = "QRadioButton::indicator{width:25px;height:25px;border-radius:10px;}";

    auto_radio_->setStyleSheet(qss);
    manual_radio->setStyleSheet(qss);
    max_manual_radio_->setStyleSheet(qss);

    button_->setFlat(true);
    button_->setStyleSheet("border-image: url(:/image/Image/Hardware/electmach_off.png)");

    auto_radio_->setChecked(true);
    old_radio_ = auto_radio_;

    timer_ = new QTimer(this);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    this->setWindowFlag(Qt::Popup);
#else
    this->setWindowFlags(Qt::Popup);
#endif

    ConnectInit();
}

void ElectMachWindow::resizeEvent(QResizeEvent *)
{
    button_->resize(75*scale_w_, 63*scale_h_);
    auto_radio_->resize(13*scale_w_, 13*scale_h_);
    manual_radio->resize(auto_radio_->size());
    max_manual_radio_->resize(manual_radio->size());

    button_->move(31*scale_w_, 6*scale_h_);
    manual_radio->move(55*scale_w_, 92*scale_h_);
    auto_radio_->move(32*scale_w_, 105*scale_h_);
    max_manual_radio_->move(78*scale_w_, 108*scale_h_);
}

void ElectMachWindow::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
    this->resizeEvent(nullptr);
}

void ElectMachWindow::ConnectInit()
{
    connect(timer_, &QTimer::timeout, this, &ElectMachWindow::TimerOut);

    connect(button_, &QPushButton::clicked, this, &ElectMachWindow::ButtonClicked);

    connect(auto_radio_, &QRadioButton::clicked, this, &ElectMachWindow::AutoRadio);

    connect(manual_radio, &QRadioButton::clicked, this, &ElectMachWindow::ManuRadio);

    connect(max_manual_radio_, &QRadioButton::clicked, this, &ElectMachWindow::MaxmRadio);
}

void ElectMachWindow::TimerOut()
{
    auto on = "border-image: url(:/image/Image/Hardware/electmach_on.png)";
    auto off = "border-image: url(:/image/Image/Hardware/electmach_off.png)";
    auto state = TeachState::instance()->GetRunMode();

    if(state == RunMode::AutoMod)
    {
        if(TeachState::instance()->GetElect())
            button_->setStyleSheet(on);
        else
            button_->setStyleSheet(off);
    }
    else
    {
        if(TeachState::instance()->GetElect())
        {
            if(button_->windowTitle() == "on")
            {
                button_->setWindowTitle("off");
                button_->setStyleSheet(off);
            }
            else
            {
                button_->setWindowTitle("on");
                button_->setStyleSheet(on);
            }
        }
        else
        {
            button_->setStyleSheet(off);
        }
    }
}

void ElectMachWindow::ButtonClicked()
{
    if(!TeachState::instance()->GetControlState())
    {
        return;
    }

    if(TeachState::instance()->GetElect())
        TeachState::instance()->SetElect(false);
    else
        TeachState::instance()->SetElect(true);
}

void ElectMachWindow::AutoRadio()
{
    if(!TeachState::instance()->GetControlState())
    {
        old_radio_->setChecked(true);
        TeachState::instance()->SendLog(EventLogType::Error, tr("运行模式切换失败"), tr("无控制权"));
        return;
    }
    else
    {
        auto mes = QMessageBox::question(this, tr("警告"), tr("是否切换到自动模式"));
        if(mes == QMessageBox::No)
        {
            old_radio_->setChecked(true);
            return;
        }
        auto *butt = dynamic_cast<QRadioButton*>(sender());
        if(butt == nullptr || !TeachState::instance()->SetRunMode(RunMode::AutoMod))
        {
            old_radio_->setChecked(true);
            return;
        }
        else
        {
            old_radio_ = butt;
        }
    }

    if(TeachState::instance()->GetRunMode() == RunMode::AutoMod)
    {
        auto url = ".ElectMachWindow{border-image: url(:/image/Image/Hardware/electmach_window_auto.png);}";
        this->setStyleSheet(url);
    }
}

void ElectMachWindow::ManuRadio()
{
    if(!TeachState::instance()->GetControlState())
    {
        old_radio_->setChecked(true);
        TeachState::instance()->SendLog(EventLogType::Error, tr("运行模式切换失败"), tr("无控制权"));
        return;
    }
    else
    {
        auto *butt = dynamic_cast<QRadioButton*>(sender());
        if(butt == nullptr || !TeachState::instance()->SetRunMode(RunMode::ManualMod))
        {
            old_radio_->setChecked(true);
            return;
        }
        else
        {
            old_radio_ = butt;
        }
    }

    if(TeachState::instance()->GetRunMode() == RunMode::ManualMod)
    {
        auto url = ".ElectMachWindow{border-image: url(:/image/Image/Hardware/electmach_window_manual.png);}";
        this->setStyleSheet(url);
    }
}

void ElectMachWindow::MaxmRadio()
{
    if(!TeachState::instance()->GetControlState())
    {
        old_radio_->setChecked(true);
        TeachState::instance()->SendLog(EventLogType::Error, tr("运行模式切换失败"), tr("无控制权"));
        return;
    }
    else
    {
        auto mes = QMessageBox::question(this, tr("警告"), tr("是否切换到全速手动模式"));
        if(mes == QMessageBox::No)
        {
            old_radio_->setChecked(true);
            return;
        }
        auto *butt = dynamic_cast<QRadioButton*>(sender());
        if(butt == nullptr || !TeachState::instance()->SetRunMode(RunMode::MaxManualMod))
        {
            old_radio_->setChecked(true);
            return;
        }
        else
        {
            old_radio_ = butt;
        }
    }

    if(TeachState::instance()->GetRunMode() == RunMode::MaxManualMod)
    {
        auto url = ".ElectMachWindow{border-image: url(:/image/Image/Hardware/electmach_window.maxmanual.png);}";
        this->setStyleSheet(url);
    }
}

void ElectMachWindow::showEvent(QShowEvent *)
{
    timer_->start(1000);
}

void ElectMachWindow::hideEvent(QHideEvent *)
{
    timer_->stop();
}

ProgKeysButton::ProgKeysButton(QWidget *parent) : QPushButton (parent)
{
    this->setWindowTitle("1");
    px_ = -1;
}

void ProgKeysButton::mousePressEvent(QMouseEvent *)
{
    emit Press();
}

void ProgKeysButton::mouseReleaseEvent(QMouseEvent *)
{
    emit Release();
}

void ProgKeysButton::setPORX(int px)
{
    px_ = px;
}

int ProgKeysButton::GetPORX()
{
    return px_;
}
