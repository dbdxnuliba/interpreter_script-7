#include "listboxobject.h"
#include <QHeaderView>

ListBoxObject::ListBoxObject(QWidget *parent, double scale_w, double scale_h) : QListWidget(parent)
{
    upper_button_ = new QPushButton(this);
    lower_button_ = new QPushButton(this);
    uppers_button_ = new QPushButton(this);
    lowers_button_ = new QPushButton(this);
    lbscrollbar_ = new LBScrollBar(this);

    scale_w_ = scale_w;
    scale_h_ = scale_h;

    double &scalew = scale_w_;
    double &scaleh = scale_h_;

    upper_button_->resize(55*scalew, 42*scaleh);
    lower_button_->resize(55*scalew, 42*scaleh);
    uppers_button_->resize(55*scalew, 42*scaleh);
    lowers_button_->resize(55*scalew, 42*scaleh);

    upper_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_upper_button.png)");
    lower_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_lower_button.png)");
    uppers_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_uppers_button.png)");
    lowers_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_lowers_button.png)");

    this->setVerticalScrollBar(lbscrollbar_); //将默认垂直滚动条替换成自己重新实现的滚动条
    this->setStyleSheet("QScrollBar{width: 5px; background: rgba(105, 89, 205, 40%)}"); //调整滚动条大小以及颜色
    this->setItemDelegate(new NoFocus(this)); //去除虚线框

    ConnectInit();
}

ListBoxObject::~ListBoxObject()
{
}

void ListBoxObject::AddWidget(QWidget *widget)
{
    QListWidgetItem *item = new QListWidgetItem;

    item->setSizeHint(widget->size()); //在未指定使用多大间隙时使用widget的大小

    this->addItem(item);
    this->setItemWidget(item, widget);
}

void ListBoxObject::AddWidget(QWidget *widget, QSize size)
{
    QListWidgetItem *item = new QListWidgetItem;

    item->setSizeHint(size);

    this->addItem(item);
    this->setItemWidget(item, widget);

    if(lbscrollbar_->maximum() != 0)
    {
        if(lbscrollbar_->value() <= lbscrollbar_->minimum())
        {
            upper_button_->hide();
            uppers_button_->hide();

            lower_button_->show();
            lowers_button_->show();
        }
        else if(lbscrollbar_->value() >= lbscrollbar_->maximum())
        {
            upper_button_->show();
            uppers_button_->show();

            lower_button_->hide();
            lowers_button_->hide();
        }
        else
        {
            upper_button_->show();
            uppers_button_->show();

            lower_button_->show();
            lowers_button_->show();
        }
    }
    else
    {
        upper_button_->hide();
        uppers_button_->hide();
        lower_button_->hide();
        lowers_button_->hide();
    }
}

QListWidgetItem *ListBoxObject::AddText(QString text, bool isedit)
{
    auto *item = new QListWidgetItem(text);

    this->addItem(item);

    if(!isedit)
        item->setFlags(Qt::NoItemFlags); //设置不可点击

    return item;
}

QListWidgetItem *ListBoxObject::AddText(QString text, int number, bool isedit)
{
    auto *item = new QListWidgetItem(text);

    this->insertItem(number, item);

    if(!isedit)
        item->setFlags(Qt::NoItemFlags);

    return item;
}

QListWidgetItem *ListBoxObject::AddText(QString ico_path, QString text, bool isedit)
{
    auto *item = new QListWidgetItem(QIcon(ico_path), text);

    this->addItem(item);

    if(!isedit)
        item->setFlags(Qt::NoItemFlags);

    return item;
}

QListWidgetItem *ListBoxObject::AddText(QString ico_path, QString text, int number, bool isedit)
{
    auto *item = new QListWidgetItem(QIcon(ico_path), text);

    this->insertItem(number, item);

    if(!isedit)
        item->setFlags(Qt::NoItemFlags);

    return item;
}

void ListBoxObject::RemoveItem(QListWidgetItem *item)
{
    auto *del_item = this->takeItem(this->row(item));

    if(del_item != nullptr)
        delete del_item;
}

void ListBoxObject::RemoveRow(int row)
{
    auto *del_item = this->takeItem(row);

    if(del_item != nullptr)
        delete del_item;
}

void ListBoxObject::resizeEvent(QResizeEvent *)
{
    double &scalew = scale_w_;
    double &scaleh = scale_h_;

    upper_button_->resize(55*scalew, 42*scaleh);
    lower_button_->resize(55*scalew, 42*scaleh);
    uppers_button_->resize(55*scalew, 42*scaleh);
    lowers_button_->resize(55*scalew, 42*scaleh);

    upper_button_->move(this->width() - upper_button_->width() - 7*scalew, 4*scaleh);
    lower_button_->move(this->width() - lower_button_->width() - 7*scalew, this->height() - lower_button_->height() - 4*scaleh);
    uppers_button_->move(upper_button_->x() - uppers_button_->width(), upper_button_->y());
    lowers_button_->move(lower_button_->x() - lowers_button_->width(), lower_button_->y());

    if(lbscrollbar_->maximum() == 0)
    {
        upper_button_->hide();
        uppers_button_->hide();
        lower_button_->hide();
        lowers_button_->hide();
    }
    else
    {
        if(lbscrollbar_->value() <= lbscrollbar_->minimum())
        {
            upper_button_->hide();
            uppers_button_->hide();

            lower_button_->show();
            lowers_button_->show();
        }
        else if(lbscrollbar_->value() >= lbscrollbar_->maximum())
        {
            upper_button_->show();
            uppers_button_->show();

            lower_button_->hide();
            lowers_button_->hide();
        }
        else
        {
            upper_button_->show();
            uppers_button_->show();

            lower_button_->show();
            lowers_button_->show();
        }
    }
}

void ListBoxObject::ConnectInit()
{
    connect(upper_button_, &QPushButton::clicked,
            [=]()
    {
        lbscrollbar_->setValue(lbscrollbar_->value() - 1);

        if(lbscrollbar_->value() <= lbscrollbar_->minimum()) //当滚到最小值时隐藏向上滚按钮
        {
            upper_button_->hide();
            uppers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0) //排除掉不需要滚动条时的情况
        {
            if(lower_button_->isHidden())
            {
                lower_button_->show();
                lowers_button_->show();
            }
        }
    });

    connect(lower_button_, &QPushButton::clicked,
            [=]()
    {
        lbscrollbar_->setValue(lbscrollbar_->value() + 1);

        if(lbscrollbar_->value() >= lbscrollbar_->maximum())
        {
            lower_button_->hide();
            lowers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0)
        {
            if(upper_button_->isHidden())
            {
                upper_button_->show();
                uppers_button_->show();
            }
        }
    });

    connect(uppers_button_, &QPushButton::clicked,
            [=]()
    {
        int num = this->count() - lbscrollbar_->maximum();
        lbscrollbar_->setValue(lbscrollbar_->value() - num);

        if(lbscrollbar_->value() <= lbscrollbar_->minimum()) //当滚到最小值时隐藏向上滚按钮
        {
            upper_button_->hide();
            uppers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0) //排除掉不需要滚动条时的情况
        {
            if(lower_button_->isHidden())
            {
                lower_button_->show();
                lowers_button_->show();
            }
        }
    });

    connect(lowers_button_, &QPushButton::clicked,
            [=]()
    {
        int num = this->count() - lbscrollbar_->maximum();
        lbscrollbar_->setValue(lbscrollbar_->value() + num);

        if(lbscrollbar_->value() >= lbscrollbar_->maximum())
        {
            lower_button_->hide();
            lowers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0)
        {
            if(upper_button_->isHidden())
            {
                upper_button_->show();
                uppers_button_->show();
            }
        }
    });
}

double ListBoxObject::GetScaleW()
{
    return scale_w_;
}

double ListBoxObject::GetScaleH()
{
    return scale_h_;
}

QPushButton *ListBoxObject::GetUpperButton()
{
    return upper_button_;
}

QPushButton *ListBoxObject::GetUppersButton()
{
    return uppers_button_;
}

QPushButton *ListBoxObject::GetLowerButton()
{
    return lower_button_;
}

QPushButton *ListBoxObject::GetLowersButton()
{
    return lowers_button_;
}

LBScrollBar *ListBoxObject::GetScrollBar()
{
    return lbscrollbar_;
}

void ListBoxObject::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

LBScrollBar::LBScrollBar(QWidget *parent) : QScrollBar(parent)
{
}

void LBScrollBar::wheelEvent(QWheelEvent *)
{
}

TableBox::TableBox(int rows, int colu, QWidget *parent) : QTableWidget (rows, colu, parent)
{
    TableBoxInit();
}

TableBox::TableBox(QWidget *parent) : QTableWidget (parent)
{
    TableBoxInit();
    this->SetRomn(0, 0);
}

TableBox::~TableBox()
{
}

void TableBox::resizeEvent(QResizeEvent *event)
{
    QTableWidget::resizeEvent(event); //为保证单元格能够被正常拉伸此行必须添加

    double &scalew = scale_w_;
    double &scaleh = scale_h_;

    upper_button_->resize(55*scalew, 42*scaleh);
    lower_button_->resize(55*scalew, 42*scaleh);
    uppers_button_->resize(55*scalew, 42*scaleh);
    lowers_button_->resize(55*scalew, 42*scaleh);

    upper_button_->move(this->width() - upper_button_->width() - 7*scalew, 4*scaleh);
    lower_button_->move(this->width() - lower_button_->width() - 7*scalew, this->height() - lower_button_->height() - 4*scaleh);
    uppers_button_->move(upper_button_->x() - uppers_button_->width(), upper_button_->y());
    lowers_button_->move(lower_button_->x() - lowers_button_->width(), lower_button_->y());

    if(lbscrollbar_->maximum() == 0)
    {
        upper_button_->hide();
        uppers_button_->hide();
        lower_button_->hide();
        lowers_button_->hide();
    }
    else
    {
        if(lbscrollbar_->value() <= lbscrollbar_->minimum())
        {
            upper_button_->hide();
            uppers_button_->hide();

            lower_button_->show();
            lowers_button_->show();
        }
        else if(lbscrollbar_->value() >= lbscrollbar_->maximum())
        {
            upper_button_->show();
            uppers_button_->show();

            lower_button_->hide();
            lowers_button_->hide();
        }
        else
        {
            upper_button_->show();
            uppers_button_->show();

            lower_button_->show();
            lowers_button_->show();
        }
    }
}

void TableBox::SetRow(int row)
{
    this->setRowCount(row);
}

void TableBox::SetColumn(int column)
{
    this->setColumnCount(column);
}

void TableBox::SetRomn(int row, int column)
{
    this->SetRow(row);
    this->SetColumn(column);
}

int TableBox::GetRow()
{
    return this->rowCount();
}

int TableBox::GetColumn()
{
    return this->columnCount();
}

QPair<int, int> TableBox::AddWidget(int row, int col, QWidget *widget)
{
    int ts_row = this->GetRow();
    int ts_col = this->GetColumn();

    if(ts_row > row && ts_col > col)
    {
        this->setCellWidget(row, col, widget);
        return qMakePair(row, col);
    }
    else if(ts_row <= row && ts_col > col)
    {
        this->insertRow(ts_row);
        this->setCellWidget(ts_row, col, widget);
        return qMakePair(ts_row, col);
    }
    else if(ts_row <= row && ts_col <= col)
    {
        this->insertRow(ts_row);
        this->insertColumn(ts_col);
        this->setCellWidget(ts_row, ts_col, widget);
        return qMakePair(ts_row, ts_col);
    }
    else if(ts_row > row && ts_col <= col)
    {
        this->insertColumn(ts_col);
        this->setCellWidget(row, ts_col, widget);
        return qMakePair(row, ts_col);
    }
    return qMakePair(-1, -1);
}

QPair<int,int> TableBox::AddText(int row, int col, QString text)
{
    int ts_row = this->GetRow();
    int ts_col = this->GetColumn();

    if(ts_row > row && ts_col > col)
    {
        this->setItem(row, col, new QTableWidgetItem(text));
        return qMakePair(row, col);
    }
    else if(ts_row <= row && ts_col > col)
    {
        this->insertRow(ts_row);
        this->setItem(ts_row, col, new QTableWidgetItem(text));
        return qMakePair(ts_row, col);
    }
    else if(ts_row <= row && ts_col <= col)
    {
        this->insertRow(ts_row);
        this->insertColumn(ts_col);
        this->setItem(ts_row, ts_col, new QTableWidgetItem(text));
        return qMakePair(ts_row, ts_col);
    }
    else if(ts_row > row && ts_col <= col)
    {
        this->insertColumn(ts_col);
        this->setItem(row, ts_col, new QTableWidgetItem(text));
        return qMakePair(row, ts_col);
    }
    return  qMakePair(-1, -1);
}

void TableBox::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
    resizeEvent(nullptr);
}

void TableBox::TableBoxInit()
{
    this->resize(640, 213);

    upper_button_ = new QPushButton(this);
    lower_button_ = new QPushButton(this);
    uppers_button_ = new QPushButton(this);
    lowers_button_ = new QPushButton(this);
    lbscrollbar_ = new LBScrollBar(this);

    this->SetScale(1, 1);
    upper_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_upper_button.png)");
    lower_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_lower_button.png)");
    uppers_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_uppers_button.png)");
    lowers_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_lowers_button.png)");

    this->setVerticalScrollBar(lbscrollbar_); //将默认垂直滚动条替换成自己重新实现的滚动条
    this->setStyleSheet("QScrollBar{width: 5px; background: rgba(105, 89, 205, 40%)}"); //调整滚动条大小以及颜色

    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //设置表格列宽自动随窗口大小进行调整
    this->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置禁止编辑
    this->setItemDelegate(new NoFocus(this)); //去除虚线框

    this->setShowGrid(false); //隐藏网格线
    this->horizontalHeader()->hide(); //隐藏列表头
    this->verticalHeader()->hide(); //隐藏行表头

    TableBoxConnect();
}

void TableBox::TableBoxConnect()
{
    connect(upper_button_, &QPushButton::clicked,
            [=]()
    {
        lbscrollbar_->setValue(lbscrollbar_->value() - 1);

        if(lbscrollbar_->value() <= lbscrollbar_->minimum()) //当滚到最小值时隐藏向上滚按钮
        {
            upper_button_->hide();
            uppers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0) //排除掉不需要滚动条时的情况
        {
            if(lower_button_->isHidden())
            {
                lower_button_->show();
                lowers_button_->show();
            }
        }
    });

    connect(lower_button_, &QPushButton::clicked,
            [=]()
    {
        lbscrollbar_->setValue(lbscrollbar_->value() + 1);

        if(lbscrollbar_->value() >= lbscrollbar_->maximum())
        {
//            lower_button_->hide(); //隐藏后会出现自动跳回最顶上
//            lowers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0)
        {
            if(upper_button_->isHidden())
            {
                upper_button_->show();
                uppers_button_->show();
            }
        }
    });

    connect(uppers_button_, &QPushButton::clicked,
            [=]()
    {
        int num = this->rowCount() - lbscrollbar_->maximum();
        lbscrollbar_->setValue(lbscrollbar_->value() - num);

        if(lbscrollbar_->value() <= lbscrollbar_->minimum()) //当滚到最小值时隐藏向上滚按钮
        {
            upper_button_->hide();
            uppers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0) //排除掉不需要滚动条时的情况
        {
            if(lower_button_->isHidden())
            {
                lower_button_->show();
                lowers_button_->show();
            }
        }
    });

    connect(lowers_button_, &QPushButton::clicked,
            [=]()
    {
        int num = this->rowCount() - lbscrollbar_->maximum();
        lbscrollbar_->setValue(lbscrollbar_->value() + num);

        if(lbscrollbar_->value() >= lbscrollbar_->maximum())
        {
//            lower_button_->hide();
//            lowers_button_->hide();
        }
        if(lbscrollbar_->maximum() != 0)
        {
            if(upper_button_->isHidden())
            {
                upper_button_->show();
                uppers_button_->show();
            }
        }
    });
}

NoFocus::NoFocus(QObject *parent) : QStyledItemDelegate (parent)
{
}

NoFocus::~NoFocus()
{
}

void NoFocus::paint(QPainter *paint, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemopt(option);
    if(itemopt.state & QStyle::State_HasFocus) //去除虚线框
    {
        itemopt.state = itemopt.state ^ QStyle::State_HasFocus;
    }
    QStyledItemDelegate::paint(paint, itemopt, index);
}
