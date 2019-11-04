#include "textbrowserobject.h"
#include <QTextDocument>
#include <QTextCursor>
#include <QDebug>

TextBrowserObject::TextBrowserObject(QWidget *parent) : QTextBrowser(parent)
{
    upper_button_ = new QPushButton(this);
    lower_button_ = new QPushButton(this);
    uppers_button_ = new QPushButton(this);
    lowers_button_ = new QPushButton(this);
    tbscrollbar_ = new TBScrollBar(this);

    scale_w_ = 1;
    scale_h_ = 1;

    upper_button_->resize(55*scale_w_, 42*scale_h_);
    lower_button_->resize(55*scale_w_, 42*scale_h_);
    uppers_button_->resize(55*scale_w_, 42*scale_h_);
    lowers_button_->resize(55*scale_w_, 42*scale_h_);

    upper_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_upper_button.png)");
    lower_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_lower_button.png)");
    uppers_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_uppers_button.png)");
    lowers_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/listboxobject_lowers_button.png)");

    this->setVerticalScrollBar(tbscrollbar_); //将默认垂直滚动条替换成自己重新实现的滚动条
    this->setStyleSheet("QScrollBar{width: 8px; background: rgba(126, 192, 238, 40%)}"); //调整滚动条大小以及颜色
    this->setContextMenuPolicy(Qt::NoContextMenu); //禁止右键鼠标弹出菜单

    ConnectInit();

    upper_button_->hide();
    uppers_button_->hide();
    lower_button_->hide();
    lowers_button_->hide();
}

TextBrowserObject::~TextBrowserObject()
{
}

void TextBrowserObject::AddData(const QString &data)
{
    this->append(data);


}

void TextBrowserObject::AddData(const QStringList &data)
{
    for(auto str : data)
        this->append(str);
}

void TextBrowserObject::resizeEvent(QResizeEvent *e)
{
    QTextBrowser::resizeEvent(e); //必须添加这句否则之后所添加的数据将会无法显示

    upper_button_->resize(55*scale_w_, 42*scale_h_);
    lower_button_->resize(55*scale_w_, 42*scale_h_);
    uppers_button_->resize(55*scale_w_, 42*scale_h_);
    lowers_button_->resize(55*scale_w_, 42*scale_h_);

    upper_button_->move(this->width() - upper_button_->width() - 7*scale_w_, 4*scale_h_);
    lower_button_->move(this->width() - lower_button_->width() - 7*scale_w_, this->height() - lower_button_->height() - 4*scale_h_);
    uppers_button_->move(upper_button_->x() - uppers_button_->width(), upper_button_->y());
    lowers_button_->move(lower_button_->x() - lowers_button_->width(), lower_button_->y());

    UpLoIsHide();
}

int TextBrowserObject::GetRow()
{
    QTextDocument *document = this->document();
    return document->lineCount();
}

double TextBrowserObject::GetScaleW()
{
    return scale_w_;
}

double TextBrowserObject::GetScaleH()
{
    return scale_h_;
}

QPushButton *TextBrowserObject::GetUpperButton()
{
    return upper_button_;
}

QPushButton *TextBrowserObject::GetUppersButton()
{
    return uppers_button_;
}

QPushButton *TextBrowserObject::GetLowerButton()
{
    return lower_button_;
}

QPushButton *TextBrowserObject::GetLowersButton()
{
    return lowers_button_;
}

TBScrollBar *TextBrowserObject::GetScrollBar()
{
    return tbscrollbar_;
}

void TextBrowserObject::UpLoIsHide()
{
    if(tbscrollbar_->maximum() != 0)
    {
        if(tbscrollbar_->value() <= tbscrollbar_->minimum())
        {
            upper_button_->hide();
            uppers_button_->hide();

            lower_button_->show();
            lowers_button_->show();
        }
        else if(tbscrollbar_->value() >= tbscrollbar_->maximum())
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

void TextBrowserObject::ConnectInit()
{
    TBScrollBar *lbscrollbar_ = tbscrollbar_;

    connect(upper_button_, &QPushButton::clicked,
            [=]()
    {
        lbscrollbar_->setValue(lbscrollbar_->value() - 10);

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
        lbscrollbar_->setValue(lbscrollbar_->value() + 10);

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
        lbscrollbar_->setValue(lbscrollbar_->value() - 100);

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
        lbscrollbar_->setValue(lbscrollbar_->value() + 100);

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

void TextBrowserObject::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
}

TBScrollBar::TBScrollBar(QWidget *parent) : QScrollBar(parent)
{

}

void TBScrollBar::wheelEvent(QWheelEvent *)
{

}
