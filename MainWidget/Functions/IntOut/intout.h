#ifndef INTOUT_H
#define INTOUT_H

#include <QWidget>
#include <QTimer>
#include <QJsonObject>
#include <QComboBox>

#include "../../windowobject.h"
#include "../../listboxobject.h"
#include "../../comboboxobject.h"

namespace robotTeachABB
{
    class IntOut; //输入输出功能
}

const int IntOut_Number = 13;
class IntOut : public QWidget
{
    Q_OBJECT
public:
    IntOut(QWidget *parent = nullptr);

signals:
    void ShowWindow();
    void HideWindow();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ActionClicked();

    void IOTimerUpdate();

    void OFFClicked();
    void ONClicked();

private:
    void hideEvent(QHideEvent *);
    void showEvent(QShowEvent *);
    void ConnectInit();
    void ReadIOInfo();

private:
    QLabel *ico_label_;
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *select_label_;
    QLabel *tips_filter_label_;
    QLabel *tips_layout_label_;
    QLabel *tips_label_;
    QLabel *filter_label_;
    QComboBox *layout_box_;

    TableBox *table_box_;

    QWidget *button_widget_;
    ComboBoxObject *filter_button_;
    QPushButton *zero_button_;
    QPushButton *one_button_;
    QPushButton *refresh_button_;
    ComboBoxObject *view_button_;

private:
    QTimer *timer_;
    QAction view_actions_[IntOut_Number];
    QJsonObject root_object_;
    QStringList di_list_;
    QStringList do_list_;
    QStringList ai_list_;
    QStringList ao_list_;
    int io_type_;
    bool isupdate_; //决定定时器是否刷新io状态
};

#endif // INTOUT_H
