#include "filemanager.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QDateTime>
#include <QHeaderView>
#include <QDir>
#include <QDebug>

FileManager::FileManager(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QVBoxLayout *tips_layout = new QVBoxLayout;
    tips_label_ = new QLabel(tr("FlexPendant资源管理器"), this);
    QLabel *lb = new QLabel(this);
    lb->setFrameShape(QFrame::HLine);
    tips_layout->addWidget(tips_label_);
    tips_layout->addWidget(lb);

    QHBoxLayout *sort_layout = new QHBoxLayout;
    show_sort_button_ = new ImageButton(":/image/Image/MainWidget/show_button.png", this);
    hide_sort_button_ = new ImageButton(":/image/Image/MainWidget/hide_button.png", this);
    show_sort_button_->GetTextLabel()->hide();
    hide_sort_button_->GetTextLabel()->hide();
    curr_path_label_ = new QLabel(this);
    sort_layout->addWidget(hide_sort_button_);
    sort_layout->addWidget(show_sort_button_);
    sort_layout->addStretch();
    sort_layout->addWidget(curr_path_label_);
    sort_layout->setStretchFactor(hide_sort_button_, 2);
    sort_layout->setStretchFactor(show_sort_button_, 2);
    sort_layout->setStretchFactor(curr_path_label_, 15);
    sort_layout->setContentsMargins(0, 0, 0, 0);
    sort_layout->setSpacing(0);

    QVBoxLayout *so_layout = new QVBoxLayout;

    QHBoxLayout *soty_layout = new QHBoxLayout;
    soty_layout->setContentsMargins(0, 0, 0, 0);
    soty_layout->setSpacing(0);

    file_box_ = new TableBox(1, 2, this);

    so_layout->addLayout(soty_layout);
    so_layout->addWidget(file_box_);
    so_layout->setContentsMargins(0, 0, 0, 0);
    so_layout->setSpacing(0);

    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    menu_button_ = new ComboBoxObject(tr("菜单"), button_widget_);
    creator_button_ = new QPushButton(button_widget_);
    upper_button_ = new QPushButton(button_widget_);
    home_button_ = new QPushButton(button_widget_);
    refresh_button_ = new QPushButton(tr("刷新"), button_widget_);
    creator_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    upper_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    home_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    menu_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    refresh_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    creator_button_->setFlat(true);
    upper_button_->setFlat(true);
    home_button_->setFlat(true);
    menu_button_->setFlat(true);
    refresh_button_->setFlat(true);
    button_layout->addWidget(menu_button_);
    button_layout->addWidget(creator_button_);
    button_layout->addWidget(upper_button_);
    button_layout->addWidget(home_button_);
    button_layout->addWidget(refresh_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addLayout(tips_layout);
    layout->addLayout(sort_layout);
    layout->addLayout(so_layout);
    layout->addWidget(button_widget_);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStretchFactor(tips_layout, 1);
    layout->setStretchFactor(sort_layout, 2);
    layout->setStretchFactor(so_layout, 8);
    layout->setStretchFactor(button_widget_, 2);

    this->setLayout(layout);

    key_board_ = new Keyboard(this);
    natule_window_ = new NatuleWindow(this);

    key_board_->hide();
    natule_window_->hide();

    file_box_->horizontalHeader()->show();
    file_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    file_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    file_box_->SetRomn(1, 2);
    file_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("类型"));

    creator_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/create_button.png)");
    upper_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/upper_button.png)");
    home_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/home_button.png)");

    show_sort_button_->SetIsSelect(true);

    root_path_ = qApp->applicationDirPath() + "/data";
    home_path_ = root_path_ + "/DATA";
    curr_path_ = home_path_;

    curr_path_label_->setText(tr("路径：") + "/DATA");
    pos_neg_ = true;

    natule_window_->SetRootPath(&root_path_);

    copy_action_ = new QAction(tr("复制"), this);
    move_action_ = new QAction(tr("移动"), this);
    stick_action_ = new QAction(tr("粘贴"), this);
    rename_action_ = new QAction(tr("重命名"), this);
    del_action_ = new QAction(tr("删除"), this);
    natule_action_ = new QAction(tr("属性"), this);

    menu_button_->AppAction(copy_action_);
    menu_button_->AppAction(move_action_);
    menu_button_->AppAction(stick_action_);
    menu_button_->AppSeparator();
    menu_button_->AppAction(rename_action_);
    menu_button_->AppSeparator();
    menu_button_->AppAction(del_action_);
    menu_button_->AppSeparator();
    menu_button_->AppAction(natule_action_);

    copy_action_->setEnabled(false);
    move_action_->setEnabled(false);
    stick_action_->setEnabled(false);
    rename_action_->setEnabled(false);
    del_action_->setEnabled(false);
    natule_action_->setEnabled(false);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
    FindDirs(curr_path_);
}

void FileManager::SetScale(double scale_w, double scale_h)
{
    file_box_->SetScale(scale_w, scale_h);

    if(!key_board_->isHidden())
    {
        key_board_->resize(this->size());
        key_board_->move(0, 0);
    }
    if(!natule_window_->isHidden())
    {
        natule_window_->resize(this->size());
        natule_window_->move(0, 0);
    }

    key_board_->SetScale(scale_w, scale_h);
}

void FileManager::Sort()
{
    if(pos_neg_)
    {
        pos_neg_ = false;

        std::stable_sort(data_list_.begin(), data_list_.end(), ReverseOrder1);
    }
    else
    {
        pos_neg_ = true;

        std::stable_sort(data_list_.begin(), data_list_.end(), PositiveSequence1);
    }

    this->AddWidget(data_list_, curr_path_);
}

void FileManager::ShowKeyboard(bool iscreator)
{
    int row = file_box_->currentRow();
    if(row == -1)
        return;

    if(iscreator)
        key_board_->Init();
    else
        key_board_->Init(data_list_.at(row));

    key_board_->resize(this->size());
    key_board_->move(0, 0);
    key_board_->show();
}

void FileManager::Refresh()
{
    this->FindDirs(curr_path_);
}

void FileManager::FileLIstCurr(QTableWidgetItem *item)
{
    int row;

    if(item == nullptr)
        row = -1;
    else
        row = file_box_->row(item);

    if(row < 0)
    {
        copy_action_->setEnabled(false);
        move_action_->setEnabled(false);
        rename_action_->setEnabled(false);
        del_action_->setEnabled(false);
        natule_action_->setEnabled(false);
        return;
    }
    else {
        copy_action_->setEnabled(true);
        move_action_->setEnabled(true);
        rename_action_->setEnabled(true);
        del_action_->setEnabled(true);
        natule_action_->setEnabled(true);
    }
}

void FileManager::KeyGetData(QString data)
{
    if(data.isEmpty())
        return;

    if(iscreator_)
    {
        if(data_list_.contains(data))
        {
            QMessageBox::warning(this, tr("提示"), tr("该文件夹已存在"));
            return;
        }

        QDir dir(curr_path_);
        if(dir.mkpath(curr_path_ + "/" + data))
        {
            this->FindDirs(curr_path_);
        }
    }
    else {
        int row = file_box_->currentRow();
        if(row == -1)
            return;

        QString path = curr_path_ + "/" + data_list_.at(row);

        QFileInfo file_info(path);

        if(file_info.isDir())
        {
            QDir dir(file_info.path());
            dir.rename(file_info.fileName(), data);
        }
        else {
            QFile::rename(path, file_info.path() + "/" + data);
        }

        this->Refresh();
    }

    key_board_->hide();
}

void FileManager::Copy()
{
    int row = file_box_->currentRow();
    if(row < 0)
        return;

    QString path = curr_path_ + "/" + data_list_.at(row);

    if(!stick_action_->isEnabled())
        stick_action_->setEnabled(true);

    cove_path_ = path;
    iscopy_ = true;
}

void FileManager::Move()
{
    int row = file_box_->currentRow();
    if(row < 0)
        return;

    QString path = curr_path_ + "/" + data_list_.at(row);

    if(!stick_action_->isEnabled())
        stick_action_->setEnabled(true);

    cove_path_ = path;
    iscopy_ = false;
}

void FileManager::Stick()
{
    if(cove_path_.isEmpty())
    {
        stick_action_->setEnabled(false);
        return;
    }

    QString target_path = curr_path_;
    if(target_path == cove_path_)
    {
        QMessageBox::warning(this, tr("错误"), tr("禁止粘贴到此处"));
        return;
    }

    CopyOrMove(cove_path_, target_path, iscopy_);

    stick_action_->setEnabled(false);

    this->Refresh();
}

void FileManager::Rename()
{
    iscreator_ = false;
    ShowKeyboard(iscreator_);
}

void FileManager::Del()
{
    int row = file_box_->currentRow();
    if(row == -1)
        return;

    QString path = curr_path_ + "/" + data_list_.at(row);
    QFileInfo file_info(path);

    if(file_info.isDir())
    {
        auto x = QMessageBox::question(this, tr("文件对话"), tr("确定要删除文件夹 ") + file_info.fileName() + tr("及其全部内容吗"));

        if(x == QMessageBox::Yes)
            this->DelDir(path);
    }
    else {
        auto x = QMessageBox::question(this, tr("文件对话框"), tr("确定要删除 ") + file_info.fileName());

        if(x == QMessageBox::Yes)
        {
            QFile::remove(path);
        }
    }

    this->Refresh();
}

void FileManager::Natule()
{
    int row = file_box_->currentRow();
    if(row == -1)
        return;

    natule_window_->Init(curr_path_ + "/" + data_list_.at(row));

    natule_window_->resize(this->size());
    natule_window_->move(0, 0);
    natule_window_->show();
}

void FileManager::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void FileManager::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void FileManager::ConnectInit()
{
    connect(show_sort_button_, &ImageButton::clicked,
            [=]()
    {
        hide_sort_button_->SetIsSelect(false);

        file_box_->horizontalHeader()->show();
    });

    connect(hide_sort_button_, &ImageButton::clicked,
            [=]()
    {
        show_sort_button_->SetIsSelect(false);

        file_box_->horizontalHeader()->hide();
    });

    connect(upper_button_, &QPushButton::clicked,
            [=]()
    {
        QDir d(curr_path_);
        d.cdUp();

        if(d.path() == root_path_)
            return;
        curr_path_ = d.path();

        this->FindDirs(curr_path_);
    });

    connect(home_button_, &QPushButton::clicked,
            [=]()
    {
        if(curr_path_ == home_path_)
            return;
        curr_path_ = home_path_;

        this->FindDirs(curr_path_);
    });

    connect(file_box_, &TableBox::itemDoubleClicked,
            [=]()
    {
        int row = file_box_->currentRow();

        if(row < 0 || row >= data_list_.size())
            return;

        QString path = curr_path_ + "/" + data_list_.at(row);

        QFileInfo fn(path);

        if(fn.isDir())
        {
            curr_path_ = path;

            this->FindDirs(curr_path_);
        }
    });

    connect(creator_button_, &QPushButton::clicked,
            [=]()
    {
        iscreator_ = true;
        this->ShowKeyboard(iscreator_);
    });

    connect(natule_window_, &NatuleWindow::Close,
            [=]()
    {
        natule_window_->hide();
    });

    connect(key_board_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        key_board_->hide();
    });

    connect(key_board_, &Keyboard::GetData, this, &FileManager::KeyGetData);

    connect(file_box_->horizontalHeader(), &QHeaderView::sectionClicked, this, &FileManager::Sort);

    connect(refresh_button_, &QPushButton::clicked, this, &FileManager::Refresh);

    connect(file_box_, &TableBox::itemClicked, this, &FileManager::FileLIstCurr);

    connect(copy_action_, &QAction::triggered, this, &FileManager::Copy);

    connect(move_action_, &QAction::triggered, this, &FileManager::Move);

    connect(stick_action_, &QAction::triggered, this, &FileManager::Stick);

    connect(rename_action_, &QAction::triggered, this, &FileManager::Rename);

    connect(del_action_, &QAction::triggered, this, &FileManager::Del);

    connect(natule_action_, &QAction::triggered, this, &FileManager::Natule);
}

void FileManager::FindDirs(QString path)
{
    QString curr = "/" + path.mid(root_path_.size() + 1);
    curr_path_label_->setText(tr("路径：") + curr);

    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    data_list_ = dir.entryList();

    this->AddWidget(data_list_, path);
}

void FileManager::AddWidget(QStringList &data_list, QString path)
{
    if(file_box_->rowCount() != 0)
    {
        file_box_->clearContents();
        file_box_->SetRomn(0, 2);
        this->FileLIstCurr(nullptr);
    }

    for(int i = 0; i < data_list.size(); i++)
    {
        auto data = QFileInfo(path + "/" + data_list.at(i));
        QString suffix = data.suffix();
        if(suffix.isEmpty())
            suffix = "file";

        if(data.isDir())
            suffix = tr("文件夹");


        file_box_->AddText(i, 0, data.fileName());
        file_box_->AddText(i, 1, suffix);
    }
}

void FileManager::CopyOrMove(QString path, QString target_path, bool iscopy)
{
    QFileInfo file_info(path);

    if(file_info.isDir()) //文件夹
    {
        QDir target_dir(target_path), dir(path);
        target_dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

        auto target_dirs = target_dir.entryList();
        if(!target_dirs.contains(file_info.fileName()))
            target_dir.mkpath(target_path + "/" + file_info.fileName());

        target_dir.cd(file_info.fileName());

        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

        auto files = dir.entryList();
        for(auto file : files)
        {
            QFile::copy(dir.path() + "/" + file, target_dir.path() + "/" + file);

            if(!iscopy)
                QFile::remove(dir.path() + "/" + file);
        }

        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

        auto dirs = dir.entryList();
        for(auto dr : dirs)
        {
            CopyOrMove(path + "/" + dr, target_dir.path(), iscopy);
        }

        if(!iscopy)
        {
            dir.rmpath(path);
        }
    }
    else //文件
    {
        QFile::copy(path, target_path + "/" + file_info.fileName());

        if(!iscopy)
            QFile::remove(path);
    }
}

void FileManager::DelDir(QString dir_path)
{
    QDir dir(dir_path);

    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    auto files = dir.entryList();
    for(auto file : files)
    {
        QFile::remove(dir_path + "/" + file);
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    auto curr_dirs = dir.entryList();
    for(auto curr_dir : curr_dirs)
    {
        this->DelDir(dir_path + "/" + curr_dir);
    }

    dir.rmpath(dir_path);
}

NatuleWindow::NatuleWindow(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    info_widget_ = new QWidget(this);
    QGridLayout *info_layout = new QGridLayout(info_widget_);

    tips_type_label_ = new QLabel(tr("类型:"), info_widget_);
    type_label_ = new QLabel (info_widget_);
    tips_pos_label_ = new QLabel(tr("位置:"), info_widget_);
    pos_label_ = new QLabel(info_widget_);
    tips_size_label_ = new QLabel(tr("大小:"), info_widget_);
    size_label_ = new QLabel(info_widget_);
    tips_info_label_ = new QLabel(tr("包括:"), info_widget_);
    info_label_ = new QLabel(info_widget_);
    tips_creator_label_ = new QLabel(tr("创建时间:"), info_widget_);
    creator_label_ = new QLabel(info_widget_);
    tips_visit_label_ = new QLabel(tr("访问时间:"), info_widget_);
    visit_label_ = new QLabel(info_widget_);
    tips_modify_label_ = new QLabel(tr("修改时间:"), info_widget_);
    modify_label_ = new QLabel(info_widget_);

    info_layout->addWidget(tips_type_label_, 0, 0);
    info_layout->addWidget(type_label_, 0, 1);
    info_layout->addWidget(tips_pos_label_, 1, 0);
    info_layout->addWidget(pos_label_, 1, 1);
    info_layout->addWidget(tips_size_label_, 2, 0);
    info_layout->addWidget(size_label_, 2, 1);
    info_layout->addWidget(tips_info_label_, 3, 0);
    info_layout->addWidget(info_label_, 3, 1);
    info_layout->addWidget(tips_creator_label_, 4, 0);
    info_layout->addWidget(creator_label_, 4, 1);
    info_layout->addWidget(tips_visit_label_, 5, 0);
    info_layout->addWidget(visit_label_, 5, 1);
    info_layout->addWidget(tips_modify_label_, 6, 0);
    info_layout->addWidget(modify_label_, 6, 1);

    info_widget_->setLayout(info_layout);

    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);

    show_size_button_ = new QPushButton(tr("显示大小"), button_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);
    show_size_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    show_size_button_->setFlat(true);
    close_button_->setFlat(true);

    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(show_size_button_);
    button_layout->addWidget(close_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(info_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(info_widget_, 8);
    layout->setStretchFactor(button_widget_, 1);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(close_button_, &QPushButton::clicked,
            [=]()
    {
        emit Close();
    });
    connect(show_size_button_, &QPushButton::clicked,
            [=]()
    {
        auto x = QMessageBox::question(this, tr("提示"), tr("大型文件夹的大小计算可能需要一段时间，要继续吗?"));
        if(x == QMessageBox::No)
            return;

        tips_size_label_->show();
        size_label_->show();

        QFileInfo file_info(curr_path_);
        size_label_->setText(QString::number(file_info.size()));
    });
}

void NatuleWindow::SetRootPath(QString *root_path)
{
    root_path_ = root_path;
}

void NatuleWindow::Init(QString path)
{
    QFileInfo file_info(path);
    curr_path_ = path;
    QDir dir(path);
    dir.cdUp();

    QString curr = "/" + dir.path().mid(root_path_->size() + 1);
    pos_label_->setText(curr);

    auto modify = file_info.lastModified();
    modify_label_->setText(modify.toString());

    if(file_info.isDir())
    {
        tips_size_label_->hide();
        size_label_->hide();

        tips_info_label_->show();
        info_label_->show();

        tips_creator_label_->hide();
        creator_label_->hide();
        tips_visit_label_->hide();
        visit_label_->hide();

        show_size_button_->show();

        type_label_->setText(tr("文件夹"));

        QDir dr(path);

        dr.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        auto dirs = dr.entryList();
        dr.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        auto files = dr.entryList();

        info_label_->setText(QString("%1 file, %2 folder").arg(dirs.size()).arg(files.size()));
    }
    else {
        tips_size_label_->show();
        size_label_->show();

        tips_info_label_->hide();
        info_label_->hide();

        tips_creator_label_->show();
        creator_label_->show();
        tips_visit_label_->show();
        visit_label_->show();

        show_size_button_->hide();

        if(file_info.suffix().isEmpty())
            type_label_->setText("file");
        else
            type_label_->setText(file_info.suffix());

        size_label_->setText(QString::number(file_info.size()));

        creator_label_->setText(file_info.created().toString());
        visit_label_->setText(file_info.lastRead().toString());
    }
}
