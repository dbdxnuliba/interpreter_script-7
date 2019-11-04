#include "programeditordata.h"
#include "teachstate.h"
#include "../../commoncontrols.h"
#include "MainWidget/Functions/HotEdit/hotedit.h"
#include "RobotClient/robotClient.h"
#include <QApplication>
#include <QMessageBox>
#include <random>
#include <QSettings>
#include <thread>
#include <QDir>

QSharedPointer<EditData> EditData::edit_data_shared_;

void UpdateCoordinate(QSharedPointer<QList<QPair<std::string, bool>>> coordinate) //与服务端坐标进行对比,更新或修改服务端坐标
{
    auto frame = rclib::RobotClient::initance()->getRobotFrame();
    auto tool_data = frame.getToolFrameList();
    auto work_data = frame.getWorkFrameList();
    auto ismodif = false; //是否修改

    for(auto var : *coordinate)
    {
        auto var_info = VarInfo::instance()->GetUcsVarInfo(var.first.c_str());
        QList<double> xyz;
        Quaternion qn;
        Euler er;

        if(var.second)
        {
            auto too = Convert::DataTypeToVarFormData("tooldata", var_info.var.toString());
            xyz = Convert::XYZCodeToDoubles(too->subnode.at(1)->subnode.at(0)->code);
            qn.SetData(too->subnode.at(1)->subnode.at(1)->code);
            er = Convert::QuaternionToEuler(qn);
            rclib::Terminal tel(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

            if(tool_data.find(var.first) == tool_data.end()) //不存在
            {
                frame.addToolFrame(var.first, tel);
                ismodif = true;
            }
            else
            {
                if(tel != tool_data[var.first]) //值不同
                {
                    frame.modifyToolFrame(var.first, tel);
                    ismodif = true;
                }
            }
        }
        else
        {
            auto too = Convert::DataTypeToVarFormData("wobjdata", var_info.var.toString());
            xyz = Convert::XYZCodeToDoubles(too->subnode.at(3)->subnode.at(0)->code);
            qn.SetData(too->subnode.at(3)->subnode.at(1)->code);
            er = Convert::QuaternionToEuler(qn);
            rclib::Terminal tel(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

            if(work_data.find(var.first) == work_data.end()) //不存在
            {
                frame.addWorkFrame(var.first, tel);
                ismodif = true;
            }
            else
            {
                if(tel != work_data[var.first]) //值不同
                {
                    frame.addWorkFrame(var.first, tel);
                    ismodif = true;
                }
            }
        }
    }
    if(ismodif)
    {
        rclib::RobotClient::initance()->setRobotFrame(frame);
    }
}

EditData::EditData(QObject *parent) : QObject(parent)
{
}

QSharedPointer<EditData> EditData::instance()
{
    if(edit_data_shared_.isNull())
    {
        edit_data_shared_ = QSharedPointer<EditData>(new EditData);

        edit_data_shared_->rout_size_ = 0;
        edit_data_shared_->main_number_ = -1;
        edit_data_shared_->mod_name_number_ = 0;
        edit_data_shared_->rou_name_number_ = 0;
    }

    return edit_data_shared_;
}

void EditData::SetTaskInfo(QString task_name, QString order_name, QString type_name, QString task_path)
{
    task_name_ = task_name;
    order_name_ = order_name;
    type_name_ = type_name;
    task_path_ = task_path;

    TeachState::instance()->SetTaskInfo(task_name_, order_name_, type_name_);
}

void EditData::SetTaskName(QString task_name)
{
    task_name_ = task_name;

    TeachState::instance()->SetTaskInfo(task_name_, order_name_, type_name_);
}

void EditData::SetTaskOrderName(QString order_name, QString task_path, QString type_name)
{
    if(order_name_ == order_name)
        return;

    order_name_ = order_name;
    if(order_name == "NewProgramName" || order_name == "<No Program>") {
        type_name_ = "Normal";
    }
    else {
        if(!task_path.isEmpty())
            task_path_ = task_path;
        if(!type_name.isEmpty())
            type_name_ = type_name;
    }

    TeachState::instance()->SetTaskInfo(task_name_, order_name_, type_name_);

    UpdateTask();
}

void EditData::SetTaskTypeName(QString type_name)
{
    if(type_name == type_name_)
        return;

    type_name_ = type_name;

    TeachState::instance()->SetTaskInfo(task_name_, order_name_, type_name_);

    UpdateTask();
}

void EditData::SetTaskPath(QString task_path)
{
    if(task_path == task_path_)
        return;

    task_path_ = task_path;
    UpdateTask();
}

QString EditData::GetTaskName()
{
    return task_name_;
}

QString EditData::GetTaskOrderName()
{
    return order_name_;
}

QString EditData::GetTaskTypeName()
{
    return type_name_;
}

QString EditData::GetTaskPath()
{
    return task_path_;
}

void EditData::SaveMod()
{
    QString path = qApp->applicationDirPath() + "/data/RAPID/Modular/" + task_path_ + "/";

    for(auto it = mod_data_.begin(); it != mod_data_.end(); ++it)
    {
        auto temp = path;
        if(it->type)
            temp.append("System/" + it.key() + ".SYS");
        else
            temp.append("User/" + it.key() + ".MOD");

        QFile file(temp);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            continue;

        auto data = this->ExportMod(it.key(), true, true);

        file.write(data.toUtf8());

        file.close();
    }
}

void EditData::SaveMod(QString mod_name)
{
    QString path = qApp->applicationDirPath() + "/data/RAPID/Modular/" + task_path_ + "/";

    if(!IsExist(mod_name))
        return;

    if(mod_data_.value(mod_name).type)
        path.append("System/" + mod_name + ".SYS");
    else
        path.append("User/" + mod_name + ".MOD");

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    auto data = ExportMod(mod_name, true, true);

    file.write(data.toUtf8());
    file.close();
}

QMap<QString, QString> *EditData::GetCodeMap()
{
    return &code_map_;
}

bool EditData::AnalysisData()
{
    if(task_name_.isEmpty() || order_name_.isEmpty() || type_name_.isEmpty())
        return false;

    if(!mod_data_.isEmpty())
    {
        DelTask();
    }

    QString path = qApp->applicationDirPath() + "/data/RAPID/Modular/" + task_path_ + "/";

    QSharedPointer<QDir> dir;
    //保存坐标，用于在分析完成后在新的一个临时线程中与服务端现有的坐标进行分析与对照，添加或修改服务端坐标坐标
    auto coordinate = QSharedPointer<QList<QPair<std::string, bool>>>(new QList<QPair<std::string, bool>>);
    for(int i = 0; i < 2; i++)
    {
        if(i == 0)
            dir = QSharedPointer<QDir>(new QDir(path + "System/"));
        else
            dir = QSharedPointer<QDir>(new QDir(path + "User/"));

        dir->setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        auto list = dir->entryInfoList();

        for(auto info : list)
        {
            if(info.suffix() != "MOD" && info.suffix() != "SYS")
                continue;
            if(info.fileName().isEmpty() || info.isHidden())
                continue;

            QFile file(info.path() + "/" + info.fileName());
            if(!file.open(QIODevice::ReadOnly))
                continue;
            QString file_data = file.readAll();
            LoadMod(file_data, coordinate);
        }
    }

    std::thread td(UpdateCoordinate, coordinate); //启动一个新的线程与服务端进行坐标同步
    td.detach(); //分离此线程，使其不受主线程影响

    TeachState::instance()->SetCodeState();

    return true;
}

bool EditData::LoadMod(QString &mod_data, QSharedPointer<QList<QPair<std::string, bool>>> coord, bool join)
{
    mod_data = mod_data.replace("\t", "").replace("\r", ""); //去除掉可能存在的制表符以及回车符

    bool iscoord = false;
    if(coord.isNull())
    {
        coord = QSharedPointer<QList<QPair<std::string, bool>>>(new QList<QPair<std::string, bool>>);
        iscoord = true;
    }

    QString mod_name; //模块名
    ModuleData mod_info; //模块数据

    QStringList data_list = mod_data.split("\n");
    for(int i = 0; i < data_list.size(); )
    {
        QString text = data_list.at(i);

        if(!text.isEmpty()) //排除掉空行
        {
            bool iscon = false;
            while(text.at(0) == ' ') //如果开头是空格
            {
                text = text.mid(1); //去除开头空格
                if(text.isEmpty())
                {
                    iscon = true;
                    break;
                }
            }
            if(iscon)
            {
                i++;
                continue;
            }
        }
        QString head = text.section(" ", 0, 0);

        if(text.isEmpty()) //空行
        {
            if(mod_name.isEmpty())
            {
                i++;
                continue;
            }
            //mod_info.mod_code.append("\n"); //去除空行，因为保留空行会出现问题
        }
        else if(text.at(0) == '!') //备注
        {
            mod_info.mod_code.append(text);
        }
        else if(head == "MODULE") //获取模块基本信息
        {
            QString data = text.mid(7);
            int a = data.indexOf("("); //判断该模块是否拥有参数

            if(a == -1) //无参数，程序模块
            {
                mod_name = data.replace(" ", ""); //获取模块名，并去除可能存在的空格
            }
            else //有参数，可能为系统模块
            {
                mod_name = data.mid(0, a).replace(" ", ""); //获取模块名，并去除可能存在的空格
                int b = data.indexOf(")");
                mod_info.param = data.mid(a+1, b-a-1); //获取参数
                if(mod_info.param.indexOf("SYSMODULE") != -1) //为系统模块
                    mod_info.type = true;
            }

            QString mod_head = "Module";

            if(mod_name.mid(0, mod_head.size()) == mod_head)
            {
                auto mod_name_number = mod_name.mid(mod_head.size());
                bool isint = false;
                int num = mod_name_number.toInt(&isint);
                if(isint)
                {
                    if(num > mod_name_number_)
                        mod_name_number_ = num;
                }
            }
        }
        else if(head == "VAR" || head == "PERS" || head == "CONST")
        {
            VarDataInfo var_info("");
            QString tmp;

            var_info.data_type = text.section(" ", 1, 1);
            if(head == "VAR")
            {
                var_info.nature = 0;
                tmp = text.mid(var_info.data_type.size() + 5);
            }
            else if(head == "PERS")
            {
                var_info.nature = 1;
                tmp = text.mid(var_info.data_type.size() + 6);
            }
            else
            {
                var_info.nature = 2;
                tmp = text.mid(var_info.data_type.size() + 7);
            }
            var_info.genre = true;
            var_info.action_scope = "all";
            var_info.mod = mod_name;

            tmp.replace(" ", ""); //去除可能存在的空格
            var_info.var = tmp.section(":=", 1, 1).replace(";", "");
            QString name_tmp = tmp.section(":=", 0, 0);
            QString var_name;
            if(name_tmp.indexOf("{") != -1)
            {
                int a = name_tmp.indexOf("{");
                int b = name_tmp.indexOf("}");

                var_info.dim = name_tmp.mid(a + 1, b - a - 1);

                var_name = name_tmp.mid(0, a);
            }
            else //维数不存在
            {
                var_name = tmp.section(":=", 0, 0);
                var_name.replace(";", "");
            }
            var_info.init_var = var_info.var;

            if(join) //为真时才将分析出来的变量保存进变量类中
            {
                var_info.var_name = var_name;
                bool isadd = VarInfo::instance()->AddVar(var_name, var_info);

                if(isadd && var_info.data_type == "tooldata")
                    coord->append(qMakePair(var_name.toStdString(), true));
                else if(isadd && var_info.data_type == "wobjdata")
                    coord->append(qMakePair(var_name.toStdString(), false));
            }

            mod_info.mod_code.append(text);
        }
        else if(text.indexOf("LOCAL VAR") != -1 || text.indexOf("LOCAL PERS") != -1 || text.indexOf("LOCAL CONST") != -1)
        {
            VarDataInfo var_info("");
            QString tmp;

            var_info.data_type = text.section(" ", 2, 2);
            if(text.indexOf("LOCAL VAR") != -1)
            {
                var_info.nature = 0;
                tmp = text.mid(var_info.data_type.size() + 10);
            }
            else if(text.indexOf("LOCAL PERS") != -1)
            {
                var_info.nature = 1;
                tmp = text.mid(var_info.data_type.size() + 11);
            }
            else
            {
                var_info.nature = 2;
                tmp = text.mid(var_info.data_type.size() + 11);
            }
            var_info.genre = true;
            var_info.action_scope = "local";
            var_info.mod = mod_name;

            tmp.replace(" ", "");
            var_info.var = tmp.section("!=", 1, 1).replace(";", "");
            QString name_tmp = tmp.section(":=", 0, 0);
            QString var_name;
            if(name_tmp.indexOf("{") != -1)
            {
                int a = name_tmp.indexOf("{");
                int b = name_tmp.indexOf("}");

                var_info.dim = name_tmp.mid(a + 1, b - a - 1);

                var_name = name_tmp.mid(0, a);
            }
            else //维数不存在
            {
                var_name = tmp.section(":=", 0, 0);
                var_name.replace(";", "");
            }
            var_info.init_var = var_info.var;

            if(join) //为真时才将分析出来的变量保存进变量类中
            {
                var_info.var_name = var_name;
                bool isadd = VarInfo::instance()->AddVar(var_name, var_info);

                if(isadd && var_info.data_type == "tooldata")
                    coord->append(qMakePair(var_name.toStdString(), true));
                else if(isadd && var_info.data_type == "wobjdata")
                    coord->append(qMakePair(var_name.toStdString(), false));
            }

            mod_info.mod_code.append(text);
        }
        else if(text.indexOf("TASK VAR") != -1 || text.indexOf("TASK PERS") != -1)
        {
            VarDataInfo var_info("");
            QString tmp;

            var_info.data_type = text.section(" ", 2, 2);
            if(text.indexOf("TASK VAR") != -1)
            {
                var_info.nature = 0;

                tmp = text.mid(var_info.data_type.size() + 10);
            }
            else if(text.indexOf("TASK PERS") != -1)
            {
                var_info.nature = 1;
                tmp = text.mid(var_info.data_type.size() + 11);
            }
            var_info.genre = true;
            var_info.action_scope = "task";
            var_info.mod = mod_name;
            tmp.replace(" ", "");

            var_info.var = tmp.section(":=", 1, 1).replace(";", "");
            QString name_tmp = tmp.section(":=", 0, 0);
            QString var_name;
            if(name_tmp.indexOf("{") != -1)
            {
                int a = name_tmp.indexOf("{");
                int b = name_tmp.indexOf("}");

                var_info.dim = name_tmp.mid(a + 1, b - a - 1);

                var_name = name_tmp.mid(0, a);
            }
            else //维数不存在
            {
                var_name = tmp.section(":=", 0, 0);
                var_name.replace(";", "");
            }
            var_info.init_var = var_info.var;

            if(join) //为真时才将分析出来的变量保存进变量类中
            {
                var_info.var_name = var_name;
                bool isadd = VarInfo::instance()->AddVar(var_name, var_info);

                if(isadd && var_info.data_type == "tooldata")
                    coord->append(qMakePair(var_name.toStdString(), true));
                else if(isadd && var_info.data_type == "wobjdata")
                    coord->append(qMakePair(var_name.toStdString(), false));
            }

            mod_info.mod_code.append(text);
        }
        else if(head == "PROC" || text.indexOf("LOCAL PROC") != -1 || head == "FUNC" || text.indexOf("LOCAL FUNC") != -1 || head == "TRAP" || text.indexOf("LOCAL TRAP") != -1) //例行程序
        {
            QString rout_name;
            RoutineData rout_info;
            QString end_rout;

            if(head == "PROC") //普通例行程序
            {
                QString tmp = text.mid(5);
                rout_name = tmp.mid(0, tmp.indexOf("("));
                rout_info.type = RoutineData::PROC;
                int a = tmp.indexOf("(");
                int b = tmp.indexOf(")");
                rout_info.param = tmp.mid(a + 1, b - a - 1);
                rout_info.local = false;

                end_rout = "ENDPROC";
            }
            else if(text.indexOf("LOCAL PROC") != -1) //本地化普通例行程序
            {
                QString tmp = text.mid(11);
                rout_name = tmp.mid(0, tmp.indexOf("("));
                rout_info.type = RoutineData::PROC;
                int a = tmp.indexOf("(");
                int b = tmp.indexOf(")");
                rout_info.param = tmp.mid(a + 1, b - a - 1);
                rout_info.local = true;

                end_rout = "ENDPROC";
            }
            else if(head == "FUNC") //功能例行程序
            {
                QString tmp = text.mid(5);
                rout_info.data_type = tmp.section(" ", 0, 0);
                tmp = tmp.mid(rout_info.data_type.size() + 1);
                rout_name = tmp.mid(0, tmp.indexOf("("));
                rout_info.type = RoutineData::FUNC;
                int a = tmp.indexOf("(");
                int b = tmp.indexOf(")");
                rout_info.param = tmp.mid(a + 1, b - a - 1);
                rout_info.local = false;

                end_rout = "ENDFUNC";
            }
            else if(text.indexOf("LOCAL FUNC") != -1) //本地化功能例行程序
            {
                QString tmp = text.mid(11);
                rout_info.data_type = tmp.section(" ", 0, 0);
                tmp = tmp.mid(rout_info.data_type.size() + 1);
                rout_name = tmp.mid(0, tmp.indexOf("("));
                rout_info.type = RoutineData::FUNC;
                int a = tmp.indexOf("(");
                int b = tmp.indexOf(")");
                rout_info.param = tmp.mid(a + 1, b - a - 1);
                rout_info.local = true;

                end_rout = "ENDFUNC";
            }
            else if(head == "TRAP") //中断程序
            {
                rout_name = text.mid(5);
                rout_info.type = RoutineData::TRAP;
                rout_info.local = false;

                end_rout = "ENDTRAP";
            }
            else //本地化中断程序
            {
                rout_name = text.mid(11);
                rout_info.type = RoutineData::TRAP;
                rout_info.local = true;

                end_rout = "ENDTRAP";
            }
            QString rout_head = "Routine";
            if(rout_name.mid(0, rout_head.size()) == rout_head)
            {
                auto rout_name_num = rout_name.mid(rout_head.size());
                bool isint = false;
                int rout_num = rout_name_num.toInt(&isint);
                if(isint)
                {
                    if(rout_num > rou_name_number_)
                        rou_name_number_ = rout_num;
                }
            }

            i++; //跳过程序名这一行

            for(; i < data_list.size(); i++)
            {
                QString tmp_text = data_list.at(i);
                QString tmp_head = tmp_text.section(" ", 0, 0);

                if(!tmp_text.isEmpty())
                {
                    bool iscon = false;
                    while(tmp_text.at(0) == ' ')
                    {
                        tmp_text = tmp_text.mid(1);
                        if(tmp_text.isEmpty())
                        {
                            iscon = true;
                            break;
                        }
                    }
                    if(iscon)
                    {
                        i++;
                        continue;
                    }
                }

                if(tmp_text == end_rout) //例行程序结束
                {
                    rout_size_++;
                    mod_info.routine_data.insert(rout_name, rout_info);
                    break;
                }
                else if(tmp_head == "VAR" || tmp_head == "CONST") //变量
                {
                    VarDataInfo var_info("");
                    QString tmp;

                    var_info.data_type = tmp_text.section(" ", 1, 1);
                    if(tmp_head == "VAR")
                    {
                        var_info.nature = 0;
                        tmp = tmp_text.mid(var_info.data_type.size() + 5);
                    }
                    else
                    {
                        var_info.nature = 2;
                        tmp = tmp_text.mid(var_info.data_type.size() + 7);
                    }
                    var_info.genre = true;
                    var_info.action_scope = rout_name;
                    var_info.mod = mod_name;

                    tmp.replace(" ", "");
                    var_info.var = tmp.section(":=", 1, 1).replace(";", "");
                    QString name_tmp = tmp.section(":=", 0, 0);
                    QString var_name;
                    if(name_tmp.indexOf("{") != -1)
                    {
                        int a = name_tmp.indexOf("{");
                        int b = name_tmp.indexOf("}");

                        var_info.dim = name_tmp.mid(a + 1, b - a - 1);

                        var_name = name_tmp.mid(0, a);
                    }
                    else //维数不存在
                    {
                        var_name = tmp.section(":=", 0, 0);
                        var_name.replace(";", "");
                    }
                    var_info.init_var = var_info.var;

                    var_info.var_name = var_name;
                    VarInfo::instance()->AddVar(var_name, var_info);

                    rout_info.code.append(tmp_text);
                }
                else if(tmp_text == "UNDO") //撤销处理
                {
                    rout_info.revoke = true;

                    rout_info.code.append(tmp_text);
                }
                else if(tmp_text == "ERROR") //错误处理
                {
                    rout_info.error = true;

                    rout_info.code.append(tmp_text);
                }
                else if(tmp_text == "BACKWARD") //向后处理
                {
                    if(rout_info.type == RoutineData::PROC) //只有普通例行程序才有
                    {
                        rout_info.back = true;

                        rout_info.code.append(tmp_text);
                    }
                }
                else //其他普通语句
                {
                    rout_info.code.append(tmp_text);
                }
            }
        }

        i++;
    }
    mod_data_.insert(mod_name, mod_info);

    if(iscoord && !coord->isEmpty())
    {
        auto frame = rclib::RobotClient::initance()->getRobotFrame();
        auto tool_data = frame.getToolFrameList();
        auto work_data = frame.getWorkFrameList();
        auto ismodif = false; //是否修改

        for(auto var : *coord)
        {
            auto var_info = VarInfo::instance()->GetUcsVarInfo(var.first.c_str());
            QList<double> xyz;
            Quaternion qn;
            Euler er;

            if(var.second)
            {
                auto too = Convert::DataTypeToVarFormData("tooldata", var_info.var.toString());
                xyz = Convert::XYZCodeToDoubles(too->subnode.at(1)->subnode.at(0)->code);
                qn.SetData(too->subnode.at(1)->subnode.at(1)->code);
                er = Convert::QuaternionToEuler(qn);
                rclib::Terminal tel(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

                if(tool_data.find(var.first) == tool_data.end()) //不存在
                {
                    frame.addToolFrame(var.first, tel);
                    ismodif = true;
                }
                else
                {
                    if(tel != tool_data[var.first]) //值不同
                    {
                        frame.modifyToolFrame(var.first, tel);
                        ismodif = true;
                    }
                }
            }
            else
            {
                auto too = Convert::DataTypeToVarFormData("wobjdata", var_info.var.toString());
                xyz = Convert::XYZCodeToDoubles(too->subnode.at(3)->subnode.at(0)->code);
                qn.SetData(too->subnode.at(3)->subnode.at(1)->code);
                er = Convert::QuaternionToEuler(qn);
                rclib::Terminal tel(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

                if(work_data.find(var.first) == work_data.end()) //不存在
                {
                    frame.addWorkFrame(var.first, tel);
                    ismodif = true;
                }
                else
                {
                    if(tel != work_data[var.first]) //值不同
                    {
                        frame.addWorkFrame(var.first, tel);
                        ismodif = true;
                    }
                }
            }
        }
        if(ismodif)
        {
            rclib::RobotClient::initance()->setRobotFrame(frame);
        }
    }

    TeachState::instance()->SetCodeState();

    return true;
}

QString EditData::ExportMod(QString mod_name, bool isshow, bool issave)
{
    if(!IsExist(mod_name))
        return QString();
    ModuleData &mod_info = mod_data_[mod_name];
    QString text_data;

    if(isshow) //全部显示
    {
        if(mod_info.type)
            text_data = "MODULE " + mod_name + " (" + mod_info.param + ")\n";
        else
            text_data = "MODULE " + mod_name + "\n";

        for(auto code : mod_info.mod_code)
        {
            if(code == "\n")
                continue;
            else
                text_data.append("\t" + code + "\n");
        }
        if(mod_info.routine_data.contains("main")) //主函数永远位于该模块的第一个函数
        {
            auto &main = mod_info.routine_data["main"];

            QString str = "\tPROC main()\n";
            QString end = "\tENDPROC\n";

            text_data.append(str);

            int num = 2;
            int ueb_num = 2;

            for(auto code : main.code)
            {
                QString head = code.section(" ", 0, 0);
                QString t;
                int temp_num;

                if(head == "FOR" || head == "IF" || head == "WHILE" || head == "TEST")
                {
                    temp_num = num++;
                }
                else if(head == "ENDFOR" || head == "ENDIF" || head == "ENDWHILE" || head == "ENDTEST")
                {
                    temp_num = --num;
                }
                else if(head == "UNDO" || head == "ERROR" || head == "BACKWARD")
                {
                    num = ueb_num;
                    temp_num = num++;
                }
                else
                {
                    temp_num = num;
                }

                if(!issave) //不需要保存，也就是说只需要显示在编辑框中时需要将移动指令中的变量值全部替换成*号
                {
                    if(head == "MoveJ" && code.at(6) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*,");
                    }
                    else if(head == "MoveAbsJ" && code.at(9) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*\\NoEoffs,");
                    }
                    else if(head == "MoveC")
                    {
                        auto var1 = code.section(" ", 1, 1);
                        auto var2 = code.section(" ", 2, 2);

                        if(var1.at(0) == '[')
                            code.replace(var1, "*,");
                        if(var2.at(0) == '[')
                            code.replace(var2, "*,");
                    }
                    else if(head == "MoveL")
                    {
                        auto var = code.section(" ", 1, 1);

                        if(var.at(0) == '[')
                            code.replace(var, "*,");
                    }
                }

                if(head == "ELSEIF" || head == "ELSE" || head == "CASE" || head == "DEFAULT:")
                    temp_num--;

                for(int i = 0; i < temp_num; i++)
                    t.append("\t");

                if(code.isEmpty())
                    text_data.append("\n");
                else
                    text_data.append(t + code + "\n");
            }

            text_data.append(end);
        }

        for(auto it = mod_info.routine_data.constBegin(); it != mod_info.routine_data.constEnd(); it++)
        {
            QString str;
            QString end;

            if(it.key() == "main") //跳过主函数
            {
                continue;
            }

            if(it->local)
                str.append("LOCAL ");

            if(it->type == RoutineData::PROC)
            {
                if(!it->param.isEmpty())
                    str.append("PROC " + it.key() + "(" + it->param + ")\n");
                else
                    str.append("PROC " + it.key() + "()\n");

                end = "\tENDPROC\n";
            }
            else if(it->type == RoutineData::FUNC)
            {
                if(!it->param.isEmpty())
                    str.append("FUNC " + it->data_type + " " + it.key() + "(" + it->param + ")\n");
                else
                    str.append("FUNC " + it->data_type + " " + it.key() + "()\n");

                end = "\tENDFUNC\n";
            }
            else
            {
                str.append("TRAP " + it.key() + "\n");

                end = "\tENDTRAP\n";
            }

            str = "\t" + str;

            text_data.append(str);

            int num = 2; //添加制表符的数量
            int ueb_num = 2;

            for(auto code : it->code)
            {
                QString head = code.section(" ", 0, 0);
                QString t;
                int temp_num;

                if(head == "FOR" || head == "IF" || head == "WHILE" || head == "TEST")
                {
                    temp_num = num++;
                }
                else if(head == "ENDFOR" || head == "ENDIF" || head == "ENDWHILE" || head == "ENDTEST")
                {
                    temp_num = --num;
                }
                else if(head == "UNDO" || head == "ERROR" || head == "BACKWARD")
                {
                    num = ueb_num;
                    temp_num = num++;
                }
                else
                {
                    temp_num = num;
                }

                if(!issave) //不需要保存，也就是说只需要显示在编辑框中时需要将移动指令中的变量值全部替换成*号
                {
                    if(head == "MoveJ" && code.at(6) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*,");
                    }
                    else if(head == "MoveAbsJ" && code.at(9) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*\\NoEoffs,");
                    }
                    else if(head == "MoveC")
                    {
                        auto var1 = code.section(" ", 1, 1);
                        auto var2 = code.section(" ", 2, 2);

                        if(var1.at(0) == '[')
                            code.replace(var1, "*,");
                        if(var2.at(0) == '[')
                            code.replace(var2, "*,");
                    }
                    else if(head == "MoveL")
                    {
                        auto var = code.section(" ", 1, 1);

                        if(var.at(0) == '[')
                            code.replace(var, "*,");
                    }
                }

                if(head == "ELSEIF" || head == "ELSE" || head == "CASE" || head == "DEFAULT:")
                    temp_num--;

                for(int i = 0; i < temp_num; i++)
                    t.append("\t");

                if(code.isEmpty())
                    text_data.append("\n");
                else
                    text_data.append(t + code + "\n");
            }

            text_data.append(end);
        }

        text_data.append("ENDMODULE\n");
    }
    else //只显示函数部分
    {
        if(mod_info.routine_data.contains("main")) //主函数永远位于该模块的第一个函数
        {
            auto &main = mod_info.routine_data["main"];

            QString str = "\tPROC main()\n";
            QString end = "\tENDPROC\n";

            text_data.append(str);

            int num = 2;
            int ueb_num = 2;

            for(auto code : main.code)
            {
                QString head = code.section(" ", 0, 0);
                QString t;
                int temp_num;

                if(head == "FOR" || head == "IF" || head == "WHILE" || head == "TEST")
                {
                    temp_num = num++;
                }
                else if(head == "ENDFOR" || head == "ENDIF" || head == "ENDWHILE" || head == "ENDTEST")
                {
                    temp_num = --num;
                }
                else if(head == "UNDO" || head == "ERROR" || head == "BACKWARD")
                {
                    num = ueb_num;
                    temp_num = num++;
                }
                else
                {
                    temp_num = num;
                }

                if(!issave) //不需要保存，也就是说只需要显示在编辑框中时需要将移动指令中的变量值全部替换成*号
                {
                    if(head == "MoveJ" && code.at(6) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*,");
                    }
                    else if(head == "MoveAbsJ" && code.at(9) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*\\NoEoffs,");
                    }
                    else if(head == "MoveC")
                    {
                        auto var1 = code.section(" ", 1, 1);
                        auto var2 = code.section(" ", 2, 2);

                        if(var1.at(0) == '[')
                            code.replace(var1, "*,");
                        if(var2.at(0) == '[')
                            code.replace(var2, "*,");
                    }
                    else if(head == "MoveL")
                    {
                        auto var = code.section(" ", 1, 1);

                        if(var.at(0) == '[')
                            code.replace(var, "*,");
                    }
                }

                if(head == "ELSEIF" || head == "ELSE" || head == "CASE" || head == "DEFAULT:")
                    temp_num--;

                for(int i = 0; i < temp_num; i++)
                    t.append("\t");

                if(code.isEmpty())
                    text_data.append("\n");
                else
                    text_data.append(t + code + "\n");
            }

            text_data.append(end);
        }

        for(auto it = mod_info.routine_data.constBegin(); it != mod_info.routine_data.constEnd(); it++)
        {
            QString str;
            QString end;

            if(it.key() == "main") //跳过主函数
            {
                continue;
            }

            if(it->local)
                str.append("LOCAL ");

            if(it->type == RoutineData::PROC)
            {
                if(!it->param.isEmpty())
                    str.append("PROC " + it.key() + "(" + it->param + ")\n");
                else
                    str.append("PROC " + it.key() + "()\n");

                end = "\tENDPROC\n";
            }
            else if(it->type == RoutineData::FUNC)
            {
                if(!it->param.isEmpty())
                    str.append("FUNC " + it->data_type + " " + it.key() + "(" + it->param + ")\n");
                else
                    str.append("FUNC " + it->data_type + " " + it.key() + "()\n");

                end = "\tENDFUNC\n";
            }
            else
            {
                str.append("TRAP " + it.key() + "\n");

                end = "\tENDTRAP\n";
            }

            str = "\t" + str;

            text_data.append(str);

            int num = 2; //添加制表符的数量
            int ueb_num = 2;

            for(auto code : it->code)
            {
                QString head = code.section(" ", 0, 0);
                QString t;
                int temp_num;

                if(head == "FOR" || head == "IF" || head == "WHILE" || head == "TEST")
                {
                    temp_num = num++;
                }
                else if(head == "ENDFOR" || head == "ENDIF" || head == "ENDWHILE" || head == "ENDTEST")
                {
                    temp_num = --num;
                }
                else if(head == "UNDO" || head == "ERROR" || head == "BACKWARD")
                {
                    num = ueb_num;
                    temp_num = num++;
                }
                else
                {
                    temp_num = num;
                }

                if(!issave) //不需要保存，也就是说只需要显示在编辑框中时需要将移动指令中的变量值全部替换成*号
                {
                    if(head == "MoveJ" && code.at(6) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*,");
                    }
                    else if(head == "MoveAbsJ" && code.at(9) == '[')
                    {
                        auto var = code.section(" ", 1, 1);

                        code.replace(var, "*\\NoEoffs,");
                    }
                    else if(head == "MoveC")
                    {
                        auto var1 = code.section(" ", 1, 1);
                        auto var2 = code.section(" ", 2, 2);

                        if(var1.at(0) == '[')
                            code.replace(var1, "*,");
                        if(var2.at(0) == '[')
                            code.replace(var2, "*,");
                    }
                    else if(head == "MoveL")
                    {
                        auto var = code.section(" ", 1, 1);

                        if(var.at(0) == '[')
                            code.replace(var, "*,");
                    }
                }

                if(head == "ELSEIF" || head == "ELSE" || head == "CASE" || head == "DEFAULT:")
                    temp_num--;

                for(int i = 0; i < temp_num; i++)
                    t.append("\t");

                if(code.isEmpty())
                    text_data.append("\n");
                else
                    text_data.append(t + code + "\n");
            }

            text_data.append(end);
        }
    }

    return text_data;
}

bool EditData::ExportMod(QString mod_name, QString path)
{
    if(!IsExist(mod_name))
        return false;

    if(path.at(path.length() - 1) != '/')
        path.append("/");

    if(mod_data_.value(mod_name).type)
        path.append(mod_name + ".SYS");
    else
        path.append(mod_name + ".MOD");

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QString data = ExportMod(mod_name, true, true);

    file.write(data.toUtf8());

    return true;
}

void EditData::SetHotVarPoint(QSharedPointer<QMap<QString, HotEditVarInfo> > hot_var_point)
{
    hot_var_point_ = hot_var_point;
}

void EditData::GetHotEditData(std::function<void(void)> func, QSharedPointer<QMap<QString, HotEditVarInfo>> point)
{
    std::thread td(std::bind(&EditData::GetHotEditDatar, edit_data_shared_.data(), func, point));

    td.detach();
}

void EditData::SyntaxCheck(std::function<void (QSharedPointer<QMap<QString, QPair<QString, int>>>)> func)
{
    std::thread td(std::bind(&EditData::SyntaxCheckr, edit_data_shared_.data(), func));

    td.detach();
}

void EditData::GetSendCode(std::function<void (bool, QSharedPointer<QString>)> func)
{
    std::thread td(std::bind(&EditData::GetSendCoder, edit_data_shared_.data(), func));

    td.detach();
}

bool EditData::IsExist(QString mod_name)
{
    return mod_data_.contains(mod_name);
}

bool EditData::IsExist(QString mod_name, QString rout_name)
{
    if(!IsExist(mod_name))
        return false;
    return mod_data_.value(mod_name).routine_data.contains(rout_name);
}

bool EditData::RoutIsExist(QString rout_name)
{
    for(auto it = mod_data_.constBegin(); it != mod_data_.constEnd(); it++)
    {
        if(it->routine_data.contains(rout_name))
            return true;
    }

    return false;
}

bool EditData::GetModType(QString mod_name)
{
    if(!IsExist(mod_name))
        return false;
    return mod_data_.value(mod_name).type;
}

QString EditData::GetRoutType(QString rout_name)
{
    if(!RoutIsExist(rout_name))
    {
        return QString();
    }

    auto &rout_data = GetRoutData(rout_name);

    switch(rout_data.type)
    {
    case RoutineData::Type::PROC: return "Procedure";
    case RoutineData::Type::FUNC: return "Function";
    case RoutineData::Type::TRAP: return "Trap";
    }
}

QStringList EditData::GetModName()
{
    return mod_data_.keys();
}

QStringList EditData::GetModName(bool type)
{
    QStringList list;

    for(auto it = mod_data_.constBegin(); it != mod_data_.constEnd(); it++)
    {
        if(it->type == type)
            list.append(it.key());
    }

    return list;
}

ModuleData &EditData::GetModData(QString mod_name)
{
    if(!IsExist(mod_name))
    {
        throw ABBException("ModuleData", "GetModData", mod_name + "模块不存在");
    }
    return mod_data_[mod_name];
}

QString EditData::GetRoutMod(QString rout_name)
{
    for(auto it = mod_data_.constBegin(); it != mod_data_.constEnd(); it++)
    {
        if(it->routine_data.contains(rout_name))
            return it.key();
    }

    return QString();
}

RoutineData &EditData::GetRoutData(QString rout_name, QString mod_name)
{
    if(!mod_name.isEmpty())
    {
        if(!IsExist(mod_name))
        {
            throw ABBException("ModuleData", "GetRoutData", mod_name + "模块不存在", 1);
        }

        if(!mod_data_.value(mod_name).routine_data.contains(rout_name))
        {
            throw ABBException("ModuleData", "GetRoutData", rout_name + "例行程序不存在", 2);
        }

        return mod_data_[mod_name].routine_data[rout_name];
    }
    else
    {
        for(auto it = mod_data_.constBegin(); it != mod_data_.constEnd(); it++)
        {
            if(it->routine_data.contains(rout_name))
            {
                return mod_data_[it.key()].routine_data[rout_name];
            }
        }

       throw ABBException("ModuleData", "GetRoutData", "数据不存在", 3);
    }
}

void EditData::DelMod()
{
    if(task_name_ != "<No Program>")
        SaveMod();

    mod_data_.clear();
    TeachState::instance()->SetCodeState();
    rout_size_ = 0;
    emit ClearMod();
}

void EditData::DelTask(bool is_del_local)
{
    if(task_name_ != "<No Program>" && !is_del_local)
        SaveMod();

    mod_data_.clear();
    TeachState::instance()->SetCodeState();
    rout_size_ = 0;
    emit ClearMod();

    QString path = qApp->applicationDirPath() + "/data/RAPID/Modular/";
    if(is_del_local) //如果需要删除已保存文件
    {
        QString dir_path = path + task_path_ + "/";

        for(int i = 0; i < 2; i++)
        {
            QString temp_path = dir_path;
            if(i == 0)
                temp_path.append("System/");
            else
                temp_path.append("User/");

            QDir dir(temp_path);
            dir.setFilter(QDir::Files);
            int count = static_cast<int>(dir.count());
            for(int i = 0; i < count; i++)
                dir.remove(dir[i]);

            dir.rmdir(temp_path);
        }

        QDir dir(path + task_path_);
        dir.rmdir(dir_path);
    }
    QString dir_path = path + "TempMod/";

    for(int i = 0; i < 2; i++)
    {
        QString temp_path = dir_path;
        if(i == 0)
            temp_path.append("System/");
        else
            temp_path.append("User/");

        QDir dir(temp_path);
        dir.setFilter(QDir::Files);
        int count = static_cast<int>(dir.count());
        for(int i = 0; i < count; i++)
            dir.remove(dir[i]);

        dir.rmdir(temp_path);
    }

    order_name_ = "<No Program>";
    type_name_ = "Normal";
    task_path_ = "TempMod";

    QDir dir(path + task_path_);
    dir.rmdir(dir_path);

    dir.mkpath(path + "TempMod/System/");
    dir.mkpath(path + "TempMod/User/");

    QFile::copy(":/ModSystem/RAPID/Modular/System/BASE.SYS", path + "TempMod/System/BASE.SYS");
    QFile::copy(":/ModSystem/RAPID/Modular/System/user.SYS", path + "TempMod/System/user.SYS");

    UpdateTask();
}

void EditData::DelMod(QString mod_name, bool del_var)
{
    mod_data_.remove(mod_name);

    if(del_var)
        emit ModDel(mod_name);
    TeachState::instance()->SetCodeState();
}

void EditData::DelMod(bool type)
{
    for(auto it = mod_data_.begin(); it != mod_data_.end();)
    {
        if(it->type == type)
        {
            emit ModDel(it.key());
            it = mod_data_.erase(it);
        }
        else
        {
            it++;
        }
    }

    TeachState::instance()->SetCodeState();
}

bool EditData::DelRout(QString rout_name, QString mod_name)
{
    if(!IsExist(mod_name, rout_name))
        return false;

    mod_data_[mod_name].routine_data.remove(rout_name);
    rout_size_--;
    TeachState::instance()->SetCodeState(true);

    return true;
}

bool EditData::RemMod(QString old_name, QString new_name)
{
    if(!IsExist(old_name))
        return false;
    if(IsExist(new_name))
        return false;

    ModuleData d = mod_data_[old_name];

    mod_data_.remove(old_name);
    mod_data_.insert(new_name, d);

    emit ModRem(old_name, new_name);

    TeachState::instance()->SetCodeState(true);

    auto path = qApp->applicationDirPath() + "/data/RAPID/Modular/" + task_path_ + "/";
    if(d.type)
        path.append("System/" + old_name + ".SYS");
    else
        path.append("User/" + old_name + ".MOD");
    QFile::remove(path); //删除存放于本地的旧模块，防止新模块保存后下一次启动时旧模块任然存在

    return true;
}

bool EditData::RemRout(QString old_name, QString mod_name, QString new_name)
{
    if(!IsExist(mod_name, old_name))
        return false;
    if(IsExist(mod_name, new_name))
        return false;

    RoutineData rout_data = mod_data_[mod_name].routine_data[old_name];

    mod_data_[mod_name].routine_data.remove(old_name);

    mod_data_[mod_name].routine_data.insert(new_name, rout_data);

    emit RoutRename(old_name, new_name);

    TeachState::instance()->SetCodeState();

    return true;
}

void EditData::ModDataMid(QString mod_name)
{
    TeachState::instance()->SetCodeState();

    emit DataMid(mod_name);
}

QStringList EditData::GetRoutName(QString mod_name)
{
    if(!IsExist(mod_name))
        return QStringList();

    return mod_data_.value(mod_name).routine_data.keys();
}

QStringList EditData::GetRoutName()
{
    QStringList list;

    for(auto it = mod_data_.begin(); it != mod_data_.end(); it++)
    {
        list.append(it->routine_data.keys());
    }

    rout_size_ = list.size();
    return list;
}

QStringList EditData::GetRoutName(RoutineData::Type type)
{
    QStringList list;

    int rout_size = 0;
    for(auto it = mod_data_.begin(); it != mod_data_.end(); it++)
    {
        for(auto rit = it->routine_data.begin(); rit != it->routine_data.end(); rit++)
        {
            rout_size++;
            if(rit->type == type)
                list.append(rit.key());
        }
    }

    if(rout_size != rout_size_)
        rout_size_ = rout_size;

    return list;
}

QString EditData::GetRoutCodeTree(QString rout_name)
{
    rout_name = rout_name.replace(" ", "");
    if(!RoutIsExist(rout_name))
        return QString();

    RoutineData &rout_data = this->GetRoutData(rout_name);
    QString str;

    if(rout_data.local)
        str.append("LOCAL ");

    if(rout_data.type == RoutineData::PROC)
    {
        if(!rout_data.param.isEmpty())
            str.append("PROC " + rout_name + "(" + rout_data.param + ")");
        else
            str.append("PROC " + rout_name + "()");
    }
    else if(rout_data.type == RoutineData::FUNC)
    {
        if(!rout_data.param.isEmpty())
            str.append("FUNC " + rout_data.data_type + " " + rout_name + "(" + rout_data.param + ")");
        else
            str.append("FUNC " + rout_data.data_type + " " + rout_name + "()");
    }
    else
    {
        str.append("TRAP " + rout_name);
    }

    return str;
}

bool EditData::ModAddCode(QString mod_name, QString code)
{
    if(mod_name.isEmpty())
        return false;
    if(!IsExist(mod_name))
        return false;

    auto list = code.split("\n");
    if(list.size() > 1)
    {
        auto &mod = mod_data_[mod_name];

        for(auto &str : list)
        {
            mod.mod_code.append(str);
        }
    }
    else
    {
        mod_data_[mod_name].mod_code.append(code);
    }

    TeachState::instance()->SetCodeState();
    return true;
}

bool EditData::RoutAddCode(QString rout_name, int row, QString code)
{
    if(!RoutIsExist(rout_name))
        return false;

    auto &list = this->GetRoutData(rout_name).code;

    auto code_list = code.split("\n");
    int deviation = 0;
    if(code_list.size() > 1)
    {
        for(int i = 0; i < code_list.size(); i++)
        {
            auto &str = code_list[i];
            str.replace("\t", "").replace("\n", "");

            if(i == 0 && row < list.size() && list[row] == "<SMT>")
            {
                list[row] = str;
                deviation = 1;
            }
            else
            {
                if(row - 1 >= 0)
                {
                    if(list[row - 1] == "<SMT>" && i == 0)
                    {
                        list[row - 1] = str;
                        deviation = -1;
                    }
                    else
                    {
                        list.insert(row + i + deviation, str);
                    }
                }
                else
                {
                    list.insert(row + i + deviation, str);
                }
            }
        }
    }
    else
    {
        code.replace("\t", "").replace("\n", "");

        if(row < list.size() && list[row] == "<SMT>")
        {
            list[row] = code;
        }
        else if(row - 1 >= 0)
        {
            if(list[row - 1] == "<SMT>")
            {
                list[row - 1] = code;
            }
            else
            {
                list.insert(row, code);
            }
        }
        else
        {
            list.insert(row, code);
        }
    }

    TeachState::instance()->SetCodeState();

    return true;
}

bool EditData::RoutRemCode(QString rout_name, int row, QString code)
{
    if(!RoutIsExist(rout_name))
        return false;

    auto &list = this->GetRoutData(rout_name).code;
    if(row < 0 || row >= list.size())
        return false;

    auto code_list = code.split("\n");
    if(code_list.size() > 1)
    {
        for(int i = 0; i < code_list.size(); i++)
        {
            auto &str = code_list[i];
            str.replace("\t", "").replace("\n", "");

            list[row + i] = str;
        }
    }
    else
    {
        code.replace("\t", "").replace("\n", "");

        list[row] = code;
    }

    TeachState::instance()->SetCodeState();

    return true;
}

bool EditData::RoutRemCode(QString rout_name, int row, int number, QString code)
{
    if(!RoutIsExist(rout_name))
        return false;

    auto &list = this->GetRoutData(rout_name).code;
    if(row < 0 || number < 0)
        return false;

    auto code_list = code.split("\n");
    int code_size = code_list.size();

    if(code_size == number)
    {
        for(int i = 0; i < number; i++)
        {
            auto &str = code_list[i];
            str.replace("\t", "").replace("\n", "");

            list[row + i] = str;
        }
    }
    else if(code_size > number)
    {
        for(int i = 0; i < number; i++)
        {
            auto &str = code_list[i];
            str.replace("\t", "").replace("\n", "");

            list[row + i] = str;
        }

        for(int i = number; i < code_size; i++)
        {
            auto &str = code_list[i];
            str.replace("\t", "").replace("\n", "");

            list.insert(row + i, str);
        }
    }
    else if(code_size < number)
    {
        for(int i = 0; i < code_size; i++)
        {
            auto &str = code_list[i];
            str.replace("\t", "").replace("\n", "");
            list[row + i] = str;

        }

        for(int i = code_size; i < number; i++)
        {
            list.removeAt(row + code_size);
        }

        if(list.isEmpty())
        {
            list.append("<SMT>");
        }
    }

    TeachState::instance()->SetCodeState();

    return true;
}

bool EditData::RoutDelCode(QString rout_name, int row, int number)
{
    if(!RoutIsExist(rout_name))
        return false;

    auto &list = this->GetRoutData(rout_name).code;
    if(row >= list.size())
        return false;
    if(row < 0)
        row = 0;

    if(number == 0)
    {
        list.removeAt(row);
    }
    else
    {
        for(int i = 0; i < number; i++)
        {
            list.removeAt(row);
        }
    }

    if(list.isEmpty())
    {
        list.append("<SMT>");
    }

    TeachState::instance()->SetCodeState();

    return true;
}

QString EditData::ExportRout(RoutineData &rout_info, QString rout_name)
{
    QString str = "\t";
    QString end;

    if(rout_info.local)
        str.append("LOCAL ");
    if(rout_info.type == RoutineData::PROC)
    {
        if(!rout_info.param.isEmpty())
            str.append("PROC " + rout_name + "(" + rout_info.param + ")\n");
        else
            str.append("PROC " + rout_name + "()\n");

        end = "\tENDPROC\n";
    }
    else if(rout_info.type == RoutineData::FUNC)
    {
        if(!rout_info.param.isEmpty())
            str.append("FUNC " + rout_info.data_type + " " + rout_name + "(" + rout_info.param + ")\n");
        else
            str.append("FUNC " + rout_info.data_type + " " + rout_name + "()\n");
        end = "\tENDFUNC\n";
    }
    else
    {
        str.append("TRAP " + rout_name + "\n");
        end = "\tENDTRAP\n";
    }

    int num = 2;
    int ueb_num = 2;
    for(auto code : rout_info.code)
    {
        QString head = code.section(" ", 0, 0);
        QString t;
        int temp_num;

        if(head == "FOR" || head == "IF")
        {
            temp_num = num++;
        }
        else if(head == "ENDFOR" || head == "ENDIF")
        {
            temp_num = --num;
        }
        else if(head == "UNDO" || head == "ERROR" || head == "BACKWARD")
        {
            num = ueb_num;
            temp_num = num++;
        }
        else
        {
            temp_num = num;
        }

        for(int i = 0; i < temp_num; i++)
            t.append("\t");

        str.append(t + code + "\n");
    }

    str.append(end);

    return str;
}

bool EditData::ModAddRout(RoutineData &rout_data, QString rout_name, QString mod_name)
{
    if(!IsExist(mod_name))
        return false;
    if(IsExist(mod_name, rout_name))
        return false;

    mod_data_[mod_name].routine_data.insert(rout_name, rout_data);
    rout_size_++;
    TeachState::instance()->SetCodeState();

    QString head = "Routine";
    if(rout_name.mid(0, head.size()) == head)
    {
        bool isint = false;
        int num = rout_name.mid(head.size()).toInt(&isint);
        if(isint)
        {
            if(num > rou_name_number_)
                rou_name_number_ = num;
        }
    }

    return true;
}

bool EditData::MoveRout(QString old_name, QString rout_name, QString new_name)
{
    if(!IsExist(old_name, rout_name))
        return false;
    if(!IsExist(new_name))
        return false;

    RoutineData rou_data = mod_data_[old_name].routine_data[rout_name];

    mod_data_[old_name].routine_data.remove(rout_name);
    mod_data_[new_name].routine_data.insert(rout_name, rou_data);

    emit RoutMove(rout_name, new_name);

    TeachState::instance()->SetCodeState();

    return true;
}

QPair<QString, int> EditData::GetRoutPoint(int rout, int number)
{
    if(!rout_map_.contains(rout))
        return qMakePair(QString("main"), -1);
    auto &rout_map = rout_map_[rout]; 
    if(!rout_map.row_map.contains(number))
        return qMakePair(rout_map.rout_name, -1);

    return qMakePair(rout_map.rout_name, rout_map.row_map.value(number));
}

QPair<int, int> EditData::GetRoutPoint(QString rout_name, int number)
{
    for(auto it = rout_map_.begin(); it != rout_map_.end(); ++it)
    {
        if(it->rout_name == rout_name)
        {
            for(auto ite = it->row_map.begin(); ite != it->row_map.end(); ++ite)
            {
                if(ite.value() == number)
                {
                    return qMakePair(it.key(), ite.key());
                }
            }
        }
    }

    return qMakePair(-1, -1);
}

int EditData::GetRoutMainPoint()
{
    return main_number_;
}

QString EditData::GetUnusedRout()
{
    QString head = "Routine" + QString::number(rou_name_number_ + 1);

    return head;
}

QString EditData::GetUnusedMod()
{
    QString head = "Module" + QString::number(mod_name_number_ + 1);

    return head;
}

void EditData::UpdateTask()
{
    QString path = qApp->applicationDirPath() + "/data/RAPID/Task/" + task_name_ + ".ini";

    QSettings st(path, QSettings::IniFormat);
    st.setValue("info/name", order_name_);
    st.setValue("/info/type", type_name_);
    st.setValue("/info/path", task_path_);
}

void EditData::GetHotEditDatar(std::function<void(void)> func, QSharedPointer<QMap<QString, HotEditVarInfo> > point)
{
    auto pt = QSharedPointer<QMap<QString, HotEditVarInfo>>(new QMap<QString, HotEditVarInfo>);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    CodeAnalysis(nullptr, nullptr, pt);
#else
    CodeAnalysis(QSharedPointer<QMap<QString,QPair<QString,int>>>(), QSharedPointer<QString>(), pt);
#endif

    for(auto it = point->begin(); it != point->end();)
    {
        if(!pt->contains(it.key()))
            it = point->erase(it);
        else
            ++it;
    }

    for(auto it = pt->begin(); it != pt->end(); ++it)
    {
        if(!point->contains(it.key()))
            point->insert(it.key(), it.value());
    }

    func();
}

void EditData::SyntaxCheckr(std::function<void (QSharedPointer<QMap<QString,QPair<QString,int>>>)> func)
{
    auto errors = QSharedPointer<QMap<QString,QPair<QString,int>>>(new QMap<QString, QPair<QString, int>>);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    CodeAnalysis(errors, nullptr, nullptr);
#else
    CodeAnalysis(errors, QSharedPointer<QString>(), QSharedPointer<QMap<QString, HotEditVarInfo>>());
#endif

    func(errors);
}

void EditData::TrimHotEdit(QString &codes)
{
    if(codes.isEmpty())
        return;
    if(hot_var_point_->isEmpty())
        return;
    if(codes.indexOf("robtarget") == -1 || codes.indexOf(":=") == -1)
        return;

    auto left = codes.section(":=", 0, 0);
    auto right = codes.section(":=", 1, 1);

    int lnum = left.lastIndexOf(" ");
    auto name = left.mid(lnum + 1);
    if(!hot_var_point_->contains(name))
        return;

    auto var_tree = Convert::DataTypeToVarFormData("robtarget", right);
    auto &hot = hot_var_point_->operator[](name);

    auto var_xyz = var_tree->subnode.at(0);
    double vx = var_xyz->subnode.at(0)->code.toDouble();
    double vy = var_xyz->subnode.at(1)->code.toDouble();
    double vz = var_xyz->subnode.at(2)->code.toDouble();
    vx += hot.x;
    vy += hot.y;
    vz += hot.z;
    var_xyz->subnode.at(0)->code = QString::number(vx);
    var_xyz->subnode.at(1)->code = QString::number(vy);
    var_xyz->subnode.at(2)->code = QString::number(vz);
    var_xyz->Update();

    auto var_abcd = var_tree->subnode.at(1);
    Euler er(hot.a, hot.b, hot.c);
    auto qn = Convert::EulerToQuaternion(er);
    double va = var_abcd->subnode.at(0)->code.toDouble();
    double vb = var_abcd->subnode.at(1)->code.toDouble();
    double vc = var_abcd->subnode.at(2)->code.toDouble();
    double vd = var_abcd->subnode.at(3)->code.toDouble();
    if(qn.w != 1.0 && qn.x != 0.0 && qn.y != 0.0 && qn.z != 0.0) //空
    {
        va += qn.w;
        vb += qn.x;
        vc += qn.y;
        vd += qn.z;
    }
    var_abcd->subnode.at(0)->code = QString::number(va);
    var_abcd->subnode.at(1)->code = QString::number(vb);
    var_abcd->subnode.at(2)->code = QString::number(vc);
    var_abcd->subnode.at(3)->code = QString::number(vd);
    var_abcd->Update();

    var_tree->Update();

    codes = left + ":=" + var_tree->code;
}

void EditData::GetSendCoder(std::function<void (bool, QSharedPointer<QString>)> func)
{
    auto mod_list = mod_data_.keys(); //获取所有模块名
    rout_map_.clear(); //清除映射关系
    main_number_ = -1;

    if(mod_list.size() <= 5) //模块小于5个时此线程直接执行
    {
        auto code = QSharedPointer<QString>(new QString);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
        CodeAnalysis(nullptr, code, nullptr);
#else
        CodeAnalysis(QSharedPointer<QMap<QString,QPair<QString,int>>>(), code, QSharedPointer<QMap<QString, HotEditVarInfo>>());
#endif

        if(!code->isEmpty())
        {
            func(true, code);
        }
        else
        {

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
            func(false, nullptr);
#else
            func(false, QSharedPointer<QString>());
#endif

        }
    }
    else //模块大于5个时在创建1个线程，也就是总共2个线程进行处理
    {
        int num = mod_list.size();
        int x = num/2;

        auto mods = mod_list.mid(0, x);
        auto mods1 = mod_list.mid(x, x + x);


        auto code = QSharedPointer<QString>(new QString);
        auto code1 = QSharedPointer<QString>(new QString);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
        std::thread t1(std::bind(&EditData::CodeAnalysis, edit_data_shared_.data(), nullptr, code1, nullptr, mods1));

        CodeAnalysis(nullptr, code, nullptr, mods);
#else
        std::thread t1(std::bind(&EditData::CodeAnalysis, edit_data_shared_.data(), QSharedPointer<QMap<QString,QPair<QString,int>>>(), code1, QSharedPointer<QMap<QString, HotEditVarInfo>>(), mods1));

        CodeAnalysis(QSharedPointer<QMap<QString,QPair<QString,int>>>(), code, QSharedPointer<QMap<QString, HotEditVarInfo>>(), mods);
#endif

        t1.join();

        code->append(*code1);

        if(!code->isEmpty())
        {
            func(true, code);
        }
        else
        {

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
            func(false, nullptr);
#else
            func(false, QSharedPointer<QString>());
#endif

        }
    }
}

void EditData::CodeAnalysis(QSharedPointer<QMap<QString,QPair<QString,int>>> errors, QSharedPointer<QString> codes,
                            QSharedPointer<QMap<QString, HotEditVarInfo>> hot_var, QStringList mod_list)
{
    if(!hot_var.isNull() && !codes.isNull()) //获取代码与获取数据之间相互冲突，两者不能共存
    {
        return;
    }

    if(mod_list.isEmpty())
        mod_list = mod_data_.keys();

    if(!codes.isNull())
    {
        codes->append("!ABB\n");
    }

    for(auto mod_name : mod_list)
    {
        auto mod_info = mod_data_[mod_name];

        if(!codes.isNull())
        {
            if(mod_info.type)
                codes->append("MODULE " + mod_name + " (" + mod_info.param + ")\n");
            else
                codes->append("MODULE " + mod_name + "\n");

            for(auto code : mod_info.mod_code)
            {
                if(code.at(0) == '!') //去除注释
                {
                    continue;
                }
                else if(code == "\n") //去除空行
                {
                    continue;
                }
                else
                {
                    TrimHotEdit(code);
                    codes->append(code + "\n");
                }
            }
        }

        for(auto it = mod_info.routine_data.constBegin(); it != mod_info.routine_data.constEnd(); ++it)
        {
            QString str;
            QString end;
            RoutMap *rotp = nullptr;
            QMap<int, int> *rt_map = nullptr;

            if(!codes.isNull())
            {
                rout_mutex_.lock();
                int num = rout_map_.size();
                rout_map_.insert(num, RoutMap());
                rotp = &rout_map_[num];
                rout_mutex_.unlock();

                rt_map = &rotp->row_map;
                rotp->rout_name = it.key();
                if(it.key() == "main")
                    main_number_ = num;
                rt_map->insert(-1, -1); //纠正偏差
            }

            if(!codes.isNull())
            {
                if(it->local)
                    str.append("LOCAL ");

                if(it->type == RoutineData::PROC)
                {
                    if(!it->param.isEmpty())
                        str.append("PROC " + it.key() + "(" + it->param + ")\n");
                    else
                        str.append("PROC " + it.key() + "()\n");

                    end = "ENDPROC\n";
                }
                else if(it->type == RoutineData::FUNC)
                {
                    if(!it->param.isEmpty())
                        str.append("FUNC " + it->data_type + " " + it.key() + "(" + it->param + ")\n");
                    else
                        str.append("FUNC " + it->data_type + " " + it.key() + "()\n");

                    end = "ENDFUNC\n";
                }
                else
                {
                    str.append("TRAP " + it.key() + "\n");

                    end = "ENDTRAP\n";
                }

                codes->append(str);
            }

            QString temp_code;
            QList<int> temp_rt_list;
            for(int code_num = 0; code_num < it->code.size(); code_num++)
            {
                auto code = it->code.at(code_num);
                auto head = code.section(" ", 0, 0);

                if(code == "\n" || code.at(0) == '!') //去除注释与空行
                    continue;
                else if(head == "VAR" || head == "CONST")
                {
                    if(!codes.isNull())
                    {
                        TrimHotEdit(code);
                        codes->append(code + "\n");
                        rt_map->insert(rt_map->size(), code_num);
                    }
                }
                else if((head == "MoveJ" && code.at(6) == '[') || (head == "MoveL" && code.at(6) == '['))
                {
                    if(!codes.isNull())
                    {
                        auto var = code.section(" ", 1, 1);
                        var = var.mid(0, var.size() - 1);
                        auto var_name = VarInfo::instance()->GetUnused();

                        auto var_code = "CONST robtarget " + var_name + ":=" + var + ";\n";
                        code.replace(var, var_name);

                        codes->append(var_code);
                        rt_map->insert(rt_map->size(), -1); //自动产生的临时性变量统一为-1
                        temp_code.append(code + "\n");
                        temp_rt_list.append(code_num);
                    }
                }
                else if(head == "MoveJ" || head == "MoveL")
                {
                    if(!hot_var.isNull())
                    {
                        auto var = code.section(" ", 1, 1);
                        var = var.mid(0, var.size() - 1);

                        if(!VarInfo::instance()->IsExist(var))
                        {
                            if(!errors.isNull())
                            {
                                errors->insert(mod_name, qMakePair(it.key(), code_num));
                                continue;
                            }
                        }
                        else
                        {
                            if(!hot_var->contains(var))
                            {
                                auto var_list = VarInfo::instance()->GetVarInfo(var);
                                HotEditVarInfo hvo;

                                for(auto var_info : var_list)
                                {
                                    if(var_info.data_type != "robtarget")
                                        continue;
                                    if(var_info.action_scope == "local" && var_info.mod != mod_name)
                                        continue;
                                    if(var_info.action_scope != "task" && var_info.action_scope != "local")
                                    {
                                        if(var_info.action_scope != "all" && var_info.mod != mod_name)
                                            continue;
                                    }

                                    hvo.mod_name = var_info.mod;
                                    hvo.action_scope = var_info.action_scope;
                                    break;
                                }
                                hot_var->insert(var, hvo);
                            }
                        }
                    }
                    else if(!codes.isNull())
                    {
                        temp_code.append(code + "\n");
                        temp_rt_list.append(code_num);
                    }
                }
                else if(head == "MoveC")
                {
                    auto var1 = code.section(" ", 1, 1);
                    auto var2 = code.section(" ", 2, 2);
                    var1 = var1.mid(0, var1.size() - 1);
                    var2 = var2.mid(0, var2.size() - 1);

                    if(var1.at(0) == '[')
                    {
                        if(!codes.isNull())
                        {
                            auto var_name = VarInfo::instance()->GetUnused();

                            auto var_code = "CONST robtarget " + var_name + ":=" + var1 + ";\n";
                            code.replace(var1, var_name);

                            codes->append(var_code);
                            rt_map->insert(rt_map->size(), -1);
                        }
                    }
                    else
                    {
                        if(!hot_var.isNull())
                        {
                            if(!VarInfo::instance()->IsExist(var1))
                            {
                                if(!errors.isNull())
                                {
                                    errors->insert(mod_name, qMakePair(it.key(), code_num));
                                    continue;
                                }
                            }
                            else {

                                if(!hot_var->contains(var1))
                                {
                                    auto var_list = VarInfo::instance()->GetVarInfo(var1);
                                    HotEditVarInfo hvo;

                                    for(auto var_info : var_list)
                                    {
                                        if(var_info.data_type != "robtarget")
                                            continue;
                                        if(var_info.action_scope == "local" && var_info.mod != mod_name)
                                            continue;
                                        if(var_info.action_scope != "task" && var_info.action_scope != "local")
                                        {
                                            if(var_info.action_scope != "all" && var_info.mod != mod_name)
                                                continue;
                                        }

                                        hvo.mod_name = var_info.mod;
                                        hvo.action_scope = var_info.action_scope;
                                        break;
                                    }
                                    hot_var->insert(var1, hvo);
                                }
                            }
                        }
                    }

                    if(var2.at(0) == '[')
                    {
                        if(!codes.isNull())
                        {
                            auto var_name = VarInfo::instance()->GetUnused();

                            auto var_code = "CONST robtarget " + var_name + ":=" + var2 + ";\n";
                            code.replace(var2, var_name);

                            codes->append(var_code);
                            rt_map->insert(rt_map->size(), -1);
                        }
                    }
                    else
                    {
                        if(!hot_var.isNull())
                        {
                            if(!VarInfo::instance()->IsExist(var2))
                            {
                                if(!errors.isNull())
                                {
                                    errors->insert(mod_name, qMakePair(it.key(), code_num));
                                    continue;
                                }
                            }
                            else
                            {
                                if(!hot_var->contains(var2))
                                {
                                    auto var_list = VarInfo::instance()->GetVarInfo(var2);
                                    HotEditVarInfo hvo;

                                    for(auto var_info : var_list)
                                    {
                                        if(var_info.data_type != "robtarget")
                                            continue;
                                        if(var_info.action_scope == "local" && var_info.mod != mod_name)
                                            continue;
                                        if(var_info.action_scope != "task" && var_info.action_scope != "local")
                                        {
                                            if(var_info.action_scope != "all" && var_info.mod != mod_name)
                                                continue;
                                        }

                                        hvo.mod_name = var_info.mod;
                                        hvo.action_scope = var_info.action_scope;
                                        break;
                                    }
                                    hot_var->insert(var2, hvo);
                                }
                            }
                        }
                    }

                    if(!codes.isNull())
                    {
                        temp_code.append(code + "\n");
                        temp_rt_list.append(code_num);
                    }
                }
                else if(head == "MoveAbsJ" && code.at(9) == '[')
                {
                    if(!codes.isNull())
                    {
                        auto var = code.section(" ", 1, 1);
                        var = var.mid(0, var.size() - 9);
                        auto var_name = VarInfo::instance()->GetUnused();

                        auto var_code = "CONST jointtarget " + var_name + ":=" + var + ";\n";
                        code.replace(var, var_name);

                        codes->append(var_code);
                        rt_map->insert(rt_map->size(), -1);
                        temp_code.append(code + "\n");
                        temp_rt_list.append(code_num);
                    }
                }
                else
                {
                    if(!codes.isNull())
                    {
                        temp_code.append(code + "\n");
                        temp_rt_list.append(code_num);
                    }
                }
            }
            if(!codes.isNull())
            {
                codes->append(temp_code);
                codes->append(end);

                for(auto num : temp_rt_list)
                    rt_map->insert(rt_map->size(), num);
            }
        }
        if(!codes.isNull())
            codes->append("ENDMODULE\n");
    }
}
