#ifndef BACKUPRECOVERY_H
#define BACKUPRECOVERY_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>

#include "../../windowobject.h"
#include "../../stackedwidobject.h"
#include "../../filemangerobject.h"
#include "../../commoncontrols.h"
#include "../../commoncontrols.h"

namespace robotTeachABB
{
    class BackupRecovery; //备份与恢复主窗口
    class BackRecoWidget; //主界面
    class BackWidget; //备份窗口
    class FolderWidget; //文件夹选择窗口
    class RecoveryWidget; //还原窗口
}

class RecoveryWidget : public WindowObject
{
    Q_OBJECT
public:
    RecoveryWidget(QWidget *parent = nullptr, int type = 6);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *tips_label_;
    QLabel *tips2_label_;

public:
    QTextEdit *path_edit_;
    QPushButton *select_button_;
    QPushButton *renew_button_;
    QPushButton *cancel_button_;
};

class FolderWidget : public FileMangerObject
{
    Q_OBJECT
public:
    FolderWidget(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *tips_label_;
    QLabel *decorate_label_;
    QLabel *tips2_label_;
    QWidget *win_button_;

public:
    QLineEdit *path_edit_;
    QPushButton *yes_button_; //确认按钮
    QPushButton *no_button_; //取消按钮
};

class BackWidget : public WindowObject
{
    Q_OBJECT
public:
    BackWidget(QWidget *parent = nullptr, int type = 6);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *tips_label_;
    QLabel *folder_label_;
    QLabel *route_label_;
    QLabel *path_label_;

public:
    QLineEdit *folder_edit_; //备份文件夹输入框
    QTextEdit *route_edit_; //备份路径输入框
    QTextEdit *path_edit_; //备份路径框
    QPushButton *clavier_button_; //键盘按钮
    QPushButton *select_button_; //选择按钮
    QPushButton *back_button_; //备份按钮
    QPushButton *cancel_button_; //取消按钮
};

class BackRecoWidget : public QWidget
{
    Q_OBJECT
public:
    BackRecoWidget(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *);

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    QLabel *back_label_; //备份按钮
    QLabel *recover_label_; //恢复按钮
    double scale_w_;
    double scale_h_;

public:
    QPushButton *back_button_; //备份按钮
    QPushButton *recovery_button_; //恢复按钮
};

class BackupRecovery : public WindowObject
{
    Q_OBJECT
public:
    explicit BackupRecovery(QWidget *parent = nullptr, int type = 6);
    void resizeEvent(QResizeEvent *);
    void ConnectInit();
    void CutDirWidget(); //切换到选择文件夹界面

signals:

public slots:
    void SetScale(double scale_w, double scale_h);

private:
    StackedWidObject *stack_widget_;
    Keyboard *keyboard_; //键盘
    bool select_; //用于决定选择文件夹中的确定和取消按钮被按下时回退到哪个界面，真为备份假为恢复

public:
    BackRecoWidget *back_reco_widget_;
    BackWidget *back_widget_;
    FolderWidget *dir_widget_;
    RecoveryWidget *reco_widget_;
};

#endif // BACKUPRECOVERY_H
