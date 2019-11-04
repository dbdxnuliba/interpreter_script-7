#include "hotedit.h"
#include "MainWidget/commoncontrols.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QtMath>
#include <QDebug>

QSharedPointer<QMap<QString,HotEditVarInfo>> HotEdit::hot_var_point_;
QPointer<ChoiceTableSub> ChoiceTableSub::sub_pointer_ = nullptr;
QMap<QString,int> ChoiceTableSub::save_sub_map_;

HotEdit::HotEdit(QWidget *parent) : HotEditObject (parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title_layout = new QHBoxLayout;
    title_layout->addWidget(&ico_label_);
    title_layout->addWidget(&title_label_);
    title_layout->setContentsMargins(0, 0, 0, 0);
    title_layout->setSpacing(0);
    title_layout->setStretchFactor(&ico_label_, 1);
    title_layout->setStretchFactor(&title_label_, 20);

    auto *group_layout = new QHBoxLayout;
    group_layout->addWidget(&set_goals_);
    group_layout->addWidget(&choice_goals_);
    group_layout->setContentsMargins(0, 0, 0, 0);
    group_layout->setSpacing(0);
    group_layout->setStretchFactor(&set_goals_, 1);
    group_layout->setStretchFactor(&choice_goals_, 1);

    auto *button_layout = new QHBoxLayout(&button_widget_);
    button_layout->addWidget(&file_button_);
    button_layout->addWidget(&base_button_);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(&define_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    file_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    base_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    file_button_.setFlat(true);
    base_button_.setFlat(true);
    define_button_.setFlat(true);

    layout->addLayout(title_layout);
    layout->addLayout(group_layout);
    layout->addWidget(&button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_layout, 1);
    layout->setStretchFactor(group_layout, 10);
    layout->setStretchFactor(&button_widget_, 2);

    ico_label_.setStyleSheet("border-image: url(:/image/Image/MainWidget/hotedit.png)");
    title_label_.setText("HotEdit");

    file_button_.setText(tr("文件"));
    base_button_.setText(tr("基线"));
    define_button_.setText(tr("应用"));

    set_goals_.setTitle(tr("设定的目标"));
    choice_goals_.setTitle(tr("选定的目标"));

    QString goals_qss;
#ifdef _MSC_VER
    goals_qss = "QGroupBox {border-color: rgb(156, 156, 156);border-width: 1px;border-style: solid;margin-top: 5.4ex;}";            //这两行qss语句的作用是
    goals_qss += "QGroupBox::title {subcontrol-origin: margin;subcontrol-position: top left;left: 10px;margin-left: 2px;padding: 0px;}"; //使QGroupBox拥有边框
#else
    goals_qss = "QGroupBox {border-color: rgb(156, 156, 156);border-width: 1px;border-style: solid;margin-top: 2.4ex;}";
    goals_qss += "QGroupBox::title {subcontrol-origin: margin;subcontrol-position: top left;left: 10px;margin-left: 2px;padding: 0px;}";
#endif

    set_goals_.setStyleSheet(goals_qss);
    choice_goals_.setStyleSheet(goals_qss);

    set_tree_ = new SetTreeWindow(&set_goals_);
    set_adjust_ = new SetAdjustWindow(&set_goals_);

    auto *set_layout = new QHBoxLayout(&set_goals_);
    set_layout->addWidget(set_tree_);
    set_layout->addWidget(set_adjust_);
    set_layout->setContentsMargins(0, 0, 0, 0);

    choice_window_ = new ChoiceWindow(&choice_goals_);
    choice_button_ = new QPushButton(tr(" <<< 调节目标"), &choice_goals_);

    auto *choice_layout =new QVBoxLayout(&choice_goals_);
    choice_layout->addWidget(choice_window_);
    choice_layout->addWidget(choice_button_);
    choice_layout->setContentsMargins(0, 0, 0, 0);
    choice_layout->setSpacing(0);
    choice_layout->setStretchFactor(choice_window_, 7);
    choice_layout->setStretchFactor(choice_button_, 1);

    set_adjust_->hide();

    hot_var_point_ = QSharedPointer<QMap<QString, HotEditVarInfo>>(new QMap<QString, HotEditVarInfo>);
    EditData::instance()->SetHotVarPoint(hot_var_point_);

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_.setAutoFillBackground(true);
    button_widget_.setPalette(p);

    ConnectInit();
    ReadData();
}

HotEdit::~HotEdit()
{
    WriteData();
}

void HotEdit::Init()
{
    EditData::instance()->GetHotEditData(std::bind(&HotEdit::ThreadAddTree, this), hot_var_point_);

    if(!set_adjust_->isHidden())
    {
        set_adjust_->hide();
        set_tree_->show();
    }
}

void HotEdit::TreeClicked(QTreeWidgetItem *item, int column)
{
    auto name = item->text(column);

    if(!hot_var_point_->contains(name))
        return;

    if(ChoiceTableSub::GetMap().contains(name))
        return;

    auto &hvp = (*hot_var_point_)[name];

    auto *sub = new ChoiceTableSub(choice_window_->table_box_);
    sub->SetName(name);
    auto num = hvp.x + hvp.y + hvp.z;
    if(num < 0)
        num = qAbs(num);
    auto x = qSqrt(num);
    sub->SetRedress(QString::number(x));

    connect(sub, &ChoiceTableSub::Clicked, this, &HotEdit::SubClicked);
    connect(this, &HotEdit::SubPointClicked, sub, &ChoiceTableSub::UpdateState);
    connect(sub, &ChoiceTableSub::DelButtonClicked, this, &HotEdit::SubDelClicked);

    int row = choice_window_->table_box_->GetRow();

    auto par = choice_window_->table_box_->AddWidget(row, 0, sub);
    sub->save_sub_map_.insert(name, par.first);
}

void HotEdit::SubClicked()
{
    emit SubPointClicked();
    choice_window_->table_box_->clearSelection();
}

void HotEdit::SubDelClicked()
{
    auto *sub = dynamic_cast<ChoiceTableSub*>(sender());
    if(sub == nullptr)
        return;

    auto name = sub->name_label_->text();

    auto &mp = ChoiceTableSub::GetMap();

    choice_window_->table_box_->removeCellWidget(mp.value(name), 0);
    delete sub;
    mp.remove(name);
    if(mp.isEmpty())
    {
        choice_window_->table_box_->clear();
        choice_window_->table_box_->SetRomn(0, 0);
    }
}

void HotEdit::DefineClicked()
{
    auto &mp = ChoiceTableSub::GetMap();

    if(mp.isEmpty())
        return;

    int type = set_adjust_->mod_box_->currentIndex();
    if(type >= 3)
        return;

    auto mag = QMessageBox::question(this, tr("提示"), tr("确定要修改吗?"));
    if(mag == QMessageBox::No)
        return;

    double q = set_adjust_->delta1_val_label_->text().toDouble();
    double w = set_adjust_->delta2_val_label_->text().toDouble();
    double e = set_adjust_->delta3_val_label_->text().toDouble();

    for(auto it = mp.begin(); it != mp.end(); ++it)
    {
        if(type == 0)
        {
            auto &hvp = (*hot_var_point_)[it.key()];
            hvp.x += q;
            hvp.y += w;
            hvp.z += e;

            auto num = hvp.x + hvp.y + hvp.z;
            if(num < 0)
                num = qAbs(num);
            auto val = qSqrt(num);

            auto *sub = dynamic_cast<ChoiceTableSub*>(choice_window_->table_box_->cellWidget(it.value(), 0));
            sub->SetRedress(QString::number(val));
        }
        else
        {
            (*hot_var_point_)[it.key()].a += q;
            (*hot_var_point_)[it.key()].b += w;
            (*hot_var_point_)[it.key()].c += e;
        }
    }

    set_adjust_->delta1_val_label_->setText("0");
    set_adjust_->delta2_val_label_->setText("0");
    set_adjust_->delta3_val_label_->setText("0");
}

void HotEdit::ConnectInit()
{
    connect(set_tree_, &SetTreeWindow::HideWindow,
            [=]()
    {
        choice_button_->setText(tr("调节目标 >>> "));
    });

    connect(set_tree_, &SetTreeWindow::ShowWindow,
            [=]()
    {
        choice_button_->setText(tr(" <<< 调节目标"));
    });

    connect(choice_button_, &QPushButton::clicked,
            [=]()
    {
        if(set_tree_->isHidden())
        {
            set_tree_->show();
            set_adjust_->hide();
        }
        else
        {
            set_tree_->hide();
            set_adjust_->show();
        }
    });

    connect(this, &HotEdit::ShowWindow, this, &HotEdit::Init);

    connect(this, &HotEdit::TAddTree, this, &HotEdit::AddTreeData);

    connect(&define_button_, &QPushButton::clicked, this, &HotEdit::DefineClicked);

    connect(set_tree_->tree_widget_, &QTreeWidget::itemClicked, this, &HotEdit::TreeClicked);
}

void HotEdit::ReadData()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/HotEdit/hotedit";
    QFile file(path);
    if(!file.exists())
    {
        file.open(QIODevice::WriteOnly);
        file.write("");
        return;
    }
    else
    {
        if(!file.open(QIODevice::ReadOnly))
            return;
    }

    QString data = file.readAll();

    auto hots = data.split("\n");

    for(auto hot : hots)
    {
        if(hot.isEmpty())
            continue;

        auto var_name = hot.section(" ", 0, 0);
        HotEditVarInfo hot_info;
        hot_info.mod_name = hot.section(" ", 1, 1);
        hot_info.action_scope = hot.section(" ", 2, 2);
        hot_info.x = hot.section(" ", 3, 3).toDouble();
        hot_info.y = hot.section(" ", 4, 4).toDouble();
        hot_info.z = hot.section(" ", 5, 5).toDouble();
        hot_info.a = hot.section(" ", 6, 6).toDouble();
        hot_info.b = hot.section(" ", 7, 7).toDouble();
        hot_info.c = hot.section(" ", 8, 8).toDouble();

        hot_var_point_->insert(std::move(var_name), std::move(hot_info));
    }

    if(!hot_var_point_->isEmpty())
    {
        AddTreeData();
    }
}

void HotEdit::WriteData()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/HotEdit/hotedit";
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QString data;
    for(auto it = hot_var_point_->begin(); it != hot_var_point_->end(); ++it)
    {
        auto hot = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9\n").arg(it.key()).arg(it->mod_name).arg(it->action_scope).arg(it->x).arg(it->y).arg(it->z).arg(it->a).arg(it->b).arg(it->c);

        data.append(std::move(hot));
    }

    file.write(data.toUtf8());
}

void HotEdit::ThreadAddTree()
{
    emit TAddTree();
}

void HotEdit::AddTreeData()
{
    auto *tree_widget = set_tree_->tree_widget_;

    choice_window_->table_box_->clear();
    choice_window_->table_box_->SetRomn(0, 0);
    ChoiceTableSub::GetMap().clear();
    tree_widget->clear();
    QMap<QString, QTreeWidgetItem*> tree_map;

    for(auto it = hot_var_point_->begin(); it != hot_var_point_->end(); ++it)
    {
        if(!tree_map.contains(it->mod_name))
        {
            auto *sub_tree = new QTreeWidgetItem;
            sub_tree->setText(0, it->mod_name);
            tree_map.insert(it->mod_name, sub_tree);
        }

        auto *sub_tree = new QTreeWidgetItem;
        sub_tree->setText(0, it.key());
        tree_map.value(it->mod_name)->addChild(sub_tree);
    }

    tree_widget->addTopLevelItems(tree_map.values());
}

SetTreeWindow::SetTreeWindow(QWidget *parent) : HotEditObject (parent)
{
    auto layout = new QHBoxLayout(this);

    tree_widget_ = new QTreeWidget(this);

    layout->addWidget(tree_widget_);

    layout->setContentsMargins(0, 0, 0, 0);

    tree_widget_->header()->hide(); //隐藏表头
}

SetAdjustWindow::SetAdjustWindow(QWidget *parent) : HotEditObject (parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *mod_layout = new QHBoxLayout;
    mod_label_ = new QLabel(tr("调节模式:"), this);
    mod_box_ = new QComboBox(this);
    mod_layout->addWidget(mod_label_);
    mod_layout->addWidget(mod_box_);

    auto *ucs_layout = new QHBoxLayout;
    ucs_label_ = new QLabel(tr("坐标系:"), this);
    ucs_box_ = new QComboBox(this);
    ucs_layout->addWidget(ucs_label_);
    ucs_layout->addWidget(ucs_box_);

    auto *insert_layout = new QHBoxLayout;
    insert_label_ = new QLabel(tr("增量:"), this);
    insert_box_ = new QComboBox(this);
    insert_layout->addWidget(insert_label_);
    insert_layout->addWidget(insert_box_);

    auto *de1_layout = new QHBoxLayout;
    delta1_name_label_ = new QLabel("delta X:", this);
    delta1_del_button_ = new QPushButton("-", this);
    delta1_val_label_ = new QLabel("0", this);
    delta1_add_button_ = new QPushButton("+", this);
    de1_layout->addWidget(delta1_name_label_);
    de1_layout->addWidget(delta1_del_button_);
    de1_layout->addWidget(delta1_val_label_);
    de1_layout->addWidget(delta1_add_button_);

    auto *de2_layout = new QHBoxLayout;
    delta2_name_label_ = new QLabel("delta Y:", this);
    delta2_del_button_ = new QPushButton("-", this);
    delta2_val_label_ = new QLabel("0", this);
    delta2_add_button_ = new QPushButton("+", this);
    de2_layout->addWidget(delta2_name_label_);
    de2_layout->addWidget(delta2_del_button_);
    de2_layout->addWidget(delta2_val_label_);
    de2_layout->addWidget(delta2_add_button_);

    auto *de3_layout = new QHBoxLayout;
    delta3_name_label_ = new QLabel("delta Z:", this);
    delta3_del_button_ = new QPushButton("-", this);
    delta3_val_label_ = new QLabel("0", this);
    delta3_add_button_ = new QPushButton("+", this);
    de3_layout->addWidget(delta3_name_label_);
    de3_layout->addWidget(delta3_del_button_);
    de3_layout->addWidget(delta3_val_label_);
    de3_layout->addWidget(delta3_add_button_);

    layout->addLayout(mod_layout);
    layout->addLayout(ucs_layout);
    layout->addLayout(insert_layout);
    layout->addLayout(de1_layout);
    layout->addLayout(de2_layout);
    layout->addLayout(de3_layout);

    mod_box_->addItem(tr("线性"));
    mod_box_->addItem(tr("重定位"));
    mod_box_->addItem(tr("外轴"));

    ucs_box_->addItem(tr("工件坐标"));
    ucs_box_->addItem(tr("工具"));

    QStringList inslist;
    inslist << "0.1" << "0.5" << "1" << "5";
    insert_box_->addItems(inslist);
    insert_box_->setCurrentIndex(2);

    ConnectInit();
}

void SetAdjustWindow::ModBoxClicked(int index)
{
    if(index == 0)
    {
        delta1_name_label_->setText("delta X:");
        delta2_name_label_->setText("delta Y:");
        delta3_name_label_->setText("delta Z:");

        delta2_val_label_->show();
        delta2_add_button_->show();
        delta2_del_button_->show();
        delta2_name_label_->show();

        delta3_val_label_->show();
        delta3_add_button_->show();
        delta3_del_button_->show();
        delta3_name_label_->show();

        delta1_val_label_->setText("0");
        delta2_val_label_->setText("0");
        delta3_val_label_->setText("0");

        insert_box_->setCurrentIndex(2);
    }
    else if(index == 1)
    {
        delta1_name_label_->setText("delta Rx:");
        delta2_name_label_->setText("delta Ry:");
        delta3_name_label_->setText("delta Rz:");

        delta2_val_label_->show();
        delta2_add_button_->show();
        delta2_del_button_->show();
        delta2_name_label_->show();

        delta3_val_label_->show();
        delta3_add_button_->show();
        delta3_del_button_->show();
        delta3_name_label_->show();

        delta1_val_label_->setText("0");
        delta2_val_label_->setText("0");
        delta3_val_label_->setText("0");

        insert_box_->setCurrentIndex(2);
    }
    else
    {
        delta1_name_label_->setText("delta");

        delta2_val_label_->hide();
        delta2_add_button_->hide();
        delta2_del_button_->hide();
        delta2_name_label_->hide();

        delta3_val_label_->hide();
        delta3_add_button_->hide();
        delta3_del_button_->hide();
        delta3_name_label_->hide();

        delta1_val_label_->setText("0");

        insert_box_->setCurrentIndex(2);
    }
}

void SetAdjustWindow::AddClicked()
{
    auto *button = dynamic_cast<QPushButton*>(sender());
    if(button == nullptr)
        return;

    double xv = insert_box_->currentText().toDouble();
    double val = 0.0;
    QLabel *val_label = nullptr;

    if(button == delta1_add_button_)
    {
        val = delta1_val_label_->text().toDouble();
        val_label = delta1_val_label_;
    }
    else if(button == delta2_add_button_)
    {
        val = delta2_val_label_->text().toDouble();
        val_label = delta2_val_label_;
    }
    else if(button == delta3_add_button_)
    {
        val = delta3_val_label_->text().toDouble();
        val_label = delta3_val_label_;
    }
    else
    {
        return;
    }

    val += xv;
    val_label->setText(QString::number(val));
}

void SetAdjustWindow::DelClicked()
{
    auto *button = dynamic_cast<QPushButton*>(sender());
    if(button ==nullptr)
        return;

    double xv = insert_box_->currentText().toDouble();
    double val = 0.0;
    QLabel *val_label = nullptr;

    if(button == delta1_del_button_)
    {
        val = delta1_val_label_->text().toDouble();
        val_label = delta1_val_label_;
    }
    else if(button == delta2_del_button_)
    {
        val = delta2_val_label_->text().toDouble();
        val_label = delta2_val_label_;
    }
    else if(button == delta3_del_button_)
    {
        val = delta3_val_label_->text().toDouble();
        val_label = delta3_val_label_;
    }
    else
    {
        return;
    }

    val -= xv;
    val_label->setText(QString::number(val));
}

void SetAdjustWindow::ConnectInit()
{
    connect(mod_box_, SIGNAL(currentIndexChanged(int)), this, SLOT(ModBoxClicked(int)));

    connect(delta1_add_button_, &QPushButton::clicked, this, &SetAdjustWindow::AddClicked);

    connect(delta2_add_button_, &QPushButton::clicked, this, &SetAdjustWindow::AddClicked);

    connect(delta3_add_button_, &QPushButton::clicked, this, &SetAdjustWindow::AddClicked);

    connect(delta1_del_button_, &QPushButton::clicked, this, &SetAdjustWindow::DelClicked);

    connect(delta2_del_button_, &QPushButton::clicked, this, &SetAdjustWindow::DelClicked);

    connect(delta3_del_button_, &QPushButton::clicked, this, &SetAdjustWindow::DelClicked);
}

ChoiceWindow::ChoiceWindow(QWidget *parent) : HotEditObject (parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *label_layout = new QHBoxLayout;
    target_label_ = new QLabel(tr("目标"), this);
    compensate_label_ = new QLabel(tr("补偿"), this);
    label_layout->addWidget(target_label_);
    label_layout->addWidget(compensate_label_);
    label_layout->setContentsMargins(0, 0, 0, 0);
    label_layout->setSpacing(0);

    table_box_ = new TableBox(0, 0, this);

    layout->addLayout(label_layout);
    layout->addWidget(table_box_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(label_layout, 1);
    layout->setStretchFactor(table_box_, 7);

    target_label_->setFrameShape(QFrame::Box);
    compensate_label_->setFrameShape(QFrame::Box);
}

HotEditObject::HotEditObject(QWidget *parent) : QFrame (parent)
{
    this->setFrameShape(QFrame::Box);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
}

void HotEditObject::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void HotEditObject::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

ChoiceTableSub::ChoiceTableSub(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QHBoxLayout(this);

    name_label_ = new QLabel(this);

    auto *button_layout = new QHBoxLayout;
    redress_label_ = new QLabel("0", this);
    del_button_ = new QPushButton(this);
    del_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_layout->addWidget(redress_label_);
    button_layout->addWidget(del_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    button_layout->setStretchFactor(redress_label_, 1);
    button_layout->setStretchFactor(del_button_, 1);

    layout->addWidget(name_label_);
    layout->addLayout(button_layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(name_label_, 1);
    layout->setStretchFactor(button_layout, 1);

    del_button_->setFlat(true);
    del_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/del_button.png)");

    del_button_->hide();

    connect(del_button_, &QPushButton::clicked,
            [=]()
    {
        emit DelButtonClicked();
    });
}

void ChoiceTableSub::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        sub_pointer_ = this;
        emit Clicked();
    }
}

void ChoiceTableSub::SetName(QString name)
{
    name_label_->setText(name);
}

void ChoiceTableSub::SetRedress(QString val)
{
    redress_label_->setText(val);
}

QMap<QString, int> &ChoiceTableSub::GetMap()
{
    return save_sub_map_;
}

QPointer<ChoiceTableSub> ChoiceTableSub::GetPointer()
{
    return sub_pointer_;
}

void ChoiceTableSub::UpdateState()
{
    if(!sub_pointer_.isNull())
    {
        if(this == sub_pointer_.data())
        {
            if(del_button_->isHidden())
            {
                del_button_->show();
            }
        }
        else
        {
            if(!del_button_->isHidden())
            {
                del_button_->hide();
            }
        }
    }
}
