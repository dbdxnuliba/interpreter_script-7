#ifndef EVENTLOG_H
#define EVENTLOG_H

#include <QWidget>
#include <QLabel>
#include <QList>
#include <QReadWriteLock>
#include <QTimer>
#include <QLineEdit>
#include <QComboBox>

#include "teachstate.h"
#include "../listboxobject.h"
#include "../textbrowserobject.h"
#include "../stackedwidobject.h"
#include "../commoncontrols.h"
#include "../comboboxobject.h"

namespace robotTeachABB
{
    class SaveLogBox; //文件显示列表框，作为保存日志窗口的一个控件存在
    class LogWidget; //日志显示窗口
    class SLBWidget; //文件显示列表框中用于承载图标文件名文件类型的窗口，只会被作为一个元素添加到文件显示框中
    class SaveLogWidget; //保存日志窗口
    class EventLog; //事件日志主窗口，包裹着日志显示窗口和保存日志窗口
    class ShowLogInfo; //显示日志详细信息窗口
}

class ShowLogInfo : public QWidget
{
    Q_OBJECT
public:
    ShowLogInfo(QWidget *parent = nullptr);
    void Init(int row, QString type, QString data, QString time, QString info = QString());

signals:
    void Upper(int row);
    void Lower(int row);
    void Close();

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    void ConnectInit();

private:
    QLabel *title_label_;

    QFrame *data_widget_;
    QLabel *type_label_;
    QLabel *date_label_;
    TextBrowserObject *text_data_;

    QWidget *button_widget_;
    QPushButton *upper_button_;
    QPushButton *lower_button_;
    QPushButton *define_button_;

    int curr_row_;
}; //显示日志详细信息窗口

class SLBWidget : public QWidget
{
    Q_OBJECT
public:
    SLBWidget(const QString &name, const QString &type, QWidget *parent = nullptr);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *ico_label_; //图标
    QLabel *name_label_; //文件名
    QLabel *type_label_; //类型
    double scale_w_;
    double scale_h_;
};

class SaveLogBox : public ListBoxObject
{
    Q_OBJECT
public:
    SaveLogBox(QWidget *parent = nullptr);
    void AddWidget(QString data); //添加数据
    void AddWidget(QStringList data);
    void ClearWidget(); //清除列表中所有数据并释放空间

signals:
    void SendScale(double scale_w, double scale_h);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QList<QWidget*> widget_list_;
};


class LogWidget : public QWidget
{
    Q_OBJECT
public:
    LogWidget(QWidget *parent = nullptr);
    ~LogWidget();
    void resizeEvent(QResizeEvent *);
    void ShowRow(int row); //在弹出日志窗口后选中row行并切换到显示日志详细信息窗口

signals:
    void PoPLogWidget(int row); //弹出日志窗口

public slots:
    void SetScale(double scale_w, double scale_h);
    void GetLogTimer();
    void PushLog(EventLogType type, QString data, QString info = QString(), QString time = QString());

private slots:
    void DelLog();
    void LogBoxClicked(QTableWidgetItem *item);
    void Upper(int row);
    void Lower(int row);
    void Close();

private:
    void showEvent(QShowEvent *);
    void ReadLog();
    void WriteLog();
    void IsPoP(EventLogType type, int row); //决定是否需要弹出事件日志窗口

private:
    QLabel *tips_label; //提示标签
    QLabel *decorate_label_; //装饰标签
    QWidget *win_button_; //按钮窗口
    double scale_w_;
    double scale_h_;
    QTimer *get_log_timer_; //获取信息

    ShowLogInfo *show_log_info_; //显示日志详细信息窗口

    bool isinit_; //是否初始化完成，未完成时所添加的日志将不会弹窗
    QList<QString> log_info_;
    QReadWriteLock *mutex_;

public:
    QLabel *view_label_; //视图标签
    TableBox *log_box_; //日志列表
    QPushButton *save_button_; //保存日志
    ComboBoxObject *del_button_; //删除日志
    ComboBoxObject *update_button_; //更新
    ComboBoxObject *view_button_; //视图
};

class SaveLogWidget : public QWidget
{
    Q_OBJECT
public:
    SaveLogWidget(QWidget *parent = nullptr);
    void Init();
    void resizeEvent(QResizeEvent *);
    void FindFiles(QString path); //寻找某个路径下的所有非隐藏的文件夹和文件
    void Sort(); //排序

signals:

public slots:
    void SetScale(double scale_w, double scale_h);
    void TogglePages(int row); //切换页面
    void LastPages(); //上一页
    void ReturnHome(); //返回主目录

private slots:
    void SaveClicked();

    void ShowKeyboard();
    void KeyRetData(QString data);

private:
    QLabel *decorate_label_; //装饰标签
    QPushButton *sort_button_; //排序按钮
    QLabel *name_label_; //文件名标签
    QLineEdit *name_edit_; //文件名输入框
    QWidget *win_button_; //按钮窗口
    double scale_w_;
    double scale_h_;
    bool is_show_; //控制是否显示排序按钮
    bool pos_neg_; //控制显示的是正向排序还是反向排序
    QString path_; //当前路径
    QString home_path_; //主目录路径
    QStringList file_list_; //所有文件

    Keyboard *keyboard_;

public:
    QLabel *tips_label_; //提示标签
    QPushButton *show_button_; //显示排序按钮
    QPushButton *hide_button_; //隐藏排序按钮
    QComboBox *filter_combox_; //文件过滤下拉框
    QLabel *type_label_; //类型标签
    SaveLogBox *save_log_box_; //文件显示框
    QPushButton *keyboard_button_; //显示键盘按钮
    QPushButton *create_button_; //新建文件夹按钮
    QPushButton *upper_button_; //上一页按钮
    QPushButton *home_button_; //主目录按钮
    QPushButton *confirm_button_; //确认按钮
    QPushButton *cancel_button_; //取消按钮
};

class EventLog : public QFrame
{
    Q_OBJECT
public:
    explicit EventLog(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void ConnectInit();
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void ChangeLogWidget(int row); //当需要弹出日志窗口时切换到日志窗口

signals:
    void HideWindow();
    void ShowWindow();

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    StackedWidObject *stacked_widget_;

public:
    LogWidget *log_widget_; //日志窗口
    SaveLogWidget *save_log_widget_; //保存日志窗口
};

#endif // EVENTLOG_H
