#include "eventlog.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QDateTime>
#include <QHeaderView>
#include <QMessageBox>
#include <RobotClient/robotClient.h>
#include <QFileInfo>
#include <QTextCodec>
#include <QDir>
#include <QDebug>

using namespace rclib;

EventLog::EventLog(QWidget *parent) : QFrame(parent)
{
    stacked_widget_ = new StackedWidObject(this);
    log_widget_ = new LogWidget(this);
    save_log_widget_ = new SaveLogWidget(this);

    stacked_widget_->SetMainWidget(log_widget_);
    stacked_widget_->AddWidget(save_log_widget_, "save_log_widget");

    this->setWindowFlags(Qt::Popup);
    this->setFrameShape(QFrame::Box);

    ConnectInit();
}

void EventLog::resizeEvent(QResizeEvent *)
{
    stacked_widget_->resize(this->size());
}

void EventLog::ConnectInit()
{
    connect(log_widget_->save_button_, &QPushButton::clicked, [=](){stacked_widget_->Change("save_log_widget");});

    connect(save_log_widget_->cancel_button_, &QPushButton::clicked, [=](){stacked_widget_->Change();});
}

void EventLog::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void EventLog::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void EventLog::ChangeLogWidget(int row)
{
    stacked_widget_->Change();
    log_widget_->ShowRow(row);
}

void EventLog::SetScale(double scale_w, double scale_h)
{
    log_widget_->SetScale(scale_w, scale_h);
    save_log_widget_->SetScale(scale_w, scale_h);
}

LogWidget::LogWidget(QWidget *parent) : QWidget(parent)
{
    mutex_ = new QReadWriteLock;

    view_label_ = new QLabel(tr("事件日志-公用"), this);
    decorate_label_ = new QLabel(this);
    tips_label = new QLabel(tr("点击一个消息便可打开"), this);
    log_box_ = new TableBox(this);
    win_button_ = new QWidget(this);
    save_button_ = new QPushButton(tr("另存所有日志为..."), this);
    del_button_ = new ComboBoxObject(tr("删除"), this);
    update_button_ = new ComboBoxObject(tr("更新"), this);
    view_button_ = new ComboBoxObject(tr("视图"), this);
    get_log_timer_ = new QTimer(this);
    show_log_info_ = new ShowLogInfo(this);

    scale_w_ = 1;
    scale_h_ = 1;
    isinit_ = false;

    decorate_label_->setFrameShape(QFrame::HLine); //使该标签显示一条直线，用作分割线
    save_button_->setFlat(true);
    del_button_->setFlat(true);
    update_button_->setFlat(true);
    view_button_->setFlat(true);
    show_log_info_->hide();

    log_box_->setSelectionBehavior(QAbstractItemView::SelectRows); //设置整行选中
    log_box_->setSelectionMode(QAbstractItemView::SingleSelection);

    win_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    save_button_->setStyleSheet("background-color: rgb(255, 255, 255)");

    log_box_->horizontalHeader()->show();
    QStringList head_name;
    head_name << tr("代码") << tr("标题") << tr("日期和时间");
    log_box_->SetRomn(0, head_name.size());
    log_box_->setHorizontalHeaderLabels(head_name);

    get_log_timer_->start(200);

    ReadLog();
    TeachState::instance()->SetSendLogFun(std::bind(&LogWidget::PushLog, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, QString()));

    connect(get_log_timer_, &QTimer::timeout, this, &LogWidget::GetLogTimer);
    connect(del_button_, &ComboBoxObject::clicked, this, &LogWidget::DelLog);
    connect(log_box_, &TableBox::itemClicked, this, &LogWidget::LogBoxClicked);
    connect(show_log_info_, &ShowLogInfo::Upper, this, &LogWidget::Upper);
    connect(show_log_info_, &ShowLogInfo::Lower, this, &LogWidget::Lower);
    connect(show_log_info_, &ShowLogInfo::Close, this, &LogWidget::Close);
}

LogWidget::~LogWidget()
{
    WriteLog();
    delete mutex_;
}

void LogWidget::resizeEvent(QResizeEvent *)
{
    view_label_->resize(640*scale_w_, 24*scale_h_);
    decorate_label_->resize(view_label_->width(), 1*scale_h_);
    tips_label->resize(view_label_->width(), view_label_->height());
    log_box_->resize(640*scale_w_, 300*scale_h_);
    win_button_->resize(640*scale_w_, 41*scale_h_);
    save_button_->resize(126*scale_w_, 41*scale_h_);
    del_button_->resize(save_button_->size());
    update_button_->resize(del_button_->size());
    view_button_->resize(update_button_->size());

    view_label_->move(0, 0);
    decorate_label_->move(0, view_label_->height());
    tips_label->move(0, decorate_label_->y() + decorate_label_->height());
    log_box_->move(0, tips_label->y() + tips_label->height());
    win_button_->move(0, log_box_->y() + log_box_->height());
    save_button_->move(win_button_->x(), win_button_->y());
    del_button_->move(save_button_->x() + save_button_->width(), save_button_->y());
    update_button_->move(del_button_->x() + del_button_->width(), del_button_->y());
    view_button_->move(log_box_->width() - view_button_->width(), update_button_->y());

    if(!show_log_info_->isHidden())
    {
        show_log_info_->resize(this->size());
        show_log_info_->move(0, 0);
    }
}

void LogWidget::ShowRow(int row)
{
    if(row < 0 || row >= log_box_->GetRow())
        return;

    log_box_->selectRow(row);
    auto *item = log_box_->item(row, 0);
    LogBoxClicked(item);
}

void LogWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    log_box_->SetScale(scale_w, scale_h);
    show_log_info_->SetScale(scale_w, scale_h);
}

bool is_str_utf8(const char* str)
{
    unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
    unsigned char chr = *str;
    bool bAllAscii = true;
    for (unsigned int i = 0; str[i] != '\0'; ++i){
        chr = *(str + i);
        //判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
        if (nBytes == 0 && (chr & 0x80) != 0){
            bAllAscii = false;
        }
        if (nBytes == 0) {
            //如果不是ASCII码,应该是多字节符,计算字节数
            if (chr >= 0x80) {
                if (chr >= 0xFC && chr <= 0xFD){
                    nBytes = 6;
                }
                else if (chr >= 0xF8){
                    nBytes = 5;
                }
                else if (chr >= 0xF0){
                    nBytes = 4;
                }
                else if (chr >= 0xE0){
                    nBytes = 3;
                }
                else if (chr >= 0xC0){
                    nBytes = 2;
                }
                else{
                    return false;
                }
                nBytes--;
            }
        }
        else{
            //多字节符的非首字节,应为 10xxxxxx
            if ((chr & 0xC0) != 0x80){
                return false;
            }
            //减到为零为止
            nBytes--;
        }
    }
    //违返UTF8编码规则
    if (nBytes != 0) {
        return false;
    }
    if (bAllAscii){ //如果全部都是ASCII, 也是UTF8
        return true;
    }
    return true;
}

void LogWidget::GetLogTimer()
{
    INFOSTATE state;
    std::string buff;
    RobotClient::initance()->getInfo(state, buff);
    if(state == INFOSTATE::INFO_NONE && buff.empty())
    {
        return;
    }

    QString mess;
    if(is_str_utf8(buff.c_str()))
    {
        mess = buff.c_str();
    }
    else
    {
        auto *codec = QTextCodec::codecForName("GB18030");
        mess = codec->toUnicode(buff.c_str());
    }

    if(mess == tr("急停开关已按下"))
    {
        TeachState::instance()->SetStop(true);
        return;
    }

    PushLog(TeachState::SoToABBLogType(state), mess);

    if(mess == tr("限位回零过程成功") || mess == tr("限位回零过程失败，请重新回零"))
    {
        QMessageBox::about(this, tr("提示"), mess);
    }
}

void LogWidget::PushLog(EventLogType type, QString data, QString info, QString time)
{
    if(type == EventLogType::None) //丢弃掉无意义消息
        return;

    QWriteLocker lk(mutex_);

    int row = log_box_->GetRow();
    log_box_->AddText(row, 0, QString::number(TeachState::LogTypeToInt(type)));
    log_box_->AddText(row, 1, data);
    if(time.isEmpty())
    {
        auto tm = QDateTime::currentDateTime();
        log_box_->AddText(row, 2, tm.toString("yyyy.MM.dd hh:mm:ss.zzz"));
    }
    else
    {
        log_box_->AddText(row, 2, time);
    }

    log_info_.append(info);
    lk.unlock();

    if(isinit_) //已初始化
        IsPoP(type, row);
}

void LogWidget::DelLog()
{
    auto mag = QMessageBox::question(this, tr("提示"), tr("确定要删除所有日志吗?"));
    if(mag == QMessageBox::No)
        return;

    QWriteLocker lk(mutex_);

    log_box_->clearContents();
    log_box_->SetRomn(0, log_box_->GetColumn());
    log_info_.clear();
}

void LogWidget::LogBoxClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;
    int row = log_box_->row(item);
    auto *type_item = log_box_->item(row, 0);
    auto *data_item = log_box_->item(row, 1);
    auto *time_item = log_box_->item(row, 2);

    show_log_info_->resize(this->size());
    show_log_info_->move(0, 0);
    show_log_info_->Init(row, type_item->text(), data_item->text(), time_item->text(), log_info_.at(row));
    show_log_info_->show();
}

void LogWidget::Upper(int row)
{
    if(row <= 0)
        return;

    row--;
    auto *type_item = log_box_->item(row, 0);
    auto *data_item = log_box_->item(row, 1);
    auto *time_item = log_box_->item(row, 2);

    show_log_info_->Init(row, type_item->text(), data_item->text(), time_item->text(), log_info_.at(row));
}

void LogWidget::Lower(int row)
{
    if(row >= log_box_->GetRow() - 1)
        return;

    row++;
    auto *type_item = log_box_->item(row, 0);
    auto *data_item = log_box_->item(row, 1);
    auto *time_item = log_box_->item(row, 2);

    show_log_info_->Init(row, type_item->text(), data_item->text(), time_item->text(), log_info_.at(row));
}

void LogWidget::Close()
{
    show_log_info_->hide();
}

void LogWidget::showEvent(QShowEvent *)
{
    auto *scbar = log_box_->verticalScrollBar();
    scbar->setValue(scbar->maximum()); //使得每次显示日志窗口时都自动跳到最后一行显示最新的日志
}

void LogWidget::ReadLog()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/Log/log";
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QWriteLocker lk(mutex_);
    log_box_->clearContents();
    log_box_->SetRomn(0, log_box_->GetColumn());
    log_info_.clear();
    isinit_ = false;
    lk.unlock();

    while(!file.atEnd())
    {
        QString data = file.readLine();
        if(data.isEmpty() || data == "\n")
            continue;

        auto state = data.section("☯", 0, 0).toInt();
        auto title = data.section("☯", 1, 1);
        auto time = data.section("☯", 2, 2);
        auto info = data.section("☯", 3, 3);

        PushLog(TeachState::IntToLogType(state), title, info, time);
    }

    isinit_ = true;
}

void LogWidget::WriteLog()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/Log/log";
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QReadLocker sk(mutex_);
    int row = log_box_->GetRow();
    for(int i = 0; i < row; i++)
    {
        auto *item1 = log_box_->item(i, 0);
        auto *item2 = log_box_->item(i, 1);
        auto *item3 = log_box_->item(i, 2);

        auto text = QString("%1☯%2☯%3☯%4").arg(item1->text()).arg(item2->text()).arg(item3->text()).arg(log_info_.value(i));
        if(text.at(text.size() - 1) != '\n')
            text.append("\n");

        file.write(text.toUtf8());
    }
}

void LogWidget::IsPoP(EventLogType type, int row)
{
    switch(type)
    {
    case EventLogType::TipsPop:
    {
        emit PoPLogWidget(row);
        break;
    }
    case EventLogType::WarningPop:
    {
        emit PoPLogWidget(row);
        break;
    }
    case EventLogType::ErrorPop:
    {
        emit PoPLogWidget(row);
        break;
    }
    default: return;
    }
}

SaveLogWidget::SaveLogWidget(QWidget *parent) : QWidget(parent)
{
    decorate_label_ = new QLabel(this);
    sort_button_ = new QPushButton(tr("名称"), this);
    name_label_ = new QLabel(tr("文件名:"), this);
    name_edit_ = new QLineEdit(this);
    win_button_ = new QWidget(this);
    tips_label_ = new QLabel(tr("另存为"), this);
    show_button_ = new QPushButton(this);
    hide_button_ = new QPushButton(this);
    filter_combox_ = new QComboBox(this);
    type_label_ = new QLabel(tr("类型"), this);
    save_log_box_ = new SaveLogBox(this);
    keyboard_button_ = new QPushButton("...", this);
    create_button_ = new QPushButton(win_button_);
    upper_button_ = new QPushButton(win_button_);
    home_button_ = new QPushButton(win_button_);
    confirm_button_ = new QPushButton(tr("确定"), win_button_);
    cancel_button_ = new QPushButton(tr("取消"), win_button_);

    scale_w_ = 1;
    scale_h_ = 1;
    is_show_ = true;
    pos_neg_ = true; //默认为正向排序

    home_path_ = qApp->applicationDirPath() + "/data"; //将示教器的运行路径下的data文件夹设置为主目录
    QDir d(home_path_);
    if(!d.exists(home_path_)) //如果data文件夹不存在
        d.mkpath(home_path_);

    path_ = home_path_; //初始时当前路径与主目录一致

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(192, 192, 192));
    win_button_->setAutoFillBackground(true);
    win_button_->setPalette(palette);

    type_label_->setAutoFillBackground(true);
    type_label_->setPalette(palette);

    keyboard_ = new Keyboard(this);
    keyboard_->hide();

    filter_combox_->addItem(tr("全部文件") + " (*.*)");

    Init();

    connect(confirm_button_, &QPushButton::clicked, this, &SaveLogWidget::SaveClicked);
    connect(keyboard_button_, &QPushButton::clicked, this, &SaveLogWidget::ShowKeyboard);
    connect(keyboard_->cancel_button_, &QPushButton::clicked, keyboard_, &Keyboard::hide);
    connect(keyboard_, &Keyboard::GetData, this, &SaveLogWidget::KeyRetData);
}

void SaveLogWidget::Init()
{
    decorate_label_->setFrameShape(QFrame::HLine);
    hide_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/hide_button.png)");
    show_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/show_button.png)");
    create_button_->setFlat(true);
    upper_button_->setFlat(true);
    home_button_->setFlat(true);
    confirm_button_->setFlat(true);
    cancel_button_->setFlat(true);
    type_label_->setFrameShape(QFrame::Box);
    sort_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    create_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/create_button.png)");
    home_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/home_button.png)");
    upper_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/upper_button.png)");

    this->FindFiles(path_);
    save_log_box_->AddWidget(file_list_);

    tips_label_->setText(tr("另存为 - ") + path_); //更新路径提示标签

    connect(save_log_box_, &SaveLogBox::currentRowChanged, this, &SaveLogWidget::TogglePages);
    connect(upper_button_, &QPushButton::clicked, this, &SaveLogWidget::LastPages);
    connect(home_button_, &QPushButton::clicked, this, &SaveLogWidget::ReturnHome);
    connect(sort_button_, &QPushButton::clicked, this, &SaveLogWidget::Sort);
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

void SaveLogWidget::resizeEvent(QResizeEvent *)
{
    tips_label_->resize(640*scale_w_, 25*scale_h_);
    decorate_label_->resize(640*scale_w_, 1*scale_h_);
    hide_button_->resize(45*scale_w_, 40*scale_h_);
    show_button_->resize(hide_button_->size());
    filter_combox_->resize(299*scale_w_, 32*scale_h_);
    sort_button_->resize(309*scale_w_, 25*scale_h_);
    type_label_->resize(sort_button_->size());
    save_log_box_->resize(619*scale_w_, 204*scale_h_);
    name_label_->resize(110*scale_w_, 25*scale_h_);
    name_edit_->resize(424*scale_w_, 25*scale_h_);
    keyboard_button_->resize(80*scale_w_, 25*scale_h_);
    win_button_->resize(640*scale_w_, 43*scale_w_);
    create_button_->resize(128*scale_w_, 43*scale_h_);
    upper_button_->resize(128*scale_w_, 43*scale_h_);
    home_button_->resize(128*scale_w_, 43*scale_h_);
    confirm_button_->resize(128*scale_w_, 43*scale_h_);
    cancel_button_->resize(128*scale_w_, 43*scale_h_);

    tips_label_->move(0, 0);
    decorate_label_->move(0, tips_label_->height());
    hide_button_->move(10*scale_w_, decorate_label_->y() + decorate_label_->height());
    show_button_->move(hide_button_->x() + hide_button_->width(), hide_button_->y());
    filter_combox_->move(this->width() - filter_combox_->width(), show_button_->y());
    if(is_show_)
    {
        sort_button_->show();
        type_label_->show();
        sort_button_->move(hide_button_->x(), hide_button_->y() + hide_button_->height());
        type_label_->move(sort_button_->x() + sort_button_->width(), sort_button_->y());
        save_log_box_->move(sort_button_->x(), sort_button_->y() + sort_button_->height());
    }
    else
    {
        sort_button_->hide();
        type_label_->hide();
        save_log_box_->resize(619*scale_w_, 229*scale_h_);
        save_log_box_->move(hide_button_->x(), hide_button_->y() + hide_button_->height());
    }
    name_label_->move(save_log_box_->x(), save_log_box_->y() + save_log_box_->height() + 16*scale_h_);
    name_edit_->move(name_label_->x() + name_label_->width(), name_label_->y());
    keyboard_button_->move(name_edit_->x() + name_edit_->width(), name_edit_->y());
    win_button_->move(0, name_label_->y() + name_label_->height() + 5*scale_h_);
    create_button_->move(0, 0);
    upper_button_->move(create_button_->x() + create_button_->width(), create_button_->y());
    home_button_->move(upper_button_->x() + upper_button_->width(), upper_button_->y());
    confirm_button_->move(home_button_->x() + home_button_->width(), home_button_->y());
    cancel_button_->move(confirm_button_->x() + confirm_button_->width(), confirm_button_->y());
}

void SaveLogWidget::FindFiles(QString path)
{
    QDir d(path);
    d.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot); //只扫描正常的文件、文件夹

    file_list_.clear();
    for(auto info : d.entryInfoList())
    {
        QString data;
        data.append(info.fileName());
        if(info.isDir())
        {
            data.append(" dir");
        }
        else
        {
            if(!info.suffix().isEmpty()) //有后缀名
                data.append(" ." + info.suffix());
            else //无后缀名
                data.append(" file");
        }
        file_list_.append(data);
    }
}

bool PositiveSequence(const QString &str1, const QString &str2) //对文件进行正向排序
{
    return str1.toLower() < str2.toLower();
}

bool ReverseOrder(const QString &str1, const QString &str2) //对文件进行倒向排序
{
    return str1.toLower() > str2.toLower();
}

void SaveLogWidget::Sort()
{
    if(pos_neg_) //如果是正向排序
    {
        pos_neg_ = false; //调整为反向排序

        std::stable_sort(file_list_.begin(), file_list_.end(), ReverseOrder); //Qt中有一个qsort函数功能与这个一样，但是使用Qt的qsort会有警告

        save_log_box_->ClearWidget();
        save_log_box_->AddWidget(file_list_);
    }
    else
    {
        pos_neg_ = true;

        std::stable_sort(file_list_.begin(), file_list_.end(), PositiveSequence);

        save_log_box_->ClearWidget();
        save_log_box_->AddWidget(file_list_);
    }
}

void SaveLogWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    save_log_box_->SetScale(scale_w, scale_h);

    if(!keyboard_->isHidden())
    {
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
    }
    keyboard_->SetScale(scale_w, scale_h);
}

void SaveLogWidget::TogglePages(int row)
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

        save_log_box_->ClearWidget();

        save_log_box_->AddWidget(file_list_);

        tips_label_->setText(tr("另存为 - ") + path_); //更新路径提示标签
    }
    else
    {
        qDebug() << "文件";
    }
}

void SaveLogWidget::LastPages()
{
    QStringList list = path_.split("/");
    if(list.count() <= 1)
        return;
    path_.clear();
    for(int i = 0; i < list.count() - 1; i++) //去除掉最后一个
    {
        if(list.at(i).isEmpty())
            continue;
        path_.append("/" + list.at(i));
    }
    if(path_.isEmpty())
    {
        path_ = "/";
    }
    FindFiles(path_);
    save_log_box_->ClearWidget();
    save_log_box_->AddWidget(file_list_);

    tips_label_->setText(tr("另存为 - ") + path_); //更新路径提示标签
}

void SaveLogWidget::ReturnHome()
{
    path_ = home_path_;

    FindFiles(path_);
    save_log_box_->ClearWidget();
    save_log_box_->AddWidget(file_list_);

    tips_label_->setText(tr("另存为 - ") + path_); //更新路径提示标签
}

void SaveLogWidget::SaveClicked()
{
    if(name_edit_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入文件名"));
        return;
    }

    auto dir = qApp->applicationDirPath() + "/data/RAPID/Log/log";
    auto path = path_ + "/" + name_edit_->text();

    QFile::copy(dir, path);
}

void SaveLogWidget::ShowKeyboard()
{
    keyboard_->resize(this->size());
    keyboard_->move(0, 0);
    keyboard_->show();
    keyboard_->Init(name_edit_->text());
}

void SaveLogWidget::KeyRetData(QString data)
{
    if(data.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入文件名"));
        return;
    }
    name_edit_->setText(data);

    keyboard_->hide();
}

SaveLogBox::SaveLogBox(QWidget *parent) : ListBoxObject(parent)
{
}

void SaveLogBox::AddWidget(QString data)
{
    if(data.isEmpty())
        return;
    QString name = data.section(" ", 0, 0); //获取用户名
    QString type = data.section(" ", 1, 1); //获取类型

    SLBWidget *w = new SLBWidget(name, type);
    widget_list_.append(w);
    ListBoxObject::AddWidget(w);

    connect(this, &SaveLogBox::SendScale, w, &SLBWidget::SetScale);
}

void SaveLogBox::AddWidget(QStringList data)
{
    if(data.isEmpty())
        return;
    for(auto str : data)
    {
        QString name = str.section(" ", 0, 0); //获取用户名
        QString type = str.section(" ", 1, 1); //获取类型

        SLBWidget *w = new SLBWidget(name, type);
        widget_list_.append(w);
        ListBoxObject::AddWidget(w);
        connect(this, &SaveLogBox::SendScale, w, &SLBWidget::SetScale);
    }
}

void SaveLogBox::ClearWidget()
{
    this->clear(); //将列表清空

    widget_list_.clear();
}

void SaveLogBox::SetScale(double scale_w, double scale_h)
{
    ListBoxObject::SetScale(scale_w, scale_h);

    emit SendScale(scale_w, scale_h);
}

SLBWidget::SLBWidget(const QString &name, const QString &type, QWidget *parent) : QWidget(parent)
{
    ico_label_ = new QLabel(this);
    name_label_ = new QLabel(name, this);
    type_label_ = new QLabel(this);
    if(type == "dir")
        type_label_->setText(tr("文件夹"));
    else
        type_label_->setText(type);

    scale_w_ = 1;
    scale_h_ = 1;

    ico_label_->resize(24*scale_w_, 32*scale_h_);
    name_label_->resize(278*scale_w_, 32*scale_h_);
    type_label_->resize(301*scale_w_, 32*scale_h_);

    ico_label_->move(0, 0);
    name_label_->move(ico_label_->x() + ico_label_->width(), ico_label_->y());
    type_label_->move(name_label_->x() + name_label_->width() + 10*scale_w_, name_label_->y());

    this->resize(ico_label_->width() + name_label_->width() + type_label_->width() + 10, ico_label_->height());
}

void SLBWidget::SetScale(double scale_w, double scale_h)
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

ShowLogInfo::ShowLogInfo(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_label_ = new QLabel(tr("事件日志 - 事件消息"), this);

    data_widget_ = new QFrame(this);
    auto *data_layout = new QVBoxLayout(data_widget_);

    auto *label_layout = new QHBoxLayout;
    type_label_ = new QLabel(data_widget_);
    date_label_ = new QLabel(data_widget_);
    label_layout->addWidget(type_label_);
    label_layout->addStretch();
    label_layout->addWidget(date_label_);

    text_data_ = new TextBrowserObject(data_widget_);
    data_layout->addLayout(label_layout);
    data_layout->addWidget(text_data_);
    data_layout->setContentsMargins(0, 0, 0, 0);
    data_layout->setSpacing(0);
    data_layout->setStretchFactor(label_layout, 1);
    data_layout->setStretchFactor(text_data_, 15);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    lower_button_ = new QPushButton(tr("下一个"), button_widget_);
    upper_button_ = new QPushButton(tr("上一个"), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    lower_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    upper_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lower_button_->setFlat(true);
    upper_button_->setFlat(true);
    define_button_->setFlat(true);
    button_layout->addStretch();
    button_layout->addWidget(lower_button_);
    button_layout->addWidget(upper_button_);
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(title_label_);
    layout->addWidget(data_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_label_, 1);
    layout->setStretchFactor(data_widget_, 10);
    layout->setStretchFactor(button_widget_, 2);

    data_widget_->setFrameShape(QFrame::Box);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void ShowLogInfo::Init(int row, QString type, QString data, QString time, QString info)
{
    curr_row_ = row;
    text_data_->clear();

    type_label_->setText(type);
    date_label_->setText(time);

    text_data_->setText(data);
    text_data_->append(info);
}

void ShowLogInfo::SetScale(double scale_w, double scale_h)
{
    text_data_->SetScale(scale_w, scale_h);
}

void ShowLogInfo::ConnectInit()
{
    connect(lower_button_, &QPushButton::clicked,
            [=]()
    {
        emit Lower(curr_row_);
    });

    connect(upper_button_, &QPushButton::clicked,
            [=]()
    {
        emit Upper(curr_row_);
    });

    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        RobotClient::initance()->setErrorClear(); //清除错误
        emit Close();
    });
}
