#ifndef TEXTBROWSEROBJECT_H
#define TEXTBROWSEROBJECT_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QTextBrowser>
#include <QPushButton>
#include <QScrollBar>

namespace robotTeachABB
{
    class TBScrollBar; //重写的一个滚动条类，作用是取消对鼠标滚轮的响应
    class TextBrowserObject; //重写的文本框基类
}

class TBScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    TBScrollBar(QWidget *parent = nullptr);
    void wheelEvent(QWheelEvent *); //通过重写一个空的滚轮事件来拒绝响应鼠标滚轮
};

class TextBrowserObject : public QTextBrowser
{
    Q_OBJECT
public:
    explicit TextBrowserObject(QWidget *parent = nullptr);
    virtual ~TextBrowserObject();
    virtual void AddData(const QString &data); //添加文本
    virtual void AddData(const QStringList &data); //批量添加文本
    void resizeEvent(QResizeEvent *e);
    int GetRow(); //获取总行数

public:
    double GetScaleW(); //将缩放比例导出
    double GetScaleH();
    QPushButton *GetUpperButton(); //将按钮导出
    QPushButton *GetUppersButton();
    QPushButton *GetLowerButton();
    QPushButton *GetLowersButton();
    TBScrollBar *GetScrollBar(); //将滚动条导出
    virtual void UpLoIsHide(); //用于根据情况判断上下按钮是否隐藏或显示

private:
    void ConnectInit();

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
    TBScrollBar *tbscrollbar_; //自己重新实现的垂直滚动条，用于防止响应鼠标滚轮
};

#endif // TEXTBROWSEROBJECT_H
