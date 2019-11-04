#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <QSlider>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSharedPointer>
#include <QRadioButton>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../windowobject.h"
#include "../../listboxobject.h"
#include "../../comboboxobject.h"
#include "../../stackedwidobject.h"
#include "teachstate.h"
#include "MainWidget/commoncontrols.h"

namespace robotTeachABB
{
    class Appearance; //外观
    class Monitor; //监控
    class FlexPendant; //FlexPendant
    class InputOut; //IO
    class Language; //语言
    class PKWidget; //ProgKeys内部显示的界面
    class ProgKeys; //ProgKeys
    class DateTime; //日期和时间
    class Diagnosis; //诊断
    class Dispose; //配置
    class TouchScreen; //触摸屏

    class IOSelectWindow; //IO配置中的io选择窗口
    class IOEditAndAddWindow; //编辑与新建io窗口
    class DeviceNetAddWindow; //DeviceNet DeviceI/O的添加窗口
    class DeviceNetEditWindow; //DeviceNet DeviceI/O的编辑窗口
    class DeviceNetTableWidget; //为DeviceNet窗口打造的tablebox表格
    class IOModVal; //IO配置窗口中的修改值窗口
    class SignalAddEditWindow; //Signal的添加与编辑窗口
    class SignalTableWidget; //为SignalAddEditWindow窗口打造的tablebox表格
}

class IOSelectWindow;
class IOModVal;

class DeviceNetTableWidget : public TableBox
{
    Q_OBJECT
public:
    DeviceNetTableWidget(QWidget *parent = nullptr);
    void Init();

signals:
    void ShowKey(QTableWidgetItem *curr_item); //显示键盘
    void ShowNumKey(QString select_name, QTableWidgetItem *curr_item); //显示数字修改界面

protected slots:
    virtual void ItemClicked(QTableWidgetItem *item);
    virtual void ComBoxClicked(QString text);

protected:
    QTableWidgetItem *old_item_;
    QPointer<QComboBox> combo_box_;
}; //为DeviceNet窗口打造的tablebox表格

class SignalTableWidget : public DeviceNetTableWidget
{
    Q_OBJECT
public:
    SignalTableWidget(QWidget *parent = nullptr);
    void SigInit(QJsonObject *json, QStringList assigneds, bool isadd);

protected slots:
    virtual void ItemClicked(QTableWidgetItem *item);
    virtual void ComBoxClicked(QString text);

private:
    void Type(QString text, int curr_row);
    void Device(QString text, int curr_row);

private:
    enum Names:char{None, Name0, Type1, Assigned2, Signal3, Category4, Access5, Safe6, Default7,
                   Analog8, Maximum9, Minimum10, Number11, Device12, Filpas13, Filact14, Invert15,
                   Maxphv16, Maxpvl17, Maxbv18, Minphv19, Minpvl20, Minbv21};
    QMap<QString, Names> str_enum_map_;
    QMap<Names, QStringList> enum_val_map_;
    QStringList name_list_;
    QJsonObject *object_;
    bool isadd_;

private:
    QString EnumToStr(Names name);
    Names StrToEnum(QString name);
}; //为SignalAddEditWindow窗口打造的tablebox表格

class SignalAddEditWindow : public QWidget
{
    Q_OBJECT
public:
    SignalAddEditWindow(QWidget *parent = nullptr);
    void Init(QJsonObject json, QStringList assigneds, QString old_name);

signals:
    void DefineClicked(QJsonObject json, QString old_name);
    void CancelClicked();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefButtonClicked();
    void CanButtonClicked();
    void ShowKey(QTableWidgetItem *curr_item); //显示键盘
    void ShowNumKey(QString select_name, QTableWidgetItem *curr_item); //显示数字修改界面

    void KeyGetData(QString data);
    void KeyHide();

    void IOMovDefine(QString val);
    void IOMovCancel();

private:
    QLabel *title_label;

    QFrame *head_widget_;
    QLabel *tips_label_;
    QLabel *tips2_label_;

    SignalTableWidget *table_box_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    Keyboard *keyboard;
    IOModVal *mod_val_;

private:
    QTableWidgetItem *curr_item_;
    QString old_name_;
    QJsonObject object_;
}; //Signal的添加与编辑窗口

class IOModVal : public QWidget
{
    Q_OBJECT
public:
    IOModVal(QWidget *parent = nullptr);
    void Init(QString data_name, QString param_name, QString val);
    void resizeEvent(QResizeEvent *);

signals:
    void DefineClicked(QString val);
    void CancelClicked();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void NumKeyDefine();
    void NumKeyCancel();
    void ItemClicked(QTableWidgetItem *item);

private:
    void Init();

private:
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *data_name_label_;
    QLabel *tips2_label_;
    QLabel *param_name_label_;
    QLabel *tips3_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    NumKey *num_key_;
    QLineEdit *line_edit_;

private:
    double scale_w_, scale_h_;
}; //IO配置窗口中的修改值窗口

class DeviceNetEditWindow : public QWidget
{
    Q_OBJECT
public:
    DeviceNetEditWindow(QWidget *parent = nullptr);
    void Init(QJsonObject json, QString old_name);

signals:
    void DefineClicked(QJsonObject json, QString old_name);
    void CancelClicked();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefButtonClicked();
    void CanButtonClicked();
    void ShowKey(QTableWidgetItem *curr_item); //显示键盘
    void ShowNumKey(QString select_name, QTableWidgetItem *curr_item); //显示数字修改界面

    void KeyGetData(QString data);
    void KeyHide();

    void IOMovDefine(QString val);
    void IOMovCancel();

private:
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips_label_;
    QLabel *name_label_;
    QLabel *tips2_label_;

    DeviceNetTableWidget *table_box_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    Keyboard *keyboard;
    IOModVal *mod_val_;

private:
    QTableWidgetItem *curr_item_;
    QJsonObject object_;
}; //DeviceNet DeviceI/O的编辑窗口

class DeviceNetAddWindow : public QWidget
{
    Q_OBJECT
public:
    DeviceNetAddWindow(QWidget *parent = nullptr);
    void Init();

signals:
    void DefineClicked(QJsonObject json);
    void CancelClicked();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefButtonClicked();
    void CanButtonClicked();
    void ShowKey(QTableWidgetItem *curr_item); //显示键盘
    void ShowNumKey(QString select_name, QTableWidgetItem *curr_item); //显示数字修改界面

    void KeyGetData(QString data);
    void KeyHide();

    void IOMovDefine(QString val);
    void IOMovCancel();

    void ComBoxClicked(int select);

private:
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *tips2_label_;

    QFrame *table_widget_;
    QLabel *tips3_label_;
    QComboBox *type_box_;
    DeviceNetTableWidget *table_box_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    Keyboard *keyboard;
    IOModVal *mod_val_;

private:
    QTableWidgetItem *curr_item_;
}; //DeviceNet DeviceI/O的添加窗口

class IOEditAndAddWindow : public QWidget
{
    Q_OBJECT
public:
    IOEditAndAddWindow(QWidget *parent = nullptr);
    void Init(QJsonObject json, bool isedit);

signals:
    void Close();
    void Define(QJsonObject json, bool isedit);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefineClicked();
    void CloseClicked();

    void TableBoxClicked(QTableWidgetItem *item);
    void KeyGetData(QString data);
    void KeyClose();

private:
    void ConnectInit();
    void Edit();
    void Add(QJsonObject json);

private:
    friend class IOSelectWindow;

    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *tips2_label_;

    QWidget *table_widget_;
    QLabel *name_label_;
    QLabel *val_label_;
    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *close_button_;

    Keyboard *keyboard_;

private:
    QSharedPointer<QJsonObject> object_json_;
    int row_;
    int col_;
    bool isedit_;
}; //编辑与新建io窗口


class IOSelectWindow : public QWidget
{
    Q_OBJECT
public:
    IOSelectWindow(QWidget *parent = nullptr);
    void Init(QJsonObject json, QString name, QStringList assigneds = QStringList());

signals:
    void Close(QJsonObject json, QString name);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void TableBoxClicked(QTableWidgetItem *item);
    void DelJsonData();
    void EditJsonData();
    void AddJsonData();
    void DefineEdAd(QJsonObject json, bool isedit);
    void CloseEdAd();

    void DeviceAddDefine(QJsonObject json);
    void DeviceAddCancel();

    void DeviceEditDefine(QJsonObject json, QString old_name);
    void DeviceEditCancel();

    void SignalDefine(QJsonObject json, QString old_name);

private:
    void ConnectInit();
    void UpdateTableBox(QStringList table_data);

private:
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips_type_label_;
    QLabel *type_label_;
    QLabel *tips_label_;

    TableBox *table_box_;

    QWidget *button_widget_;
    QPushButton *edit_button_;
    QPushButton *add_button_;
    QPushButton *del_button_;
    QPushButton *close_button_;

    IOEditAndAddWindow *io_edit_window_;
    DeviceNetEditWindow *device_edit_window_;
    DeviceNetAddWindow *device_add_window_;
    SignalAddEditWindow *signal_window_;

private:
    QJsonObject object_json_;
    QString select_name_;
    QStringList assigneds_;
}; //IO配置中的io选择窗口

class Appearance : public WindowObject
{
    Q_OBJECT
public:
    explicit Appearance(QWidget *parent = nullptr, int type = -1); //type设置成为-1代表由子类设置图标
    void resizeEvent(QResizeEvent *);

signals:

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QGroupBox *light_group_; //亮度
    QLabel *min_label_;
    QLabel *max_label_;

public:
    QSlider *light_slider_; //调节亮度的滑块
    QPushButton *add_button_; //加按钮
    QPushButton *red_button_; //减按钮
    QPushButton *default_button_; //默认设置按钮
    QPushButton *right_button_; //向右旋转按钮
    QPushButton *confirm_button_; //确认按钮
    QPushButton *cancel_button_; //取消按钮
}; //外观



class Monitor : public WindowObject
{
    Q_OBJECT
public:
    Monitor(QWidget *parent = nullptr, int type = -1);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QFrame *task_frame_; //任务框
    QLabel *tips_task_label_;
    QLabel *tips_path_label_;
    QLabel *tips_manual_label_;
    QLabel *tips_sens_label_;
    QLabel *tips_sens2_label_;
    QFrame *set_up_frame_; //执行设置
    QLabel *tips_setup_label_;
    QLabel *tips_label_;

public:
    QComboBox *task_box_; //任务选择
    QLineEdit *sensitivity_edit_; //灵敏度
    QLineEdit *sensitivity2_edit_;
    QRadioButton *path_open_button_; //路径监控开
    QRadioButton *path_shut_button_; //路径监控关
    QRadioButton *manual_open_button_; //手动监控开
    QRadioButton *manual_shut_button_; //手动监控关
    QRadioButton *setup_open_button_; //执行设置开
    QRadioButton *setup_shut_button_; //执行设置关
    QPushButton *refresh_button_; //刷新按钮
    QPushButton *confirm_button_; //确认按钮
    QPushButton *cancel_button_; //取消按钮
}; //监控



class FlexPendant : public WindowObject
{
    Q_OBJECT
public:
    FlexPendant(QWidget *parent = nullptr, int type = -1);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *tips_label_;
    QLabel *tips2_label_;

public:
    QLabel *attribute_label_;
    ListBoxObject *listbox_;
    QPushButton *close_button_;
}; //FlexPendant



class InputOut : public WindowObject
{
    Q_OBJECT
public:
    InputOut(QWidget *parent = nullptr, int type = -1);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *tips_label_;
    QLabel *tips2_label_;
    QLabel *name_label_;
    QLabel *type_label_;

public:
    ListBoxObject *listbox_;
    QPushButton *whole_button_; //全部按钮
    QPushButton *nothing_button_; //无按钮
    QPushButton *preview_button_; //预览按钮
    QPushButton *app_button_; //应用按钮
    QPushButton *close_button_; //关闭按钮
}; //IO



class Language : public WindowObject
{
    Q_OBJECT
public:
    Language(QWidget *parent = nullptr, int type = -1);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void DefineClicked();

private:
    void ReadLanguage(); //从本地读取语言文件

private:
    QLabel *tips_label_;

public:
    QLabel *lang_ico_label_;
    QLabel *lang_label_; //显示当前语言
    QLabel *lang_button_; //调整语言选择框中语言的显示顺序
    ListBoxObject *lang_listbox_; //语言选择框
    QPushButton *confirm_button_; //确认按钮
    QPushButton *cancel_button_; //取消按钮
}; //语言



class PKWidget : public QWidget
{
    Q_OBJECT
public:
    PKWidget(int num, QTabWidget *tabwidget, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void Init();

public slots:
    void SetScale(double scale_w, double scale_h);
    void DefineClicked();

private slots:
    void TypeBoxClicked(int num);

    void OutItemClicked(QTableWidgetItem *item);
    void IntItemClicked(QTableWidgetItem *item);

private:
    void ConnectInit();

private:
    int num_; //标明此窗口是按键几，用于修改标签页标题
    ProgKeysName key_name_; //快捷键名称
    QTabWidget *tab_widget_; //用于修改标题
    QLabel *type_label_; //类型标签
    double scale_w_;
    double scale_h_;
    QTableWidgetItem *out_item_;
    QTableWidgetItem *int_item_;

public:
    QLabel *press_label_; //按下按键标签
    QLabel *allow_label_; //允许自动模式标签
    QComboBox *type_box_; //类型选择框
    QComboBox *press_output_box_; //类型为输出时的按下按键框
    QComboBox *press_system_box_; //类型为系统时的按下按键框
    QComboBox *allow_box_; //是否允许自动模式框
    QLabel *output_label_; //数字输出标签
    TableBox *output_frame_; //数字输出窗口
    QLabel *input_label_; //数字输入标签
    TableBox *input_frame_; //数字输入窗口
}; //ProgKeys内部显示的界面



class ProgKeys : public WindowObject
{
    Q_OBJECT
public:
    ProgKeys(QWidget *parent = nullptr, int type = -1);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *); //使得当此界面正常显示时调整tabwidget使其每页宽度相同

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QTabWidget *tab_widget_;

public:
    QPushButton *confirm_button_; //确认按钮
    QPushButton *cancel_button_; //取消按钮
    PKWidget *prog_key1_; //自定义按键1
    PKWidget *prog_key2_; //自定义按键2
    PKWidget *prog_key3_; //自定义按键3
    PKWidget *prog_key4_; //自定义按键4
}; //ProgKeys



class Dispose : public QWidget
{
    Q_OBJECT
public:
    Dispose(QWidget *parent = nullptr);
    ~Dispose();
    void ReadIOInfo(); //从本地读取io数据
    void WriteIOInfo(); //将io数据写入到本地

signals:
    void Close();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ShowAllButton(); //显示全部按钮

    void TableBoxClicked(QTableWidgetItem *item);

    void IOSelectClose(QJsonObject json, QString name);

private:
    void ConnectInit();
    void UpdateListBox(QStringList list_data); //更新list_box数据

private:
    QLabel *ico_label_;
    QLabel *title_label_;

    QFrame *head_widget_;
    QLabel *tips1_label_;
    QLabel *tips2_label_;
    QLabel *theme_label_;
    QLabel *tips3_label_;

    QFrame *list_widget_;
    TableBox *table_box_;

    QWidget *button_widget_;
    ComboBoxObject *file_button_;
    ComboBoxObject *theme_button_;
    QPushButton *show_button_;
    QPushButton *close_button_;

    QAction *load_action_;
    QAction *eio_save_action_;
    QAction *all_save_action_;

    QAction *man_action_;
    QAction *con_action_;
    QAction *com_action_;
    QAction *mot_action_;
    QAction *io_action_;

    IOSelectWindow *io_select_window_;

private:
    QString select_name_;
    QJsonObject root_json_;
}; //配置



class TouchScreen : public WindowObject
{
    Q_OBJECT
public:
    TouchScreen(QWidget *parent = nullptr, int type = -1);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *tips_label_;

public:
    QPushButton *re_cali_button_; //重校
    QPushButton *cancel_button_; //取消按钮
}; //触摸屏

#endif // CONTROL_H
