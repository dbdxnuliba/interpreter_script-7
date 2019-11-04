#include "programdata.h"
#include "teachstate.h"
#include "MainWidget/Functions/ProgramEditor/programeditordata.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDebug>

ProgramData::ProgramData(QWidget *parent, int type) : WindowObject(type, parent)
{
    QVBoxLayout *main_layout = new QVBoxLayout(main_widget_);
    tips_label_ = new QLabel(tr("从列表中选择一个数据类型。"), main_widget_);

    QHBoxLayout *range_layout = new QHBoxLayout;
    range_label_ = new QLabel(tr("范围:"), main_widget_);
    range_button_ = new QPushButton(tr("更改范围"), main_widget_);
    range_text_ = range_label_->text();
    range_label_->setText(range_text_ + "RAPID");
    range_layout->addWidget(range_label_);
    range_layout->addWidget(range_button_);
    range_layout->setStretchFactor(range_label_, 4);
    range_layout->setStretchFactor(range_button_, 1);

    data_type_box_ = new TableBox(this);

    main_layout->addWidget(tips_label_);
    main_layout->addLayout(range_layout);
    main_layout->addWidget(data_type_box_);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    main_layout->setStretchFactor(tips_label_, 2);
    main_layout->setStretchFactor(range_layout, 1);
    main_layout->setStretchFactor(data_type_box_, 10);
    main_widget_->setLayout(main_layout);

    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    show_data_button_ = new QPushButton(tr("显示数据"), button_widget_);
    view_button_ = new ComboBoxObject(tr("视图"), button_widget_);
    show_data_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(show_data_button_);
    button_layout->addWidget(view_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_widget_->setLayout(button_layout);

    range_label_->setFrameShape(QFrame::Box);
    show_data_button_->setFlat(true);
    view_button_->setFlat(true);

    isshowall_ = false;

    modify_ranges_ = new ModifyRanges(this);
    modify_ranges_->hide();

    SetDataTypeBox(DataType::instance()->GetShowTypeName());

    show_main_widget_ = new QWidget(this);
    QHBoxLayout *show_layout = new QHBoxLayout(show_main_widget_);
    show_varinfo_ = new ShowVarInfo(show_main_widget_);
    show_layout->addWidget(show_varinfo_);
    show_edit_ = new DataEditWindow(show_main_widget_);
    show_layout->addWidget(show_edit_);
    show_layout->setContentsMargins(0, 0, 0, 0);
    show_layout->setSpacing(0);

    show_varinfo_->hide();
    show_main_widget_->hide();
    show_edit_->hide();
    show_data_button_->hide();

    show_varinfo_->SetModi(modify_ranges_);

    all_an_ = new QAction("  " + tr("全部数据类型"));
    used_an_ = new QAction("√" + tr("已用数据类型"));

    view_button_->AppAction(all_an_);
    view_button_->AppAction(used_an_);

    connect(data_type_box_, &TableBox::itemClicked, this, &ProgramData::ItemClicked);
    connect(show_varinfo_->return_button_, &QPushButton::clicked, this, &ProgramData::ShowMainWidget);
    connect(show_varinfo_, &ShowVarInfo::VarButtonClicked, this, &ProgramData::VarButtonClicked);
    connect(show_edit_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        show_edit_->hide();
        show_varinfo_->show();
    });
    connect(all_an_, &QAction::triggered,
            [=]()
    {
        all_an_->setText("√" + tr("全部数据类型"));
        used_an_->setText("  " + tr("已用数据类型"));

        isshowall_ = true;

        this->SetDataTypeBox((DataType::instance()->GetTypeName()));
    });
    connect(used_an_, &QAction::triggered,
            [=]()
    {
        all_an_->setText("  " + tr("全部数据类型"));
        used_an_->setText("√" + tr("已用数据类型"));

        isshowall_ = false;

        this->SetDataTypeBox((DataType::instance()->GetShowTypeName()));
    });
    connect(range_button_, &QPushButton::clicked,
            [=]()
    {
        if(modify_ranges_->isHidden())
        {
            modify_ranges_->resize(426*ScaleW(), 283*ScaleH());
            modify_ranges_->move(220*ScaleW(), 100*ScaleH());
            modify_ranges_->show();

            range_button_->setText(tr("确定"));

            if(modify_ranges_->GetMod().isEmpty())
                modify_ranges_->Refresh();
        }
        else
        {
            modify_ranges_->hide();

            range_button_->setText(tr("更改范围"));
        }
    });
    connect(modify_ranges_, &ModifyRanges::BuiltIn,
            [=]()
    {
        range_label_->setText(range_text_ + modify_ranges_->GetRange());

        if(isshowall_)
            return;

        type_ = 0;

        this->SetDataTypeBox((DataType::instance()->GetShowTypeName()));
    });
    connect(modify_ranges_, &ModifyRanges::CurrTask,
            [=]()
    {
        range_label_->setText(range_text_ + modify_ranges_->GetRange());

        if(isshowall_)
            return;

        QStringList list;
        QStringList all_list = DataType::instance()->GetTypeName();

        for(auto data_type : all_list)
        {
            auto var_list = VarInfo::instance()->GetTypeVar(data_type);

            bool x = false;

            for(auto var : var_list)
            {
                if(var.second.genre)
                {
                    x = true;
                    break;
                }
            }

            if(x)
                list.append(data_type);
        }

        type_ = 1;

        this->SetDataTypeBox(list);
    });
    connect(modify_ranges_, &ModifyRanges::Mod,
            [=](QString mod_name)
    {
        range_label_->setText(range_text_ + modify_ranges_->GetRange());

        if(isshowall_)
            return;

        QStringList list;
        QStringList all_list = DataType::instance()->GetTypeName();

        for(auto data_type : all_list)
        {
            auto var_list = VarInfo::instance()->GetTypeVar(data_type);

            bool x = false;
            for(auto var : var_list)
            {
                if(var.second.genre && var.second.mod == mod_name)
                {
                    x = true;
                    break;
                }
            }

            if(x)
                list.append(data_type);
        }

        type_ = 2;
        this->SetDataTypeBox(list);
    });
    connect(modify_ranges_, &ModifyRanges::Rout,
            [=](QString rout_name)
    {
        range_label_->setText(range_text_ + modify_ranges_->GetRange());

        if(isshowall_)
            return;

        QStringList list;
        QStringList all_list = DataType::instance()->GetTypeName();

        for(auto data_type : all_list)
        {
            auto var_list = VarInfo::instance()->GetTypeVar(data_type);

            bool x = false;
            for(auto var : var_list)
            {
                if(var.second.genre && var.second.action_scope == rout_name)
                {
                    x = true;
                    break;
                }
            }

            if(x)
                list.append(data_type);
        }

        type_ = 3;
        this->SetDataTypeBox(list);
    });
}

void ProgramData::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    if(!show_main_widget_->isHidden())
    {
        if(!main_widget_->isHidden())
        {
            main_widget_->hide();
        }
        show_main_widget_->resize(main_widget_->width(), main_widget_->height() + button_widget_->height());
        show_main_widget_->move(0, this->GetUnderLabel()->y() + this->GetUnderLabel()->height());
    }
}

void ProgramData::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    data_type_box_->SetScale(scale_w, scale_h);
    show_varinfo_->SetScale(scale_w, scale_h);

    this->resizeEvent(nullptr);

    if(!show_varinfo_->isHidden())
    {
        show_varinfo_->Refresh();
        show_varinfo_->filter_key_->resize(394*scale_w, 275*scale_h);
        show_varinfo_->filter_key_->move(246*scale_w, 22*scale_h);
        show_varinfo_->filter_key_->SetScale(scale_w, scale_h);
    }
    show_varinfo_->GetCreateWidget()->SetScale(scale_w, scale_h);

    if(!modify_ranges_->isHidden())
    {
        modify_ranges_->resize(426*scale_w, 283*scale_h);
        modify_ranges_->move(220*scale_w, 100*scale_h);
    }
}

void ProgramData::ButtonClicked(QString name)
{
    HideWidget();

    show_varinfo_->show();

    VarShowType type;
    QString str;

    switch (type_)
    {
    case 0: //内置
    {
        type = VarShowType::Genre_Sys;
        break;
    }
    case 1: //当前与任务
    {
        type = VarShowType::Genre_User;
        break;
    }
    case 2: //模块
    {
        type = VarShowType::Mod;
        str = modify_ranges_->GetMod();
        break;
    }
    case 3: //例行程序
    {
        type = VarShowType::Rout;
        str = modify_ranges_->GetRout();
        break;
    }
    default:
    {
        type = VarShowType::Genre_Sys;
        break;
    }
    }

    show_varinfo_->Init(name, type, str);
}

void ProgramData::HideWidget()
{
    main_widget_->hide();
    button_widget_->hide();

    if(show_main_widget_->isHidden())
    {
        show_main_widget_->show();

        show_main_widget_->resize(main_widget_->width(), main_widget_->height() + button_widget_->height());
        show_main_widget_->move(0, this->GetUnderLabel()->y() + this->GetUnderLabel()->height());
    }
    if(modify_ranges_->isHidden())
    {
        modify_ranges_->hide();
    }
}

void ProgramData::ShowMainWidget()
{
    show_main_widget_->hide();

    if(!show_varinfo_->isHidden())
        show_varinfo_->hide();

    if(!show_edit_->isHidden())
        show_edit_->hide();

    if(main_widget_->isHidden())
    {
        main_widget_->show();
        button_widget_->show();
    }
}

void ProgramData::VarButtonClicked(QString name, QString var, QString type)
{
    show_varinfo_->hide();
    show_edit_->show();
    show_edit_->Init(name, var, type);
}

void ProgramData::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    this->ButtonClicked(item->text());
}

void ProgramData::SetDataTypeBox(QStringList list)
{
    data_type_box_->clear();
    data_type_box_->SetRomn(0, 0);

    int r = 0, c = 0;
    for(auto &type : list)
    {
        data_type_box_->AddText(r, c++, type);
        if(c > 2)
        {
            r++;
            c = 0;
        }
    }
}

ShowVarInfo::ShowVarInfo(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *tips_layout = new QHBoxLayout;
    tips_data_label_ = new QLabel(tr("选择想要编辑的数据。"), this);
    tips_filter_label_ = new QLabel(tr("活动过滤器:"), this);
    tips_filter_label_->setAlignment(Qt::AlignTop);
    tips_layout->addWidget(tips_data_label_);
    tips_layout->addWidget(tips_filter_label_);

    tips_range_label_ = new QLabel(tr("范围:RAPID"), this);
    tips_range_label_->setFrameShape(QFrame::Box);

    table_box_ = new TableBox(1, 4, this);

    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    filter_button_ = new ComboBoxObject(button_widget_);
    create_button_ = new QPushButton(tr("新建"), button_widget_);
    edit_button_ = new ComboBoxObject(tr("编辑"), button_widget_);
    refresh_button_ = new QPushButton(tr("刷新"), button_widget_);
    return_button_ = new QPushButton(tr("查看数据类型"), button_widget_);
    filter_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    create_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    refresh_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    filter_button_->setFlat(true);
    create_button_->setFlat(true);
    edit_button_->setFlat(true);
    refresh_button_->setFlat(true);
    return_button_->setFlat(true);
    filter_button_->SetFilterButton();
    button_layout->addWidget(filter_button_);
    button_layout->addWidget(create_button_);
    button_layout->addWidget(edit_button_);
    button_layout->addWidget(refresh_button_);
    button_layout->addWidget(return_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    button_widget_->setLayout(button_layout);

    layout->addLayout(tips_layout);
    layout->addWidget(tips_range_label_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStretchFactor(tips_layout, 2);
    layout->setStretchFactor(tips_range_label_, 1);
    layout->setStretchFactor(table_box_, 9);
    layout->setStretchFactor(button_widget_, 2);
    this->setLayout(layout);

    delete_act_ = new QAction(tr("删除"), this);
    state_act_ = new QAction(tr("更改声明"), this);
    val_act_ = new QAction(tr("更改值"), this);
    copy_act_ = new QAction(tr("复制"), this);
    define_act_ = new QAction(tr("定义"), this);
    delete_act_->setEnabled(false);
    state_act_->setEnabled(false);
    val_act_->setEnabled(false);
    copy_act_->setEnabled(false);
    define_act_->setEnabled(false);
    edit_button_->AppAction(delete_act_);
    edit_button_->AppAction(state_act_);
    edit_button_->AppAction(val_act_);
    edit_button_->AppAction(copy_act_);
    edit_button_->AppSeparator();
    edit_button_->AppAction(define_act_);

    filter_key_ = new FilterKey(this);
    filter_key_->resize(394, 275);
    filter_key_->move(246, 29);
    filter_key_->hide();
    filter_key_->setFrameShape(QFrame::Box);

    create_new_var_ = new CreateNewVar(this);
    create_new_var_->hide();

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList head_name;
    head_name << tr("名称") << tr("值") << tr("任务") << "";
    table_box_->setHorizontalHeaderLabels(head_name);

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    filter_key_->setAutoFillBackground(true);
    filter_key_->setPalette(p);
    create_new_var_->setAutoFillBackground(true);
    create_new_var_->setPalette(p);

    connect(table_box_, &TableBox::itemDoubleClicked, this, &ShowVarInfo::ItemDoubleClicked);
    connect(create_button_, &QPushButton::clicked, this, &ShowVarInfo::CreateVar);
    connect(refresh_button_, &QPushButton::clicked, this, &ShowVarInfo::Refresh);
    connect(filter_key_, &FilterKey::GetData, this, &ShowVarInfo::Filter);
    connect(create_new_var_, &CreateNewVar::CreateVar, [=](){create_new_var_->hide();});
    connect(create_new_var_->cancel_button_, &QPushButton::clicked, [=](){create_new_var_->hide();});
    connect(filter_key_, &FilterKey::ShowWindow, filter_button_, &ComboBoxObject::LowerArrow);
    connect(filter_key_, &FilterKey::HideWindow, filter_button_, &ComboBoxObject::UpperArrow);
    connect(filter_key_->reset_button_, &QPushButton::clicked,
            [=]()
    {
        tips_filter_label_->setText(tr("活动过滤器:"));
        filter_name_.clear();
        this->Refresh();
    });
    connect(filter_button_, &ComboBoxObject::clicked,
            [=]()
    {
        filter_key_->resize(394*scale_w_, 275*scale_h_);
        filter_key_->move(246*scale_w_, 29*scale_h_);

        if(filter_key_->isHidden())
            filter_key_->show();
        else
            filter_key_->hide();
    });
}

void ShowVarInfo::resizeEvent(QResizeEvent *)
{
    if(!create_new_var_->isHidden())
    {
        create_new_var_->resize(this->size());
        create_new_var_->move(0, 0);
    }
}

void ShowVarInfo::SetModi(ModifyRanges *modi)
{
    modi_ = modi;
}

CreateNewVar *ShowVarInfo::GetCreateWidget()
{
    return create_new_var_;
}

void ShowVarInfo::Init(QString &name, VarShowType vartype, QString str)
{
    table_box_->clearContents();
    table_box_->SetRomn(0, 4);

    type_ = vartype;
    str_ = str;

    if(name != var_name_)
    {
        if(!list_.isEmpty())
            list_.clear();
        list_ = VarInfo::instance()->GetTypeVar(name);
        var_name_ = name;
    }

    int row = 0;
    for(auto &temp : list_)
    {
        QString name, val, type, task, action_scope;

        if(vartype == VarShowType::Genre_Sys) //内置
        {
            if(temp.second.genre) //不显示用户定义变量
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            task = tr("所有");
            action_scope = tr("本地");
        }
        else if(vartype == VarShowType::Genre_User) //当前与任务
        {
            if(!temp.second.genre)
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            task = modi_->GetTask();
        }
        else if(vartype == VarShowType::Mod) //模块
        {
            if(!temp.second.genre || temp.second.mod != str)
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            task = modi_->GetTask();
            action_scope = str;
        }
        else if(vartype == VarShowType::Rout) //例行程序
        {
            if(!temp.second.genre || temp.second.action_scope != str)
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            action_scope = str;
        }
        else
        {
            return;
        }

        table_box_->AddText(row, 0, name);
        table_box_->AddText(row, 1, val);
        table_box_->AddText(row, 2, task);
        table_box_->AddText(row++, 3, action_scope);
    }
}

void ShowVarInfo::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    table_box_->SetScale(scale_w, scale_h);

    if(!filter_key_->isHidden())
    {
        filter_key_->resize(394*scale_w_, 275*scale_h_);
        filter_key_->move(246*scale_w_, 29*scale_h_);
    }
    filter_key_->SetScale(scale_w, scale_h);
}

void ShowVarInfo::Refresh()
{
    if(!filter_key_->isHidden())
    {
        this->Filter(filter_name_);
    }
    else
    {
        this->Init(var_name_, type_, str_);
    }
}

void ShowVarInfo::ItemDoubleClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;
    int row = table_box_->row(item);

    auto *item1 = table_box_->item(row, 0);
    auto *item2 = table_box_->item(row, 1);

    this->Clicked(item1->text(), item2->text(), var_name_);
}

void ShowVarInfo::Clicked(QString name, QString var, QString type)
{
    emit VarButtonClicked(name, var, type);
}

void ShowVarInfo::Filter(QString text)
{
    tips_filter_label_->setText(tr("活动过滤器:") + text);

    filter_name_ = text;
    table_box_->clearContents();

    for(int i = 0; i < list_.size(); i++)
    {
        auto &temp = list_[i];
        QString name, val, type, task, action_scope;

        if(type_ == VarShowType::Genre_Sys) //内置
        {
            if(temp.second.genre) //不显示用户定义变量
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            task = tr("所有");
            action_scope = tr("本地");
        }
        else if(type_ == VarShowType::Genre_User) //当前与任务
        {
            if(!temp.second.genre)
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            task = modi_->GetTask();
        }
        else if(type_ == VarShowType::Mod) //模块
        {
            if(!temp.second.genre || temp.second.mod != str_)
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            task = modi_->GetTask();
            action_scope = str_;
        }
        else if(type_ == VarShowType::Rout) //例行程序
        {
            if(!temp.second.genre || temp.second.action_scope != str_)
                continue;

            name = temp.first;
            val = temp.second.var.toString();
            type = temp.second.data_type;
            action_scope = str_;
        }
        else
        {
            return;
        }
        table_box_->AddText(i, 0, name);
        table_box_->AddText(i, 1, val);
        table_box_->AddText(i, 2, task);
        table_box_->AddText(i, 3, action_scope);
    }
}

void ShowVarInfo::CreateVar()
{
    if(type_ == VarShowType::Genre_Sys)
        return;
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

    create_new_var_->resize(this->size());
    create_new_var_->move(0, 0);
    create_new_var_->Init(var_name_, modi_->GetMod());
    create_new_var_->show();
}

DataEditWindow::DataEditWindow(QWidget *parent) : VarEditWindow(parent)
{
    QVBoxLayout *frame_label = new QVBoxLayout(in_widget_);

    QHBoxLayout *fa_layout = new QHBoxLayout;
    frame_name_label_ = new QLabel(tr("名称"), in_widget_);
    frame_var_label_ = new QLabel(tr("值"), in_widget_);
    frame_type_label_ = new QLabel(tr("数据类型"), in_widget_);
    frame_name_label_->setFrameShape(QFrame::Box);
    frame_var_label_->setFrameShape(QFrame::Box);
    frame_type_label_->setFrameShape(QFrame::Box);
    fa_layout->addWidget(frame_name_label_);
    fa_layout->addWidget(frame_var_label_);
    fa_layout->addWidget(frame_type_label_);

    QHBoxLayout *data_layout = new QHBoxLayout;
    data_name_ = new QLabel(in_widget_);
    data_var_ = new QLabel(in_widget_);
    data_type_ = new QLabel(in_widget_);
    data_layout->addWidget(data_name_);
    data_layout->addWidget(data_var_);
    data_layout->addWidget(data_type_);

    frame_label->addLayout(fa_layout);
    frame_label->addLayout(data_layout);
    frame_label->addStretch();
    frame_label->setContentsMargins(0, 0, 0, 0);
    frame_label->setSpacing(0);

    in_widget_->setLayout(frame_label);
    in_widget_->setFrameShape(QFrame::Box);

    QHBoxLayout *button_layout = new QHBoxLayout(lower_widget_);
    revoke_button_ = new QPushButton(tr("撤销"), lower_widget_);
    determine_button_ = new QPushButton(tr("确定"), lower_widget_);
    cancel_button_ = new QPushButton(tr("取消"), lower_widget_);
    revoke_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    determine_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    revoke_button_->setFlat(true);
    determine_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(revoke_button_);
    button_layout->addWidget(determine_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    lower_widget_->setLayout(button_layout);

    revoke_button_->hide();
    determine_button_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    frame_name_label_->setAutoFillBackground(true);
    frame_name_label_->setPalette(p);
    frame_var_label_->setAutoFillBackground(true);
    frame_var_label_->setPalette(p);
    frame_type_label_->setAutoFillBackground(true);
    frame_type_label_->setPalette(p);
}

void DataEditWindow::Init(QString name, QString val, QString type)
{
    SetNameLabel(name);

    name_ = name;
    var_ = val;
    type_ = type;

    if(name.size() > 15)
        name = name.mid(0, 14) + "...";
    if(val.size() > 20)
        val = val.mid(0, 20) + "...";
    if(type.size() > 15)
        type = type.mid(0, 14) + "...";

    data_name_->setText(name);
    data_var_->setText(val);
    data_type_->setText(type);
}

ModifyRanges::ModifyRanges(QWidget *parent) : QFrame(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tips_label_ = new QLabel("RAPID");

    QGridLayout *button_layout = new QGridLayout;

    built_in_ = new QRadioButton(tr("仅限内置数据:"), this);
    current_ = new QRadioButton(tr("当前执行:"), this);
    task_ = new QRadioButton(tr("任务:"), this);
    task_box_ = new QComboBox(this);
    mod_ = new QRadioButton(tr("模块:"), this);
    mod_box_ = new QComboBox(this);
    rout_ = new QRadioButton(tr("例行程序:"), this);
    rout_box_ = new QComboBox(this);

    button_layout->addWidget(built_in_, 0, 0);
    button_layout->addWidget(current_, 1, 0);
    button_layout->addWidget(task_, 2, 0);
    button_layout->addWidget(task_box_, 2, 1);
    button_layout->addWidget(mod_, 3, 0);
    button_layout->addWidget(mod_box_, 3, 1);
    button_layout->addWidget(rout_, 4, 0);
    button_layout->addWidget(rout_box_, 4, 1);

    tips2_label_ = new QLabel(tr("请注意仅当使用例行程序范围时才会列出例行程序中声明的数据"), this);

    layout->addWidget(tips_label_);
    layout->addLayout(button_layout);
    layout->addWidget(tips2_label_);

    this->setLayout(layout);
    this->setFrameShape(QFrame::Box);

    task_box_->addItem("T_ROB1");
    built_in_->setChecked(true);
    task_box_->setEnabled(false);
    mod_box_->setEnabled(false);
    rout_box_->setEnabled(false);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);

    connect(built_in_, &QRadioButton::clicked,
            [=]()
    {
        tips_label_->setText("RAPID");

        emit BuiltIn();

        task_box_->setEnabled(false);
        mod_box_->setEnabled(false);
        rout_box_->setEnabled(false);
    });
    connect(current_, &QRadioButton::clicked,
            [=]()
    {
        tips_label_->setText("RAPID/" + task_box_->currentText());

        emit CurrTask();

        task_box_->setEnabled(false);
        mod_box_->setEnabled(false);
        rout_box_->setEnabled(false);
    });
    connect(task_, &QRadioButton::clicked,
            [=]()
    {
        tips_label_->setText("RAPID/" + task_box_->currentText());

        emit CurrTask();

        task_box_->setEnabled(true);

        mod_box_->setEnabled(false);
        rout_box_->setEnabled(false);
    });
    connect(mod_, &QRadioButton::clicked,
            [=]()
    {
        QString str = "RAPID/" + task_box_->currentText() + "/" + mod_box_->currentText();
        tips_label_->setText(str);

        emit Mod(mod_box_->currentText());

        task_box_->setEnabled(false);

        mod_box_->setEnabled(true);

        rout_box_->setEnabled(false);
    });
    connect(rout_, &QRadioButton::clicked,
            [=]()
    {
        QString str = "RAPID/" + task_box_->currentText() + "/" + mod_box_->currentText() + "/" + rout_box_->currentText();
        tips_label_->setText(str);

        emit Rout(rout_box_->currentText());

        task_box_->setEnabled(false);
        mod_box_->setEnabled(false);

        rout_box_->setEnabled(true);
    });
    connect(mod_box_, &QComboBox::currentTextChanged,
            [=](QString text)
    {
        QString str = "RAPID/" + task_box_->currentText() + "/" + mod_box_->currentText();
        tips_label_->setText(str);

        emit Mod(text);

        rout_box_->clear();

        QStringList rou_list = EditData::instance()->GetRoutName(text);

        if(rou_list.isEmpty())
        {
            rout_->setEnabled(false);
            rout_box_->setEnabled(false);
        }
        else
        {
            if(!rout_->isEnabled())
            {
                rout_->setEnabled(true);
            }
        }

        rout_box_->addItems(rou_list);
    });
    connect(rout_box_, &QComboBox::currentTextChanged,
            [=](QString text)
    {
        if(!rout_box_->isEnabled())
        {
            return;
        }

        QString str = "RAPID/" + task_box_->currentText() + "/" + mod_box_->currentText() + "/" + rout_box_->currentText();
        tips_label_->setText(str);

        emit Rout(text);
    });
}

void ModifyRanges::Refresh()
{
    mod_box_->clear();

    QStringList mod_list = EditData::instance()->GetModName();

    mod_box_->addItems(mod_list);

    rout_box_->clear();

    QStringList rou_list = EditData::instance()->GetRoutName(mod_box_->currentText());

    rout_box_->addItems(rou_list);
}

QString ModifyRanges::GetTask()
{
    return task_box_->currentText();
}

QString ModifyRanges::GetMod()
{
    return mod_box_->currentText();
}

QString ModifyRanges::GetRout()
{
    return rout_box_->currentText();
}

QString ModifyRanges::GetRange()
{
    return tips_label_->text();
}
