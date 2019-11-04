#include "upperwidget.h"
#include "teachstate.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSysInfo>
#include <QDebug>

UpperWidget::UpperWidget(QWidget *parent) : QWidget(parent)
{
    menu_button_ = new QPushButton(this);
    task_button_ = new QPushButton(this);
    state_button_ = new StateButton(this);
    close_button_ = new QPushButton(this);

    //设置背景为白色
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    ButtonInit();
}

void UpperWidget::ButtonInit()
{
    menu_button_->resize(90, 45);
    menu_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/menu_button.png)");
    menu_button_->setFlat(true);


    task_button_->resize(50, 45);
    task_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/task_button.png)");
    task_button_->setFlat(true);


    state_button_->resize(449, 45);


    close_button_->resize(50, 45);
    close_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/close_button.png)");
    close_button_->setFlat(true);


    menu_button_->move(0, 0);
    task_button_->move(menu_button_->x() + menu_button_->width(), menu_button_->y());
    state_button_->move(task_button_->x() + task_button_->width(), task_button_->y());
    close_button_->move(state_button_->x() + state_button_->width(), state_button_->y());
}

void UpperWidget::SetScale(double scale_w, double scale_h)
{
    menu_button_->resize(90*scale_w, 45*scale_h);
    task_button_->resize(50*scale_w, 45*scale_h);
    state_button_->resize(449*scale_w, 45*scale_h);
    close_button_->resize(50*scale_w, 45*scale_h);

    menu_button_->move(0, 0);
    task_button_->move(menu_button_->x()+ menu_button_->width(), menu_button_->y());
    state_button_->move(task_button_->x()+ task_button_->width(), task_button_->y());
    close_button_->move(state_button_->x() + state_button_->width(), state_button_->y());
}

StateButton::StateButton(QWidget *parent) : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QVBoxLayout *moti_layout = new QVBoxLayout;
    mod_label_ = new QLabel(this);
    tips_label_ = new QLabel(this);
    moti_layout->addWidget(mod_label_);
    moti_layout->addWidget(tips_label_);
    moti_layout->setSpacing(0);
    moti_layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *stsp_layout = new QVBoxLayout;

    QHBoxLayout *stim_layout = new QHBoxLayout;
    state_label_ = new QLabel(this);
    image_label_ = new QLabel(this);
    stim_layout->addWidget(state_label_);
    stim_layout->addWidget(image_label_);
    stim_layout->setStretchFactor(state_label_, 7);
    stim_layout->setStretchFactor(image_label_, 1);

    QHBoxLayout *rusp_layou = new QHBoxLayout;
    run_state_label_ = new QLabel(this);
    speed_label_ = new QLabel(this);
    rusp_layou->addWidget(run_state_label_);
    rusp_layou->addWidget(speed_label_);
    rusp_layou->setStretchFactor(run_state_label_, 1);
    rusp_layou->setStretchFactor(speed_label_, 4);

    stsp_layout->addLayout(stim_layout);
    stsp_layout->addLayout(rusp_layou);

    layout->addLayout(moti_layout);
    layout->addLayout(stsp_layout);
    layout->setStretchFactor(moti_layout, 1);
    layout->setStretchFactor(stsp_layout, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    Init();

    connect(TeachState::instance().data(), &TeachState::RunMod,
            [=](RunMode mod)
    {
        switch(mod)
        {
        case RunMode::AutoMod:
        {
            mod_label_->setText(tr("自动"));
            break;
        }
        case RunMode::ManualMod:
        {
            mod_label_->setText(tr("手动"));
            break;
        }
        case RunMode::MaxManualMod:
        {
            mod_label_->setText(tr("全速手动"));
            break;
        }
        }
    });
    connect(TeachState::instance().data(), &TeachState::Stop,
            [=](bool isstop)
    {
        if(isstop)
        {
            state_label_->setText(tr("紧急停止"));
        }
        else
        {
            if(TeachState::instance()->GetElect())
                state_label_->setText(tr("防护装置停止"));
            else
                state_label_->setText(tr("等待电机开启"));
        }
    });
    connect(TeachState::instance().data(), &TeachState::Elect,
            [=](bool state)
    {
        if(state)
        {
            if(TeachState::instance()->GetRunMode() == RunMode::AutoMod)
                state_label_->setText(tr("电机开启"));
            else
                state_label_->setText(tr("防护装置停止"));
        }
        else
        {
            state_label_->setText(tr("电机关闭"));
        }
    });
    connect(TeachState::instance().data(), &TeachState::RunState, this, &StateButton::SetRunState);
}

void StateButton::SetMod(QString mod)
{
    mod_label_->setText(mod);
}

void StateButton::SetState(QString state)
{
    state_label_->setText(state);
}

void StateButton::SetRunState(bool state)
{
    if(state)
        run_state_label_->setText(tr("正在运行"));
    else
        run_state_label_->setText(tr("已停止"));
}

void StateButton::SetSpeed(int speed)
{
    speed_label_->setText(tr("(速度 ") + QString::number(speed) + "%)");
}

void StateButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit clicked();
}

void StateButton::Init()
{
    mod_label_->setText(tr("手动"));
    state_label_->setText(tr("防护装置停止"));
    image_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/unit_button.png)");
    tips_label_->setText("IRB_120_...(" + QSysInfo::kernelType() + "-" + QSysInfo::kernelVersion() + ")");
    run_state_label_->setText(tr("已停止"));
    speed_label_->setText(tr("(速度 5%)"));

    this->setFrameShape(QFrame::Box);
    image_label_->setFrameShape(QFrame::Box);

    this->setStyleSheet("background-color: rgb(255, 255, 255)");
}
