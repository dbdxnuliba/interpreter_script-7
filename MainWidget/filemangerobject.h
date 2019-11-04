#ifndef FILEMANGEROBJECT_H
#define FILEMANGEROBJECT_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QWidget>
#include <QList>
#include <QLabel>

#include "listboxobject.h"
#include "comboboxobject.h"
#include "commoncontrols.h"
#include "stackedwidobject.h"

namespace robotTeachABB
{
    class FLWidget; //用作文件列表的元素存在
    class FileList; //文件列表
    class FileMangerObject; //文件资源管理器基类
}

class FLWidget : public QWidget
{
    Q_OBJECT
public:
    FLWidget(const QString &name, const QString &type, int scale_w = 1, int scale_h = 1, QWidget *parent = nullptr);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *ico_label_; //图标
    QLabel *name_label_; //文件名
    QLabel *type_label_; //类型
    double scale_w_;
    double scale_h_;
};

class FileList : public ListBoxObject
{
    Q_OBJECT
public:
    FileList(QWidget *parent = nullptr);
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

bool PositiveSequence1(const QString &str1, const QString &str2); //对文件进行正向排序
bool ReverseOrder1(const QString &str1, const QString &str2); //对文件进行倒向排序
class FileMangerObject : public QWidget
{
    Q_OBJECT
public:
    explicit FileMangerObject(QWidget *parent = nullptr, bool isinherit = false);
    void SetInherit(bool isinherit);
    void resizeEvent(QResizeEvent *);
    virtual void FindFiles(QString path); //寻找某个路径下的所有非隐藏的文件夹和文件
    void Sort(); //排序
    void AddFilter(QString str); //为文件过滤器增加过滤项
    void SetFileFilter(QStringList &filter); //设置文件过滤器
    void FileFilterClear(); //清空文件过滤器
    virtual QString CurrentText(QString text); //文件被点击以后调用，虚函数，此处只将数据返回

signals:
    void PathChange(QString path); //当前目录发生改变

public slots:
    virtual void SetScale(double scale_w, double scale_h);
    void TogglePages(QListWidgetItem *); //切换页面的双击版本
    void TogglePagesi(int row); //切换页面的单击版本
    void LastPages(); //上一页
    void Refresh(); //刷新
    void ReturnHome(); //返回主目录


protected slots:
    virtual void ShowKeyboardWidget(); //显示键盘界面
    virtual void HideKeyboardWidget(); //隐藏键盘界面
    void GetKeyboardData(QString text); //获取键盘输入

private:
    void Init();

private:
    bool isinherit_; //使用方式是否是继承方式，为真时使用的是子类所以这个父类需要将文件显示框高度缩小一部分以便子类添加下方的按钮，为假时文件显示框正常显示，默认为假
    QStringList file_filter_; //文件过滤器
    Keyboard *keyboard_create_dir_; //键盘

public:
    QPushButton *sort_button_; //排序按钮
    double scale_w_;
    double scale_h_;
    bool is_show_; //控制是否显示排序按钮
    bool pos_neg_; //控制显示的是正向排序还是反向排序
    QString root_path_; //所设定的根目录，上一页的最高层目录，默认为/
    QString path_; //当前路径
    QString home_path_; //主目录路径
    QStringList file_list_; //所有文件
    QPushButton *show_button_; //显示排序按钮
    QPushButton *hide_button_; //隐藏排序按钮
    QLabel *type_label_; //类型标签
    FileList *filelist_; //文件显示框
    QPushButton *create_button_; //新建文件夹按钮
    QPushButton *upper_button_; //上一页按钮
    QPushButton *home_button_; //主目录按钮
};

#endif // FILEMANGEROBJECT_H
