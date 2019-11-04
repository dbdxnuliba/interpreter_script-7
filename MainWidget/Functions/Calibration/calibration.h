#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QWidget>
#include <QScrollArea>
#include <QCheckBox>
#include <QStackedWidget>
#include "../../windowobject.h"
#include "MainWidget/listboxobject.h"

namespace robotTeachABB
{
    class Calibration; //校准
    class CalibrWindow; //校准选择界面
    class ButtonWidget; //自制的tabBar按钮
    class UpdateCounterWindow; //转数计数器更新界面
    class UpdateListWidget; //转数计数器更新界面中的轴选择界面
    class UpdateList; //转数计数器选择界面中的选择项
}

class UpdateList : public QWidget
{
    Q_OBJECT
public:
    UpdateList(QString check_text, QWidget *parnet = nullptr);
    void SetSelect(bool isselect); //设置是否选中
    void SetText(QString text); //设置文本内容
    bool GetSelect(); //获取是否选中
    QString GetText(); //获取文本内容

private:
    void mousePressEvent(QMouseEvent *event);

private:
    QCheckBox *check_box_;
    QLabel *state_label_;
}; //转数计数器选择界面中的选择项

class UpdateListWidget : public QWidget
{
    Q_OBJECT
public:
    UpdateListWidget(QWidget *parent = nullptr);

public slots:
    void SelectAll();
    void ClearAll();

private:
    QScrollArea *scroll_area_;
    QWidget *scroll_widget_;

    QLabel *tips1_label_;
    QLabel *tips2_label_;

public:
    QList<UpdateList*> list_;
}; //转数计数器更新界面中的轴选择界面

class UpdateCounterWindow : public QWidget
{
    Q_OBJECT
public:
    UpdateCounterWindow(QWidget *parent = nullptr);
    void Init();

signals:
    void Close();

private:
    void ConnectInit();

private:
    QLabel *tips_label_;
    QLabel *tips1_label_;
    QLabel *tips2_label_;
    QLabel *tips3_label_;

    UpdateListWidget *list_widget_;

    QWidget *button_widget_;
    QPushButton *select_all_button_;
    QPushButton *clear_all_button_;
    QPushButton *update_button_;
    QPushButton *close_button_;
}; //转数计数器更新界面

class ButtonWidget : public QWidget
{
    Q_OBJECT
public:
    ButtonWidget(QString ico_path, QString text, QWidget *parent = nullptr);
    ButtonWidget(QWidget *parent = nullptr);
    void SetIsClick(bool isclick);

signals:
    void Clicked();

private:
    void Init(QString ico_path = QString(), QString text = QString());
    void ConnectInit();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);

private:
    bool isclick_;

    QLabel *ico_label_;
    QLabel *text_label_;
}; //自制的tabBar按钮

class CalibrWindow : public QWidget
{
    Q_OBJECT
public:
    CalibrWindow(QWidget *parent = nullptr);
    UpdateCounterWindow *GetUpdateCouWindow();

signals:
    void Close();

private slots:

private:
    void ConnectInit();

private:
    ButtonWidget *counter_button_; //转数计数器
    ButtonWidget *calibration_button_; //校准 参数
    ButtonWidget *smb_button_; //SMB内存
    ButtonWidget *pedestal_button_; //基座

    QWidget *counter_window_;
    QPushButton *update_counter_button_;
    UpdateCounterWindow *updateCounter_window_;

    QWidget *calibration_window_;
    QPushButton *cal_button1_;
    QPushButton *cal_button2_;
    QPushButton *cal_button3_;

    QWidget *smb_window_;
    QPushButton *smb_button1_;
    QPushButton *smb_button2_;
    QPushButton *smb_button3_;

    QWidget *pedestal_window_;
    QPushButton *ped_button1_;

    QStackedWidget *stack_widget_;

    QWidget *button_widget_;
    QPushButton *close_button_;
}; //校准选择界面

class Calibration : public WindowObject
{
    Q_OBJECT
public:
    explicit Calibration(QWidget *parent = nullptr, int type = 7);
    void resizeEvent(QResizeEvent *);

signals:

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ShowCalibrWindow();
    void ItemClicked(QTableWidgetItem *item);

private:
    QLabel *tips1_label_;
    QLabel *tips2_label_;

    TableBox *table_box_;

    CalibrWindow *calibr_window_;
};

#endif // CALIBRATION_H
