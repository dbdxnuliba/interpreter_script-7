#ifndef LISTBOXOBJECT_H
#define LISTBOXOBJECT_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QScrollArea>
#include <QStyledItemDelegate>

//可更简单的将控件添加到列表框中，取消了通过滚轮上下移动功能，增加四个按钮负责进行上下移动

namespace robotTeachABB
{
    class LBScrollBar; //重写的一个滚动条类，作用是取消掉了对鼠标滚轮的响应
    class ListBoxObject; //重写的一个列表类，使其成为一个列表基类，功能：能够更方便的添加控件、不响应滚轮滚动、新增根据列表中数据数量而显示或隐藏的上下移动按钮
    class TableBox; //重写的一个表格类，功能与ListBoxObject类似，但更适合用于添加多列数据
    class NoFocus; //用于去除虚线框
}

class LBScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    LBScrollBar(QWidget *parent = nullptr);
    void wheelEvent(QWheelEvent *); //通过重写一个空的滚轮事件来拒绝响应鼠标滚轮
};

class NoFocus : public QStyledItemDelegate
{
    Q_OBJECT
public:
    NoFocus(QObject *parent = nullptr);
    ~NoFocus();

    void paint(QPainter *paint, const QStyleOptionViewItem &option, const QModelIndex &index) const;
}; //用于去除虚线框

class ListBoxObject : public QListWidget
{
    Q_OBJECT
public:
    explicit ListBoxObject(QWidget *parent = nullptr, double scale_w = 1, double scale_h = 1);
    virtual ~ListBoxObject();
    virtual void AddWidget(QWidget *widget); //以默认间隙添加控件
    virtual void AddWidget(QWidget *widget, QSize size); //以指定间隙添加控件
    virtual QListWidgetItem *AddText(QString text, bool isedit = true); //添加文字
    virtual QListWidgetItem *AddText(QString text, int number, bool isedit = true); //在指定位置的前一位处添加文字
    virtual QListWidgetItem *AddText(QString ico_path, QString text, bool isedit = true); //添加带图标的文字
    virtual QListWidgetItem *AddText(QString ico_path, QString text, int number, bool isedit = true);
    virtual void RemoveItem(QListWidgetItem *item); //删除元素
    virtual void RemoveRow(int row);
    virtual void resizeEvent(QResizeEvent *);
    void ConnectInit();

protected:
    double GetScaleW(); //将缩放比例导出供子类使用
    double GetScaleH();
    QPushButton *GetUpperButton(); //将按钮导出供子类进行修改
    QPushButton *GetUppersButton();
    QPushButton *GetLowerButton();
    QPushButton *GetLowersButton();
    LBScrollBar *GetScrollBar(); //将滚动条导出供子类进行修改

signals:

public slots:
    virtual void SetScale(double scale_w, double scale_h);

private:
    double scale_w_;
    double scale_h_;
    QPushButton *upper_button_;
    QPushButton *lower_button_;
    QPushButton *uppers_button_;
    QPushButton *lowers_button_;
    LBScrollBar *lbscrollbar_; //自己重新实现的垂直滚动条，用于防止响应鼠标滚轮
};

class TableBox : public QTableWidget
{
    Q_OBJECT
public:
    TableBox(int rows, int colu, QWidget *parent = nullptr); //当colu大于1时为表格模式，等于1时为列表模式
    TableBox(QWidget *parent = nullptr);
    virtual ~TableBox();
    virtual void resizeEvent(QResizeEvent *event);
    void SetRow(int row); //设置行数
    void SetColumn(int column); //设置列数
    void SetRomn(int row, int column); //设置行数与列数
    int GetRow(); //获取行数
    int GetColumn(); //获取列数
    virtual QPair<int,int> AddWidget(int row, int col, QWidget *widget); //添加控件
    QPair<int, int> AddText(int row, int col, QString text); //添加文本

public slots:
    virtual void SetScale(double scale_w, double scale_h);


private:
    void TableBoxInit();
    void TableBoxConnect();

private:
    double scale_w_, scale_h_;
    QPushButton *upper_button_;
    QPushButton *lower_button_;
    QPushButton *uppers_button_;
    QPushButton *lowers_button_;
    LBScrollBar *lbscrollbar_; //自己重新实现的垂直滚动条，用于防止响应鼠标滚轮
};

#endif // LISTBOXOBJECT_H
