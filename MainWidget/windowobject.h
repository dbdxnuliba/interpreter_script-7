#ifndef WINDOWOBJECT_H
#define WINDOWOBJECT_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QWidget>
#include <QLabel>

namespace robotTeachABB
{
    class WindowObject; //重写的窗口基类，将窗口划分为上中下三部分，上面是是一个图标加上功能名，下面是按钮存放区，中间是由子类完成的具体功能的显示
}

class WindowObject : public QWidget
{
    Q_OBJECT
public:
    explicit WindowObject(int type, QWidget *parent = nullptr);
    virtual void resizeEvent(QResizeEvent *);

signals:
    void ShowWindow(); //窗口显示信号
    void HideWindow(); //窗口隐藏信号

public slots:
    virtual void SetScale(double scale_w, double scale_h);

protected:
    QWidget *main_widget_; //主窗口，用于承载子类的功能
    QWidget *button_widget_; //下方的按钮窗口

protected:
    QLabel *GetTitleLabel();
    QLabel *GetIcoLabel();
    QLabel *GetUnderLabel();
    double ScaleW();
    double ScaleH();
    void showEvent(QShowEvent *); //发送窗口显示信号
    void hideEvent(QHideEvent *); //发送窗口隐藏信号

private:
    QLabel *ico_label_; //图标
    QLabel *title_label_; //标题
    QLabel *underline_; //下划线
    double scale_w_;
    double scale_h_;

private:
    void Init(int type); //设置图标
};

#endif // WINDOWOBJECT_H
