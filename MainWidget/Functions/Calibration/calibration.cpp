#include "calibration.h"
#include "RobotClient/robotClient.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QHeaderView>
#include <QPainter>
#include <QDebug>

using namespace rclib;

Calibration::Calibration(QWidget *parent, int type) : WindowObject(type, parent)
{
    tips1_label_ = new QLabel(tr("为使用系统，所有机械单元必须校准"), main_widget_);
    tips2_label_ = new QLabel(tr("选择需要校准的机械单元。"), main_widget_);
    table_box_ = new TableBox(1, 2, main_widget_);

    calibr_window_ = new CalibrWindow(this);
    calibr_window_->hide();

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(1, 2);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("机械单元") << tr("状态"));

    table_box_->AddText(0, 0, "ROB_1");
    table_box_->AddText(0, 1, tr("校准"));

    connect(table_box_, &TableBox::itemClicked, this, &Calibration::ItemClicked);

    connect(calibr_window_, &CalibrWindow::Close, calibr_window_, &CalibrWindow::hide);
}

void Calibration::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips1_label_->resize(main_widget_->width(), 67*ScaleH());
    tips2_label_->resize(main_widget_->width(), 33*ScaleH());
    table_box_->resize(main_widget_->width(), main_widget_->height() - tips1_label_->height() - tips2_label_->height());

    tips1_label_->move(0, 0);
    tips2_label_->move(tips1_label_->x(), tips1_label_->y() + tips1_label_->height());
    table_box_->move(tips2_label_->x(), tips2_label_->y() + tips2_label_->height());

    if(!calibr_window_->isHidden())
    {
        calibr_window_->resize(this->size());
        calibr_window_->move(0, 0);
    }
}

void Calibration::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);

    table_box_->SetScale(scale_w, scale_h);

    if(!calibr_window_->GetUpdateCouWindow()->isHidden())
    {
        auto updateCounter_window = calibr_window_->GetUpdateCouWindow();

        updateCounter_window->resize(this->size());
        updateCounter_window->move(0, 0);
    }
}

void Calibration::ShowCalibrWindow()
{
    calibr_window_->resize(this->size());
    calibr_window_->move(0, 0);
    calibr_window_->show();
}

void Calibration::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    this->ShowCalibrWindow();
}

CalibrWindow::CalibrWindow(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *main_layout = new QHBoxLayout;

    QVBoxLayout *button_layout = new QVBoxLayout;
    counter_button_ = new ButtonWidget(":/image/Image/MainWidget/counter_button.png", tr("转数计数器"), this);
    calibration_button_ = new ButtonWidget(":/image/Image/MainWidget/calibration_button.png", tr("校准 参数"), this);
    smb_button_ = new ButtonWidget(":/image/Image/MainWidget/SMB_button.png", tr("SMB 内存"), this);
    pedestal_button_ = new ButtonWidget(":/image/Image/MainWidget/linear_mod.png", tr("基座"), this);
    button_layout->addWidget(counter_button_);
    button_layout->addWidget(calibration_button_);
    button_layout->addWidget(smb_button_);
    button_layout->addWidget(pedestal_button_);

    stack_widget_ = new QStackedWidget(this);

    counter_window_ = new QWidget(this);
    QVBoxLayout *cou_layout = new QVBoxLayout(counter_window_);
    update_counter_button_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("更新转数计数器"), counter_window_);
    update_counter_button_->setFlat(true);
    cou_layout->addWidget(update_counter_button_);
    cou_layout->addStretch(1);

    calibration_window_ = new QWidget(this);
    QVBoxLayout *cal_layout = new QVBoxLayout(calibration_window_);
    cal_button1_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("加载点击校准"), calibration_window_);
    cal_button2_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("编辑点击校准偏移"), calibration_window_);
    cal_button3_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("微校"), calibration_window_);
    cal_button1_->setFlat(true);
    cal_button2_->setFlat(true);
    cal_button3_->setFlat(true);
    cal_layout->addWidget(cal_button1_);
    cal_layout->addWidget(cal_button2_);
    cal_layout->addWidget(cal_button3_);
    cal_layout->addStretch(1);

    smb_window_ = new QWidget(this);
    QVBoxLayout *smb_layout = new QVBoxLayout(smb_window_);
    smb_button1_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("显示状态"), smb_window_);
    smb_button2_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("更新"), smb_window_);
    smb_button3_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("高级"), smb_window_);
    smb_button1_->setFlat(true);
    smb_button2_->setFlat(true);
    smb_button3_->setFlat(true);
    smb_layout->addWidget(smb_button1_);
    smb_layout->addWidget(smb_button2_);
    smb_layout->addWidget(smb_button3_);
    smb_layout->addStretch(1);

    pedestal_window_ = new QWidget(this);
    QVBoxLayout *ped_layout = new QVBoxLayout(pedestal_window_);
    ped_button1_ = new QPushButton(QIcon(":/image/Image/MainWidget/calibr_window_button_ico.png"), tr("4 点 xz"), pedestal_window_);
    ped_button1_->setFlat(true);
    ped_layout->addWidget(ped_button1_);
    ped_layout->addStretch(1);

    main_layout->addLayout(button_layout);
    main_layout->addWidget(stack_widget_);
    main_layout->setSpacing(0);
    main_layout->setStretchFactor(button_layout, 1);
    main_layout->setStretchFactor(stack_widget_, 3);

    button_widget_ = new QWidget(this);
    QHBoxLayout *bt_layout = new QHBoxLayout(button_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setFlat(true);
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addWidget(close_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addLayout(main_layout);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(main_layout, 6);
    layout->setStretchFactor(button_widget_, 1);

    stack_widget_->addWidget(counter_window_);
    stack_widget_->addWidget(calibration_window_);
    stack_widget_->addWidget(smb_window_);
    stack_widget_->addWidget(pedestal_window_);

    counter_button_->SetIsClick(true);
    stack_widget_->setCurrentIndex(0);

    updateCounter_window_ = new UpdateCounterWindow(this);

    updateCounter_window_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

UpdateCounterWindow *CalibrWindow::GetUpdateCouWindow()
{
    return updateCounter_window_;
}

void CalibrWindow::ConnectInit()
{
    connect(counter_button_, &ButtonWidget::Clicked,
            [=]()
    {
        calibration_button_->SetIsClick(false);
        smb_button_->SetIsClick(false);
        pedestal_button_->SetIsClick(false);
        stack_widget_->setCurrentIndex(0);
    });

    connect(calibration_button_, &ButtonWidget::Clicked,
            [=]()
    {
        counter_button_->SetIsClick(false);
        smb_button_->SetIsClick(false);
        pedestal_button_->SetIsClick(false);
        stack_widget_->setCurrentIndex(1);
    });

    connect(smb_button_, &ButtonWidget::Clicked,
            [=]()
    {
        counter_button_->SetIsClick(false);
        calibration_button_->SetIsClick(false);
        pedestal_button_->SetIsClick(false);
        stack_widget_->setCurrentIndex(2);
    });

    connect(pedestal_button_, &ButtonWidget::Clicked,
            [=]()
    {
        counter_button_->SetIsClick(false);
        calibration_button_->SetIsClick(false);
        smb_button_->SetIsClick(false);
        stack_widget_->setCurrentIndex(3);
    });

    connect(close_button_, &QPushButton::clicked,
            [=]()
    {
        emit Close();
    });

    connect(updateCounter_window_, &UpdateCounterWindow::Close,
            [=]()
    {
        updateCounter_window_->hide();
    });

    connect(update_counter_button_, &QPushButton::clicked,
            [=]()
    {
        updateCounter_window_->Init();
        updateCounter_window_->resize(this->size());
        updateCounter_window_->move(0, 0);
        updateCounter_window_->show();
    });
}

ButtonWidget::ButtonWidget(QString ico_path, QString text, QWidget *parent) : QWidget(parent)
{
    this->Init(ico_path, text);
}

ButtonWidget::ButtonWidget(QWidget *parent) : QWidget(parent)
{
    this->Init();
}

void ButtonWidget::SetIsClick(bool isclick)
{
    isclick_ = isclick;

    this->update();
}

void ButtonWidget::Init(QString ico_path, QString text)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    ico_label_ = new QLabel(this);
    text_label_ = new QLabel(this);
    text_label_->setAlignment(Qt::AlignCenter);

    QHBoxLayout *ico_layout = new QHBoxLayout;
    QLabel *tips_label = new QLabel(this);
    QLabel *tips2_label = new QLabel(this);
    ico_layout->addWidget(tips_label);
    ico_layout->addWidget(ico_label_);
    ico_layout->addWidget(tips2_label);

    layout->addLayout(ico_layout);
    layout->addWidget(text_label_);
    layout->setSpacing(0);
    layout->setStretchFactor(ico_layout, 3);
    layout->setStretchFactor(text_label_, 1);
    this->setLayout(layout);

    isclick_ = false;
    this->update();

    if(!ico_path.isEmpty())
    {
        QString url = "border-image: url(" + ico_path + ")";
        ico_label_->setStyleSheet(url);
    }
    if(!text.isEmpty())
    {
        text_label_->setText(text);
    }

    ConnectInit();
}

void ButtonWidget::ConnectInit()
{
    //
}

void ButtonWidget::paintEvent(QPaintEvent *)
{
    QPainter painter;

    painter.begin(this);
    painter.setPen(QPen(Qt::black, 4, Qt::SolidLine));

    if(!isclick_)
    {
        painter.drawRect(this->rect());
    }
    else
    {
        QVector<QLineF> lines;

        QLineF line1(0, 0, this->width(), 0);
        QLineF line2(0, 0, 0, this->height());
        QLineF line3(0, this->height(), this->width(), this->height());

        lines << line1 << line2 << line3;

        painter.drawLines(lines);
    }

    painter.end();
}

void ButtonWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        this->SetIsClick(true);

        emit Clicked();
    }
}

UpdateCounterWindow::UpdateCounterWindow(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QVBoxLayout *label_layout = new QVBoxLayout;
    tips_label_ = new QLabel(tr("更新转数计数器"), this);

    QHBoxLayout *tips_layout = new QHBoxLayout;
    tips1_label_ = new QLabel(tr("机械单元"), this);
    tips2_label_ = new QLabel(tr("ROB_1"), this);
    tips_layout->addWidget(tips1_label_);
    tips_layout->addWidget(tips2_label_);

    tips3_label_ = new QLabel(tr("要更新转数计数器,选择轴并点击更新"), this);

    label_layout->addWidget(tips_label_);
    label_layout->addLayout(tips_layout);
    label_layout->addWidget(tips3_label_);

    list_widget_ = new UpdateListWidget(this);

    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    select_all_button_ = new QPushButton(tr("全选"), button_widget_);
    clear_all_button_ = new QPushButton(tr("全部清除"), button_widget_);
    update_button_ = new QPushButton(tr("更新"), button_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);
    select_all_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    clear_all_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    update_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    select_all_button_->setFlat(true);
    clear_all_button_->setFlat(true);
    update_button_->setFlat(true);
    close_button_->setFlat(true);
    button_layout->addWidget(select_all_button_);
    button_layout->addWidget(clear_all_button_);
    button_layout->addStretch();
    button_layout->addWidget(update_button_);
    button_layout->addWidget(close_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addLayout(label_layout);
    layout->addWidget(list_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(label_layout, 2);
    layout->setStretchFactor(list_widget_, 4);
    layout->setStretchFactor(button_widget_, 1);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void UpdateCounterWindow::Init()
{
    list_widget_->ClearAll();
}

void UpdateCounterWindow::ConnectInit()
{
    connect(select_all_button_, &QPushButton::clicked, list_widget_, &UpdateListWidget::SelectAll);

    connect(clear_all_button_, &QPushButton::clicked, list_widget_, &UpdateListWidget::ClearAll);

    connect(close_button_, &QPushButton::clicked,
            [=]()
    {
        emit Close();
    });

    connect(update_button_, &QPushButton::clicked,
            [=]()
    {
        auto x = QMessageBox::question(this, tr("提示"), tr("确定要更新转数计数器吗？此操作不可撤销"));
        if(x == QMessageBox::No)
            return;

        bool isselect = false;

        for(int i = 0; i < list_widget_->list_.size(); i++)
        {
            if(list_widget_->list_.at(i)->GetSelect())
            {
                isselect = true;

                JOINTINDEX joint = JOINTINDEX(i);

                RobotClient::initance()->setJointZero(joint);
            }
        }
        if(!isselect)
        {
            QMessageBox::warning(this, tr("提示"), tr("请选择轴以后再点击更新"));
            return ;
        }
    });
}

UpdateListWidget::UpdateListWidget(QWidget *parent) : QWidget (parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    scroll_area_ = new QScrollArea(this);

    scroll_widget_ = new QWidget(scroll_area_);
    QVBoxLayout *scroll_layout = new QVBoxLayout(scroll_widget_);

    QHBoxLayout *label_layout = new QHBoxLayout;
    tips1_label_ = new QLabel(tr("轴"), scroll_widget_);
    tips2_label_ = new QLabel(tr("状态"), scroll_widget_);
    tips1_label_->setFrameShape(QFrame::Box);
    tips2_label_->setFrameShape(QFrame::Box);
    label_layout->addWidget(tips1_label_);
    label_layout->addWidget(tips2_label_);
    label_layout->setContentsMargins(0, 0, 0, 0);
    label_layout->setSpacing(0);

    scroll_layout->addLayout(label_layout);

    QStringList rob;
    rob << "rob1_1" << "rob1_2" << "rob1_3" << "rob1_4" << "rob1_5" << "rob1_6";

    for(auto name : rob)
    {
        auto button = new UpdateList(name, scroll_widget_);
        list_.append(button);

        scroll_layout->addWidget(button);
    }
    scroll_layout->setContentsMargins(0, 0, 0, 0);

    scroll_area_->setWidget(scroll_widget_);

    QHBoxLayout *area_layout = new QHBoxLayout(scroll_area_);
    area_layout->addWidget(scroll_widget_);
    area_layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(scroll_area_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    scroll_widget_->setAutoFillBackground(true);
    this->setPalette(p);
    scroll_widget_->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    tips1_label_->setAutoFillBackground(true);
    tips2_label_->setAutoFillBackground(true);
    tips1_label_->setPalette(p);
    tips2_label_->setPalette(p);
}

void UpdateListWidget::SelectAll()
{
    for(auto list : list_)
    {
        list->SetSelect(true);
    }
}

void UpdateListWidget::ClearAll()
{
    for(auto list : list_)
    {
        list->SetSelect(false);
    }
}

UpdateList::UpdateList(QString check_text, QWidget *parnet) : QWidget (parnet)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    check_box_ = new QCheckBox(check_text, this);
    state_label_ = new QLabel(tr("转数计数器已更新"), this);

    layout->addWidget(check_box_);
    layout->addWidget(state_label_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
}

void UpdateList::SetSelect(bool isselect)
{
    if(isselect)
        check_box_->setCheckState(Qt::Checked);
    else
        check_box_->setCheckState(Qt::Unchecked);
}

void UpdateList::SetText(QString text)
{
    state_label_->setText(text);
}

bool UpdateList::GetSelect()
{
    if(check_box_->checkState() == Qt::Checked)
        return true;
    else
        return false;
}

QString UpdateList::GetText()
{
    return state_label_->text();
}

void UpdateList::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(GetSelect())
            SetSelect(false);
        else
            SetSelect(true);
    }
}
