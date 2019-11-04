#include "stackedwidobject.h"

StackedWidObject::StackedWidObject(QWidget *parent) : QStackedWidget(parent)
{
}

void StackedWidObject::SetMainWidget(QWidget *widget)
{
    if(this->count() != 0) //如果0索引已经存在页面则进行替换
    {
        this->removeWidget(this->widget(0)); //先移除原有页面但需要注意这里并不从内存中释放该页面
    }
    this->insertWidget(0, widget);

    map_page_num_.insert("main", 0);
}

bool StackedWidObject::AddWidget(QWidget *widget, QString name)
{
    if(this->count() == 0) //当未设置主界面时不允许添加新的页面，为了保证主页面索引必须为0
    {
        return false;
    }

    if(!name.isEmpty())
    {
        if(!map_page_num_.contains(name)) //检查该名称是否已经有某一个页面拥有
        {
            int num = this->addWidget(widget);
            map_page_num_.insert(name, num);
            return true;
        }
        else
            return false;
    }
    else
    {
        return false;
    }
}

bool StackedWidObject::Change(QString name)
{
    if(name.isEmpty()) //跳转到主界面
    {
        this->setCurrentIndex(0);
        return true;
    }
    else
    {
        if(map_page_num_.contains(name))
        {
            this->setCurrentIndex(map_page_num_[name]); //跳转到对应页面
            return true;
        }
        else
            return false;
    }
}

QString StackedWidObject::GetName(int num)
{
    if(num < -1 || num >= map_page_num_.count()) //超出索引
        return QString();

    if(num == -1 && map_page_num_.size() == 0) //超出索引
    {
        return QString();
    }
    else if(num == -1)
    {
        num = this->currentIndex();
    }

    for(auto it = map_page_num_.begin(); it != map_page_num_.end(); it++)
    {
        if(it.value() == num)
            return it.key();
    }
    return QString();
}

bool StackedWidObject::IsExist(QString name)
{
    if(map_page_num_.contains(name)) //如果存在
        return true;
    else
        return false;
}

QWidget *StackedWidObject::GetWidget(QString name)
{
    if(name.isEmpty()) //主界面
    {
        return this->widget(0);
    }
    if(!map_page_num_.contains(name)) //如果窗口不存在
        return nullptr;
    else
        return this->widget(map_page_num_[name]);
}

int StackedWidObject::GetNum(QString name)
{
    if(name.isEmpty()) //主界面
        return 0;
    if(!map_page_num_.contains(name)) //不存在
        return -1;
    else
        return map_page_num_[name];
}

int StackedWidObject::GetCount()
{
    return map_page_num_.size();
}

void StackedWidObject::ChangeMain()
{
    Change();
}
