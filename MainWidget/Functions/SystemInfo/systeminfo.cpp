#include "systeminfo.h"
#include <teachstate.h>

SystemInfo::SystemInfo(QWidget *parent, int type) : WindowObject(type, parent)
{
    attribute_ = new QListWidget(main_widget_);
    refresh_button_ = new QPushButton(tr("刷新"), main_widget_);
    text_widget_ = new SystemInfoText(main_widget_);

    refresh_button_->setEnabled(false);

    this->button_widget_->hide();

    attribute_->addItem(tr("示教器版本"));

    ConnectInit();
}

void SystemInfo::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    attribute_->resize(main_widget_->width()/2, main_widget_->height());
    refresh_button_->resize(140*ScaleW(), 40*ScaleH());
    text_widget_->resize(main_widget_->width() - attribute_->width(), main_widget_->height() - refresh_button_->height());

    attribute_->move(0, 0);
    refresh_button_->move(attribute_->x() + attribute_->width() + 80*ScaleW(), main_widget_->height() - refresh_button_->height());
    text_widget_->move(attribute_->x() + attribute_->width(), 0);
}

void SystemInfo::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);
}

void SystemInfo::ListItemClicked(QListWidgetItem *item)
{
    if(item == nullptr)
        return;
    int row = attribute_->row(item);

    if(row == 0)
    {
        text_widget_->setText(TeachState::instance()->GetTeachVersion());
    }
}

void SystemInfo::ConnectInit()
{
    connect(attribute_, &QListWidget::itemClicked, this, &SystemInfo::ListItemClicked);
}

SystemInfoText::SystemInfoText(QWidget *parent) : QTextBrowser (parent)
{
    this->setAlignment(Qt::AlignCenter);

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setContextMenuPolicy(Qt::NoContextMenu);
}

void SystemInfoText::mousePressEvent(QMouseEvent *)
{
}

void SystemInfoText::mouseDoubleClickEvent(QMouseEvent *)
{
}

void SystemInfoText::mouseMoveEvent(QMouseEvent *)
{
}

void SystemInfoText::mouseReleaseEvent(QMouseEvent *)
{
}
