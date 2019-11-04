#include "control.h"
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QDebug>

Appearance::Appearance(QWidget *parent, int type) : WindowObject(type, parent)
{
    QPixmap p(":/image/Image/MainWidget/Appearance.png");
    this->GetIcoLabel()->setPixmap(p);
    this->GetTitleLabel()->setText(tr("控制面板 - 外观"));

    QString group_qss = "QGroupBox {border-color: rgb(156, 156, 156);border-width: 1px;border-style: solid;margin-top: 0.5ex;}";            //这两行qss语句的作用是
    group_qss += "QGroupBox::title {subcontrol-origin: margin;subcontrol-position: top left;left: 10px;margin-left: 2px;padding: 0  0px;}"; //使QGroupBox拥有边框

    light_group_ = new QGroupBox(main_widget_);
    min_label_ = new QLabel(tr("最小"), light_group_);
    max_label_ = new QLabel(tr("最大"), light_group_);
    light_slider_ = new QSlider(light_group_);
    add_button_ = new QPushButton("+", light_group_);
    red_button_ = new QPushButton("-", light_group_);
    default_button_ = new QPushButton(tr("默认设置"), button_widget_);
    right_button_ = new QPushButton(tr("向右旋转"), button_widget_);
    confirm_button_ = new QPushButton(tr("确认"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);

    light_group_->setTitle(tr("亮度"));
    light_group_->setStyleSheet(group_qss);
    add_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    red_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    default_button_->setFlat(true);
    right_button_->setFlat(true);
    confirm_button_->setFlat(true);
    cancel_button_->setFlat(true);
    light_slider_->setTickPosition(QSlider::TicksBelow);
}

void Appearance::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    light_group_->resize(280*ScaleW(), 184*ScaleH());
    min_label_->resize(31*ScaleW(), 20*ScaleH());
    max_label_->resize(min_label_->size());
    light_slider_->resize(260*ScaleW(), 37*ScaleH());
    add_button_->resize(48*ScaleW(), 48*ScaleH());
    red_button_->resize(add_button_->size());
    default_button_->resize(126*ScaleW(), button_widget_->height());
    right_button_->resize(default_button_->size());
    confirm_button_->resize(right_button_->size());
    cancel_button_->resize(confirm_button_->size());

    light_group_->move(203*ScaleW(), 83*ScaleH());
    min_label_->move(18*ScaleW(), 45*ScaleH());
    max_label_->move(min_label_->x() + min_label_->width() + 194*ScaleW(), min_label_->y());
    light_slider_->move(24*ScaleW(), 63*ScaleH());
    red_button_->move(light_slider_->x(), light_slider_->y() + light_slider_->height() + 18*ScaleH());
    add_button_->move(red_button_->x() + red_button_->width() + 101*ScaleW(), red_button_->y());
    default_button_->move(0, 0);
    right_button_->move(default_button_->x() + default_button_->width(), default_button_->y());
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    confirm_button_->move(cancel_button_->x() - confirm_button_->width(), cancel_button_->y());
}

void Appearance::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

Monitor::Monitor(QWidget *parent, int type) : WindowObject(type, parent)
{
    QPixmap p(":/image/Image/MainWidget/Monitor.png");
    this->GetIcoLabel()->setPixmap(p);
    this->GetTitleLabel()->setText(tr("控制面板 - 监控"));

    task_frame_ = new QFrame(main_widget_);
    tips_task_label_ = new QLabel(tr("任务"), task_frame_);
    task_box_ = new QComboBox(task_frame_);
    tips_path_label_ = new QLabel(tr("路径监控"), task_frame_);
    path_shut_button_ = new QRadioButton(tr("关"), task_frame_);
    path_open_button_ = new QRadioButton(tr("开"), task_frame_);
    tips_sens_label_ = new QLabel(tr("灵敏度"), task_frame_);
    sensitivity_edit_ = new QLineEdit(task_frame_);
    tips_manual_label_ = new QLabel(tr("手动操纵监视"), task_frame_);
    manual_shut_button_ = new QRadioButton(tr("关"), task_frame_);
    manual_open_button_ = new QRadioButton(tr("开"), task_frame_);
    tips_sens2_label_ = new QLabel(tr("灵敏度"), task_frame_);
    sensitivity2_edit_ = new QLineEdit(task_frame_);

    set_up_frame_ = new QFrame(main_widget_);
    tips_setup_label_ = new QLabel(tr("执行设置"), set_up_frame_);
    tips_label_ = new QLabel(tr("无动作执行项"), set_up_frame_);
    setup_open_button_ = new QRadioButton(tr("开"), set_up_frame_);
    setup_shut_button_ = new QRadioButton(tr("关"), set_up_frame_);

    refresh_button_ = new QPushButton(tr("刷新"), button_widget_);
    confirm_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);

    task_frame_->setFrameShape(QFrame::Box);
    set_up_frame_->setFrameShape(QFrame::Box);

    refresh_button_->setFlat(true);
    confirm_button_->setFlat(true);
    cancel_button_->setFlat(true);
}

void Monitor::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    task_frame_->resize(414*ScaleW(), 287*ScaleH());
    tips_task_label_->resize(321*ScaleW(), 20*ScaleH());
    task_box_->resize(374*ScaleW(), 26*ScaleH());
    tips_path_label_->resize(tips_task_label_->size());
    path_shut_button_->resize(58*ScaleW(), 32*ScaleH());
    path_open_button_->resize(path_shut_button_->size());
    tips_sens_label_->resize(tips_path_label_->size());
    sensitivity_edit_->resize(71*ScaleW(), 25*ScaleH());
    tips_manual_label_->resize(tips_path_label_->size());
    manual_shut_button_->resize(path_shut_button_->size());
    manual_open_button_->resize(manual_shut_button_->size());
    tips_sens2_label_->resize(tips_sens_label_->size());
    sensitivity2_edit_->resize(sensitivity_edit_->size());

    set_up_frame_->resize(204*ScaleW(), 287*ScaleH());
    tips_setup_label_->resize(tips_path_label_->size());
    tips_label_->resize(tips_setup_label_->size());

    refresh_button_->resize(126*ScaleW(), button_widget_->height());
    confirm_button_->resize(refresh_button_->size());
    cancel_button_->resize(confirm_button_->size());

    task_frame_->move(7*ScaleW(), 14*ScaleH());
    tips_task_label_->move(17*ScaleW(), 13*ScaleH());
    task_box_->move(tips_task_label_->x(), tips_task_label_->y() + tips_task_label_->height());
    tips_path_label_->move(task_box_->x(), task_box_->y() + task_box_->height() + 5*ScaleH());
    path_shut_button_->move(tips_path_label_->x() + 16*ScaleW(), tips_path_label_->y() + tips_path_label_->height());
    path_open_button_->move(path_shut_button_->x(), path_shut_button_->y() + path_shut_button_->height());
    tips_sens_label_->move(path_shut_button_->x() + path_shut_button_->width(), path_shut_button_->y() + 5*ScaleH());
    sensitivity_edit_->move(tips_sens_label_->x() + 14*ScaleW(), tips_sens_label_->y() + tips_sens_label_->height());
    tips_manual_label_->move(tips_path_label_->x(), path_open_button_->y() + path_open_button_->height());
    manual_shut_button_->move(path_shut_button_->x(), tips_manual_label_->y() + tips_manual_label_->height());
    manual_open_button_->move(manual_shut_button_->x(), manual_shut_button_->y() + manual_shut_button_->height());
    tips_sens2_label_->move(tips_sens_label_->x(), manual_shut_button_->y() + 4*ScaleH());
    sensitivity2_edit_->move(sensitivity_edit_->x(), manual_open_button_->y());

    set_up_frame_->move(task_frame_->x() + task_frame_->width() + 6*ScaleW(), task_frame_->y());
    tips_setup_label_->move(7*ScaleW(), 14*ScaleH());
    tips_label_->move(tips_setup_label_->x() + 8*ScaleW(), tips_setup_label_->y() + tips_setup_label_->height());
    setup_shut_button_->move(tips_label_->x(), tips_label_->y() + tips_label_->height());
    setup_open_button_->move(setup_shut_button_->x() + setup_shut_button_->width() + 10*ScaleW(), setup_shut_button_->y());

    refresh_button_->move(0, 0);
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    confirm_button_->move(cancel_button_->x() - confirm_button_->width(), cancel_button_->y());
}

void Monitor::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

FlexPendant::FlexPendant(QWidget *parent, int type) : WindowObject(type, parent)
{
    QPixmap p(":/image/Image/MainWidget/FlexPendant.png");
    this->GetIcoLabel()->setPixmap(p);
    this->GetTitleLabel()->setText(tr("控制面板 - FlexPendant"));

    tips_label_ = new QLabel(tr("FlexPendant 配置属性:"), main_widget_);
    tips2_label_ = new QLabel(tr("点击待编辑的属性"), main_widget_);
    attribute_label_ = new QLabel(tr("属性"), main_widget_);
    listbox_ = new ListBoxObject(main_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(192, 192, 192));
    attribute_label_->setAutoFillBackground(true);
    attribute_label_->setPalette(palette);

    attribute_label_->setFrameShape(QFrame::Box);
    close_button_->setFlat(true);
}

void FlexPendant::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips_label_->resize(main_widget_->width(), 73*ScaleH());
    tips2_label_->resize(main_widget_->width(), 27*ScaleH());
    attribute_label_->resize(main_widget_->width(), 26*ScaleH());
    listbox_->resize(main_widget_->width(), main_widget_->height() - tips_label_->height() - tips2_label_->height() - attribute_label_->height());
    close_button_->resize(126*ScaleW(), button_widget_->height());

    tips_label_->move(10*ScaleW(), 0);
    tips2_label_->move(tips_label_->x(), tips_label_->y() + tips_label_->height());
    attribute_label_->move(0, tips2_label_->y() + tips2_label_->height());
    listbox_->move(attribute_label_->x(), attribute_label_->y() + attribute_label_->height());
    close_button_->move(button_widget_->width() - close_button_->width(), 0);
}

void FlexPendant::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

InputOut::InputOut(QWidget *parent, int type) : WindowObject(type, parent)
{
    QPixmap p(":/image/Image/MainWidget/InputOut.png");
    this->GetIcoLabel()->setPixmap(p);
    this->GetTitleLabel()->setText(tr("控制面板 - I/O"));

    tips_label_ = new QLabel(tr("I/O 参数选择"), main_widget_);
    tips2_label_ = new QLabel(tr("常用 I/O 信号\n所选项目已列入常用列表。"), main_widget_);
    name_label_ = new QLabel(tr("名称"), main_widget_);
    type_label_ = new QLabel(tr("类型"), main_widget_);
    listbox_ = new ListBoxObject(main_widget_);
    whole_button_ = new QPushButton(tr("全部"), button_widget_);
    nothing_button_ = new QPushButton(tr("无"), button_widget_);
    preview_button_ = new QPushButton(tr("预览"), button_widget_);
    app_button_ = new QPushButton(tr("应用"), button_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);

    tips_label_->setFrameShape(QFrame::StyledPanel);
    name_label_->setFrameShape(QFrame::Box);
    type_label_->setFrameShape(QFrame::Box);
    tips_label_->setAlignment(Qt::AlignCenter); //设置文字水平居中

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(192, 192, 192));
    name_label_->setAutoFillBackground(true);
    type_label_->setAutoFillBackground(true);
    name_label_->setPalette(palette);
    type_label_->setPalette(palette);

    whole_button_->setFlat(true);
    nothing_button_->setFlat(true);
    preview_button_->setFlat(true);
    app_button_->setFlat(true);
    close_button_->setFlat(true);
}

void InputOut::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips_label_->resize(main_widget_->width(), 40*ScaleH());
    tips2_label_->resize(main_widget_->width(), 50*ScaleH());
    name_label_->resize(main_widget_->width()/2, 25*ScaleH());
    type_label_->resize(name_label_->size());
    listbox_->resize(main_widget_->width(), main_widget_->height() - tips2_label_->height() - tips_label_->height() - name_label_->height());
    whole_button_->resize(126*ScaleW(), button_widget_->height());
    nothing_button_->resize(whole_button_->size());
    preview_button_->resize(nothing_button_->size());
    app_button_->resize(preview_button_->size());
    close_button_->resize(app_button_->size());

    tips_label_->move(0, 0);
    tips2_label_->move(tips_label_->x(), tips_label_->y() + tips_label_->height());
    name_label_->move(tips2_label_->x(), tips2_label_->y() + tips2_label_->height());
    type_label_->move(name_label_->x() + name_label_->width(), name_label_->y());
    listbox_->move(name_label_->x(), name_label_->y() + name_label_->height());
    whole_button_->move(0, 0);
    nothing_button_->move(whole_button_->x() + whole_button_->width(), whole_button_->y());
    preview_button_->move(nothing_button_->x() + nothing_button_->width(), nothing_button_->y());
    app_button_->move(preview_button_->x() + preview_button_->width(), preview_button_->y());
    close_button_->move(app_button_->x() + app_button_->width(), app_button_->y());
}

void InputOut::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

Language::Language(QWidget *parent, int type) : WindowObject(type, parent)
{
    QPixmap p(":/image/Image/MainWidget/Language.png");
    this->GetIcoLabel()->setPixmap(p);
    this->GetTitleLabel()->setText(tr("控制面板 - 语言"));

    tips_label_ = new QLabel(tr("当前语言:"), main_widget_);
    lang_ico_label_ = new QLabel(main_widget_);
    lang_label_ = new QLabel(main_widget_);
    lang_button_ = new QLabel(tr("已安装语言"), main_widget_);
    lang_listbox_ = new ListBoxObject(main_widget_);
    confirm_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);

    lang_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    tips_label_->setAlignment(Qt::AlignCenter);
    lang_button_->setFrameShape(QFrame::Box);
    confirm_button_->setFlat(true);
    cancel_button_->setFlat(true);

    lang_listbox_->setResizeMode(QListView::Adjust);

    ReadLanguage();

    connect(confirm_button_, &QPushButton::clicked, this, &Language::DefineClicked);
}

void Language::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips_label_->resize(150*ScaleW(), 100*ScaleH());
    lang_ico_label_->resize(32*ScaleW(), 22*ScaleH());
    lang_label_->resize(50*ScaleW(), lang_ico_label_->height());
    lang_button_->resize(main_widget_->width(), 25*ScaleH());
    lang_listbox_->resize(main_widget_->width(), main_widget_->height() - tips_label_->height() - lang_button_->height());
    confirm_button_->resize(126*ScaleW(), button_widget_->height());
    cancel_button_->resize(confirm_button_->size());

    tips_label_->move(0, 0);
    lang_ico_label_->move(tips_label_->x() + tips_label_->width(), tips_label_->y() + 40*ScaleH());
    lang_label_->move(lang_ico_label_->x() + lang_ico_label_->width() + 10*ScaleW(), lang_ico_label_->y());
    lang_button_->move(tips_label_->x(), tips_label_->y() + tips_label_->height());
    lang_listbox_->move(lang_button_->x(), lang_button_->y() + lang_button_->height());
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    confirm_button_->move(cancel_button_->x() - cancel_button_->width(), cancel_button_->y());
}

void Language::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

void Language::DefineClicked()
{
    auto *item = lang_listbox_->currentItem();
    if(item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择语言"));
        return;
    }

    auto path = qApp->applicationDirPath() + "/data/RAPID/Language/language.ini";

    QSettings st(path, QSettings::IniFormat);
    st.setValue("language/current", item->text());

    QMessageBox::about(this, tr("提示"), tr("设置成功，请重启示教器生效"));

    cancel_button_->click();
}

void Language::ReadLanguage()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/Language/";
    auto qm_path = path + "qm/";
    auto ico_path = path + "ico/";

    QDir qm_dir(qm_path);
    qm_dir.setFilter(QDir::Files);
    auto qm_list = qm_dir.entryList();
    for(auto &list : qm_list)
    {
        auto name = list.mid(0, list.lastIndexOf("."));
        auto ico = ico_path + name + ".png";

        lang_listbox_->AddText(ico, name);
    }
    QSettings st(path + "language.ini", QSettings::IniFormat);
    auto current_name = st.value("/language/current").toString();

    auto qss = "border-image: url(" + ico_path + current_name + ".png" + ")";

    lang_ico_label_->setStyleSheet(qss);
    lang_label_->setText(current_name);
}

ProgKeys::ProgKeys(QWidget *parent, int type) : WindowObject(type, parent)
{
    QPixmap p(":/image/Image/MainWidget/ProgKeys.png");
    this->GetIcoLabel()->setPixmap(p);
    this->GetTitleLabel()->setText(tr("控制面板 - ProgKeys"));

    tab_widget_ = new QTabWidget(main_widget_);
    confirm_button_ = new QPushButton(tr("确认"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    prog_key1_ = new PKWidget(0, tab_widget_, this);
    prog_key2_ = new PKWidget(1, tab_widget_, this);
    prog_key3_ = new PKWidget(2, tab_widget_, this);
    prog_key4_ = new PKWidget(3, tab_widget_, this);

    confirm_button_->setFlat(true);
    cancel_button_->setFlat(true);

    tab_widget_->addTab(prog_key1_, QIcon(":/image/Image/MainWidget/prog_keys1.png"), tr("按键 1\n无"));
    tab_widget_->addTab(prog_key2_, QIcon(":/image/Image/MainWidget/prog_keys2.png"), tr("按键 2\n无"));
    tab_widget_->addTab(prog_key3_, QIcon(":/image/Image/MainWidget/prog_keys3.png"), tr("按键 3\n无"));
    tab_widget_->addTab(prog_key4_, QIcon(":/image/Image/MainWidget/prog_keys4.png"
                                          ""), tr("按键 4\n无"));

    connect(confirm_button_, &QPushButton::clicked,
            [=]()
    {
        prog_key1_->DefineClicked();
        prog_key2_->DefineClicked();
        prog_key3_->DefineClicked();
        prog_key4_->DefineClicked();
        TeachState::instance()->SaveProgKeyInfo();

        cancel_button_->click();
    });
}

void ProgKeys::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tab_widget_->resize(main_widget_->size());
    confirm_button_->resize(126*ScaleW(), button_widget_->height());
    cancel_button_->resize(126*ScaleW(), button_widget_->height());

    tab_widget_->move(0, 0);
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    confirm_button_->move(cancel_button_->x() - cancel_button_->width(), cancel_button_->y());
}

void ProgKeys::showEvent(QShowEvent *)
{
    int width = tab_widget_->width();
    int tab_count = tab_widget_->count();
    int tab_width = width/tab_count;
    tab_widget_->setStyleSheet(QString("QTabBar::tab{width:%1px; min-height: 40px;}").arg(tab_width));

    prog_key1_->Init();
    prog_key2_->Init();
    prog_key3_->Init();
    prog_key4_->Init();
    tab_widget_->setCurrentIndex(0);
}

void ProgKeys::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
    showEvent(nullptr);

    prog_key1_->SetScale(scale_w, scale_h);
    prog_key2_->SetScale(scale_w, scale_h);
    prog_key3_->SetScale(scale_w, scale_h);
    prog_key4_->SetScale(scale_w, scale_h);
}

TouchScreen::TouchScreen(QWidget *parent, int type) : WindowObject(type, parent)
{
    QPixmap p(":/image/Image/MainWidget/TouchScreen.png");
    this->GetIcoLabel()->setPixmap(p);
    this->GetTitleLabel()->setText(tr("控制面板 - 触摸屏"));

    tips_label_ = new QLabel(tr("如果触摸屏无法正常响应您的击键，就需要重校屏幕。\n要开始校准，请点击“重校”。"), main_widget_);
    re_cali_button_ = new QPushButton(tr("重校"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);

    re_cali_button_->setFlat(true);
    cancel_button_->setFlat(true);
}

void TouchScreen::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips_label_->resize(417*ScaleW(), 84*ScaleH());
    re_cali_button_->resize(126*ScaleW(), button_widget_->height());
    cancel_button_->resize(re_cali_button_->size());

    tips_label_->move(97*ScaleW(), 87*ScaleH());
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    re_cali_button_->move(cancel_button_->x() - re_cali_button_->width(), cancel_button_->y());
}

void TouchScreen::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

PKWidget::PKWidget(int num, QTabWidget *tabwidget, QWidget *parent) : QWidget(parent)
{
    num_ = num;
    tab_widget_ = tabwidget;

    type_label_ = new QLabel(tr("类型"), this);
    type_box_ = new QComboBox(this);
    press_label_ = new QLabel(tr("按下按键"), this);
    press_output_box_ = new QComboBox(this);
    press_system_box_ = new QComboBox(this);
    allow_label_ = new QLabel(tr("允许自动模式"), this);
    allow_box_ = new QComboBox(this);
    output_label_ = new QLabel(tr("数字输出:"), this);
    output_frame_ = new TableBox(0, 1, this);
    input_label_ = new QLabel(tr("数字输入"), this);
    input_frame_ = new TableBox(0, 1, this);

    scale_w_ = 1;
    scale_h_ = 1;

    output_frame_->setFrameShape(QFrame::Box);
    input_frame_->setFrameShape(QFrame::Box);

    type_box_->addItem(tr("无"));
    type_box_->addItem(tr("输入"));
    type_box_->addItem(tr("输出"));
    type_box_->addItem(tr("系统"));

    press_output_box_->addItem(tr("切换"));
    press_output_box_->addItem(tr("设为 1"));
    press_output_box_->addItem(tr("设为 0"));
    press_output_box_->addItem(tr("按下/松开"));
    press_output_box_->addItem(tr("脉冲"));

    press_system_box_->addItem(tr("PP 移至主程序"));

    allow_box_->addItem(tr("否"));
    allow_box_->addItem(tr("是"));

    if(num_ == 0)
        key_name_ = ProgKeysName::PK1;
    else if(num_ == 1)
        key_name_ = ProgKeysName::PK2;
    else if(num == 2)
        key_name_ = ProgKeysName::PK3;
    else
        key_name_ = ProgKeysName::PK4;

    output_frame_->setSelectionBehavior(QAbstractItemView::SelectRows);
    output_frame_->setSelectionMode(QAbstractItemView::SingleSelection);

    input_frame_->setSelectionBehavior(QAbstractItemView::SelectRows);
    input_frame_->setSelectionMode(QAbstractItemView::SingleSelection);

    for(int i = 0; i < 16; i++)
    {
        QString di_name = "DI" + QString::number(i + 1);
        QString do_name = "DO" + QString::number(i + 1);

        input_frame_->AddText(i, 0, di_name);
        output_frame_->AddText(i, 0, do_name);
    }

    int_item_ = nullptr;
    out_item_ = nullptr;

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    ConnectInit();
}

void PKWidget::resizeEvent(QResizeEvent *)
{
    type_label_->resize(249*scale_w_, 33*scale_h_);
    type_box_->resize(type_label_->size());
    press_label_->resize(type_box_->size());
    press_output_box_->resize(press_label_->size());
    press_system_box_->resize(press_output_box_->size());
    allow_label_->resize(press_system_box_->size());
    allow_box_->resize(allow_label_->size());
    output_label_->resize(310*scale_w_, type_label_->height());
    output_frame_->resize(310*scale_w_, 210*scale_h_);
    input_label_->resize(output_label_->size());
    input_frame_->resize(output_frame_->size());

    type_label_->move(15*scale_w_, 0);
    type_box_->move(type_label_->x(), type_label_->y() + type_label_->height());
    if(!press_label_->isHidden())
    {
        press_label_->move(type_box_->x(), type_box_->y() + type_box_->height());
        if(!press_output_box_->isHidden())
        {
            press_output_box_->move(press_label_->x(), press_label_->y() + press_label_->height());
            output_label_->move(type_label_->x() + type_label_->width() + 36*scale_w_, type_label_->y());
            output_frame_->move(output_label_->x(), type_box_->y());
        }
        if(!press_system_box_->isHidden())
        {
            press_system_box_->move(press_label_->x(), press_label_->y() + press_label_->height());
        }
    }
    if(!input_label_->isHidden())
    {
        input_label_->move(type_label_->x() + type_label_->width() + 36*scale_w_, type_label_->y());
        input_frame_->move(input_label_->x(), input_label_->y() + input_label_->height());
    }
    if(!allow_label_->isHidden())
    {
        allow_label_->move(type_box_->x(), type_box_->y() + type_box_->height() + 72*scale_h_);
        allow_box_->move(allow_label_->x(), allow_label_->y() + allow_label_->height());
    }
}

void PKWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    resizeEvent(nullptr);
}

void PKWidget::DefineClicked()
{
    int type = type_box_->currentIndex();
    int keys = -1;
    int isauto = -1;
    QString select = "-1";

    if(type == 1)
    {
        isauto = allow_box_->currentIndex();

        if(int_item_ != nullptr)
            select = int_item_->text();
    }
    else if(type == 2)
    {
        keys = press_output_box_->currentIndex();
        isauto = allow_box_->currentIndex();

        if(out_item_ != nullptr)
            select = out_item_->text();
    }
    else if(type == 3)
    {
        keys = press_system_box_->currentIndex();
        isauto = allow_box_->currentIndex();
    }

    TeachState::instance()->SetProgKeyInfo(key_name_, type, keys, isauto, select);
}

void PKWidget::TypeBoxClicked(int num)
{
    press_label_->hide();
    allow_label_->hide();
    press_output_box_->hide();
    press_system_box_->hide();
    allow_box_->hide();
    output_label_->hide();
    output_frame_->hide();
    input_label_->hide();
    input_frame_->hide();

    QString text = tab_widget_->tabText(num_).section("\n", 0, 0) + "\n";

    switch(num)
    {
    case 0: //无
    {
        tab_widget_->setTabText(num_, text + tr("无"));
        break;
    }
    case 1: //输入
    {
        allow_label_->show();
        allow_box_->show();
        input_label_->show();
        input_frame_->show();
        tab_widget_->setTabText(num_, text + tr("输入"));
        break;
    }
    case 2: //输出
    {
        press_label_->show();
        press_output_box_->show();
        allow_label_->show();
        allow_box_->show();
        output_frame_->show();
        output_label_->show();
        tab_widget_->setTabText(num_, text + tr("输出"));
        break;
    }
    case 3: //系统
    {
        press_label_->show();
        press_system_box_->show();
        allow_label_->show();
        allow_box_->show();
        tab_widget_->setTabText(num_, text + tr("系统"));
        break;
    }
    }
    resizeEvent(nullptr);
}

void PKWidget::OutItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    if(out_item_ == item)
    {
        output_frame_->clearSelection();
        out_item_ = nullptr;
    }
    else
    {
        out_item_ = item;
    }
}

void PKWidget::IntItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    if(int_item_ == item)
    {
        input_frame_->clearSelection();
        int_item_ = nullptr;
    }
    else
    {
        int_item_ = item;
    }
}

void PKWidget::Init()
{
    type_box_->setCurrentIndex(0);
    press_output_box_->setCurrentIndex(0);
    press_system_box_->setCurrentIndex(0);
    output_frame_->clearSelection();
    input_frame_->clearSelection();
    out_item_ = nullptr;
    int_item_ = nullptr;

    int type = TeachState::instance()->GetPKType(key_name_);
    int keys = TeachState::instance()->GetPKKeys(key_name_);
    int isauto = TeachState::instance()->GetPKAuto(key_name_);
    auto select = TeachState::instance()->GetPKSelect(key_name_);

    type_box_->setCurrentIndex(type);
    TypeBoxClicked(type);

    switch(type)
    {
    case 1:
    {
        allow_box_->setCurrentIndex(isauto);

        bool isselect = false;
        for(int i = 0; i < input_frame_->GetRow(); i++)
        {
            auto *item = input_frame_->item(i, 0);

            if(select == item->text())
            {
                input_frame_->selectRow(i);
                int_item_ = item;
                isselect = true;
                break;
            }
        }
        if(!isselect)
        {
            input_frame_->clearSelection();
            int_item_ = nullptr;
        }

        break;
    }
    case 2:
    {
        press_output_box_->setCurrentIndex(keys);
        allow_box_->setCurrentIndex(isauto);

        bool isselect = false;
        for(int i = 0; i < output_frame_->GetRow(); i++)
        {
            auto *item = output_frame_->item(i, 0);

            if(select == item->text())
            {
                output_frame_->selectRow(i);
                out_item_ = item;
                isselect = true;
                break;
            }
        }
        if(!isselect)
        {
            output_frame_->clearSelection();
            out_item_ = nullptr;
        }

        break;
    }
    case 3:
    {
        press_system_box_->setCurrentIndex(keys);
        allow_box_->setCurrentIndex(isauto);
        break;
    }
    default:
    {
        break;
    }
    }
}

void PKWidget::ConnectInit()
{
    connect(type_box_, QOverload<int>::of(&QComboBox::activated), this, &PKWidget::TypeBoxClicked);

    connect(input_frame_, &TableBox::itemClicked, this, &PKWidget::IntItemClicked);

    connect(output_frame_, &TableBox::itemClicked, this, &PKWidget::OutItemClicked);
}

Dispose::Dispose(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title_layout = new QHBoxLayout;
    ico_label_ = new QLabel(this);
    title_label_ = new QLabel(tr("控制面板 - 配置"), this);
    title_layout->addWidget(ico_label_);
    title_layout->addWidget(title_label_);
    title_layout->setContentsMargins(0, 0, 0, 0);
    title_layout->setSpacing(0);
    title_layout->setStretchFactor(ico_label_, 1);
    title_layout->setStretchFactor(title_label_, 15);

    head_widget_ = new QFrame(this);
    auto head_layout = new QGridLayout(head_widget_);
    tips1_label_ = new QLabel(tr("每个主题都包含用于配置系统的不同类型。"), head_widget_);
    tips2_label_ = new QLabel(tr("当前主题:"), head_widget_);
    theme_label_ = new QLabel("I/O", head_widget_);
    tips3_label_ = new QLabel(tr("选择您需要查看的主题和实例类型。"), head_widget_);
    head_layout->addWidget(tips1_label_, 0, 0);
    head_layout->addWidget(tips2_label_, 1, 0);
    head_layout->addWidget(theme_label_, 1, 1);
    head_layout->addWidget(tips3_label_, 2, 0);

    list_widget_ = new QFrame(this);
    auto *list_layout = new QHBoxLayout(list_widget_);
    table_box_ = new TableBox(list_widget_);
    list_layout->addWidget(table_box_);
    list_layout->setContentsMargins(0, 0, 0, 0);
    list_layout->setSpacing(0);

    button_widget_ = new QWidget(this);
    auto button_layout = new QHBoxLayout(button_widget_);
    file_button_ = new ComboBoxObject(tr("文件"), button_widget_);
    theme_button_ = new ComboBoxObject(tr("主题"), button_widget_);
    show_button_ = new QPushButton(tr("显示全部"), button_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);
    file_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    theme_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    show_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    file_button_->setFlat(true);
    theme_button_->setFlat(true);
    show_button_->setFlat(true);
    close_button_->setFlat(true);
    button_layout->addWidget(file_button_);
    button_layout->addWidget(theme_button_);
    button_layout->addStretch();
    button_layout->addWidget(show_button_);
    button_layout->addWidget(close_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addLayout(title_layout);
    layout->addWidget(head_widget_);
    layout->addWidget(list_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_layout, 1);
    layout->setStretchFactor(head_widget_, 4);
    layout->setStretchFactor(list_widget_, 9);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);
    list_widget_->setFrameShape(QFrame::Box);

    ico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/Dispose.png)");

    load_action_ = new QAction(tr("加载参数..."), this);
    eio_save_action_ = new QAction(tr("'EIO' 另存为"), this);
    all_save_action_ = new QAction(tr("全部另存为..."), this);

    file_button_->AppAction(load_action_);
    file_button_->AppSeparator();
    file_button_->AppAction(eio_save_action_);
    file_button_->AppAction(all_save_action_);

    man_action_ = new QAction("Man-machine communication", this);
    con_action_ = new QAction("Controller", this);
    com_action_ = new QAction("Communication", this);
    mot_action_ = new QAction("Motion", this);
    io_action_ = new QAction("I/O", this);

    theme_button_->AppAction(man_action_);
    theme_button_->AppAction(con_action_);
    theme_button_->AppAction(com_action_);
    theme_button_->AppAction(mot_action_);
    theme_button_->AppAction(io_action_);

    load_action_->setEnabled(false);
    eio_save_action_->setEnabled(false);
    all_save_action_->setEnabled(false);
    man_action_->setEnabled(false);
    con_action_->setEnabled(false);
    com_action_->setEnabled(false);
    mot_action_->setEnabled(false);
    show_button_->setEnabled(false);

    io_select_window_ = new IOSelectWindow(this);

    io_select_window_->hide();

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

Dispose::~Dispose()
{
    WriteIOInfo();
}

void Dispose::SetScale(double scale_w, double scale_h)
{
    io_select_window_->resize(this->size());
    io_select_window_->move(0, 0);

    table_box_->SetScale(scale_w, scale_h);
    io_select_window_->SetScale(scale_w, scale_h);
}

void Dispose::ShowAllButton()
{
    if(select_name_.isEmpty())
    {
        show_button_->setEnabled(false);
        return;
    }
    if(!root_json_.contains(select_name_))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法寻找到该配置"));
        return;
    }

    auto select_json = root_json_.value(select_name_).toObject();

    io_select_window_->resize(this->size());
    io_select_window_->move(0, 0);
    io_select_window_->show();
    if(select_name_ == "Signal")
    {
        auto devices = root_json_.value("DeviceNet Device").toObject();
        QStringList dev_keys;
        dev_keys << "";
        dev_keys.append(devices.keys());
        dev_keys << "DN_Internal_Device";
        dev_keys.removeAll("devicetype€");
        dev_keys.removeAll("operate");
        io_select_window_->Init(std::move(select_json), select_name_, dev_keys);
    }
    else
    {
        io_select_window_->Init(std::move(select_json), select_name_);
    }
}

void Dispose::TableBoxClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    select_name_ = item->text();
    show_button_->setEnabled(true);
}

void Dispose::IOSelectClose(QJsonObject json, QString name)
{
    root_json_[name] = json;

    io_select_window_->hide();
}

void Dispose::ConnectInit()
{
    connect(close_button_, &QPushButton::clicked,
            [=]()
    {
        emit Close();
    });

    connect(show_button_, &QPushButton::clicked, this, &Dispose::ShowAllButton);

    connect(io_select_window_, &IOSelectWindow::Close, this, &Dispose::IOSelectClose);

    connect(table_box_, &TableBox::itemClicked, this, &Dispose::TableBoxClicked);
}

void Dispose::ReadIOInfo()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/IO/io.json";
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        throw ABBException("Dispose", "ReadIOInfo", "无法打开io配置文件");
    }
    auto data = file.readAll();

    auto json = QJsonDocument::fromJson(data);
    root_json_ = json.object();

    UpdateListBox(root_json_.keys());
}

void Dispose::WriteIOInfo()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/IO/io.json";
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument json;
    json.setObject(root_json_);

    file.write(json.toJson());
}

void Dispose::UpdateListBox(QStringList list_data)
{
    int rw = 0;
    for(int i = 0; i < list_data.size();)
    {

        if(i + 1 < list_data.size())
        {
            table_box_->AddText(rw, 0, list_data.at(i));
            table_box_->AddText(rw++, 1, list_data.at(i+1));
            i += 2;
        }
        else
        {
            table_box_->AddText(rw++, 0, list_data.at(i));
            i++;
        }
    }

    table_box_->clearSelection();
}

IOSelectWindow::IOSelectWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_label_ = new QLabel(tr("控制面板 - 配置 - I/O"), this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips_type_label_ = new QLabel(tr("目前类型:"), head_widget_);
    type_label_ = new QLabel(head_widget_);
    tips_label_ = new QLabel(tr("新增或从列表中选择一个进行编辑或删除"), head_widget_);
    head_layout->addWidget(tips_type_label_, 0, 0);
    head_layout->addWidget(type_label_, 0, 1);
    head_layout->addWidget(tips_label_, 1, 0);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    edit_button_ = new QPushButton(tr("编辑"), button_widget_);
    add_button_ = new QPushButton(tr("添加"), button_widget_);
    del_button_ = new QPushButton(tr("删除"), button_widget_);
    close_button_ = new QPushButton(tr("后退"), button_widget_);
    edit_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    add_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    del_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_button_->setFlat(true);
    add_button_->setFlat(true);
    del_button_->setFlat(true);
    close_button_->setFlat(true);
    button_layout->addWidget(edit_button_);
    button_layout->addWidget(add_button_);
    button_layout->addWidget(del_button_);
    button_layout->addStretch();
    button_layout->addWidget(close_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(title_label_);
    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_label_, 1);
    layout->setStretchFactor(head_widget_, 5);
    layout->setStretchFactor(table_box_, 12);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);

    io_edit_window_ = new IOEditAndAddWindow(this);
    device_edit_window_ = new DeviceNetEditWindow(this);
    device_add_window_ = new DeviceNetAddWindow(this);
    signal_window_ = new SignalAddEditWindow(this);

    io_edit_window_->hide();
    device_edit_window_->hide();
    device_add_window_->hide();
    signal_window_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void IOSelectWindow::Init(QJsonObject json, QString name, QStringList assigneds)
{
    qSwap(object_json_, json);

    type_label_->setText(name);
    assigneds_ = assigneds;

    auto keys = object_json_.keys();
    keys.removeAll("operate");
    keys.removeAll("devicetype€");

    UpdateTableBox(keys);

    edit_button_->setEnabled(false);
    del_button_->setEnabled(false);

    if((object_json_.value("operate") == false || keys.isEmpty()) && name != "DeviceNet Device")
    {
        add_button_->setEnabled(false);
    }
    else
    {
        add_button_->setEnabled(true);
    }
}

void IOSelectWindow::SetScale(double scale_w, double scale_h)
{
    io_edit_window_->resize(this->size());
    io_edit_window_->move(0, 0);
    device_edit_window_->resize(this->size());
    device_edit_window_->move(0, 0);
    device_add_window_->resize(this->size());
    device_add_window_->move(0, 0);
    signal_window_->resize(this->size());
    signal_window_->move(0, 0);

    table_box_->SetScale(scale_w, scale_h);
    io_edit_window_->SetScale(scale_w, scale_h);
    device_edit_window_->SetScale(scale_w, scale_h);
    device_add_window_->SetScale(scale_w, scale_h);
    signal_window_->SetScale(scale_w, scale_h);
}

void IOSelectWindow::TableBoxClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    select_name_ = item->text();

    auto json = object_json_.value(select_name_).toObject();

    edit_button_->setEnabled(true);
    if(json.value("operate") == false)
    {
        del_button_->setEnabled(false);
    }
    else
    {
        del_button_->setEnabled(true);
    }
}

void IOSelectWindow::DelJsonData()
{
    object_json_.remove(select_name_);
}

void IOSelectWindow::EditJsonData()
{
    if(type_label_->text() == "DeviceNet Device")
    {
        device_edit_window_->resize(this->size());
        device_edit_window_->move(0, 0);
        device_edit_window_->show();

        auto json = object_json_.value(select_name_).toObject();

        device_edit_window_->Init(qMove(json), select_name_);
    }
    else if(type_label_->text() == "Signal")
    {
        signal_window_->resize(this->size());
        signal_window_->move(0, 0);
        signal_window_->show();

        auto json = object_json_.value(select_name_).toObject();

        signal_window_->Init(qMove(json), assigneds_, select_name_);
    }
    else
    {
        io_edit_window_->resize(this->size());
        io_edit_window_->move(0, 0);
        io_edit_window_->show();

        auto json = object_json_.value(select_name_).toObject();

        io_edit_window_->Init(qMove(json), true);
    }
}

void IOSelectWindow::AddJsonData()
{
    if(type_label_->text() == "DeviceNet Device")
    {
        device_add_window_->resize(this->size());
        device_add_window_->move(0, 0);
        device_add_window_->show();
        device_add_window_->Init();
    }
    else if(type_label_->text() == "Signal")
    {
        signal_window_->resize(this->size());
        signal_window_->move(0, 0);
        signal_window_->show();

        signal_window_->Init(QJsonObject(), assigneds_, QString());
    }
    else
    {
        io_edit_window_->resize(this->size());
        io_edit_window_->move(0, 0);
        io_edit_window_->show();

        if(select_name_.isEmpty())
        {
            auto *item = table_box_->item(0, 0);
            if(item->text().isEmpty())
                return;
            auto json = object_json_.value(item->text()).toObject();

            io_edit_window_->Init(qMove(json), false);

            return;
        }

        auto json = object_json_.value(select_name_).toObject();

        io_edit_window_->Init(qMove(json), false);
    }
}

void IOSelectWindow::DefineEdAd(QJsonObject json, bool isedit)
{
    if(isedit)
        object_json_[json.value("0_Name").toString()] = json;
    else
        object_json_.insert(json.value("0_Name").toString(), json);

    io_edit_window_->hide();
}

void IOSelectWindow::CloseEdAd()
{
    io_edit_window_->hide();
}

void IOSelectWindow::DeviceAddDefine(QJsonObject json)
{
    auto name = json.value("0_Name").toString();
    if(object_json_.contains(name))
    {
        QMessageBox::warning(this, tr("提示"), tr("已存在"));
        return;
    }
    object_json_.insert(name, qMove(json));
    QMessageBox::about(this, tr("提示"), tr("添加成功，请重启以生效"));
    device_add_window_->hide();
}

void IOSelectWindow::DeviceAddCancel()
{
    device_add_window_->hide();
}

void IOSelectWindow::DeviceEditDefine(QJsonObject json, QString old_name)
{
    auto name = json.value("0_Name").toString();
    if(old_name != name)
    {
        if(object_json_.contains(name))
        {
            QMessageBox::warning(this, tr("提示"), tr("已存在"));
            return;
        }
        object_json_.remove(old_name);
        object_json_.insert(name, qMove(json));
    }
    else
    {
        object_json_[name] = json;
    }

    QMessageBox::about(this, tr("提示"), tr("修改成功，请重启以生效"));
    device_edit_window_->hide();
}

void IOSelectWindow::DeviceEditCancel()
{
    device_edit_window_->hide();
}

void IOSelectWindow::SignalDefine(QJsonObject json, QString old_name)
{
    if(old_name.isEmpty()) //添加
    {
        auto name = json.value("0_Name").toString();
        if(object_json_.contains(name))
        {
            QMessageBox::warning(this, tr("提示"), tr("已存在"));
            return;
        }
        object_json_.insert(name, qMove(json));
        QMessageBox::about(this, tr("提示"), tr("添加成功，请重启以生效"));
        signal_window_->hide();
    }
    else //编辑
    {
        if(json.value("operate") == false)
        {
            signal_window_->hide();
            return;
        }

        auto name = json.value("0_Name").toString();

        if(old_name != name)
        {
            if(object_json_.contains(name))
            {
                QMessageBox::warning(this, tr("提示"), tr("已存在"));
                return;
            }
            object_json_.remove(old_name);
            object_json_.insert(name, qMove(json));
        }
        else
        {
            object_json_[name] = json;
        }

        QMessageBox::about(this, tr("提示"), tr("修改成功，请重启以生效"));
        signal_window_->hide();
    }
}

void IOSelectWindow::ConnectInit()
{
    connect(close_button_, &QPushButton::clicked,
            [=]()
    {
        emit Close(qMove(object_json_), type_label_->text());
    });

    connect(table_box_, &TableBox::itemClicked, this, &IOSelectWindow::TableBoxClicked);

    connect(del_button_, &QPushButton::clicked, this, &IOSelectWindow::DelJsonData);

    connect(edit_button_, &QPushButton::clicked, this, &IOSelectWindow::EditJsonData);

    connect(add_button_, &QPushButton::clicked, this, &IOSelectWindow::AddJsonData);

    connect(io_edit_window_, &IOEditAndAddWindow::Define, this, &IOSelectWindow::DefineEdAd);

    connect(io_edit_window_, &IOEditAndAddWindow::Close, this, &IOSelectWindow::CloseEdAd);

    connect(device_add_window_, &DeviceNetAddWindow::DefineClicked, this, &IOSelectWindow::DeviceAddDefine);

    connect(device_add_window_, &DeviceNetAddWindow::CancelClicked, this, &IOSelectWindow::DeviceAddCancel);

    connect(device_edit_window_, &DeviceNetEditWindow::DefineClicked, this, &IOSelectWindow::DeviceEditDefine);

    connect(device_edit_window_, &DeviceNetEditWindow::CancelClicked, this, &IOSelectWindow::DeviceEditCancel);

    connect(signal_window_, &SignalAddEditWindow::DefineClicked, this, &IOSelectWindow::SignalDefine);

    connect(signal_window_, &SignalAddEditWindow::CancelClicked, signal_window_, &SignalAddEditWindow::hide);
}

void IOSelectWindow::UpdateTableBox(QStringList table_data)
{
    table_box_->clear();
    table_box_->SetRomn(0, 0);

    int rw = 0;
    for(int i = 0; i < table_data.size();)
    {
        if(i + 1 < table_data.size())
        {
            table_box_->AddText(rw, 0, table_data.at(i));
            table_box_->AddText(rw++, 1, table_data.at(i+1));
            i += 2;
        }
        else
        {
            table_box_->AddText(rw++, 0, table_data.at(i));
            i++;
        }
    }
}

IOEditAndAddWindow::IOEditAndAddWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_label_ = new QLabel(tr("控制面板 - 配置 - I/O"), this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QVBoxLayout(head_widget_);
    tips1_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel(head_widget_);
    head_layout->addWidget(tips1_label_);
    head_layout->addWidget(tips2_label_);

    table_widget_ = new QWidget(this);
    auto *table_layou = new QVBoxLayout(table_widget_);

    auto *label_layout = new QHBoxLayout;
    name_label_ = new QLabel(tr("参数名称"), table_widget_);
    val_label_ = new QLabel(tr("值"), table_widget_);
    label_layout->addWidget(name_label_);
    label_layout->addWidget(val_label_);
    label_layout->setContentsMargins(0, 0, 0, 0);
    label_layout->setSpacing(0);

    table_box_ = new TableBox(table_widget_);
    table_layou->addLayout(label_layout);
    table_layou->addWidget(table_box_);
    table_layou->setContentsMargins(0, 0, 0, 0);
    table_layou->setSpacing(0);
    table_layou->setStretchFactor(label_layout, 1);
    table_layou->setStretchFactor(table_box_, 8);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    close_button_->setFlat(true);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(close_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(title_label_);
    layout->addWidget(head_widget_);
    layout->addWidget(table_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_label_, 1);
    layout->setStretchFactor(head_widget_, 5);
    layout->setStretchFactor(table_widget_, 15);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);
    name_label_->setFrameShape(QFrame::Box);
    val_label_->setFrameShape(QFrame::Box);

    keyboard_ = new Keyboard(this);

    keyboard_->hide();

    row_ = -1;
    col_ = -1;

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    name_label_->setAutoFillBackground(true);
    val_label_->setAutoFillBackground(true);
    name_label_->setPalette(p);
    val_label_->setPalette(p);

    ConnectInit();
}

void IOEditAndAddWindow::Init(QJsonObject json, bool isedit)
{
    object_json_ = QSharedPointer<QJsonObject>(new QJsonObject);
    isedit_ = isedit;
    row_ = -1;
    col_ = -1;

    if(isedit)
    {
        qSwap(*object_json_, json);

        tips1_label_->setText(tr("只读"));
        tips2_label_->setText(tr("名称:") + object_json_->value("Name").toString());

        Edit();
    }
    else
    {
        tips1_label_->setText(tr("新增时必须将所有必要输入项设置为一个值"));
        tips2_label_->setText(tr("双击一个参数以修改"));

        Add(qMove(json));
    }
}

void IOEditAndAddWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
    keyboard_->SetScale(scale_w, scale_h);

    if(!keyboard_->isHidden())
    {
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
    }
}

void IOEditAndAddWindow::DefineClicked()
{
    if(isedit_)
    {
        if(object_json_->value("operate") == false)
        {
            emit Close();
            return;
        }
        else
        {
            int row = table_box_->GetRow();
            for(int i = 0; i < row; i++)
            {
                auto *item1 = table_box_->item(i, 0);
                auto *item2 = table_box_->item(i, 1);

                auto name = QString::number(i) + "_" + item1->text();
                auto data = item2->text();

                if(object_json_->value(name).toString() != data)
                    object_json_->operator[](name) = data;
            }

            QMessageBox::about(this, tr("提示"), tr("添加或修改完后请重启示教器"));
            emit Define(qMove(*object_json_), isedit_);
        }
    }
    else
    {
        int row = table_box_->GetRow();
        for(int i = 0; i < row; i++)
        {
            auto *item1 = table_box_->item(i, 0);
            auto *item2 = table_box_->item(i, 1);

            auto name = QString::number(i) + "_" + item1->text();
            auto data = item2->text();

            object_json_->insert(name, data);
        }

        object_json_->insert("operate", true);

        QMessageBox::about(this, tr("提示"), tr("添加或修改完后请重启示教器"));
        emit Define(qMove(*object_json_), isedit_);
    }
}

void IOEditAndAddWindow::CloseClicked()
{
    emit Close();
}

void IOEditAndAddWindow::TableBoxClicked(QTableWidgetItem *item)
{
    if(isedit_)
    {
        if(object_json_->value("operate") == false)
            return;
    }

    int r = table_box_->row(item);
    int c = table_box_->column(item);

    if(c != 1)
    {
        row_ = -1;
        col_ = -1;
    }

    if(r == row_ && c == col_)
    {
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
        keyboard_->show();
        keyboard_->Init(item->text());
    }
    else
    {
        row_ = r;
        col_ = c;
    }
}

void IOEditAndAddWindow::KeyGetData(QString data)
{
    auto *item = table_box_->item(row_, col_);
    item->setText(data);

    keyboard_->hide();
}

void IOEditAndAddWindow::KeyClose()
{
    keyboard_->hide();
}

void IOEditAndAddWindow::ConnectInit()
{
    connect(define_button_, &QPushButton::clicked, this, &IOEditAndAddWindow::DefineClicked);

    connect(close_button_, &QPushButton::clicked, this, &IOEditAndAddWindow::CloseClicked);

    connect(table_box_, &TableBox::itemClicked, this, &IOEditAndAddWindow::TableBoxClicked);

    connect(keyboard_, &Keyboard::GetData, this, &IOEditAndAddWindow::KeyGetData);

    connect(keyboard_->cancel_button_, &QPushButton::clicked, this, &IOEditAndAddWindow::KeyClose);
}

void IOEditAndAddWindow::Edit()
{
    table_box_->clear();
    table_box_->SetRomn(0, 0);

    auto keys = object_json_->keys();
    keys.removeAll("operate");

    int row = 0;
    for(auto key : keys)
    {
        table_box_->AddText(row, 0, key.mid(key.indexOf("_") + 1));

        auto *item1 = table_box_->item(row, 0);
        item1->setFlags(Qt::NoItemFlags);

        table_box_->AddText(row, 1, object_json_->value(key).toString());
        auto *item2 = table_box_->item(row, 1);

        if(object_json_->value("operate") == false)
            item2->setFlags(Qt::NoItemFlags);

        row++;
    }
}

void IOEditAndAddWindow::Add(QJsonObject json)
{
    table_box_->clear();
    table_box_->SetRomn(0, 0);

    auto keys = json.keys();
    keys.removeAll("operate");

    int row = 0;
    for(auto key : keys)
    {
        table_box_->AddText(row, 0, key.mid(key.indexOf("_") + 1));

        auto *item1 = table_box_->item(row, 0);
        item1->setFlags(Qt::NoItemFlags);

        table_box_->AddText(row, 1, json.value(key).toString());

        row++;
    }
}

DeviceNetEditWindow::DeviceNetEditWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_label_ = new QLabel(tr("控制面板 - 配置 - I/O - DeviceNet Device"), this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips_label_ = new QLabel(tr("名称"), head_widget_);
    name_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel(tr("双击一个参数以修改"), head_widget_);
    head_layout->addWidget(tips_label_, 0, 0);
    head_layout->addWidget(name_label_, 0, 1);
    head_layout->addWidget(tips2_label_, 1, 0);

    table_box_ = new DeviceNetTableWidget(this);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addStretch(1);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(title_label_);
    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(title_label_);
    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setStretchFactor(title_label_, 1);
    layout->setStretchFactor(head_widget_, 5);
    layout->setStretchFactor(table_box_, 17);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);

    keyboard = new Keyboard(this);
    mod_val_ = new IOModVal(this);

    keyboard->hide();
    mod_val_->hide();

    curr_item_ = nullptr;

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked, this, &DeviceNetEditWindow::DefButtonClicked);
    connect(cancel_button_, &QPushButton::clicked, this, &DeviceNetEditWindow::CanButtonClicked);
    connect(keyboard, &Keyboard::GetData, this, &DeviceNetEditWindow::KeyGetData);
    connect(keyboard->cancel_button_, &QPushButton::clicked, this, &DeviceNetEditWindow::KeyHide);
    connect(mod_val_, &IOModVal::DefineClicked, this, &DeviceNetEditWindow::IOMovDefine);
    connect(mod_val_, &IOModVal::CancelClicked, this, &DeviceNetEditWindow::IOMovCancel);
    connect(table_box_, &DeviceNetTableWidget::ShowKey, this, &DeviceNetEditWindow::ShowKey);
    connect(table_box_, &DeviceNetTableWidget::ShowNumKey, this, &DeviceNetEditWindow::ShowNumKey);
}

void DeviceNetEditWindow::Init(QJsonObject json, QString old_name)
{
    qSwap(object_, json);
    name_label_->setText(old_name);
    table_box_->Init();

    bool ismod = object_.value("operate").toBool();

    int row = table_box_->GetRow();
    for(int i = 0; i < row; i++)
    {
        auto *item1 = table_box_->item(i, 0);
        auto *item2 = table_box_->item(i, 1);
        if(item1 == nullptr || item2 == nullptr)
            continue;

        auto name = QString::number(i) + "_" + item1->text();

        item2->setText(object_.value(name).toString());

        if(!ismod)
        {
            item1->setFlags(Qt::NoItemFlags);
            item2->setFlags(Qt::NoItemFlags);
        }
    }
}

void DeviceNetEditWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
    keyboard->SetScale(scale_w, scale_h);
    mod_val_->SetScale(scale_w, scale_h);

    if(!keyboard->isHidden())
    {
        keyboard->resize(this->size());
        keyboard->move(0, 0);
    }
    if(!mod_val_->isHidden())
    {
        mod_val_->resize(this->size());
        mod_val_->move(0, 0);
    }
}

void DeviceNetEditWindow::DefButtonClicked()
{
    auto *item = table_box_->item(0, 1);
    if(item == nullptr)
    {
        return;
    }
    if(item->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("必须为参数 'Name' 提供值"));
        return;
    }

    int row = table_box_->GetRow();
    for(int i = 0; i < row; i++)
    {
        auto *item1 = table_box_->item(i, 0);
        auto *item2 = table_box_->item(i, 1);
        if(item1 == nullptr || item2 == nullptr)
            continue;

        auto name = QString::number(i) + "_" + item1->text();
        auto val = item2->text();

        if(object_.value(name) != val)
        {
            object_[name] = val;
        }
    }

    emit DefineClicked(object_, name_label_->text());
}

void DeviceNetEditWindow::CanButtonClicked()
{
    auto mag = QMessageBox::question(this, tr("提示"), tr("确定要放弃修改吗"));
    if(mag == QMessageBox::Yes)
    {
        emit CancelClicked();
    }
}

void DeviceNetEditWindow::ShowKey(QTableWidgetItem *curr_item)
{
    curr_item_ = curr_item;
    if(curr_item_ == nullptr)
    {
        return;
    }

    keyboard->resize(this->size());
    keyboard->move(0, 0);
    keyboard->Init(curr_item_->text());
    keyboard->show();
}

void DeviceNetEditWindow::ShowNumKey(QString select_name, QTableWidgetItem *curr_item)
{
    curr_item_ = curr_item;
    if(curr_item_ == nullptr)
    {
        return;
    }

    mod_val_->resize(this->size());
    mod_val_->move(0, 0);

    auto *item = table_box_->item(0, 1);
    if(item == nullptr)
        return;
    mod_val_->Init(item->text(), select_name, curr_item->text());
    mod_val_->show();
}

void DeviceNetEditWindow::KeyGetData(QString data)
{
    if(curr_item_ == nullptr)
    {
        keyboard->hide();
        return;
    }

    curr_item_->setText(data);
    keyboard->hide();
}

void DeviceNetEditWindow::KeyHide()
{
    keyboard->hide();
}

void DeviceNetEditWindow::IOMovDefine(QString val)
{
    if(curr_item_ == nullptr)
    {
        mod_val_->hide();
        return;
    }

    curr_item_->setText(val);
    mod_val_->hide();
}

void DeviceNetEditWindow::IOMovCancel()
{
    mod_val_->hide();
}

DeviceNetAddWindow::DeviceNetAddWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_label_ = new QLabel(tr("控制面板 - 配置 - I/O - DeviceNet Device - 添加"), this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QVBoxLayout(head_widget_);
    tips1_label_ = new QLabel(tr("新增时必须将所有必要输入项设置为一个值"), head_widget_);
    tips2_label_ = new QLabel(tr("双击一个参数以修改"), head_widget_);
    head_layout->addWidget(tips1_label_);
    head_layout->addWidget(tips2_label_);

    table_widget_ = new QFrame(this);
    auto *table_layout = new QVBoxLayout(table_widget_);

    auto *type_layout = new QHBoxLayout;
    tips3_label_ = new QLabel("Use values from template:", table_widget_);
    type_box_ = new QComboBox(table_widget_);
    type_layout->addWidget(tips3_label_);
    type_layout->addWidget(type_box_);
    type_layout->setContentsMargins(0, 0, 0, 0);
    type_layout->setSpacing(0);
    type_layout->setStretchFactor(tips3_label_, 1);
    type_layout->setStretchFactor(type_box_, 1);

    table_box_ = new DeviceNetTableWidget(table_widget_);
    table_layout->addLayout(type_layout);
    table_layout->addWidget(table_box_);
    table_layout->setContentsMargins(0, 0, 0, 0);
    table_layout->setSpacing(0);
    table_layout->setStretchFactor(type_layout, 1);
    table_layout->setStretchFactor(table_box_, 5);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确认"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    button_layout->addStretch(1);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);

    layout->addWidget(title_label_);
    layout->addWidget(head_widget_);
    layout->addWidget(table_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_label_, 1);
    layout->setStretchFactor(head_widget_, 5);
    layout->setStretchFactor(table_widget_, 11);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);
    table_widget_->setFrameShape(QFrame::Box);

    keyboard = new Keyboard(this);
    mod_val_ = new IOModVal(this);

    keyboard->hide();
    mod_val_->hide();

    curr_item_ = nullptr;

    QStringList types;
    types << "None" << "DSQC 651 Combi I/O Device" << "DSQC 652 24 VDC I/O Device" << "DSQC 653 Relay I/O Device";

    type_box_->addItems(types);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked, this, &DeviceNetAddWindow::DefButtonClicked);
    connect(cancel_button_, &QPushButton::clicked, this, &DeviceNetAddWindow::CanButtonClicked);
    connect(keyboard, &Keyboard::GetData, this, &DeviceNetAddWindow::KeyGetData);
    connect(keyboard->cancel_button_, &QPushButton::clicked, this, &DeviceNetAddWindow::KeyHide);
    connect(mod_val_, &IOModVal::DefineClicked, this, &DeviceNetAddWindow::IOMovDefine);
    connect(mod_val_, &IOModVal::CancelClicked, this, &DeviceNetAddWindow::IOMovCancel);
    connect(table_box_, &DeviceNetTableWidget::ShowKey, this, &DeviceNetAddWindow::ShowKey);
    connect(table_box_, &DeviceNetTableWidget::ShowNumKey, this, &DeviceNetAddWindow::ShowNumKey);
    connect(type_box_, SIGNAL(currentIndexChanged(int)), this, SLOT(ComBoxClicked(int)));
}

void DeviceNetAddWindow::Init()
{
    type_box_->setCurrentIndex(0);
    table_box_->Init();
}

void DeviceNetAddWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
    keyboard->SetScale(scale_w, scale_h);
    mod_val_->SetScale(scale_w, scale_h);

    if(!keyboard->isHidden())
    {
        keyboard->resize(this->size());
        keyboard->move(0, 0);
    }
    if(!mod_val_->isHidden())
    {
        mod_val_->resize(this->size());
        mod_val_->move(0, 0);
    }
}

void DeviceNetAddWindow::DefButtonClicked()
{
    auto *item = table_box_->item(0, 1);
    if(item == nullptr)
    {
        return;
    }
    if(item->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("必须为参数 'Name' 提供值"));
        return;
    }

    QJsonObject object;
    object.insert("devicetype€", type_box_->currentText());
    object.insert("operate", true);

    int row = table_box_->GetRow();
    for(int i = 0; i < row; i++)
    {
        auto *item1 = table_box_->item(i, 0);
        auto *item2 = table_box_->item(i, 1);
        if(item1 == nullptr || item2 == nullptr)
            continue;

        auto name = QString::number(i) + "_" + item1->text();
        auto val = item2->text();

        object.insert(name, val);
    }

    emit DefineClicked(qMove(object));
}

void DeviceNetAddWindow::CanButtonClicked()
{
    auto mag = QMessageBox::question(this, tr("提示"), tr("确定要放弃修改吗"));
    if(mag == QMessageBox::Yes)
    {
        emit CancelClicked();
    }
}

void DeviceNetAddWindow::ShowKey(QTableWidgetItem *curr_item)
{
    curr_item_ = curr_item;
    if(curr_item_ == nullptr)
    {
        return;
    }

    keyboard->resize(this->size());
    keyboard->move(0, 0);
    keyboard->Init(curr_item->text());
    keyboard->show();
}

void DeviceNetAddWindow::ShowNumKey(QString select_name, QTableWidgetItem *curr_item)
{
    curr_item_ = curr_item;
    if(curr_item_ == nullptr)
    {
        return;
    }

    mod_val_->resize(this->size());
    mod_val_->move(0, 0);

    auto *item = table_box_->item(0, 1);
    if(item == nullptr)
        return;
    mod_val_->Init(item->text(), select_name, curr_item->text());
    mod_val_->show();
}

void DeviceNetAddWindow::KeyGetData(QString data)
{
    if(curr_item_ == nullptr)
    {
        keyboard->hide();
        return;
    }

    curr_item_->setText(data);
    keyboard->hide();
}

void DeviceNetAddWindow::KeyHide()
{
    keyboard->hide();
}

void DeviceNetAddWindow::IOMovDefine(QString val)
{
    if(curr_item_ == nullptr)
    {
        mod_val_->hide();
        return;
    }

    curr_item_->setText(val);
    mod_val_->hide();
}

void DeviceNetAddWindow::IOMovCancel()
{
    mod_val_->hide();
}

void DeviceNetAddWindow::ComBoxClicked(int select)
{
    auto *item8 = table_box_->item(8, 1);
    auto *item10 = table_box_->item(10, 1);
    auto *item13 = table_box_->item(13, 1);
    auto *item14 = table_box_->item(14, 1);
    auto *item16 = table_box_->item(16, 1);
    auto *item17 = table_box_->item(17, 1);

    switch(select)
    {
    case 0:
    {
        return;
    }
    case 1:
    {
        item8->setText("Combi I/O Device");
        item10->setText("DSQC 651 Combi I/O Device");
        item13->setText("25");
        item14->setText("100");
        item16->setText("5");
        item17->setText("1");

        return;
    }
    case 2:
    {
        item8->setText("24 VDC I/O Device");
        item10->setText("DSQC 652 24 VDC I/O Device");
        item13->setText("26");
        item14->setText("7");
        item16->setText("2");
        item17->setText("2");

        return;
    }
    case 3:
    {
        item8->setText("Relay I/O Device");
        item10->setText("DSQC 653 Relay I/O Device");
        item13->setText("27");
        item14->setText("7");
        item16->setText("1");
        item17->setText("1");

        return;
    }
    default:
    {
        return;
    }
    }
}

DeviceNetTableWidget::DeviceNetTableWidget(QWidget *parent) : TableBox (parent)
{
    this->horizontalHeader()->show();

    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    old_item_ = nullptr;

    Init();
    
    connect(this, &DeviceNetTableWidget::itemClicked, this, &DeviceNetTableWidget::ItemClicked);
}

void DeviceNetTableWidget::ItemClicked(QTableWidgetItem *item)
{
    static int i = 0;
    if(item == nullptr)
        return;
    
    int row = this->row(item);
    auto *name_item = this->item(row, 0);
    item = this->item(row, 1);
    QStringList list;

    if(combo_box_.isNull())
    {
        combo_box_ = new QComboBox;
        connect(combo_box_, SIGNAL(currentIndexChanged(QString)), this, SLOT(ComBoxClicked(QString)));
    }
    else
    {
        if(old_item_ != nullptr)
        {
            int old_row = this->row(old_item_);
            auto *cbx = dynamic_cast<QComboBox*>(this->cellWidget(old_row, 1));
            if(cbx != nullptr)
            {
                this->removeCellWidget(old_row, 1);
                old_item_ = nullptr;
            }
        }
    }
    
    switch(row)
    {
    case 0: //Name
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case 2: //StateWhenStartup
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            list << "Activated" << "Deactivated" << "Last";
            auto select = item->text();
            combo_box_->addItems(list);
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case 3: //TrustLevel
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            list << "DefaultTrustLevel" << "SafetyTrustLevel";
            auto select = item->text();
            combo_box_->addItems(list);
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case 4: //Simulated
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            list << "0" << "1";
            auto select = item->text();
            combo_box_->addItems(list);
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case 5: //VendorName
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case 6: //ProductName
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case 7: //RecoveryTime
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 8: //Label
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case 9: //Address
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 10: //Vendor ID
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 11: //Product Code
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 12: //Device Type
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 13: //Production Inhibit Time (ms)
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 14: //ConnectionType
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            list << "Polled" << "Strobe" << "Change-Of-State (COS)" << "Cyclic";
            list << "Change-Of-State with Acknowledge Suppression";
            list << "Cyclic with Acknowledge Suppression";
            auto select = item->text();
            combo_box_->addItems(list);
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case 15: //PollRate
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 16: //Connection Output Size (bytes)
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 17: //Connection Input Size (bytes)
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case 18: //Quick Connect
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    default:
    {
        return;
    }
    }
}

void DeviceNetTableWidget::ComBoxClicked(QString text)
{
    if(old_item_ == nullptr || combo_box_.isNull())
        return;
    old_item_->setText(text);

    this->removeCellWidget(this->row(old_item_), 1);
}

void DeviceNetTableWidget::Init()
{
    this->clear();
    this->SetRomn(19, 2);

    QStringList heads;
    heads.append(tr("参数名称"));
    heads.append(tr("值"));

    this->setHorizontalHeaderLabels(heads);

    QStringList names;
    names << "Name" << "Network" << "StateWhenStartup" << "TrustLevel" << "Simulated" << "VendorName" << "ProductName";
    names << "RecoveryTime" << "Label" << "Address" << "Vendor ID" << "Product Code" << "Device Type";
    names << "Production Inhibit Time (ms)" << "ConnectionType" << "PollRate" << "Connection Output Size (bytes)";
    names << "Connection Input Size (bytes)" << "Quick Connect";

    QStringList vals;
    vals << "" << "DeviceNet" << "Activated" << "DefaultTrustLevel" << "0" << "" << "" << "5000" << "";
    vals << "63" << "75" << "0" << "0" << "10" << "Polled" << "1000" << "0" << "0" << "Deactivated";

    int row = this->GetRow();
    for(int i = 0; i < row; i++)
    {
        this->AddText(i, 0, names.at(i));
        this->AddText(i, 1, vals.at(i));
    }

    auto *item1 = this->item(1, 0);
    auto *item2 = this->item(1, 1);
    item1->setFlags(Qt::NoItemFlags);
    item2->setFlags(Qt::NoItemFlags);

    old_item_ = nullptr;
}

IOModVal::IOModVal(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_label_ = new QLabel(tr("编辑配置参数"), this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips1_label_ = new QLabel(tr("数据名称:"), head_widget_);
    data_name_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel(tr("参数名称:"), head_widget_);
    param_name_label_ = new QLabel(head_widget_);
    tips3_label_ = new QLabel(tr("点击一个字段以编辑"), head_widget_);
    head_layout->addWidget(tips1_label_, 0, 0);
    head_layout->addWidget(data_name_label_, 0, 1);
    head_layout->addWidget(tips2_label_, 1, 0);
    head_layout->addWidget(param_name_label_, 1, 1);
    head_layout->addWidget(tips3_label_, 2, 0);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addStretch(1);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(title_label_);
    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_label_, 1);
    layout->setStretchFactor(head_widget_, 5);
    layout->setStretchFactor(table_box_, 12);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);

    num_key_ = new NumKey(this);
    line_edit_ = new QLineEdit(this);

    num_key_->hide();

    scale_w_ = scale_h_ = 1;

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    Init();

    connect(table_box_, &TableBox::itemClicked, this, &IOModVal::ItemClicked);
    connect(num_key_, &NumKey::DefineButton, this, &IOModVal::NumKeyDefine);
    connect(num_key_, &NumKey::CancelButton, this, &IOModVal::NumKeyCancel);
    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        emit DefineClicked(line_edit_->text());
    });
    connect(cancel_button_, &QPushButton::clicked,
            [=]()
    {
        emit CancelClicked();
    });
}

void IOModVal::Init(QString data_name, QString param_name, QString val)
{
    data_name_label_->setText(data_name);
    param_name_label_->setText(param_name);

    line_edit_->setText(val);

    auto *item = table_box_->item(0, 0);
    if(item == nullptr)
    {
        throw ABBException("IOModVal", "Init", "itm为空");
    }
    item->setText(param_name + " := ");

    num_key_->resize(200*scale_w_, 250*scale_h_);
    num_key_->move(428*scale_w_, 83*scale_h_);
    num_key_->show();
    num_key_->Init(line_edit_);
}

void IOModVal::resizeEvent(QResizeEvent *)
{
    num_key_->resize(200*scale_w_, 250*scale_h_);
    num_key_->move(428*scale_w_, 83*scale_h_);
}

void IOModVal::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

void IOModVal::NumKeyDefine()
{
    num_key_->hide();
}

void IOModVal::NumKeyCancel()
{
    num_key_->hide();
}

void IOModVal::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;
    if(!num_key_->isHidden())
        return;

    num_key_->resize(200*scale_w_, 250*scale_h_);
    num_key_->move(428*scale_w_, 83*scale_h_);
    num_key_->show();
    num_key_->Init(line_edit_);
}

void IOModVal::Init()
{
    table_box_->AddText(0, 0, "");
    table_box_->AddWidget(0, 1, line_edit_);

    QStringList heads;
    heads << tr("参数名称") << tr("值");

    table_box_->setHorizontalHeaderLabels(heads);
}

SignalAddEditWindow::SignalAddEditWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_label = new QLabel(tr("控制面板 - 配置 - I/O - Signal - 添加"), this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QVBoxLayout(head_widget_);
    tips_label_ = new QLabel(tr("新增时必须将所有必要输入项设置为一个值"), head_widget_);
    tips2_label_ = new QLabel(tr("双击一个参数以修改"), head_widget_);
    head_layout->addWidget(tips_label_);
    head_layout->addWidget(tips2_label_);

    table_box_ = new SignalTableWidget(this);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确认"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    button_layout->addStretch(1);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);

    layout->addWidget(title_label);
    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_label, 1);
    layout->setStretchFactor(head_widget_, 5);
    layout->setStretchFactor(table_box_, 11);
    layout->setStretchFactor(button_widget_, 2);

    head_widget_->setFrameShape(QFrame::Box);

    keyboard = new Keyboard(this);
    mod_val_ = new IOModVal(this);

    keyboard->hide();
    mod_val_->hide();

    curr_item_ = nullptr;

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked, this, &SignalAddEditWindow::DefButtonClicked);
    connect(cancel_button_, &QPushButton::clicked, this, &SignalAddEditWindow::CanButtonClicked);
    connect(keyboard, &Keyboard::GetData, this, &SignalAddEditWindow::KeyGetData);
    connect(keyboard->cancel_button_, &QPushButton::clicked, this, &SignalAddEditWindow::KeyHide);
    connect(mod_val_, &IOModVal::DefineClicked, this, &SignalAddEditWindow::IOMovDefine);
    connect(mod_val_, &IOModVal::CancelClicked, this, &SignalAddEditWindow::IOMovCancel);
    connect(table_box_, &DeviceNetTableWidget::ShowKey, this, &SignalAddEditWindow::ShowKey);
    connect(table_box_, &DeviceNetTableWidget::ShowNumKey, this, &SignalAddEditWindow::ShowNumKey);
}

void SignalAddEditWindow::Init(QJsonObject json, QStringList assigneds, QString old_name)
{
    qSwap(object_, json);
    old_name_ = old_name;

    if(old_name_.isEmpty())
        table_box_->SigInit(&object_, assigneds, true);
    else
        table_box_->SigInit(&object_, assigneds, false);
}

void SignalAddEditWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);

    if(!keyboard->isHidden())
    {
        keyboard->resize(this->size());
        keyboard->move(0, 0);
    }
    keyboard->SetScale(scale_w, scale_h);

    if(!mod_val_->isHidden())
    {
        mod_val_->resize(this->size());
        mod_val_->move(0, 0);
    }
    mod_val_->SetScale(scale_w, scale_h);
}

void SignalAddEditWindow::DefButtonClicked()
{
    auto *item = table_box_->item(0, 1);
    if(item == nullptr)
    {
        return;
    }
    if(item->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("必须为参数 'Name' 提供值"));
        return;
    }

    bool isadd;
    if(old_name_.isEmpty())
    {
        isadd = true;

        object_.insert("operate", true);

        auto tp = table_box_->item(1, 1)->text();

        QString type;
        if(tp == "Digital Input")
            type = "DI"; //数字输入
        else if(tp == "Digital Output")
            type = "DO"; //数字输出
        else if(tp == "Analog Input")
            type = "AI"; //模拟输入
        else if(tp == "Analog Output")
            type = "AO"; //模拟输出
        else if(tp == "Group Input")
            type = "GI"; //组输入
        else
            type = "GO"; //组输出

        object_.insert("signal_type㎏", type);
    }
    else
    {
        isadd = false;
    }

    int row = table_box_->GetRow();
    for(int i = 0; i < row; i++)
    {
        auto *item1 = table_box_->item(i, 0);
        auto *item2 = table_box_->item(i, 1);
        if(item1 == nullptr || item2 == nullptr)
            continue;

        auto name = QString::number(i) + "_" + item1->text();
        auto val = item2->text();

        if(isadd) //添加
        {
            object_.insert(name, val);
        }
        else //编辑
        {
            if(object_.value(name) != val)
            {
                object_[name] = val;
            }
        }
    }

    emit DefineClicked(object_, old_name_);
}

void SignalAddEditWindow::CanButtonClicked()
{
    auto ismod = object_.value("operate").toBool();
    if(!ismod && !old_name_.isEmpty())
    {
        emit CancelClicked();
        return;
    }

    auto mag = QMessageBox::question(this, tr("提示"), tr("确定要放弃修改吗"));
    if(mag == QMessageBox::Yes)
    {
        emit CancelClicked();
    }
}

void SignalAddEditWindow::ShowKey(QTableWidgetItem *curr_item)
{
    curr_item_ = curr_item;
    if(curr_item_ == nullptr)
    {
        return;
    }

    keyboard->resize(this->size());
    keyboard->move(0, 0);
    keyboard->Init(curr_item->text());
    keyboard->show();
}

void SignalAddEditWindow::ShowNumKey(QString select_name, QTableWidgetItem *curr_item)
{
    curr_item_ = curr_item;
    if(curr_item_ == nullptr)
    {
        return;
    }

    mod_val_->resize(this->size());
    mod_val_->move(0, 0);

    auto *item = table_box_->item(0, 1);
    if(item == nullptr)
        return;
    mod_val_->Init(item->text(), select_name, curr_item->text());
    mod_val_->show();
}

void SignalAddEditWindow::KeyGetData(QString data)
{
    if(curr_item_ == nullptr)
    {
        keyboard->hide();
        return;
    }

    curr_item_->setText(data);
    keyboard->hide();
}

void SignalAddEditWindow::KeyHide()
{
    keyboard->hide();
}

void SignalAddEditWindow::IOMovDefine(QString val)
{
    if(curr_item_ == nullptr)
    {
        mod_val_->hide();
        return;
    }

    curr_item_->setText(val);
    mod_val_->hide();
}

void SignalAddEditWindow::IOMovCancel()
{
    mod_val_->hide();
}

SignalTableWidget::SignalTableWidget(QWidget *parent) : DeviceNetTableWidget (parent)
{
    name_list_ << "Name" << "Type of Signal" << "Assigned to Device" << "Signal Identification Label";
    name_list_ << "Category" << "Access Level" << "Safe Level" << "Default Value" << "Analog Encoding Type";
    name_list_ << "Maximum Logical Value" << "Minimum Logical Value" << "Number of Bits" << "Device Mapping";
    name_list_ << "Filter Time Passive (ms)" << "Filter Time Active (ms)" << "Invert Physical Value";
    name_list_ << "Maximum Physical Value" << "Maximum Physical Value Limit" << "Maximum Bit Value";
    name_list_ << "Minimum Physical Value" << "Minimum Physical Value Limit" << "Minimum Bit Value";

    str_enum_map_.insert(name_list_.at(0), Names::Name0);
    str_enum_map_.insert(name_list_.at(1), Names::Type1);
    str_enum_map_.insert(name_list_.at(2), Names::Assigned2);
    str_enum_map_.insert(name_list_.at(3), Names::Signal3);
    str_enum_map_.insert(name_list_.at(4), Names::Category4);
    str_enum_map_.insert(name_list_.at(5), Names::Access5);
    str_enum_map_.insert(name_list_.at(6), Names::Safe6);
    str_enum_map_.insert(name_list_.at(7), Names::Default7);
    str_enum_map_.insert(name_list_.at(8), Names::Analog8);
    str_enum_map_.insert(name_list_.at(9), Names::Maximum9);
    str_enum_map_.insert(name_list_.at(10), Names::Minimum10);
    str_enum_map_.insert(name_list_.at(11), Names::Number11);
    str_enum_map_.insert(name_list_.at(12), Names::Device12);
    str_enum_map_.insert(name_list_.at(13), Names::Filpas13);
    str_enum_map_.insert(name_list_.at(14), Names::Filact14);
    str_enum_map_.insert(name_list_.at(15), Names::Invert15);
    str_enum_map_.insert(name_list_.at(16), Names::Maxphv16);
    str_enum_map_.insert(name_list_.at(17), Names::Maxpvl17);
    str_enum_map_.insert(name_list_.at(18), Names::Maxbv18);
    str_enum_map_.insert(name_list_.at(19), Names::Minphv19);
    str_enum_map_.insert(name_list_.at(20), Names::Minpvl20);
    str_enum_map_.insert(name_list_.at(21), Names::Minbv21);

    enum_val_map_.insert(Names::Name0, QStringList() << "");
    enum_val_map_.insert(Names::Type1, QStringList() << "" << "Digital Input" << "Digital Output" << "Analog Input" << "Analog Output" << "Group Input" << "Group Output");
    enum_val_map_.insert(Names::Assigned2, QStringList() << "");
    enum_val_map_.insert(Names::Signal3, QStringList() << "");
    enum_val_map_.insert(Names::Category4, QStringList() << "");
    enum_val_map_.insert(Names::Access5, QStringList() << "Default" << "ReadOnly" << "All");
    enum_val_map_.insert(Names::Safe6, QStringList() << "DefaultSafeLevel" << "SafetySafeLevel");
    enum_val_map_.insert(Names::Default7, QStringList() << "0");
    enum_val_map_.insert(Names::Analog8, QStringList() << "Two Complement" << "Unsigned");
    enum_val_map_.insert(Names::Maximum9, QStringList() << "0");
    enum_val_map_.insert(Names::Minimum10, QStringList() << "0");
    enum_val_map_.insert(Names::Number11, QStringList() << "23");
    enum_val_map_.insert(Names::Device12, QStringList() << "");
    enum_val_map_.insert(Names::Filpas13, QStringList() << "0");
    enum_val_map_.insert(Names::Filact14, QStringList() << "0");
    enum_val_map_.insert(Names::Invert15, QStringList() << "No" << "Yes");
    enum_val_map_.insert(Names::Maxphv16, QStringList() << "0");
    enum_val_map_.insert(Names::Maxpvl17, QStringList() << "0");
    enum_val_map_.insert(Names::Maxbv18, QStringList() << "0");
    enum_val_map_.insert(Names::Minphv19, QStringList() << "0");
    enum_val_map_.insert(Names::Minpvl20, QStringList() << "0");
    enum_val_map_.insert(Names::Minbv21, QStringList() << "0");

}

void SignalTableWidget::SigInit(QJsonObject *json, QStringList assigneds, bool isadd)
{
    object_ = json;
    isadd_ = isadd;
    old_item_ = nullptr;

    if(assigneds.isEmpty())
        assigneds << "";
    enum_val_map_[Names::Assigned2] = assigneds;

    this->clear();
    this->SetRomn(7, 2);

    QStringList heads;
    heads << tr("参数名称") << tr("值");

    this->setHorizontalHeaderLabels(heads);

    if(isadd) //添加
    {
        for(int i = 0; i < 7; i++)
        {
            auto name = name_list_.at(i);

            this->AddText(i, 0, name);
            this->AddText(i, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }
    }
    else //编辑
    {
        auto keys = object_->keys();
        keys.removeAll("operate");
        keys.removeAll("signal_type㎏");
        bool ismod = object_->value("operate").toBool();

        for(int i = 0; i < keys.size(); i++)
        {
            auto key = keys.at(i);
            auto name = key.mid(key.indexOf("_") + 1);

            this->AddText(i, 0, name);
            this->AddText(i, 1, object_->value(key).toString());

            if(!ismod)
            {
                auto *item1 = this->item(i, 0);
                auto *item2 = this->item(i, 1);

                item1->setFlags(Qt::NoItemFlags);
                item2->setFlags(Qt::NoItemFlags);
            }
        }
    }
}

void SignalTableWidget::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    int row = this->row(item);
    auto *name_item = this->item(row, 0);
    if(name_item == nullptr)
        return;
    if(name_item == item)
        item = this->item(row, 1);


    if(combo_box_.isNull())
    {
        combo_box_ = new QComboBox(this);
        connect(combo_box_, SIGNAL(currentIndexChanged(QString)), this, SLOT(ComBoxClicked(QString)));
    }
    else
    {
        if(old_item_ != nullptr)
        {
            int old_row = this->row(old_item_);
            auto *cbx = dynamic_cast<QComboBox*>(this->cellWidget(old_row, 1));
            if(cbx != nullptr)
            {
                this->removeCellWidget(old_row, 1);
                old_item_ = nullptr;
            }
        }
    }

    auto name = StrToEnum(name_item->text());
    switch(name)
    {
    case Names::Name0:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case Names::Type1:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            auto select = item->text();
            combo_box_->addItems(enum_val_map_.value(name));
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case Names::Assigned2:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            auto select = item->text();
            combo_box_->addItems(enum_val_map_.value(name));
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case Names::Signal3:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case Names::Category4:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case Names::Access5:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            auto select = item->text();
            combo_box_->addItems(enum_val_map_.value(name));
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case Names::Safe6:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            auto select = item->text();
            combo_box_->addItems(enum_val_map_.value(name));
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case Names::Default7:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Analog8:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            auto select = item->text();
            combo_box_->addItems(enum_val_map_.value(name));
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case Names::Maximum9:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Minimum10:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Number11:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Device12:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowKey(item);
        }
        break;
    }
    case Names::Filpas13:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Filact14:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Invert15:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            auto select = item->text();
            combo_box_->addItems(enum_val_map_.value(name));
            combo_box_->setCurrentText(select);
            this->AddWidget(row, 1, combo_box_);
        }
        break;
    }
    case Names::Maxphv16:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Maxpvl17:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Maxbv18:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Minphv19:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Minpvl20:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    case Names::Minbv21:
    {
        if(item != old_item_)
        {
            old_item_ = item;
        }
        else
        {
            emit ShowNumKey(name_item->text(), item);
        }
        break;
    }
    default:
    {
        return;
    }
    }
}

void SignalTableWidget::ComBoxClicked(QString text)
{
    if(old_item_ == nullptr)
    {
        return;
    }

    auto old_name = old_item_->text();
    int item_row = this->row(old_item_);
    if(text == old_name)
    {
        this->removeCellWidget(item_row, 1);
        return;
    }

    auto *name_item1 = this->item(item_row, 0);

    auto name_type = StrToEnum(name_item1->text());

    DeviceNetTableWidget::ComBoxClicked(text);

    if(name_type == Names::Type1)
    {
        if(item(2, 1)->text().isEmpty())
            Type(text, 1);
        else
            Device(text, 1);
    }
    else if(name_type == Names::Assigned2)
    {
        auto type_name = item(1, 1)->text();
        if(text == "")
        {
            Type(type_name, 2);
            return;
        }
        else
        {
            Device(type_name, 2);
        }
    }
}

void SignalTableWidget::Type(QString text, int curr_row)
{
    QMap<QString, QString> curr_map;
    int row = this->GetRow();
    for(int i = 0; i < row; i++)
    {
        auto *item1 = this->item(i, 0);
        auto *item2 = this->item(i, 1);

        curr_map.insert(item1->text(), item2->text());
    }

    auto &list = enum_val_map_[Names::Type1];
    int num = list.indexOf(text);
    this->clearContents();
    this->SetRomn(5, 2);

    for(int i = 0; i < 6; i++)
    {
        auto name = name_list_.at(i);
        this->AddText(i, 0, name);
        this->AddText(i, 1, curr_map.value(name));
    }

    if(num == 0)
    {
        auto name = name_list_.at(6);
        this->AddText(6, 0, name);
        this->AddText(6, 1, curr_map.value(name));
    }
    else if(num == 1 || num == 2)
    {
        auto name = name_list_.at(7);
        this->AddText(6, 0, name);
        if(curr_map.contains(name))
        {
            this->AddText(6, 1, curr_map.value(name));
        }
        else if(object_->contains("6_" + name))
        {
            this->AddText(6, 1, object_->value("6_" + name).toString());
        }
        else
        {
            this->AddText(6, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }

        name = name_list_.at(6);
        this->AddText(7, 0, name);
        if(curr_map.contains(name))
        {
            this->AddText(7, 1, curr_map.value(name));
        }
        else if(object_->contains("7_" + name))
        {
            this->AddText(7, 1, object_->value("7_" + name).toString());
        }
        else
        {
            this->AddText(7, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }
    }
    else if(num == 3 || num == 4)
    {
        for(int i = 7; i < 11; i++)
        {
            auto name = name_list_.at(i);
            this->AddText(i-1, 0, name);

            if(curr_map.contains(name))
            {
                this->AddText(i-1, 1, curr_map.value(name));
            }
            else if(object_->contains(QString::number(i-1) + "_" + name))
            {
                this->AddText(i-1, 1, object_->value(QString::number(i-1) + "_" + name).toString());
            }
            else
            {
                this->AddText(i-1, 1, enum_val_map_.value(StrToEnum(name)).at(0));
            }
        }

        auto name = name_list_.at(6);
        this->AddText(10, 0, name);

        if(curr_map.contains(name))
        {
            this->AddText(10, 1, curr_map.value(name));
        }
        else if(object_->contains("10_" + name))
        {
            this->AddText(10, 1, object_->value("10_" + name).toString());
        }
        else
        {
            this->AddText(10, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }

        name = name_list_.at(11);
        this->AddText(11, 0, name);

        if(curr_map.contains(name))
        {
            this->AddText(11, 1, curr_map.value(name));
        }
        else if(object_->contains("11_" + name))
        {
            this->AddText(11, 1, object_->value("11_" + name).toString());
        }
        else
        {
            this->AddText(11, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }
    }
    else if(num == 5 || num == 6)
    {
        auto name = name_list_.at(7);
        this->AddText(6, 0, name);
        if(curr_map.contains(name))
        {
            this->AddText(6, 1, curr_map.value(name));
        }
        else if(object_->contains("6_" + name))
        {
            this->AddText(6, 1, object_->value("6_" + name).toString());
        }
        else
        {
            this->AddText(6, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }

        name = name_list_.at(6);
        this->AddText(7, 0, name);
        if(curr_map.contains(name))
        {
            this->AddText(7, 1, curr_map.value(name));
        }
        else if(object_->contains("7_" + name))
        {
            this->AddText(7, 1, object_->value("7_" + name).toString());
        }
        else
        {
            this->AddText(7, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }

        name = name_list_.at(11);
        this->AddText(8, 0, name);
        if(curr_map.contains(name))
        {
            this->AddText(8, 1, curr_map.value(name));
        }
        else if(object_->contains("8_" + name))
        {
            this->AddText(8, 1, object_->value("8_" + name).toString());
        }
        else
        {
            this->AddText(8, 1, enum_val_map_.value(StrToEnum(name)).at(0));
        }
    }
    old_item_ = this->item(curr_row, 1);
}

void SignalTableWidget::Device(QString text, int curr_row)
{
    QStringList list1;

    auto &list = list1;
    list << "Name" << "Type of Signal" << "Assigned to Device" << "Signal Identification Label";
    list << "Device Mapping" << "Category" << "Access Level" << "Default Value";

    if(text == "")
    {
        return;
    }
    else if(text == "Digital Input" || text == "Group Input")
    {
        list << "Filter Time Passive (ms)" << "Filter Time Active (ms)";
        list << "Invert Physical Value" << "Safe Level";
    }
    else if(text == "Digital Output" || text == "Group Output")
    {
        list << "Invert Physical Value" << "Safe Level";
    }
    else if(text == "Analog Input" || text == "Anglog Output")
    {
        list << "Analog Encoding Type" << "Maximum Logical Value";
        list << "Maximum Physical Value" << "Maximum Physical Value Limit" << "Maximum Bit Value";
        list << "Minimum Logical Value" << "Minimum Physical Value" << "Minimum Physical Value Limit";
        list << "Minimum Bit Value" << "Safe Level";
    }
    else
    {
        return;
    }

    QMap<QString, QString> curr_map;
    int row = this->GetRow();
    for(int i = 0; i < row; i++)
    {
        auto *item1 = this->item(i, 0);
        auto *item2 = this->item(i, 1);

        curr_map.insert(item1->text(), item2->text());
    }

    this->clearContents();
    this->SetRomn(5, 2);


    for(int i = 0; i < list1.size(); i++)
    {
        auto name = list1.at(i);

        this->AddText(i, 0, name);

        if(curr_map.contains(name))
            this->AddText(i, 1, curr_map.value(name));
        else if(object_->contains(QString::number(i) + "_" + name))
            this->AddText(i, 1, object_->value(QString::number(i) + "_" + name).toString());
        else
            this->AddText(i, 1, enum_val_map_.value(StrToEnum(name)).at(0));
    }
    old_item_ = this->item(curr_row, 1);
}

QString SignalTableWidget::EnumToStr(SignalTableWidget::Names name)
{
    if(name == Names::None)
        return QString();
    else
        return str_enum_map_.key(name);
}

SignalTableWidget::Names SignalTableWidget::StrToEnum(QString name)
{
    if(!str_enum_map_.contains(name))
        return Names::None;
    else
        return str_enum_map_.value(name);
}
