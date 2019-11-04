#ifndef COMBOBOXOBJECT_H
#define COMBOBOXOBJECT_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QPushButton>
#include <QAction>
#include <QLabel>
#include <QMenu>

//实现了菜单按钮，点击按钮后在按钮上方弹出菜单

namespace robotTeachABB
{
    class ComboBoxObject; //重写的一个按钮类，使其成为一个菜单按钮基类，被点击后会在按钮上方弹出一个菜单
    class ComboMenu; //重写的一个菜单类，功能是使ComboBoxObject类得到菜单的显示与关闭信号
}

class ComboMenu : public QMenu
{
    Q_OBJECT
public:
    ComboMenu(QWidget *parent = nullptr);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

signals:
    void ShowWindow();
    void HideWindow();
};

class ComboBoxObject : public QPushButton
{
    Q_OBJECT
public:
    explicit ComboBoxObject(QWidget *parent = nullptr);
    explicit ComboBoxObject(QString name, QWidget *parent = nullptr);
    ~ComboBoxObject();
    void SetIsMenu(bool ismenu); //用于修改是否显示菜单
    void AppAction(QAction *action); //向菜单添加菜单项
    void AppSeparator(); //向菜单添加分隔符
    void SetFilterButton(); //将按钮设置为过滤器按钮
    void resizeEvent(QResizeEvent *);
    ComboMenu *GetMenu();

signals:
    void LowerSig(); //箭头向下后的信号
    void UpperSig(); //箭头向上后的信号

public slots:
    void LowerArrow(); //使箭头向下
    void UpperArrow(); //使箭头向上

protected:
    QLabel *GetIcoLabel();

private slots:
    void ShowMenu(); //将普通按钮改造成菜单按钮的核心功能

private:
    void Init();

private:
    ComboMenu *menu_;
    QLabel *ico_label_;
    bool ismenu_; //用于控制是否需要显示菜单
};

#endif // COMBOBOXOBJECT_H
