#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QWidget>

#include "../../filemangerobject.h"
#include "../../commoncontrols.h"
#include "../../comboboxobject.h"
#include "../../windowobject.h"

namespace robotTeachABB
{
    class FileManager; //文件资源管理器
    class NatuleWindow; //属性窗口
}

class NatuleWindow : public QWidget
{
    Q_OBJECT
public:
    NatuleWindow(QWidget *parent = nullptr);
    void SetRootPath(QString *root_path);
    void Init(QString path);

signals:
    void Close();

private:
    QWidget *info_widget_;
    QLabel *tips_type_label_;
    QLabel *type_label_;
    QLabel *tips_pos_label_;
    QLabel *pos_label_;
    QLabel *tips_size_label_;
    QLabel *size_label_;
    QLabel *tips_info_label_;
    QLabel *info_label_;
    QLabel *tips_creator_label_;
    QLabel *creator_label_;
    QLabel *tips_visit_label_;
    QLabel *visit_label_;
    QLabel *tips_modify_label_;
    QLabel *modify_label_;

    QWidget *button_widget_;
    QPushButton *show_size_button_;
    QPushButton *close_button_;

private:
    QString *root_path_;
    QString curr_path_;
}; //属性窗口

class FileManager : public QWidget
{
    Q_OBJECT
public:
    FileManager(QWidget *parent = nullptr);

signals:
    void ShowWindow();
    void HideWindow();

public slots:
    void SetScale(double scale_w, double scale_h);

private slots:
    void Sort();
    void ShowKeyboard(bool iscreator); //显示键盘
    void Refresh(); //刷新
    void FileLIstCurr(QTableWidgetItem *item); //文件列表框被点击
    void KeyGetData(QString data); //键盘的返回值

    void Copy();
    void Move();
    void Stick();
    void Rename();
    void Del();
    void Natule();

private:
    void hideEvent(QHideEvent *);
    void showEvent(QShowEvent *);
    void ConnectInit();
    void FindDirs(QString path); //寻找某个路径下的所有文件以及文件夹
    void AddWidget(QStringList &data_list, QString path); //为数据框添加数据
    void CopyOrMove(QString path, QString target_path, bool iscopy); //递归复制或移动文件或文件夹，iscopy为真时复制否则移动
    void DelDir(QString dir_path); //递归删除一个文件夹及其所有内容

private:
    QLabel *tips_label_;

    ImageButton *show_sort_button_;
    ImageButton *hide_sort_button_;
    QLabel *curr_path_label_;

    TableBox *file_box_;

    QWidget *button_widget_;
    ComboBoxObject *menu_button_;
    QPushButton *creator_button_;
    QPushButton *upper_button_;
    QPushButton *home_button_;
    QPushButton *refresh_button_;

    Keyboard *key_board_;
    NatuleWindow *natule_window_;

    QAction *copy_action_; //复制
    QAction *move_action_; //移动
    QAction *stick_action_; //粘贴
    QAction *rename_action_; //重命名
    QAction *del_action_; //删除
    QAction *natule_action_; //属性

private:
    QString root_path_; //根目录
    QString home_path_; //主目录
    QString curr_path_; //当前路径

    QStringList data_list_; //数据

    QString cove_path_; //复制与移动的原路径
    bool iscopy_; //真为复制假为移动

    bool pos_neg_; //用于排序，真为正序假为倒序
    bool iscreator_; //是否是新建文件夹，真为是
};

#endif // FILEMANAGER_H
