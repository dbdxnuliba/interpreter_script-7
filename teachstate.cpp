#include "teachstate.h"
#include <QApplication>
#include <QSettings>
#include "RobotClient/robotClient.h"
#include "MainWidget/commoncontrols.h"

using namespace std;
using namespace rclib;

mutex TeachState::point_mutex_;
QSharedPointer<TeachState> TeachState::teach_state_pointer_;

TeachState::TeachState() : QObject (nullptr)
{
    mutex_ = new QReadWriteLock;
    is_modify_code_ = true;
    stop_ = false;
    elect_mach_ = false;
    enabled_ = false;
    isrun_ = false;
    run_mode_ = RunMode::AutoMod;
    action_mode_ = ActionMode::Axis1_3;
    coordin_system_ = CoordinSys::Tool;
    increment_ = Increment::None;
    SetIncrValpriv(0.0001, 0.05, 0.0005);

    this->SetTeachVersion("1.1.4");
}

TeachState::~TeachState()
{
    if(ispkmod_)
        WritePK();
    delete mutex_;
}

QSharedPointer<TeachState> TeachState::instance()
{
    if(teach_state_pointer_.isNull())
    {
        point_mutex_.lock();
        if(teach_state_pointer_.isNull())
        {
            teach_state_pointer_ = QSharedPointer<TeachState>(new TeachState);
            teach_state_pointer_->ReadPK();
        }
        point_mutex_.unlock();
    }

    return teach_state_pointer_;
}

EventLogType TeachState::SoToABBLogType(int type)
{
    switch(type)
    {
    case 1: return EventLogType::Error;
    case 2: return EventLogType::Warning;
    case 3: return EventLogType::Tips;
    default: return EventLogType::None;
    }
}

EventLogType TeachState::IntToLogType(int type)
{
    switch(type)
    {
    case 1: return EventLogType::Tips;
    case 2: return EventLogType::TipsPop;
    case 3: return EventLogType::Warning;
    case 4: return EventLogType::WarningPop;
    case 5: return EventLogType::Error;
    case 6: return EventLogType::ErrorPop;
    default: return EventLogType::None;
    }
}

int TeachState::LogTypeToInt(EventLogType type)
{
    switch(type)
    {
    case EventLogType::Tips: return 1;
    case EventLogType::TipsPop: return 2;
    case EventLogType::Warning: return 3;
    case EventLogType::WarningPop: return 4;
    case EventLogType::Error: return 5;
    case EventLogType::ErrorPop: return 6;
    default: return 0;
    }
}

QString TeachState::GetTeachVersion()
{
    return version_;
}

void TeachState::SendStateSignals()
{
    SetCurrentTool("tool0");
    SetCurrentWork("wobj0");
    SetCurrentLoad("load0");

    auto state = RobotClient::initance()->getRobotState();

    if(state.getServoState() == SWITCHSTATE::SWITCHON) //如果机器伺服已打开，则调整自身伺服状态
    {
        if(GetControlState() && GetElect() && GetRunMode() == RunMode::ManualMod)
        {
            QWriteLocker lk(mutex_);
            enabled_ = true;
        }
    }

    QReadLocker sk(mutex_);

    emit Stop(stop_);
    emit Elect(elect_mach_);
    emit Enabled(enabled_);
    emit RunMod(run_mode_);
    emit ActionMod(action_mode_);
    emit CoordSys(coordin_system_);
    emit Incrent(increment_);
    emit CordTool(curr_tool_);
    emit CordWork(curr_work_);

    sk.unlock();
}

bool TeachState::GetControlState()
{
    if(RobotClient::initance()->getControlState() == SWITCHSTATE::SWITCHON)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TeachState::SetRunState(bool isrun)
{
    QWriteLocker lk(mutex_);

    isrun_ = isrun;
    lk.unlock();

    emit RunState(isrun);
}

void TeachState::SetCodeState(bool ismodify)
{
    if(this->GetCodeState() == ismodify)
        return;

    QWriteLocker lk(mutex_);

    is_modify_code_ = ismodify;
}

bool TeachState::GetCodeState()
{
    QReadLocker sl(mutex_);

    return is_modify_code_;
}

void TeachState::SetSendLogFun(std::function<void(EventLogType, QString, QString)> func)
{
    log_func_ = func;
}

void TeachState::SendLog(EventLogType type, QString data, QString info)
{
    try {
        log_func_(type, data, info);
    } catch (exception) {
        return;
    }
}

bool TeachState::SetStop(bool stop)
{
    if(!GetControlState())
    {
        SendLog(EventLogType::ErrorPop, tr("操控急停按钮失败"), tr("无控制权"));
        return false;
    }

    if(stop)
    {
        if(GetElect())
            SetElect(false);
    }

    QWriteLocker lk(mutex_);
    if(stop == stop_)
        return true;

    stop_ = stop;
    lk.unlock();

    emit Stop(stop);

    if(stop)
        SendLog(EventLogType::WarningPop, tr("急停开启"));
    else
        SendLog(EventLogType::Tips, tr("急停关闭"));

    return true;
}

bool TeachState::GetStop()
{
    QReadLocker sk(mutex_);
    return stop_;
}

bool TeachState::SetElect(bool elect)
{
    if(!GetControlState())
    {
        SendLog(EventLogType::ErrorPop, tr("操控电机失败"), tr("无控制权"));
        return false;
    }

    if(!elect)
    {
        if(GetEnabled())
            SetEnabled(false);
    }

    QWriteLocker lk(mutex_);

    if(stop_ && elect)
    {
        SendLog(EventLogType::ErrorPop, tr("电机开启失败"), tr("当前处于急停状态"));
        return false;
    }
    if(elect == elect_mach_)
        return true;

    elect_mach_ = elect;
    lk.unlock();

    emit Elect(elect);

    if(elect)
        SendLog(EventLogType::Tips, tr("电机开启"));
    else
        SendLog(EventLogType::Tips, tr("电机关闭"));

    return true;
}

bool TeachState::GetElect()
{
    QReadLocker sk(mutex_);
    return  elect_mach_;
}

bool TeachState::SetEnabled(bool enabled)
{
    if(!GetControlState())
    {
        SendLog(EventLogType::ErrorPop, tr("操控使能按钮失败"), tr("无控制权"));
        return false;
    }
    if(!GetElect())
    {
        SendLog(EventLogType::ErrorPop, tr("操控使能按钮失败"), tr("电机未开启"));
        return false;
    }
    if((GetRunMode() != RunMode::ManualMod) && enabled != false)
    {
        SendLog(EventLogType::ErrorPop, tr("操控使能按钮失败"), tr("该运行模式下不允许此操作"));
        return false;
    }

    if(!enabled)
    {
        if(GetRunState() != 0) //如果程序正在运行
        {
            RobotClient::initance()->programStop(); //停止程序
            RobotClient::initance()->stop();
            this->SetRunState(false);
        }
    }

    QWriteLocker lk(mutex_);
    if(enabled == enabled_)
        return true;

    enabled_ = enabled;
    lk.unlock();

    SWITCHSTATE state;
    if(!enabled)
        state = SWITCHOFF;
    else
        state = SWITCHON;
    RobotClient::initance()->setServoState(state);

    emit Enabled(enabled);

    return true;
}

bool TeachState::GetEnabled()
{
    QReadLocker sl(mutex_);
    return  enabled_;
}

bool TeachState::GetRunState()
{
    return isrun_;
}

bool TeachState::SetRunMode(RunMode mode)
{
    if(!GetControlState())
    {
        SendLog(EventLogType::ErrorPop, tr("切换运行模式失败"), tr("无控制权"));
        return false;
    }
    if(GetRunState() != 0)
    {
        SendLog(EventLogType::ErrorPop, tr("切换运行模式失败"), tr("当前程序正在运行"));
        return false;
    }

    QWriteLocker lk(mutex_);
    if(mode == run_mode_)
        return true;
    run_mode_ = mode;
    lk.unlock();

    emit RunMod(mode);

    switch(mode)
    {
    case RunMode::AutoMod:
    {
        SendLog(EventLogType::Tips, tr("切换至自动模式"));
        this->SetElect(false); //电机下电
        break;
    }
    case RunMode::ManualMod:
    {
        SendLog(EventLogType::Tips, tr("切换至手动模式"));
        break;
    }
    case RunMode::MaxManualMod:
    {
        SendLog(EventLogType::Tips, tr("切换至全速手动模式"));
        break;
    }
    }

    return true;
}

RunMode TeachState::GetRunMode()
{
    QReadLocker sk(mutex_);
    return run_mode_;
}

bool TeachState::SetActionMode(ActionMode mode)
{
    QWriteLocker lk(mutex_);

    action_mode_ = mode;
    lk.unlock();

    emit ActionMod(mode);

    return true;
}

ActionMode TeachState::GetActionMode()
{
    QReadLocker sk(mutex_);
    return  action_mode_;
}

bool TeachState::SetCoordinSys(CoordinSys cos)
{
    QWriteLocker lk(mutex_);

    if(action_mode_ == ActionMode::Axis1_3 || action_mode_ == ActionMode::Axis4_6)
        return false;

    coordin_system_ = cos;
    lk.unlock();

    emit CoordSys(cos);

    return true;
}

CoordinSys TeachState::GetCoordinSys()
{
    QReadLocker sk(mutex_);
    return coordin_system_;
}

bool TeachState::SetIncrement(Increment ict)
{
    QWriteLocker lk(mutex_);

    increment_ = ict;

    lk.unlock();

    emit Incrent(ict);

    return true;
}

bool TeachState::SetIncrVal(double as, double lr, double re)
{
    QWriteLocker lk(mutex_);

    if(increment_ != Increment::User)
        return false;

    if(as < 0.0 || as > 0.0025) //限定最大值
        return false;
    if(lr < 0.0 || lr > 5.0)
        return false;
    if(re < 0.0 || re > 0.009)
        return false;

    incr_val_.SetVal(as, lr, re);

    return true;
}

bool TeachState::SetIncrVal(ActionMode act_mod, double val)
{
    QWriteLocker lk(mutex_);

    if(increment_ != Increment::User)
        return false;

    switch(act_mod)
    {
    case ActionMode::Axis1_3:
    {
        if(val < 0.0 || val > 0.0025)
            return false;
        incr_val_.axis = val;
        return true;
    }
    case ActionMode::Axis4_6:
    {
        if(val < 0.0 || val > 0.0025)
            return false;
        incr_val_.axis = val;
        return true;
    }
    case ActionMode::Linear:
    {
        if(val < 0.0 || val > 5.0)
            return false;
        incr_val_.linear = val;
        return true;
    }
    case ActionMode::Relocate:
    {
        if(val < 0.0 || val > 0.009)
            return false;
        incr_val_.relocate = val;
        return true;
    }
    }
}

Increment TeachState::GetIncrement()
{
    QReadLocker sk(mutex_);
    return increment_;
}

double TeachState::GetIncrVar(ActionMode act_mod)
{
    QReadLocker sk(mutex_);
    auto mod = increment_;

    switch(act_mod)
    {
    case ActionMode::Axis1_3:
    {
        switch(mod)
        {
        case Increment::None: return 0;
        case Increment::Small: return 0.0001;
        case Increment::Centre: return 0.0004;
        case Increment::Large: return 0.0025;
        case Increment::User: return incr_val_.axis;
        }
    }
    case ActionMode::Axis4_6:
    {
        switch(mod)
        {
        case Increment::None: return 0;
        case Increment::Small: return 0.0001;
        case Increment::Centre: return 0.0004;
        case Increment::Large: return 0.0025;
        case Increment::User: return incr_val_.axis;
        }
    }
    case ActionMode::Linear:
    {
        switch(mod)
        {
        case Increment::None: return 0;
        case Increment::Small: return 0.05;
        case Increment::Centre: return 1;
        case Increment::Large: return 5;
        case Increment::User: return incr_val_.linear;
        }
    }
    case ActionMode::Relocate:
    {
        switch(mod)
        {
        case Increment::None: return 0;
        case Increment::Small: return 0.0005;
        case Increment::Centre: return 0.004;
        case Increment::Large: return 0.009;
        case Increment::User: return incr_val_.relocate;
        }
    }
    }
}

bool TeachState::SetCurrentTool(QString name)
{
    if(!GetControlState())
    {
        SendLog(EventLogType::ErrorPop, tr("修改工具坐标系失败"), tr("无控制权"));
        return false;
    }
    if(GetRunState() != 0)
    {
        SendLog(EventLogType::ErrorPop, tr("修改工具坐标系失败"), tr("当前程序正在运行"));
        return false;
    }

    try {
        auto &info = VarInfo::instance()->GetUcsVarInfo(name);
        auto point = Convert::DataTypeToVarFormData("tooldata", info.var.toString());

        if(point->subnode.at(2)->subnode.at(0)->code == "-1")
        {
            throw ABBException("TeachState", "SetCurrentTool", "1");
        }

        auto frame = RobotClient::initance()->getRobotFrame();
        if(frame.setCurrentToolFrame(name.toStdString()) == 0)
        {
            QWriteLocker lk(mutex_);

            curr_tool_ = name;
            lk.unlock();

            RobotClient::initance()->setRobotFrame(frame);

            emit CordTool(name);
            return true;
        }
        else
        {
            return false;
        }
    } catch (ABBException) {
        SendLog(EventLogType::WarningPop, tr("修改工具坐标系失败"), tr("变量不存在"));
        return false;
    }

}

QString TeachState::GetCurrentTool()
{
    QReadLocker sk(mutex_);
    return curr_tool_;
}

bool TeachState::SetCurrentWork(QString name)
{
    if(!GetControlState())
    {
        SendLog(EventLogType::ErrorPop, tr("修改工件坐标系失败"), tr("无控制权"));
        return false;
    }
    if(GetRunState() != 0)
    {
        SendLog(EventLogType::ErrorPop, tr("修改工件坐标系失败"), tr("当前程序正在运行"));
        return false;
    }

    try {
        auto frame = RobotClient::initance()->getRobotFrame();
        if(frame.setCurrentWorkFrame(name.toStdString()) == 0)
        {
            QWriteLocker lk(mutex_);

            curr_work_ = name;
            lk.unlock();

            RobotClient::initance()->setRobotFrame(frame);

            emit CordWork(name);

            return true;
        }
        else
        {
            return false;
        }
    } catch (ABBException) {
        SendLog(EventLogType::WarningPop, tr("修改工件坐标系失败"), tr("变量不存在"));
        return false;
    }

}

QString TeachState::GetCurrentWork()
{
    QReadLocker sk(mutex_);
    return curr_work_;
}

bool TeachState::SetCurrentLoad(QString load)
{
    if(!VarInfo::instance()->IsExist(load))
    {
        SendLog(EventLogType::ErrorPop, tr("修改有效载荷失败"), tr("变量不存在"));
        return false;
    }

    QWriteLocker lk(mutex_);

    curr_load_ = load;

    lk.unlock();
    emit CordLoad(load);

    return true;
}

QString TeachState::GetCurrentLoad()
{
    QReadLocker sk(mutex_);
    return curr_load_;
}

void TeachState::SetLanguage(QString lang_type)
{
    language_type_ = lang_type;
}

QString TeachState::GetLanguage()
{
    return language_type_;
}

void TeachState::SetProgKeyInfo(ProgKeysName key_name, int type, int keys, int isauto, QString select)
{
    switch(key_name)
    {
    case ProgKeysName::PK1:
    {
        pks_[0].type = type;
        pks_[0].keys = keys;
        pks_[0].isauto = isauto;
        pks_[0].select = select;
        ispkmod_ = true;

        break;
    }
    case ProgKeysName::PK2:
    {
        pks_[1].type = type;
        pks_[1].keys = keys;
        pks_[1].isauto = isauto;
        pks_[1].select = select;
        ispkmod_ = true;

        break;
    }
    case ProgKeysName::PK3:
    {
        pks_[2].type = type;
        pks_[2].keys = keys;
        pks_[2].isauto = isauto;
        pks_[2].select = select;
        ispkmod_ = true;

        break;
    }
    case ProgKeysName::PK4:
    {
        pks_[3].type = type;
        pks_[3].keys = keys;
        pks_[3].isauto = isauto;
        pks_[3].select = select;
        ispkmod_ = true;

        break;
    }
    }
}

void TeachState::SaveProgKeyInfo()
{
    if(ispkmod_)
        WritePK();
}

int TeachState::GetPKType(ProgKeysName key_name)
{
    switch(key_name)
    {
    case ProgKeysName::PK1: return pks_[0].type;
    case ProgKeysName::PK2: return pks_[1].type;
    case ProgKeysName::PK3: return pks_[2].type;
    case ProgKeysName::PK4: return pks_[3].type;
    }
}

int TeachState::GetPKKeys(ProgKeysName key_name)
{
    switch(key_name)
    {
    case ProgKeysName::PK1: return pks_[0].keys;
    case ProgKeysName::PK2: return pks_[1].keys;
    case ProgKeysName::PK3: return pks_[2].keys;
    case ProgKeysName::PK4: return pks_[3].keys;
    }
}

int TeachState::GetPKAuto(ProgKeysName key_name)
{
    switch(key_name)
    {
    case ProgKeysName::PK1: return pks_[0].isauto;
    case ProgKeysName::PK2: return pks_[1].isauto;
    case ProgKeysName::PK3: return pks_[2].isauto;
    case ProgKeysName::PK4: return pks_[3].isauto;
    }
}

QString TeachState::GetPKSelect(ProgKeysName key_name)
{
    switch(key_name)
    {
    case ProgKeysName::PK1: return pks_[0].select;
    case ProgKeysName::PK2: return pks_[1].select;
    case ProgKeysName::PK3: return pks_[2].select;
    case ProgKeysName::PK4: return pks_[3].select;
    }
}

void TeachState::SetTaskInfo(QString name, QString order_name, QString type)
{
    task_name_ = name;
    task_order_name_ = order_name;
    task_type_name_ = type;
}

QString TeachState::GetTaskName()
{
    return task_name_;
}

QString TeachState::GetTaskOrderName()
{
    return task_order_name_;
}

QString TeachState::GetTaskType()
{
    return task_type_name_;
}

void TeachState::SetTeachVersion(QString version)
{
    version_ = version;
}

void TeachState::SetIncrValpriv(double as, double lr, double re)
{
    incr_val_.SetVal(as, lr, re);
}

void TeachState::ReadPK()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/ProgKeys/pk.ini";

    QSettings st(path, QSettings::IniFormat);

    pks_[0].type = st.value("pk1/type").toInt();
    pks_[0].keys = st.value("pk1/keys").toInt();
    pks_[0].isauto = st.value("pk1/auto").toInt();
    pks_[0].select = st.value("pk1/select").toString();

    pks_[1].type = st.value("pk2/type").toInt();
    pks_[1].keys = st.value("pk2/keys").toInt();
    pks_[1].isauto = st.value("pk2/auto").toInt();
    pks_[1].select = st.value("pk2/select").toString();

    pks_[2].type = st.value("pk3/type").toInt();
    pks_[2].keys = st.value("pk3/keys").toInt();
    pks_[2].isauto = st.value("pk3/auto").toInt();
    pks_[2].select = st.value("pk3/select").toString();

    pks_[3].type = st.value("pk4/type").toInt();
    pks_[3].keys = st.value("pk4/keys").toInt();
    pks_[3].isauto = st.value("pk4/auto").toInt();
    pks_[3].select = st.value("pk4/select").toString();

    ispkmod_ = false;
}

void TeachState::WritePK()
{
    auto path = qApp->applicationDirPath() + "/data/RAPID/ProgKeys/pk.ini";

    QSettings st(path, QSettings::IniFormat);

    st.setValue("pk1/type", pks_[0].type);
    st.setValue("pk1/keys", pks_[0].keys);
    st.setValue("pk1/auto", pks_[0].isauto);
    if(pks_[0].select.isEmpty())
        st.setValue("pk1/select", -1);
    else
        st.setValue("pk1/select", pks_[0].select);

    st.setValue("pk2/type", pks_[1].type);
    st.setValue("pk2/keys", pks_[1].keys);
    st.setValue("pk2/auto", pks_[1].isauto);
    if(pks_[1].select.isEmpty())
        st.setValue("pk2/select", -1);
    else
        st.setValue("pk2/select", pks_[1].select);

    st.setValue("pk3/type", pks_[2].type);
    st.setValue("pk3/keys", pks_[2].keys);
    st.setValue("pk3/auto", pks_[2].isauto);
    if(pks_[2].select.isEmpty())
        st.setValue("pk3/select", -1);
    else
        st.setValue("pk3/select", pks_[2].select);

    st.setValue("pk4/type", pks_[3].type);
    st.setValue("pk4/keys", pks_[3].keys);
    st.setValue("pk4/auto", pks_[3].isauto);
    if(pks_[3].select.isEmpty())
        st.setValue("pk4/select", -1);
    else
        st.setValue("pk4/select", pks_[3].select);

    ispkmod_ = false;
}
