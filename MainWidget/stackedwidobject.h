#ifndef STACKEDWIDOBJECT_H
#define STACKEDWIDOBJECT_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QStackedWidget>
#include <QMap>

//更改了原版QStackedWidget通过索引切换显示页面的方法，现在可通过控件名称进行页面的切换

namespace robotTeachABB
{
    class StackedWidObject; //重写的一个多窗口切换类，能够更方便的将各个小功能整合成为一个大功能，并且提供能根据功能名切换窗口的功能
}

class StackedWidObject : public QStackedWidget
{
    Q_OBJECT
public:
    explicit StackedWidObject(QWidget *parent = nullptr);  
    void SetMainWidget(QWidget *widget); //设置主界面
    virtual bool AddWidget(QWidget *widget, QString name); //添加页面时可以为该页面指定名字
    bool Change(QString name = QString()); //切换页面，默认为切换到主界面
    QString GetName(int num = -1); //根据编号返回页面名字，num为-1时返回当前页面，num小于-1或大于页面总数时返回空
    bool IsExist(QString name); //用于判断某个界面是否存在
    QWidget *GetWidget(QString name = QString()); //返回窗口指针，默认返回主界面窗口，若窗口不存在返回nullptr
    int GetNum(QString name = QString()); //返回对应页面的编号，默认返回主界面0，若窗口不存在则返回-1
    int GetCount(); //返回总页数

signals:

public slots:
    void ChangeMain(); //切换到主界面

private:
    QMap<QString, int> map_page_num_;
};

#endif // STACKEDWIDOBJECT_H
