#include "filemangerobject.h"
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QDebug>

FileMangerObject::FileMangerObject(QWidget *parent, bool isinherit) : QWidget(parent)
{
    sort_button_ = new QPushButton(tr("名称"), this);
    show_button_ = new QPushButton(this);
    hide_button_ = new QPushButton(this);
    type_label_ = new QLabel(tr("类型"), this);
    filelist_ = new FileList(this);
    create_button_ = new QPushButton(this);
    upper_button_ = new QPushButton(this);
    home_button_ = new QPushButton(this);
    keyboard_create_dir_ = new Keyboard(this);

    scale_w_ = 1;
    scale_h_ = 1;
    is_show_ = true;
    pos_neg_ = true; //默认为正向排序
    root_path_ = "/";
    isinherit_ = isinherit;

    keyboard_create_dir_->hide();

    home_path_ = qApp->applicationDirPath() + "/data/DATA/"; //将示教器的运行路径下的data文件夹设置为主目录
    QDir d(home_path_);
    if(!d.exists(home_path_)) //如果data文件夹不存在
        d.mkpath(home_path_);

    path_ = home_path_; //初始时当前路径与主目录一致

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(192, 192, 192));
    type_label_->setAutoFillBackground(true);
    type_label_->setPalette(palette);

    Init();
}

void FileMangerObject::Init()
{
    hide_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/hide_button.png)");
    show_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/show_button.png)");
    create_button_->setFlat(true);
    upper_button_->setFlat(true);
    home_button_->setFlat(true);
    type_label_->setFrameShape(QFrame::Box);
    sort_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    create_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/create_button.png)");
    home_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/home_button.png)");
    upper_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/upper_button.png)");

    this->Refresh();

    //connect(filelist_, &FileList::itemDoubleClicked, this, &FileMangerObject::TogglePages); //因为有时需要单击打开有时需要双击打开，所以交由子类或者使用时看情况绑定
    connect(create_button_, &QPushButton::clicked, this, &FileMangerObject::ShowKeyboardWidget);
    connect(keyboard_create_dir_->cancel_button_, &QPushButton::clicked, this, &FileMangerObject::HideKeyboardWidget);
    connect(keyboard_create_dir_, &Keyboard::GetData, this, &FileMangerObject::GetKeyboardData);
    connect(upper_button_, &QPushButton::clicked, this, &FileMangerObject::LastPages);
    connect(home_button_, &QPushButton::clicked, this, &FileMangerObject::ReturnHome);
    connect(sort_button_, &QPushButton::clicked, this, &FileMangerObject::Sort);
    connect(hide_button_, &QPushButton::clicked,
            [=]()
    {
        is_show_ = false;
        resizeEvent(nullptr);
    });
    connect(show_button_, &QPushButton::clicked,
            [=]()
    {
        is_show_ = true;
        resizeEvent(nullptr);
    });
}

void FileMangerObject::SetInherit(bool isinherit)
{
    isinherit_ = isinherit;
}

void FileMangerObject::resizeEvent(QResizeEvent *)
{
    hide_button_->resize(45*scale_w_, 40*scale_h_);
    show_button_->resize(hide_button_->size());
    sort_button_->resize(309*scale_w_, 25*scale_h_);
    type_label_->resize(sort_button_->size());
    create_button_->resize(128*scale_w_, 43*scale_h_);
    upper_button_->resize(128*scale_w_, 43*scale_h_);
    home_button_->resize(128*scale_w_, 43*scale_h_);
    if(isinherit_) //继承式使用
        filelist_->resize(619*scale_w_, this->height() - hide_button_->height() - sort_button_->height() - upper_button_->height());
    else //直接使用
        filelist_->resize(619*scale_w_, this->height() - hide_button_->height() - sort_button_->height());

    hide_button_->move(10*scale_w_, 0);
    show_button_->move(hide_button_->x() + hide_button_->width(), hide_button_->y());
    if(is_show_)
    {
        if(keyboard_create_dir_->isHidden())
        {
            sort_button_->show();
            type_label_->show();
        }
        sort_button_->move(hide_button_->x(), hide_button_->y() + hide_button_->height());
        type_label_->move(sort_button_->x() + sort_button_->width(), sort_button_->y());
        filelist_->move(sort_button_->x(), sort_button_->y() + sort_button_->height());
    }
    else
    {
        sort_button_->hide();
        type_label_->hide();
        if(isinherit_)
            filelist_->resize(619*scale_w_, this->height() - hide_button_->height() - upper_button_->height());
        else
            filelist_->resize(619*scale_w_, this->height() - hide_button_->height());
        filelist_->move(hide_button_->x(), hide_button_->y() + hide_button_->height());
    }

    if(!keyboard_create_dir_->isHidden())
    {
        keyboard_create_dir_->resize(this->size());
        keyboard_create_dir_->move(0, 0);
    }
}

void FileMangerObject::Refresh()
{
    this->filelist_->ClearWidget();
    this->FindFiles(this->path_);
    this->filelist_->AddWidget(this->file_list_);
}

void FileMangerObject::FindFiles(QString path)
{
    QDir d(path);
    d.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot); //只扫描正常的文件、文件夹

    file_list_.clear();
    for(auto info : d.entryInfoList())
    {
        QString data;

        if(info.isDir())
        {
            data.append(info.fileName() + " dir");
        }
        else
        {
            QString file_suffix = info.suffix(); //获取文件后缀名

            if(!file_filter_.isEmpty()) //需要过滤文件时
            {
                if(file_suffix.isEmpty()) //这里必须要求有后缀名所以没有后缀名的文件全部舍弃
                    continue;

                for(auto suffix : file_filter_)
                {
                    if(suffix == "*.*") //显示所有文件
                    {
                        data.append(info.fileName() + " ." + file_suffix + " " +info.filePath());
                        continue;
                    }
                    suffix = suffix.mid(suffix.lastIndexOf(".") + 1);

                    if(file_suffix == suffix)
                    {
                        data.append(info.fileName() + " ." + file_suffix + " " +info.filePath());
                        continue;
                    }
                }
            }
            else //不需要进行文件过滤
            {
                if(file_suffix.isEmpty()) //无后缀名
                    data.append(info.fileName() + " file");
                else //有后缀名
                    data.append(info.fileName() + " ." + file_suffix);
            }
        }

        if(!data.isEmpty()) //不将空数据添加进来
            file_list_.append(data);
    }
}

bool PositiveSequence1(const QString &str1, const QString &str2) //对文件进行正向排序
{
    return str1.toLower() < str2.toLower();
}

bool ReverseOrder1(const QString &str1, const QString &str2) //对文件进行倒向排序
{
    return str1.toLower() > str2.toLower();
}
void FileMangerObject::Sort()
{
    if(pos_neg_) //如果是正向排序
    {
        pos_neg_ = false; //调整为反向排序

        std::stable_sort(file_list_.begin(), file_list_.end(), ReverseOrder1); //Qt中有一个qsort函数功能与这个一样，但是使用Qt的qsort会有警告

        filelist_->ClearWidget();
        filelist_->AddWidget(file_list_);
    }
    else
    {
        pos_neg_ = true;

        std::stable_sort(file_list_.begin(), file_list_.end(), PositiveSequence1);

        filelist_->ClearWidget();
        filelist_->AddWidget(file_list_);
    }
}

void FileMangerObject::AddFilter(QString str)
{
    file_filter_.append(str);
}

void FileMangerObject::SetFileFilter(QStringList &filter)
{
    file_filter_ = filter;
}

void FileMangerObject::FileFilterClear()
{
    file_filter_.clear();
}

QString FileMangerObject::CurrentText(QString text)
{
    return text.section(" ", 0, 0);
}

void FileMangerObject::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    filelist_->SetScale(scale_w, scale_h);
    keyboard_create_dir_->SetScale(scale_w, scale_h);

    this->Refresh();
}

void FileMangerObject::TogglePages(QListWidgetItem *)
{
    int row = filelist_->currentRow();
    TogglePagesi(row);
}

void FileMangerObject::TogglePagesi(int row)
{
    if(row < 0)
        return;
    QString type = file_list_.at(row).section(" ", 1, 1);
    if(type == "dir")
    {
        if(path_ == "/")
            path_.append(file_list_.at(row).section(" ", 0, 0));
        else
            path_.append("/" + file_list_.at(row).section(" ", 0, 0));

        FindFiles(path_);

        filelist_->ClearWidget();

        filelist_->AddWidget(file_list_);

        emit PathChange(path_);
    }
    else
    {
        CurrentText(file_list_.at(row));
    }
}

void FileMangerObject::LastPages()
{
    QDir dir(path_);

    dir.cdUp();

    if(dir.path() == root_path_)
        return;

    path_ = dir.path();

    FindFiles(path_);
    filelist_->ClearWidget();
    filelist_->AddWidget(file_list_);

    emit PathChange(path_);

}

void FileMangerObject::ReturnHome()
{
    path_ = home_path_;

    FindFiles(path_);
    filelist_->ClearWidget();
    filelist_->AddWidget(file_list_);

    emit PathChange(path_);
}

void FileMangerObject::ShowKeyboardWidget()
{
    keyboard_create_dir_->Init();
    keyboard_create_dir_->show();
    keyboard_create_dir_->resize(this->size());
    keyboard_create_dir_->move(0, 0);

    sort_button_->hide();
    show_button_->hide();
    hide_button_->hide();
    type_label_->hide();
    filelist_->hide();
}

void FileMangerObject::HideKeyboardWidget()
{
    keyboard_create_dir_->hide();

    sort_button_->show();
    show_button_->show();
    hide_button_->show();
    type_label_->show();
    filelist_->show();
}

void FileMangerObject::GetKeyboardData(QString text)
{
    if(text.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入内容"));
        return;
    }

    QString dir_path = path_ + "/" + text;
    QDir dir(dir_path);

    if(dir.exists())
    {
        QMessageBox::warning(this, tr("提示"), tr("该文件夹已存在，请重新输入"));
        return;
    }
    else
    {
        if(dir.mkpath(dir_path))
        {
            this->Refresh();
        }
        else
        {
            QMessageBox::warning(this, tr("提示"), tr("文件夹创建失败，请检查您是否拥有相关权限"));
        }
    }

    HideKeyboardWidget();
}

FLWidget::FLWidget(const QString &name, const QString &type, int scale_w, int scale_h, QWidget *parent) : QWidget(parent)
{
    ico_label_ = new QLabel(this);
    name_label_ = new QLabel(name, this);
    type_label_ = new QLabel(this);
    if(type == "dir")
        type_label_->setText(tr("文件夹"));
    else
        type_label_->setText(type);

    scale_w_ = scale_w;
    scale_h_ = scale_h;

    ico_label_->resize(24*scale_w_, 32*scale_h_);
    name_label_->resize(278*scale_w_, 32*scale_h_);
    type_label_->resize(301*scale_w_, 32*scale_h_);

    ico_label_->move(0, 0);
    name_label_->move(ico_label_->x() + ico_label_->width(), ico_label_->y());
    type_label_->move(name_label_->x() + name_label_->width() + 10*scale_w_, name_label_->y());

    this->resize(ico_label_->width() + name_label_->width() + type_label_->width() + 10, ico_label_->height());
}

void FLWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    ico_label_->resize(24*scale_w_, 32);
    name_label_->resize(278*scale_w_, 32);
    type_label_->resize(301*scale_w_, 32);

    ico_label_->move(0, 0);
    name_label_->move(ico_label_->x() + ico_label_->width(), ico_label_->y());
    type_label_->move(name_label_->x() + name_label_->width() + 10*scale_w_, name_label_->y());
}

FileList::FileList(QWidget *parent) : ListBoxObject(parent)
{

}

void FileList::AddWidget(QString data)
{
    if(data.isEmpty())
        return;
    QString name = data.section(" ", 0, 0); //获取用户名
    QString type = data.section(" ", 1, 1); //获取类型

    FLWidget *w = new FLWidget(name, type);
    widget_list_.append(w);
    ListBoxObject::AddWidget(w);

    connect(this, &FileList::SendScale, w, &FLWidget::SetScale);
}

void FileList::AddWidget(QStringList data)
{
    if(data.isEmpty())
        return;
    for(auto str : data)
    {
        QString name = str.section(" ", 0, 0); //获取用户名
        QString type = str.section(" ", 1, 1); //获取类型

        FLWidget *w = new FLWidget(name, type, GetScaleW(), GetScaleH(), this);
        widget_list_.append(w);
        ListBoxObject::AddWidget(w);
        connect(this, &FileList::SendScale, w, &FLWidget::SetScale);
    }
}

void FileList::ClearWidget()
{
    this->clear(); //将列表清空

    widget_list_.clear();
}

void FileList::SetScale(double scale_w, double scale_h)
{
    ListBoxObject::SetScale(scale_w, scale_h);

    emit SendScale(scale_w, scale_h);
}
