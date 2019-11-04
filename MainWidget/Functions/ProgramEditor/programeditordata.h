#ifndef PROGRAMEDITORDATA_H
#define PROGRAMEDITORDATA_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QMutex>
#include <functional>

namespace robotTeachABB
{
    class EditData; //程序编辑器数据管理类
}

struct HotEditVarInfo
{
    QString mod_name; //模块名
    QString action_scope; //作用域
    double x = 0;
    double y = 0;
    double z = 0;
    double a = 0;
    double b = 0;
    double c = 0;
}; //HotEdit界面中使用的数据

struct RoutMap
{
    QString rout_name;
    QMap<int, int> row_map;
}; //原始代码与运行代码的映射结构

struct RoutineData
{
    enum Type{PROC, FUNC, TRAP}; //程序、功能、中断
    Type type = PROC; //程序类型，默认为PROC
    QString data_type = "num"; //数据类型，默认为num，当程序类型为Func时起作用
    QString param; //参数，默认为空，当程序类型为TRAP时无效
    bool local = false; //是否为本地化程序，默认为假
    bool revoke = false; //是否勾选了撤销处理程序，默认为未勾选
    bool error = false; //是否勾选了错误处理程序，默认为未勾选
    bool back = false; //是否勾选了向后处理程序，默认为未勾选
    QStringList code; //代码
}; //例行程序数据
struct ModuleData
{
    QString task = "T_ROB1"; //所属任务，默认为 T_ROB1
    bool type = false; //类型，真为系统模块假为程序模块，默认为假
    QString param; //参数，系统模块拥有参数，程序模块无参数
    QMap<QString, RoutineData> routine_data; //例行程序
    QStringList mod_code; //不属于例行程序的代码
}; //模块数据
class EditData : public QObject
{
    Q_OBJECT
public:
    EditData(QObject *parent = nullptr);
    EditData(const EditData &edit) = delete;
    EditData &operator =(const EditData &edit) = delete;
    static QSharedPointer<EditData> instance();
    void SetTaskInfo(QString task_name, QString order_name, QString type_name, QString task_path); //设置当前任务信息
    void SetTaskName(QString task_name); //设置当前任务名称
    void SetTaskOrderName(QString order_name, QString task_path = QString(), QString type_name = QString()); //设置当前任务程序名称
    void SetTaskTypeName(QString type_name); //设置当前任务类型名称
    void SetTaskPath(QString task_path); //设置当前任务路径
    QString GetTaskName(); //获取当前任务名称
    QString GetTaskOrderName(); //获取当前任务程序名称
    QString GetTaskTypeName(); //获取当前任务类型名称
    QString GetTaskPath(); //获取当前任务路径
    void SaveMod(); //保存所有模块到本地
    void SaveMod(QString mod_name); //保存某个模块到本地
    void DelTask(bool is_del_local = false); //删除当前任务，会删除所有模块数据，包括系统模块数据也会被删除

signals:
    void RoutRename(QString old_name, QString new_name); //例行程序名被修改
    void RoutMove(QString rout_name, QString new_name); //例行程序被移动
    void ModRem(QString old_name, QString new_name); //模块名被修改
    void ModDel(QString mod_name); //模块被删除
    void RoutDel(QString rout_name); //例行程序被删除
    void ClearMod(); //清除所有模块
    void DataMid(QString mod_name); //模块内容被修改

public:
    QMap<QString, QString> *GetCodeMap(); //获取指令容器指针
    bool AnalysisData(); //开始分析任务
    bool LoadMod(QString &mod_data, QSharedPointer<QList<QPair<std::string, bool>>> coord = QSharedPointer<QList<QPair<std::string, bool>>>(), bool join = true); //加载模块，根据所传入的模块文本进行分析，join为真分析时保存变量
    QString ExportMod(QString mod_name, bool isshow = true, bool issave = false); //根据传入的模块名，导出文本格式的模块内容，用于存储至本地磁盘中，isshow为真时需要显示模块中的代码，为假时不显示,issave为真时不会讲移动指令中的变量值转换为*号
    bool ExportMod(QString mod_name, QString path); //将模块文本格式导出保存到指定路径中
    void SetHotVarPoint(QSharedPointer<QMap<QString, HotEditVarInfo>> hot_var_point);
    void GetHotEditData(std::function<void(void)> func, QSharedPointer<QMap<QString, HotEditVarInfo> > point); //获取HotEdit所需数据
    void SyntaxCheck(std::function<void(QSharedPointer<QMap<QString,QPair<QString,int>>>)> func); //语法检查
    void GetSendCode(std::function<void(bool,QSharedPointer<QString>)> func); //获取将要发送给服务端的代码
    bool IsExist(QString mod_name); //判断模块是否存在
    bool IsExist(QString mod_name, QString rout_name); //判断模块中是否存在某个例行程序
    bool RoutIsExist(QString rout_name); //在所有模块中判断某个例行程序是否存在
    bool GetModType(QString mod_name); //判断是系统模块还是程序模块，真为系统假为程序
    QString GetRoutType(QString rout_name); //获取某个例行程序的类型
    QStringList GetModName(); //获取所有模块名
    QStringList GetModName(bool type); //type为真时获取所有系统模块名，为假时获取所有程序模块名
    ModuleData &GetModData(QString mod_name); //获取某个模块全部数据
    QString GetRoutMod(QString rout_name); //返回某个例行程序所属的模块名
    RoutineData &GetRoutData(QString rout_name, QString mod_name = QString()); //获取某个模块中的某个例行程序的全部数据
    void DelMod(); //删除所有模块，但不删除本地文件
    void DelMod(QString mod_name, bool del_var = true); //删除某个模块数据，del_var为真时会删除该模块变量
    void DelMod(bool type); //删除某种类型的所有模块数据
    bool DelRout(QString rout_name, QString mod_name); //删除某个例行程序
    bool RemMod(QString old_name, QString new_name); //修改模块名，注意修改后会删除旧模块名在本地的存放文件
    bool RemRout(QString old_name, QString mod_name, QString new_name); //修改例行程序名
    void ModDataMid(QString mod_name); //发送模块被修改信号

public:
    QStringList GetRoutName(QString mod_name); //获取某个模块的所有例行程序名
    QStringList GetRoutName(); //获取所有例行程序名
    QStringList GetRoutName(RoutineData::Type type); //获取某一种类型的所有例行程序名
    QString GetRoutCodeTree(QString rout_name); //返回适合于CodeTree中的ShowCode函数所添加的例行程序名段

    bool ModAddCode(QString mod_name, QString code); //向模块添加代码
    bool RoutAddCode(QString rout_name, int row, QString code); //向例行程序添加代码，插入位置为row，如果原row为<SMT>则原row将会被替换，否则原row将会在现row处之下
    bool RoutRemCode(QString rout_name, int row, QString code); //修改例行程序中的某行代码
    bool RoutRemCode(QString rout_name, int row, int number, QString code); //修改例行程序中的某一部分代码，row开始，number为涉及行数，若code中的行数小于number则会有代码被删除，大于则部分代码会被添加
    bool RoutDelCode(QString rout_name, int row, int number = 0); //删除例行程序中的代码，从row开始到number为止，number为0时只删除row行

public:
    QString ExportRout(RoutineData &rout_info, QString rout_name); //根据例行程序产生其文本格式
    bool ModAddRout(RoutineData &rout_data, QString rout_name, QString mod_name); //向某个模块添加例行程序
    bool MoveRout(QString old_name, QString rout_name, QString new_name); //将某个模块中的例行程序转移到另一个模块中
    QPair<QString, int> GetRoutPoint(int rout, int number); //获取一个程序指针所对应的例行程序与所在行
    QPair<int, int> GetRoutPoint(QString rout_name, int number); //根据例行程序名以及所在行获取一个程序指针
    int GetRoutMainPoint(); //获取main函数的程序指针
    QString GetUnusedMod(); //产生一个未被使用的模块名
    QString GetUnusedRout(); //产生一个未被使用的例行程序名

private:
    void UpdateTask(); //当新建、删除、重命名、加载任务以后会被执行，用于更新本地文件

    void GetHotEditDatar(std::function<void(void)> func, QSharedPointer<QMap<QString, HotEditVarInfo>> point); //获取HotEdit所需数据
    void SyntaxCheckr(std::function<void(QSharedPointer<QMap<QString,QPair<QString,int>>>)> func); //语法检查
    void TrimHotEdit(QString &codes); //根据hotedit数据对发送给服务端的代码进行微调
    void GetSendCoder(std::function<void(bool,QSharedPointer<QString>)> func); //获取将要发送给服务端的代码

    void CodeAnalysis(QSharedPointer<QMap<QString,QPair<QString,int>>> errors, QSharedPointer<QString> codes,
                      QSharedPointer<QMap<QString, HotEditVarInfo>> hot_var, QStringList mod_list = QStringList()); //代码分析

private:
    static QSharedPointer<EditData> edit_data_shared_;

    QSharedPointer<QMap<QString, HotEditVarInfo>> hot_var_point_;

    QString task_name_; //当前任务名称
    QString order_name_; //当前任务程序名称
    QString type_name_; //当前任务类型名称
    QString task_path_; //当前任务所在路径

    QMap<QString, ModuleData> mod_data_; //存放所有模块的数据
    QMap<QString, QString> code_map_; //存放指令的容器
    QMap<int, RoutMap> rout_map_; //原始代码与服务端代码的映射

    QMutex rout_mutex_;
    int rout_size_; //例行程序数量
    int main_number_; //主函数在rout_map_中的映射

    int mod_name_number_; //模块名编号
    int rou_name_number_; //程序名编号
}; //程序编辑器数据管理类

#endif // PROGRAMEDITORDATA_H
