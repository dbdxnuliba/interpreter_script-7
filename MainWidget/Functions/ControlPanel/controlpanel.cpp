#include "controlpanel.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDebug>

ControlPanel::ControlPanel(QWidget *parent, int type) : WindowObject(type, parent)
{
    stack_widget_ = new StackedWidObject(this);
    table_box_ = new TableBox(10, 2, main_widget_);

    appearan_ = new Appearance(this);
    monitor_ = new Monitor(this);
    flex_pendant_ = new FlexPendant(this);
    input_out_ = new InputOut(this);
    language_ = new Language(this);
    prog_keys_ = new ProgKeys(this);
    dispose_ = new Dispose(this);
    touch_screen_ = new TouchScreen(this);

    button_widget_->hide();

    table_box_->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("备注"));
    TableInit();

    stack_widget_->SetMainWidget(main_widget_); //设置主页面
    stack_widget_->AddWidget(appearan_, "appearan");
    stack_widget_->AddWidget(monitor_, "monitor");
    stack_widget_->AddWidget(flex_pendant_, "flex_pendant");
    stack_widget_->AddWidget(input_out_, "input_out");
    stack_widget_->AddWidget(language_, "language");
    stack_widget_->AddWidget(prog_keys_, "prog_keys");
    stack_widget_->AddWidget(dispose_, "dispose");
    stack_widget_->AddWidget(touch_screen_, "touch_screen");

    ConnectInit();
}

void ControlPanel::resizeEvent(QResizeEvent *)
{
    if(stack_widget_->GetName() != "main") //当页面不在主页面时
    {
        QWidget *widget = stack_widget_->GetWidget(stack_widget_->GetName());
        widget->resize(this->size());
        widget->move(0, 0);

        stack_widget_->resize(widget->size());
        stack_widget_->move(widget->pos());
        return;
    }
    else
    {
        GetIcoLabel()->resize(33*ScaleW(), 26*ScaleH());
        GetTitleLabel()->resize(this->width() - GetIcoLabel()->width(), 26*ScaleH());
        GetUnderLabel()->resize(this->width(), 1*ScaleH());
        main_widget_->resize(this->width(), this->height() - GetIcoLabel()->height() - GetUnderLabel()->height());

        GetIcoLabel()->move(0, 0);
        GetTitleLabel()->move(GetIcoLabel()->x() + GetIcoLabel()->width(), GetIcoLabel()->y());
        GetUnderLabel()->move(GetIcoLabel()->x(), GetIcoLabel()->y() + GetIcoLabel()->height());
        main_widget_->move(0,27*ScaleH());

        stack_widget_->resize(main_widget_->size());
        stack_widget_->move(0, 27*ScaleH());

        table_box_->resize(main_widget_->size());
        table_box_->move(0, 0);
    }
}

void ControlPanel::ConnectInit()
{
    connect(table_box_, &TableBox::itemClicked, this, &ControlPanel::SelectFunc);
    connect(appearan_->cancel_button_, &QPushButton::clicked, this, &ControlPanel::CancelReturnHome);
    connect(monitor_->cancel_button_, &QPushButton::clicked, this, &ControlPanel::CancelReturnHome);
    connect(flex_pendant_->close_button_, &QPushButton::clicked, this, &ControlPanel::CancelReturnHome);
    connect(input_out_->close_button_, &QPushButton::clicked, this, &ControlPanel::CancelReturnHome);
    connect(language_->cancel_button_, &QPushButton::clicked, this, &ControlPanel::CancelReturnHome);
    connect(prog_keys_->cancel_button_, &QPushButton::clicked, this, &ControlPanel::CancelReturnHome);
    connect(dispose_, &Dispose::Close, this, &ControlPanel::CancelReturnHome);
    connect(touch_screen_->cancel_button_, &QPushButton::clicked, this, &ControlPanel::CancelReturnHome);
}

void ControlPanel::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    table_box_->SetScale(scale_w, scale_h);
    appearan_->SetScale(scale_w, scale_h);
    monitor_->SetScale(scale_w, scale_h);
    flex_pendant_->SetScale(scale_w, scale_h);
    input_out_->SetScale(scale_w, scale_h);
    language_->SetScale(scale_w, scale_h);
    prog_keys_->SetScale(scale_w, scale_h);
    dispose_->SetScale(scale_w, scale_h);
    touch_screen_->SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

void ControlPanel::SelectFunc(QTableWidgetItem *item)
{
    int row = table_box_->row(item);
    switch(row)
    {
    case 0:
    {
        this->HideWidget();
        stack_widget_->Change("appearan");
        resizeEvent(nullptr);
        break;
    }
    case 1:
    {
        this->HideWidget();
        stack_widget_->Change("monitor");
        resizeEvent(nullptr);
        break;
    }
    case 2:
    {
        this->HideWidget();
        stack_widget_->Change("flex_pendant");
        resizeEvent(nullptr);
        break;
    }
    case 3:
    {
        this->HideWidget();
        stack_widget_->Change("input_out");
        resizeEvent(nullptr);
        break;
    }
    case 4:
    {
        this->HideWidget();
        stack_widget_->Change("language");
        resizeEvent(nullptr);
        break;
    }
    case 5:
    {
        this->HideWidget();
        stack_widget_->Change("prog_keys");
        resizeEvent(nullptr);
        break;
    }
    case 8:
    {
        this->HideWidget();
        stack_widget_->Change("dispose");
        resizeEvent(nullptr);
        break;
    }
    case 9:
    {
        this->HideWidget();
        stack_widget_->Change("touch_screen");
        resizeEvent(nullptr);
        break;
    }
    }
}

void ControlPanel::CancelReturnHome()
{
    ShowWidget();
    stack_widget_->Change();
    resizeEvent(nullptr);
}

void ControlPanel::HideWidget()
{
    this->GetIcoLabel()->hide();
    this->GetTitleLabel()->hide();
    this->GetUnderLabel()->hide();
}

void ControlPanel::ShowWidget()
{
    this->GetIcoLabel()->show();
    this->GetTitleLabel()->show();
    this->GetUnderLabel()->show();
}

void ControlPanel::TableInit()
{
    table_box_->clearContents();

    struct TabList
    {
        TabList(QString ti, QString ic, QString in)
        {
            ico = ic;
            title = ti;
            info = in;
        }
        QString ico;
        QString title;
        QString info;
    };

    QList<TabList> table_lists_;

    table_lists_.append(TabList(tr("外观"), ":/image/Image/MainWidget/Appearance.png", tr("自定义显示器")));
    table_lists_.append(TabList(tr("监控"), ":/image/Image/MainWidget/Monitor.png", tr("动作监控和执行设置")));
    table_lists_.append(TabList("FlexPendant", ":/image/Image/MainWidget/FlexPendant.png", tr("配置 FlexPendant 系统")));
    table_lists_.append(TabList("I/O", ":/image/Image/MainWidget/InputOut.png", tr("配置常用 I/O 信号")));
    table_lists_.append(TabList(tr("语言"), ":/image/Image/MainWidget/Language.png", tr("设置当前语言")));
    table_lists_.append(TabList("ProgKeys", ":/image/Image/MainWidget/ProgKeys.png", tr("配置可编程按键")));
    table_lists_.append(TabList(tr("日期和时间"), ":/image/Image/MainWidget/DateTime.png", tr("设置机器人控制器的日期和时间")));
    table_lists_.append(TabList(tr("诊断"), ":/image/Image/MainWidget/Diagnosis.png", tr("系统诊断")));
    table_lists_.append(TabList(tr("配置"), ":/image/Image/MainWidget/Dispose.png", tr("配置系统参数")));
    table_lists_.append(TabList(tr("触摸屏"), ":/image/Image/MainWidget/TouchScreen.png", tr("校准触摸屏")));

    int i = 0;
    for(auto &list : table_lists_)
    {
        table_box_->AddText(i, 0, list.title);
        table_box_->AddText(i, 1, list.info);

        auto *item = table_box_->item(i, 0);
        item->setIcon(QIcon(list.ico));

        if(i == 6 || i == 7)
        {
            item->setFlags(Qt::NoItemFlags);
            table_box_->item(i, 1)->setFlags(Qt::NoItemFlags);
        }

        i++;
    }
}
