#include "comboboxobject.h"

ComboBoxObject::ComboBoxObject(QWidget *parent) : QPushButton(parent)
{
   Init();
}

ComboBoxObject::ComboBoxObject(QString name, QWidget *parent) : QPushButton(name, parent)
{
    Init();
}

ComboBoxObject::~ComboBoxObject()
{
    delete ico_label_;
}

void ComboBoxObject::SetIsMenu(bool ismenu)
{
    ismenu_ = ismenu;
}

void ComboBoxObject::AppAction(QAction *action)
{
    menu_->addAction(action);
}

void ComboBoxObject::AppSeparator()
{
    menu_->addSeparator();
}

void ComboBoxObject::SetFilterButton()
{
    auto *qss = ".ComboBoxObject{border-image: url(:/image/Image/MainWidget/routine_filter_button.png);}";

    this->setStyleSheet(qss);
}

void ComboBoxObject::resizeEvent(QResizeEvent *)
{
    ico_label_->resize(this->width()/7, this->height()/3);

    ico_label_->move(this->width() - ico_label_->width(), 0);
}

void ComboBoxObject::LowerArrow()
{
    this->setFlat(false);
    ico_label_->setText("▼");

    emit LowerSig();
}

void ComboBoxObject::UpperArrow()
{
    this->setFlat(true);
    ico_label_->setText("▲");

    emit UpperSig();
}

ComboMenu *ComboBoxObject::GetMenu()
{
    return menu_;
}

QLabel *ComboBoxObject::GetIcoLabel()
{
    return ico_label_;
}

void ComboBoxObject::ShowMenu()
{
    if(!ismenu_) //不显示菜单
        return;
    QList<QAction*> list_action = menu_->actions();
    if(list_action.count() == 0) //当菜单为空时不显示菜单
        return;

    menu_->setMinimumWidth(this->width()); //指定菜单最下宽度与按钮宽度相等
    menu_->setMaximumWidth(this->width()); //设置菜单最大宽度与按钮宽度相等

    QPoint pos;
    pos.setX(0);
    pos.setY(-menu_->sizeHint().height());

    menu_->exec(this->mapToGlobal(pos));
}

void ComboBoxObject::Init()
{
    menu_ = new ComboMenu(this);
    ismenu_ = true; //默认显示
    ico_label_ = new QLabel("▲", this);

    connect(this, &ComboBoxObject::clicked, this, &ComboBoxObject::ShowMenu); //使弹出菜单在按钮上方
    connect(menu_, &ComboMenu::ShowWindow,
            [=]()
    {
        this->LowerArrow();
    });
    connect(menu_, &ComboMenu::HideWindow,
            [=]()
    {
        this->UpperArrow();
    });
}

ComboMenu::ComboMenu(QWidget *parent) : QMenu(parent)
{

}

void ComboMenu::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void ComboMenu::hideEvent(QHideEvent *)
{
    emit HideWindow();
}
