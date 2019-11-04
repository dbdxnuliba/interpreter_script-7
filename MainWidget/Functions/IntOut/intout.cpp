#include "intout.h"
#include <QApplication>
#include "RobotClient/robotClient.h"
#include <QJsonDocument>
#include <QHeaderView>
#include <QFile>
#include <QDebug>

using namespace rclib;

IntOut::IntOut(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title_layout = new QHBoxLayout;
    ico_label_ = new QLabel(this);
    title_label_ = new QLabel(tr("输入输出"), this);
    title_layout->addWidget(ico_label_);
    title_layout->addWidget(title_label_);
    title_layout->setContentsMargins(0, 0, 0, 0);
    title_layout->setStretchFactor(ico_label_, 1);
    title_layout->setStretchFactor(title_label_, 15);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    select_label_ = new QLabel(tr("常用"), head_widget_);
    tips_filter_label_ = new QLabel(tr("活动过滤器:"), head_widget_);
    tips_layout_label_ = new QLabel(tr("选择布局"), head_widget_);
    tips_label_ = new QLabel(tr("从列表中选择一个I/O信号"), head_widget_);
    filter_label_ = new QLabel(head_widget_);
    layout_box_ = new QComboBox(head_widget_);
    head_layout->addWidget(select_label_, 0, 0);
    head_layout->addWidget(tips_filter_label_, 0, 1);
    head_layout->addWidget(tips_layout_label_, 0, 2);
    head_layout->addWidget(tips_label_, 1, 0);
    head_layout->addWidget(filter_label_, 1, 1);
    head_layout->addWidget(layout_box_, 1, 2);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    filter_button_ = new ComboBoxObject(button_widget_);
    zero_button_ = new QPushButton("0", button_widget_);
    one_button_ = new QPushButton("1", button_widget_);
    refresh_button_ = new QPushButton(button_widget_);
    view_button_ = new ComboBoxObject(tr("视图"), button_widget_);
    filter_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    zero_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    one_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    refresh_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    filter_button_->setFlat(true);
    zero_button_->setFlat(true);
    one_button_->setFlat(true);
    refresh_button_->setFlat(true);
    view_button_->setFlat(true);
    button_layout->addWidget(filter_button_);
    button_layout->addWidget(zero_button_);
    button_layout->addWidget(one_button_);
    button_layout->addWidget(refresh_button_);
    button_layout->addWidget(view_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addLayout(title_layout);
    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_layout, 1);
    layout->setStretchFactor(head_widget_, 2);
    layout->setStretchFactor(table_box_, 8);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);
    layout_box_->addItem(tr("默认"));
    layout_box_->addItem(tr("标签"));

    ico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/int_out.png)");
    filter_button_->SetFilterButton();
    filter_button_->hide();

    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows); //设置整行选中
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);

    table_box_->horizontalHeader()->show();
    QStringList head_name;
    head_name << tr("名称") << tr("值") << tr("类型") << tr("设备");
    table_box_->SetRomn(0, head_name.size());
    table_box_->setHorizontalHeaderLabels(head_name);
    timer_ = new QTimer(this);
    isupdate_ = false;
    io_type_ = -1;

    QStringList action_lists;
    action_lists << tr("化名I/O") << tr("工业网络") << tr("IO设备") << tr("全部信号") << tr("数字输入") << tr("数字输出") << tr("模拟输入");
    action_lists << tr("模拟输出") << tr("组输入") << tr("组输出") << tr("常用") << tr("安全信号") << tr("仿真信号");

    for(int i = 0; i < IntOut_Number; i++)
    {
        view_actions_[i].setText(action_lists.at(i));
        view_button_->AppAction(&view_actions_[i]);
    }

    for(int i = 0; i < 16; i++)
    {
        QString name1 = "DI" + QString::number(i + 1);
        QString name2 = "DO" + QString::number((i + 1));

        di_list_.append(name1);
        do_list_.append(name2);
    }

    for(int i = 0; i < 4; i++)
    {
        QString name1 = "AI" + QString::number(i + 1);
        QString name2 = "AO" + QString::number((i + 1));

        ai_list_.append(name1);
        ao_list_.append(name2);
    }


    zero_button_->setEnabled(false);
    one_button_->setEnabled(false);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
    ReadIOInfo();
}

void IntOut::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
}

void IntOut::ActionClicked()
{
    auto *action = dynamic_cast<QAction*>(sender());
    if(action == nullptr)
    {
        return;
    }
    if(tips_label_->text() == action->text())
        return;
    table_box_->clear();

    select_label_->setText(action->text());
    QStringList names;

    if(action == &view_actions_[1]) //工业网络
    {
        tips_filter_label_->hide();
        tips_layout_label_->hide();
        tips_label_->hide();
        filter_label_->hide();
        layout_box_->hide();

        names << tr("名称") << tr("地址") << tr("状态");
        table_box_->SetRomn(0, names.size());
        table_box_->setHorizontalHeaderLabels(names);

        auto json = root_object_.value("Industrial Network").toObject();
        auto keys = json.keys();

        for(int i = 0; i < keys.size(); i++)
        {
            auto &key = keys[i];
            if(key == "operate")
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "-");
            table_box_->AddText(i, 2, "-");
        }
    }
    else if(action == &view_actions_[2]) //IO设备
    {
        tips_filter_label_->hide();
        tips_layout_label_->hide();
        filter_label_->hide();
        layout_box_->hide();
        tips_label_->show();
        tips_label_->setText(tr("从列表中选择一个I/O设备。"));

        names << tr("名称") << tr("网络") << tr("地址") << tr("状态");
        table_box_->SetRomn(0, names.size());
        table_box_->setHorizontalHeaderLabels(names);
    }
    else
    {
        tips_filter_label_->show();
        tips_layout_label_->show();
        tips_label_->show();
        filter_label_->show();
        layout_box_->show();
        tips_label_->setText(tr("从列表中选择一个I/O信号"));

        names << tr("名称") << tr("值") << tr("类型") << tr("设备");
        table_box_->SetRomn(0, names.size());
        table_box_->setHorizontalHeaderLabels(names);
    }
    zero_button_->setEnabled(false);
    one_button_->setEnabled(false);
    isupdate_ = false;
    io_type_ = -1;
    if(timer_->isActive())
        timer_->stop();

    if(action == &view_actions_[11]) //安全信号
    {
        auto json = root_object_.value("Signal").toObject();
        auto keys = json.keys();
        keys.removeAll("operate");

        QJsonObject curr_json;
        int i = 0;
        for(auto &key : keys)
        {
            curr_json = json.value(key).toObject();

            if(curr_json.contains("signal_type㎏")) //过滤掉自定义信号
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "0");

            auto type = curr_json.value("1_Type of Signal").toString();
            if(type == "Digital Input")
                table_box_->AddText(i, 2, "DI");
            else
                table_box_->AddText(i, 2, "DO");

            table_box_->AddText(i, 3, curr_json.value("2_Assigned to Device").toString());
            i++;
        }
    }
    else if(action == &view_actions_[4]) //数字输入
    {
        for(int i = 0; i < 16; i++)
        {
            auto &name = di_list_[i];
            PORTINDEX px = PORTINDEX(i);
            SWITCHSTATE state = RobotClient::initance()->getDigitalInput(px);

            table_box_->AddText(i, 0, name);
            table_box_->AddText(i, 1, QString::number(state));
            table_box_->AddText(i, 2, "DI");
        }

        auto json = root_object_.value("Signal").toObject();
        auto keys = json.keys();
        keys.removeAll("operate");

        QJsonObject curr_json;
        int i = 16;
        for(auto &key : keys)
        {
            curr_json = json.value(key).toObject();

            if(!curr_json.contains("signal_type㎏")) //过滤掉内置信号
                continue;

            auto type = curr_json.value("signal_type㎏").toString();
            if(type != "DI")
                continue;
            if(di_list_.contains(key))
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "0");
            table_box_->AddText(i, 2, type);
            i++;
        }
        isupdate_ = true;
        io_type_ = 4;
        timer_->start(200);
    }
    else if(action == &view_actions_[5]) //数字输出
    {
        for(int i = 0; i < 16; i++)
        {
            auto &name = do_list_[i];
            PORTINDEX px = PORTINDEX(i);
            SWITCHSTATE state = RobotClient::initance()->getDigitalOutput(px);

            table_box_->AddText(i, 0, name);
            table_box_->AddText(i, 1, QString::number(state));
            table_box_->AddText(i, 2, "DO");
        }

        auto json = root_object_.value("Signal").toObject();
        auto keys = json.keys();
        keys.removeAll("operate");

        QJsonObject curr_json;
        int i = 16;
        for(auto &key : keys)
        {
            curr_json = json.value(key).toObject();

            if(!curr_json.contains("signal_type㎏")) //过滤掉内置信号
                continue;

            auto type = curr_json.value("signal_type㎏").toString();
            if(type != "DO")
                continue;
            if(do_list_.contains(key))
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "0");
            table_box_->AddText(i, 2, type);
            i++;
        }

        zero_button_->setEnabled(true);
        one_button_->setEnabled(true);
        isupdate_ = true;
        io_type_ = 5;
        timer_->start(200);
    }
    else if(action == &view_actions_[6]) //模拟输入
    {
        for(int i = 0; i < 4; i++)
        {
            auto &name = ai_list_[i];
            PORTINDEX px = PORTINDEX(i);
            double state = RobotClient::initance()->getAnalogInput(px);

            table_box_->AddText(i, 0, name);
            table_box_->AddText(i, 1, QString::number(state));
            table_box_->AddText(i, 2, "AI");
        }

        auto json = root_object_.value("Signal").toObject();
        auto keys = json.keys();
        keys.removeAll("operate");

        QJsonObject curr_json;
        int i = 4;
        for(auto &key : keys)
        {
            curr_json = json.value(key).toObject();

            if(!curr_json.contains("signal_type㎏")) //过滤掉内置信号
                continue;

            auto type = curr_json.value("signal_type㎏").toString();
            if(type != "AI")
                continue;
            if(ai_list_.contains(key))
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "0");
            table_box_->AddText(i, 2, type);
            i++;
        }

        isupdate_ = true;
        io_type_ = 6;
        timer_->start(200);
    }
    else if(action == &view_actions_[7]) //模拟输出
    {
        for(int i = 0; i < 4; i++)
        {
            auto &name = ao_list_[i];
            PORTINDEX px = PORTINDEX(i);
            double state = RobotClient::initance()->getAnalogOutput(px);

            table_box_->AddText(i, 0, name);
            table_box_->AddText(i, 1, QString::number(state));
            table_box_->AddText(i, 2, "AO");
        }

        auto json = root_object_.value("Signal").toObject();
        auto keys = json.keys();
        keys.removeAll("operate");

        QJsonObject curr_json;
        int i = 0;
        for(auto &key : keys)
        {
            curr_json = json.value(key).toObject();

            if(!curr_json.contains("signal_type㎏")) //过滤掉内置信号
                continue;

            auto type = curr_json.value("signal_type㎏").toString();
            if(type != "AO")
                continue;
            if(ao_list_.contains(key))
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "0");
            table_box_->AddText(i, 2, type);
            i++;
        }

        isupdate_ = true;
        io_type_ = 7;
        timer_->start(200);
    }
    else if(action == &view_actions_[8]) //组输入
    {
        auto json = root_object_.value("Signal").toObject();
        auto keys = json.keys();
        keys.removeAll("operate");

        QJsonObject curr_json;
        int i = 0;
        for(auto &key : keys)
        {
            curr_json = json.value(key).toObject();

            if(!curr_json.contains("signal_type㎏")) //过滤掉内置信号
                continue;

            auto type = curr_json.value("signal_type㎏").toString();
            if(type != "GI")
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "0");
            table_box_->AddText(i, 2, type);
            i++;
        }
    }
    else if(action == &view_actions_[9]) //组输出
    {
        auto json = root_object_.value("Signal").toObject();
        auto keys = json.keys();
        keys.removeAll("operate");

        QJsonObject curr_json;
        int i = 0;
        for(auto &key : keys)
        {
            curr_json = json.value(key).toObject();

            if(!curr_json.contains("signal_type㎏")) //过滤掉内置信号
                continue;

            auto type = curr_json.value("signal_type㎏").toString();
            if(type != "GO")
                continue;

            table_box_->AddText(i, 0, key);
            table_box_->AddText(i, 1, "0");
            table_box_->AddText(i, 2, type);
            i++;
        }
    }
}

void IntOut::IOTimerUpdate()
{
    if(!isupdate_ || io_type_ == -1)
        return;

    switch(io_type_)
    {
    case 4: //数字输入
    {
        for(int i = 0; i < di_list_.size(); i++)
        {
            PORTINDEX px = PORTINDEX(i);
            SWITCHSTATE state = RobotClient::initance()->getDigitalInput(px);

            auto *item = table_box_->item(i, 1);
            if(item != nullptr)
                item->setText(QString::number(state));
        }

        return;
    }
    case 5: //数字输出
    {
        for(int i = 0; i < do_list_.size(); i++)
        {
            PORTINDEX px = PORTINDEX(i);
            SWITCHSTATE state = RobotClient::initance()->getDigitalOutput(px);

            auto *item = table_box_->item(i, 1);
            if(item != nullptr)
                item->setText(QString::number(state));
        }

        return;
    }
    case 6: //模拟输入
    {
        for(int i = 0; i < ai_list_.size(); i++)
        {
            PORTINDEX px = PORTINDEX(i);
            double state = RobotClient::initance()->getAnalogInput(px);

            auto *item = table_box_->item(i, 1);
            if(item != nullptr)
                item->setText(QString::number(state));
        }

        return;
    }
    case 7: //模拟输出
    {
        for(int i = 0; i < ao_list_.size(); i++)
        {
            PORTINDEX px = PORTINDEX(i);
            double state = RobotClient::initance()->getAnalogOutput(px);

            auto *item = table_box_->item(i, 1);
            if(item != nullptr)
                item->setText(QString::number(state));
        }

        return;
    }
    default:
    {
        return;
    }
    }
}

void IntOut::OFFClicked()
{
    auto *item = table_box_->currentItem();
    if(item == nullptr)
        return;
    int row = table_box_->row(item);
    auto io_item = table_box_->item(row, 0);
    auto io_name = io_item->text();

    if(do_list_.contains(io_name))
    {
        PORTINDEX px = PORTINDEX(row);

        RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHOFF);
    }
}

void IntOut::ONClicked()
{
    auto *item = table_box_->currentItem();
    if(item == nullptr)
        return;
    int row = table_box_->row(item);
    auto io_item = table_box_->item(row, 0);
    auto io_name = io_item->text();

    if(do_list_.contains(io_name))
    {
        PORTINDEX px = PORTINDEX(row);

        RobotClient::initance()->setDigitalOutput(px, SWITCHSTATE::SWITCHON);
    }
}

void IntOut::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void IntOut::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void IntOut::ConnectInit()
{
    for(int i = 0; i < IntOut_Number; i++)
    {
        connect(&view_actions_[i], &QAction::triggered, this, &IntOut::ActionClicked);
    }

    connect(zero_button_, &QPushButton::clicked, this, &IntOut::OFFClicked);

    connect(one_button_, &QPushButton::clicked, this, &IntOut::ONClicked);

    connect(timer_, &QTimer::timeout, this, &IntOut::IOTimerUpdate);
}

void IntOut::ReadIOInfo()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/IO/io.json";
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto data = file.readAll();

    auto json = QJsonDocument::fromJson(data);
    root_object_ = json.object();
}
