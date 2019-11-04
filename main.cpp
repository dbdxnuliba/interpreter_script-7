#include "widget.h"
#include "RobotClient/robotClient.h"
#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QTranslator>
#include <QLockFile>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto path = a.applicationDirPath() + "/data/RAPID/Language/";
    QSettings st(path + "language.ini", QSettings::IniFormat);
    auto lang_name = st.value("/language/current").toString();

    rclib::ERROR_INIF_LANGUAGE lang_type = rclib::ERROR_INIF_LANGUAGE::CHINESE;
    if(lang_name == "zh_cn")
        lang_type = rclib::ERROR_INIF_LANGUAGE::CHINESE;
    else if(lang_name == "en_gb")
        lang_type = rclib::ERROR_INIF_LANGUAGE::ENGLISH;

    QTranslator tr;
    auto lang_path = path + "qm/" + lang_name + ".qm";
    tr.load(lang_path);
    a.installTranslator(&tr);

    QString ip = "127.0.0.1";
    int port = 8080;

    if(argc == 3)
    {
        ip = argv[1];

        bool isint;
        QString str_port(argv[2]);
        port = str_port.toInt(&isint);
        if(!isint)
        {
            QMessageBox::warning(nullptr, QObject::tr("错误"), QObject::tr("所获取的ip或端口有误"));
            return -1;
        }
    }

    if(!rclib::RobotClient::initance()->initSystem(ip.toUtf8().data(), port))
    {
        QMessageBox::warning(nullptr, QObject::tr("错误"), QObject::tr("与服务端连接失败"));
        a.quit();
        return -1;
    }

    rclib::RobotClient::initance()->sendSZYY(lang_type);
    TeachState::instance()->SetLanguage(lang_name);

    Widget w;

    w.show();

    return a.exec();
}
