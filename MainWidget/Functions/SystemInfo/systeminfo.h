#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QListWidget>

#include "../../windowobject.h"

namespace robotTeachABB
{
    class SystemInfo;
    class SystemInfoText;
}

class SystemInfoText : public QTextBrowser
{
    Q_OBJECT
public:
    SystemInfoText(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
};

class SystemInfo : public WindowObject
{
    Q_OBJECT
public:
    explicit SystemInfo(QWidget *parent = nullptr, int type = 11);
    void resizeEvent(QResizeEvent *);

signals:

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ListItemClicked(QListWidgetItem *item);

private:
    void ConnectInit();

private:
    QListWidget *attribute_; //属性
    QPushButton *refresh_button_; //属性
    SystemInfoText *text_widget_;
};

#endif // SYSTEMINFO_H
