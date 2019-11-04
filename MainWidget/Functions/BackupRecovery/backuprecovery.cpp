#include "backuprecovery.h"
#include<QDebug>

BackupRecovery::BackupRecovery(QWidget *parent, int type) : WindowObject(type, parent)
{
    this->button_widget_->hide();

    stack_widget_ = new StackedWidObject(this);
    back_reco_widget_ = new BackRecoWidget(this);
    back_widget_ = new BackWidget(this);
    dir_widget_ = new FolderWidget(this);
    reco_widget_ = new RecoveryWidget(this);
    keyboard_ = new Keyboard(this);

    stack_widget_->SetMainWidget(back_reco_widget_);
    stack_widget_->AddWidget(back_widget_, "back_widget");
    stack_widget_->AddWidget(dir_widget_, "dir_widget");
    stack_widget_->AddWidget(reco_widget_, "reco_widget");
    stack_widget_->AddWidget(keyboard_, "keyboard");

    ConnectInit();
}

void BackupRecovery::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    QString name = stack_widget_->GetName(); //获取当前页面的名称
    if(name == "keyboard")
    {
        stack_widget_->resize(this->width(), this->height() - this->GetIcoLabel()->height() - this->GetUnderLabel()->height());
        stack_widget_->move(main_widget_->pos());
        return;
    }
    if(name != "main")
    {
        main_widget_->resize(this->size());
        main_widget_->move(0, 0);

        stack_widget_->resize(main_widget_->size());
        stack_widget_->move(main_widget_->pos());
        return;
    }
    else
    {
        stack_widget_->resize(this->width(), this->height() - this->GetIcoLabel()->height() - this->GetUnderLabel()->height());
        stack_widget_->move(main_widget_->pos());
    }


}

void BackupRecovery::ConnectInit()
{
    connect(back_reco_widget_->back_button_, &QPushButton::clicked,
            [=]()
    {
        stack_widget_->Change("back_widget");

        this->GetIcoLabel()->hide();
        this->GetTitleLabel()->hide();
        this->GetUnderLabel()->hide();

        resizeEvent(nullptr);
    });

    connect(back_reco_widget_->recovery_button_, &QPushButton::clicked,
            [=]()
    {
        stack_widget_->Change("reco_widget");

        this->GetIcoLabel()->hide();
        this->GetTitleLabel()->hide();
        this->GetUnderLabel()->hide();

        resizeEvent(nullptr);
    });

    connect(back_widget_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        stack_widget_->Change();

        this->GetIcoLabel()->show();
        this->GetTitleLabel()->show();
        this->GetUnderLabel()->show();
    });

    connect(reco_widget_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        stack_widget_->Change();

        this->GetIcoLabel()->show();
        this->GetTitleLabel()->show();
        this->GetUnderLabel()->show();
    });

    connect(back_widget_->select_button_, &QPushButton::clicked,
            [=]()
    {
        select_ = true;
        CutDirWidget();
    });

    connect(reco_widget_->select_button_, &QPushButton::clicked,
            [=]()
    {
        select_ = false;
        CutDirWidget();
    });

    connect(dir_widget_->yes_button_, &QPushButton::clicked,
            [=]()
    {
        if(select_)
        {
            back_widget_->route_edit_->setText(dir_widget_->path_edit_->text());
            QString path = back_widget_->route_edit_->toPlainText();
            if(path != "/")
                path += "/";
            if(!back_widget_->folder_edit_->text().isEmpty())
                path += back_widget_->folder_edit_->text() + "/";
            back_widget_->path_edit_->setText(path);

            stack_widget_->Change("back_widget");
        }
        else
        {
            reco_widget_->path_edit_->setText(dir_widget_->path_edit_->text());
            stack_widget_->Change("reco_widget");
        }
    });

    connect(dir_widget_->no_button_, &QPushButton::clicked,
            [=]()
    {
        if(select_) //从备份进来的
        {
            stack_widget_->Change("back_widget");
        }
        else //从恢复进来的
        {
            stack_widget_->Change("reco_widget");
        }
    });

    connect(back_widget_->clavier_button_, &QPushButton::clicked,
            [=]()
    {
        keyboard_->Init();

        stack_widget_->Change("keyboard");

        this->GetIcoLabel()->show();
        this->GetTitleLabel()->show();
        this->GetUnderLabel()->show();
        this->GetTitleLabel()->setText(tr("备份到当前系统"));

        resizeEvent(nullptr);
    });

    connect(keyboard_, &Keyboard::GetData,
            [=](QString str)
    {
        back_widget_->folder_edit_->setText(str);

        QString path;
        if(!back_widget_->route_edit_->toPlainText().isEmpty())
        {
            path = back_widget_->route_edit_->toPlainText();
            if(path != "/")
                path += "/";
            path += str + "/";
            back_widget_->path_edit_->setText(path);
        }

        stack_widget_->Change("back_widget");

        this->GetIcoLabel()->hide();
        this->GetTitleLabel()->hide();
        this->GetUnderLabel()->hide();
        this->GetTitleLabel()->setText(tr("备份与恢复"));

        resizeEvent(nullptr);
    });

    connect(keyboard_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        stack_widget_->Change("back_widget");

        this->GetIcoLabel()->hide();
        this->GetTitleLabel()->hide();
        this->GetUnderLabel()->hide();
        this->GetTitleLabel()->setText(tr("备份与恢复"));

        resizeEvent(nullptr);
    });
}

void BackupRecovery::CutDirWidget()
{
    dir_widget_->ReturnHome(); //使文件选择框恢复到初始状态
    stack_widget_->Change("dir_widget");

    this->GetIcoLabel()->hide();
    this->GetTitleLabel()->hide();
    this->GetUnderLabel()->hide();
}

void BackupRecovery::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    back_reco_widget_->SetScale(scale_w, scale_h);
    back_widget_->SetScale(scale_w, scale_h);
    dir_widget_->SetScale(scale_w, scale_h);
    reco_widget_->SetScale(scale_w, scale_h);
    keyboard_->SetScale(scale_w, scale_h);
    resizeEvent(nullptr);
}

BackRecoWidget::BackRecoWidget(QWidget *parent) : QWidget(parent)
{
    back_button_ = new QPushButton(this);
    recovery_button_ = new QPushButton(this);
    back_label_ = new QLabel(tr("备份到当前系统..."), this);
    recover_label_ = new QLabel(tr("恢复系统..."), this);

    scale_w_ = 1;
    scale_h_ = 1;

    back_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/back_button.png)");
    recovery_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/recovery_button.png)");
}

void BackRecoWidget::resizeEvent(QResizeEvent *)
{
    back_button_->resize(62*scale_w_, 55*scale_h_);
    recovery_button_->resize(back_button_->size());
    back_label_->resize(back_button_->width() + 30*scale_w_, 20*scale_h_);
    recover_label_->resize(back_label_->size());

    back_button_->move(164*scale_w_, 148*scale_h_);
    recovery_button_->move(back_button_->x() + back_button_->width() + 188*scale_w_, back_button_->y());
    back_label_->move(back_button_->x(), back_button_->y() + back_button_->height());
    recover_label_->move(recovery_button_->x(), recovery_button_->y() + recovery_button_->height());
}

void BackRecoWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

BackWidget::BackWidget(QWidget *parent, int type) : WindowObject(type, parent)
{
    GetTitleLabel()->setText((tr("备份当前系统"))); //修改标题

    tips_label_ = new QLabel(tr("所有模块和系统参数均将存储与备份文件夹中。\n选择其他文件夹或接受默认文件夹。然后按一下“备份”。"), main_widget_);
    folder_label_ = new QLabel(tr("备份文件夹:"), main_widget_);
    route_label_ = new QLabel(tr("备份路径:"), main_widget_);
    path_label_ = new QLabel(tr("备份将被创建在:"), main_widget_);
    folder_edit_ = new QLineEdit(main_widget_);main_widget_->resize(this->size());
    main_widget_->move(0, 0);
    route_edit_ = new QTextEdit(main_widget_);
    path_edit_ = new QTextEdit(main_widget_);
    clavier_button_ = new QPushButton("ABC...", main_widget_);
    select_button_ = new QPushButton("...", main_widget_);
    back_button_ = new QPushButton(tr("备份"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);

    back_button_->setFlat(true);
    cancel_button_->setFlat(true);
    path_edit_->setEnabled(false);
}

void BackWidget::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips_label_->resize(449*ScaleW(), 53*ScaleH());
    folder_label_->resize(150*ScaleW(), 22*ScaleH());
    folder_edit_->resize(448*ScaleW(), 40*ScaleH());
    route_label_->resize(folder_label_->size());
    route_edit_->resize(folder_edit_->size());
    path_label_->resize(route_label_->size());
    path_edit_->resize(576*ScaleW(), 40*ScaleH());
    clavier_button_->resize(108*ScaleW(), 40*ScaleH());
    select_button_->resize(clavier_button_->size());
    back_button_->resize(128*ScaleW(), button_widget_->height());
    cancel_button_->resize(back_button_->size());

    tips_label_->move(15*ScaleW(), 0);
    folder_edit_->move(42*ScaleW(), 100*ScaleH());
    folder_label_->move(folder_edit_->x(), folder_edit_->y() - folder_label_->height());
    route_label_->move(folder_edit_->x(), folder_edit_->y() + folder_edit_->height());
    route_edit_->move(route_label_->x(), route_label_->y() + route_label_->height());
    path_label_->move(route_edit_->x(), route_edit_->y() + route_edit_->height());
    path_edit_->move(path_label_->x(), path_label_->y() + path_label_->height());
    clavier_button_->move(folder_edit_->x() + folder_edit_->width() + 5*ScaleW(), folder_edit_->y());
    select_button_->move(route_edit_->x() + route_edit_->width() + 5*ScaleW(), route_edit_->y());
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    back_button_->move(cancel_button_->x() - cancel_button_->width(), cancel_button_->y());
}

void BackWidget::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

FolderWidget::FolderWidget(QWidget *parent) : FileMangerObject(parent)
{
    tips_label_ = new QLabel(tr("选择文件夹"), this);
    decorate_label_ = new QLabel(this);
    tips2_label_ = new QLabel(tr("选定的文件夹"), this);
    path_edit_ = new QLineEdit(this);
    win_button_ = new QWidget(this);
    yes_button_ = new QPushButton(tr("确定"), win_button_);
    no_button_ = new QPushButton(tr("取消"), win_button_);

    decorate_label_->setFrameShape(QFrame::Box);
    path_edit_->setText(path_);

    yes_button_->setFlat(true);
    no_button_->setFlat(true);

    create_button_->setParent(win_button_); //修改这三个按钮的父窗口，使其可以正常显示
    upper_button_->setParent(win_button_);
    home_button_->setParent(win_button_);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(192, 192, 192));
    win_button_->setAutoFillBackground(true);
    win_button_->setPalette(palette);

    connect(filelist_, &FileList::currentRowChanged, this, &FolderWidget::TogglePagesi);
    connect(this, &FolderWidget::PathChange, [=](QString path){path_edit_->setText(path);});
}

void FolderWidget::resizeEvent(QResizeEvent *)
{
    tips_label_->resize(this->width(), 25*scale_h_);
    decorate_label_->resize(this->width(), 1*scale_h_);
    hide_button_->resize(45*scale_w_, 40*scale_h_);
    show_button_->resize(hide_button_->size());
    sort_button_->resize(309*scale_w_, 25*scale_h_);
    type_label_->resize(sort_button_->size());
    filelist_->resize(619*scale_w_, 204*scale_h_);
    tips2_label_->resize(110*scale_w_, 25*scale_h_);
    path_edit_->resize(504*scale_w_, 25*scale_h_);
    win_button_->resize(this->width(), 43*scale_h_);
    create_button_->resize(128*scale_w_, 43*scale_h_);
    upper_button_->resize(128*scale_w_, 43*scale_h_);
    home_button_->resize(128*scale_w_, 43*scale_h_);
    yes_button_->resize(home_button_->size());
    no_button_->resize(yes_button_->size());

    tips_label_->move(0, 0);
    decorate_label_->move(0, tips_label_->height());
    hide_button_->move(10*scale_w_, decorate_label_->y() + decorate_label_->height());
    show_button_->move(hide_button_->x() + hide_button_->width(), hide_button_->y());
    if(is_show_)
    {
        sort_button_->show();
        sort_button_->move(hide_button_->x(), hide_button_->y() + hide_button_->height());
        type_label_->move(sort_button_->x() + sort_button_->width(), sort_button_->y());
        filelist_->move(sort_button_->x(), sort_button_->y() + sort_button_->height());
    }
    else
    {
        sort_button_->hide();
        type_label_->hide();
        filelist_->resize(619*scale_w_, 229*scale_h_);
        filelist_->move(hide_button_->x(), hide_button_->y() + hide_button_->height());
    }
    tips2_label_->move(filelist_->x(), filelist_->y() + filelist_->height() + 16*scale_h_);
    path_edit_->move(tips2_label_->x() + tips2_label_->width(), tips2_label_->y());
    win_button_->move(0, path_edit_->y() + path_edit_->height() + 5*scale_h_);
    create_button_->move(0, 0);
    upper_button_->move(create_button_->x() + create_button_->width(), create_button_->y());
    home_button_->move(upper_button_->x() + upper_button_->width(), upper_button_->y());
    no_button_->move(win_button_->width() - no_button_->width(), 0);
    yes_button_->move(no_button_->x() - no_button_->width(), no_button_->y());
}

void FolderWidget::SetScale(double scale_w, double scale_h)
{
    FileMangerObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

RecoveryWidget::RecoveryWidget(QWidget *parent, int type) : WindowObject(type, parent)
{
    this->GetIcoLabel()->setStyleSheet("border-image: url(:/image/Image/MainWidget/recovery_button.png)");
    this->GetTitleLabel()->setText(tr("恢复系统"));

    tips_label_ = new QLabel(tr("在恢复系统时发生了重启，任何针对系统参数和模块的修改若未保存则会丢失。\n浏览到要使用的备份文件夹。然后按“恢复”。"), main_widget_);
    tips2_label_ = new QLabel(tr("备份文件夹:"), main_widget_);
    path_edit_ = new QTextEdit(main_widget_);
    select_button_ = new QPushButton(tr("..."), main_widget_);
    renew_button_ = new QPushButton(tr("恢复"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);

    renew_button_->setFlat(true);
    cancel_button_->setFlat(true);
}

void RecoveryWidget::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips_label_->resize(449*ScaleW(), 141*ScaleH());
    tips2_label_->resize(150*ScaleW(), 22*ScaleH());
    path_edit_->resize(448*ScaleW(), 42*ScaleH());
    select_button_->resize(108*ScaleW(), 40*ScaleH());
    renew_button_->resize(128*ScaleW(), button_widget_->height());
    cancel_button_->resize(renew_button_->size());

    tips_label_->move(15*ScaleW(), 0);
    tips2_label_->move(43*ScaleW(), tips_label_->y() + tips_label_->height());
    path_edit_->move(42*ScaleW(), tips2_label_->y() + tips2_label_->height());
    select_button_->move(path_edit_->x() + path_edit_->width() + 6*ScaleW(), path_edit_->y());
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    renew_button_->move(cancel_button_->x() - renew_button_->width(), cancel_button_->y());
}

void RecoveryWidget::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}
