#ifndef COMMONCONTROLS_H
#define COMMONCONTROLS_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QWidget>
#include <QReadWriteLock>
#include <exception>
#include <QLabel>
#include <QComboBox>
#include <QPointer>
#include <QMouseEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QSharedPointer>

namespace robotTeachABB
{
    class Keyboard; //虚拟键盘
    class KeyButton; //虚拟键盘中的按键
    class FilterKey; //过滤器使用的键盘
    class NumKey; //数字键盘
    class ImageButton; //图片按钮
    class DataType; //数据类型
    class VarInfo; //变量信息
    class ThreeSectionWindow; //三段式窗口
    class VarEditWindow; //变量编辑窗口
    class CreateNewVar; //新建变量窗口
    class ArraySize; //定义数组大小窗口
    class Convert; //数据转换
    class ModVarWindow; //修改变量值窗口
    class ModVarListBoxItem; //修改值窗口中的列表项
    class ABBException; //异常类
    class KeyLineEdit; //键盘专用编辑框
    struct Euler;
    struct Quaternion;
}

class ABBException;
class ListBoxObject;
class Keyboard;
class NumKey;
struct VarFormData
{
    VarFormData(QString nm = QString(), QString cd = QString(), QString tp = QString(), QWeakPointer<VarFormData> pn = QWeakPointer<VarFormData>());
    void Update(); //根据子节点的值更新自身的值
    void LastUpdate(); //让子节点根据父节点的值更新自身的值

    QString name;
    QString code;
    QString type;
    QWeakPointer<VarFormData> parent;
    QList<QSharedPointer<VarFormData>> subnode;
}; //变量组成节点
struct Quaternion;
struct Euler
{
    Euler();
    Euler(double x, double y, double z);
    void SetData(double x, double y, double z); //设置数据
    Quaternion GetQuaternion(); //未解决某些因命名空间冲突导致的莫名其妙的问题

    double roll; //EX
    double pitch; //EY
    double yaw; //EZ
}; //欧拉角
struct Quaternion
{
    Quaternion();
    Quaternion(QString code);
    void SetData(QString code); //设置数据

    double w; //q1
    double x; //q2
    double y; //q3
    double z; //q4
};//四元数

class ModVarListBoxItem : public QWidget
{
    Q_OBJECT
public:
    ModVarListBoxItem(QWeakPointer<VarFormData> pointer, int num, QWidget *parent = nullptr);
    QLineEdit *ShowEdit();
    void HideEdit(bool issave = false);
    void SetNum(int num);
    int GetNum();
    void SetVal(QString val);
    QString GetVal();
    QString GetType();
    void Refresh(); //刷新，一般是其他节点的值改变以后需要进行

signals:
    void Clicked(QString type, ModVarListBoxItem *item);

private:
    void mousePressEvent(QMouseEvent *e);

private:
    QLabel *name_label_;
    QLabel *val_label_;
    QLineEdit *val_edit_;
    QLabel *type_label_;

    bool isclick_; //决定是否可以被点击修改，当数据类型为基础类型时可被修改
    int num_; //保存自身是第几位元素
    QWeakPointer<VarFormData> pointer_;
}; //修改值窗口中的列表项

class ModVarWindow : public QWidget
{
    Q_OBJECT
public:
    ModVarWindow(QWidget *parent = nullptr);
    virtual ~ModVarWindow() {}
    void HideTitle(); //隐藏标题栏
    void Init(QString var_type, QString var_name = QString());
    void Init(QWeakPointer<VarFormData> point, QString var_name, QString mod_name, QString action_scope, bool is_static = false);

signals:
    void Cancel();
    void Define(QString code);
    void Defineed();

public slots:
    void SetScale(double scale_w, double scale_h);
    void AddListItem(QWeakPointer<VarFormData> pointer);

private slots:
    void DefineClicked();
    void CancelClicked();

    void KeyboardGetData(QString data); //键盘的返回值
    void ItemClicked(QString type, ModVarListBoxItem *item); //某元素被点击，需要修改数据
    void TrueButtonClicked(); //true按钮被点击
    void FalseButtonClicked(); //false按钮被点击

private:
    void ConnectInit();
    void AddListItem(QWeakPointer<VarFormData> pointer, int num); //递归向list_box_添加数据，
    void Refresh();

private:
    friend class CreateNewVar;
    friend class ProgEditMain;

    QWidget *head_widget_;
    QLabel *ico_label_;
    QLabel *title_label_;

    QLabel *tips_name_label_;
    QLabel *name_label_;
    QLabel *tips_label_;

    QLabel *list_name_label_;
    QLabel *list_val_label_;
    QLabel *list_type_label_;
    ListBoxObject *list_box_;

    QWidget *button_widget_;
    QPushButton *true_button_;
    QPushButton *false_button_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    Keyboard *keyboard_;
    NumKey *num_key_;

private:
    QSharedPointer<VarFormData> varform_pointer_;
    QWeakPointer<VarFormData> pointer_;
    double scale_w_;
    double scale_h_;

    QPointer<ModVarListBoxItem> clicked_item_;
    QString mod_name_; //模块名
    QString action_scope_; //作用域
    bool isstatic_; //是否为静态，为真时不可修改
}; //修改变量值窗口

class Convert
{
public:
    static Quaternion EulerToQuaternion(Euler &er); //欧拉角转四元数
    static Euler QuaternionToEuler(Quaternion &qn); //四元数转欧拉角
    static double AngleToRadian(double angle); //角度转弧度
    static double RadianToAngle(double radian); //弧度转角度
    static QList<double> XYZCodeToDoubles(QString code); //xyz的代码转xyz的double
    static QString PositDoubleToPositCode(double x, double y, double z, Euler &er); //服务端格式坐标数据转ABB坐标变量值
    static QSharedPointer<VarFormData> DataTypeToVarFormData(QString data_type, QString code = QString()); //变量类型转与之对应的具有层次结构的值，若code为空则使用默认值

private:
    static QSharedPointer<VarFormData> GetForms(QString data_type); //递归获取组成，VarCodeToVarFormData的辅助函数
}; //数据转换

class QTableWidget;
class QTableWidgetItem;
class ArraySize : public QWidget
{
    Q_OBJECT
public:
    ArraySize(QWidget *parent = nullptr);
    void Init(QString var_name, int size);

signals:
    void Define(QString dim_str);
    void Cancel();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ItemClicked(QTableWidgetItem *item);
    void DefineClicked();

    void KeyDefine();
    void KeyCancel();

private:
    void ConnectInit();

private:
    QFrame *head_widget_;
    QLabel *tips_label_;
    QLabel *var_name_label_;
    QLabel *tips2_label_;

    QTableWidget *table_widget_;

    QWidget *button_widget_;
    QPushButton *define_button_;
    QPushButton *cancel_button_;

    NumKey *num_key_;
    QPointer<QLineEdit> edit_key_;
    QTableWidgetItem *current_item_;

private:
    double scale_w_, scale_h_;
}; //定义数组大小窗口

class CreateNewVar : public QWidget
{
    Q_OBJECT
public:
    CreateNewVar(QWidget *parent = nullptr);
    void Init(QString type, QString mod);

signals:
    void CreateVar(QString mod_name); //新建变量

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void ConnectInit();
    void GetVar();

    void ShowArraySize();
    void ArraySizeDim(QString dim_str);

private:
    void Init(QString &mod);

private:
    friend class VarEditInterface;

    QString init_var_; //初始值

    Keyboard *keyboard_;
    QWidget *main_widget_;
    ModVarWindow *mod_var_window_; //修改初始值窗口

    QLabel *tips_type_label_;
    QLabel *type_name_label_;
    QLabel *tips_task_label_;
    QLabel *task_name_label_;

    QLabel *name_label_;
    QLineEdit *name_edit_; //名称
    QPushButton *name_key_button_;

    QLabel *range_label_;
    QComboBox *range_box_; //范围

    QLabel *type_label_;
    QComboBox *type_box_; //存储类型

    QLabel *task_label_;
    QComboBox *task_box_; //任务

    QLabel *mod_label_;
    QComboBox *mod_box_; //模块

    QLabel *routine_label_;
    QComboBox *routine_box_; //例行程序

    QLabel *dimen_label_;
    QComboBox *dimen_box_; //维数
    QLineEdit *dimen_edit_;
    QPushButton *dimen_key_button_;

    QWidget *button_widget_;
    QPushButton *init_var_button_; //初始值
    QPushButton *define_button_;

    ArraySize *array_size_;

public:
    QPushButton *cancel_button_;
}; //新建变量窗口

class ThreeSectionWindow : public QFrame
{
    Q_OBJECT
public:
    ThreeSectionWindow(QWidget *parent = nullptr);

protected:
    QWidget *upper_widget_;
    QFrame *in_widget_;
    QWidget *lower_widget_;
}; //三段式窗口

class VarEditWindow : public ThreeSectionWindow
{
    Q_OBJECT
public:
    VarEditWindow(QWidget *parent = nullptr);
    void SetNameLabel(QString str);

private:
    QLabel *tips_name_label_;
    QLabel *name_label_;
    QLabel *tips_label_;
}; //变量编辑窗口

struct VarDataInfo
{
    VarDataInfo();
    VarDataInfo(QString name);
    void SetVar(QVariant newvar); //修改值，所做的修改将会同步到代码中（主要用于坐标变量）
    void SetVar(QSharedPointer<VarFormData> pointer); //修改值，所做的修改将会同步到代码中

    QString var_name; //变量名
    QString data_type; //数据类型
    QString type; //所属类型，用户定义变量时与数据类型相同，内置变量时大部分时候与数据类型不相同
    QVariant var; //值
    QVariant init_var; //初始值
    int nature; //变量性质，0为变量、1为可变量、2为常量
    bool genre; //变量类别，true为用户定义变量，false为内置变量
    QString action_scope; //作用域，all为全局，local为本地，task为任务，其他则为具体函数
    QString mod; //所属模块，当genre为true用户定义变量时才有用
    QString dim; //维数数据，可从中分析出维数
}; //变量数据信息

class VarInfo : public QObject
{
    Q_OBJECT
public:
    VarInfo(QObject *parent = nullptr);
    ~VarInfo();
    VarInfo(const VarInfo &info) = delete;
    VarInfo &operator=(const VarInfo &info) = delete;
    static QSharedPointer<VarInfo> instance();

signals:
    void VarRemove(QString mod_name); //变量被删除

public slots:
    void RoutRename(QString old_name, QString new_name); //例行程序名被修改
    void RoutMove(QString rout_name, QString new_name); //例行程序被移动
    void ModRename(QString old_name, QString new_name); //模块名被修改
    void ModDel(QString mod_name); //模块被删除，需要将所属于被删除的模块的变量全部删除
    void RoutDel(QString rout_name); //例行程序被删除
    void ClearMod(); //清除所有模块，需要删除除了内置变量以外的其他所有变量

public:
    bool AddVar(QString &name, VarDataInfo &info); //添加变量
    bool IsExist(QString &name); //判断某个变量名是否存在
    bool IsExist(QString &name, VarDataInfo &info); //判断某个变量是否存在
    QList<VarDataInfo> GetVarInfo(QString name); //获取某个变量名下所有变量的信息
    QStringList GetVarName(); //获取所有变量名
    QList<QPair<QString, VarDataInfo>> GetTypeVar(QString &type); //获取某一数据类型的所有变量名及变量信息
    QStringList GetVarName(QString mod_name); //获取某个模块中的所有变量
    QStringList GetVarName(QString mod_name, QString rout_name); //获取某个模块中的某个例行程序中的所有变量
    QVariant GetVar(QString type, QString dim); //根据变量类型与维数返回变量初始值
    QString GetForms(QString type); //递归获取组成
    bool RemoveVar(QString var_name, QString data_type = QString(), QString action_scope = QString()); //删除某个变量名对应的所有变量，与之对应的代码语句也会被删除
    QString GetCode(VarDataInfo &var_info, QString var_name); //获取某变量所对应的代码语句
    VarDataInfo &GetUcsVarInfo(QString var_name); //获取坐标变量的信息
    VarDataInfo &GetVarInfo(QString var_name, QString mod_name, QString action_scope); //根据变量名、所属模块、作用域获取某个变量信息
    QString GetUnused(QString type = QString()); //产生一个未被使用的变量名

private:
    QPair<QString, int> GetStcVarName(QString &type);

private:
    friend class DataType;
    static QSharedPointer<VarInfo> varinfo_point_;
    struct VarName
    {
        VarName()
        {
            this->head = "var";
            this->num = 0;
            this->weight = 1;
        }
        VarName(QString &head, int num, int weight)
        {
            this->head = head;
            this->num = num;
            this->weight = weight;
        }
        QString head;
        int num;
        int weight;
    };

    QMap<QString, VarDataInfo> varinfo_map_;
    QMap<QString, VarName> var_name_map_;
    QReadWriteLock *shmutex_; //读写锁
}; //变量信息

class DataType : public QObject
{
    Q_OBJECT
public:
    DataType(QObject *parent = nullptr);
    ~DataType();
    DataType(const DataType &type) = delete;
    DataType &operator=(const DataType &type) = delete;
    static QSharedPointer<DataType> instance();

public:
    bool IsExist(QString &name);
    bool IsBase(QString &name);
    QSharedPointer<QList<QPair<QString, QString>>> GetForms(QString &name); //获取某个类型的组成
    QStringList GetTypeName(); //获取所有数据类型名
    QStringList GetShowTypeName(); //获取可显示的数据类型名

private:
    void GetFile(); //从本地获取数据类型文件
    void Analysis(QByteArray &json); //分析从本地读取出的json数据
    int GetInfoRow(QString name); //获取某个数据类型在data_type_info_list_中的索引

private:
    static QSharedPointer<DataType> data_type_point_;

    struct TypeInfo
    {
        bool isbase; //是否是基础类型
        bool isshow; //是否可在程序数据界面中显示
        bool isdata; //是否拥有内置变量
        QSharedPointer<QList<QPair<QString, QString>>> form; //组成，只有当isbase为false时才生效
    }; //数据类型信息

    QList<QPair<QString, TypeInfo>> data_type_info_list_;
}; //数据类型

class ImageButton : public QFrame
{
    Q_OBJECT
public:
    ImageButton(QString text, QString image_path, QWidget *parent = nullptr);
    ImageButton(QString image_path, QWidget *parent = nullptr);
    ImageButton(QWidget *parent = nullptr);
    QLabel *GetImageLabel();
    QLabel *GetTextLabel();
    void SetImage(QString image_path);
    void SetText(QString text);
    void SetButton(QString text, QString image_path);
    void SetIsSelect(bool isselect);
    bool GetIsSelect();
    void SetLeverSelect(bool leverselect);
    bool GetLeverSelect();
    QString GetText();

signals:
    void clicked();

private:
    void mousePressEvent(QMouseEvent *e);
    void Init(QString &image_path);

private:
    bool isselect_; //是否选中
    bool lever_select_;
    QLabel *image_label_;
    QLabel *text_label_;
}; //图片按钮

class NumKey : public QFrame
{
    Q_OBJECT
public:
    NumKey(QWidget *parent = nullptr);
    void Init(QPointer<QLineEdit> edit);
    void Clear();
    QPushButton *GetFEButton();

signals:
    void DefineButton();
    void CancelButton();

private slots:
    void ButtonClicked();

private:
    void ConnectInit();
    bool eventFilter(QObject *object, QEvent *event); //事件过滤器

private:
    friend class ArraySize;
    QPushButton *key0_;
    QPushButton *key1_;
    QPushButton *key2_;
    QPushButton *key3_;
    QPushButton *key4_;
    QPushButton *key5_;
    QPushButton *key6_;
    QPushButton *key7_;
    QPushButton *key8_;
    QPushButton *key9_;
    QPushButton *left_move_;
    QPushButton *right_move_;
    QPushButton *delete_;
    QPushButton *plus_minus_;
    QPushButton *point_;
    QPushButton *f_e_;

    QPushButton *define_button_;
    QPushButton *cancel_button_;

private:
    QPointer<QLineEdit> edit_;
    bool isf_;
}; //数字键盘

class KeyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    KeyLineEdit(QWidget *parent = nullptr);

private:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *);
}; //键盘专用编辑框

class FilterKey : public QFrame
{
    Q_OBJECT
public:
    FilterKey(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void Init();
    QString GetFilter();

signals:
    void GetData(QString data);
    void HideWindow();
    void ShowWindow();

public slots:
    void SetScale(double scale_w, double scale_h);
    void ButtonClicked(); //中转站，响应按键将按键信息转交给处理中心进行处理

public:
    QLabel *title_label_; //标题
    QPushButton *reset_button_; //重置按钮

private:
    void ShowNum(); //显示数字
    void ShowLetter(); //显示字母
    void ProcessCenter(QString text); //处理中心，负责处理键盘中的各种按键
    void ConnectInit();

private:
    QString filter_text_;

    double scale_w_;
    double scale_h_;
    KeyLineEdit *edit_;
    QPushButton *keya_;
    QPushButton *keyb_;
    QPushButton *keyc_;
    QPushButton *keyd_;
    QPushButton *keye_;
    QPushButton *keyf_;
    QPushButton *keyg_;
    QPushButton *keyh_;
    QPushButton *keyi_;
    QPushButton *keyj_;
    QPushButton *keyk_;
    QPushButton *keyl_;
    QPushButton *keym_;
    QPushButton *keyn_;
    QPushButton *keyo_;
    QPushButton *keyp_;
    QPushButton *keyq_;
    QPushButton *keyr_;
    QPushButton *keys_;
    QPushButton *keyt_;
    QPushButton *keyu_;
    QPushButton *keyv_;
    QPushButton *keyw_;
    QPushButton *keyx_;
    QPushButton *keyy_;
    QPushButton *keyz_;
    QPushButton *delete_; //删除
    QPushButton *clear_button_; //清除按钮
    QPushButton *change_button_; //切换字母或数字按钮
    QPushButton *determine_button_; //过滤器按钮，确认按钮
    bool isnum_; //是否为数字状态，真为是假为否
}; //过滤器使用的键盘

class KeyButton : public QPushButton
{
    Q_OBJECT
public:
    KeyButton(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    QString GetTitle();
    void setTitle(QString title);

private:
    QLabel *title_label_;
}; //虚拟键盘中的按键

class Keyboard : public QWidget
{
    Q_OBJECT
public:
    explicit Keyboard(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);
    void Init(); //初始化
    void Init(QString text); //设定文字后的初始化

signals:
    void GetData(QString data);

public slots:
    void SetScale(double scale_w, double scale_h);
    void ButtonClicked(); //中转站，响应按键将按键信息转交给处理中心进行处理

private:
    void NumNormal(); //数字栏正常显示数字
    void NumSymbol(); //数字栏显示符号
    void LetterSmall(); //字母栏显示小写字母
    void LetterLarge(); //字母栏显示大写字母
    void ZztsOne(); //特殊符号栏显示第一类特殊符号
    void ZztsTwo(); //特殊符号栏显示第二类特殊符号
    void IntlSmall(); //显示国际化小写字母
    void IntlLarge(); //显示国际化大写字母
    void ProcessCenter(QString text); //处理中心，负责处理键盘中的各种按键
    void ConnectInit();

private:
    double scale_w_;
    double scale_h_;
    KeyLineEdit *edit_;
    QPushButton *cap_;
    QPushButton *shift_;
    QPushButton *intl_;
    QPushButton *blank_space_;
    QPushButton *upper_;
    QPushButton *lower_;
    QPushButton *left_;
    QPushButton *right_;
    QPushButton *end_;
    QPushButton *home_;
    QPushButton *left_square_;
    QPushButton *right_square_;
    QPushButton *delete_;
    KeyButton *one_;
    KeyButton *two_;
    KeyButton *three_;
    KeyButton *four_;
    KeyButton *five_;
    KeyButton *six_;
    KeyButton *seven_;
    KeyButton *eight_;
    KeyButton *nine_;
    KeyButton *zero_;
    KeyButton *reduce_;
    KeyButton *equal_;
    QPushButton *keya_;
    QPushButton *keyb_;
    QPushButton *keyc_;
    QPushButton *keyd_;
    QPushButton *keye_;
    QPushButton *keyf_;
    QPushButton *keyg_;
    QPushButton *keyh_;
    QPushButton *keyi_;
    QPushButton *keyj_;
    QPushButton *keyk_;
    QPushButton *keyl_;
    QPushButton *keym_;
    QPushButton *keyn_;
    QPushButton *keyo_;
    QPushButton *keyp_;
    QPushButton *keyq_;
    QPushButton *keyr_;
    QPushButton *keys_;
    QPushButton *keyt_;
    QPushButton *keyu_;
    QPushButton *keyv_;
    QPushButton *keyw_;
    QPushButton *keyx_;
    QPushButton *keyy_;
    QPushButton *keyz_;
    KeyButton *zzts0_;
    KeyButton *zzts1_;
    KeyButton *zzts2_;
    KeyButton *zzts3_;
    KeyButton *zzts4_;
    KeyButton *zzts5_;
    KeyButton *zzts6_;
    KeyButton *zzts7_;
    QWidget *button_widget_;
    QPushButton *determine_button_; //确认按钮
    bool iscap_; //大小写状态，真为大写假为小写
    bool isintl_; //是否显示国际化字母，真为显示假为不显示
    bool isshift_; //判断shift键是否按下，真为按下假为未按下

public:
    QPushButton *cancel_button_; //取消按钮
}; //虚拟键盘

class ABBException : std::exception
{
public:
    ABBException(QString text);
    ABBException(QString &text);
    ABBException(QString class_name, QString func_name, QString error_str, int number = 0);
    const char *what() const noexcept;

private:
    QString exp_str_;
}; //异常类

#endif // COMMONCONTROLS_H
