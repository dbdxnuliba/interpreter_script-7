#ifndef TEACHSTATE_H
#define TEACHSTATE_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QSharedPointer>
#include <shared_mutex>
#include <functional>
#include <QReadWriteLock>

namespace robotTeachABB
{
    class TeachState; //示教器状态
}

enum class RunMode:char{AutoMod, ManualMod, MaxManualMod}; //运行模式
enum class ActionMode:char{Axis1_3, Axis4_6, Linear, Relocate}; //动作模式
enum class CoordinSys:char{Earth, Base, Tool, Work}; //坐标系
enum class Increment:char{None, Small, Centre, Large, User}; //增量
enum class EventLogType:char{None, Tips, TipsPop, Warning, WarningPop, Error, ErrorPop}; //事件日志消息类型
enum class ProgKeysName:char{PK1, PK2, PK3, PK4}; //快捷键名称

class TeachState : public QObject
{
    Q_OBJECT
public:
    TeachState();
    ~TeachState();
    static QSharedPointer<TeachState> instance();
    static EventLogType SoToABBLogType(int type); //将接口中的消息类型转为ABB消息类型
    static EventLogType IntToLogType(int type); //将将整数转为相对应的ABB消息类型
    static int LogTypeToInt(EventLogType type); //将ABB消息类型转为整数
    QString GetTeachVersion(); //获取示教器版本

signals:
    void Stop(bool stop); //急停
    void Elect(bool elect); //电机状态
    void Enabled(bool ebl); //使能状态
    void RunState(bool state); //运行状态
    void RunMod(RunMode mode); //运行模式
    void ActionMod(ActionMode mode); //动作模式
    void CoordSys(CoordinSys cord); //坐标系
    void Incrent(Increment ict); //增量
    void CordTool(QString tool_name); //工具坐标系
    void CordWork(QString work_name); //工件坐标系
    void CordLoad(QString load_name); //有效载荷

public:
    void SendStateSignals(); //发送一次所有信号，一般用于初始化，同时会对坐标等数据进行初始化

    bool GetControlState(); //判断是否拥有控制权

    void SetRunState(bool isrun); //设置程序运行状态
    bool GetRunState(); //获取程序运行状态

    void SetCodeState(bool ismodify = true); //修改代码状态，默认为真
    bool GetCodeState();

    void SetSendLogFun(std::function<void(EventLogType, QString, QString)> func); //设置消息接收者
    void SendLog(EventLogType type, QString data, QString info = QString()); //发送消息到事件日志中

    bool SetStop(bool stop); //设置急停状态
    bool GetStop();

    bool SetElect(bool elect); //设置电机状态
    bool GetElect();

    bool SetEnabled(bool enabled); //设置使能状态
    bool GetEnabled();

    bool SetRunMode(RunMode mode); //设置运行模式
    RunMode GetRunMode();

    bool SetActionMode(ActionMode mode); //设置动作模式
    ActionMode GetActionMode();

    bool SetCoordinSys(CoordinSys cos); //设置坐标系
    CoordinSys GetCoordinSys();

    bool SetIncrement(Increment ict); //设置增量
    bool SetIncrVal(double as, double lr, double re); //设置增量为User时的增量值
    bool SetIncrVal(ActionMode act_mod, double val);
    Increment GetIncrement();
    double GetIncrVar(ActionMode act_mod);

    bool SetCurrentTool(QString name); //修改当前工具坐标系
    QString GetCurrentTool();

    bool SetCurrentWork(QString name); //修改当前工件坐标系
    QString GetCurrentWork();

    bool SetCurrentLoad(QString load); //修改当前有效载荷
    QString GetCurrentLoad();

    void SetLanguage(QString lang_type); //设置示教器语言
    QString GetLanguage();

    void SetProgKeyInfo(ProgKeysName key_name, int type, int keys, int isauto, QString select); //修改快捷键信息
    void SaveProgKeyInfo(); //将快捷键信息保存到本地，如果被修改了的话
    int GetPKType(ProgKeysName key_name); //获取快捷键类型 0为无，1为输入，2为输出，3为系统
    int GetPKKeys(ProgKeysName key_name); //获取快捷键按下后的动作
    int GetPKAuto(ProgKeysName key_name); //获取快捷键是否允许在自动模式下运行
    QString GetPKSelect(ProgKeysName key_name); //获取所选中项，在快捷键类型为1和2时有效

    void SetTaskInfo(QString name, QString order_name, QString type);
    QString GetTaskName();
    QString GetTaskOrderName();
    QString GetTaskType();

private:
    void SetTeachVersion(QString version); //设置示教器版本
    void SetIncrValpriv(double as, double lr, double re); //强制设置增量

    void ReadPK(); //从本地读取快捷键设置信息
    void WritePK(); //当快捷键信息被修改后将其写入到本地

private:
    static std::mutex point_mutex_;
    static QSharedPointer<TeachState> teach_state_pointer_;
    QString version_;
    QReadWriteLock *mutex_;
    std::atomic_bool is_modify_code_; //代码是否修改，为真时代表已修改，已修改则需要将代码重写发送给服务端
    std::atomic_bool isrun_; //程序是否运行

    struct IncrementVal
    {
        void SetVal(double as, double lr, double re)
        {
            axis = as;
            linear = lr;
            relocate = re;
        }

        double axis;
        double linear;
        double relocate;
    }; //增量值

    struct ProgKeysInfo
    {
        QString select;
        int type;
        int keys;
        int isauto;
    }; //快捷键

    ProgKeysInfo pks_[4]; //快捷键
    IncrementVal incr_val_; //增量值
    std::function<void(EventLogType type, QString data, QString info)> log_func_; //日志回调函数
    QString curr_tool_; //当前工具坐标系
    QString curr_work_; //当前工件坐标系
    QString curr_load_; //当前有效载荷
    QString language_type_; //示教器语言类型
    QString task_name_; //任务名称
    QString task_order_name_; //程序名称
    QString task_type_name_; //类型
    bool stop_; //急停状态
    bool elect_mach_; //电机状态
    bool enabled_; //使能状态
    bool ispkmod_; //快捷键是否被修改
    RunMode run_mode_; //运行模式
    ActionMode action_mode_; //动作模式
    CoordinSys coordin_system_; //坐标系 
    Increment increment_; //增量

}; //示教器状态

#endif // TEACHSTATE_H
