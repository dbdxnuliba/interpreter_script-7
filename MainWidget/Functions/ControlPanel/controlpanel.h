#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

#include "control.h"

namespace robotTeachABB
{
    class ControlPanel; //控制面板窗口
}

class ControlPanel : public WindowObject
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr, int type = 8);
    void resizeEvent(QResizeEvent *);
    void ConnectInit();

signals:

public slots:
    void SetScale(double scale_w, double scale_h);
    void SelectFunc(QTableWidgetItem *item); //根据列表框所点击的行数决定打开那个窗口
    void CancelReturnHome(); //点击取消以后返回主界面

private:
    StackedWidObject *stack_widget_;

private:
    void HideWidget(); //隐藏主窗口控件
    void ShowWidget(); //显示主窗口控件
    void TableInit();

public:
    TableBox *table_box_; //表格

public:
    Appearance *appearan_; //外观
    Monitor *monitor_; //监控
    FlexPendant *flex_pendant_; //FlexPendant
    InputOut *input_out_; //输入输出
    Language *language_; //语言
    ProgKeys *prog_keys_; //ProgKeys
    Dispose *dispose_; //配置
    TouchScreen *touch_screen_; //触摸屏
};

#endif // CONTROLPANEL_H
