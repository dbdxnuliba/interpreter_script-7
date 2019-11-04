#include "progeditmain.h"
#include "teachstate.h"
#include "RobotClient/robotClient.h"
#include <QTextBlock>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QHeaderView>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QDebug>

using namespace rclib;

ProgEditMain::ProgEditMain(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *tmo_layout = new QHBoxLayout;
    task_button_ = new QPushButton(tr("任务与程序"), this);
    module_button_ = new QPushButton(tr("模块"), this);
    order_button_ = new QPushButton(tr("例行程序"), this);
    task_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    module_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    order_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tmo_layout->addWidget(task_button_);
    tmo_layout->addWidget(module_button_);
    tmo_layout->addWidget(order_button_);
    tmo_layout->setSpacing(0);

    QHBoxLayout *code_layout = new QHBoxLayout;
    row_num_ = new CodeRow(this);
    code_edit_ = new CodeEdit(this);
    add_code_window_ = new AddCodeWindow(this);
    edit_window_ = new EditWindow(this);
    debug_window_ = new DebugWindow(this);
    code_layout->addWidget(row_num_);
    code_layout->addWidget(code_edit_);
    code_layout->addWidget(add_code_window_);
    code_layout->addWidget(edit_window_);
    code_layout->addWidget(debug_window_);
    row_num_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    code_layout->setStretchFactor(row_num_, 1);    //设置控件之间
    code_layout->setStretchFactor(code_edit_, 25); //的比例
    code_layout->setStretchFactor(add_code_window_, 20);
    code_layout->setStretchFactor(edit_window_, 20);
    code_layout->setStretchFactor(debug_window_, 20);
    code_layout->setContentsMargins(0, 0, 0, 0);
    code_layout->setSpacing(0);

    button_widget_ = new QWidget(this);
    QHBoxLayout *butt_layout = new QHBoxLayout(button_widget_);
    add_button_ = new ComboBoxObject(tr("添加指令"), button_widget_);
    edit_button_ = new ComboBoxObject(tr("编辑"), button_widget_);
    debug_button_ = new ComboBoxObject(tr("调试"), button_widget_);
    modify_button_ = new QPushButton(tr("修改位置"), button_widget_);
    state_button_ = new QPushButton(tr("隐藏声明"), button_widget_);
    add_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    debug_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    modify_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    state_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    butt_layout->addWidget(add_button_);
    butt_layout->addWidget(edit_button_);
    butt_layout->addWidget(debug_button_);
    butt_layout->addWidget(modify_button_);
    butt_layout->addWidget(state_button_);
    butt_layout->setContentsMargins(0, 0, 0, 0);
    butt_layout->setSpacing(0);
    button_widget_->setLayout(butt_layout);

    layout->addLayout(tmo_layout);
    layout->addLayout(code_layout);
    layout->addWidget(button_widget_);
    layout->setStretchFactor(tmo_layout, 1);
    layout->setStretchFactor(code_layout, 7);
    layout->setStretchFactor(button_widget_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    add_code_window_->hide();
    edit_window_->hide();
    debug_window_->hide();

    keyboard_ = new Keyboard(this);
    keyboard_->hide();

    mod_var_window_ = new ModVarWindow(this);
    mod_var_window_->head_widget_->hide();
    mod_var_window_->hide();

    search_window_ = new SearchRoutine(this);
    search_window_->hide();

    data_window_ = new ShowSystemData(this);
    data_window_->hide();

    change_if_for_window_ = new ChangeIFandFOR(this);
    change_if_for_window_->hide();

    change_move_window_ = new ChangeMoveWindow(this);
    change_move_window_->hide();

    change_select_window_ = new ChangeSelectionWindow(this);
    change_select_window_->hide();

    pp_move_rout_window_ = new PPtoRoutWindow(this);
    pp_move_rout_window_->hide();

    isshow_ = true;
    timer_start_ = false;
    this_isshow_ = false;
    current_row_ = -1;
    next_row_ = -1;

    add_button_->setFlat(true);
    edit_button_->setFlat(true);
    debug_button_->setFlat(true);
    modify_button_->setFlat(true);
    state_button_->setFlat(true);

    task_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    module_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    order_button_->setStyleSheet("background-color: rgb(192, 192, 192)");

    code_edit_->SetCodeRow(row_num_);
    code_edit_->SetCopyStr(&copy_str_);
    code_edit_->SetEditWindow(edit_window_);
    code_edit_->SetDebugWindow(debug_window_);

    update_pointer_ = new QTimer(this);

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    keyboard_->setAutoFillBackground(true);
    keyboard_->setPalette(p);

    ConnectInit();
    EditWinConnectInit();
}

ProgEditMain::~ProgEditMain()
{
    EditData::instance()->SaveMod();
}

void ProgEditMain::showEvent(QShowEvent *)
{
    if(timer_start_)
    {
        if(!update_pointer_->isActive())
            update_pointer_->start(100);
    }
    this_isshow_ = true;
}

void ProgEditMain::hideEvent(QHideEvent *)
{
    if(timer_start_)
    {
        if(update_pointer_->isActive())
            update_pointer_->stop();
        else
            timer_start_ = false;
    }
    this_isshow_ = false;
}

void ProgEditMain::SetScale(double scale_w, double scale_h)
{
    if(!keyboard_->isHidden())
    {
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
    }
    keyboard_->SetScale(scale_w, scale_h);

    if(!mod_var_window_->isHidden())
    {
        mod_var_window_->resize(this->size());
        mod_var_window_->move(0, 0);
    }
    mod_var_window_->SetScale(scale_w, scale_h);

    if(!search_window_->isHidden())
    {
        search_window_->resize(this->size());
        search_window_->move(0, 0);
    }
    search_window_->SetScale(scale_w, scale_h);

    if(!data_window_->isHidden())
    {
        data_window_->resize(this->size());
        data_window_->move(this->pos());
    }
    data_window_->SetScale(scale_w, scale_h);

    if(!change_if_for_window_->isHidden())
    {
        change_if_for_window_->resize(this->size());
        change_if_for_window_->move(0, 0);
    }
    change_if_for_window_->SetScale(scale_w, scale_h);

    if(!change_move_window_->isHidden())
    {
        change_move_window_->resize(this->size());
        change_move_window_->move(0, 0);
    }
    change_move_window_->SetScale(scale_w, scale_h);

    if(!change_select_window_->isHidden())
    {
        change_select_window_->resize(this->size());
        change_select_window_->move(0, 0);
    }
    change_select_window_->SetScale(scale_w, scale_h);

    if(!pp_move_rout_window_->isHidden())
    {
        pp_move_rout_window_->resize(this->size());
        pp_move_rout_window_->move(0, 0);
    }
    pp_move_rout_window_->SetScale(scale_w, scale_h);
}

void ProgEditMain::StartRun()
{
    if(!update_pointer_->isActive())
    {
        if(!this->isHidden() && this_isshow_)
            update_pointer_->start(100);
        timer_start_ = true;
    }
}

void ProgEditMain::ConnectInit()
{
    connect(add_button_, &ComboBoxObject::clicked,
            [ = ]()
    {
        if(add_code_window_->isHidden())
        {
            if(!edit_window_->isHidden())
                edit_window_->hide();

            if(!debug_window_->isHidden())
                debug_window_->hide();

            add_code_window_->show();
        }
        else
        {
            add_code_window_->hide();
        }
    });

    connect(edit_button_, &ComboBoxObject::clicked,
            [=]()
    {
        if(edit_window_->isHidden())
        {
            if(!add_code_window_->isHidden())
                add_code_window_->hide();

            if(!debug_window_->isHidden())
                debug_window_->hide();

            edit_window_->show();
        }
        else
        {
            edit_window_->hide();
        }
    });

    connect(debug_button_, &ComboBoxObject::clicked,
            [=]()
    {
        if(debug_window_->isHidden())
        {
            if(!add_code_window_->isHidden())
                add_code_window_->hide();

            if(!edit_window_->isHidden())
                edit_window_->hide();

            debug_window_->show();
        }
        else
            debug_window_->hide();
    });

    connect(add_code_window_, &AddCodeWindow::ShowWindow,
            [ = ]()
    {
        add_button_->LowerArrow();
        code_edit_->ShowAddCode();
    });

    connect(add_code_window_, &AddCodeWindow::HideWindow,
            [ = ]()
    {
        add_button_->UpperArrow();
        code_edit_->HideAddCode();
    });

    connect(edit_window_, &EditWindow::ShowWindow,
            [=]()
    {
        edit_button_->LowerArrow();
        code_edit_->ShowAddCode();
    });

    connect(edit_window_, &EditWindow::HideWindow,
            [=]()
    {
        edit_button_->UpperArrow();
        code_edit_->HideAddCode();
    });

    connect(debug_window_, &DebugWindow::ShowWindow,
            [=]()
    {
        debug_button_->LowerArrow();
        code_edit_->ShowAddCode();
    });

    connect(debug_window_, &DebugWindow::HideWindow,
            [=]()
    {
        debug_button_->UpperArrow();
        code_edit_->HideAddCode();
    });

    connect(state_button_, &QPushButton::clicked,
            [=]()
    {
        if(isshow_)
        {
            isshow_ = false;
            state_button_->setText(tr("显示声明"));
        }
        else
        {
            isshow_ = true;
            state_button_->setText(tr("隐藏声明"));
        }

        code_edit_->SetIsShow(isshow_);
    });

    connect(mod_var_window_, &ModVarWindow::Defineed,
            [=]()
    {
        mod_var_window_->hide();
    });

    connect(mod_var_window_, &ModVarWindow::Cancel,
            [=]()
    {
        mod_var_window_->hide();
    });

    connect(search_window_, &SearchRoutine::Close,
            [=]()
    {
        search_window_->hide();
    });

    connect(data_window_, &ShowSystemData::Cancel,
            [=]()
    {
        data_window_->hide();
    });

    connect(change_if_for_window_, &ChangeIFandFOR::Cancel, change_if_for_window_, &ChangeIFandFOR::hide);

    connect(change_if_for_window_, &ChangeIFandFOR::Define, this, &ProgEditMain::ChangeIFFor);

    connect(change_move_window_, &ChangeMoveWindow::Cancel, change_move_window_, &ChangeMoveWindow::hide);

    connect(change_move_window_, &ChangeMoveWindow::Define, this, &ProgEditMain::ChangeMove);

    connect(&debug_window_->see_value_button_, &QPushButton::clicked, this, &ProgEditMain::ModVar);

    connect(add_code_window_, &AddCodeWindow::AddCode, code_edit_, &CodeEdit::AddCode);

    connect(keyboard_->cancel_button_, &QPushButton::clicked, [=](){keyboard_->hide();});

    connect(keyboard_, &Keyboard::GetData, this, &ProgEditMain::KeyData);

    connect(modify_button_, &QPushButton::clicked, this, &ProgEditMain::ModifyPos);

    connect(this, &ProgEditMain::Inspt, this, &ProgEditMain::Insp);

    connect(&debug_window_->search_routine_button_, &QPushButton::clicked, this, &ProgEditMain::Search);

    connect(&debug_window_->see_system_button_, &QPushButton::clicked, this, &ProgEditMain::SystemData);

    connect(&debug_window_->pp_to_main_button_, &QPushButton::clicked, this, &ProgEditMain::PPtoMain);

    connect(&debug_window_->pp_to_curso_button_, &QPushButton::clicked, this, &ProgEditMain::PPtoCursor);

    connect(&debug_window_->pp_to_routine_button_, &QPushButton::clicked, this, &ProgEditMain::PPtoRout);

    connect(&debug_window_->curso_topp_button_, &QPushButton::clicked, this, &ProgEditMain::CursortoPP);

    connect(code_edit_, &CodeEdit::DoubleClick, this, &ProgEditMain::ChangeSelect);

    connect(change_move_window_->GetChSl()->creat_var_window_, &CreateNewVar::CreateVar, code_edit_, &CodeEdit::CreateVar);

    connect(change_select_window_, &ChangeSelectionWindow::Cancel, change_select_window_, &ChangeSelectionWindow::hide);

    connect(change_select_window_, &ChangeSelectionWindow::Define, this, &ProgEditMain::ChangeSelection);

    connect(change_select_window_->creat_var_window_, &CreateNewVar::CreateVar, code_edit_, &CodeEdit::CreateVar);

    connect(code_edit_, &CodeEdit::ShowChangeSelect, this, &ProgEditMain::CodeEditShowChangeSelect);

    connect(update_pointer_, &QTimer::timeout, this, &ProgEditMain::UpdatePointer);

    connect(pp_move_rout_window_, &PPtoRoutWindow::Cancel, pp_move_rout_window_, &PPtoRoutWindow::hide);

    connect(pp_move_rout_window_, &PPtoRoutWindow::Define, this, &ProgEditMain::PPMoveRout);

    connect(&edit_window_->shear_button_, &QPushButton::clicked, this, &ProgEditMain::Cut);

    connect(&edit_window_->copy_button_, &QPushButton::clicked, this, &ProgEditMain::Copy);

    connect(&edit_window_->stick_button_, &QPushButton::clicked, this, &ProgEditMain::Paste);

    connect(&edit_window_->upper_stick_button_, &QPushButton::clicked, this, &ProgEditMain::UpPaste);
}

void ProgEditMain::EditWinConnectInit()
{
    connect(&edit_window_->upper_button_, &QPushButton::clicked,
            [=]()
    {
        code_edit_->MvStart();
        row_num_->MvStart();
    });

    connect(&edit_window_->lower_button_, &QPushButton::clicked,
            [=]()
    {
        code_edit_->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        row_num_->MvEnd();
    });


    connect(&edit_window_->remarks_button_, &QPushButton::clicked, this, &ProgEditMain::Remark);

    connect(&edit_window_->chang_data_button_, &QPushButton::clicked, this, &ProgEditMain::ChangeSelect);

    connect(&edit_window_->del_button_, &QPushButton::clicked, this, &ProgEditMain::DelCode);
}

void ProgEditMain::ShowChangeSelect(QString code, QString orig_code, QString mod_name, int select, bool rep)
{
    change_select_window_->resize(this->size());
    change_select_window_->move(0, 0);
    change_select_window_->show();
    change_select_window_->Init(code, orig_code, mod_name, select, rep);
}

void ProgEditMain::Remark()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    ModuleData &mod_info = EditData::instance()->GetModData(code_edit_->GetMod());
    if(mod_info.type) //系统模块不允许修改
    {
        QMessageBox::warning(this, tr("提示"), tr("您无法进行修改"));
        return;
    }

    auto sor = code_edit_->textCursor();
    auto block = sor.block();
    auto word = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(word == "<SMT>" || word.isEmpty() || line.isEmpty())
        return;
    if(code_edit_->GetIsShow())
    {
        if(sor.blockNumber() <= code_edit_->GetRow())
            return;
    }

    int row = block.blockNumber();
    auto pir = code_edit_->Convert(row);

    auto list = word.split("\u2029");

    if(list.size() != 1)
    {
        //多条语句去除注释时存在问题，此功能暂时搁置
//        QString temp_str;
//        for(int i = 0; i < list.size(); i++)
//        {
//            auto &str = list[i];

//            if(temp_str.isEmpty())
//                temp_str.append("!" + str);
//            else
//                temp_str.append("\n!" + str);
//        }

//        EditData::instance()->RoutRemCode(pir.first, pir.second - list.size() + 1, list.size(), temp_str);

//        int pos = code_edit_->verticalScrollBar()->value();
//        code_edit_->SetData(code_edit_->GetMod());

//        code_edit_->verticalScrollBar()->setValue(pos);
//        row_num_->verticalScrollBar()->setValue(pos);
    }
    else
    {
        if(line.at(0) == '!')
            line = line.mid(1);
        else
            line.insert(0, "!");

        EditData::instance()->RoutRemCode(pir.first, pir.second, line);

        int pos = code_edit_->verticalScrollBar()->value();
        code_edit_->SetData(code_edit_->GetMod());

        code_edit_->verticalScrollBar()->setValue(pos);
        row_num_->verticalScrollBar()->setValue(pos);
    }
}

void ProgEditMain::DelCode()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    ModuleData &mod_info = EditData::instance()->GetModData(code_edit_->GetMod());
    if(mod_info.type) //系统模块不允许修改
    {
        QMessageBox::warning(this, tr("提示"), tr("您无法进行修改"));
        return;
    }

    auto sor = code_edit_->textCursor();
    auto block = sor.block();
    auto word = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(word == "<SMT>" || word.isEmpty() || line.isEmpty())
        return;

    int row = block.blockNumber();
    auto pir = code_edit_->Convert(row);

    auto list = word.split("\u2029");

    if(list.size() != 1)
    {
        EditData::instance()->RoutDelCode(pir.first, pir.second - list.size() + 1, list.size());
    }
    else
    {
        EditData::instance()->RoutDelCode(pir.first, pir.second);
    }

    int pos = code_edit_->verticalScrollBar()->value();
    code_edit_->SetData(code_edit_->GetMod());

    code_edit_->verticalScrollBar()->setValue(pos);
    row_num_->verticalScrollBar()->setValue(pos);
}

void ProgEditMain::Copy()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    if(EditData::instance()->GetModType(code_edit_->GetMod()))
        return;

    auto sor = code_edit_->textCursor();
    auto block = sor.block();

    int row_num = sor.blockNumber();
    auto select = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(select.isEmpty() || line.isEmpty() || line == "<SMT>")
        return;
    if(code_edit_->GetIsShow())
    {
        if(row_num <= code_edit_->GetRow())
            return;
    }

    auto head = select.section(" ", 0, 0);
    if(head == "IF" || head == "FOR" || head == "WHILE" || head == "TEST")
    {
        copy_str_ = select;
        copy_str_.replace("\u2029", "\n");
    }
    else
    {
        auto pair = code_edit_->Convert(row_num);
        auto &rout_data = EditData::instance()->GetRoutData(pair.first);
        copy_str_ = rout_data.code.at(pair.second);
    }
    edit_window_->stick_button_.setEnabled(true);
    edit_window_->upper_stick_button_.setEnabled(true);
}

void ProgEditMain::Cut()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    if(EditData::instance()->GetModType(code_edit_->GetMod()))
        return;

    auto sor = code_edit_->textCursor();
    auto block = sor.block();

    int row_num = sor.blockNumber();
    auto select = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(select.isEmpty() || line.isEmpty() || line == "<SMT>")
        return;
    if(code_edit_->GetIsShow())
    {
        if(row_num <= code_edit_->GetRow())
            return;
    }

    auto head = select.section(" ", 0, 0);
    if(head == "IF" || head == "FOR" || head == "WHILE" || head == "TEST")
    {
        copy_str_ = select;
        copy_str_.replace("\u2029", "\n");
    }
    else
    {
        auto pair = code_edit_->Convert(row_num);
        auto &rout_data = EditData::instance()->GetRoutData(pair.first);
        copy_str_ = rout_data.code.at(pair.second);
    }

    this->DelCode();
    edit_window_->stick_button_.setEnabled(true);
    edit_window_->upper_stick_button_.setEnabled(true);
}

void ProgEditMain::Paste()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    if(copy_str_.isEmpty() || EditData::instance()->GetModType(code_edit_->GetMod()))
        return;

    auto sor = code_edit_->textCursor();
    auto block = sor.block();

    int row_num = sor.blockNumber();
    auto select = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(select.isEmpty() || line.isEmpty())
        return;
    if(code_edit_->GetIsShow())
    {
        if(row_num <= code_edit_->GetRow())
            return;
    }

    auto pair = code_edit_->Convert(row_num);

    auto code_list = select.split("\n");
    EditData::instance()->RoutAddCode(pair.first, pair.second + code_list.size(), copy_str_);
    code_edit_->Refresh();
}

void ProgEditMain::UpPaste()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    if(copy_str_.isEmpty() || EditData::instance()->GetModType(code_edit_->GetMod()))
        return;

    auto sor = code_edit_->textCursor();
    auto block = sor.block();

    int row_num = sor.blockNumber();
    auto select = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(select.isEmpty() || line.isEmpty())
        return;
    if(code_edit_->GetIsShow())
    {
        if(row_num <= code_edit_->GetRow())
            return;
    }

    auto pair = code_edit_->Convert(row_num);

    select.replace("\u2029", "\n");
    auto code_list = select.split("\n");
    EditData::instance()->RoutAddCode(pair.first, pair.second - code_list.size() + 1, copy_str_);
    code_edit_->Refresh();
}

void ProgEditMain::ChangeSelect()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    ModuleData &mod_info = EditData::instance()->GetModData(code_edit_->GetMod());
    if(mod_info.type) //系统模块不允许修改
    {
        QMessageBox::warning(this, tr("提示"), tr("您无法进行修改"));
        return;
    }

    auto sor = code_edit_->textCursor();
    auto block = sor.block();

    auto word = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(word == "<SMT>" || word.isEmpty() || line.isEmpty())
        return;
    if(code_edit_->GetIsShow())
    {
        if(sor.blockNumber() <= code_edit_->GetRow())
            return;
    }

    if(word.size() > line.size()) //类似于IF的语句块
    {
        auto code_name = word.section(" ", 0, 0);
        if(code_name == "WHILE")
        {
            QMessageBox::about(this, tr("提示"), tr("是否需要编辑语句?\n点击表达式"));
            return;
        }
        else if(code_name == "IF")
        {
            change_if_for_window_->resize(this->size());
            change_if_for_window_->move(0, 0);
            change_if_for_window_->show();
            change_if_for_window_->Init(0, word);
        }
        else if(code_name == "FOR")
        {
            change_if_for_window_->resize(this->size());
            change_if_for_window_->move(0, 0);
            change_if_for_window_->show();
            change_if_for_window_->Init(1, word);
        }
        else if(code_name == "TEST")
        {
            change_if_for_window_->resize(this->size());
            change_if_for_window_->move(0, 0);
            change_if_for_window_->show();
            change_if_for_window_->Init(2, word);
        }
        else
        {
            return;
        }
    }
    else //普通的语句
    {
        auto head = line.section(" ", 0, 0);
        if(word == line)
        {
            if(head == "MoveJ" || head == "MoveAbsJ" || head == "MoveC" || head == "MoveL")
            {
                int row = sor.blockNumber();
                auto rout_info = code_edit_->Convert(row);

                change_move_window_->resize(this->size());
                change_move_window_->move(0, 0);
                change_move_window_->show();
                change_move_window_->Init(line, code_edit_->GetMod(), rout_info.first, rout_info.second);
            }
            else if(line.at(0) == '!')
            {
                keyboard_->resize(this->size());
                keyboard_->move(0, 0);
                keyboard_->show();
                keyboard_->Init(line);
            }
        }
        else
        {
            auto temp_code = line.mid(head.size() + 1);
            auto code_list = temp_code.split(", ");

            if(head == "MoveJ" || head == "MoveAbsJ" || head == "MoveC" || head == "MoveL")
            {
                auto info = code_edit_->Convert(sor.blockNumber());
                auto orig_code = EditData::instance()->GetRoutData(info.first).code.at(info.second);

                if(head == "MoveJ" || head == "MoveL")
                {
                    if(word == code_list.at(0))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 0);
                    }
                    else if(word == code_list.at(1))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 1);
                    }
                    else if(word == code_list.at(2))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 2);
                    }
                    else
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 3);
                    }
                }
                else if(head == "MoveC")
                {
                    if(word == code_list.at(0))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 0);
                    }
                    else if(word == code_list.at(1))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 1);
                    }
                    else if(word == code_list.at(2))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 2);
                    }
                    else if(word == code_list.at(3))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 3);
                    }
                    else
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 4);
                    }
                }
                else if(head == "MoveAbsJ")
                {
                    if(word == code_list.at(1))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 1);
                    }
                    else if(word == code_list.at(2))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 2);
                    }
                    else if(word + ";" == code_list.at(3))
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 3);
                    }
                    else
                    {
                        ShowChangeSelect(line, orig_code, code_edit_->GetMod(), 0);
                    }
                }
            }
            else if(head != "VAR" && head != "CONST" && line.indexOf(":=") != -1)
            {
                emit code_edit_->ShowInsertExpres(word, "none", true, true);

                return;
            }
            else if(head == "Set" || head == "Add" || head == "PulseDO" || head == "SetDO" || head == "WaitDI" || head == "WaitDO" || head == "WaitTime")
            {
                ShowChangeSelect(line, line, code_edit_->GetMod());
                return;
            }
            else if(head == "WHILE")
            {
                emit code_edit_->ShowInsertExpres(word, "bool", true, true);
            }
            else if(head == "FOR")
            {
                int pos = sor.position() - block.position() - (block.text().size() - line.size()) - word.size();

                if(pos == 4)
                {
                    keyboard_->resize(this->size());
                    keyboard_->move(0, 0);
                    keyboard_->show();
                    keyboard_->Init(word);

                    return;
                }
                else if(line.mid(pos - 5, 4) == "FROM" && line.mid(pos + word.size() + 1, 2) == "TO")
                {
                    emit code_edit_->ShowInsertExpres(word, "none", true, true);
                }
                else if(line.mid(pos - 3, 2) == "TO" && line.mid(pos + word.size() + 1, 2) == "DO")
                {
                    emit code_edit_->ShowInsertExpres(word, "none", true, true);
                }
            }
            else if(line.at(0) == '!')
            {
                keyboard_->resize(this->size());
                keyboard_->move(0, 0);
                keyboard_->show();
                keyboard_->Init(line);
            }
            else if(head == "IF" || head == "ELSEIF")
            {
                emit code_edit_->ShowInsertExpres(word, "bool", true, true);
            }
            else if(head == "TEST" || head == "CASE")
            {
                emit code_edit_->ShowInsertExpres(word, "none", true, true);
            }
        }
    }
}

void ProgEditMain::ModifyPos()
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    if(EditData::instance()->GetModType(code_edit_->GetMod()))
    {
        return;
    }

    auto sor = code_edit_->textCursor();

    auto work = sor.selectedText();
    auto line = sor.block().text();
    line.replace("\t", "");
    if(work.isEmpty() || line.isEmpty())
    {
        return;
    }

    auto head = line.section(" ", 0, 0);
    if((head != "MoveJ") && (head != "MoveL") && (head != "MoveC") && (head != "MoveAbsJ"))
    {
        return;
    }

    if(!VarInfo::instance()->IsExist(work) && work != "*")
    {
        return;
    }

    auto rout = code_edit_->Convert(sor.blockNumber());

    if(head == "MoveAbsJ") //MoveAbsJ更特殊采用的是robjoint数据类型所以需要单独拎出来处理
    {
        auto joint = RobotClient::initance()->getRobotMotion().getCurrentJointPosition();
        QString jo_code = QString("[[%1,%2,%3,%4,%5,%6],[9E+09,9E+09,9E+09,9E+09,9E+09,9E+09]]").arg(joint.getValue(JOINT_1)).arg(joint.getValue(JOINT_2))
                .arg(joint.getValue(JOINT_3)).arg(joint.getValue(JOINT_4)).arg(joint.getValue(JOINT_5)).arg(joint.getValue(JOINT_6));

        if(work == "*")
        {
            auto x = QMessageBox::question(this, tr("提示"), tr("您确定要修改位置吗?"));
            if(x == QMessageBox::Yes)
            {
                line.replace(work, jo_code);
                EditData::instance()->RoutRemCode(rout.first, rout.second, line);
            }
        }
        else
        {
            auto var_point = Convert::DataTypeToVarFormData("jointtarget");
            var_point->code = jo_code;
            var_point->LastUpdate();

            auto vars_info = VarInfo::instance()->GetVarInfo(work);
            auto mod_name = code_edit_->GetMod();
            for(auto var : vars_info)
            {
                if(var.data_type != "jointtarget")
                    continue;
                if(var.action_scope == "local" && var.mod != mod_name)
                    continue;
                if(var.action_scope != "task" && var.action_scope != "local")
                {
                    if(var.action_scope != "all" && var.mod != mod_name)
                        continue;
                }

                auto x =  QMessageBox::question(this, tr("提示"), tr("您确定要修改位置吗?"));
                if(x == QMessageBox::Yes)
                {
                    var.SetVar(var_point);

                    auto &var_info = VarInfo::instance()->GetVarInfo(var.var_name, var.mod, var.action_scope);
                    var_info.var = var_point->code;

                    return;
                }
                else
                {
                    return;
                }
            }
        }

        return;
    }

    auto motion = RobotClient::initance()->getRobotMotion();
    auto tm = motion.getCurrentTerminal();

    auto x = tm.getValue(TERMINALINDEX::TERMINAL_X);
    auto y = tm.getValue(TERMINALINDEX::TERMINAL_Y);
    auto z = tm.getValue(TERMINALINDEX::TERMINAL_Z);
    Euler er(tm.getValue(TERMINALINDEX::TERMINAL_A), tm.getValue(TERMINALINDEX::TERMINAL_B), tm.getValue(TERMINALINDEX::TERMINAL_C));
    auto var_code = Convert::PositDoubleToPositCode(x, y, z, er).mid(1); //获取坐标值
    var_code = var_code.mid(0, var_code.size() - 1);

    auto var_point = Convert::DataTypeToVarFormData("robtarget");
    auto str1 = var_code.section("],[", 0, 0) + "]";
    auto str2 = "[" + var_code.section("],[", 1, 1);
    var_point->subnode.at(0)->code = str1;
    var_point->subnode.at(1)->code = str2;
    var_point->subnode.at(0)->LastUpdate();
    var_point->subnode.at(1)->LastUpdate();
    var_point->Update();
    var_code = var_point->code;

    if(work == "*")
    {
        auto x = QMessageBox::question(this, tr("提示"), tr("您确定要修改位置吗?"));
        if(x == QMessageBox::Yes)
        {
            if(head == "MoveC" && (line.section(", ", 0, 0) == "MoveC *") && (line.section(", ", 1, 1) == "*")) //MoveC有两个*号
            {
                auto block = sor.block();
                auto line_text = block.text();
                int num = sor.position() - block.position() - work.size();
                auto head_text = line_text.mid(num - 6, num -2);

                if(head_text == "MoveC ") //第一个*号
                {
                    line.replace(6, 1, var_code);
                }
                else
                {
                    line.replace(9, 1, var_code);
                }

                EditData::instance()->RoutRemCode(rout.first, rout.second, line);
            }
            else
            {
                line.replace("*", var_code);
                EditData::instance()->RoutRemCode(rout.first, rout.second, line);
            }
        }
    }
    else
    {
        auto vars_info = VarInfo::instance()->GetVarInfo(work);
        auto mod_name = code_edit_->GetMod();
        for(auto var : vars_info)
        {
            if(var.data_type != "robtarget")
                continue;
            if(var.action_scope == "local" && var.mod != mod_name)
                continue;
            if(var.action_scope != "task" && var.action_scope != "local")
            {
                if(var.action_scope != "all" && var.mod != mod_name)
                    continue;
            }

            auto x =  QMessageBox::question(this, tr("提示"), tr("您确定要修改位置吗?"));
            if(x == QMessageBox::Yes)
            {
                var.SetVar(var_point);

                auto &var_info = VarInfo::instance()->GetVarInfo(var.var_name, var.mod, var.action_scope);
                var_info.var = var_point->code;

                return;
            }
            else
            {
                return;
            }
        }
    }
}

void ProgEditMain::ModVar()
{
    auto &mod_info = EditData::instance()->GetModData(code_edit_->GetMod());
    if(mod_info.type) //系统模块不允许修改
    {
        return;
    }

    QTextCursor sor = code_edit_->textCursor();
    QString word_str = sor.selectedText();
    QString line_str = sor.block().text();
    int row_curr = sor.blockNumber();
    line_str.replace("\t", "");
    if(word_str.isEmpty() || line_str.isEmpty())
    {
        return;
    }
    if(code_edit_->GetIsShow()) //需要显示声明
    {
        if(row_curr < code_edit_->GetRow())
        {
            return;
        }
    }

    if(word_str != "*")
    {
        auto var_info = VarInfo::instance()->GetVarInfo(word_str);
        for(auto &var : var_info)
        {
            if(!var.genre) //内置变量
            {
                var_point_ = Convert::DataTypeToVarFormData(var.data_type, var.var.toString());

                mod_var_window_->Init(var_point_, word_str, var.mod, var.action_scope, true);
                mod_var_window_->resize(this->size());
                mod_var_window_->move(0, 0);
                mod_var_window_->show();
                return;
            }

            bool issys = EditData::instance()->GetModType(var.mod);

            var_point_ = Convert::DataTypeToVarFormData(var.data_type, var.var.toString());

            mod_var_window_->Init(var_point_, word_str, var.mod, var.action_scope, issys);
            mod_var_window_->resize(this->size());
            mod_var_window_->move(0, 0);
            mod_var_window_->show();
            return;
        }
    }
    else
    {
        auto info = code_edit_->Convert(row_curr);

        auto &rout_info = mod_info.routine_data[info.first];

        auto text = rout_info.code.at(info.second);

        if(text.section(" ", 0, 0) == "MoveC")
        {
            auto block = sor.block();
            auto line = block.text();
            int num = sor.position() - block.position() - word_str.size();
            auto head_text= line.mid(num - 6, num - 2);

            if(head_text == "MoveC ")
            {
                auto var = text.section(" ", 1, 1);
                var = var.mid(0, var.size() - 1);

                var_point_ = Convert::DataTypeToVarFormData("robtarget", var);
            }
            else
            {
                auto var = text.section(" ", 2, 2);
                var = var.mid(0, var.size() - 1);

                var_point_ = Convert::DataTypeToVarFormData("robtarget", var);
            }

            mod_var_window_->Init(var_point_, word_str, code_edit_->GetMod(), info.first, true);
            mod_var_window_->resize(this->size());
            mod_var_window_->move(0, 0);
            mod_var_window_->show();
        }
        else if(text.section(" ", 0, 0) == "MoveAbsJ")
        {
            auto var = text.section(" ", 1, 1);
            var = var.mid(0, var.size() - 9);

            var_point_ = Convert::DataTypeToVarFormData("jointtarget", var);

            mod_var_window_->Init(var_point_, word_str, code_edit_->GetMod(), info.first, true);
            mod_var_window_->resize(this->size());
            mod_var_window_->move(0, 0);
            mod_var_window_->show();
        }
        else
        {
            auto var = text.section(" ", 1, 1);
            var = var.mid(0, var.size() - 1);

            var_point_ = Convert::DataTypeToVarFormData("robtarget", var);

            mod_var_window_->Init(var_point_, word_str, code_edit_->GetMod(), info.first, true);
            mod_var_window_->resize(this->size());
            mod_var_window_->move(0, 0);
            mod_var_window_->show();
        }
    }
}

void ProgEditMain::Insp()
{
    if(insp_.isNull())
        return;

    if(insp_->isEmpty())
    {
        QMessageBox::about(this, tr("提示"), tr("您无法进行修改"));
        return;
    }
    else
    {
        QString error;
        for(auto it = insp_->begin(); it != insp_->end(); ++it)
        {
            QString tmp;
            if(it->first.isEmpty())
                tmp = QString("%1模块 %2行出现问题\n").arg(it.key()).arg(it->second);
            else
                tmp = QString("%1模块 %2例行程序 %3行出现问题\n").arg(it.key()).arg(it->first).arg(it->second);
            error.append(tmp);
        }

        QMessageBox::about(this, tr("提示"), error);
    }
}

void ProgEditMain::Search()
{
    search_window_->resize(this->size());
    search_window_->move(0, 0);
    search_window_->Init();
    search_window_->show();
}

void ProgEditMain::SystemData()
{
    data_window_->resize(this->size());
    data_window_->move(0, 0);
    data_window_->Init();
    data_window_->show();
}

void ProgEditMain::PPtoMain()
{
    ProgramPointer point;
    point.nFunction = EditData::instance()->GetRoutMainPoint();
    point.nSentence = 0;

    RobotClient::initance()->setPointer(point);
}

void ProgEditMain::PPtoCursor()
{
    auto sor = code_edit_->textCursor();
    if(sor.selectedText().isEmpty())
        return;

    auto pair = code_edit_->Convert(sor.blockNumber());
    auto pair_point = EditData::instance()->GetRoutPoint(pair.first, pair.second);
    if(pair_point.second == -1)
        return;
    auto point = RobotClient::initance()->getProgramPointer();
    if(point.nFunction != pair_point.first)
    {
        QString mess = tr("无法将 PP 移至选定的位置。命令") + " Move PP to Cursor " + tr("仅可使程序指针在执行栈内移动。");
        mess.append(tr("如果您需要将程序指针移至别处，可使用另一命令，如\"PP移至例行程序\"..."));
        mess.append(tr("它可以清空执行栈。"));
        QMessageBox::warning(this, tr("移动 PP"), mess);
        return;
    }

    point.nSentence = pair_point.second;
    RobotClient::initance()->setPointer(point);
}

void ProgEditMain::PPtoRout()
{
    auto point = RobotClient::initance()->getProgramPointer();
    auto pair = EditData::instance()->GetRoutPoint(point.nFunction, point.nSentence);
    if(pair.second == -1)
        return;

    pp_move_rout_window_->resize(this->size());
    pp_move_rout_window_->move(0, 0);
    pp_move_rout_window_->show();
    pp_move_rout_window_->Init(pair.first);
}

void ProgEditMain::PPMoveRout(int func)
{
    ProgramPointer point;
    point.nFunction = func;
    point.nSentence = 0;

    RobotClient::initance()->setPointer(point);
    pp_move_rout_window_->hide();
}

void ProgEditMain::CursortoPP()
{
    auto point = RobotClient::initance()->getProgramPointer();
    auto pair = EditData::instance()->GetRoutPoint(point.nFunction, point.nSentence);
    if(pair.second == -1)
        return;
    int row = code_edit_->Convert(pair.first, pair.second);
    code_edit_->SetSelect(row);
}

void ProgEditMain::UpdatePointer()
{
    if(!TeachState::instance()->GetRunState())
    {
        update_pointer_->stop();
        timer_start_ = false;
        return;
    }

    auto point = RobotClient::initance()->getProgramPointer();
    auto pt = EditData::instance()->GetRoutPoint(point.nFunction, point.nSentence);
    if(pt.second == -1)
        return;

    auto mod_name = EditData::instance()->GetRoutMod(pt.first);
    if(mod_name != code_edit_->GetMod()) //如果当前例行程序不在所显示的模块中
    {
        code_edit_->SetData(mod_name); //显示该模块
    }

    int row = code_edit_->Convert(pt.first, pt.second);
    auto sor = row_num_->textCursor();
    auto *docu = sor.document();

    if(current_row_ != -1)
    {
        auto block = docu->findBlockByLineNumber(current_row_);
        sor.setPosition(block.position() + block.text().size(), QTextCursor::MoveAnchor);
        sor.setPosition(block.position(), QTextCursor::KeepAnchor);
        sor.insertText(QString::number(current_row_));
        current_row_ = -1;
    }

    {
        auto block = docu->findBlockByLineNumber(row);
        sor.setPosition(block.position() + block.text().size(), QTextCursor::MoveAnchor);
        sor.setPosition(block.position(), QTextCursor::KeepAnchor);
        sor.insertText("➡");
        current_row_ = row;
    }

//    if(next_row_ != -1)
//    {
//        auto block = docu->findBlockByLineNumber(next_row_);
//        sor.setPosition(block.position() + block.text().size(), QTextCursor::MoveAnchor);
//        sor.setPosition(block.position(), QTextCursor::KeepAnchor);
//        sor.insertText(QString::number(next_row_));
//        next_row_ = -1;
//    }

//    {
//        int next = row + 1;

//        if(EditData::instance()->GetRoutPoint(point.nFunction, next).second != -1)
//        {
//            auto block = docu->findBlockByLineNumber(next);
//            sor.setPosition(block.position() + block.text().size(), QTextCursor::MoveAnchor);
//            sor.setPosition(block.position(), QTextCursor::KeepAnchor);
//            sor.insertText("￡");
//            next_row_ = next;
//        }
//    }


    row_num_->setTextCursor(sor);
    int val = row_num_->verticalScrollBar()->value();
    code_edit_->verticalScrollBar()->setValue(val);
}

void ProgEditMain::Inspect(QSharedPointer<QMap<QString, QPair<QString, int>>> insp)
{
    insp_ = insp;
    emit Inspt();
}

void ProgEditMain::KeyData(QString data)
{
    if(data.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入内容"));
        return;
    }

    if(data.at(0) == '!') //备注
    {
        QTextCursor sor = code_edit_->textCursor();
        int row_curr = sor.blockNumber();
        QPair<QString, int> cur = code_edit_->Convert(row_curr);
        sor.removeSelectedText();
        sor.insertText(data);
        QString text = sor.block().text();

        code_edit_->setTextCursor(sor);

        text.replace("\t", "");

        EditData::instance()->RoutRemCode(cur.first, cur.second, text);

        keyboard_->hide();
        return;
    }

    if(data.indexOf(QRegExp("^[A-Za-z0-9]+$")) == -1)
    {
        QMessageBox::warning(this, tr("提示"), tr("您输入的格式有误"));
        return;
    }
    bool isint;
    data.mid(0, 1).toInt(&isint);
    if(isint)
    {
        QMessageBox::warning(this, tr("提示"), tr("您输入的格式有误"));
        return;
    }

    QTextCursor sor = code_edit_->textCursor();
    int row_curr = sor.blockNumber();
    QPair<QString, int> cur = code_edit_->Convert(row_curr);
    sor.removeSelectedText();
    sor.insertText(data);
    QString text = sor.block().text();

    code_edit_->setTextCursor(sor);

    text.replace("\t", "");

    EditData::instance()->RoutRemCode(cur.first, cur.second, text);

    keyboard_->hide();
}

void ProgEditMain::ChangeIFFor(QString code)
{
    auto sor = code_edit_->textCursor();
    auto info = code_edit_->Convert(sor.blockNumber());

    auto text = sor.selectedText();
    int number = text.split("\u2029").size();

    EditData::instance()->RoutRemCode(info.first, info.second - number + 1, number, code);

    int pos = code_edit_->verticalScrollBar()->value();
    code_edit_->SetData(code_edit_->GetMod());
    code_edit_->verticalScrollBar()->setValue(pos);
    row_num_->verticalScrollBar()->setValue(pos);

    change_if_for_window_->hide();
}

void ProgEditMain::ChangeMove(QString code)
{
    auto sor = code_edit_->textCursor();
    auto info = code_edit_->Convert(sor.blockNumber());

    EditData::instance()->RoutRemCode(info.first, info.second, code);

    int pos = code_edit_->verticalScrollBar()->value();
    code_edit_->SetData(code_edit_->GetMod());
    code_edit_->verticalScrollBar()->setValue(pos);
    row_num_->verticalScrollBar()->setValue(pos);

    change_move_window_->hide();
}

void ProgEditMain::CodeEditShowChangeSelect(QString code)
{
    ShowChangeSelect(code, code, code_edit_->GetMod(), -1, false);
}

void ProgEditMain::ChangeSelection(QString code, bool rep)
{
    if(rep) //替换
    {
        auto sor = code_edit_->textCursor();
        auto info = code_edit_->Convert(sor.blockNumber());

        EditData::instance()->RoutRemCode(info.first, info.second, code);

        int pos = code_edit_->verticalScrollBar()->value();
        code_edit_->SetData(code_edit_->GetMod());
        code_edit_->verticalScrollBar()->setValue(pos);
        row_num_->verticalScrollBar()->setValue(pos);
    }
    else //添加
    {
        code_edit_->ChangeSelect(code);
    }

    change_select_window_->hide();
}

CodeEdit::CodeEdit(QWidget *parent) : TextBrowserObject(parent)
{
    this->GetScrollBar()->hide(); //隐藏垂直滚动条
    horiz_scroll_bar_ = this->horizontalScrollBar();
    horiz_scroll_bar_->hide(); //隐藏水平滚动条
    this->setLineWrapMode(QTextBrowser::NoWrap); //取消自动换行
    enlarge_button_ = new QPushButton(this);
    narrow_button_ = new QPushButton(this);
    left_button_ = new QPushButton(this);
    right_button_ = new QPushButton(this);
    QFont font(this->font());
    font.setPointSize(font.pointSize() + 5);
    this->setFont(font);
    font_size_ = font.pointSize(); //获取初始字体大小
    font_max_size_ = font_size_ + 30;
    row_main_ = 0;
    isshow_ = true; //默认显示所有
    enlarge_button_->resize(55 * GetScaleW(), 42 * GetScaleH());
    narrow_button_->resize(enlarge_button_->size());
    left_button_->resize(narrow_button_->size());
    right_button_->resize(left_button_->size());
    enlarge_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/codeedit_enlarge_button.png)");
    narrow_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/codeedit_narrow_button.png)");
    left_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/codeedit_left_button.png)");
    right_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/codeedit_right_button.png)");
    MvStart();
    ConnInit();

    QPalette p(this->palette());
    p.setColor(QPalette::Text, QColor(72, 128, 0)); //将文字颜色改为绿色
    this->setPalette(p);
}

CodeEdit::~CodeEdit()
{
    SaveData();
}

void CodeEdit::resizeEvent(QResizeEvent *e)
{
    TextBrowserObject::resizeEvent(e);
    enlarge_button_->resize(55 * GetScaleW(), 42 * GetScaleH());
    narrow_button_->resize(enlarge_button_->size());
    left_button_->resize(narrow_button_->size());
    right_button_->resize(left_button_->size());
    enlarge_button_->move(this->width() - enlarge_button_->width() - 119 * GetScaleW(), 4 * GetScaleH());
    narrow_button_->move(enlarge_button_->x(), this->height() - narrow_button_->height() - 4 * GetScaleH());
    left_button_->move(0, this->height() / 2);
    right_button_->move(this->width() - right_button_->width() - 7 * GetScaleW(), left_button_->y());
    ButtonIsHide();
}

void CodeEdit::MvStart()
{
    this->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}

QString CodeEdit::GetMod()
{
    return mod_name_;
}

void CodeEdit::ShowAddCode()
{
    enlarge_button_->hide();
    narrow_button_->hide();
}

void CodeEdit::HideAddCode()
{
    enlarge_button_->show();
    narrow_button_->show();
}

void CodeEdit::SetData(QString mod_name)
{
    SaveData();

    mod_name_ = mod_name;

    if(isshow_)
    {
        ModuleData &md = EditData::instance()->GetModData(mod_name);
        row_main_ = md.mod_code.size() + 1;
    }
    else
    {
        row_main_ = -1;
    }

    this->setText(EditData::instance()->ExportMod(mod_name, isshow_));

    ModCodeRow();
}

void CodeEdit::Refresh(bool isupdate)
{
    if(!isupdate)
    {
        int value = this->GetScrollBar()->value();

        this->SetData(mod_name_);

        this->GetScrollBar()->setValue(value);
        code_row_->verticalScrollBar()->setValue(value);
    }
    else
    {
        this->SetData(mod_name_);
    }
}

QPair<QString, int> CodeEdit::Convert(int row)
{
    QString rout_name;
    int rout_num;

    if(row <= row_main_)
        return qMakePair(rout_name, row);

    QTextDocument *docu = this->document();

    for(int i = row; i > row_main_ - 1; i--)
    {
        QString code = docu->findBlockByLineNumber(i).text();
        code.replace("\t", "").replace("\n", "");
        QString head = code.section(" ", 0, 0);

        if(head == "FUNC" || code.indexOf("LOCAL FUNC") != -1)
        {
            if(head == "FUNC")
            {
                QString temp = code.section(" ", 2, 2);
                rout_name = temp.mid(0, temp.indexOf("("));
            }
            else
            {
                QString temp = code.section(" ", 3, 3);
                rout_name = temp.mid(0, temp.indexOf("("));
            }

            return qMakePair(rout_name, row - i - 1);
        }
        else if(head == "PROC" || code.indexOf("LOCAL PROC") != -1)
        {
            if(head == "PROC")
            {
                QString temp = code.section(" ", 1, 1);
                rout_name = temp.mid(0, temp.indexOf("("));
            }
            else
            {
                QString temp = code.section(" ", 2, 2);
                rout_name = temp.mid(0, temp.indexOf("("));
            }

            return qMakePair(rout_name, row - i - 1);
        }
        else if(head == "TRAP" || code.indexOf("LOCAL TRAP") != -1)
        {
            if(head == "TRAP")
            {
                rout_name = code.section(" ", 1, 1);
            }
            else
            {
                rout_name = code.section(" ", 2, 2);
            }

            return qMakePair(rout_name, row - i - 1);
        }
    }

    return qMakePair(rout_name, rout_num);
}

int CodeEdit::Convert(QString code)
{
    QTextDocument *docu = this->document();

    for(int i = 0; i < docu->lineCount(); i++)
    {
        QString temp_code = docu->findBlockByLineNumber(i).text();
        temp_code.replace("\t", "").replace("\n", "");

        if(code == temp_code)
            return i;
    }

    return -1;
}

int CodeEdit::Convert(QString rout_name, int row)
{
    auto *docu = this->document();

    for(int i = 0; i < docu->lineCount(); i++)
    {
        auto code = docu->findBlockByLineNumber(i).text();
        code.replace("\t", "").replace("\n", "");
        auto head = code.section(" ", 0, 0);

        if(head == "LOCAL")
        {
            code = code.mid(head.size() + 1);
            head = code.section(" ", 0, 0);
        }
        if(head == "PROC" || head == "FUNC" || head == "TRAP")
        {
            auto name = code.section(" ", 1, 1);
            if(head == "FUNC")
                name = code.section(" ", 2, 2);
            name = name.mid(0, name.indexOf("("));
            if(name == rout_name)
                return i + row + 1;
        }
    }

    return -1;
}

void CodeEdit::SetCodeRow(CodeRow *point)
{
    code_row_ = point;

    QFont font(this->font());
    code_row_->SetFont(font);
}

void CodeEdit::SetEditWindow(EditWindow *point)
{
    edit_window_ = point;
}

void CodeEdit::SetDebugWindow(DebugWindow *point)
{
    debug_window_ = point;
}

void CodeEdit::SetCopyStr(QString *point)
{
    copy_str_ = point;
}

void CodeEdit::SetSelect(int row)
{
    if(row < 0)
        return;

    auto sor = this->textCursor();
    auto *docu = sor.document();
    if(row >= docu->blockCount())
        return;
    auto block = docu->findBlockByLineNumber(row);
    auto line = block.text().replace("\t", "").replace("\n", "");

    auto head = line.section(" ", 0, 0);
    if(head.mid(0, 3) == "END")
    {
        return;
    }
    else if(head == "ELSE" || head == "ELSEIF")
    {
        int num = row;
        int start = 0;
        bool isok = false;

        for(; num > 0; num--)
        {
            auto temp_block = docu->findBlockByLineNumber(num);
            auto str = temp_block.text().replace("\t", "").replace("\n", "");
            auto head = str.section(" ", 0, 0);

            if(head == "IF")
            {
                start = temp_block.position() + (temp_block.text().size() - str.size());
                isok = true;
                break;
            }
        }

        if(isok)
        {
            for(int n = num + 1; n < this->document()->blockCount(); n++)
            {
                auto next_block = this->document()->findBlockByLineNumber(n);
                auto str = next_block.text().replace("\t", "").replace("\n", "");
                auto head = str.section(" ", 0, 0);

                if(head.size() > 3)
                {
                    if(head.mid(0, 3) == "END")
                    {
                        int end = next_block.position() + next_block.text().size();
                        sor.setPosition(start, QTextCursor::MoveAnchor);
                        sor.setPosition(end, QTextCursor::KeepAnchor);

                        old_select_ = sor.selectedText();
                        old_position_ = sor.position();

                        this->setTextCursor(sor);

                        start_row_ = num;
                        end_row_ = n - start_row_;

                        return;
                    }
                }
            }
        }
    }
    else if(head == "CASE" || head == "DEFAULT:" || head == "DEFAULT")
    {
        int num = row;
        int start = 0;
        bool isok = false;

        for(; num > 0; num--)
        {
            auto temp_block = docu->findBlockByLineNumber(num);
            auto str = temp_block.text().replace("\t", "").replace("\n", "");
            auto str_head = str.section(" ", 0, 0);

            if(str_head == "TEST")
            {
                start = temp_block.position() + (temp_block.text().size() - str.size());
                isok = true;
                break;
            }
        }

        if(isok)
        {
            for(int n = num + 1; n < docu->blockCount(); n++)
            {
                auto next_block = docu->findBlockByLineNumber(n);
                auto str = next_block.text().replace("\t", "").replace("\n", "");
                auto str_head = str.section(" ", 0, 0);

                if(str_head.size() > 3)
                {
                    if(str_head.mid(0, 3) == "END")
                    {
                        int end = next_block.position() + next_block.text().size();
                        sor.setPosition(start, QTextCursor::MoveAnchor);
                        sor.setPosition(end, QTextCursor::KeepAnchor);

                        old_select_= sor.selectedText();
                        old_position_ = sor.position();

                        this->setTextCursor(sor);

                        start_row_ = num;
                        end_row_ = n - start_row_;

                        return;
                    }
                }
            }
        }
    }
    else if(head == "FOR" || head == "WHILE" || head == "IF" || head == "TEST")
    {
        int x = 1;
        int num = row;
        int n = num + 1;
        int start = block.position() + (block.text().size() - line.size());

        for(; n < docu->blockCount(); n++)
        {
            auto next_block = this->document()->findBlockByLineNumber(n);
            auto str = next_block.text().replace("\t", "").replace("\n", "");
            auto str_head = str.section(" ", 0, 0);

            if(str_head == "IF" || str_head == "FOR" || str_head == "WHILE" || str_head == "TEST")
            {
                x++;
            }
            else if(str_head.size() > 3)
            {
                if(str_head.mid(0, 3) == "END")
                {
                    x--;

                    if(x == 0)
                    {
                        int end = next_block.position() + next_block.text().size();
                        sor.setPosition(start, QTextCursor::MoveAnchor);
                        sor.setPosition(end, QTextCursor::KeepAnchor);

                        old_select_ = sor.selectedText();
                        old_position_ = sor.position();

                        this->setTextCursor(sor);

                        start_row_ = num;
                        end_row_ = n - start_row_;

                        return;
                    }
                }
            }
        }
    }
    else
    {
        int start = block.position() + (block.text().size() - line.size());
        int end = start + line.size();
        sor.setPosition(end, QTextCursor::MoveAnchor);
        sor.setPosition(start, QTextCursor::KeepAnchor);

        old_select_ = line;
        old_position_ = sor.position();

        start_row_ = sor.blockNumber();
        end_row_ = 0;

        this->setTextCursor(sor);
        return;
    }
}

int CodeEdit::GetStartRow()
{
    return start_row_;
}

int CodeEdit::GetEndRow()
{
    return end_row_;
}

void CodeEdit::AddCode(QString code)
{
    if(TeachState::instance()->GetRunMode() != RunMode::ManualMod)
    {
        QMessageBox::warning(this, tr("提示"), tr("该运行模式下，此操作被禁止"));
        return;
    }
    if(TeachState::instance()->GetRunState())
    {
        QMessageBox::warning(this, tr("提示"), tr("程序正在运行，此操作被禁止"));
        return;
    }
    ModuleData &mod_info = EditData::instance()->GetModData(mod_name_);
    if(mod_info.type) //系统模块不允许修改
        return;

    auto sor = this->textCursor();
    auto block = sor.block();
    if(isshow_)
    {
        if(sor.blockNumber() <= row_main_)
        {
            QMessageBox::warning(this, tr("添加指令"), tr("尝试添加指令") + code + tr("时出现错误。"));
            return;
        }
    }

    auto word = sor.selectedText();
    auto line = block.text().replace("\t", "").replace("\n", "");
    if(word.isEmpty() || line.isEmpty())
        return;
    if(line.section(" ", 0, 0) == "CONST" || line.section(" ", 1, 1) == "VAR")
    {
        QMessageBox::warning(this, tr("添加指令"), tr("尝试添加指令") + code + tr("时出现错误。"));
        return;
    }
    if(start_row_ == -1)
    {
        QMessageBox::about(this, tr("提示"), tr("请选择要添加的位置"));
        return;
    }

    old_code_ = code;
    auto *code_map = EditData::instance()->GetCodeMap();

    if(code == ":=")
    {
        emit ShowInsertExpres(code_map->value(code), "num", false, false);
        return;
    }
    else if(code == "IF")
    {
        emit ShowInsertExpres("<EXP>", "bool", true, false);
        return;
    }
    else if(code == "ProcCall")
    {
        emit ShowAddFunc();
        return;
    }
    else if(code == "Set")
    {
        emit ShowChangeSelect(code_map->value(code));
    }
    else if(code == "Add")
    {
        emit ShowChangeSelect(code_map->value(code));
    }
    else if(code == "PulseDO")
    {
        emit ShowChangeSelect(code_map->value(code));
    }
    else if(code == "SetDO")
    {
        emit ShowChangeSelect(code_map->value(code));
    }
    else if(code == "WaitDI")
    {
        emit ShowChangeSelect(code_map->value(code));
    }
    else if(code == "WaitDO")
    {
        emit ShowChangeSelect(code_map->value(code));
    }
    else if(code == "WaitTime")
    {
        emit ShowChangeSelect(code_map->value(code));
    }
    else
    {
        auto temp_code = code_map->value(code);
        auto move_code = temp_code;

        if(code == "MoveJ" || code == "MoveAbsJ" || code == "MoveC" || code == "MoveL")
        {
            auto head = move_code.section(" ", 0, 0);

            auto motion = RobotClient::initance()->getRobotMotion();
            auto tm = motion.getCurrentTerminal();
            auto tool_name = TeachState::instance()->GetCurrentTool();

            auto x = tm.getValue(TERMINALINDEX::TERMINAL_X);
            auto y = tm.getValue(TERMINALINDEX::TERMINAL_Y);
            auto z = tm.getValue(TERMINALINDEX::TERMINAL_Z);
            Euler er(tm.getValue(TERMINALINDEX::TERMINAL_A), tm.getValue(TERMINALINDEX::TERMINAL_B), tm.getValue(TERMINALINDEX::TERMINAL_C));
            auto var_code = Convert::PositDoubleToPositCode(x, y, z, er).mid(1); //获取坐标值
            var_code = var_code.mid(0, var_code.size() - 1);

            auto var_point = Convert::DataTypeToVarFormData("robtarget");
            auto str1 = var_code.section("],[", 0, 0) + "]";
            auto str2 = "[" + var_code.section("],[", 1, 1);
            var_point->subnode.at(0)->code = str1;
            var_point->subnode.at(1)->code = str2;
            var_point->subnode.at(0)->LastUpdate();
            var_point->subnode.at(1)->LastUpdate();
            var_point->Update();
            var_code = var_point->code;

            QPair<bool, QPair<QString, QString>> iscreatr_info;
            if(head == "MoveAbsJ")
                iscreatr_info = IsCreateVarAbsj(sor.blockNumber());
            else
                iscreatr_info = IsCreateVarMove(sor.blockNumber());

            bool iscreate = iscreatr_info.first;

            if(head == "MoveJ")
            {
                auto var = move_code.section(" ", 1, 1);
                if(var.indexOf("*") != -1)
                {
                    if(iscreate)
                    {
                        auto var_name = VarInfo::instance()->GetUnused("robtarget");

                        auto str_code = "CONST robtarget " + var_name + ":=" + var_code + ";";
                        EditData::instance()->ModAddCode(mod_name_, str_code);

                        VarDataInfo var_info;
                        var_info.var_name = var_name;
                        var_info.data_type = "robtarget";
                        var_info.var = var_code;
                        var_info.nature = 2;
                        var_info.genre = true;
                        var_info.action_scope = "all";
                        var_info.mod = mod_name_;
                        VarInfo::instance()->AddVar(var_name, var_info);

                        move_code.replace(var, var_name + ",");
                    }
                    else
                    {
                        move_code.replace(var, var_code + ",");
                    }
                    move_code.replace("tool0", tool_name);
                    temp_code.replace("tool0", tool_name);
                }

                if(!iscreatr_info.second.first.isEmpty() && !iscreatr_info.second.second.isEmpty()) //当速度与区域参数都不为空时需要修改这条语句的速度和区域参数
                {
                    move_code.replace("v1000,", iscreatr_info.second.first);
                    move_code.replace("z50,", iscreatr_info.second.second);
                    temp_code.replace("v1000,", iscreatr_info.second.first);
                    temp_code.replace("z50,", iscreatr_info.second.second);
                }
            }
            else if(head == "MoveAbsJ")
            {
                auto var = move_code.section(" ", 1, 1);
                if(var.indexOf("*") != -1)
                {
                    auto joint = RobotClient::initance()->getRobotMotion().getCurrentJointPosition(); //获取关节角度
                    QString jo_code = QString("[[%1,%2,%3,%4,%5,%6],[9E+09,9E+09,9E+09,9E+09,9E+09,9E+09]]").arg(joint.getValue(JOINT_1)).arg(joint.getValue(JOINT_2))
                            .arg(joint.getValue(JOINT_3)).arg(joint.getValue(JOINT_4)).arg(joint.getValue(JOINT_5)).arg(joint.getValue(JOINT_6));

                    if(iscreate)
                    {
                        auto var_name = VarInfo::instance()->GetUnused("jointtarget");

                        auto str_code = "CONST jointtarget " + var_name + ":=" + jo_code + ";";
                        EditData::instance()->ModAddCode(mod_name_, str_code);

                        VarDataInfo var_info;
                        var_info.var_name = var_name;
                        var_info.data_type = "jointtarget";
                        var_info.var = jo_code;
                        var_info.nature = 2;
                        var_info.genre = true;
                        var_info.action_scope = "all";
                        var_info.mod = mod_name_;
                        VarInfo::instance()->AddVar(var_name, var_info);

                        move_code.replace(var, var_name + "\\NoEOffs,");
                    }
                    else
                    {
                        move_code.replace(var, jo_code + "\\NoEOffs,");
                    }

                    move_code.replace("tool0", tool_name);
                    temp_code.replace("tool0", tool_name);
                }

                if(!iscreatr_info.second.first.isEmpty() && !iscreatr_info.second.second.isEmpty()) //当速度与区域参数都不为空时需要修改这条语句的速度和区域参数
                {
                    move_code.replace("v1000,", iscreatr_info.second.first);
                    move_code.replace("z50,", iscreatr_info.second.second);
                    temp_code.replace("v1000,", iscreatr_info.second.first);
                    temp_code.replace("z50,", iscreatr_info.second.second);
                }
            }
            else if(head == "MoveC")
            {
                auto var1 = move_code.section(" ", 1, 1);
                auto var2 = move_code.section(" ", 2, 2);

                if(iscreate)
                {
                    auto var_name1 = VarInfo::instance()->GetUnused("robtarget");
                    int num1 = var_name1.mid(1).toInt();
                    auto var_name2 = "p" + QString::number(num1 + 10);

                    auto str_code1 = "CONST robtarget " + var_name1 + ":=" + var_code + ";";
                    auto str_code2 = "CONST robtarget " + var_name2 + ":=" + var_code + ";";
                    EditData::instance()->ModAddCode(mod_name_, str_code1);
                    EditData::instance()->ModAddCode(mod_name_, str_code2);

                    VarDataInfo var_info1;
                    var_info1.var_name = var_name1;
                    var_info1.data_type = "robtarget";
                    var_info1.var = var_code;
                    var_info1.nature = 2;
                    var_info1.genre = true;
                    var_info1.action_scope = "all";
                    var_info1.mod = mod_name_;
                    VarInfo::instance()->AddVar(var_name1, var_info1);
                    VarDataInfo var_info2;
                    var_info2.var_name = var_name2;
                    var_info2.data_type = "robtarget";
                    var_info2.var = var_code;
                    var_info2.nature = 2;
                    var_info2.genre = true;
                    var_info2.action_scope = "all";
                    var_info2.mod = mod_name_;
                    VarInfo::instance()->AddVar(var_name2, var_info2);

                    move_code.replace(6, 2, var_name1 + ",");
                    move_code.replace(var2, var_name2 + ",");
                }
                else
                {
                    move_code.replace(var1, var_code + ",");
                }
                move_code.replace("tool0", tool_name);
                temp_code.replace("tool0", tool_name);

                if(!iscreatr_info.second.first.isEmpty()) //MoveC只修改速度参数，区域参数不动
                {
                    move_code.replace("v1000,", iscreatr_info.second.first);
                    temp_code.replace("v1000,", iscreatr_info.second.first);
                }
            }
            else if(head == "MoveL")
            {
                auto var = temp_code.section(" ", 1, 1);
                if(var.indexOf("*") != -1)
                {
                    if(iscreate)
                    {
                        auto var_name = VarInfo::instance()->GetUnused("robtarget");

                        auto str_code = "CONST robtarget " + var_name + ":=" + var_code + ";";
                        EditData::instance()->ModAddCode(mod_name_, str_code);

                        VarDataInfo var_info;
                        var_info.var_name = var_name;
                        var_info.data_type = "robtarget";
                        var_info.var = var_code;
                        var_info.nature = 2;
                        var_info.genre = true;
                        var_info.action_scope = "all";
                        var_info.mod = mod_name_;
                        VarInfo::instance()->AddVar(var_name, var_info);

                        move_code.replace(var, var_name + ",");
                    }
                    else
                    {
                        move_code.replace(var, var_code + ",");
                    }
                    move_code.replace("tool0", tool_name);
                    temp_code.replace("tool0", tool_name);
                }

                if(!iscreatr_info.second.first.isEmpty() && !iscreatr_info.second.second.isEmpty()) //当速度与区域参数都不为空时需要修改这条语句的速度和区域参数
                {
                    move_code.replace("v1000,", iscreatr_info.second.first);
                    move_code.replace("z50,", iscreatr_info.second.second);
                    temp_code.replace("v1000,", iscreatr_info.second.first);
                    temp_code.replace("z50,", iscreatr_info.second.second);
                }
            }
        }

        bool ismsg = false;

        auto upper_block = this->document()->findBlockByLineNumber(start_row_ - 1);
        auto upper_str = upper_block.text().replace("\t", "").replace("\n", "");
        if(!upper_str.isEmpty())
        {
            auto head = upper_str.section(" ", 0, 0);
            if(head == "LOCAL" || head == "PROC" || head == "FUNC" || head == "TRAP" || head == "CONST" || head == "VAR")
            {
                ismsg = true;
            }
        }

        bool isnext = true;

        if(ismsg)
        {
            QMessageBox mess(QMessageBox::Question, tr("添加指令"), tr("是否需要在当前选定的项目之上或之下插入指令?")); //会出现arm平台中无法弹出问题
            mess.addButton(tr("上方"), QMessageBox::ActionRole);
            mess.addButton(tr("下方"), QMessageBox::ActionRole);
            mess.addButton(tr("取消"), QMessageBox::ActionRole);
            auto msg = mess.exec();

            if(msg == 0)
                isnext = false;
            else if(msg == 1)
                isnext = true;
            else
                return;
        }

        if(isnext) //下方插入
        {
            auto rout_info = this->Convert(start_row_ + end_row_ + 1);

            if(code == "MoveJ" || code == "MoveAbsJ" || code == "MoveC" || code == "MoveL")
                EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, move_code);
            else
                EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, temp_code);
        }
        else //上方插入
        {
            auto rout_info = this->Convert(start_row_);

            if(code == "MoveJ" || code == "MoveAbsJ" || code == "MoveC" || code == "MoveL")
                EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, move_code);
            else
                EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, temp_code);
        }

        int pos_val = GetScrollBar()->value(); //保存滚动条当前位置

        this->SetData(mod_name_);

        GetScrollBar()->setValue(pos_val);
        code_row_->verticalScrollBar()->setValue(pos_val);

        old_select_.clear();
        old_code_.clear();
        old_position_ = start_row_ = end_row_ = -1;
    }
}

void CodeEdit::ChangeShow(int row, bool iskeep, int num, int size)
{
    QTextCursor sor = this->textCursor();
    int start = this->document()->findBlockByNumber(row).position() + num; //获取要显示的指令的坐标
    if(iskeep) //需要选中
    {
        int end = start + size;

        sor.setPosition(start, QTextCursor::MoveAnchor);
        sor.setPosition(end, QTextCursor::KeepAnchor);
    }
    else
    {
        sor.setPosition(start, QTextCursor::MoveAnchor);
    }
    this->setTextCursor(sor);
    code_row_->MvPos(row);
}

void CodeEdit::RetInsertExpres(QString data, bool rep)
{
    if(!rep) //添加
    {
        if(old_code_ == "IF")
        {
            data = "IF " + data + " THEN\n<SMT>\nENDIF";
        }

        bool ismsg = false;

        auto upper_block = this->document()->findBlockByLineNumber(start_row_ - 1);
        auto upper_str = upper_block.text().replace("\t", "").replace("\n", "");
        if(!upper_str.isEmpty())
        {
            auto head = upper_str.section(" ", 0, 0);
            if(head == "LOCAL" || head == "PROC" || head == "FUNC" || head == "TRAP" || head == "CONST" || head == "VAR")
            {
                ismsg = true;
            }
        }

        bool isnext = true;

        if(ismsg)
        {
            QMessageBox mess(QMessageBox::Question, tr("添加指令"), tr("是否需要在当前选定的项目之上或之下插入指令?")); //会出现arm平台中无法弹出问题
            mess.addButton(tr("上方"), QMessageBox::ActionRole);
            mess.addButton(tr("下方"), QMessageBox::ActionRole);
            mess.addButton(tr("取消"), QMessageBox::ActionRole);
            auto msg = mess.exec();

            if(msg == 0)
                isnext = false;
            else if(msg == 1)
                isnext = true;
            else
                return;
        }

        auto code = data.section(" ", 0, 0);
        if(isnext) //下方插入
        {
            auto rout_info = this->Convert(start_row_ + end_row_ + 1);

            EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, data);
        }
        else //上方插入
        {
            auto rout_info = this->Convert(start_row_);

            EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, data);
        }

        int pos_val = GetScrollBar()->value(); //保存滚动条当前位置

        this->SetData(mod_name_);

        GetScrollBar()->setValue(pos_val);
        code_row_->verticalScrollBar()->setValue(pos_val);

        old_select_.clear();
        old_code_.clear();
        old_position_ = start_row_ = end_row_ = -1;
    }
    else //替换
    {
        auto sor = this->textCursor();
        sor.insertText(data);

        auto text = sor.block().text().replace("\t", "").replace("\n", "");

        auto info = this->Convert(sor.blockNumber());
        EditData::instance()->RoutRemCode(info.first, info.second, text);

        this->setTextCursor(sor);
    }
}

void CodeEdit::RetAddFunc(QString data)
{
    bool ismsg = false;

    auto upper_block = this->document()->findBlockByLineNumber(start_row_ - 1);
    auto upper_str = upper_block.text().replace("\t", "").replace("\n", "");
    if(!upper_str.isEmpty())
    {
        auto head = upper_str.section(" ", 0, 0);
        if(head == "LOCAL" || head == "PROC" || head == "FUNC" || head == "TRAP" || head == "CONST" || head == "VAR")
        {
            ismsg = true;
        }
    }

    bool isnext = true;

    if(ismsg)
    {
        QMessageBox mess(QMessageBox::Question, tr("添加指令"), tr("是否需要在当前选定的项目之上或之下插入指令?")); //会出现arm平台中无法弹出问题
        mess.addButton(tr("上方"), QMessageBox::ActionRole);
        mess.addButton(tr("下方"), QMessageBox::ActionRole);
        mess.addButton(tr("取消"), QMessageBox::ActionRole);
        auto msg = mess.exec();

        if(msg == 0)
            isnext = false;
        else if(msg == 1)
            isnext = true;
        else
            return;
    }

    auto code = data.section(" ", 0, 0);
    if(isnext) //下方插入
    {
        auto rout_info = this->Convert(start_row_ + end_row_ + 1);

        EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, data);
    }
    else //上方插入
    {
        auto rout_info = this->Convert(start_row_);

        EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, data);
    }

    int pos_val = GetScrollBar()->value(); //保存滚动条当前位置

    this->SetData(mod_name_);

    GetScrollBar()->setValue(pos_val);
    code_row_->verticalScrollBar()->setValue(pos_val);

    old_select_.clear();
    old_code_.clear();
    old_position_ = start_row_ = end_row_ = -1;
}

void CodeEdit::CreateVar(QString mod_name)
{
    if(mod_name != mod_name_)
    {
        return;
    }

    auto sor = this->textCursor(); //获取选中光标
    auto block = sor.block(); //获取光标所在段落

    auto select_str = sor.selectedText();
    auto line_str = block.text();
    int row = sor.blockNumber(); //获取当前所在行数
    int all_size = this->document()->blockCount(); //获取当前总行数

    int srp = sor.position();
    int bkp = block.position();
    int num = srp - bkp - select_str.size();
    int pos_val = GetScrollBar()->value(); //保存滚动条当前位置

    this->SetData(mod_name_);

    int new_all_size = this->document()->blockCount(); //获取新的总行数
    row += new_all_size - all_size; //获取当前所在的新行数

    if(!select_str.isEmpty())
    {
        this->ChangeShow(row, true, num, select_str.size());
    }
    else
    {
        line_str.replace("\t", "");
        this->ChangeShow(row, true, num, line_str.size());
    }

    GetScrollBar()->setValue(pos_val); //切换回之前的位置
    code_row_->verticalScrollBar()->setValue(pos_val);
}

void CodeEdit::ChangeSelect(QString data)
{
    bool ismsg = false;

    auto upper_block = this->document()->findBlockByLineNumber(start_row_ - 1);
    auto upper_str = upper_block.text().replace("\t", "").replace("\n", "");
    if(!upper_str.isEmpty())
    {
        auto head = upper_str.section(" ", 0, 0);
        if(head == "LOCAL" || head == "PROC" || head == "FUNC" || head == "TRAP" || head == "CONST" || head == "VAR")
        {
            ismsg = true;
        }
    }

    bool isnext = true;

    if(ismsg)
    {
        QMessageBox mess(QMessageBox::Question, tr("添加指令"), tr("是否需要在当前选定的项目之上或之下插入指令?")); //会出现arm平台中无法弹出问题
        mess.addButton(tr("上方"), QMessageBox::ActionRole);
        mess.addButton(tr("下方"), QMessageBox::ActionRole);
        mess.addButton(tr("取消"), QMessageBox::ActionRole);
        auto msg = mess.exec();

        if(msg == 0)
            isnext = false;
        else if(msg == 1)
            isnext = true;
        else
            return;
    }

    auto code = data.section(" ", 0, 0);
    if(isnext) //下方插入
    {
        auto rout_info = this->Convert(start_row_ + end_row_ + 1);

        EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, data);
    }
    else //上方插入
    {
        auto rout_info = this->Convert(start_row_);

        EditData::instance()->RoutAddCode(rout_info.first, rout_info.second, data);
    }

    int pos_val = GetScrollBar()->value(); //保存滚动条当前位置

    this->SetData(mod_name_);

    GetScrollBar()->setValue(pos_val);
    code_row_->verticalScrollBar()->setValue(pos_val);

    old_select_.clear();
    old_code_.clear();
    old_position_ = start_row_ = end_row_ = -1;
}

void CodeEdit::mousePressEvent(QMouseEvent *e)
{
    QTextBrowser::mousePressEvent(e);

    if(e->button() == Qt::LeftButton)
    {
        bool isenable = false;
        if(TeachState::instance()->GetRunMode() == RunMode::ManualMod && !TeachState::instance()->GetRunState())
        {
            isenable = true;
        }

        auto sor = this->textCursor();
        auto block = sor.block();

        auto word = sor.selectedText();
        auto line = block.text().replace("\t", "").replace("\n", "");
        if(line.isEmpty())
        {
            return;
        }

        if(word.isEmpty())
        {
            sor.clearSelection();
            sor.select(QTextCursor::WordUnderCursor);
            word = sor.selectedText();
        }

        if(word == "THEN" || word == "ELSEIF" || word == "ELSE") //IF
        {
            int num = sor.blockNumber();
            int start = 0;
            bool isok = false;

            for(; num > 0; num--)
            {
                auto temp_block = this->document()->findBlockByLineNumber(num);
                auto str = temp_block.text().replace("\t", "").replace("\n", "");
                auto head = str.section(" ", 0, 0);

                if(head == "IF")
                {
                    start = temp_block.position() + (temp_block.text().size() - str.size());
                    isok = true;
                    break;
                }
            }

            if(isok)
            {
                for(int n = num + 1; n < this->document()->blockCount(); n++)
                {
                    auto next_block = this->document()->findBlockByLineNumber(n);
                    auto str = next_block.text().replace("\t", "").replace("\n", "");
                    auto head = str.section(" ", 0, 0);

                    if(head.size() > 3)
                    {
                        if(head.mid(0, 3) == "END")
                        {
                            int end = next_block.position() + next_block.text().size();
                            sor.setPosition(start, QTextCursor::MoveAnchor);
                            sor.setPosition(end, QTextCursor::KeepAnchor);

                            old_select_ = sor.selectedText();
                            old_position_ = sor.position();

                            this->setTextCursor(sor);
                            if(isenable)
                            {
                                edit_window_->shear_button_.setEnabled(true);
                                edit_window_->copy_button_.setEnabled(true);
                                edit_window_->chang_data_button_.setEnabled(true);
                                edit_window_->del_button_.setEnabled(true);
                                edit_window_->remarks_button_.setEnabled(true);

                                if(!copy_str_->isEmpty())
                                {
                                    edit_window_->stick_button_.setEnabled(true);
                                    edit_window_->upper_stick_button_.setEnabled(true);
                                }
                            }

                            start_row_ = num;
                            end_row_ = n - start_row_;

                            return;
                        }
                    }
                }
            }
        }
        else if(word == "FROM" || word == "TO" || word == "STEP" || word == "DO") //FOR WHILE
        {
            if(line.section(" ", 0, 0) == "FOR" || line.section(" ", 0, 0) == "WHILE")
            {
                int x = 1;
                int num = sor.blockNumber();
                int n = num + 1;
                int start = block.position() + (block.text().size() - line.size());

                for(; n < this->document()->blockCount(); n++)
                {
                    auto next_block = this->document()->findBlockByLineNumber(n);
                    auto str = next_block.text().replace("\t", "").replace("\n", "");
                    auto head = str.section(" ", 0, 0);

                    if(head == "IF" || head == "FOR" || head == "WHILE")
                    {
                        x++;
                    }
                    else if(head.size() > 3)
                    {
                        if(head.mid(0, 3) == "END")
                        {
                            x--;

                            if(x == 0)
                            {
                                int end = next_block.position() + next_block.text().size();
                                sor.setPosition(start, QTextCursor::MoveAnchor);
                                sor.setPosition(end, QTextCursor::KeepAnchor);

                                old_select_ = sor.selectedText();
                                old_position_ = sor.position();

                                this->setTextCursor(sor);
                                if(isenable)
                                {
                                    edit_window_->shear_button_.setEnabled(true);
                                    edit_window_->copy_button_.setEnabled(true);
                                    edit_window_->chang_data_button_.setEnabled(true);
                                    edit_window_->del_button_.setEnabled(true);
                                    edit_window_->remarks_button_.setEnabled(true);

                                    if(!copy_str_->isEmpty())
                                    {
                                        edit_window_->stick_button_.setEnabled(true);
                                        edit_window_->upper_stick_button_.setEnabled(true);
                                    }
                                }

                                start_row_ = num;
                                end_row_ = n - start_row_;

                                return;
                            }
                        }
                    }
                }
            }
        }
        else if(line.section(" ", 0, 0) == "FOR")
        {
            auto for_str = line.section(" ", 1, 1);
            if(word == "ID" && for_str.size() == 4)
            {
                if(for_str.at(0) == '<' && for_str.at(3) == '>')
                {
                    int start = sor.position() - 3;
                    int end = sor.position() + 1;

                    sor.setPosition(start, QTextCursor::MoveAnchor);
                    sor.setPosition(end, QTextCursor::KeepAnchor);

                    old_select_ = sor.selectedText();
                    old_position_ = sor.position();

                    this->setTextCursor(sor);
                    if(isenable)
                    {
                        edit_window_->shear_button_.setEnabled(true);
                        edit_window_->copy_button_.setEnabled(true);
                        edit_window_->chang_data_button_.setEnabled(true);
                        edit_window_->del_button_.setEnabled(true);
                        edit_window_->remarks_button_.setEnabled(true);

                        if(!copy_str_->isEmpty())
                        {
                            edit_window_->stick_button_.setEnabled(true);
                            edit_window_->upper_stick_button_.setEnabled(true);
                        }
                    }

                    start_row_ = sor.blockNumber();
                    end_row_ = 0;

                    return;
                }
            }
        }
        else if(word == "CASE" || word == "DEFAULT:" || word == "DEFAULT")
        {
            int num = sor.blockNumber();
            int start = 0;
            bool isok = false;

            for(; num > 0; num--)
            {
                auto temp_block = this->document()->findBlockByLineNumber(num);
                auto str = temp_block.text().replace("\t", "").replace("\n", "");
                auto head = str.section(" ", 0, 0);

                if(head == "TEST")
                {
                    start = temp_block.position() + (temp_block.text().size() - str.size());
                    isok = true;
                    break;
                }
            }

            if(isok)
            {
                for(int n = num + 1; n < this->document()->blockCount(); n++)
                {
                    auto next_block = this->document()->findBlockByLineNumber(n);
                    auto str = next_block.text().replace("\t", "").replace("\n", "");
                    auto head = str.section(" ", 0, 0);

                    if(head.size() > 3)
                    {
                        if(head.mid(0, 3) == "END")
                        {
                            int end = next_block.position() + next_block.text().size();
                            sor.setPosition(start, QTextCursor::MoveAnchor);
                            sor.setPosition(end, QTextCursor::KeepAnchor);

                            old_select_ = sor.selectedText();
                            old_position_ = sor.position();

                            this->setTextCursor(sor);
                            if(isenable)
                            {
                                edit_window_->shear_button_.setEnabled(true);
                                edit_window_->copy_button_.setEnabled(true);
                                edit_window_->chang_data_button_.setEnabled(true);
                                edit_window_->del_button_.setEnabled(true);
                                edit_window_->remarks_button_.setEnabled(true);

                                if(!copy_str_->isEmpty())
                                {
                                    edit_window_->stick_button_.setEnabled(true);
                                    edit_window_->upper_stick_button_.setEnabled(true);
                                }
                            }

                            start_row_ = num;
                            end_row_ = n - start_row_;

                            return;
                        }
                    }
                }
            }
        }

        auto *code_map = EditData::instance()->GetCodeMap();

        if(code_map->contains(word) && word == line.section(" ", 0, 0)) //点击的是指令头部
        {
            if(word == "IF" || word == "FOR" || word == "WHILE" || word == "TEST")
            {
                int x = 1, n = sor.blockNumber() + 1, num = sor.blockNumber();

                for(; n < this->document()->blockCount(); n++)
                {
                    auto next_block = this->document()->findBlockByLineNumber(n);
                    auto str = next_block.text().replace("\t", "").replace("\n", "");
                    auto head = str.section(" ", 0, 0);

                    if(head == "IF" || head == "FOR" || head == "WHILE")
                    {
                        x++;
                    }
                    else if(head.size() > 3)
                    {
                        if(head.mid(0, 3) == "END")
                        {
                            x--;

                            if(x == 0)
                            {
                                int start = sor.position() - word.size();
                                int end = next_block.position() + next_block.text().size();
                                sor.setPosition(start, QTextCursor::MoveAnchor);
                                sor.setPosition(end, QTextCursor::KeepAnchor);

                                old_select_ = sor.selectedText();
                                old_position_ = sor.position();
                                if(isenable)
                                {
                                    edit_window_->shear_button_.setEnabled(true);
                                    edit_window_->copy_button_.setEnabled(true);
                                    edit_window_->chang_data_button_.setEnabled(true);
                                    edit_window_->del_button_.setEnabled(true);
                                    edit_window_->remarks_button_.setEnabled(true);

                                    if(!copy_str_->isEmpty())
                                    {
                                        edit_window_->stick_button_.setEnabled(true);
                                        edit_window_->upper_stick_button_.setEnabled(true);
                                    }
                                }

                                this->setTextCursor(sor);

                                start_row_ = num;
                                end_row_ = n - start_row_;

                                return;
                            }
                        }
                    }
                }
            }
            else
            {
                int start = sor.position() - word.size();
                int end = start + line.size();
                sor.setPosition(end, QTextCursor::MoveAnchor);
                sor.setPosition(start, QTextCursor::KeepAnchor);

                old_select_ = line;
                old_position_ = sor.position();

                start_row_ = sor.blockNumber();
                end_row_ = 0;

                if(isenable)
                {
                    edit_window_->shear_button_.setEnabled(true);
                    edit_window_->copy_button_.setEnabled(true);
                    edit_window_->chang_data_button_.setEnabled(true);
                    edit_window_->del_button_.setEnabled(true);
                    edit_window_->remarks_button_.setEnabled(true);

                    if(!copy_str_->isEmpty())
                    {
                        edit_window_->stick_button_.setEnabled(true);
                        edit_window_->upper_stick_button_.setEnabled(true);
                    }
                }
            }
        }
        else if(word == "," || word == ";" || word == ":" || word == ">:")
        {
            old_select_.clear();
            old_position_ = -1;
            start_row_ = end_row_ = old_position_;

            if(isenable)
            {
                edit_window_->shear_button_.setEnabled(false);
                edit_window_->copy_button_.setEnabled(false);
                edit_window_->chang_data_button_.setEnabled(false);
                edit_window_->del_button_.setEnabled(false);
                edit_window_->remarks_button_.setEnabled(false);
                edit_window_->stick_button_.setEnabled(false);
                edit_window_->upper_stick_button_.setEnabled(false);
            }

            return;
        }
        else
        {
            auto head = line.section(" ", 0, 0);

            if(isshow_)
            {
                if(sor.blockNumber() < row_main_)
                {
                    if(head == "MODULE")
                    {
                        sor.clearSelection();
                    }
                    else
                    {
                        int start = block.position() + block.text().size() - line.size();
                        int end = start + line.size();

                        sor.setPosition(end, QTextCursor::MoveAnchor);
                        sor.setPosition(start, QTextCursor::KeepAnchor);
                    }

                    this->setTextCursor(sor);
                    if(isenable)
                    {
                        edit_window_->shear_button_.setEnabled(false);
                        edit_window_->copy_button_.setEnabled(false);
                        edit_window_->chang_data_button_.setEnabled(false);
                        edit_window_->del_button_.setEnabled(false);
                        edit_window_->remarks_button_.setEnabled(false);
                        edit_window_->stick_button_.setEnabled(false);
                        edit_window_->upper_stick_button_.setEnabled(false);
                    }

                    old_select_.clear();
                    old_position_ = -1;
                    start_row_ = end_row_ = old_position_;
                    return;
                }
            }

            if(word == "<" || word == ">")
            {
                auto str = block.text();
                int pos = sor.position() - block.position() - 1;

                int start = 0, end = 0;
                if(str.at(pos - 1) != ' ' || str.at(pos + 1) != ' ')
                {
                    if(word == "<")
                    {
                        start = block.position() + pos;
                        end = start + 5;
                    }
                    else
                    {
                        end = block.position() + pos + 1;
                        start = end - 5;
                    }

                    sor.setPosition(end, QTextCursor::MoveAnchor);
                    sor.setPosition(start, QTextCursor::KeepAnchor);

                    old_select_ = sor.selectedText();
                    old_position_ = sor.position();
                    start_row_ = sor.blockNumber();
                    end_row_ = 0;

                    this->setTextCursor(sor);
                    if(isenable)
                    {
                        edit_window_->shear_button_.setEnabled(true);
                        edit_window_->copy_button_.setEnabled(true);
                        edit_window_->chang_data_button_.setEnabled(true);
                        edit_window_->del_button_.setEnabled(true);
                        edit_window_->remarks_button_.setEnabled(true);

                        if(!copy_str_->isEmpty())
                        {
                            edit_window_->stick_button_.setEnabled(true);
                            edit_window_->upper_stick_button_.setEnabled(true);
                        }
                    }

                    return;
                }
            }

            if(head == "MoveAbsJ")
            {
                if(word == "NoEoffs" || word == "\\")
                {
                    int start = block.position() + block.text().size() - line.size();
                    int end = start + line.size();

                    sor.setPosition(end, QTextCursor::MoveAnchor);
                    sor.setPosition(start, QTextCursor::KeepAnchor);

                    old_select_ = sor.selectedText();
                    old_position_ = sor.position();
                    start_row_ = sor.blockNumber();
                    end_row_ = 0;

                    this->setTextCursor(sor);
                    if(isenable)
                    {
                        edit_window_->shear_button_.setEnabled(true);
                        edit_window_->copy_button_.setEnabled(true);
                        edit_window_->chang_data_button_.setEnabled(true);
                        edit_window_->del_button_.setEnabled(true);
                        edit_window_->remarks_button_.setEnabled(true);

                        if(!copy_str_->isEmpty())
                        {
                            edit_window_->stick_button_.setEnabled(true);
                            edit_window_->upper_stick_button_.setEnabled(true);
                        }
                    }

                    return;
                }
                else if(word == "*\\")
                {
                    int start = sor.position() - 2;
                    int end = sor.position() - 1;

                    sor.setPosition(start, QTextCursor::MoveAnchor);
                    sor.setPosition(end, QTextCursor::KeepAnchor);

                    old_select_ = sor.selectedText();
                    old_position_ = sor.position();
                    start_row_ = sor.blockNumber();
                    end_row_ = 0;

                    this->setTextCursor(sor);
                    if(isenable)
                    {
                        edit_window_->shear_button_.setEnabled(true);
                        edit_window_->copy_button_.setEnabled(true);
                        edit_window_->chang_data_button_.setEnabled(true);
                        edit_window_->del_button_.setEnabled(true);
                        edit_window_->remarks_button_.setEnabled(true);

                        if(!copy_str_->isEmpty())
                        {
                            edit_window_->stick_button_.setEnabled(true);
                            edit_window_->upper_stick_button_.setEnabled(true);
                        }
                    }

                    return;
                }
            }

            if(head == "LOCAL" || head == "PROC" || head == "FUNC" || head == "TRAP" || head == "ENDMODULE" || head == "ENDPROC" || head == "ENDFUNC" || head == "ENDTRAP")
            {
                sor.clearSelection();

                this->setTextCursor(sor);
                if(isenable)
                {
                    edit_window_->shear_button_.setEnabled(false);
                    edit_window_->copy_button_.setEnabled(false);
                    edit_window_->chang_data_button_.setEnabled(false);
                    edit_window_->del_button_.setEnabled(false);
                    edit_window_->remarks_button_.setEnabled(false);
                    edit_window_->stick_button_.setEnabled(false);
                    edit_window_->upper_stick_button_.setEnabled(false);
                }

                old_select_.clear();
                old_position_ = -1;
                start_row_ = end_row_ = old_position_;
                return;
            }
            else if(head == "CONST" || head == "VAR")
            {
                int start = block.position() + (block.text().size() - line.size());
                int end = start + line.size();

                sor.setPosition(end, QTextCursor::MoveAnchor);
                sor.setPosition(start, QTextCursor::KeepAnchor);

                old_select_.clear();
                old_position_ = -1;
                start_row_ = end_row_ = -1;

                this->setTextCursor(sor);
                if(isenable)
                {
                    edit_window_->shear_button_.setEnabled(false);
                    edit_window_->copy_button_.setEnabled(false);
                    edit_window_->chang_data_button_.setEnabled(false);
                    edit_window_->del_button_.setEnabled(false);
                    edit_window_->remarks_button_.setEnabled(false);
                    edit_window_->stick_button_.setEnabled(false);
                    edit_window_->upper_stick_button_.setEnabled(false);
                }

                return;
            }
            else if(word == old_select_ && sor.position() == old_position_) //双击
            {
                if(word.isEmpty())
                    return;

                this->setTextCursor(sor);

                emit DoubleClick();

                return;
            }
            else if(line.at(0) == '!') //备注
            {
                int start = block.position() + block.text().size() - line.size();
                int end = start + line.size();
                sor.setPosition(end, QTextCursor::MoveAnchor);
                sor.setPosition(start, QTextCursor::KeepAnchor);

                old_select_ = line;
                old_position_ = sor.position();
                start_row_ = sor.blockNumber();
                end_row_ = 0;

                if(isenable)
                {
                    edit_window_->shear_button_.setEnabled(true);
                    edit_window_->copy_button_.setEnabled(true);
                    edit_window_->chang_data_button_.setEnabled(true);
                    edit_window_->del_button_.setEnabled(true);
                    edit_window_->remarks_button_.setEnabled(true);

                    if(!copy_str_->isEmpty())
                    {
                        edit_window_->stick_button_.setEnabled(true);
                        edit_window_->upper_stick_button_.setEnabled(true);
                    }
                }
            }
            else if(word == ":=" || word == "{" || word == "}")
            {
                int start = block.position() + (block.text().size() - line.size());
                int end = start + line.size();

                sor.setPosition(end, QTextCursor::MoveAnchor);
                sor.setPosition(start, QTextCursor::KeepAnchor);

                this->setTextCursor(sor);
                if(isenable)
                {
                    edit_window_->shear_button_.setEnabled(true);
                    edit_window_->copy_button_.setEnabled(true);
                    edit_window_->chang_data_button_.setEnabled(true);
                    edit_window_->del_button_.setEnabled(true);
                    edit_window_->remarks_button_.setEnabled(true);

                    if(!copy_str_->isEmpty())
                    {
                        edit_window_->stick_button_.setEnabled(true);
                        edit_window_->upper_stick_button_.setEnabled(true);
                    }
                }

                old_select_ = line;
                old_position_ = sor.position();
                start_row_ = sor.blockNumber();
                end_row_ = 0;
                return;
            }
            else if(word == "EXP" || word == "VAR" || word == "SMT")
            {
                int start = sor.position() - 4;
                int end = sor.position() + 1;

                sor.setPosition(start, QTextCursor::MoveAnchor);
                sor.setPosition(end, QTextCursor::KeepAnchor);

                old_select_ = sor.selectedText();
                old_position_ = sor.position();
                start_row_ = sor.blockNumber();
                end_row_ = 0;

                if(isenable)
                {
                    edit_window_->shear_button_.setEnabled(true);
                    edit_window_->copy_button_.setEnabled(true);
                    edit_window_->chang_data_button_.setEnabled(true);
                    edit_window_->del_button_.setEnabled(true);
                    edit_window_->remarks_button_.setEnabled(true);

                    if(!copy_str_->isEmpty())
                    {
                        edit_window_->stick_button_.setEnabled(true);
                        edit_window_->upper_stick_button_.setEnabled(true);
                    }
                }
            }
            else
            {
                auto select = sor.selectedText();
                if(!select.isEmpty())
                {
                    if(select.size() > 3)
                    {
                        if(select.mid(0, 3) == "END")
                        {
                            sor.clearSelection();

                            old_select_.clear();
                            old_position_ = -1;
                            start_row_ = end_row_ = old_position_;

                            this->setTextCursor(sor);
                            if(isenable)
                            {
                                edit_window_->shear_button_.setEnabled(false);
                                edit_window_->copy_button_.setEnabled(false);
                                edit_window_->chang_data_button_.setEnabled(false);
                                edit_window_->del_button_.setEnabled(false);
                                edit_window_->remarks_button_.setEnabled(false);
                                edit_window_->stick_button_.setEnabled(false);
                                edit_window_->upper_stick_button_.setEnabled(false);
                            }

                            return;
                        }
                    }

                    if(select.at(select.size() - 1) == ',' || select.at(select.size() - 1) == ';')
                    {
                        int start = sor.position() - select.size();
                        int end = start + select.size() - 1;
                        sor.setPosition(end, QTextCursor::MoveAnchor);
                        sor.setPosition(start, QTextCursor::KeepAnchor);
                        word = sor.selectedText();
                    }
                    else
                    {
                        int a = block.text().size() - line.size();
                        int pos = sor.position() - block.position() - a;
                        if(pos < line.size())
                        {
                            if(line.at(pos) == '{')
                            {
                                int n = 1, p = pos + 1;
                                for(; p < line.size(); p++)
                                {
                                    if(line.at(p) == '{')
                                    {
                                        n++;
                                    }
                                    else if(line.at(p) == '}')
                                    {
                                        n--;

                                        if(n == 0)
                                        {
                                            sor.setPosition(block.position() + p + a + 1, QTextCursor::KeepAnchor);

                                            this->setTextCursor(sor);
                                            if(isenable)
                                            {
                                                edit_window_->shear_button_.setEnabled(true);
                                                edit_window_->copy_button_.setEnabled(true);
                                                edit_window_->chang_data_button_.setEnabled(true);
                                                edit_window_->del_button_.setEnabled(true);
                                                edit_window_->remarks_button_.setEnabled(true);

                                                if(!copy_str_->isEmpty())
                                                {
                                                    edit_window_->stick_button_.setEnabled(true);
                                                    edit_window_->upper_stick_button_.setEnabled(true);
                                                }
                                            }

                                            old_select_ = sor.selectedText();
                                            old_position_ = sor.position();
                                            start_row_ = sor.blockNumber();
                                            end_row_ = 0;

                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                old_select_ = word;
                old_position_ = sor.position();
                start_row_ = sor.blockNumber();
                end_row_ = 0;
            }
        }

        this->setTextCursor(sor);
        if(isenable)
        {
            edit_window_->shear_button_.setEnabled(true);
            edit_window_->copy_button_.setEnabled(true);
            edit_window_->chang_data_button_.setEnabled(true);
            edit_window_->del_button_.setEnabled(true);
            edit_window_->remarks_button_.setEnabled(true);

            if(!copy_str_->isEmpty())
            {
                edit_window_->stick_button_.setEnabled(true);
                edit_window_->upper_stick_button_.setEnabled(true);
            }
        }
    }
}

void CodeEdit::mouseDoubleClickEvent(QMouseEvent *)
{
}

void CodeEdit::mouseMoveEvent(QMouseEvent *)
{
}

void CodeEdit::mouseReleaseEvent(QMouseEvent *)
{
}

void CodeEdit::ModCodeRow()
{
    int row = this->document()->blockCount();
    int num = code_row_->document()->blockCount();

    if(row == num)
        return;

        code_row_->clear();

        for(int i = 0; i < row; i++)
        {
            code_row_->AddData(QString::number(i));
        }

        code_row_->MvStart();
}

QPair<bool, QPair<QString, QString>> CodeEdit::IsCreateVarMove(int row)
{
    if(row <= row_main_)
        return qMakePair(false, qMakePair(QString(), QString()));

    auto *docu = this->document();
    QString v0, z0;
    for(int i = row; i > row_main_ - 1; i--)
    {
        auto code = docu->findBlockByLineNumber(i).text();
        code.replace("\t", "").replace("\n", "");
        auto head = code.section(" ", 0, 0);

        if(head == "FUNC" || code.indexOf("LOCAL FUNC") != -1)
        {
            return qMakePair(false, qMakePair(v0, z0));
        }
        else if(head == "PROC" || code.indexOf("LOCAL PROC") != -1)
        {
            return qMakePair(false, qMakePair(v0, z0));
        }
        else if(head == "TRAP" || code.indexOf("LOCAL TRAP") != -1)
        {
            return qMakePair(false, qMakePair(v0, z0));
        }
        else if(head == "MoveJ" || head == "MoveL")
        {
            if(v0.isEmpty()) //保证只获取第一次出现的速度参数
            {
                v0 = code.section(" ", 2, 2); //注意，获取到的速度参数是带有,的
            }
            if(z0.isEmpty()) //保证只获取第一次出现的区域参数
            {
                z0 = code.section(" ", 3, 3); //注意，获取到的区域参数是带有,的
            }

            if(code.section(" ", 1, 1) != "*,")
            {
                return qMakePair(true, qMakePair(v0, z0));
            }
        }
        else if(head == "MoveAbsJ") //为MoveAbsJ时需要获取速度参数和区域参数，但是还处于假状态
        {
            if(v0.isEmpty())
            {
                v0 = code.section(" ", 2, 2);
            }
            if(z0.isEmpty())
            {
                z0 = code.section(" ", 3, 3);
            }
        }
        else if(head == "MoveC") //MoveC时区域参数不获取只需要获取速度参数
        {
            if(v0.isEmpty())
            {
                v0 = code.section(" ", 3, 3);
            }

            if(code.section(" ", 1, 1) != "*,")
            {
                return qMakePair(true, qMakePair(v0, z0));
            }
            if(code.section(" ", 2, 2) != "*,")
            {
                return qMakePair(true, qMakePair(v0, z0));
            }
        }
    }

    return qMakePair(false, qMakePair(v0, z0));
}

QPair<bool, QPair<QString, QString> > CodeEdit::IsCreateVarAbsj(int row)
{
    if(row <= row_main_)
        return qMakePair(false, qMakePair(QString(), QString()));

    auto *docu = this->document();
    QString v0, z0;
    for(int i = row; i > row_main_ - 1; i--)
    {
        auto code = docu->findBlockByLineNumber(i).text();
        code.replace("\t", "").replace("\n", "");
        auto head = code.section(" ", 0, 0);

        if(head == "FUNC" || code.indexOf("LOCAL FUNC") != -1)
        {
            return qMakePair(false, qMakePair(v0, z0));
        }
        else if(head == "PROC" || code.indexOf("LOCAL PROC") != -1)
        {
            return qMakePair(false, qMakePair(v0, z0));
        }
        else if(head == "TRAP" || code.indexOf("LOCAL TRAP") != -1)
        {
            return qMakePair(false, qMakePair(v0, z0));
        }
        else if(head == "MoveJ" || head == "MoveL") //只获取速度和区域参数，但任处于假状态
        {
            if(v0.isEmpty()) //保证只获取第一次出现的速度参数
            {
                v0 = code.section(" ", 2, 2); //注意，获取到的速度参数是带有,的
            }
            if(z0.isEmpty()) //保证只获取第一次出现的区域参数
            {
                z0 = code.section(" ", 3, 3); //注意，获取到的区域参数是带有,的
            }
        }
        else if(head == "MoveAbsJ")
        {
            if(v0.isEmpty())
            {
                v0 = code.section(" ", 2, 2);
            }
            if(z0.isEmpty())
            {
                z0 = code.section(" ", 3, 3);
            }

            if(code.section(" ", 1, 1) != "\\NoEOffs,")
            {
                return qMakePair(true, qMakePair(v0, z0));
            }
        }
        else if(head == "MoveC") //MoveC时区域参数不获取只需要获取速度参数，任处于假状态
        {
            if(v0.isEmpty())
            {
                v0 = code.section(" ", 3, 3);
            }
        }
    }

    return qMakePair(false, qMakePair(v0, z0));
}

int CodeEdit::GetRow()
{
    return row_main_;
}

void CodeEdit::SetIsShow(bool isshow)
{
    if(isshow_ != isshow)
    {
        isshow_ = isshow;

        this->SetData(mod_name_);
    }
}

bool CodeEdit::GetIsShow()
{
    return isshow_;
}

void CodeEdit::SaveData()
{
    if(mod_name_.isEmpty()) //当未打开某个模块时不需要保存
    {
        return;
    }

    EditData::instance()->SaveMod(mod_name_);
}

void CodeEdit::ConnInit()
{
    connect(enlarge_button_, &QPushButton::clicked,
            [ = ]()
    {
        if(font_size_ + 10 >= font_max_size_)
        {
            font_size_ = font_max_size_;
            enlarge_button_->hide();
            if(narrow_button_->isHidden())
            {
                narrow_button_->show();
            }
        }
        else
        {
            font_size_ += 10;
            if(narrow_button_->isHidden())
            {
                narrow_button_->show();
            }
        }
        QFont font(this->font());
        font.setPointSize(font_size_);
        this->setFont(font);

        code_row_->SetFont(font);
    });

    connect(narrow_button_, &QPushButton::clicked,
            [ = ]()
    {
        int font_min_size = font_max_size_ - 60;
        if(font_min_size <= 1) //字体大小不允许小于0
        {
            font_min_size = 1;
        }
        if(font_size_ - 10 <= font_min_size )
        {
            font_size_ = font_min_size;
            narrow_button_->hide();
            if(enlarge_button_->isHidden())
            {
                enlarge_button_->show();
            }
        }
        else
        {
            font_size_ -= 10;
            if(enlarge_button_->isHidden())
            {
                enlarge_button_->show();
            }
        }
        QFont font(this->font());
        font.setPointSize(font_size_);
        this->setFont(font);

        code_row_->SetFont(font);
    });

    connect(left_button_, &QPushButton::clicked,
            [ = ]()
    {
        horiz_scroll_bar_->setValue(horiz_scroll_bar_->value() - 20);
        if(horiz_scroll_bar_->value() <= horiz_scroll_bar_->minimum())
        {
            left_button_->hide();
            left_button_->hide();
        }
        if(horiz_scroll_bar_->maximum() != 0)
        {
            if(right_button_->isHidden())
            {
                right_button_->show();
                right_button_->show();
            }
        }
    });

    connect(right_button_, &QPushButton::clicked,
            [ = ]()
    {
        horiz_scroll_bar_->setValue(horiz_scroll_bar_->value() + 20);
        if(horiz_scroll_bar_->value() >= horiz_scroll_bar_->maximum())
        {
            right_button_->hide();
            right_button_->hide();
        }
        if(horiz_scroll_bar_->maximum() != 0)
        {
            if(left_button_->isHidden())
            {
                left_button_->show();
                left_button_->show();
            }
        }
    });

    connect(this->GetUpperButton(), &QPushButton::clicked,
            [=]()
    {
        code_row_->Upper();
    });

    connect(this->GetLowerButton(), &QPushButton::clicked,
            [=]()
    {
        code_row_->Lower();
    });

    connect(this->GetUppersButton(), &QPushButton::clicked,
            [=]()
    {
        code_row_->Uppers();
    });

    connect(this->GetLowersButton(), &QPushButton::clicked,
            [=]()
    {
        code_row_->Lowers();
    });

    connect(VarInfo::instance().data(), &VarInfo::VarRemove, this, &CodeEdit::CreateVar);

    connect(EditData::instance().data(), &EditData::DataMid, this, &CodeEdit::CreateVar);
}

void CodeEdit::ButtonIsHide()
{
    int font_min_size = font_max_size_ - 60;
    if(font_min_size <= 1)
    {
        font_min_size = 1;
    }
    if(font_size_ >= font_max_size_)
    {
        enlarge_button_->hide();
        narrow_button_->show();
    }
    else if(font_size_ <= font_min_size)
    {
        enlarge_button_->show();
        narrow_button_->hide();
    }
    else
    {
        enlarge_button_->show();
        narrow_button_->show();
    }
    if(horiz_scroll_bar_->maximum() != 0)
    {
        if(horiz_scroll_bar_->value() <= horiz_scroll_bar_->minimum())
        {
            left_button_->hide();
            right_button_->show();
        }
        else if(horiz_scroll_bar_->value() >= horiz_scroll_bar_->maximum())
        {
            left_button_->show();
            right_button_->hide();
        }
        else
        {
            left_button_->show();
            right_button_->show();
        }
    }
    else
    {
        left_button_->hide();
        right_button_->hide();
    }
}

TaskOrder::TaskOrder(QWidget *parent, int type) : WindowObject(type, parent)
{
    tips_label_ = new QLabel(tr("任务与程序"), main_widget_);
    table_box_ = new TableBox(main_widget_);
    file_button_ = new ComboBoxObject(tr("文件"), button_widget_);
    show_button_ = new QPushButton(tr("显示模块"), button_widget_);
    open_button_ = new QPushButton(tr("打开"), button_widget_);

    create_order_ = new QAction(tr("新建程序"), this);
    load_order_ = new QAction(tr("加载程序"), this);
    save_order_ = new QAction(tr("另存程序为"), this);
    rename_order_ = new QAction(tr("重命名程序"), this);
    delete_order_ = new QAction(tr("删除程序"), this);

    key_board_ = new Keyboard(this);
    file_manger_ = new TaskFileManger(this);

    file_button_->AppAction(create_order_);
    file_button_->AppAction(load_order_);
    file_button_->AppSeparator();
    file_button_->AppAction(save_order_);
    file_button_->AppSeparator();
    file_button_->AppAction(rename_order_);
    file_button_->AppSeparator();
    file_button_->AppAction(delete_order_);

    file_button_->setFlat(true);
    show_button_->setFlat(true);
    open_button_->setFlat(true);

    key_board_->hide();
    file_manger_->hide();

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(1, 3);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("任务名称") << tr("程序名称") << tr("类型"));

    connect(create_order_, &QAction::triggered, this, &TaskOrder::CreateOrder);
    connect(rename_order_, &QAction::triggered, this, &TaskOrder::RenameOrder);
    connect(delete_order_, &QAction::triggered, this, &TaskOrder::DeleteOrder);
    connect(key_board_, &Keyboard::GetData, this, &TaskOrder::GetDate);
    connect(key_board_->cancel_button_, &QPushButton::clicked, this, &TaskOrder::HideKey);
    connect(file_manger_, &TaskFileManger::SaveTask, this, &TaskOrder::SaveTask);
    connect(file_manger_, &TaskFileManger::LoadTask, this, &TaskOrder::LoadTask);
    connect(file_manger_, &TaskFileManger::CancelButton,
            [=]()
    {
        file_manger_->hide();
    });
    connect(load_order_, &QAction::triggered,
            [=]()
    {
        file_manger_->Init(false);
        file_manger_->resize(this->size());
        file_manger_->move(0, 0);
        file_manger_->show();
    });
    connect(save_order_, &QAction::triggered,
            [=]()
    {
        file_manger_->Init(true, false);
        file_manger_->resize(this->size());
        file_manger_->move(0, 0);
        file_manger_->show();
    });
}

TaskOrder::~TaskOrder()
{
}

void TaskOrder::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);
    tips_label_->resize(main_widget_->width(), 28 * ScaleH());
    table_box_->resize(main_widget_->width(), main_widget_->height() - tips_label_->height());
    file_button_->resize(126 * ScaleW(), button_widget_->height());
    show_button_->resize(file_button_->size());
    open_button_->resize(show_button_->size());

    tips_label_->move(11 * ScaleW(), 0);
    table_box_->move(0, tips_label_->y() + tips_label_->height());

    file_button_->move(0, 0);
    open_button_->move(button_widget_->width() - open_button_->width(), file_button_->y());
    show_button_->move(open_button_->x() - show_button_->width(), open_button_->y());
    if(!key_board_->isHidden())
    {
        key_board_->resize(this->size());
        key_board_->move(0, 0);
    }
    if(!file_manger_->isHidden())
    {
        file_manger_->resize(this->size());
        file_manger_->move(0, 0);
    }
}

void TaskOrder::ReadTask()
{
    QString path = qApp->applicationDirPath() + "/data/RAPID/Task/"; //存放任务的文件夹
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    auto tasks = dir.entryList();

    for(auto task : tasks)
    {
        QSettings init_info(path + task, QSettings::IniFormat);

        QString task_name = task.mid(0, task.size() - 4);
        QString name = init_info.value("/info/name").toString();
        QString type = init_info.value("/info/type").toString();
        QString path = init_info.value("/info/path").toString();
        if(name.isEmpty())
            name = "<No Program>";
        if(type.isEmpty())
            type = "Normal";
        if(path.isEmpty())
            path = "TempMod";

        EditData::instance()->SetTaskInfo(task_name, name, type, path);

        int row = table_box_->GetRow() - 1;
        table_box_->AddText(row, 0, task_name);
        table_box_->AddText(row, 1, name);
        table_box_->AddText(row, 2, type);

        EditData::instance()->AnalysisData();
    }
    if(EditData::instance()->GetTaskOrderName() != "<No Program>")
    {
        emit LoadTaSig();
    }
}

void TaskOrder::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);
    key_board_->SetScale(scale_w, scale_h);
    file_manger_->SetScale(scale_w, scale_h);
    resizeEvent(nullptr);
}

void TaskOrder::CreateOrder()
{
    auto age = QMessageBox::question(this, tr("提示"), tr("新建程序前是否需要保存当前程序?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if(age == QMessageBox::Cancel)
    {
        return;
    }
    else if(age == QMessageBox::Yes)
    {
        file_manger_->show();
        file_manger_->Init(true, true);
        file_manger_->resize(this->size());
        file_manger_->move(0, 0);
    }
    else
    {
        EditData::instance()->DelTask();

        EditData::instance()->SetTaskOrderName("NewProgramName");

        EditData::instance()->AnalysisData();

        QString data = "MODULE MainModule\n\tPROC main()\n\t\t<SMT>\n\tENDPROC\nENDMODULE";
        EditData::instance()->LoadMod(data);

        auto *item1 = table_box_->item(0, 1);
        auto *item2 = table_box_->item(0, 2);
        item1->setText("NewProgramName");
        item2->setText("Normal");

        emit CreateSig();
    }
}

void TaskOrder::RenameOrder()
{
    key_board_->Init();
    key_board_->show();
    GetIcoLabel()->hide();
    GetTitleLabel()->hide();
    GetUnderLabel()->hide();
    main_widget_->hide();
    button_widget_->hide();
    tips_label_->hide();
    table_box_->hide();
    key_board_->resize(this->size());
    key_board_->move(0, 0);
}

void TaskOrder::DeleteOrder()
{
    if(QMessageBox::question(this, tr("提示"), tr("确定要删除程序吗？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    EditData::instance()->DelTask(true);

    auto *item1 = table_box_->item(0, 1);
    auto *item2 = table_box_->item(0, 2);
    item1->setText(EditData::instance()->GetTaskOrderName());
    item2->setText(EditData::instance()->GetTaskTypeName());
    emit DeleteSig();
}

void TaskOrder::GetDate(QString data)
{
    EditData::instance()->SetTaskOrderName(data);
    auto *item1 = table_box_->item(0, 1);
    item1->setText(data);
    HideKey();
}

void TaskOrder::HideKey()
{
    key_board_->hide();
    GetIcoLabel()->show();
    GetTitleLabel()->show();
    GetUnderLabel()->show();
    main_widget_->show();
    button_widget_->show();
    tips_label_->show();
    table_box_->show();
}

void TaskOrder::SaveTask()
{
    file_manger_->hide();

    auto *item1 = table_box_->item(0, 0);
    auto *item2 = table_box_->item(0, 1);
    auto *item3 = table_box_->item(0, 2);

    item1->setText(EditData::instance()->GetTaskName());
    item2->setText(EditData::instance()->GetTaskOrderName());
    item3->setText(EditData::instance()->GetTaskTypeName());

    emit CreateSig();
}

void TaskOrder::LoadTask()
{
    file_manger_->hide();

    auto *item1 = table_box_->item(0, 0);
    auto *item2 = table_box_->item(0, 1);
    auto *item3 = table_box_->item(0, 2);

    item1->setText(EditData::instance()->GetTaskName());
    item2->setText(EditData::instance()->GetTaskOrderName());
    item3->setText(EditData::instance()->GetTaskTypeName());
    emit LoadTaSig();
}

Modular::Modular(QWidget *parent, int type) : WindowObject(type, parent)
{
    tips_label_ = new QLabel(tr("模块"), main_widget_);
    table_box_ = new TableBox(main_widget_);
    file_button_ = new ComboBoxObject(tr("文件"), button_widget_);
    refresh_button_ = new QPushButton(tr("刷新"), button_widget_);
    show_button_ = new QPushButton(tr("显示模块"), button_widget_);
    retreat_button_ = new QPushButton(tr("后退"), button_widget_);
    create_mod_ = new QAction(tr("新建模块"), this);
    load_mod_ = new QAction(tr("加载模块"), this);
    save_mod_ = new QAction(tr("另存模块为"), this);
    change_state_ = new QAction(tr("更改声明"), this);
    del_mod_ = new QAction(tr("删除模块"), this);
    create_widget_ = new CreateModular(this);
    load_widget_ = new LoadModular(this);
    save_widget_ = new SaveModular(this);
    change_widget_ = new CreateModular(this);
    file_button_->AppAction(create_mod_);
    file_button_->AppAction(load_mod_);
    file_button_->AppSeparator();
    file_button_->AppAction(save_mod_);
    file_button_->AppSeparator();
    file_button_->AppAction(change_state_);
    file_button_->AppSeparator();
    file_button_->AppAction(del_mod_);
    create_widget_->hide();
    load_widget_->hide();
    save_widget_->hide();
    change_widget_->hide();
    change_widget_->GetDecoLabel()->setText(tr("更改模块声明"));
    file_button_->setFlat(true);
    refresh_button_->setFlat(true);
    show_button_->setFlat(true);
    retreat_button_->setFlat(true);

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(0, 3);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("类型") << tr("更改"));

    connect(refresh_button_, &QPushButton::clicked, this, &Modular::ReadMod);
    connect(create_mod_, &QAction::triggered, this, &Modular::ShowCreateMod);
    connect(create_widget_->cancel_button_, &QPushButton::clicked, this, &Modular::ShowControl);
    connect(create_widget_->define_button_, &QPushButton::clicked, this, &Modular::CreateMod);
    connect(load_mod_, &QAction::triggered, this, &Modular::ShowLoadMod);
    connect(load_widget_->cancel_button_, &QPushButton::clicked, this, &Modular::ShowControl);
    connect(load_widget_->define_button_, &QPushButton::clicked, this, &Modular::LoadMod);
    connect(save_mod_, &QAction::triggered, this, &Modular::ShowSaveMod);
    connect(save_widget_->cancel_button_, &QPushButton::clicked, this, &Modular::ShowControl);
    connect(save_widget_->define_button_, &QPushButton::clicked, this, &Modular::SaveMod);
    connect(change_state_, &QAction::triggered, this, &Modular::ShowChangeState);
    connect(change_widget_->cancel_button_, &QPushButton::clicked, this, &Modular::ShowControl);
    connect(change_widget_->define_button_, &QPushButton::clicked, this, &Modular::ChangeState);
    connect(del_mod_, &QAction::triggered, this, &Modular::DeleteMod);
}

void Modular::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);
    tips_label_->resize(main_widget_->width(), 28 * ScaleH());
    table_box_->resize(main_widget_->width(), main_widget_->height() - tips_label_->height());
    file_button_->resize(126 * ScaleW(), button_widget_->height());
    refresh_button_->resize(file_button_->size());
    show_button_->resize(refresh_button_->size());
    retreat_button_->resize(show_button_->size());

    tips_label_->move(11 * ScaleW(), 0);
    table_box_->move(0, tips_label_->y() + tips_label_->height());
    file_button_->move(0, 0);
    refresh_button_->move(file_button_->x() + file_button_->width(), file_button_->y());
    retreat_button_->move(button_widget_->width() - retreat_button_->width(), refresh_button_->y());
    show_button_->move(retreat_button_->x() - show_button_->width(), retreat_button_->y());
    if(!create_widget_->isHidden())
    {
        create_widget_->resize(this->width(), 365 * ScaleH());
        create_widget_->move(0, GetUnderLabel()->y() + GetUnderLabel()->height());
    }
    if(!load_widget_->isHidden())
    {
        load_widget_->resize(this->width(), 365 * ScaleH());
        load_widget_->move(0, GetUnderLabel()->y() + GetUnderLabel()->height());
    }
    if(!save_widget_->isHidden())
    {
        save_widget_->resize(this->width(), 365 * ScaleH());
        save_widget_->move(0, GetUnderLabel()->y() + GetUnderLabel()->height());
    }
    if(!change_widget_->isHidden())
    {
        change_widget_->resize(this->width(), 365 * ScaleH());
        change_widget_->move(0, GetUnderLabel()->y() + GetUnderLabel()->height());
    }
}

void Modular::ReadMod()
{
    if(table_box_->GetRow() != 0)
    {
        table_box_->clearContents();
        table_box_->SetRomn(0, 3);
    }

    int row = 0;
    QStringList sys_list = EditData::instance()->GetModName(true); //获取所有系统模块
    for(auto &name : sys_list)
    {
        table_box_->AddText(row, 0, name);
        table_box_->AddText(row++, 1, tr("系统模块"));
    }

    QStringList use_list = EditData::instance()->GetModName(false); //获取所有程序模块
    for(auto &name : use_list)
    {
        table_box_->AddText(row, 0, name);
        table_box_->AddText(row++, 1, tr("程序模块"));
    }
}

void Modular::HideControl()
{
    main_widget_->hide();
    button_widget_->hide();
}

void Modular::ShowControl()
{
    if(!create_widget_->isHidden())
    {
        create_widget_->hide();
    }
    if(!load_widget_->isHidden())
    {
        load_widget_->hide();
    }
    if(!save_widget_->isHidden())
    {
        save_widget_->hide();
    }
    if(!change_widget_->isHidden())
    {
        change_widget_->hide();
    }
    main_widget_->show();
    button_widget_->show();
}

void Modular::ShowCreateMod()
{
    if(QMessageBox::question(this, tr("提示"), tr("添加新模块后，您将丢失程序指针，是否继续?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        return;
    }
    create_widget_->Init();
    create_widget_->show();
    resizeEvent(nullptr);
    HideControl();
}

void Modular::ShowLoadMod()
{
    if(QMessageBox::question(this, tr("提示"), tr("添加新模块后，您将丢失程序指针，是否继续?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        return;
    }
    load_widget_->InIt();
    load_widget_->show();
    resizeEvent(nullptr);
    HideControl();
}

void Modular::ShowSaveMod()
{
    auto *item = table_box_->currentItem();
    if(item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(item), 0);
    if(name_item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }

    QString name;
    if(EditData::instance()->GetModType(name_item->text()))
    {
        name = name_item->text() + ".SYS";
        save_widget_->SetFilterBoxText(".SYS");
    }
    else
    {
        name = name_item->text() + ".MOD";
        save_widget_->SetFilterBoxText(".MOD");
    }
    save_widget_->InIt();
    save_widget_->show();
    resizeEvent(nullptr);
    save_widget_->file_edit_->setText(name);
    HideControl();
}

void Modular::ShowChangeState()
{
    auto *item = table_box_->currentItem();
    if(item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(item), 0);
    if(name_item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }

    QString name = name_item->text();
    QString type;
    if(EditData::instance()->GetModType(name))
    {
        type = "System";
    }
    else
    {
        type = "Program";
    }
    if(type == "System") //禁止修改系统模块
    {
        QMessageBox::warning(this, tr("提示"), tr("系统模块无法修改"));
        return;
    }
    change_widget_->name_edit_->setText(name);
    change_widget_->type_box_->setCurrentText(type);
    change_widget_->show();
    resizeEvent(nullptr);
    HideControl();
}

void Modular::DeleteMod()
{
    auto *item = table_box_->currentItem();
    if(item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(item), 0);
    if(name_item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }
    auto mod_name = name_item->text();

    if(EditData::instance()->GetModType(mod_name))
    {
        QMessageBox::warning(this, tr("提示"), tr("系统模块无法修改"));
        return;
    }
    QString path = qApp->applicationDirPath() + "/data/RAPID/Modular/" + EditData::instance()->GetTaskPath() + "/User/" + mod_name + ".MOD";
    if(QMessageBox::question(this, tr("询问"), tr("您确定要删除该模块吗？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        return;
    }
    if(!QFile::remove(path))
    {
        QMessageBox::about(this, tr("提示"), tr("删除失败，请检查您是否拥有相关权限"));
    }

    EditData::instance()->DelMod(mod_name);

    ReadMod();
    emit RevMod(mod_name);
}

void Modular::CreateMod()
{
    QString name = create_widget_->name_edit_->text();
    QString type = create_widget_->type_box_->currentText();
    if(name.isEmpty() || type.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入内容"));
        return;
    }
    QString path = qApp->applicationDirPath() + "/data/RAPID/Modular/";
    if(type == "Program") //程序模块
    {
        path = path + "User/" + name + ".MOD";
        QString data = "\nMODULE " + name + "\n\nENDMODULE";

        EditData::instance()->LoadMod(data);
        this->ReadMod();
    }
    ShowControl();
}

void Modular::LoadMod()
{
    QString file_path = load_widget_->GetModPath(); //获取模块全路径
    QString file_name = load_widget_->file_edit_->text(); //获取模块名
    if(file_path.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择文件"));
        return;
    }
    QFile file(file_path);
    if(!file.exists())
    {
        QMessageBox::warning(this, tr("提示"), tr("您选择的文件不存在"));
        return;
    }
    QString dir = qApp->applicationDirPath() + "/data/RAPID/Modular/" + EditData::instance()->GetTaskPath() + "/";

    QString suffix = file_name.mid(file_name.lastIndexOf(".") + 1);

    QString fe_name, fi_name;
    if(suffix == "sys")
        fe_name = file_name.mid(0, file_name.indexOf(".")) + ".SYS";
    else
        fe_name = file_name;
    if(suffix == "mod")
        fi_name = file_name.mid(0, file_name.indexOf(".")) + ".MOD";
    else
        fi_name = file_name;

    QFile fe(dir + "System/" + fe_name);
    QFile fi(dir + "User/" + fi_name);
    QString data;
    if(fe.exists() || fi.exists()) //所选择的模块在示教器模块文件夹中已有同名模块存在
    {
        QMessageBox::warning(this, tr("提示"), tr("您选择的模块示教器中已存在同名模块，请重新选择"));
        return;
    }

    if(suffix == "SYS" || suffix == "sys") //系统模块
    {
        if(suffix == "sys")
        {
            file_name = file_name.mid(0, file_name.indexOf(".")) + ".SYS";
        }
        if(!file.copy(dir + "System/" + file_name))
        {
            QMessageBox::warning(this, tr("提示"), tr("移动模块失败，请确认您是否拥有相关权限"));
            return;
        }
        fe.open(QIODevice::ReadOnly);
        data = fe.readAll();
    }
    else if(suffix == "MOD" || suffix == "mod") //用户模块
    {
        if(suffix == "mod")
        {
            file_name = file_name.mid(0, file_name.indexOf(".")) + ".MOD";
        }

        if(!file.copy(dir + "User/" + file_name))
        {
            QMessageBox::warning(this, tr("提示"), tr("移动模块失败，请确认您是否拥有相关权限"));
            return;
        }
        fi.open(QIODevice::ReadOnly);
        data = fi.readAll();
    }

    EditData::instance()->LoadMod(data);
    this->ReadMod();

    ShowControl();
}

void Modular::SaveMod()
{
    QString file_path = save_widget_->path_ + "/" + save_widget_->file_edit_->text();

    auto *item = table_box_->currentItem();
    if(item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(item), 0);
    if(name_item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }

    QString path;
    if(EditData::instance()->GetModType(name_item->text()))
    {
        path = qApp->applicationDirPath() + "/data/RAPID/Modular/" + EditData::instance()->GetTaskPath() + "/System/" + name_item->text() + ".SYS";
    }
    else
    {
        path = qApp->applicationDirPath() + "/data/RAPID/Modular/" + EditData::instance()->GetTaskPath() + "/User/" + name_item->text() + ".MOD";
    }
    QFile file(file_path);
    if(file.exists())
    {
        if(QMessageBox::question(this, tr("提示"), tr("该文件已存在，是否替换"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return;
        }
        file.remove(); //先删除已有文件
        if(!file.copy(path, file_path))
        {
            QMessageBox::warning(this, tr("提示"), tr("复制失败，请检查您是否拥有相关权限"));
            return;
        }
    }
    else
    {
        if(!file.copy(path, file_path))
        {
            QMessageBox::warning(this, tr("提示"), tr("复制失败，请检查您是否拥有相关权限"));
            return;
        }
    }
    ShowControl();
}

void Modular::ChangeState()
{
    QString name = change_widget_->name_edit_->text();
    if(name.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("模块名不能为空"));
        return;
    }
    if(EditData::instance()->IsExist(name)) //如果新模块名已存在
    {
        QMessageBox::warning(this, tr("提示"), tr("该模块已存在，修改失败"));
        return;
    }

    auto *item = table_box_->currentItem();
    if(item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(item), 0);
    if(name_item == nullptr)
    {
        ShowControl();
        QMessageBox::warning(this, tr("提示"), tr("您还未选择模块"));
        return;
    }

    auto mod_name = name_item->text();
    EditData::instance()->RemMod(mod_name, name);

    ShowControl();
    ReadMod();

    emit RevMod(mod_name);
}

void Modular::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);
    resizeEvent(nullptr);;

    table_box_->SetScale(scale_w, scale_h);
    create_widget_->SetScale(scale_w, scale_h);
    load_widget_->SetScale(scale_w, scale_h);
    save_widget_->SetScale(scale_w, scale_h);
    change_widget_->SetScale(scale_w, scale_h);
}

void Modular::Init()
{
    if(table_box_->GetRow() == 0)
        ReadMod();
}

Routine::Routine(QWidget *parent, int type) : WindowObject(type, parent)
{
    tips_label_ = new QLabel(tr("例行程序"), main_widget_);
    tips2_label_ = new QLabel(tr("活动过滤器:"), main_widget_);
    filter_label_ = new QLabel(main_widget_);
    table_box_ = new TableBox(main_widget_);
    file_button_ = new ComboBoxObject(tr("文件"), button_widget_);
    filter_button_ = new ComboBoxObject(button_widget_);
    show_button_ = new QPushButton(tr("显示例行程序"), button_widget_);
    retreat_button_ = new QPushButton(tr("后退"), button_widget_);
    create_routine_ = new QAction(tr("新建例行程序"), this);
    copy_routine_ = new QAction(tr("复制例行程序"), this);
    move_routine_ = new QAction(tr("移动例行程序"), this);
    change_state_ = new QAction(tr("更改声明"), this);
    rename_routine_ = new QAction(tr("重命名"), this);
    delete_routine_ = new QAction(tr("删除例行程序"), this);
    create_routine_widget_ = new RoutineWidget(false, true, this);
    copy_routine_widget_ = new RoutineWidget(true, false, this);
    move_routine_widget_ = new RoutineWidget(true, false, this);
    change_state_widget_ = new RoutineWidget(false, true, this);

    file_button_->AppAction(create_routine_);
    file_button_->AppAction(copy_routine_);
    file_button_->AppAction(move_routine_);
    file_button_->AppSeparator();
    file_button_->AppAction(change_state_);
    file_button_->AppAction(rename_routine_);
    file_button_->AppSeparator();
    file_button_->AppAction(delete_routine_);

    create_routine_widget_->hide();
    copy_routine_widget_->hide();
    move_routine_widget_->hide();
    change_state_widget_->hide();

    filter_button_->SetFilterButton();

    file_button_->setFlat(true);
    filter_button_->setFlat(true);
    show_button_->setFlat(true);
    retreat_button_->setFlat(true);

    //create_routine_widget_->data_type_edit_->setEnabled(false);
    create_routine_widget_->data_type_box_->setEnabled(false);
    create_routine_widget_->show_type_button_->setEnabled(false);
    //copy_routine_widget_->data_type_edit_->setEnabled(false);
    copy_routine_widget_->data_type_box_->setEnabled(false);
    copy_routine_widget_->show_type_button_->setEnabled(false);
    move_routine_widget_->name_edit_->setEnabled(false);
    move_routine_widget_->show_key_button_->setEnabled(false);
    move_routine_widget_->type_box_->setEnabled(false);
    move_routine_widget_->param_edit_->setEnabled(false);
    move_routine_widget_->show_param_button_->setEnabled(false);
    //move_routine_widget_->data_type_edit_->setEnabled(false);
    move_routine_widget_->data_type_box_->setEnabled(false);
    move_routine_widget_->show_type_button_->setEnabled(false);
    move_routine_widget_->local_box_->setEnabled(false);
    move_routine_widget_->revoke_box_->setEnabled(false);
    move_routine_widget_->error_box_->setEnabled(false);
    move_routine_widget_->retral_box_->setEnabled(false);
    change_state_widget_->name_edit_->setEnabled(false);
    change_state_widget_->show_key_button_->setEnabled(false);
    //change_state_widget_->data_type_edit_->setEnabled(false);
    change_state_widget_->data_type_box_->setEnabled(false);
    change_state_widget_->show_type_button_->setEnabled(false);

    keyboard_ = new Keyboard(this);
    filter_key_ = new FilterKey(this);

    filter_key_->resize(394*ScaleW(), 275*ScaleH());
    filter_key_->move(245*ScaleW(), 65*ScaleH());
    filter_key_->setFrameShape(QFrame::Box);
    keyboard_->hide();
    filter_key_->hide();

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(0, 3);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("模块") << tr("类型"));

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    filter_key_->setAutoFillBackground(true);
    filter_key_->setPalette(p);

    connect(create_routine_, &QAction::triggered, this, &Routine::ShowCreateRoutine);
    connect(create_routine_widget_->cancel_button_, &QPushButton::clicked, this, &Routine::ShowControl);
    connect(copy_routine_, &QAction::triggered, this, &Routine::ShowCopyRoutine);
    connect(copy_routine_widget_->cancel_button_, &QPushButton::clicked, this, &Routine::ShowControl);
    connect(move_routine_, &QAction::triggered, this, &Routine::ShowMoveRoutine);
    connect(move_routine_widget_->cancel_button_, &QPushButton::clicked, this, &Routine::ShowControl);
    connect(change_state_, &QAction::triggered, this, &Routine::ShowChangeState);
    connect(change_state_widget_->cancel_button_, &QPushButton::clicked, this, &Routine::ShowControl);
    connect(create_routine_widget_->define_button_, &QPushButton::clicked, this, &Routine::CreateRoutine);
    connect(copy_routine_widget_->define_button_, &QPushButton::clicked, this, &Routine::CopyRoutine);
    connect(move_routine_widget_->define_button_, &QPushButton::clicked, this, &Routine::MoveRoutine);
    connect(change_state_widget_->define_button_, &QPushButton::clicked, this, &Routine::ChangeState);
    connect(rename_routine_, &QAction::triggered, this, &Routine::ShowRenameRout);
    connect(keyboard_->cancel_button_, &QPushButton::clicked, this, &Routine::ShowControl);
    connect(keyboard_, &Keyboard::GetData, this, &Routine::RenameRout);
    connect(delete_routine_, &QAction::triggered, this, &Routine::DelRout);
    connect(filter_button_, &ComboBoxObject::clicked,
            [=]()
    {
        if(filter_key_->isHidden())
        {
            filter_key_->resize(394*ScaleW(), 275*ScaleH());
            filter_key_->move(245*ScaleW(), 65*ScaleH());
            filter_key_->Init();
            filter_key_->show();
        }
        else
        {
            filter_key_->hide();
        }
    });
    connect(filter_key_->reset_button_, &QPushButton::clicked,
            [=]()
    {
        filter_label_->clear();
        ReadRoutine(mod_name_);
    });
    connect(filter_key_, &FilterKey::ShowWindow,
            [=]()
    {
        filter_button_->LowerArrow();
    });
    connect(filter_key_, &FilterKey::HideWindow,
            [=]()
    {
        filter_button_->UpperArrow();
    });
    connect(filter_key_, &FilterKey::GetData,
            [=](QString data)
    {
        filter_label_->setText(data);
        ReadRoutine(mod_name_, data);
    });
}

void Routine::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    tips_label_->resize(363 * ScaleW(), 28 * ScaleH());
    tips2_label_->resize(139 * ScaleW(), 28 * ScaleH());
    filter_label_->resize(main_widget_->width() - tips_label_->width() - tips2_label_->width(), tips2_label_->height());
    table_box_->resize(main_widget_->width(), main_widget_->height() - tips2_label_->height());
    file_button_->resize(126 * ScaleW(), button_widget_->height());
    filter_button_->resize(file_button_->size());
    show_button_->resize(filter_button_->size());
    retreat_button_->resize(show_button_->size());

    tips_label_->move(11 * ScaleW(), 0);
    tips2_label_->move(tips_label_->x() + tips_label_->width(), tips_label_->y());
    filter_label_->move(tips2_label_->x() + tips2_label_->width(), tips2_label_->y());
    table_box_->move(0, tips2_label_->y() + tips2_label_->height());
    file_button_->move(0, 0);
    filter_button_->move(file_button_->x() + file_button_->width(), file_button_->y());
    retreat_button_->move(button_widget_->width() - retreat_button_->width(), filter_button_->y());
    show_button_->move(retreat_button_->x() - show_button_->width(), retreat_button_->y());
    int control_height = this->height() - GetIcoLabel()->height() - GetUnderLabel()->height();
    int control_y = GetUnderLabel()->y() + GetUnderLabel()->height();
    if(!create_routine_widget_->isHidden())
    {
        create_routine_widget_->resize(this->width(), control_height);
        create_routine_widget_->move(0, control_y);
    }
    if(!copy_routine_widget_->isHidden())
    {
        copy_routine_widget_->resize(this->width(), control_height);
        copy_routine_widget_->move(0, control_y);
    }
    if(!move_routine_widget_->isHidden())
    {
        move_routine_widget_->resize(this->width(), control_height);
        move_routine_widget_->move(0, control_y);
    }
    if(!change_state_widget_->isHidden())
    {
        change_state_widget_->resize(this->width(), control_height);
        change_state_widget_->move(0, control_y);
    }
    if(!keyboard_->isHidden())
    {
        keyboard_->resize(this->width(), control_height);
        keyboard_->move(0, control_y);
    }
    if(!filter_key_->isHidden())
    {
        filter_key_->resize(394*ScaleW(), 275*ScaleH());
        filter_key_->move(245*ScaleW(), 65*ScaleH());
    }
}

void Routine::ReadRoutine(QString mod_name, QString filter)
{
    if(mod_name.isEmpty())
    {
        return;
    }

    if(table_box_->GetRow() != 0) //为防止重复添加数据，先将旧数据删除掉
    {
        table_box_->clearContents();
        table_box_->SetRomn(0, 3);
    }

    QStringList list = EditData::instance()->GetRoutName(mod_name);

    for(int i = 0; i < list.size(); i++)
    {
        auto &str = list[i];

        QString type, name;
        RoutineData &rout = EditData::instance()->GetRoutData(str);

        if(rout.type == RoutineData::PROC)
        {
            type = "Procedure";
            if(rout.param.isEmpty())
                name = str + "()";
            else
                name = str + "(" + rout.param + ")";
        }
        else if(rout.type == RoutineData::FUNC)
        {
            type = "Function";
            if(rout.param.isEmpty())
                name = str + "()";
            else
                name = str + "(" + rout.param + ")";
        }
        else
        {
            type = "Trap";
            name = str;
        }

        if(filter.isEmpty())
        {
            table_box_->AddText(i, 0, name);
            table_box_->AddText(i, 1, mod_name);
            table_box_->AddText(i, 2, type);
        }
        else
        {
            if(name.toUpper().indexOf(filter) != -1)
            {
                table_box_->AddText(i, 0, name);
                table_box_->AddText(i, 1, mod_name);
                table_box_->AddText(i, 2, type);
            }
        }
    }

    mod_name_ = mod_name;
}

void Routine::CreateRoutine()
{
    if(create_routine_widget_->name_edit_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入程序名"));
        return;
    }

    QPair<RoutineData, QString> pair = create_routine_widget_->GetCode();

    RoutineData rout_data = pair.first;
    QString rout_name = pair.second.section("0|0", 0, 0);
    QString mod_name = pair.second.section("0|0", 1, 1);

    if(!EditData::instance()->IsExist(mod_name))
    {
        QMessageBox::warning(this, tr("提示"), tr("您选择的模块不存在"));
        return;
    }

    if(EditData::instance()->RoutIsExist(rout_name))
    {
        QMessageBox::warning(this, tr("提示"), tr("该例行程序已存在"));
        return;
    }

    if(mod_name == mod_name_) //在本模块添加
    {
        EditData::instance()->ModAddRout(rout_data, rout_name, mod_name);

        emit RevMod(mod_name);
    }
    else //其他模块添加
    {
        if(!EditData::instance()->ModAddRout(rout_data, rout_name, mod_name))
        {
            QMessageBox::warning(this, tr("提示"), tr("添加失败"));
            return;
        }
    }

    ShowControl();
}

void Routine::CopyRoutine()
{
    if(copy_routine_widget_->name_edit_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入程序名"));
        return;
    }

    QPair<RoutineData, QString> pair = copy_routine_widget_->GetCode();

    RoutineData rout_data = pair.first;
    QString rout_name = pair.second.section("0|0", 0, 0);
    QString mod_name = pair.second.section("0|0", 1, 1);

    auto *curr_item = table_box_->currentItem();
    if(curr_item == nullptr)
    {
        QMessageBox::warning(this, tr("错误"), tr("例行程序不存在"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        QMessageBox::warning(this, tr("错误"), tr("例行程序不存在"));
        return;
    }

    RoutineData &rout = EditData::instance()->GetRoutData(name_item->text());
    if(!rout.code.isEmpty())
        rout_data.code = rout.code;

    if(!EditData::instance()->IsExist(mod_name))
    {
        QMessageBox::warning(this, tr("提示"), tr("您选择的模块不存在"));
        return;
    }

    if(EditData::instance()->RoutIsExist(rout_name))
    {
        QMessageBox::warning(this, tr("提示"), tr("该例行程序已存在"));
        return;
    }

    if(mod_name == mod_name_) //在本模块添加
    {
        EditData::instance()->ModAddRout(rout_data, rout_name, mod_name);

        emit RevMod(mod_name);
    }
    else //其他模块添加
    {
        if(!EditData::instance()->ModAddRout(rout_data, rout_name, mod_name))
        {
            QMessageBox::warning(this, tr("提示"), tr("添加失败"));
            return;
        }
    }

    ShowControl();
}

void Routine::MoveRoutine()
{
    if(move_routine_widget_->name_edit_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入程序名"));
        return;
    }

    QPair<RoutineData, QString> pair = move_routine_widget_->GetCode();

    QString rout_name = pair.second.section("0|0", 0, 0);
    QString mod_name = pair.second.section("0|0", 1, 1);
    QString old_name = EditData::instance()->GetRoutMod(rout_name);

    if(!EditData::instance()->IsExist(old_name, rout_name))
    {
        QMessageBox::warning(this, tr("提示"), tr("无法找到该模块或例行程序"));
        return;
    }

    if(mod_name == old_name) //选了自身模块
    {
        QMessageBox::warning(this, tr("提示"), tr("您未选择新的模块"));
        return;
    }

    EditData::instance()->MoveRout(old_name, rout_name, mod_name);

    emit RevMod(old_name);

    ShowControl();
}

void Routine::ChangeState()
{
    if(change_state_widget_->name_edit_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入程序名"));
        return;
    }

    QPair<RoutineData, QString> pair = change_state_widget_->GetCode();

    RoutineData rout_data = pair.first;
    QString rout_name = pair.second.section("0|0", 0, 0);
    QString mod_name = pair.second.section("0|0", 1, 1);

    RoutineData &rout = EditData::instance()->GetRoutData(rout_name, mod_name);

    rout.type = rout_data.type;
    rout.data_type = rout_data.data_type;
    rout.param = rout_data.param;
    rout.local = rout_data.local;
    rout.revoke = rout_data.revoke;
    rout.error = rout_data.error;
    rout.back = rout_data.back;

    rout_data.code.removeAt(0); //去除掉<SMT>

    rout.code.append(rout_data.code);

    emit RevMod(mod_name);

    ShowControl();
}

void Routine::RenameRout(QString new_name)
{
    if(new_name.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入程序名"));
        return;
    }

    auto *curr_item = table_box_->currentItem();
    if(curr_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("无法查找到要修改的例行程序"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("无法查找到要修改的例行程序"));
        return;
    }

    QString old_name = name_item->text();
    old_name = old_name.mid(0, old_name.indexOf("("));

    EditData::instance()->RemRout(old_name, mod_name_, new_name);

    emit RevMod(mod_name_);

    ShowControl();
}

void Routine::DelRout()
{
    auto *curr_item = table_box_->currentItem();
    if(curr_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }

    QString rout_name = name_item->text();
    rout_name = rout_name.mid(0, rout_name.indexOf("("));

    EditData::instance()->DelRout(rout_name, mod_name_);

    emit RevMod(mod_name_);
}

void Routine::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);
    resizeEvent(nullptr);

    table_box_->SetScale(scale_w, scale_h);
    create_routine_widget_->SetScale(scale_w, scale_h);
    copy_routine_widget_->SetScale(scale_w, scale_h);
    move_routine_widget_->SetScale(scale_w, scale_h);
    keyboard_->SetScale(scale_w, scale_h);
    filter_key_->SetScale(scale_w, scale_h);
}

void Routine::HideControl()
{
    main_widget_->hide();
    button_widget_->hide();
}

void Routine::ShowControl()
{
    if(!create_routine_widget_->isHidden())
    {
        create_routine_widget_->hide();
    }

    if(!copy_routine_widget_->isHidden())
    {
        copy_routine_widget_->hide();
    }

    if(!move_routine_widget_->isHidden())
    {
        move_routine_widget_->hide();
    }

    if(!change_state_widget_->isHidden())
    {
        change_state_widget_->hide();
    }

    if(!keyboard_->isHidden())
    {
        keyboard_->hide();
    }

    main_widget_->show();
    button_widget_->show();
}

void Routine::ShowCreateRoutine()
{
    if(mod_name_.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择模块"));
        return;
    }

    if(EditData::instance()->GetModType(mod_name_))
        return;

    HideControl();
    int control_height = this->height() - GetIcoLabel()->height() - GetUnderLabel()->height();
    int control_y = GetUnderLabel()->y() + GetUnderLabel()->height();
    create_routine_widget_->resize(this->width(), control_height);
    create_routine_widget_->move(0, control_y);
    create_routine_widget_->show();
    create_routine_widget_->Init(mod_name_);
}

void Routine::ShowCopyRoutine()
{
    auto *curr_item = table_box_->currentItem();
    if(curr_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }

    QString name = name_item->text();
    name.replace(" ", "");
    name = name.mid(0, name.indexOf("("));

    RoutineData &rou = EditData::instance()->GetRoutData(name);

    copy_routine_widget_->Init(EditData::instance()->GetRoutMod(name));

    copy_routine_widget_->name_edit_->setText(name + "Copy");

    if(rou.type == RoutineData::PROC)
        copy_routine_widget_->type_box_->setCurrentIndex(0);
    else if(rou.type == RoutineData::FUNC)
        copy_routine_widget_->type_box_->setCurrentIndex(1);
    else
        copy_routine_widget_->type_box_->setCurrentIndex(2);

    copy_routine_widget_->param_edit_->setText(rou.param);
    copy_routine_widget_->data_type_box_->setCurrentText(rou.data_type);

    if(rou.local)
        copy_routine_widget_->local_box_->setCheckState(Qt::Checked);
    else
        copy_routine_widget_->local_box_->setCheckState(Qt::Unchecked);
    if(rou.revoke)
        copy_routine_widget_->revoke_box_->setCheckState(Qt::Checked);
    else
        copy_routine_widget_->revoke_box_->setCheckState(Qt::Unchecked);
    if(rou.error)
        copy_routine_widget_->error_box_->setCheckState(Qt::Checked);
    else
        copy_routine_widget_->error_box_->setCheckState(Qt::Unchecked);
    if(rou.back)
        copy_routine_widget_->retral_box_->setCheckState(Qt::Checked);
    else
        copy_routine_widget_->retral_box_->setCheckState(Qt::Unchecked);

    HideControl();
    int control_height = this->height() - GetIcoLabel()->height() - GetUnderLabel()->height();
    int control_y = GetUnderLabel()->y() + GetUnderLabel()->height();
    copy_routine_widget_->resize(this->width(), control_height);
    copy_routine_widget_->move(0, control_y);
    copy_routine_widget_->show();
}

void Routine::ShowMoveRoutine()
{
    auto *curr_item = table_box_->currentItem();
    if(curr_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }

    QString name = name_item->text();
    name.replace(" ", "");
    name = name.mid(0, name.indexOf("("));

    RoutineData &rou = EditData::instance()->GetRoutData(name);

    move_routine_widget_->Init(EditData::instance()->GetRoutMod(name));

    move_routine_widget_->name_edit_->setText(name);

    if(rou.type == RoutineData::PROC)
        move_routine_widget_->type_box_->setCurrentIndex(0);
    else if(rou.type == RoutineData::FUNC)
        move_routine_widget_->type_box_->setCurrentIndex(1);
    else
        move_routine_widget_->type_box_->setCurrentIndex(2);

    move_routine_widget_->param_edit_->setText(rou.param);
    move_routine_widget_->data_type_box_->setCurrentText(rou.data_type);

    if(rou.local)
        move_routine_widget_->local_box_->setCheckState(Qt::Checked);
    else
        move_routine_widget_->local_box_->setCheckState(Qt::Unchecked);
    if(rou.revoke)
        move_routine_widget_->revoke_box_->setCheckState(Qt::Checked);
    else
        move_routine_widget_->revoke_box_->setCheckState(Qt::Unchecked);
    if(rou.error)
        move_routine_widget_->error_box_->setCheckState(Qt::Checked);
    else
        move_routine_widget_->error_box_->setCheckState(Qt::Unchecked);
    if(rou.back)
        move_routine_widget_->retral_box_->setCheckState(Qt::Checked);
    else
        move_routine_widget_->retral_box_->setCheckState(Qt::Unchecked);

    HideControl();
    int control_height = this->height() - GetIcoLabel()->height() - GetUnderLabel()->height();
    int control_y = GetUnderLabel()->y() + GetUnderLabel()->height();
    move_routine_widget_->resize(this->width(), control_height);
    move_routine_widget_->move(0, control_y);
    move_routine_widget_->show();
}

void Routine::ShowChangeState()
{
    auto *curr_item = table_box_->currentItem();
    if(curr_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }

    QString name = name_item->text();
    name.replace(" ", "");
    name = name.mid(0, name.indexOf("("));

    RoutineData &rou = EditData::instance()->GetRoutData(name);

    change_state_widget_->Init(EditData::instance()->GetRoutMod(name));

    change_state_widget_->name_edit_->setText(name);

    if(rou.type == RoutineData::PROC)
        change_state_widget_->type_box_->setCurrentIndex(0);
    else if(rou.type == RoutineData::FUNC)
        change_state_widget_->type_box_->setCurrentIndex(1);
    else
        change_state_widget_->type_box_->setCurrentIndex(2);

    change_state_widget_->param_edit_->setText(rou.param);
    change_state_widget_->data_type_box_->setCurrentText(rou.data_type);

    if(rou.local)
        change_state_widget_->local_box_->setCheckState(Qt::Checked);
    else
        change_state_widget_->local_box_->setCheckState(Qt::Unchecked);
    if(rou.revoke)
        change_state_widget_->revoke_box_->setCheckState(Qt::Checked);
    else
        change_state_widget_->revoke_box_->setCheckState(Qt::Unchecked);
    if(rou.error)
        change_state_widget_->error_box_->setCheckState(Qt::Checked);
    else
        change_state_widget_->error_box_->setCheckState(Qt::Unchecked);
    if(rou.back)
        change_state_widget_->retral_box_->setCheckState(Qt::Checked);
    else
        change_state_widget_->retral_box_->setCheckState(Qt::Unchecked);

    change_state_widget_->data_type_box_->setEnabled(false);
    change_state_widget_->show_type_button_->setEnabled(false);

    HideControl();
    int control_height = this->height() - GetIcoLabel()->height() - GetUnderLabel()->height();
    int control_y = GetUnderLabel()->y() + GetUnderLabel()->height();
    change_state_widget_->resize(this->width(), control_height);
    change_state_widget_->move(0, control_y);
    change_state_widget_->show();
}

void Routine::ShowRenameRout()
{
    auto *curr_item = table_box_->currentItem();
    if(curr_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }
    auto *name_item = table_box_->item(table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择例行程序"));
        return;
    }

    QString name = name_item->text();
    name = name.mid(0, name.indexOf("("));

    HideControl();
    int control_height = this->height() - GetIcoLabel()->height() - GetUnderLabel()->height();
    int control_y = GetUnderLabel()->y() + GetUnderLabel()->height();
    keyboard_->resize(this->width(), control_height);
    keyboard_->move(0, control_y);

    keyboard_->Init(name);
    keyboard_->show();
}

AddCodeWindow::AddCodeWindow(QWidget *parent) : QFrame(parent)
{
    stack_widget_ = new StackedWidObject(this);
    show_type_ = new ComboBoxObject(this);
    type_widget_ = new ACWButton("main", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(show_type_);
    layout->addWidget(stack_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    show_type_->SetIsMenu(false); //设置为不显示菜单，虽然不设置也可以但是设置了可以提高运行速度
    stack_widget_->SetMainWidget(type_widget_);
    type_widget_->HideButton();

    QPalette p(this->palette());
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    this->setFrameShape(QFrame::Box);

    code_map_ = EditData::instance()->GetCodeMap();
    ReadCode();
    if(stack_widget_->count() >= 1)
    {
        stack_widget_->setCurrentIndex(1);
        show_type_->setText(stack_widget_->GetName(1));
    }

    connect(type_widget_, &ACWButton::ShowWindow, show_type_, &ComboBoxObject::UpperArrow);
    connect(type_widget_, &ACWButton::HideWindow, show_type_, &ComboBoxObject::LowerArrow);
    connect(show_type_, &ComboBoxObject::clicked, stack_widget_, &StackedWidObject::ChangeMain);
    connect(TeachState::instance().data(), &TeachState::RunMod,
            [=](RunMode mode)
    {
        switch(mode)
        {
        case RunMode::ManualMod:
        {
            this->SetEnableAll(true);
            return;
        }
        default:
        {
            this->SetEnableAll(false);
            return;
        }
        }
    });
}

void AddCodeWindow::ReadCode()
{
    button_list_.clear();
    QString path = qApp->applicationDirPath() + "/data/RAPID/Code/"; //存放指令的文件夹
    QDir dir(path);
    if(!dir.exists()) //如果该文件夹不存在
    {
        dir.mkpath(path);
        QMessageBox::warning(this, tr("警告"), tr("无法寻找到指令"));
        return;
    }
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks); //只获取文件
    QStringList path_list = dir.entryList();
    for(auto name : path_list)
    {
        QFile file(path + name);
        file.open(QIODevice::ReadOnly);
        if(name == "=") //=实际上是!=因为Windows系统不允许文件名出现:为保证平台兼容性而采用=
        {
            name = ":=";
        }
        QString type_text = file.readLine(); //读取第一行获得该指令类型，注意类型可能不止一个
        QString data = file.readAll(); //将剩余数据（也就是使用格式）全部读取出来
        if(data[data.size() - 1] == '\n') //去除末尾的换行符
        {
            data = data.mid(0, data.size() - 1);
        }
        QStringList type_list = type_text.split(","); //对该指令的类型进行拆分
        for(QString type : type_list)
        {
            type = type.replace("\n", "");
            if(type_map_.contains(type)) //如果该窗口已存在
            {
                ACWButton *w_button = dynamic_cast<ACWButton*>(stack_widget_->GetWidget(type)); //获取对应类型窗口
                QPushButton *button = new QPushButton(name, this);
                code_map_->insert(name, data);
                w_button->AddButton(button);
                button_list_.append(button);
                connect(button, &QPushButton::clicked, this, &AddCodeWindow::ButtonClicked);
            }
            else //如果不存在则需要先创建该类型窗口然后再添加
            {
                ACWButton *w_button = new ACWButton(type, this);
                if(!stack_widget_->AddWidget(w_button, type)) //如果添加窗口失败
                {
                    delete w_button; //先释放内存
                    continue; //在终止本次循环
                }
                else
                {
                    type_map_.insert(type, stack_widget_->GetNum(type));
                    code_map_->insert(name, data);
                    QPushButton *type_button = new QPushButton(type, this);
                    type_widget_->AddButton(type_button);
                    QPushButton *button = new QPushButton(name, this);
                    w_button->AddButton(button);
                    button_list_.append(button);
                    connect(button, &QPushButton::clicked, this, &AddCodeWindow::ButtonClicked);
                    connect(type_button, &QPushButton::clicked, this, &AddCodeWindow::ChangeWindow);
                }
                connect(w_button, &ACWButton::UpperButton, this, &AddCodeWindow::UpperButton);
                connect(w_button, &ACWButton::LowerButton, this, &AddCodeWindow::LowerButton);
            }
        }
    }
}

void AddCodeWindow::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void AddCodeWindow::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void AddCodeWindow::SetEnableAll(bool enable)
{
    for(auto &button : button_list_)
        button->setEnabled(enable);
}

void AddCodeWindow::ButtonClicked()
{
    QPushButton *button = dynamic_cast<QPushButton*>(sender());
    emit AddCode(button->text(), true);
}

void AddCodeWindow::ChangeWindow()
{
    QPushButton *button = dynamic_cast<QPushButton*>(sender());
    stack_widget_->Change(button->text());
    show_type_->setText(button->text());
}

void AddCodeWindow::UpperButton(QString name)
{
    int num = stack_widget_->GetNum(name);
    if(num <= 1) //因为主窗口占据了0索引
    {
        stack_widget_->setCurrentIndex(stack_widget_->GetCount() - 1); //跳转到最后一页
        show_type_->setText(stack_widget_->GetName(stack_widget_->GetCount() - 1));
    }
    else
    {
        stack_widget_->setCurrentIndex(num - 1);
        show_type_->setText(stack_widget_->GetName(num - 1));
    }
}

void AddCodeWindow::LowerButton(QString name)
{
    int num = stack_widget_->GetNum(name);
    if(num >= stack_widget_->GetCount() - 1) //如果已经是最后一页了
    {
        stack_widget_->setCurrentIndex(1); //跳转到第一页
        show_type_->setText(stack_widget_->GetName(1));
    }
    else
    {
        stack_widget_->setCurrentIndex(num + 1);
        show_type_->setText(stack_widget_->GetName(num + 1));
    }
}

ACWButton::ACWButton(QString name, QWidget *parent) : QWidget(parent)
{
    stack_widget_ = new QStackedWidget(this);
    upper_button_ = new QPushButton(tr("<- 上一个"), this);
    lower_button_ = new QPushButton(tr("下一个 ->"), this);
    QHBoxLayout *button_layout = new QHBoxLayout;
    QVBoxLayout *layout = new QVBoxLayout(this);
    button_layout->addWidget(upper_button_);
    button_layout->addWidget(lower_button_);
    layout->addWidget(stack_widget_);
    layout->addLayout(button_layout);
    layout->setStretchFactor(stack_widget_, 6);
    layout->setStretchFactor(button_layout, 1);
    w_layout_ = nullptr;
    button_layout_ = nullptr;
    save_btton_num_ = 0;
    name_ = name;
    layout_num_ = 0;
    this->setLayout(layout);
    connect(upper_button_, &QPushButton::clicked, this, &ACWButton::UpperClicket);
    connect(lower_button_, &QPushButton::clicked, this, &ACWButton::LowerClicket);
}

void ACWButton::AddButton(QPushButton *button1, QPushButton *button2)
{
    if(save_btton_num_ >= 12) //如果当前页已存放满了按钮
    {
        w_layout_ = nullptr;
    }
    if(w_layout_ == nullptr) //无窗口时或上一页窗口已满需要添加新窗口时
    {
        QWidget *w = new QWidget(this);
        w_layout_ = new QVBoxLayout(w);
        stack_widget_->addWidget(w);
        save_btton_num_ = 0;
        button_layout_ = nullptr;
    }
    if(layout_num_ >= 2)
    {
        button_layout_ = nullptr;
    }
    if(button_layout_ == nullptr)
    {
        button_layout_ = new QHBoxLayout;
        layout_num_ = 0;
        if(button2 == nullptr)
        {
            button_layout_->addWidget(button1);
            layout_num_++;
        }
        else
        {
            button_layout_->addWidget(button1);
            button_layout_->addWidget(button2);
            layout_num_ = 2;
        }
        w_layout_->addLayout(button_layout_);
    }
    else
    {
        if(button2 == nullptr)
        {
            button_layout_->addWidget(button1);
            layout_num_++;
        }
        else
        {
            button_layout_->addWidget(button1);
            button_layout_ = new QHBoxLayout;
            button_layout_->addWidget(button2);
            layout_num_ = 1;
            w_layout_->addLayout(button_layout_);
        }
    }
}

void ACWButton::SetNum(QString name)
{
    name_ = name;
}

void ACWButton::HideButton()
{
    upper_button_->hide();
    lower_button_->hide();
}

void ACWButton::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void ACWButton::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void ACWButton::UpperClicket()
{
    int size = stack_widget_->count() - 1; //获取总窗口页数
    int num = stack_widget_->currentIndex(); //获取当前窗口页数
    if(size == 0 || num == 0)
    {
        emit UpperButton(name_);
    }
    else
    {
        stack_widget_->setCurrentIndex(num - 1);
    }
}

void ACWButton::LowerClicket()
{
    int size = stack_widget_->count() - 1; //获取总窗口页数
    int num = stack_widget_->currentIndex(); //获取当前窗口页数
    if(size == 0 || num >= size)
    {
        emit LowerButton(name_);
    }
    else
    {
        stack_widget_->setCurrentIndex(num + 1);
    }
}

CreateModular::CreateModular(QWidget *parent) : QWidget(parent)
{
    decorate_label_ = new QLabel(tr("新模块"), this);
    tips_name_label_ = new QLabel(tr("名称:"), this);
    tips_type_label_ = new QLabel(tr("类型:"), this);
    keyboard_ = new Keyboard(this);
    key_button_ = new QPushButton("ABC...", this);
    button_widget_ = new QWidget(this);
    name_edit_ = new QLineEdit(this);
    type_box_ = new QComboBox(this);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    scale_w_ = scale_h_ = 1;
    type_box_->addItem("Program");
    //type_box_->addItem("System");
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    keyboard_->hide();
    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    p.setColor(QPalette::Text, QColor(72, 128, 0));
    name_edit_->setPalette(p);
    connect(key_button_, &QPushButton::clicked, this, &CreateModular::HideControl);
    connect(keyboard_->cancel_button_, &QPushButton::clicked, this, &CreateModular::ShowControl);
    connect(keyboard_, &Keyboard::GetData, this, &CreateModular::GetData);
}

void CreateModular::resizeEvent(QResizeEvent *)
{
    decorate_label_->resize(this->width(), 119 * scale_h_);
    tips_name_label_->resize(215 * scale_w_, 31 * scale_h_);
    tips_type_label_->resize(tips_name_label_->size());
    key_button_->resize(100 * scale_w_, 31 * scale_h_);
    button_widget_->resize(this->width(), 43 * scale_h_);
    name_edit_->resize(231 * scale_w_, 31 * scale_h_);
    type_box_->resize(331 * scale_w_, 31 * scale_h_);
    define_button_->resize(126 * scale_w_, button_widget_->height());
    cancel_button_->resize(define_button_->size());
    decorate_label_->move(5 * scale_w_, 0);
    tips_name_label_->move(51 * scale_w_, decorate_label_->y() + decorate_label_->height());
    name_edit_->move(tips_name_label_->x() + tips_name_label_->width(), tips_name_label_->y());
    key_button_->move(name_edit_->x() + name_edit_->width() + scale_w_, name_edit_->y());
    button_widget_->move(0, 321 * scale_h_);
    tips_type_label_->move(tips_name_label_->x(), tips_name_label_->y() + tips_name_label_->height() + 25 * scale_h_);
    type_box_->move(tips_type_label_->x() + tips_type_label_->width(), tips_type_label_->y());
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    define_button_->move(cancel_button_->x() - define_button_->width(), cancel_button_->y());
    if(!keyboard_->isHidden())
    {
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
    }
}

void CreateModular::Init()
{
    name_edit_->setText(EditData::instance()->GetUnusedMod());
}

void CreateModular::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;
    resizeEvent(nullptr);
    keyboard_->SetScale(scale_w_, scale_h_);
}

QLabel *CreateModular::GetDecoLabel()
{
    return decorate_label_;
}

void CreateModular::GetData(QString data)
{
    name_edit_->setText(data);
    ShowControl();
}

void CreateModular::ShowControl()
{
    keyboard_->hide();
    decorate_label_->show();
    tips_name_label_->show();
    tips_type_label_->show();
    key_button_->show();
    button_widget_->show();
    name_edit_->show();
    type_box_->show();
}

void CreateModular::HideControl()
{
    keyboard_->Init(name_edit_->text());
    keyboard_->show();
    resizeEvent(nullptr);
    decorate_label_->hide();
    tips_name_label_->hide();
    tips_type_label_->hide();
    key_button_->hide();
    button_widget_->hide();
    name_edit_->hide();
    type_box_->hide();
}

LoadModular::LoadModular(QWidget *parent) : FileMangerObject(parent, true)
{
    filter_box_ = new QComboBox(this);
    tips_name_label_ = new QLabel(tr("文件名:"), this);
    file_edit_ = new QLineEdit(this);
    button_widget_ = new QWidget(this);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    create_button_->setParent(button_widget_);
    upper_button_->setParent(button_widget_);
    home_button_->setParent(button_widget_);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    filter_box_->addItem("*.MOD");
    filter_box_->addItem("*.SYS");
    filter_box_->addItem("*.*");
    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    AddFilter("*.MOD"); //初始设置过滤器
    Refresh(); //刷新
    connect(filelist_, &FileList::currentRowChanged, this, &LoadModular::TogglePagesi);
    connect(filter_box_, &QComboBox::currentTextChanged,
            [ = ](const QString & str)
    {
        QStringList filter;
        filter.append(str);
        FileFilterClear();
        SetFileFilter(filter);
        Refresh();
    });
}

void LoadModular::resizeEvent(QResizeEvent *)
{   
    double &scalew = scale_w_;
    double scaleh = scale_h_;

    FileMangerObject::resizeEvent(nullptr);
    filter_box_->resize(299 * scalew, 31 * scaleh);
    tips_name_label_->resize(110 * scalew, 25 * scaleh);
    file_edit_->resize(504 * scalew, 25 * scaleh);
    filelist_->resize(filelist_->width(), filelist_->height() - 47 * scaleh);
    button_widget_->resize(this->width(), 43 * scaleh);
    define_button_->resize(128 * scalew, button_widget_->height());
    cancel_button_->resize(define_button_->size());
    filter_box_->move(show_button_->x() + show_button_->width() + 231 * scalew, show_button_->y());
    tips_name_label_->move(filelist_->x(), filelist_->y() + filelist_->height() + 15 * scaleh);
    file_edit_->move(tips_name_label_->x() + tips_name_label_->width(), tips_name_label_->y());
    button_widget_->move(0, file_edit_->y() + file_edit_->height() + 7 * scaleh);
    create_button_->move(0, 0);
    upper_button_->move(create_button_->x() + create_button_->width(), create_button_->y());
    home_button_->move(upper_button_->x() + upper_button_->width(), upper_button_->y());
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    define_button_->move(cancel_button_->x() - cancel_button_->width(), cancel_button_->y());
}

void LoadModular::InIt()
{
    file_edit_->clear();
    mod_path_.clear();
    filter_box_->setCurrentIndex(0);
    ReturnHome();
}

QString LoadModular::CurrentText(QString text)
{
    file_edit_->setText(text.section(" ", 0, 0));
    mod_path_ = text.section(" ", 2, 2);
    return text;
}

QString LoadModular::GetModPath()
{
    return mod_path_;
}

void LoadModular::SetFilterBoxText(int row)
{
    if(row > filter_box_->currentIndex() || row < 0)
    {
        row = 0;
    }
    filter_box_->setCurrentIndex(row);
}

void LoadModular::SetFilterBoxText(QString text)
{
    int row = filter_box_->findText(text); //查找text在下拉框中的索引，不存在时为-1
    if(row == -1)
    {
        row = 0;
    }
    filter_box_->setCurrentIndex(row);
}

void LoadModular::SetScale(double scale_w, double scale_h)
{
    FileMangerObject::SetScale(scale_w, scale_h);
    resizeEvent(nullptr);
}

void LoadModular::ShowKeyboardWidget()
{
    FileMangerObject::ShowKeyboardWidget();
    filter_box_->hide();
    tips_name_label_->hide();
    button_widget_->hide();
    file_edit_->hide();
}

void LoadModular::HideKeyboardWidget()
{
    FileMangerObject::HideKeyboardWidget();
    filter_box_->show();
    tips_name_label_->show();
    button_widget_->show();
    file_edit_->show();
}

SaveModular::SaveModular(QWidget *parent) : LoadModular(parent)
{
    key_button_ = new QPushButton("...", this);
    keyboard_ = new Keyboard(this);
    keyboard_->hide();
    connect(key_button_, &QPushButton::clicked, this, &SaveModular::ShowKeyWidget);
    connect(keyboard_, &Keyboard::GetData, this, &SaveModular::GetData);
    connect(keyboard_->cancel_button_, &QPushButton::clicked, this, &SaveModular::HideKeyWidget);
}

void SaveModular::resizeEvent(QResizeEvent *)
{
    double &scalew = scale_w_;

    LoadModular::resizeEvent(nullptr);
    file_edit_->resize(424 * scalew, file_edit_->height());
    key_button_->resize(80 * scalew, file_edit_->height());
    key_button_->move(file_edit_->x() + file_edit_->width(), file_edit_->y());
    if(!keyboard_->isHidden())
    {
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
        sort_button_->hide();
        type_label_->hide();
    }
}

void SaveModular::SetScale(double scale_w, double scale_h)
{
    LoadModular::SetScale(scale_w, scale_h);
    keyboard_->SetScale(scale_w, scale_h);
    resizeEvent(nullptr);
}

void SaveModular::ShowKeyboardWidget()
{
    LoadModular::ShowKeyboardWidget();
    key_button_->hide();
}

void SaveModular::HideKeyboardWidget()
{
    LoadModular::HideKeyboardWidget();
    key_button_->show();
}

void SaveModular::ShowKeyWidget()
{
    keyboard_->Init();
    keyboard_->show();
    keyboard_->resize(this->size());
    keyboard_->move(0, 0);
    sort_button_->hide();
    show_button_->hide();
    hide_button_->hide();
    type_label_->hide();
    filelist_->hide();
    filter_box_->hide();
    tips_name_label_->hide();
    button_widget_->hide();
    file_edit_->hide();
    key_button_->hide();
}

void SaveModular::HideKeyWidget()
{
    keyboard_->hide();
    sort_button_->show();
    show_button_->show();
    hide_button_->show();
    type_label_->show();
    filelist_->show();
    filter_box_->show();
    tips_name_label_->show();
    button_widget_->show();
    file_edit_->show();
    key_button_->show();
}

void SaveModular::GetData(QString text)
{
    if(text.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入内容"));
        return;
    }
    file_edit_->setText(text + filter_box_->currentText().mid(1));
    HideKeyWidget();
}

RoutineWidget::RoutineWidget(bool show_task, bool show_tips, QWidget *parent) : QWidget(parent)
{
    show_task_ = show_task;
    show_tips_ = show_tips;
    main_widget_ = new QWidget(this);
    QVBoxLayout *main_layout_ = new QVBoxLayout(main_widget_);
    main_widget_->setLayout(main_layout_);
    tips_label_ = new QLabel(tr("例行程序声明"), this);
    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    result_button_ = new QPushButton(tr("结果..."), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    result_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_layout->addWidget(result_button_);
    button_layout->addStretch(); //增加一个拉缩空间用于填充位置
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_widget_->setLayout(button_layout);
    QHBoxLayout *name_layout = new QHBoxLayout;
    name_label_ = new QLabel(tr("名称:"), this);
    name_edit_ = new QLineEdit(this);
    show_key_button_ = new QPushButton("ABC...", this);
    name_layout->addWidget(name_label_);
    name_layout->addWidget(name_edit_);
    name_layout->addWidget(show_key_button_);
    name_layout->setSpacing(0);
    name_layout->setStretchFactor(name_label_, 3);
    name_layout->setStretchFactor(name_edit_, 2);
    name_layout->setStretchFactor(show_key_button_, 1);
    QHBoxLayout *type_layout = new QHBoxLayout;
    type_label_ = new QLabel(tr("类型:"), this);
    type_box_ = new QComboBox(this);
    type_layout->addWidget(type_label_);
    type_layout->addWidget(type_box_);
    type_layout->setSpacing(0);
    QHBoxLayout *param_layout = new QHBoxLayout;
    param_label_ = new QLabel(tr("参数:"), this);
    param_edit_ = new QLineEdit(this);
    show_param_button_ = new QPushButton("...", this);
    param_layout->addWidget(param_label_);
    param_layout->addWidget(param_edit_);
    param_layout->addWidget(show_param_button_);
    param_layout->setSpacing(0);
    param_layout->setStretchFactor(param_label_, 3);
    param_layout->setStretchFactor(param_edit_, 2);
    param_layout->setStretchFactor(show_param_button_, 1);
    QHBoxLayout *data_type_layout = new QHBoxLayout;
    data_type_label_ = new QLabel(tr("数据类型:"), this);
    //data_type_edit_ = new QLineEdit("num", this);
    data_type_box_ = new QComboBox(this);
    show_type_button_ = new QPushButton("...", this);
    data_type_layout->addWidget(data_type_label_);
    //data_type_layout->addWidget(data_type_edit_);
    data_type_layout->addWidget(data_type_box_);
    data_type_layout->addWidget(show_type_button_);
    data_type_layout->setSpacing(0);
    data_type_layout->setStretchFactor(data_type_label_, 3);
    data_type_layout->setStretchFactor(data_type_box_, 2);
    data_type_layout->setStretchFactor(show_type_button_, 1);
    QHBoxLayout *task_layout = new QHBoxLayout;
    task_label_ = new QLabel(tr("任务"), this);
    task_box_ = new QComboBox(this);
    task_layout->addWidget(task_label_);
    task_layout->addWidget(task_box_);
    task_layout->setSpacing(0);
    QHBoxLayout *mod_layout = new QHBoxLayout;
    mod_label_ = new QLabel(tr("模块:"), this);
    mod_box_ = new QComboBox(this);
    mod_layout->addWidget(mod_label_);
    mod_layout->addWidget(mod_box_);
    mod_layout->setSpacing(0);
    QHBoxLayout *lr_layout = new QHBoxLayout;
    QLabel *local_label = new QLabel(tr("本地声明:"), this);
    local_box_ = new QCheckBox(this);
    QLabel *revoke_label = new QLabel(tr("撤销处理程序:"), this);
    revoke_box_ = new QCheckBox(this);
    revoke_label->setAlignment(Qt::AlignCenter);
    lr_layout->addWidget(local_label);
    lr_layout->addWidget(local_box_);
    lr_layout->addWidget(revoke_label);
    lr_layout->addWidget(revoke_box_);
    QHBoxLayout *er_layout = new QHBoxLayout;
    QLabel *error_label = new QLabel(tr("错误处理程序:"), this);
    error_box_ = new QCheckBox(this);
    QLabel *retral_label = new QLabel(tr("向后处理程序:"), this);
    retral_box_ = new QCheckBox(this);
    retral_label->setAlignment(Qt::AlignCenter);
    er_layout->addWidget(error_label);
    er_layout->addWidget(error_box_);
    er_layout->addWidget(retral_label);
    er_layout->addWidget(retral_box_);
    main_layout_->addLayout(name_layout);
    main_layout_->addLayout(type_layout);
    main_layout_->addLayout(param_layout);
    main_layout_->addLayout(data_type_layout);
    main_layout_->addLayout(task_layout);
    main_layout_->addLayout(mod_layout);
    main_layout_->addLayout(lr_layout);
    main_layout_->addLayout(er_layout);
    layout_ = new QVBoxLayout(this);
    layout_->addWidget(tips_label_);
    layout_->addWidget(main_widget_);
    layout_->addWidget(button_widget_);
    layout_->setStretchFactor(main_widget_, 5);
    layout_->setStretchFactor(button_widget_, 1);
    layout_->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout_);
    keyboard_ = new Keyboard(this);
    show_code_ = new ShowAddRoutine(this);
    param_widget_ = new ParamWidget(this);
    layout_->addWidget(keyboard_);
    layout_->addWidget(show_code_);
    layout_->addWidget(param_widget_);
    if(!show_tips_)
    {
        tips_label_->hide();
    }
    if(!show_task_)
    {
        task_label_->hide();
        task_box_->hide();
    }
    keyboard_->hide();
    show_code_->hide();
    param_widget_->hide();
    result_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    local_box_->setLayoutDirection(Qt::RightToLeft); //将复选框标签放到方框左边
    revoke_box_->setLayoutDirection(Qt::RightToLeft);
    error_box_->setLayoutDirection(Qt::RightToLeft);
    retral_box_->setLayoutDirection(Qt::RightToLeft);
    type_box_->addItem(tr("程序"));
    type_box_->addItem(tr("功能"));
    type_box_->addItem(tr("中断"));
    task_box_->addItem("T_ROB1");
    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    QStringList list = DataType::instance()->GetTypeName();
    data_type_box_->addItems(list);

    ConnectInit();
}

void RoutineWidget::SetShowTask(bool show_task)
{
    show_task_ = show_task;
}

void RoutineWidget::Init()
{
    name_edit_->setText(EditData::instance()->GetUnusedRout());
    type_box_->setCurrentIndex(0);
    param_edit_->clear();
    //data_type_edit_->setText("num");
    data_type_box_->setCurrentText("num");
    mod_box_->clear();

    QStringList list = EditData::instance()->GetModName(false);

    mod_box_->addItems(list);
}

void RoutineWidget::Init(QString mod_name)
{
    this->Init();

    mod_box_->setCurrentText(mod_name);
    show_type_button_->setText("");
    show_type_button_->setFlat(true);
}

QPair<RoutineData, QString> RoutineWidget::GetCode()
{
    RoutineData rout_info;
    QString rout_name;

    rout_name = name_edit_->text();
    rout_info.param = param_edit_->text();

    if(local_box_->checkState() == Qt::Checked)
    {
        rout_info.local = true;
    }
    if(type_box_->currentIndex() == 0) //程序
    {
        rout_info.type = RoutineData::PROC;
    }
    else if(type_box_->currentIndex() == 1) //功能
    {
        rout_info.type = RoutineData::FUNC;
        //rout_info.data_type = data_type_edit_->text();
        rout_info.data_type = data_type_box_->currentText();
    }
    else if(type_box_->currentIndex() == 2) //中断
    {
        rout_info.type = RoutineData::TRAP;
    }

    rout_info.code.append("<SMT>");

    if(revoke_box_->checkState() == Qt::Checked)
    {
        rout_info.revoke = true;
        rout_info.code.append("UNDO");
        rout_info.code.append("<SMT>");
    }
    if(error_box_->checkState() == Qt::Checked)
    {
        rout_info.error = true;
        rout_info.code.append("ERROR");
        rout_info.code.append("<SMT>");
    }
    if(retral_box_->checkState() == Qt::Checked)
    {
        rout_info.back = true;
        rout_info.code.append("BACKWARD");
        rout_info.code.append("<SMT>");
    }

    rout_info.param = param_edit_->text();

    rout_name.append("0|0" + mod_box_->currentText());

    return qMakePair(rout_info, rout_name);
}

void RoutineWidget::SetScale(double scale_w, double scale_h)
{
    keyboard_->SetScale(scale_w, scale_h);
    param_widget_->SetScale(scale_w, scale_h);
}

void RoutineWidget::HideControl()
{
    tips_label_->hide();
    button_widget_->hide();
    main_widget_->hide();
}

void RoutineWidget::ShowControl()
{
    if(!keyboard_->isHidden())
    {
        keyboard_->hide();
    }
    if(!show_code_->isHidden())
    {
        show_code_->hide();
    }
    if(!param_widget_->isHidden())
    {
        param_widget_->hide();
    }
    if(show_tips_)
    {
        tips_label_->show();
    }
    if(show_task_)
    {
        task_label_->show();
        task_box_->show();
    }
    button_widget_->show();
    main_widget_->show();
}

void RoutineWidget::ConnectInit()
{
    connect(show_key_button_, &QPushButton::clicked,
            [ = ]()
    {
        keyboard_->Init(name_edit_->text());
        keyboard_->show();
        HideControl();
    });

    connect(keyboard_, &Keyboard::GetData,
            [ = ](QString data)
    {
        if(data.isEmpty())
        {
            QMessageBox::warning(this, tr("提示"), tr("请输入内容"));
            return;
        }
        name_edit_->setText(data);
        ShowControl();
    });

    connect(param_widget_, &ParamWidget::clicked,
            [=]()
    {
        param_edit_->setText(param_widget_->GetParam());
        ShowControl();
    });

    connect(show_param_button_, &QPushButton::clicked,
            [=]()
    {
        if(param_edit_->text().isEmpty())
            param_widget_->Init();
        else
            param_widget_->Init(param_edit_->text());
        param_widget_->show();
        HideControl();
    });

    connect(type_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index)
    {
        if(index == 1)
        {
            data_type_box_->setEnabled(true);
            //data_type_edit_->setEnabled(true);
            show_type_button_->setEnabled(true);
            retral_box_->setCheckState(Qt::Unchecked);
            retral_box_->setEnabled(false);
        }
        else
        {
            data_type_box_->setEnabled(false);
            //data_type_edit_->setEnabled(false);
            show_type_button_->setEnabled(false);
            retral_box_->setEnabled(true);
        }
    });

    connect(keyboard_->cancel_button_, &QPushButton::clicked, this, &RoutineWidget::ShowControl);
    connect(show_code_->close_button_, &QPushButton::clicked, this, &RoutineWidget::ShowControl);
    connect(param_widget_, &ParamWidget::cacclicked, this, &RoutineWidget::ShowControl);
    connect(result_button_, &QPushButton::clicked,
            [ = ]()
    {
        QPair<RoutineData, QString> pair = this->GetCode();

        RoutineData rout_info = pair.first;
        QString rout_name = pair.second.section("0|0", 0, 0);

        QString code = EditData::instance()->ExportRout(rout_info, rout_name);

        show_code_->SetCode(code);
        show_code_->show();

        HideControl();
    });
}

ShowAddRoutine::ShowAddRoutine(QWidget *parent) : QWidget(parent)
{
    show_code_ = new QTextBrowser(this);
    button_widget_ = new QWidget(this);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);
    QLabel *dec_label = new QLabel(button_widget_);
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    layout->addWidget(show_code_);
    layout->addWidget(button_widget_);
    layout->setStretchFactor(show_code_, 4);
    layout->setStretchFactor(close_button_, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    button_layout->addWidget(dec_label);
    button_layout->addWidget(close_button_);
    button_layout->setStretchFactor(dec_label, 4);
    button_layout->setStretchFactor(close_button_, 1);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    button_widget_->setLayout(button_layout);
    this->setLayout(layout);
    close_button_->setFlat(true);
    show_code_->setContextMenuPolicy(Qt::NoContextMenu);
    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    p.setColor(QPalette::Text, QColor(72, 128, 0));
    show_code_->setPalette(p);
}

void ShowAddRoutine::SetCode(QString &code)
{
    show_code_->clear();
    show_code_->append(tr("结果:\n"));
    show_code_->append(code);
}

ParamWidget::ParamWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    QHBoxLayout *layout = new QHBoxLayout;
    keyboard_ = new Keyboard(this);
    parm_tree_ = new QTreeWidget(this);
    attribute_widget_ = new QWidget(this);
    button_widget_ = new QWidget(this);
    layout->addWidget(parm_tree_);
    layout->addWidget(attribute_widget_);
    layout->addWidget(keyboard_);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStretchFactor(parm_tree_, 1);
    layout->setStretchFactor(attribute_widget_, 1);
    QLabel *tips2_label = new QLabel(tr("属性:"), attribute_widget_);
    name_label_ = new QLabel(tr("名称"), attribute_widget_);
    type_label_ = new QLabel(tr("数据类型:"), attribute_widget_);
    argno_label_ = new QLabel("ArgNo:", attribute_widget_);
    altno_label_ = new QLabel("AltNo:", attribute_widget_);
    ask_label_ = new QLabel(tr("要求:"), attribute_widget_);
    pattern_label_ = new QLabel(tr("模式:"), attribute_widget_);
    dimen_label_ = new QLabel(tr("维数:"), attribute_widget_);
    QLabel *tips3_label = new QLabel(tr("值:"), attribute_widget_);
    name_button_ = new QPushButton(attribute_widget_);
    type_button_ = new QPushButton(attribute_widget_);
    argno_button_ = new QPushButton(attribute_widget_);
    altno_button_ = new QPushButton(attribute_widget_);
    ask_button_ = new QPushButton(attribute_widget_);
    pattern_box_ = new QComboBox(attribute_widget_);
    dimen_box_ = new QComboBox(attribute_widget_);
    QHBoxLayout *tips_layout = new QHBoxLayout;
    tips_layout->addWidget(tips2_label);
    tips_layout->addWidget(tips3_label);
    tips_layout->setSpacing(0);
    QHBoxLayout *name_layout = new QHBoxLayout;
    name_layout->addWidget(name_label_);
    name_layout->addWidget(name_button_);
    name_layout->setSpacing(0);
    QHBoxLayout *type_layout = new QHBoxLayout;
    type_layout->addWidget(type_label_);
    type_layout->addWidget(type_button_);
    type_layout->setSpacing(0);
    QHBoxLayout *argno_layout = new QHBoxLayout;
    argno_layout->addWidget(argno_label_);
    argno_layout->addWidget(argno_button_);
    argno_layout->setSpacing(0);
    QHBoxLayout *altno_layout = new QHBoxLayout;
    altno_layout->addWidget(altno_label_);
    altno_layout->addWidget(altno_button_);
    altno_layout->setSpacing(0);
    QHBoxLayout *ask_layout = new QHBoxLayout;
    ask_layout->addWidget(ask_label_);
    ask_layout->addWidget(ask_button_);
    ask_layout->setSpacing(0);
    QHBoxLayout *pattern_layout = new QHBoxLayout;
    pattern_layout->addWidget(pattern_label_);
    pattern_layout->addWidget(pattern_box_);
    pattern_layout->setSpacing(0);
    QHBoxLayout *dimen_layout = new QHBoxLayout;
    dimen_layout->addWidget(dimen_label_);
    dimen_layout->addWidget(dimen_box_);
    dimen_layout->setSpacing(0);
    QVBoxLayout *vb_layout = new QVBoxLayout(attribute_widget_);
    vb_layout->addLayout(tips_layout);
    vb_layout->addStretch();
    vb_layout->addLayout(name_layout);
    vb_layout->addStretch();
    vb_layout->addLayout(type_layout);
    vb_layout->addStretch();
    vb_layout->addLayout(argno_layout);
    vb_layout->addStretch();
    vb_layout->addLayout(altno_layout);
    vb_layout->addStretch();
    vb_layout->addLayout(ask_layout);
    vb_layout->addStretch();
    vb_layout->addLayout(pattern_layout);
    vb_layout->addStretch();
    vb_layout->addLayout(dimen_layout);
    vb_layout->addStretch();
    vb_layout->setSpacing(0);
    vb_layout->setContentsMargins(0, 0, 0, 0);
    attribute_widget_->setLayout(vb_layout);
    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    tips2_label->setAutoFillBackground(true);
    tips3_label->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    tips2_label->setPalette(p);
    tips3_label->setPalette(p);
    tips2_label->setFrameShape(QFrame::Box);
    tips3_label->setFrameShape(QFrame::Box);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    add_button_ = new ComboBoxObject(tr("添加"), button_widget_);
    del_button_ = new QPushButton(tr("删除"), button_widget_);
    upper_button_ = new QPushButton(tr("上"), button_widget_);
    lower_button_ = new QPushButton(tr("下"), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    button_layout->addWidget(add_button_);
    button_layout->addWidget(del_button_);
    button_layout->addWidget(upper_button_);
    button_layout->addWidget(lower_button_);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    parm_tree_->setHeaderLabel(tr("参数:"));
    parm_tree_->headerItem()->setBackgroundColor(0, QColor(192, 192, 192));
    keyboard_->hide();
    add_button_->setFlat(true);
    del_button_->setFlat(true);
    upper_button_->setFlat(true);
    lower_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    name_button_->setFlat(true);
    type_button_->setFlat(true);
    argno_button_->setFlat(true);
    altno_button_->setFlat(true);
    ask_button_->setFlat(true);
    main_layout->addLayout(layout);
    main_layout->addWidget(button_widget_);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);
    add_param_ = new QAction(tr("添加参数"), this);
    add_opt_param_ = new QAction(tr("添加可选参数"), this);
    add_share_param_ = new QAction(tr("添加可选共用参数"), this);
    add_button_->AppAction(add_param_);
    add_button_->AppAction(add_opt_param_);
    add_button_->AppAction(add_share_param_);
    this->setLayout(main_layout);
    argno_button_->setEnabled(false);
    altno_button_->setEnabled(false);
    auto qss = "QMenu::item{padding-left:0px;}";
    add_button_->GetMenu()->setStyleSheet(qss);
    ask_button_->setEnabled(false);

    HideAttr();

    pattern_box_->addItem("In");
    pattern_box_->addItem("InOut");
    pattern_box_->addItem("Var");
    pattern_box_->addItem("Pers");
    dimen_box_->addItem("0");
    dimen_box_->addItem("1");
    dimen_box_->addItem("2");
    dimen_box_->addItem("3");

    connect(add_param_, &QAction::triggered, this, &ParamWidget::AddParm);
    connect(keyboard_->cancel_button_, &QPushButton::clicked, this, &ParamWidget::ShowControl);
    connect(keyboard_, &Keyboard::GetData, this, &ParamWidget::GetData);
    connect(add_opt_param_, &QAction::triggered, this, &ParamWidget::AddOptParam);
    connect(add_share_param_, &QAction::triggered, this, &ParamWidget::AddShareParam);
    connect(parm_tree_, &QTreeWidget::itemClicked, this, &ParamWidget::ParmClicked);
    connect(upper_button_, &QPushButton::clicked, this, &ParamWidget::UpperButtonClicked);
    connect(lower_button_, &QPushButton::clicked, this, &ParamWidget::LowerButtonClicked);
    connect(del_button_, &QPushButton::clicked, this, &ParamWidget::DeleteButtonClicked);
    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        emit clicked();
    });
    connect(cancel_button_, &QPushButton::clicked,
            [=]()
    {
        emit cacclicked();
    });
    connect(name_button_, &QPushButton::clicked,
            [ = ]()
    {
        add_type_ = 3;
        HideControl();
        keyboard_->Init(name_button_->text());
        keyboard_->show();
    });
    connect(pattern_box_, &QComboBox::currentTextChanged,
            [ = ](const QString & text)
    {
        param_map_[name_button_->text()].pattern = text;
    });
    connect(dimen_box_, &QComboBox::currentTextChanged,
            [ = ](const QString & text)
    {
        param_map_[name_button_->text()].dimen = text;
    });
}

void ParamWidget::SetParam(QString &code)
{
    param_map_.clear();
    if(code.isEmpty())
    {
        return;
    }
}

QString ParamWidget::GetParam()
{
    QString code;

    QTreeWidgetItemIterator it(parm_tree_);

    while(*it)
    {
        QString str;
        QTreeWidgetItem *item = (*it);
        QString name = item->text(0);
        ParamInfo &info = param_map_[name];

        if(info.isadd)
        {
            str.append("\\");
            if(info.pattern != "In")
                str.append(info.pattern + " ");
            str.append(info.type + " " + name);
            if(info.dimen == "1")
                str.append("{*}");
            else if(info.dimen == "2")
                str.append("{*,*}");
            else if(info.dimen == "3")
                str.append("{*,*,*}");

            it++;
            QString s;
            while(*it)
            {
                if((*it)->parent() != item)
                    break;

                QString s_name = (*it)->text(0);
                ParamInfo &s_info = param_map_[s_name];

                s.append(" | ");
                if(s_info.pattern != "In")
                    s.append(s_info.pattern + " ");
                s.append(s_info.type + " " + s_name);
                if(s_info.dimen == "1")
                    s.append("{*}");
                else if(s_info.dimen == "2")
                    s.append("{*,*}");
                else if(s_info.dimen == "3")
                    s.append("{*,*,*}");

                it++;
            }

            str.append(s);
            if(code.isEmpty())
                code.append(str);
            else
                code.append(", " + str);

            continue;
        }
        else
        {
            if(info.pattern != "In")
                str.append(info.pattern + " ");
            str.append(info.type + " " + name);
            if(info.dimen == "1")
                str.append("{*}");
            else if(info.dimen == "2")
                str.append("{*,*}");
            else if(info.dimen == "3")
                str.append("{*,*,*}");

            if(code.isEmpty())
                code.append(str);
            else
                code.append(", " + str);
        }
        it++;
    }
    return code;
}

void ParamWidget::Init()
{
    param_map_.clear();
    parm_tree_->clear();

    HideAttr();
}

void ParamWidget::Init(QString code)
{
    param_map_.clear();
    parm_tree_->clear();

    QStringList str_list = code.split(", ");
    int argno = 0;

    for(QString str : str_list)
    {
        if(str.at(0) == '\\')
        {
            str = str.mid(1);

            QStringList s_list = str.split(" | ");

            QTreeWidgetItem *item = nullptr;
            int altno = 0;
            for(int i = 0; i < s_list.size(); i++)
            {
                QString name;
                ParamInfo info;
                QString s = s_list[i];
                info.argno = argno;
                info.altno = altno++;
                info.ask = false;
                info.isadd = true;

                if(i == 0)
                {
                    item = new QTreeWidgetItem;

                    QString head = s.section(" ", 0, 0);
                    if(head == "InOut" || head == "Var" || head == "Pers")
                    {
                        info.pattern = head;
                        s = s.mid(head.size() + 1);
                    }
                    else
                        info.pattern = "In";
                    info.type = s.section(" ", 0, 0);
                    QString dimen_name = s.section(" ", 1, 1);
                    int x = dimen_name.indexOf("{");
                    if(x != -1)
                    {
                        name = dimen_name.mid(0, dimen_name.indexOf("{"));
                        QString dimen = dimen_name.mid(name.size() + 1);
                        if(dimen == "{*}")
                            info.dimen = "1";
                        else if(dimen == "{*,*}")
                            info.dimen = "2";
                        else if(dimen == "{*,*,*}")
                            info.dimen = "3";
                        else
                            info.dimen = "0";
                    }
                    else
                    {
                        name = dimen_name;
                        info.dimen = "0";
                    }
                    item->setText(0, name);
                    parm_tree_->addTopLevelItem(item);
                    param_map_.insert(name, info);
                }
                else
                {
                    QTreeWidgetItem *it = new QTreeWidgetItem;

                    QString head = s.section(" ", 0, 0);
                    if(head == "InOut" || head == "Var" || head == "Pers")
                    {
                        info.pattern = head;
                        s = s.mid(head.size() + 1);
                    }
                    else
                        info.pattern = "In";
                    info.type = s.section(" ", 0, 0);
                    QString dimen_name = s.section(" ", 1, 1);
                    int x = dimen_name.indexOf("{");
                    if(x != -1)
                    {
                        name = dimen_name.mid(0, dimen_name.indexOf("{"));
                        QString dimen = dimen_name.mid(name.size() + 1);
                        if(dimen == "{*}")
                            info.dimen = "1";
                        else if(dimen == "{*,*}")
                            info.dimen = "2";
                        else if(dimen == "{*,*,*}")
                            info.dimen = "3";
                        else
                            info.dimen = "0";
                    }
                    else
                    {
                        name = dimen_name;
                        info.dimen = "0";
                    }

                    it->setText(0, name);
                    item->addChild(it);
                    param_map_.insert(name, info);
                }
            }
        }
        else
        {
            QTreeWidgetItem *item = new QTreeWidgetItem;
            QString name;
            ParamInfo info;
            info.argno = argno;
            info.altno = 0;
            info.ask = true;
            info.isadd = false;

            QString head = str.section(" ", 0, 0);
            if(head == "InOut" || head == "Var" || head == "Pers")
            {
                info.pattern = head;
                str = str.mid(head.size() + 1);
            }
            else
                info.pattern = "In";
            info.type = str.section(" ", 0, 0);
            QString dimen_name = str.section(" ", 1, 1);
            int x = dimen_name.indexOf("{");
            if(x != -1)
            {
                name = dimen_name.mid(0, dimen_name.indexOf("{"));
                QString dimen = dimen_name.mid(name.size() + 1);
                if(dimen == "{*}")
                    info.dimen = "1";
                else if(dimen == "{*,*}")
                    info.dimen = "2";
                else if(dimen == "{*,*,*}")
                    info.dimen = "3";
                else
                    info.dimen = "0";
            }
            else
            {
                name = dimen_name;
                info.dimen = "0";
            }

            item->setText(0, name);
            parm_tree_->addTopLevelItem(item);
            param_map_.insert(name, info);
        }
        argno++;
    }

    if(name_button_->isHidden() && !param_map_.isEmpty())
    {
        ShowAttr();
    }
}

void ParamWidget::HideAttr()
{
    name_label_->hide();
    type_label_->hide();
    argno_label_->hide();
    altno_label_->hide();
    ask_label_->hide();
    pattern_label_->hide();
    dimen_label_->hide();
    name_button_->hide();
    type_button_->hide();
    argno_button_->hide();
    altno_button_->hide();
    ask_button_->hide();
    pattern_box_->hide();
    dimen_box_->hide();
}

void ParamWidget::ShowAttr()
{
    name_label_->show();
    type_label_->show();
    argno_label_->show();
    altno_label_->show();
    ask_label_->show();
    pattern_label_->show();
    dimen_label_->show();
    name_button_->show();
    type_button_->show();
    argno_button_->show();
    altno_button_->show();
    ask_button_->show();
    pattern_box_->show();
    dimen_box_->show();
}

void ParamWidget::SetScale(double scale_w, double scale_h)
{
    keyboard_->SetScale(scale_w, scale_h);
}

void ParamWidget::GetData(QString data)
{
    if(data.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入内容"));
        return;
    }
    if(param_map_.contains(data))
    {
        QMessageBox::warning(this, tr("提示"), tr("您输入的参数名无效"));
        return;
    }
    bool isint;
    data.toInt(&isint);
    if(isint)
    {
        QMessageBox::warning(this, tr("提示"), tr("您输入的参数名无效"));
        return;
    }
    if(add_type_ == 3)
    {
        QTreeWidgetItem *item = parm_tree_->currentItem();
        QString used_name = item->text(0);
        ParamInfo info = param_map_[used_name];
        param_map_.remove(used_name);
        param_map_.insert(data, info);
        ShowControl();
        item->setText(0, data);
        this->ParmClicked(item, 0);
        return;
    }
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, data);
    ParamInfo info;
    info.type = "num";
    info.pattern = "In";
    info.dimen = "0";
    if(add_type_ == 0 || add_type_ == 1)
    {
        parm_tree_->addTopLevelItem(item);
        if(add_type_ == 0)
        {
            info.ask = true;
            info.altno = 0;
            info.isadd = false;
        }
        else
        {
            info.ask = false;
            info.altno = 1;
            info.isadd = true;
        }
        info.argno = parm_tree_->indexOfTopLevelItem(item) + 1;
    }
    else
    {
        info.ask = false;
        info.isadd = true;
        QTreeWidgetItem *curr_item = parm_tree_->currentItem();
        QTreeWidgetItem *upper_item = curr_item->parent(); //获取当前所选中的参数的父节点
        if(upper_item == nullptr) //无父节点
        {
            curr_item->addChild(item);
            info.altno = 2;
            info.argno = param_map_[curr_item->text(0)].argno;
        }
        else
        {
            upper_item->addChild(item);
            info.altno = upper_item->childCount() + 1;
            info.argno = param_map_[upper_item->text(0)].argno;
        }
    }
    param_map_.insert(data, info);
    parm_tree_->setCurrentItem(item);
    ParmClicked(item, 0);
    if(name_button_->isHidden() && dimen_label_->isHidden()) //根据两个控件属于隐藏状态判断全部属于隐藏状态
    {
        ShowAttr();
    }
    ShowControl();
}

void ParamWidget::ParmClicked(QTreeWidgetItem *item, int)
{
    QString name = item->text(0);
    ParamInfo &info = param_map_[name];
    name_button_->setText(name);
    type_button_->setText(info.type);
    argno_button_->setText(QString::number(info.argno));
    altno_button_->setText(QString::number(info.altno));
    if(info.ask)
    {
        ask_button_->setText("True");
    }
    else
    {
        ask_button_->setText("False");
    }
    pattern_box_->setCurrentText(info.pattern);
    dimen_box_->setCurrentText(info.dimen);
}

void ParamWidget::UpperButtonClicked()
{
    QTreeWidgetItem *item = parm_tree_->currentItem();
    if(item == nullptr)
    {
        return;
    }
    QTreeWidgetItem *upper_item = parm_tree_->itemAbove(item); //获取当前节点的上一个节点
    if(upper_item == nullptr)
    {
        return;
    }
    parm_tree_->setCurrentItem(upper_item);
    ParmClicked(upper_item, 0);
}

void ParamWidget::LowerButtonClicked()
{
    QTreeWidgetItem *item = parm_tree_->currentItem();
    if(item == nullptr || parm_tree_->itemBelow(item) == nullptr) //未选中或已经是最后一个节点
    {
        return;
    }
    parm_tree_->setCurrentItem(parm_tree_->itemBelow(item)); //获取当前节点的下一个节点
    ParmClicked(parm_tree_->currentItem(), 0);
}

void ParamWidget::DeleteButtonClicked()
{
    QTreeWidgetItem *item = parm_tree_->currentItem();
    if(item == nullptr)
    {
        return;
    }
    if(item->parent() == nullptr) //无父节点
    {
        parm_tree_->takeTopLevelItem(parm_tree_->currentIndex().row());
    }
    else
    {
        item->parent()->takeChild(parm_tree_->currentIndex().row());
    }
    QTreeWidgetItem *curr_item = parm_tree_->currentItem();
    if(curr_item == nullptr)
    {
        HideAttr();
    }
    else
    {
        ParmClicked(curr_item, 0);
    }
    param_map_.remove(item->text(0));
    delete item;
}

void ParamWidget::HideControl()
{
    keyboard_->Init();
    parm_tree_->hide();
    attribute_widget_->hide();
    button_widget_->hide();
}

void ParamWidget::ShowControl()
{
    if(!keyboard_->isHidden())
    {
        keyboard_->hide();
    }
    parm_tree_->show();
    attribute_widget_->show();
    button_widget_->show();
}

void ParamWidget::AddParm()
{
    add_type_ = 0;
    HideControl();
    keyboard_->show();
}

void ParamWidget::AddOptParam()
{
    add_type_ = 1;
    HideControl();
    keyboard_->show();
}

void ParamWidget::AddShareParam()
{
    QTreeWidgetItem *item = parm_tree_->currentItem();
    if(item == nullptr)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先选择一个可选参数"));
        return;
    }
    if(!param_map_[item->text(0)].isadd)
    {
        QMessageBox::warning(this, tr("提示"), tr("您所选择的不是一个可选参数"));
        return;
    }
    add_type_ = 2;
    HideControl();
    keyboard_->show();
}

EditWindow::EditWindow(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    shear_button_.setText(tr("剪切"));
    copy_button_.setText(tr("复制"));
    stick_button_.setText(tr("黏贴"));
    chang_data_button_.setText(tr("更改选择内容"));
    key_button_.setText(tr("ABC..."));
    chang_button_.setText(tr("更改为..."));
    revoke_button_.setText(tr("撤销"));
    edit_button_.setText(tr("编辑"));
    upper_button_.setText(tr("至顶部"));
    lower_button_.setText(tr("至底部"));
    upper_stick_button_.setText(tr("在上面黏贴"));
    del_button_.setText(tr("删除"));
    image_button_.setText(tr("镜像..."));
    remarks_button_.setText(tr("备注"));
    redo_button_.setText(tr("重做"));
    select_button_.setText(tr("选择一项"));

    layout->addWidget(&shear_button_, 0, 0);
    layout->addWidget(&copy_button_, 1, 0);
    layout->addWidget(&stick_button_, 2, 0);
    layout->addWidget(&chang_data_button_, 3, 0);
    layout->addWidget(&key_button_, 4, 0);
    layout->addWidget(&chang_button_, 5, 0);
    layout->addWidget(&revoke_button_, 6, 0);
    layout->addWidget(&edit_button_, 7, 0);
    layout->addWidget(&upper_button_, 0, 1);
    layout->addWidget(&lower_button_, 1, 1);
    layout->addWidget(&upper_stick_button_, 2, 1);
    layout->addWidget(&del_button_, 3, 1);
    layout->addWidget(&image_button_, 4, 1);
    layout->addWidget(&remarks_button_, 5, 1);
    layout->addWidget(&redo_button_, 6, 1);
    layout->addWidget(&select_button_, 7, 1);

    stick_button_.setEnabled(false);
    chang_data_button_.setEnabled(false);
    upper_stick_button_.setEnabled(false);
    remarks_button_.setEnabled(false);
    del_button_.setEnabled(false);
    key_button_.setEnabled(false);
    chang_button_.setEnabled(false);
    revoke_button_.setEnabled(false);
    edit_button_.setEnabled(false);
    image_button_.setEnabled(false);
    redo_button_.setEnabled(false);
    select_button_.setEnabled(false);

    this->setLayout(layout);

    connect(TeachState::instance().data(), &TeachState::RunMod,
            [=](RunMode mod)
    {
        switch(mod)
        {
        case RunMode::AutoMod:
        {
            this->SetEnableAll(false);
            return;
        }
        case RunMode::ManualMod:
        {
            upper_button_.setEnabled(true);
            lower_button_.setEnabled(true);
            return;
        }
        case RunMode::MaxManualMod:
        {
            this->SetEnableAll(false);
        }
        }
    });
}

void EditWindow::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void EditWindow::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void EditWindow::SetEnableAll(bool enable)
{
    auto list = this->findChildren<QPushButton*>();

    for(auto *pt : list)
    {
        pt->setEnabled(enable);
    }
}

DebugWindow::DebugWindow(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    pp_to_main_button_.setText(tr("PP 移至 main"));
    pp_to_routine_button_.setText(tr("PP 移至例行程序..."));
    curso_to_mp_button_.setText(tr("光标移至 MP"));
    call_routine_button_.setText(tr("调用例行程序..."));
    see_value_button_.setText(tr("查看值"));
    see_system_button_.setText(tr("查看系统数据"));
    pp_to_curso_button_.setText(tr("PP 移至光标"));
    curso_topp_button_.setText(tr("光标移至PP"));
    move_pos_button_.setText(tr("移至位置"));
    cancel_call_button_.setText(tr("取消调用例行程序"));
    inspect_button_.setText(tr("检查程序"));
    search_routine_button_.setText(tr("搜索例行程序"));

    layout->addWidget(&pp_to_main_button_, 0, 0);
    layout->addWidget(&pp_to_routine_button_, 1, 0);
    layout->addWidget(&curso_to_mp_button_, 2, 0);
    layout->addWidget(&call_routine_button_, 3, 0);
    layout->addWidget(&see_value_button_, 4, 0);
    layout->addWidget(&see_system_button_, 5, 0);
    layout->addWidget(&pp_to_curso_button_, 0, 1);
    layout->addWidget(&curso_topp_button_, 1, 1);
    layout->addWidget(&move_pos_button_, 2, 1);
    layout->addWidget(&cancel_call_button_, 3, 1);
    layout->addWidget(&inspect_button_, 4, 1);
    layout->addWidget(&search_routine_button_, 5, 1);

    curso_to_mp_button_.setEnabled(false);
    call_routine_button_.setEnabled(false);
    move_pos_button_.setEnabled(false);
    cancel_call_button_.setEnabled(false);

    this->setLayout(layout);

    connect(TeachState::instance().data(), &TeachState::RunMod,
            [=](RunMode mod)
    {
        switch(mod)
        {
        case RunMode::AutoMod:
        {
            this->SetEnableAll(false);
            return;
        }
        default:
        {
            pp_to_main_button_.setEnabled(true);
            pp_to_routine_button_.setEnabled(true);
            see_value_button_.setEnabled(true);
            see_system_button_.setEnabled(true);
            pp_to_curso_button_.setEnabled(true);
            curso_topp_button_.setEnabled(true);
//            inspect_button_.setEnabled(true);
            search_routine_button_.setEnabled(true);
        }
        }
    });
}

void DebugWindow::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void DebugWindow::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void DebugWindow::SetEnableAll(bool enable)
{
    auto list = this->findChildren<QPushButton*>();

    for(auto *pt : list)
    {
        pt->setEnabled(enable);
    }
}

ChangeDataType::ChangeDataType(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    table_box_ = new TableBox(this);
    button_widget_ = new QWidget(this);

    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    view_button_ = new ComboBoxObject(tr("视图"), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    view_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addWidget(view_button_);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    button_widget_->setLayout(button_layout);

    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(table_box_, 6);
    layout->setStretchFactor(button_widget_, 1);
    this->setLayout(layout);

    Init();

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    view_button_->hide();

    connect(table_box_, &TableBox::itemClicked, this, &ChangeDataType::ItemClicked);
    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        emit ChangeType(type_);
    });
}

void ChangeDataType::Init(QString type)
{
    type_ = type;

    define_button_->setEnabled(false);

    table_box_->clearSelection();
}

void ChangeDataType::Refresh()
{
    table_box_->clear();

    int r = 0, c = 0;
    for(auto &name : name_list_)
    {
        table_box_->AddText(r, c++, name);

        if(c > 2)
        {
            r++;
            c = 0;
        }
    }
}

void ChangeDataType::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
}

void ChangeDataType::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    type_ = item->text();
    if(!define_button_->isEnabled())
        define_button_->setEnabled(true);
}

void ChangeDataType::Init()
{
    name_list_.clear();

    name_list_ = DataType::instance()->GetTypeName();

    Refresh();
}

AddFunc::AddFunc(QWidget *parent) : QFrame(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    auto *title_layout = new QVBoxLayout;
    title_label_ = new QLabel(tr("添加指令 - 子程序调用"), this);
    auto *hline_label = new QLabel(this);
    hline_label->setFrameShape(QFrame::HLine);
    title_layout->addWidget(title_label_);
    title_layout->addWidget(hline_label);
    title_layout->setContentsMargins(0, 0, 0, 0);
    title_layout->setSpacing(0);

    QHBoxLayout *label_layout = new QHBoxLayout;
    tips_label_ = new QLabel(tr("选择有带待添加至程序的子程序"), this);
    tips2_label_ = new QLabel(tr("活动过滤器"), this);
    filter_label_ = new QLabel(this);
    label_layout->addWidget(tips_label_);
    label_layout->addWidget(tips2_label_);
    label_layout->addWidget(filter_label_);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    filter_button_ = new ComboBoxObject(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    filter_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    filter_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addWidget(filter_button_);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addLayout(title_layout);
    layout->addLayout(label_layout);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_layout, 1);
    layout->setStretchFactor(label_layout, 1);
    layout->setStretchFactor(table_box_, 10);
    layout->setStretchFactor(button_widget_, 2);
    this->setLayout(layout);

    filter_button_->SetFilterButton();
    filter_button_->hide();

    insert_window_ = new InsertExpresWindow(this);
    insert_window_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        if(func_code_.isEmpty())
        {
            QMessageBox::warning(this, tr("提示"), tr("请选择函数"));
            return;
        }

        emit AddFuncClicked(func_code_);
    });
    connect(cancel_button_, &QPushButton::clicked,
            [=]()
    {
        emit CancelClicked();
    });
    connect(table_box_, &TableBox::itemClicked, this, &AddFunc::ButtonClicked);
}

void AddFunc::Init()
{
    func_code_.clear();
    auto rout_list = EditData::instance()->GetRoutName(RoutineData::PROC);

    this->AddFuncList(rout_list);
}

void AddFunc::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
    insert_window_->SetScale(scale_w, scale_h);
}

void AddFunc::AddFuncList(QStringList &func_list)
{
    table_box_->clear();

    if(func_list.isEmpty())
        return;

    int r = 0, c = 0;
    for(auto &name : func_list)
    {
        table_box_->AddText(r, c++, name);
        if(c > 1)
        {
            r++;
            c = 0;
        }
    }

    table_box_->clearSelection();
}

void AddFunc::ButtonClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    func_code_ = item->text() + ";";
}

TaskFileManger::TaskFileManger(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *label_layout = new QHBoxLayout;
    tips_label_ = new QLabel(tr("另存为 - "), this);
    curr_path_label_ = new QLabel(this);
    QLabel *la = new QLabel(this);
    la->setFrameShape(QFrame::HLine);
    label_layout->addWidget(tips_label_);
    label_layout->addWidget(curr_path_label_);
    label_layout->setContentsMargins(0, 0, 0, 0);
    label_layout->setSpacing(0);

    QHBoxLayout *sort_layout = new QHBoxLayout;
    show_sort_button_ = new ImageButton(":/image/Image/MainWidget/show_button.png", this);
    hide_sort_button_ = new ImageButton(":/image/Image/MainWidget/hide_button.png", this);
    show_sort_button_->GetTextLabel()->hide();
    hide_sort_button_->GetTextLabel()->hide();
    QLabel *test_label = new QLabel(this);
    sort_layout->addWidget(hide_sort_button_);
    sort_layout->addWidget(show_sort_button_);
    sort_layout->addWidget(test_label);
    sort_layout->setStretchFactor(hide_sort_button_, 2);
    sort_layout->setStretchFactor(show_sort_button_, 2);
    sort_layout->setStretchFactor(test_label, 15);
    sort_layout->setContentsMargins(0, 0, 0, 0);
    sort_layout->setSpacing(0);

    file_box_ = new TableBox(this);

    QHBoxLayout *edit_layout = new QHBoxLayout;
    name_label_ = new QLabel(tr("文件夹名:"), this);
    dir_edit_ = new QLineEdit(this);
    key_button_ = new QPushButton("...", this);
    edit_layout->addWidget(name_label_);
    edit_layout->addWidget(dir_edit_);
    edit_layout->addWidget(key_button_);

    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    creator_button_ = new QPushButton(button_widget_);
    upper_button_ = new QPushButton(button_widget_);
    home_button_ = new QPushButton(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton (tr("取消"), button_widget_);
    creator_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    upper_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    home_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    creator_button_->setFlat(true);
    upper_button_->setFlat(true);
    home_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addWidget(creator_button_);
    button_layout->addWidget(upper_button_);
    button_layout->addWidget(home_button_);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addLayout(label_layout);
    layout->addWidget(la);
    layout->addLayout(sort_layout);
    layout->addWidget(file_box_);
    layout->addLayout(edit_layout);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStretchFactor(label_layout, 1);
    layout->setStretchFactor(sort_layout, 1);
    layout->setStretchFactor(file_box_, 6);
    layout->setStretchFactor(edit_layout, 2);
    layout->setStretchFactor(button_widget_, 1);

    this->setLayout(layout);

    key_board_ = new Keyboard(this);

    key_board_->hide();

    creator_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/create_button.png)");
    upper_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/upper_button.png)");
    home_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/home_button.png)");

    show_sort_button_->SetIsSelect(true);

    root_path_ = qApp->applicationDirPath() + "/data/RAPID";
    home_path_ = root_path_ + "/Modular";
    curr_path_ = home_path_;

    curr_path_label_->setText(home_path_);

    pos_neg_ = true;

    file_box_->horizontalHeader()->show();
    file_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    file_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    file_box_->SetRomn(0, 2);
    file_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("类型"));

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void TaskFileManger::Init(bool issave, QString path)
{
    issave_ = issave;
    if(issave)
        tips_label_->setText(tr("另存为 - "));
    else
        tips_label_->setText(tr("加载 - "));

    if(!issave_)
        key_button_->hide();
    else
        key_button_->show();
    dir_edit_->clear();

    if(path.isEmpty())
        curr_path_ = home_path_;
    else
        curr_path_ = path;

    crsa_ = true; //默认为新建任务时的保存

    FindDirs(curr_path_);
}

void TaskFileManger::Init(bool issave, bool crsa)
{
    this->Init(issave);

    crsa_ = crsa;
}

void TaskFileManger::SetScale(double scale_w, double scale_h)
{
    file_box_->SetScale(scale_w, scale_h);

    if(!key_board_->isHidden())
    {
        key_board_->resize(this->size());
        key_board_->move(0, 0);
    }
    key_board_->SetScale(scale_w, scale_h);
}

void TaskFileManger::Sort()
{
    if(pos_neg_)
    {
        pos_neg_ = false;

        std::stable_sort(data_list_.begin(), data_list_.end(), ReverseOrder1);
    }
    else
    {
        pos_neg_ = true;

        std::stable_sort(data_list_.begin(), data_list_.end(), PositiveSequence1);
    }

    this->AddWidget(data_list_);
}

void TaskFileManger::ShowKeyboard(bool iscrea)
{
    iscrea_ = iscrea;
    if(iscrea_) //新建文件夹
    {
        key_board_->Init();
    }
    else
    {
        key_board_->Init(dir_edit_->text());
    }

    key_board_->resize(this->size());
    key_board_->move(0, 0);
    key_board_->show();
}

void TaskFileManger::DefineClicked()
{
    if(dir_edit_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("文件夹名为空"));
        return;
    }
    QString task_name = dir_edit_->text();
    if(task_name.at(task_name.size() - 1) == '/')
        task_name = task_name.mid(0, task_name.size() - 1);
    if(task_name == EditData::instance()->GetTaskName())
    {
        QMessageBox::warning(this, tr("提示"), tr("请勿选择同一个任务"));
        return;
    }
    if(task_name == "TempMod")
    {
        QMessageBox::warning(this, tr("提示"), tr("请勿选择临时任务"));
        return;
    }

    if(issave_) //保存
    {
        QString new_path = curr_path_ + "/" + task_name + "/";

        QDir dir;
        for(int i = 0; i < 2; i++)
        {
            QString new_dir_path = new_path;
            if(i == 0)
            {
                new_dir_path.append("User/");

                if(!dir.exists(new_dir_path))
                    dir.mkpath(new_dir_path);
            }
            else
            {
                new_dir_path.append("System/");

                if(!dir.exists(new_dir_path))
                    dir.mkpath(new_dir_path);
            }

            auto mods = EditData::instance()->GetModName();
            for(auto mod : mods)
            {
                auto type = EditData::instance()->GetModType(mod);
                if(!type && i == 0)
                    EditData::instance()->ExportMod(mod, new_dir_path);
                else if(type && i == 1)
                    EditData::instance()->ExportMod(mod, new_dir_path);
            }
        }

        if(crsa_) //新建
        {
            EditData::instance()->DelTask();

            EditData::instance()->SetTaskOrderName("NewProgramName");

            EditData::instance()->AnalysisData();

            QString data = "MODULE MainModule\n\tPROC main()\n\t\t<SMT>\n\tENDPROC\nENDMODULE";

            EditData::instance()->LoadMod(data);
        }
        else //另存为
        {
            auto path = curr_path_.mid(home_path_.size() + 1);
            if(!path.isEmpty())
            {
                if(path.at(path.size() - 1) != '/')
                    path.append("/");
            }
            path.append(task_name);

            EditData::instance()->DelMod();

            EditData::instance()->SetTaskOrderName(task_name, path);

            EditData::instance()->AnalysisData();
        }

        emit SaveTask();
    }
    else //加载
    {
        auto path = curr_path_.mid(home_path_.size() + 1);
        if(!path.isEmpty() && path.at(path.size() - 1) != '/')
            path.append("/");
        path.append(task_name);

        EditData::instance()->DelMod();
        EditData::instance()->SetTaskOrderName(task_name, path);
        EditData::instance()->AnalysisData();

        emit LoadTask();
    }
}

void TaskFileManger::FindDirs(QString path)
{
    QString curr = "/" + path.mid(root_path_.size() + 1);
    curr_path_label_->setText(curr);

    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    data_list_ = dir.entryList();

    this->AddWidget(data_list_);
}

void TaskFileManger::ConnectInit()
{
    connect(show_sort_button_, &ImageButton::clicked,
            [=]()
    {
        hide_sort_button_->SetIsSelect(false);

        file_box_->horizontalHeader()->show();
    });

    connect(hide_sort_button_, &ImageButton::clicked,
            [=]()
    {
        show_sort_button_->SetIsSelect(false);

        file_box_->horizontalHeader()->hide();
    });

    connect(upper_button_, &QPushButton::clicked,
            [=]()
    {
        QDir d(curr_path_);
        d.cdUp();

        if(d.path() == root_path_)
            return;
        curr_path_ = d.path();

        this->FindDirs(curr_path_);
    });

    connect(home_button_, &QPushButton::clicked,
            [=]()
    {
        if(curr_path_ == home_path_)
            return;
        curr_path_ = home_path_;

        this->FindDirs(curr_path_);
    });

    connect(file_box_, &TableBox::itemClicked,
            [=](QTableWidgetItem *item)
    {
        if(item == nullptr)
            return;
        int row = file_box_->row(item);

        if(row < 0 || row >= data_list_.size())
            return;

        QDir d(curr_path_);
        d.cd(data_list_.at(row));

        if(!issave_) //加载
        {
            d.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            auto file_list = d.entryList();
            if(file_list.isEmpty())
            {
                dir_edit_->setText(data_list_.at(row));
                return;
            }
            else if(file_list.size() == 2)
            {
                if(file_list.contains("User") && file_list.contains("System"))
                {
                    dir_edit_->setText(data_list_.at(row));
                    return;
                }
            }
        }

        curr_path_ = d.path();

        this->FindDirs(curr_path_);
    });

    connect(creator_button_, &QPushButton::clicked,
            [=]()
    {
        this->ShowKeyboard(true);
    });

    connect(key_button_, &QPushButton::clicked,
            [=]()
    {
        this->ShowKeyboard(false);
    });

    connect(key_board_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        key_board_->hide();
    });

    connect(key_board_, &Keyboard::GetData,
            [=](QString kst)
    {
        if(kst.isEmpty())
            return;

        if(data_list_.contains(kst))
        {
            QMessageBox::warning(this, tr("提示"), tr("该文件夹已存在"));
            return;
        }

        if(iscrea_) //创建文件夹
        {
            QDir d(curr_path_);
            if(d.mkdir(kst))
            {
                this->FindDirs(curr_path_);
            }
        }
        else
        {
            dir_edit_->setText(kst);
        }

        key_board_->hide();
    });

    connect(cancel_button_, &QPushButton::clicked,
            [=]()
    {
        emit CancelButton();
    });

    connect(file_box_->horizontalHeader(), &QHeaderView::sectionClicked, this, &TaskFileManger::Sort);

    connect(define_button_, &QPushButton::clicked, this, &TaskFileManger::DefineClicked);
}

void TaskFileManger::AddWidget(QStringList &data_list)
{
    if(file_box_->GetRow() != 0)
    {
        file_box_->clearContents();
        file_box_->SetRomn(0, 2);
    }

    for(int i = 0; i < data_list.size(); i++)
    {
        auto &data = data_list[i];

        file_box_->AddText(i, 0, data);
        file_box_->AddText(i, 1, tr("文件夹"));
    }
}

CodeRow::CodeRow(QWidget *parent) : QTextBrowser (parent)
{
    tbscrollbar_ = new TBScrollBar(this);

    this->setVerticalScrollBar(tbscrollbar_);
    this->setAlignment(Qt::AlignCenter);

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setContextMenuPolicy(Qt::NoContextMenu);
}

void CodeRow::clear()
{
    QTextBrowser::clear();

    this->setAlignment(Qt::AlignCenter);
}

void CodeRow::AddData(QString data)
{
    this->append(qMove(data));
}

void CodeRow::MvStart()
{
    this->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}

void CodeRow::MvEnd()
{
    moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

void CodeRow::MvPos(int row)
{
    auto sor = this->textCursor();

    int start = this->document()->findBlockByNumber(row).position();

    sor.setPosition(start, QTextCursor::MoveAnchor);

    this->setTextCursor(sor);
}

void CodeRow::SetFont(QFont &font)
{
    this->setFont(font);
}

void CodeRow::Upper()
{
    tbscrollbar_->setValue(tbscrollbar_->value() - 10);
}

void CodeRow::Lower()
{
    tbscrollbar_->setValue(tbscrollbar_->value() + 10);
}

void CodeRow::Uppers()
{
    tbscrollbar_->setValue(tbscrollbar_->value() - 100);
}

void CodeRow::Lowers()
{
    tbscrollbar_->setValue(tbscrollbar_->value() + 100);
}

void CodeRow::mousePressEvent(QMouseEvent *)
{
}

void CodeRow::mouseDoubleClickEvent(QMouseEvent *)
{
}

void CodeRow::mouseMoveEvent(QMouseEvent *)
{
}

void CodeRow::mouseReleaseEvent(QMouseEvent *)
{
}

SearchRoutine::SearchRoutine(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    table_widget_ = new QFrame(this);
    auto *table_layout = new QHBoxLayout(table_widget_);
    table_box_ = new TableBox(table_widget_);
    table_layout->addWidget(table_box_);
    table_layout->setContentsMargins(0, 0, 0, 0);
    table_layout->setSpacing(0);

    button_widget_ = new QWidget(this);
    auto *button_layout = new QHBoxLayout(button_widget_);
    close_button_ = new QPushButton(tr("后退"), button_widget_);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setFlat(true);
    button_layout->addStretch(2);
    button_layout->addWidget(close_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(table_widget_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(table_widget_, 10);
    layout->setStretchFactor(button_widget_, 2);

    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->horizontalHeader()->show();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(close_button_, &QPushButton::clicked, this, &SearchRoutine::Close);
}

void SearchRoutine::Init(QString filter)
{
    table_box_->clear();
    table_box_->SetRomn(1, 3);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("模块") << tr("类型"));

    auto rout_list = EditData::instance()->GetRoutName();
    int i = 0;
    for(auto &rout : rout_list)
    {
        if(!filter.isEmpty())
        {
            if(rout.toUpper().indexOf(filter.toUpper()) == -1)
            {
                continue;
            }
        }

        table_box_->AddText(i, 0, rout);
        table_box_->AddText(i, 1, EditData::instance()->GetRoutMod(rout));
        table_box_->AddText(i, 2, EditData::instance()->GetRoutType(rout));
        i++;
    }
}

void SearchRoutine::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
}

ShowSystemData::ShowSystemData(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips1_label_ = new QLabel(tr("选择的任务"), head_widget_);
    select_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel("Select a task from the list", head_widget_);
    head_layout->addWidget(tips1_label_, 0, 0);
    head_layout->addWidget(select_label_, 0, 1);
    head_layout->addWidget(tips2_label_, 1, 0);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    auto *bt_layout = new QHBoxLayout(button_widget_);
    refresh_button_ = new QPushButton(tr("刷新"), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    refresh_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    refresh_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addWidget(refresh_button_);
    bt_layout->addWidget(define_button_);
    bt_layout->addWidget(cancel_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(head_widget_, 4);
    layout->setStretchFactor(table_box_, 9);
    layout->setStretchFactor(button_widget_, 2);

    refresh_button_->hide();

    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->horizontalHeader()->show();

    mod_var_window_ = new ModVarWindow(this);
    mod_var_window_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    connect(define_button_, &QPushButton::clicked, this, &ShowSystemData::Define);
    connect(refresh_button_, &QPushButton::clicked, this, &ShowSystemData::Refresh);
    connect(cancel_button_, &QPushButton::clicked, this, &ShowSystemData::Cancel);
    connect(table_box_, &TableBox::itemClicked, this, &ShowSystemData::ItemTableClicked);
    connect(mod_var_window_, &ModVarWindow::Cancel, this, &ShowSystemData::ModVarCancel);
}

void ShowSystemData::Init()
{
    table_box_->clear();
    table_box_->SetRomn(1, 2);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("类型"));

    table_box_->AddText(0, 0, "T_ROB1");
    table_box_->AddText(0, 1, "Normal");

    refresh_button_->hide();
    define_button_->show();
    tips2_label_->show();
    select_label_->clear();
    define_button_->setEnabled(false);
}

void ShowSystemData::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);

    if(!mod_var_window_->isHidden())
    {
        mod_var_window_->resize(this->size());
        mod_var_window_->move(0, 0);
    }
    mod_var_window_->SetScale(scale_w, scale_h);
}

void ShowSystemData::Refresh()
{
    table_box_->clear();
    table_box_->SetRomn(3, 2);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("值"));

    table_box_->AddText(0, 0, "Current Pay Load");
    table_box_->AddText(0, 1, TeachState::instance()->GetCurrentLoad());

    table_box_->AddText(1, 0, "Current Tool");
    table_box_->AddText(1, 1, TeachState::instance()->GetCurrentTool());

    table_box_->AddText(2, 0, "Current Work Object");
    table_box_->AddText(2, 1, TeachState::instance()->GetCurrentWork());
}

void ShowSystemData::Define()
{
    auto *item = table_box_->item(table_box_->currentRow(), 1);
    if(item == nullptr)
        return;

    tips2_label_->hide();
    select_label_->setText(item->text());
    define_button_->hide();
    refresh_button_->show();

    Refresh();
}

void ShowSystemData::ItemTableClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
    {
        return;
    }

    if(!define_button_->isHidden())
    {
        define_button_->setEnabled(true);
        return;
    }

    auto *name_item = table_box_->item(table_box_->row(item), 1);
    auto name = name_item->text();

    auto vars = VarInfo::instance()->GetVarInfo(name);
    auto &var = vars[0];

    var_point_ = Convert::DataTypeToVarFormData(var.data_type, var.var.toString());

    mod_var_window_->Init(var_point_, name, var.mod, var.action_scope, true);
    mod_var_window_->resize(this->size());
    mod_var_window_->move(0, 0);
    mod_var_window_->show();
}

void ShowSystemData::ModVarCancel()
{
    mod_var_window_->hide();
}

InsertExpresWindow::InsertExpresWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    title_widget_ = new QFrame(this);
    auto *title_layout = new QHBoxLayout(title_widget_);
    ico_label_ = new QLabel(title_widget_);
    title_label_ = new QLabel(tr("插入表达式"), title_widget_);
    title_layout->addWidget(ico_label_);
    title_layout->addWidget(title_label_);
    title_layout->setStretchFactor(ico_label_, 1);
    title_layout->setStretchFactor(title_label_, 20);
    title_layout->setContentsMargins(0, 0, 0, 0);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips_activity_label_ = new QLabel(tr("活动"), head_widget_);
    activity_label_ = new QLabel(head_widget_);
    tips_result_label_ = new QLabel(tr("结果"), head_widget_);
    result_label_ = new QLabel(this);
    tips_filter_label_ = new QLabel(tr("活动过滤器"), this);
    filter_label_ = new QLabel(head_widget_);
    tips_tips_label_ = new QLabel(tr("提示"), head_widget_);
    tips_label_ = new QLabel(head_widget_);
    head_layout->addWidget(tips_activity_label_, 0, 0);
    head_layout->addWidget(activity_label_, 0, 1);
    head_layout->addWidget(tips_result_label_, 0, 2);
    head_layout->addWidget(result_label_, 0, 3);
    head_layout->addWidget(tips_filter_label_, 1, 0);
    head_layout->addWidget(filter_label_, 1, 1);
    head_layout->addWidget(tips_tips_label_, 1, 2);
    head_layout->addWidget(tips_label_, 1, 3);

    auto *widget_layout = new QHBoxLayout;

    auto *data_layout = new QVBoxLayout;
    data_edit_ = new InsertCodeText(this);

    tab_widget_ = new QTabWidget(this);
    data_box_ = new TableBox(this);
    func_box_ = new TableBox(this);
    tab_widget_->addTab(data_box_, tr("数据"));
    tab_widget_->addTab(func_box_, tr("功能"));

    button_widget_ = new QWidget(this);
    auto *button_layout = new QVBoxLayout(button_widget_);
    left_button_ = new QPushButton("←", button_widget_);
    right_button_ = new QPushButton("→", button_widget_);
    add_button_ = new QPushButton("＋", button_widget_);
    red_button_ = new QPushButton("－", button_widget_);
    brack_button_ = new QPushButton("()", button_widget_);
    rebra_button_ = new QPushButton("(●)", button_widget_);
    left_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    add_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    red_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    brack_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rebra_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_layout->addWidget(left_button_);
    button_layout->addWidget(right_button_);
    button_layout->addWidget(add_button_);
    button_layout->addWidget(red_button_);
    button_layout->addWidget(brack_button_);
    button_layout->addWidget(rebra_button_);
    button_layout->setSpacing(0);
    button_layout->setContentsMargins(0, 0, 0, 0);

    data_layout->addWidget(data_edit_);
    data_layout->addWidget(tab_widget_);
    data_layout->setStretchFactor(data_edit_, 1);
    data_layout->setStretchFactor(tab_widget_, 5);
    data_layout->setContentsMargins(0, 0, 0, 0);
    data_layout->setSpacing(0);

    widget_layout->addLayout(data_layout);
    widget_layout->addWidget(button_widget_);
    widget_layout->setContentsMargins(0, 0, 0, 0);
    widget_layout->setSpacing(0);
    widget_layout->setStretchFactor(data_layout, 10);
    widget_layout->setStretchFactor(button_widget_, 1);

    lower_widget_ = new QWidget(this);
    auto *lower_layout = new QHBoxLayout(lower_widget_);
    filter_button_ = new ComboBoxObject(lower_widget_);
    edit_button_ = new ComboBoxObject(tr("编辑"), lower_widget_);
    change_type_button_ = new QPushButton(tr("更改数据类型"), lower_widget_);
    define_button_ = new QPushButton(tr("确定"), lower_widget_);
    cancel_button_ = new QPushButton(tr("取消"), lower_widget_);
    filter_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    change_type_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    filter_button_->setFlat(true);
    edit_button_->setFlat(true);
    change_type_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    lower_layout->addWidget(filter_button_);
    lower_layout->addWidget(edit_button_);
    lower_layout->addWidget(change_type_button_);
    lower_layout->addWidget(define_button_);
    lower_layout->addWidget(cancel_button_);
    lower_layout->setContentsMargins(0, 0, 0, 0);
    lower_layout->setSpacing(0);
    lower_widget_->setLayout(lower_layout);

    layout->addWidget(title_widget_);
    layout->addWidget(head_widget_);
    layout->addLayout(widget_layout);
    layout->addWidget(lower_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(title_widget_, 1);
    layout->setStretchFactor(head_widget_, 3);
    layout->setStretchFactor(widget_layout, 12);
    layout->setStretchFactor(lower_widget_, 2);

    option_an_ = new QAction("Optional Arguments", this);
    add_an_ = new QAction(tr("添加记录组件"), this);
    del_an_ = new QAction(tr("删除记录组件"), this);
    annay_an_ = new QAction(tr("数组索引"), this);
    all_an_ = new QAction(tr("全部"), this);
    select_an_ = new QAction(tr("仅限选定内容"), this);
    edit_button_->AppAction(option_an_);
    edit_button_->AppAction(add_an_);
    edit_button_->AppAction(del_an_);
    edit_button_->AppAction(annay_an_);
    edit_button_->AppAction(all_an_);
    edit_button_->AppAction(select_an_);

    auto qss = "QMenu::item{padding-left:0px;}";
    edit_button_->GetMenu()->setStyleSheet(qss);

    option_an_->setEnabled(false);
    add_an_->setEnabled(false);
    del_an_->setEnabled(false);
    annay_an_->setEnabled(false);

    keyboard_ = new Keyboard(this);
    keyboard_->hide();
    filter_key_ = new FilterKey(this);
    filter_key_->resize(394, 275);
    key_x_ = 245;
    key_y_ = 65;
    filter_key_->move(key_x_, key_y_);
    filter_key_->hide();
    filter_key_->setFrameShape(QFrame::Box);

    tab_widget_->resize(558, tab_widget_->height());

    data_edit_->SetParentPointer(this);
    ico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/program_Editor.png)");
    filter_button_->SetFilterButton();
    head_widget_->setFrameShape(QFrame::Box);

    data_edit_->installEventFilter(this);
    iscreate_ = true;

    add_list_ << "+" << "-" << "*" << "/" << "<" << "<=" << "<>" << "=" << ">" << ">=" << "AND" << "DIV" << "MOD" << "OR" << "XOR";
    for(int i = 0; i < 16; i++)
    {
        QString name1 = "DI" + QString::number(i + 1);
        QString name2 = "DO" + QString::number((i + 1));

        di_list_.append(name1);
        do_list_.append(name2);
    }

    for(int i = 0; i < 4; i++)
    {
        QString name1 = "AI" + QString::number(i + 1);
        QString name2 = "AO" + QString::number((i + 1));

        ai_list_.append(name1);
        ao_list_.append(name2);
    }

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    keyboard_->setAutoFillBackground(true);
    keyboard_->setPalette(p);
    filter_key_->setAutoFillBackground(true);
    filter_key_->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    lower_widget_->setAutoFillBackground(true);
    lower_widget_->setPalette(p);

    ConnectInit();
}

void InsertExpresWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int width = tab_widget_->width();
    int tab_count = tab_widget_->count();
    int tab_width = width/tab_count;
    tab_widget_->setStyleSheet(QString("QTabBar::tab{width:%1px; min-height: 40px;}").arg(tab_width));
}

void InsertExpresWindow::Init(QString data, QString mod_name, QString type, bool lerg, bool rep)
{
    rep_ = rep;
    lerg_ = lerg;
    mod_name_ = mod_name;
    curr_ = 0;
    curr_point_.clear();
    select_pos_.clear();
    data_dim_.clear();

    if(type == "none")
    {
        type = "num";
        tips_label_->setText("any type");
    }
    else
    {
        tips_label_->setText(type);
    }

    activity_label_->setText(type);
    result_label_->setText(type);

    filter_label_->clear();
    if(!filter_key_->isHidden())
        filter_key_->show();

    data_edit_->setPlainText(data);

    if(add_list_.contains(data))
    {
        if(type == "robtarget" && data == "*")
        {
            UpdateTabBox(type);
            Analysis();
            return;
        }
        lerg_ = false;

        UpdateTabBox(type);

        activity_label_->clear();
        result_label_->clear();

        data_box_->clear();
        func_box_->clear();

        int r = 0, c = 0;
        for(auto &str : add_list_)
        {
            data_box_->AddText(r, c++, str);
            if(c > 1)
            {
                r++;
                c = 0;
            }
        }

        Analysis();
    }
    else
    {

        UpdateTabBox(type);
        Analysis();
    }
}

void InsertExpresWindow::SetScale(double scale_w, double scale_h)
{
    data_box_->SetScale(scale_w, scale_h);
    func_box_->SetScale(scale_w, scale_h);

    if(!keyboard_->isHidden())
    {
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
    }
    keyboard_->SetScale(scale_w, scale_h);

    if(!filter_key_->isHidden())
    {
        filter_key_->resize(394*scale_w, 275*scale_h);
        filter_key_->move(key_x_*scale_w, key_y_*scale_h);
    }
    filter_key_->SetScale(scale_w, scale_h);

}

void InsertExpresWindow::ChangeDataType(QString type)
{
    Init(type, lerg_);
}

void InsertExpresWindow::CreateVared()
{
    Init(activity_label_->text(), lerg_);
}

void InsertExpresWindow::DefineClicked()
{
    auto code = data_edit_->toPlainText();
    if(code.indexOf("<VAR>") != -1 || code.indexOf("<EXP>") != -1)
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择变量或功能"));
        return;
    }

    emit ReturnCode(code, rep_);
}

void InsertExpresWindow::DataBoxItem(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    int row = data_box_->row(item);
    int column = data_box_->column(item);
    auto text = item->text();

    if(add_list_.count(text) != 0)
    {
        auto pt = curr_point_.lock();
        if(pt.isNull())
            return;
        auto data = data_edit_->toPlainText();
        data.replace(pt->seat, pt->code.size(), text);

        data_edit_->setPlainText(data);

        int old_curr = curr_;
        Analysis();
        auto pos = select_pos_.at(old_curr).lock();
        if(!pos.isNull())
        {
            curr_ = old_curr;
            curr_point_ = pos;
            SelectText(pos->seat, pos->code.size());
        }

        return;
    }

    if(row == 0 && column == 0 && iscreate_) //新建变量
    {
        emit ShowCreateNewVar(activity_label_->text());
        return;
    }

    auto curr_pt = curr_point_.lock();
    if(curr_pt.isNull())
        return;

    auto &dim = data_dim_[item];

    if(!dim.isEmpty())
    {
        int dim_num = dim.count(',');

        text.append('{');
        for(int i = 0; i < dim_num; i++)
        {
            text.append("<EXP>");
            if(i + 1 < dim_num)
                text.append(",");
        }
        text.append("}");
    }

    auto data = data_edit_->toPlainText();
    data.replace(curr_pt->seat, curr_pt->code.size(), text);

    data_edit_->setPlainText(data);
    Analysis();
}

void InsertExpresWindow::FuncBoxItem(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    int row = func_box_->row(item);
    int column = func_box_->column(item);
    auto text = item->text();

    qDebug() << "func:" << row << column << text;
}

void InsertExpresWindow::ChangeType()
{
    emit ShowChangeDataType();
}

void InsertExpresWindow::Left()
{
    if(curr_ - 1 < 0 || curr_ > select_pos_.size())
        return;

    auto pos = select_pos_.at(curr_).lock();
    if(!pos.isNull())
    {
        if(add_list_.count(pos->code) != 0)
        {
            auto type = activity_label_->text();
            UpdateTabBox(type, filter_key_->GetFilter());
        }
    }

    curr_--;
    curr_point_ = select_pos_.at(curr_);
    auto ses = curr_point_.lock();
    if(ses.isNull())
    {
        curr_++;
        curr_point_.clear();
        return;
    }

    SelectText(ses->seat, ses->code.size());

    if(separate_ != 0.0 && lerg_ && ses->seat < separate_)
    {
        Init(activity_label_->text(), false);

        return;
    }

    if(add_list_.count(ses->code) != 0)
    {
        data_box_->clear();
        data_box_->SetRomn(1, 2);

        int r = 0, c = 0;
        for(auto &str : add_list_)
        {
            data_box_->AddText(r, c++, str);
            if(c > 1)
            {
                r++;
                c = 0;
            }
        }
    }
}

void InsertExpresWindow::Right()
{
    if(curr_ + 1 >= select_pos_.size())
        return;

    auto pos = select_pos_.at(curr_).lock();
    if(!pos.isNull())
    {
        if(add_list_.count(pos->code) != 0)
        {
            auto type = activity_label_->text();
            UpdateTabBox(type, filter_key_->GetFilter());
        }
    }

    curr_++;

    curr_point_ = select_pos_.value(curr_);
    auto ses = curr_point_.lock();
    if(ses.isNull())
    {
        curr_--;
        curr_point_.clear();
        return;
    }

    SelectText(ses->seat, ses->code.size());

    if(separate_ != 0.0 && !lerg_ && ses->seat > separate_)
    {
        Init(activity_label_->text(), true);

        return;
    }

    if(add_list_.count(ses->code) != 0)
    {
        data_box_->clear();
        data_box_->SetRomn(1, 2);

        int r = 0, c = 0;
        for(auto &str : add_list_)
        {
            data_box_->AddText(r, c++, str);
            if(c > 1)
            {
                r++;
                c = 0;
            }
        }
    }
}

void InsertExpresWindow::ShidFilterWindow()
{
    if(filter_key_->isHidden())
    {
        filter_key_->Init();
        filter_key_->show();

        filter_button_->LowerArrow();
    }
    else
    {
        filter_key_->hide();

        filter_button_->UpperArrow();
    }
}

void InsertExpresWindow::ResetFilter()
{
    filter_label_->clear();

    auto type = activity_label_->text();
    UpdateTabBox(type);
}

void InsertExpresWindow::FilterText(QString text)
{
    filter_label_->setText(text);

    auto type = activity_label_->text();
    UpdateTabBox(type, text);
}

void InsertExpresWindow::AllData()
{
    keyboard_->Init(data_edit_->toPlainText());
    keyboard_->resize(this->size());
    keyboard_->move(0, 0);
    keyboard_->show();
    isall_ = true;
}

void InsertExpresWindow::SelectData()
{
    auto pos = curr_point_.lock();
    if(pos.isNull())
        return;

    keyboard_->Init(pos->code);
    keyboard_->resize(this->size());
    keyboard_->move(0, 0);
    keyboard_->show();
    isall_ = false;
}

void InsertExpresWindow::HideKey()
{
    keyboard_->hide();
}

void InsertExpresWindow::GetKeyData(QString text)
{
    if(text.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入文本"));
        return;
    }
    if(data_edit_->toPlainText().indexOf(" := ") != -1 && isall_)
    {
        if(text.indexOf(" := ") == -1)
        {
            if(text.indexOf(":=") == -1)
            {
                QMessageBox::warning(this, tr("提示"), tr("赋值语句必须拥有:="));
                return;
            }
            else
            {
                text.replace(":=", " := ");
            }
        }
    }

    for(auto &key : add_list_) //为运算符与值之间添加空格
    {
        if(text.indexOf(key) != -1)
            text.replace(key, " " + key + " ");
    }

    if(isall_)
    {
        if(data_edit_->toPlainText().indexOf(";") != -1)
        {
            if(text.at(text.size() - 1) != ';')
            {
                QMessageBox::warning(this, tr("提示"), tr("请以;结尾"));
                return;
            }
        }

        data_edit_->setPlainText(text);
    }
    else
    {
        auto pt = curr_point_.lock();
        if(pt.isNull())
        {
            keyboard_->hide();
            return;
        }

        auto data = data_edit_->toPlainText();
        data.replace(pt->seat, pt->code.size(), text);

        data_edit_->setPlainText(data);
    }

    Analysis();
    keyboard_->hide();
}

void InsertExpresWindow::AddBt()
{
    auto pt = curr_point_.lock();
    if(pt.isNull())
        return;

    auto data = data_edit_->toPlainText();
    data.insert(pt->seat + pt->code.size(), " + <EXP>");
    data_edit_->setPlainText(data);

    int old_curr = curr_;
    Analysis();
    auto pos = select_pos_.at(old_curr).lock();
    if(!pos.isNull())
    {
        curr_ = old_curr;
        curr_point_ = pos;
        SelectText(pos->seat, pos->code.size());
    }

    if(separate_ != 0.0)
    {
        auto curr_pt = select_pos_.at(curr_).lock();
        bool new_lg;
        if(curr_pt->seat > separate_)
            new_lg = true;
        else
            new_lg = false;
        if(new_lg != lerg_)
            Init(activity_label_->text(), new_lg);
    }
}

void InsertExpresWindow::RedBt()
{
    auto data = data_edit_->toPlainText();

    auto point = curr_point_.lock();
    if(point.isNull())
        return;

    auto *parent = point->parent;
    if(parent != nullptr)
    {
        if(parent->code.indexOf(" := ") != -1)
            parent = nullptr;
    }

    if(parent == nullptr)
    {
        auto text = point->code;
        for(auto &list : add_list_)
        {
            int pos = text.indexOf(list);
            if(pos != -1)
            {
                for(int i = 0; i < point->nodes.size(); i++)
                {
                    auto &pt = point->nodes[i];
                    if(pt->code == list)
                    {
                        auto &next = point->nodes[i + 1];

                        auto str = " " + pt->code + " " + next->code;
                        auto text_back = text;
                        text.remove(text.indexOf(str), str.size());
                        data.replace(data.indexOf(text_back), text_back.size(), text);
                        data_edit_->setPlainText(data);

                        int old_curr = curr_;
                        Analysis();
                        if(old_curr < select_pos_.size())
                        {
                            auto ps = select_pos_.at(old_curr).lock();
                            if(!ps.isNull())
                            {
                                curr_ = old_curr;
                                curr_point_ = ps;
                                SelectText(ps->seat, ps->code.size());
                            }
                        }

                        if(separate_ != 0.0)
                        {
                            auto curr_pt = select_pos_.at(curr_).lock();
                            bool new_lg;
                            if(curr_pt->seat > separate_)
                                new_lg = true;
                            else
                                new_lg = false;
                            if(new_lg != lerg_)
                                Init(activity_label_->text(), new_lg);
                        }

                        return;
                    }
                }
            }
        }
    }
    else
    {
        auto text = parent->code;
        for(auto &list : add_list_)
        {
            int pos = text.indexOf(list);
            if(pos != -1)
            {
                for(int i = 0; i < parent->nodes.size(); i++)
                {
                    auto &pt = parent->nodes[i];
                    if(pt->code == list)
                    {
                        auto &next = parent->nodes[i + 1];

                        auto str = " " + pt->code + " " + next->code;
                        auto text_back = text;
                        text.remove(text.indexOf(str), str.size());
                        data.replace(data.indexOf(text_back), text_back.size(), text);
                        data_edit_->setPlainText(data);

                        int old_curr = curr_;
                        Analysis();
                        if(old_curr < select_pos_.size())
                        {
                            auto ps = select_pos_.at(old_curr).lock();
                            if(!ps.isNull())
                            {
                                curr_ = old_curr;
                                curr_point_ = ps;
                                SelectText(ps->seat, ps->code.size());
                            }
                        }

                        if(separate_ != 0.0)
                        {
                            auto curr_pt = select_pos_.at(curr_).lock();
                            bool new_lg;
                            if(curr_pt->seat > separate_)
                                new_lg = true;
                            else
                                new_lg = false;
                            if(new_lg != lerg_)
                                Init(activity_label_->text(), new_lg);
                        }

                        return;
                    }
                }
            }
        }
    }
}

void InsertExpresWindow::BakBt()
{
    auto pt = curr_point_.lock();
    if(pt.isNull())
        return;

    auto str = pt->code;
    if(add_list_.count(str) != 0)
    {
        return;
    }

    auto data = data_edit_->toPlainText();
    auto text = '(' + str + ')';
    data.replace(pt->seat, str.size(), text);
    data_edit_->setPlainText(data);

    int old_curr = curr_;
    Analysis();
    auto pos = select_pos_.at(old_curr).lock();
    if(!pos.isNull())
    {
        curr_ = old_curr;
        curr_point_ = pos;
        SelectText(pos->seat, pos->code.size());
    }

    if(separate_ != 0.0)
    {
        auto curr_pt = select_pos_.at(curr_).lock();
        bool new_lg;
        if(curr_pt->seat > separate_)
            new_lg = true;
        else
            new_lg = false;
        if(new_lg != lerg_)
            Init(activity_label_->text(), new_lg);
    }
}

void InsertExpresWindow::RbaBt()
{
    auto pt = curr_point_.lock();
    if(pt.isNull())
        return;

    auto str = pt->code;
    if(add_list_.count(str) != 0)
    {
        return;
    }

    auto data = data_edit_->toPlainText();
    if(str.at(0) == '(' && str.at(str.size() - 1) == ')')
    {
        str = str.mid(1, str.size() - 2);
    }
    else
    {
        if(pt->seat == 0 || pt->seat + str.size() >= data.size())
        {
            return;
        }
    }
    if(str == pt->code)
        return;
    data.replace(pt->seat, pt->code.size(), str);
    data_edit_->setPlainText(data);

    int old_curr = curr_;
    Analysis();
    auto pos = select_pos_.at(old_curr).lock();
    if(!pos.isNull())
    {
        curr_ = old_curr;
        curr_point_ = pos;
        SelectText(pos->seat, pos->code.size());
    }

    if(separate_ != 0.0)
    {
        auto curr_pt = select_pos_.at(curr_).lock();
        bool new_lg;
        if(curr_pt->seat > separate_)
            new_lg = true;
        else
            new_lg = false;
        if(new_lg != lerg_)
            Init(activity_label_->text(), new_lg);
    }
}

void InsertExpresWindow::Init(QString type, bool lerg)
{
    lerg_ = lerg;
    activity_label_->setText(type);
    result_label_->setText(type);

    if(filter_key_->isHidden())
    {
        UpdateTabBox(type);
    }
    else
    {
        UpdateTabBox(type, filter_key_->GetFilter());
    }
}

void InsertExpresWindow::ConnectInit()
{
    connect(define_button_, &QPushButton::clicked, this, &InsertExpresWindow::DefineClicked);

    connect(data_box_, &TableBox::itemClicked, this, &InsertExpresWindow::DataBoxItem);

    connect(func_box_, &TableBox::itemClicked, this, &InsertExpresWindow::FuncBoxItem);

    connect(left_button_, &QPushButton::clicked, this, &InsertExpresWindow::Left);

    connect(right_button_, &QPushButton::clicked, this, &InsertExpresWindow::Right);

    connect(filter_button_, &ComboBoxObject::clicked, this, &InsertExpresWindow::ShidFilterWindow);

    connect(filter_key_->reset_button_, &QPushButton::clicked, this, &InsertExpresWindow::ResetFilter);

    connect(filter_key_, &FilterKey::GetData, this, &InsertExpresWindow::FilterText);

    connect(all_an_, &QAction::triggered, this, &InsertExpresWindow::AllData);

    connect(select_an_, &QAction::triggered, this, &InsertExpresWindow::SelectData);

    connect(keyboard_->cancel_button_, &QPushButton::clicked, this, &InsertExpresWindow::HideKey);

    connect(keyboard_, &Keyboard::GetData, this, &InsertExpresWindow::GetKeyData);

    connect(add_button_, &QPushButton::clicked, this, &InsertExpresWindow::AddBt);

    connect(red_button_, &QPushButton::clicked, this, &InsertExpresWindow::RedBt);

    connect(brack_button_, &QPushButton::clicked, this, &InsertExpresWindow::BakBt);

    connect(rebra_button_, &QPushButton::clicked, this, &InsertExpresWindow::RbaBt);

    connect(change_type_button_, &QPushButton::clicked, this, &InsertExpresWindow::ChangeType);
}

void InsertExpresWindow::UpdateTabBox(QString &type, QString filter)
{
    data_box_->clear();
    data_box_->SetRomn(1, 2);
    func_box_->clear();
    func_box_->SetRomn(0, 0);
    data_dim_.clear();

    int r1 = 0, c1 = 0;

    if(type == "signaldi") //IO
    {
        for(auto &str : di_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }
    else if(type == "signaldo")
    {
        for(auto &str : do_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }
    else if(type == "signalai")
    {
        for(auto &str : ai_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }
    else if(type == "signalao")
    {
        for(auto &str : ao_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }

    auto list = VarInfo::instance()->GetTypeVar(type);

    data_box_->AddText(r1, c1++, tr("新建"));
    iscreate_ = true;

    for(auto &p : list)
    {
        if(!filter.isEmpty())
        {
            if(p.first.toUpper().indexOf(filter.toUpper()) == -1)
                continue;
        }

        if(!lerg_) //左侧
        {
            if(p.second.genre) //只显示用户定义变量
            {
                if(p.second.action_scope == "local" && p.second.mod != mod_name_)
                {
                    continue;
                }
                else if(p.second.action_scope != "all")
                {
                    if(p.second.action_scope != "task")
                    {
                        if(p.second.mod != mod_name_)
                            continue;
                    }
                }
                else if(p.second.action_scope != "task")
                {
                    if(p.second.action_scope != "all")
                    {
                        if(p.second.mod != mod_name_)
                            continue;
                    }
                }

                data_box_->AddText(r1, c1++, p.first);
                auto *item = data_box_->item(r1, c1 - 1);
                data_dim_.insert(item, p.second.dim);
                if(c1 > 1)
                {
                    r1++;
                    c1 = 0;
                }
            }
        }
        else
        {
            if(p.second.action_scope == "local" && p.second.mod != mod_name_)
            {
                continue;
            }
            else if(p.second.action_scope != "all")
            {
                if(p.second.action_scope != "task")
                {
                    if(p.second.mod != mod_name_)
                        continue;
                }
            }
            else if(p.second.action_scope != "task")
            {
                if(p.second.action_scope != "all")
                {
                    if(p.second.mod != mod_name_)
                        continue;
                }
            }

            data_box_->AddText(r1, c1++, p.first);
            auto *item = data_box_->item(r1, c1 - 1);
            data_dim_.insert(item, p.second.dim);
            if(c1 > 1)
            {
                r1++;
                c1 = 0;
            }
        }
    }

    if(lerg_)
    {
        add_button_->setEnabled(true);
        red_button_->setEnabled(true);
        brack_button_->setEnabled(true);
        rebra_button_->setEnabled(true);

        auto temp_func_list = EditData::instance()->GetRoutName(RoutineData::FUNC);

        int r2 = 0, c2 = 0;
        for(auto rout : temp_func_list)
        {
            auto &rout_info = EditData::instance()->GetRoutData(rout);

            if(rout_info.data_type == type)
            {
                func_box_->AddText(r2, c2++, rout + "()");
                if(c2 > 1)
                {
                    r2++;
                    c2 = 0;
                }
            }
        }
    }
    else
    {
        add_button_->setEnabled(false);
        red_button_->setEnabled(false);
        brack_button_->setEnabled(false);
        rebra_button_->setEnabled(false);
    }
}

void InsertExpresWindow::DataEditClicked()
{
    auto sor = data_edit_->textCursor();

    sor.clearSelection();
    sor.select(QTextCursor::WordUnderCursor);
    auto word = sor.selectedText();
    int pos = sor.position();

    int i;
    QString str;
    int s = 0, e = 0;
    for(i = 0; i < select_pos_.count(); i++)
    {
        auto it = select_pos_[i].lock();
        if(it.isNull())
            continue;

        if(it->seat <= pos && pos <= it->seat + it->code.size())
        {
            s = it->seat;
            e = it->code.size();

            str = curr_point_.lock()->code;

            curr_ = i;
            curr_point_ = it;
            break;
        }
    }
    if(i >= select_pos_.count())
        return;

    if(separate_ != 0.0)
    {
        if(pos > separate_ && !lerg_)
            Init(activity_label_->text(), true);
        else if(pos < separate_ && lerg_)
            Init(activity_label_->text(), false);
    }

    if(add_list_.count(str) != 0)
    {
        auto type = activity_label_->text();
        UpdateTabBox(type, filter_key_->GetFilter());
    }

    SelectText(s, e);
}

void InsertExpresWindow::Analysis()
{
    int old_pos = curr_;
    int old_size = select_pos_.size();
    select_pos_.clear();
    curr_ = 0;
    separate_ = 0;
    curr_point_.clear();

    auto text = data_edit_->toPlainText();

    node_point_ = InsertCodeNode::MakePointer(text, nullptr);

    if(node_point_->code.indexOf(":=") != -1) //赋值语句
    {
        node_point_->SetSeat(select_pos_, 0);

        auto str = node_point_->code.section(" := ", 0, 0);
        if(str.indexOf('{') != -1)
        {
            separate_ = str.indexOf('{') + 0.5;
        }
        else
        {
            separate_ = str.size() + 0.5;
        }
    }
    else //其他语句
    {
        select_pos_.append(node_point_);
        node_point_->SetSeat(select_pos_, 0);
    }

    if(select_pos_.isEmpty())
    {
        return;
    }

    if(old_size == select_pos_.size())
    {
        curr_ = old_pos;
    }
    else
    {
        curr_ = 0;
    }

    auto pos = select_pos_[curr_].lock();
    if(!pos.isNull())
    {
        curr_point_ = pos;
        SelectText(pos->seat, pos->code.size());
    }
}

bool InsertExpresWindow::SelectText(QString text, int num)
{
    QTextDocument *document = data_edit_->document();
    bool found = false;

    QTextCursor curr(document);
    QTextCursor end_curr(document);

    curr.beginEditBlock();
    QTextCharFormat color(curr.charFormat());
    color.setForeground(Qt::blue);

    int i = 0;
    while(!curr.isNull() && !curr.atEnd())
    {
        curr = document->find(text, curr, QTextDocument::FindWholeWords);
        if(!curr.isNull())
        {
            if(!found)
                found = true;
            i++;
            if(i >= num && num != -1)
            {
                int start = curr.position() - text.size();
                int end = curr.position();

                curr.setPosition(start, QTextCursor::MoveAnchor);
                curr.setPosition(end, QTextCursor::KeepAnchor);

                data_edit_->setTextCursor(curr);
                break;
            }
        }
    }

    end_curr.endEditBlock();

    return found;
}

void InsertExpresWindow::SelectText(int start, int size)
{
    QTextCursor sor = data_edit_->textCursor();

    int end = start + size;

    sor.setPosition(start, QTextCursor::MoveAnchor);
    sor.setPosition(end, QTextCursor::KeepAnchor);

    data_edit_->setTextCursor(sor);
}

InsertCodeText::InsertCodeText(QWidget *parent) : QTextBrowser (parent)
{
    this->setContextMenuPolicy(Qt::NoContextMenu); //禁止右键鼠标弹出菜单
}

void InsertCodeText::SetParentPointer(InsertExpresWindow *pointer)
{
    parent_ = pointer;
}

void InsertCodeText::mousePressEvent(QMouseEvent *event)
{
    QTextBrowser::mousePressEvent(event);

    if(!parent_.isNull())
        parent_->DataEditClicked();
}

void InsertCodeText::mouseDoubleClickEvent(QMouseEvent *)
{
}

void InsertCodeText::mouseMoveEvent(QMouseEvent *)
{
}

void InsertCodeText::mouseReleaseEvent(QMouseEvent *)
{
}

InsertCodeNode::InsertCodeNode(InsertCodeNode *parent)
{
    this->parent = parent;
    seat = 0;
}

QSharedPointer<InsertCodeNode> InsertCodeNode::MakePointer(QString text, InsertCodeNode *parent)
{
    QSharedPointer<InsertCodeNode> point(new InsertCodeNode(parent));
    point->SetCode(qMove(text));

    return point;
}

void InsertCodeNode::SetCode(QString text)
{
    if(text.indexOf(" := ") != -1)
    {
        code = text;

        auto left_str = text.section(" := ", 0, 0);
        auto right_str = text.section(" := ", 1, 1);
        right_str = right_str.mid(0, right_str.size() - 1);

        auto left_point = MakePointer(left_str, this);
        auto right_point = MakePointer(right_str, this);

        nodes.append(left_point);
        nodes.append(right_point);

        return;
    }

    code = text;

    int brackets = 0;
    QString temp_code;
    for(int i = 0; i < text.size(); i++)
    {
        auto curr_str = text.at(i);

        if(curr_str == '{')
        {
            temp_code.append(curr_str);
            brackets++;
        }
        else if(curr_str == '}')
        {
            temp_code.append(curr_str);
            brackets--;

            if(temp_code == code)
            {
                if(brackets == 0)
                {
                    int start = temp_code.indexOf('{') + 1;
                    int end = temp_code.indexOf('}') - start;
                    if(start == -1 || end == -1)
                        continue;

                    auto str = temp_code.mid(start, end);
                    auto point = MakePointer(str, this);
                    nodes.append(point);
                    temp_code.clear();
                }
                continue;
            }
            if(brackets == 1)
            {
                int a = temp_code.indexOf('{');
                int b = temp_code.indexOf(',');

                QString str;

                if(b != -1 && b < a)
                {
                    str = temp_code.mid(b + 1);
                }
                else
                {
                    b = temp_code.indexOf(' ');

                    if(b != -1 && b < a)
                        str = temp_code.mid(b + 1);
                    else
                        str = temp_code;
                }

                if(str.at(0) == '(')
                    str = str.mid(1);
                if(str.at(str.size() - 1) == ')')
                    str = str.mid(0, str.size() - 1);

                auto point = MakePointer(str, this);
                nodes.append(point);

                temp_code.clear();
            }
        }
        else if(curr_str == ',' || curr_str == ' ')
        {
            if(brackets <= 1)
            {
                int pos = temp_code.indexOf('{');
                if(pos == -1)
                    pos = 0;

                if(pos != 0)
                    pos++;
                auto str(temp_code.mid(pos));
                if(str.at(0) == '(')
                    str = str.mid(1);
                if(str.at(str.size() - 1) == ')')
                    str = str.mid(0, str.size() - 1);

                auto point = MakePointer(str, this);
                nodes.append(point);

                temp_code.clear();
            }
            else
            {
                temp_code.append(curr_str);
            }
        }
        else
        {
            temp_code.append(curr_str);
        }
    }

    if(code != temp_code && temp_code != "}" && !temp_code.isEmpty())
    {
        if(temp_code.at(temp_code.size() - 1) == '}')
            temp_code = temp_code.mid(0, temp_code.size() - 1);

        auto point = MakePointer(temp_code, this);
        nodes.append(point);
    }
}

void InsertCodeNode::SetSeat(QList<QWeakPointer<InsertCodeNode>> &select_pos, int st)
{
    int num = 0;
    for(auto &point : nodes)
    {
        int pos = code.indexOf(point->code, num);
        point->seat = pos + st;
        num = point->code.size() + pos;
        select_pos.append(point);

        point->SetSeat(select_pos, point->seat);
    }
}

ChangeIFandFOR::ChangeIFandFOR(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    tips_label_ = new QLabel(tr("选择待更改的部分。"), this);
    list_widget_ = new ListBoxObject(this);
    button_widget_ = new QWidget(this);
    auto *bt_layout = new QHBoxLayout(button_widget_);

    button1_ = new QPushButton(button_widget_);
    button2_ = new QPushButton(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    button1_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button2_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button1_->setFlat(true);
    button2_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    bt_layout->addWidget(button1_);
    bt_layout->addWidget(button2_);
    bt_layout->addStretch();
    bt_layout->addWidget(define_button_);
    bt_layout->addWidget(cancel_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addWidget(tips_label_);
    layout->addWidget(list_widget_);
    layout->addWidget(button_widget_);
    layout->setStretchFactor(tips_label_, 1);
    layout->setStretchFactor(list_widget_, 11);
    layout->setStretchFactor(button_widget_, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void ChangeIFandFOR::Init(int type, QString code)
{
    type_ = type;
    list_widget_->clear();
    code_map_.clear();
    code_list_.clear();
    is_else_ = false;
    code.replace("\t", "");

    code_list_ = code.split("\u2029");

    if(type == 1) //FOR
    {
        auto &str = code_list_[0];
        button1_->show();

        if(str.lastIndexOf("STEP") != -1)
        {
            old_item_ = list_widget_->AddText("FOR <ID> FROM <EXT> TO <EXP> STEP <EXP> DO");

            button1_->setText(tr("删除 STEP"));
            button1_->setWindowTitle("1");
        }
        else
        {
            old_item_ = list_widget_->AddText("FOR <ID> FROM <EXT> TO <EXP> DO");

            button1_->setText(tr("添加 STEP"));
            button1_->setWindowTitle("0");
        }

        list_widget_->AddText("<SMT>", false);
        list_widget_->AddText("ENDFOR", false);
        list_widget_->setCurrentRow(0);
        button2_->hide();
    }
    else if(type == 0) //IF
    {
        QListWidgetItem *item = nullptr;
        QString code;
        for(auto &str : code_list_)
        {
            auto head = str.section(" ", 0, 0);

            if(head == "IF")
            {
                if(str.section(" ", 1, 1) != "<EXP>")
                    item = list_widget_->AddText("IF <Expression> THEN");
                else
                    item = list_widget_->AddText("IF <EXP> THEN");

                code.append(str);
            }
            else if(head == "ELSEIF")
            {
                list_widget_->AddText("<SMT>", false);
                code_map_.insert(item, qMove(code));
                item = nullptr;
                code.clear();

                if(str.section(" ", 1, 1) == "<EXP>")
                    item = list_widget_->AddText("ELSEIF <EXP> THEN");
                else
                    item = list_widget_->AddText("ELSEIF <Expression> THEN");

                code.append(str);
            }
            else if(head == "ELSE")
            {
                list_widget_->AddText("<SMT>", false);
                code_map_.insert(item, qMove(code));
                item = nullptr;
                code.clear();
                is_else_ = true;

                item = list_widget_->AddText("ELSE");
                code.append(str);
            }
            else if(head == "ENDIF")
            {
                list_widget_->AddText("<SMT>", false);
                code_map_.insert(item, qMove(code));
                item = nullptr;
                code.clear();

                item = list_widget_->AddText("ENDIF", false);
                code_map_.insert(item, "ENDIF");
            }
            else
            {
                code.append("\n" + str);
            }
        }

        list_widget_->setCurrentRow(0);
        old_item_ = list_widget_->item(0);
        button1_->setText(tr("添加 ELSE"));
        button2_->setText(tr("添加 ELSEIF"));
        button1_->setWindowTitle("0");
        button2_->setWindowTitle("0");
        if(is_else_)
        {
            button1_->hide();
            button2_->show();
        }
        else
        {
            button1_->show();
            button2_->show();
        }
    }
    else //TEST
    {
        QListWidgetItem *item = nullptr;
        QString code;
        for(auto &str : code_list_)
        {
            auto head = str.section(" ", 0, 0);

            if(head == "TEST")
            {
                auto *test_item = list_widget_->AddText("TEST <Expression>");
                item = nullptr;
                code.clear();

                code_map_.insert(test_item, str);
            }
            else if(head == "CASE")
            {
                if(item != nullptr)
                {
                    list_widget_->AddText("<SMT>", false);
                    code_map_.insert(item, qMove(code));
                    item = nullptr;
                    code.clear();
                }

                item = list_widget_->AddText("CASE <Test Value>:");
                code.append(str);
            }
            else if(head == "DEFAULT:")
            {
                if(item != nullptr)
                {
                    list_widget_->AddText("<SMT>", false);
                    code_map_.insert(item, qMove(code));
                    item = nullptr;
                    code.clear();
                    is_else_ = true;
                }

                item = list_widget_->AddText("DEFAULT:");
                code.append(str);
            }
            else if(head == "ENDTEST")
            {
                if(item != nullptr)
                {
                    list_widget_->AddText("<SMT>", false);
                    code_map_.insert(item, qMove(code));
                    item = nullptr;
                    code.clear();
                }

                item = list_widget_->AddText("ENDTEST", false);
                code_map_.insert(item, "ENDTEST");
            }
            else
            {
                code.append("\n" + str);
            }
        }

        list_widget_->setCurrentRow(0);
        old_item_ = list_widget_->item(0);
        button1_->setText(tr("添加 DEFAULT"));
        button2_->setText(tr("添加 CASE"));
        if(is_else_)
        {
            button1_->hide();
            button2_->show();
        }
        else
        {
            button1_->show();
            button2_->show();
        }
    }
}

void ChangeIFandFOR::SetScale(double scale_w, double scale_h)
{
    list_widget_->SetScale(scale_w, scale_h);
}

void ChangeIFandFOR::DefineClicked()
{
    QString code;

    if(type_ == 0 || type_ == 2)
    {
        for(int i = 0; i < list_widget_->count(); i++)
        {
            auto *item = list_widget_->item(i);

            if(!code_map_.contains(item))
                continue;

            auto &val = code_map_[item];

            if(code.isEmpty())
                code.append(qMove(val));
            else
                code.append("\n" + qMove(val));
        }
    }
    else
    {
        for(auto &str : code_list_)
        {
            if(code.isEmpty())
                code.append(qMove(str));
            else
                code.append("\n" + qMove(str));
        }
    }

    emit Define(code);
}

void ChangeIFandFOR::Button1Clicked()
{
    auto *item = list_widget_->currentItem();
    if(item != old_item_)
        return;

    if(type_ == 1) //FOR
    {
        auto &str = code_list_[0];
        if(button1_->windowTitle() == "0") //添加
        {
            item->setText("FOR <ID> FROM <EXT> TO <EXP> STEP <EXP> DO");

            int n = str.lastIndexOf(" ");
            str.insert(n, " STEP <EXP>");

            button1_->setText(tr("删除 STEP"));
            button1_->setWindowTitle("1");
        }
        else //删除
        {
            item->setText("FOR <ID> FROM <EXT> TO <EXP> DO");

            int n = str.lastIndexOf("STEP");
            str.remove(n, str.size() - n);
            str.append("DO");

            button1_->setText(tr("添加 STEP"));
            button1_->setWindowTitle("0");
        }
    }
    else if(type_ == 0) //IF
    {
        auto head = item->text().section(" ", 0, 0);

        if(head == "IF")
        {
            auto *else_item = list_widget_->AddText("ELSE", list_widget_->count() - 1);
            list_widget_->AddText("<SMT>", list_widget_->count() - 1, false);
            code_map_.insert(else_item, "ELSE\n<SMT>");

            button1_->hide();
            is_else_ = true;
        }
        else if(head == "ELSEIF")
        {
            auto &pair = code_map_[item];

            auto msg = tr("确定要删除ELSEIF语句?\n以下指令将会丢失:\n") + pair;

            if(QMessageBox::warning(this, tr("警告"), msg, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                int row = list_widget_->row(item);
                code_map_.remove(item);
                auto *next_item = list_widget_->item(row + 1);

                list_widget_->RemoveItem(item);
                list_widget_->RemoveItem(next_item);

                list_widget_->setCurrentRow(0);
                this->ListItemClicked(list_widget_->item(0));

            }
        }
        else if(head == "ELSE")
        {
            auto &pair = code_map_[item];

            auto msg = tr("确定要删除ELSE语句?\n以下指令将会丢失:\n") + pair;

            if(QMessageBox::warning(this, tr("警告"), msg, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                int row = list_widget_->row(item);
                code_map_.remove(item);
                auto *next_item = list_widget_->item(row + 1);

                list_widget_->RemoveItem(item);
                list_widget_->RemoveItem(next_item);

                is_else_ = false;
                list_widget_->setCurrentRow(0);
                this->ListItemClicked(list_widget_->item(0));
            }
        }
    }
    else //TEST
    {
        auto head = item->text().section(" ", 0, 0);
        if(head == "TEST")
        {
            auto *def_item = list_widget_->AddText("DEFAULT:", list_widget_->count() - 1);
            list_widget_->AddText("<SMT>", list_widget_->count() - 1, false);
            code_map_.insert(def_item, "DEFAULT:\n<SMT>");

            button1_->hide();
            is_else_ = true;
        }
        else if(head == "CASE")
        {
            auto &pair = code_map_[item];

            auto msg = tr("确定要删除CASE语句?\n以下指令将会丢失:\n") + pair;

            if(QMessageBox::warning(this, tr("警告"), msg, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                int row = list_widget_->row(item);
                code_map_.remove(item);
                auto *next_item = list_widget_->item(row + 1);

                list_widget_->RemoveItem(item);
                list_widget_->RemoveItem(next_item);

                list_widget_->setCurrentRow(0);
                this->ListItemClicked(list_widget_->item(0));

            }
        }
        else if(head == "DEFAULT:")
        {
            auto &pair = code_map_[item];

            auto msg = tr("确定要删除DEFAULT语句?\n以下指令将会丢失:\n") + pair;

            if(QMessageBox::warning(this, tr("警告"), msg, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                int row = list_widget_->row(item);
                code_map_.remove(item);
                auto *next_item = list_widget_->item(row + 1);

                list_widget_->RemoveItem(item);
                list_widget_->RemoveItem(next_item);

                is_else_ = false;
                list_widget_->setCurrentRow(0);
                this->ListItemClicked(list_widget_->item(0));
            }
        }
    }
}

void ChangeIFandFOR::Button2Clicked()
{
    auto *item = list_widget_->currentItem();
    if(item != old_item_)
            return;

    if(type_ == 0) //IF
    {
        if(is_else_)
        {
            auto *elif_item = list_widget_->AddText("ELSEIF <EXP> THEN", list_widget_->count() - 3);
            list_widget_->AddText("<SMT>", list_widget_->count() - 3, false);
            code_map_.insert(elif_item, "ELSEIF <EXP> THEN\n<SMT>");
        }
        else
        {
            auto *elif_item = list_widget_->AddText("ELSEIF <EXP> THEN", list_widget_->count() - 1);
            list_widget_->AddText("<SMT>", list_widget_->count() - 1, false);
            code_map_.insert(elif_item, "ELSEIF <EXP> THEN\n<SMT>");
        }
    }
    else if(type_ == 2) //TEST
    {
        if(is_else_)
        {
            auto *def_item = list_widget_->AddText("CASE <Test Value>:", list_widget_->count() - 3);
            list_widget_->AddText("<SMT>", list_widget_->count() - 3, false);
            code_map_.insert(def_item, "CASE <EXP>:\n<SMT>");
        }
        else
        {
            auto *def_item = list_widget_->AddText("CASE <Test Value>:", list_widget_->count() - 1);
            list_widget_->AddText("<SMT>", false);
            code_map_.insert(def_item, "ELSEIF <EXP> THEN\n<SMT>");
        }
    }
}

void ChangeIFandFOR::ListItemClicked(QListWidgetItem *item)
{
    if(item == nullptr)
        return;

    if(type_ == 0)
    {
        if(!code_map_.contains(item) || item->text() == "ENDIF")
            return;
        old_item_ = item;

        auto head = item->text().section(" ", 0, 0);
        if(head == "IF")
        {
            if(is_else_)
            {
                button1_->hide();
            }
            else
            {
                button1_->show();
                button1_->setText(tr("添加 ELSE"));
                button1_->setWindowTitle("0");
            }
            button2_->show();
            button2_->setText(tr("添加 ELSEIF"));
            button2_->setWindowTitle("0");
        }
        else if(head == "ELSEIF")
        {
            button1_->show();
            button1_->setText(tr("删除 ELSEIF"));
            button1_->setWindowTitle("1");

            button2_->show();
            button2_->setText(tr("添加 ELSEIF"));
            button2_->setWindowTitle("0");
        }
        else if(head == "ELSE")
        {
            button1_->show();
            button1_->setText(tr("删除 ELSE"));
            button1_->setWindowTitle("1");

            button2_->hide();
        }
    }
    else if(type_ == 2) //TEST
    {
        if(!code_map_.contains(item) || item->text() == "ENDTEST")
            return;
        old_item_ = item;

        auto head = item->text().section(" ", 0, 0);
        if(head == "TEST")
        {
            if(is_else_)
            {
                button1_->hide();
            }
            else
            {
                button1_->show();
                button1_->setText(tr("添加 DEFAULT"));
                button1_->setWindowTitle("0");
            }
            button2_->show();
            button2_->setText(tr("添加 CASE"));
            button2_->setWindowTitle("0");
        }
        else if(head == "CASE")
        {
            button1_->show();
            button1_->setText(tr("删除 CASE"));
            button1_->setWindowTitle("1");

            button2_->show();
            button2_->setText(tr("添加 CASE"));
            button2_->setWindowTitle("0");
        }
        else if(head == "DEFAULT:")
        {
            button1_->show();
            button1_->setText(tr("删除 DEFAULT"));
            button1_->setWindowTitle("1");

            button2_->hide();
        }
    }
}

void ChangeIFandFOR::ConnectInit()
{
    connect(cancel_button_, &QPushButton::clicked, this, &ChangeIFandFOR::Cancel);

    connect(define_button_, &QPushButton::clicked, this, &ChangeIFandFOR::DefineClicked);

    connect(button1_, &QPushButton::clicked, this, &ChangeIFandFOR::Button1Clicked);

    connect(button2_, &QPushButton::clicked, this, &ChangeIFandFOR::Button2Clicked);

    connect(list_widget_, &ListBoxObject::itemClicked, this, &ChangeIFandFOR::ListItemClicked);
}

ChangeMoveWindow::ChangeMoveWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    head_widget_ = new QWidget(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips1_label_ = new QLabel(tr("当前指令:"), head_widget_);
    current_label_= new QLabel(head_widget_);
    tips2_label_ = new QLabel(tr("选择待更改的变量"), head_widget_);
    head_layout->addWidget(tips1_label_, 0, 0);
    head_layout->addWidget(current_label_, 0, 1);
    head_layout->addWidget(tips2_label_, 1, 0);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    auto *bt_layout = new QHBoxLayout(button_widget_);
    opt_var_button_ = new QPushButton(tr("可选变量"), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    opt_var_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    opt_var_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    bt_layout->addWidget(opt_var_button_);
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addWidget(define_button_);
    bt_layout->addWidget(cancel_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(head_widget_, 2);
    layout->setStretchFactor(table_box_, 5);
    layout->setStretchFactor(button_widget_, 1);

    table_box_->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(1, 2);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("自变量") << tr("值"));

    opt_var_window_ = new MoveOptVarWindow(this);
    opt_var_window_->hide();
    change_select_window_ = new ChangeSelectionWindow(this);
    change_select_window_->hide();

    opt_var_button_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void ChangeMoveWindow::Init(QString code, QString mod_name, QString rout_name, int row)
{
    mod_name_ = mod_name;
    rout_name_ = rout_name;
    row_ = row;

    Init(code);
}

ChangeSelectionWindow *ChangeMoveWindow::GetChSl()
{
    return change_select_window_;
}

void ChangeMoveWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);

    if(!opt_var_window_->isHidden())
    {
        opt_var_window_->resize(this->size());
        opt_var_window_->move(0, 0);
    }
    opt_var_window_->SetScale(scale_w, scale_h);

    if(!change_select_window_->isHidden())
    {
        change_select_window_->resize(this->size());
        change_select_window_->move(0, 0);
    }
    change_select_window_->SetScale(scale_w, scale_h);
}

void ChangeMoveWindow::DefineClicked()
{
    auto head = current_label_->text();

    if(head == "MoveC")
    {
        auto var1 = code_.section(" ", 1, 1);
        auto var2 = code_.section(" ", 2, 2);
        if(var1 == "*,")
        {
            auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
            auto s1 = rout_str.section(" ", 1, 1);

            code_.replace(var1, s1);
        }
        if(var2 == "*,")
        {
            auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
            auto s2 = rout_str.section(" ", 1, 1);

            code_.replace(var2, s2);
        }
    }
    else if(head == "MoveJ" || head == "MoveL")
    {
        auto var1 = code_.section(" ", 1, 1);

        if(var1 == "*,")
        {
            auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
            auto s1 = rout_str.section(" ", 1, 1);

            code_.replace(var1, s1);
        }
    }
    else if(head == "MoveAbsJ")
    {
        auto var1 = code_.section(" ", 1, 1);

        if(var1 == "*\\NoEoffs,")
        {
            auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
            auto s1 = rout_str.section(" ", 1, 1);

            code_.replace(var1, s1);
        }
    }

    emit Define(code_);
}

void ChangeMoveWindow::ShowOptVar()
{
    opt_var_window_->resize(this->size());
    opt_var_window_->move(0, 0);
    opt_var_window_->show();
}

void ChangeMoveWindow::ItemChicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;
    if(item->flags() == Qt::NoItemFlags)
        return;

    int select_row = table_box_->row(item);
    if(select_row != 0 && code_.section(" ", 0, 0) == "MoveAbsJ")
        select_row--;

    auto orig_code = EditData::instance()->GetRoutData(rout_name_).code.at(row_);

    change_select_window_->resize(this->size());
    change_select_window_->move(0, 0);
    change_select_window_->show();
    change_select_window_->Init(code_, orig_code, mod_name_, select_row);
}

void ChangeMoveWindow::ChangeSelectDefine(QString code)
{
    change_select_window_->hide();

    auto head = code.section(" ", 0, 0);
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

    Init(code);
}

void ChangeMoveWindow::ConnectInit()
{
    connect(define_button_, &QPushButton::clicked, this, &ChangeMoveWindow::DefineClicked);

    connect(cancel_button_, &QPushButton::clicked, this, &ChangeMoveWindow::Cancel);

    connect(opt_var_window_, &MoveOptVarWindow::CloseWindow, opt_var_window_, &MoveOptVarWindow::hide);

    connect(opt_var_button_, &QPushButton::clicked, this, &ChangeMoveWindow::ShowOptVar);

    connect(table_box_, &TableBox::itemClicked, this, &ChangeMoveWindow::ItemChicked);

    connect(change_select_window_, &ChangeSelectionWindow::Cancel, change_select_window_, &ChangeSelectionWindow::hide);

    connect(change_select_window_, &ChangeSelectionWindow::Define, this, &ChangeMoveWindow::ChangeSelectDefine);
}

void ChangeMoveWindow::Init(QString code)
{
    code_ = code;
    auto head = code.section(" ", 0, 0);
    current_label_->setText(head);
    code = code.mid(head.size() + 1);
    auto code_list_ = code.split(", ");
    table_box_->clearContents();

    if(head == "MoveC")
    {
        if(code_list_.size() == 5)
        {
            auto s1 = code_list_[0];
            auto s2 = code_list_[1];
            auto s3 = code_list_[2];
            auto s4 = code_list_[3];
            auto s5 = code_list_[4];

            if(s1 == "*")
            {
                auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
                s1 = rout_str.section(" ", 1, 1);
            }
            if(s2 == "*")
            {
                auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
                s2 = rout_str.section(" ", 2, 2);
            }

            if(s1.at(s1.size() - 1) == ',')
                s1 = s1.mid(0, s1.size() - 1);
            if(s2.at(s2.size() - 1) == ',')
                s2 = s2.mid(0, s2.size() - 1);
            if(s5.at(s5.size() - 1) == ';')
                s5 = s5.mid(0, s5.size() - 1);

            table_box_->AddText(0, 0, "CirPoint");
            table_box_->AddText(0, 1, s1);
            table_box_->AddText(1, 0, "ToPoint");
            table_box_->AddText(1, 1, s2);
            table_box_->AddText(2, 0, "Speed");
            table_box_->AddText(2, 1, s3);
            table_box_->AddText(3, 0, "Zone");
            table_box_->AddText(3, 1, s4);
            table_box_->AddText(4, 0, "Tool");
            table_box_->AddText(4, 1, s5);
        }
    }
    else if(head == "MoveJ" || head == "MoveL")
    {
        if(code_list_.size() == 4)
        {
            auto s1 = code_list_.at(0);
            auto s2 = code_list_.at(1);
            auto s3 = code_list_.at(2);
            auto s4 = code_list_.at(3);

            if(s1 == "*")
            {
                auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
                s1 = rout_str.section(" ", 1, 1);
            }

            if(s1.at(s1.size() - 1) == ',')
                s1 = s1.mid(0, s1.size() - 1);
            if(s4.at(s4.size() - 1) == ';')
                s4 = s4.mid(0, s4.size() - 1);

            table_box_->AddText(0, 0, "ToPoint");
            table_box_->AddText(0, 1, s1);
            table_box_->AddText(1, 0, "Speed");
            table_box_->AddText(1, 1, s2);
            table_box_->AddText(2, 0, "Zone");
            table_box_->AddText(2, 1, s3);
            table_box_->AddText(3, 0, "Tool");
            table_box_->AddText(3, 1, s4);
        }
    }
    else if(head == "MoveAbsJ")
    {
        if(code_list_.size() == 4)
        {
            auto s1 = code_list_.at(0);
            auto s2 = code_list_.at(1);
            auto s3 = code_list_.at(2);
            auto s4 = code_list_.at(3);

            if(s1.indexOf("\\NoEOffs") != -1)
                s1 = s1.mid(0, s1.indexOf("\\NoEOffs"));
            else if(s1.indexOf("\\NoEoffs") != -1)
                s1 = s1.mid(0, s1.indexOf("\\NoEoffs"));

            if(s1 == "*")
            {
                auto rout_str = EditData::instance()->GetRoutData(rout_name_).code.at(row_);
                s1 = rout_str.section(" ", 1, 1);
            }

            if(s1.at(s1.size() - 1) == ',')
                s1 = s1.mid(0, s1.size() - 1);
            if(s4.at(s4.size() - 1) == ';')
                s4 = s4.mid(0, s4.size() - 1);

            if(s1.indexOf("\\NoEOffs") != -1)
                s1 = s1.mid(0, s1.indexOf("\\NoEOffs"));
            else if(s1.indexOf("\\NoEoffs") != -1)
                s1 = s1.mid(0, s1.indexOf("\\NoEoffs"));

            table_box_->AddText(0, 0, "ToJointPos");
            table_box_->AddText(0, 1, s1);
            table_box_->AddText(1, 0, "NoEOffs");
            table_box_->AddText(1, 1, "");
            table_box_->AddText(2, 0, "Speed");
            table_box_->AddText(2, 1, s2);
            table_box_->AddText(3, 0, "Zone");
            table_box_->AddText(3, 1, s3);
            table_box_->AddText(4, 0, "Tool");
            table_box_->AddText(4, 1, s4);

            auto *item1 = table_box_->item(1, 0);
            auto *item2 = table_box_->item(1, 1);
            item1->setFlags(Qt::NoItemFlags);
            item2->setFlags(Qt::NoItemFlags);
        }
    }
}

MoveOptVarWindow::MoveOptVarWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    head_label_ = new QLabel(tr("选择要使用或不使用的可选参变量。"), this);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    auto *bt_layout = new QHBoxLayout(button_widget_);
    use_button_ = new QPushButton(tr("使用"), button_widget_);
    no_use_button_ = new QPushButton(tr("不使用"), button_widget_);
    close_button_ = new QPushButton(tr("关闭"), button_widget_);
    use_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    no_use_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    use_button_->setFlat(true);
    no_use_button_->setFlat(true);
    close_button_->setFlat(true);
    bt_layout->addWidget(use_button_);
    bt_layout->addWidget(no_use_button_);
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addWidget(close_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addWidget(head_label_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(head_label_, 1);
    layout->setStretchFactor(table_box_, 7);
    layout->setStretchFactor(button_widget_, 1);

    table_box_->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("自变量") << tr("状态"));

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void MoveOptVarWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
}

void MoveOptVarWindow::ConnectInit()
{
    connect(close_button_, &QPushButton::clicked, this, &MoveOptVarWindow::CloseWindow);
}

ChangeSelectionWindow::ChangeSelectionWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips1_label_ = new QLabel(tr("当前变量:"), head_widget_);
    current_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel(tr("选择自变量值。"), head_widget_);
    tips_filter_label_ = new QLabel(tr("活动过滤器:"), head_widget_);
    filter_label_ = new QLabel(head_widget_);
    head_layout->addWidget(tips1_label_, 0, 0);
    head_layout->addWidget(current_label_, 0, 1);
    head_layout->addWidget(tips2_label_, 1, 0);
    head_layout->addWidget(tips_filter_label_, 1, 3);
    head_layout->addWidget(filter_label_, 1, 4);

    data_edit_ = new ChangeSelectText(this);

    tab_widget_ = new QTabWidget(this);
    data_box_ = new TableBox(this);
    func_box_ = new TableBox(this);
    tab_widget_->addTab(data_box_, tr("数据"));
    tab_widget_->addTab(func_box_, tr("功能"));

    button_widget_ = new QWidget(this);
    auto *bt_layout = new QHBoxLayout(button_widget_);
    filter_button_ = new ComboBoxObject(button_widget_);
    b123_button_ = new QPushButton("123...", button_widget_);
    expression_button_ = new QPushButton(tr("表达式..."), button_widget_);
    edit_button_ = new ComboBoxObject(tr("编辑"), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    filter_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    b123_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    expression_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    filter_button_->setFlat(true);
    b123_button_->setFlat(true);
    expression_button_->setFlat(true);
    edit_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    bt_layout->addWidget(filter_button_);
    bt_layout->addWidget(b123_button_);
    bt_layout->addWidget(expression_button_);
    bt_layout->addWidget(edit_button_);
    bt_layout->addWidget(define_button_);
    bt_layout->addWidget(cancel_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addWidget(head_widget_);
    layout->addWidget(data_edit_);
    layout->addWidget(tab_widget_);
    layout->addWidget(button_widget_);
    layout->setStretchFactor(head_widget_, 1);
    layout->setStretchFactor(data_edit_, 1);
    layout->setStretchFactor(tab_widget_, 5);
    layout->setStretchFactor(button_widget_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    change_data_type_ = new QAction(tr("更改数据类型"), this);
    opt_var_ = new QAction(tr("可选变元"), this);
    abc_key_ = new QAction("ABC", this);

    auto qss = "QMenu::item{padding-left:0px;}"; //去除前面的空白
    edit_button_->GetMenu()->setStyleSheet(qss);

    b123_button_->setEnabled(false);
    edit_button_->AppAction(change_data_type_);
    edit_button_->AppAction(opt_var_);
    edit_button_->AppAction(abc_key_);
    change_data_type_->setEnabled(false);
    opt_var_->setEnabled(false);
    abc_key_->setEnabled(false);

    filter_key_ = new FilterKey(this);
    filter_key_->resize(394, 275);
    filter_key_->move(245, 40);
    filter_key_->hide();
    filter_key_->setFrameShape(QFrame::Box);

    creat_var_window_ = new CreateNewVar(this);
    creat_var_window_->hide();

    insert_window_ = new InsertExpresWindow(this);
    insert_window_->hide();

    change_type_ = new ChangeDataType(this);
    change_type_->hide();

    insert_window_->title_widget_->hide();
    insert_window_->key_x_ = 245;
    insert_window_->key_y_ = 40;
    insert_window_->filter_key_->move(245, 40);

    data_edit_->SetParentPointer(this);
    filter_button_->SetFilterButton();
    head_widget_->setFrameShape(QFrame::Box);

    iscreate_ = true;
    for(int i = 0; i < 16; i++)
    {
        QString name1 = "DI" + QString::number(i + 1);
        QString name2 = "DO" + QString::number((i + 1));

        di_list_.append(name1);
        do_list_.append(name2);
    }

    for(int i = 0; i < 4; i++)
    {
        QString name1 = "AI" + QString::number(i + 1);
        QString name2 = "AO" + QString::number((i + 1));

        ai_list_.append(name1);
        ao_list_.append(name2);
    }

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    filter_key_->setAutoFillBackground(true);
    filter_key_->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void ChangeSelectionWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int width = tab_widget_->width();
    int tab_count = tab_widget_->count();
    int tab_width = width/tab_count;
    tab_widget_->setStyleSheet(QString("QTabBar::tab{width:%1px; min-height: 40px;}").arg(tab_width));
}

void ChangeSelectionWindow::Init(QString code, QString orig_code, QString mod_name, int select, bool rep)
{
    rep_ = rep;
    mod_name_ = mod_name;
    original_code_ = orig_code;
    data_edit_->setPlainText(code);
    b123_button_->setEnabled(false);
    filter_label_->clear();

    Init(select);
}

void ChangeSelectionWindow::SetScale(double scale_w, double scale_h)
{
    data_box_->SetScale(scale_w, scale_h);
    func_box_->SetScale(scale_w, scale_h);

    if(!filter_key_->isHidden())
    {
        filter_key_->resize(394*scale_w, 275*scale_h);
        filter_key_->move(245*scale_w, 40*scale_h);
    }
    filter_key_->SetScale(scale_w, scale_h);

    if(!creat_var_window_->isHidden())
    {
        creat_var_window_->resize(this->size());
        creat_var_window_->move(0, 0);
    }
    creat_var_window_->SetScale(scale_w, scale_h);

    if(!insert_window_->isHidden())
    {
        insert_window_->resize(this->size());
        insert_window_->move(0, 0);
    }
    insert_window_->SetScale(scale_w, scale_h);

    if(!change_type_->isHidden())
    {
        change_type_->resize(this->size());
        change_type_->move(0, 0);
    }
    change_type_->SetScale(scale_w, scale_h);
}

void ChangeSelectionWindow::DefineClicked()
{
    auto code = data_edit_->toPlainText();
    auto head = code.section(" ", 0, 0);

    if(head == "MoveC")
    {
        auto var1 = code.section(" ", 1, 1);
        auto var2 = code.section(" ", 2, 2);
        if(var1 == "*,")
        {
            auto s1 = original_code_.section(" ", 1, 1);

            code.replace(var1, s1);
        }
        if(var2 == "*,")
        {
            auto s2 = original_code_.section(" ", 1, 1);

            code.replace(var2, s2);
        }
    }
    else if(head == "MoveJ" || head == "MoveL")
    {
        auto var1 = code.section(" ", 1, 1);

        if(var1 == "*,")
        {
            auto s1 = original_code_.section(" ", 1, 1);

            code.replace(var1, s1);
        }
    }
    else if(head == "MoveAbsJ")
    {
        auto var1 = code.section(" ", 1, 1);

        if(var1 == "*\\NoEoffs,")
        {
            auto s1 = original_code_.section(" ", 1, 1);

            code.replace(var1, s1);
        }
    }

    if(code.at(code.size() - 1) != ';')
        code.append(";");

    emit Define(code, rep_);
}

void ChangeSelectionWindow::DataBoxItem(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    int row = data_box_->row(item);
    int column = data_box_->column(item);
    auto text = item->text();

    if(row == 0 && column == 0 && iscreate_) //新建变量
    {
        ShowCreateVarWindow(current_type_);
        return;
    }

    auto &dim = data_dim_[item];

    if(!dim.isEmpty())
    {
        int dim_num = dim.count(',');

        text.append('{');
        for(int i = 0; i < dim_num; i++)
        {
            text.append("<EXP>");
            if(i + 1 < dim_num)
                text.append(",");
        }
        text.append("}");
    }

    auto data = data_edit_->toPlainText();
    auto &pir = select_pos_[select_];
    data.replace(pir.first, pir.second, text);

    data_edit_->setPlainText(data);
    filter_label_->clear();
    Init(select_);
}

void ChangeSelectionWindow::FuncBoxItem(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;
}

void ChangeSelectionWindow::ShidFilterWindow()
{
    if(filter_key_->isHidden())
    {
        filter_key_->Init();
        filter_key_->show();

        filter_button_->LowerArrow();
    }
    {
        filter_key_->hide();

        filter_button_->UpperArrow();
    }
}

void ChangeSelectionWindow::ResetFilter()
{
    filter_label_->clear();

    UpdateTabBox();
}

void ChangeSelectionWindow::FilterText(QString text)
{
    filter_label_->setText(text);

    UpdateTabBox(text);
}

void ChangeSelectionWindow::ShowCreateVarWindow(QString type)
{
    creat_var_window_->resize(this->size());
    creat_var_window_->move(0, 0);
    creat_var_window_->show();
    creat_var_window_->Init(type, mod_name_);

    if(!insert_window_->isHidden())
    {
        insert_window_->setWindowTitle("0");
        insert_window_->hide();
    }
}

void ChangeSelectionWindow::HideCreateVarWindow()
{
    creat_var_window_->hide();

    if(insert_window_->windowTitle() == "0")
    {
        insert_window_->show();
        insert_window_->setWindowTitle("1");
    }
}

void ChangeSelectionWindow::CreateVar(QString)
{
    UpdateTabBox(filter_key_->GetFilter());
    creat_var_window_->hide();

    if(insert_window_->windowTitle() == "0")
    {
        insert_window_->show();
        insert_window_->setWindowTitle("1");
        insert_window_->CreateVared();
    }
}

void ChangeSelectionWindow::ChangeType(QString type)
{
    change_type_->hide();

    if(insert_window_->windowTitle() == "0")
    {
        insert_window_->show();
        insert_window_->setWindowTitle("1");
        insert_window_->ChangeDataType(type);
    }
}

void ChangeSelectionWindow::ShowInsertWindow()
{
    insert_window_->resize(this->size());
    insert_window_->move(0, 0);
    insert_window_->show();

    auto sor = data_edit_->textCursor();

    insert_window_->Init(sor.selectedText(), mod_name_, current_type_, true, true);
}

void ChangeSelectionWindow::ShowChangeDataType()
{
    change_type_->resize(this->size());
    change_type_->move(0, 0);
    change_type_->show();
    change_type_->Init(current_type_);

    if(!insert_window_->isHidden())
    {
        insert_window_->setWindowTitle("0");
        insert_window_->hide();
    }
}

void ChangeSelectionWindow::HideChangeDataType()
{
    change_type_->hide();

    if(insert_window_->windowTitle() == "0")
    {
        insert_window_->show();
        insert_window_->setWindowTitle("1");
    }
}

void ChangeSelectionWindow::InsertRetCode(QString code, bool)
{
    auto sor = data_edit_->textCursor();

    sor.insertText(code);

    data_edit_->setTextCursor(sor);
    Init(select_);

    insert_window_->hide();

}

void ChangeSelectionWindow::ConnectInit()
{
    connect(define_button_, &QPushButton::clicked, this, &ChangeSelectionWindow::DefineClicked);

    connect(cancel_button_, &QPushButton::clicked, this, &ChangeSelectionWindow::Cancel);

    connect(filter_button_, &QPushButton::clicked, this, &ChangeSelectionWindow::ShidFilterWindow);

    connect(filter_key_, &FilterKey::GetData, this, &ChangeSelectionWindow::FilterText);

    connect(filter_key_->reset_button_, &QPushButton::clicked, this, &ChangeSelectionWindow::ResetFilter);

    connect(creat_var_window_->cancel_button_, &QPushButton::clicked, this, &ChangeSelectionWindow::HideCreateVarWindow);

    connect(creat_var_window_, &CreateNewVar::CreateVar, this, &ChangeSelectionWindow::CreateVar);

    connect(data_box_, &TableBox::itemClicked, this, &ChangeSelectionWindow::DataBoxItem);

    connect(func_box_, &TableBox::itemClicked, this, &ChangeSelectionWindow::FuncBoxItem);

    connect(expression_button_, &QPushButton::clicked, this, &ChangeSelectionWindow::ShowInsertWindow);

    connect(insert_window_, &InsertExpresWindow::ShowChangeDataType, this, &ChangeSelectionWindow::ShowChangeDataType);

    connect(insert_window_, &InsertExpresWindow::ShowCreateNewVar, this, &ChangeSelectionWindow::ShowCreateVarWindow);

    connect(insert_window_, &InsertExpresWindow::ReturnCode, this, &ChangeSelectionWindow::InsertRetCode);

    connect(insert_window_->cancel_button_, &QPushButton::clicked, insert_window_, &InsertExpresWindow::hide);

    connect(change_type_, &ChangeDataType::ChangeType, this, &ChangeSelectionWindow::ChangeType);

    connect(change_type_->cancel_button_, &QPushButton::clicked, this, &ChangeSelectionWindow::HideChangeDataType);
}

void ChangeSelectionWindow::MoveAnalysis(int select)
{
    auto code = data_edit_->toPlainText();
    auto head = code.section(" ", 0, 0);
    auto code_str = code.mid(head.size() + 1);
    auto code_list = code_str.split(", ");

    int num = 0;
    for(auto &str : code_list)
    {
        if(str.indexOf("\\NoEOffs") != -1)
            str = str.mid(0, str.indexOf("\\NoEOffs"));
        else if(str.indexOf("\\NoEoffs") != -1)
            str = str.mid(0, str.indexOf("\\NoEoffs"));

        if(str.at(str.size() - 1) == ',')
            str = str.mid(0, str.size() - 1);
        if(str.at(str.size() - 1) == ';')
            str = str.mid(0, str.size() - 1);

        int pos = code.indexOf(str, num);
        select_pos_.append(qMakePair(pos, str.size()));
        if(num == 0)
            num = pos + str.size();
        else
            num += str.size();
    }

    if(select >= 0 && select < select_pos_.size())
    {
        int start = select_pos_.at(select).first;
        int end = select_pos_.at(select).second;
        select_ = select;
        SelectText(start, end);
    }
}

void ChangeSelectionWindow::SetAnalysis()
{
    auto code = data_edit_->toPlainText();
    auto str = code.mid(4);
    select_pos_.append(qMakePair(4, str.size() - 1));

    select_ = 0;
    int start = select_pos_.at(select_).first;
    int end = select_pos_.at(select_).second;
    SelectText(start, end);
}

void ChangeSelectionWindow::AddAnalysis(int select)
{
    auto code = data_edit_->toPlainText();
    auto str = code.mid(4);

    auto str1 = str.section(", ", 0, 0);
    auto str2 = str.section(", ", 1, 1);
    if(str2.at(str2.size() - 1) == ';')
        str2 = str2.mid(0, str2.size() - 1);

    int s1 = code.indexOf(str1);
    int s2 = code.indexOf(str2, s1 + str1.size());

    select_pos_.append(qMakePair(s1, str1.size()));
    select_pos_.append(qMakePair(s2, str2.size()));

    if(select >= 0 && select < select_pos_.size())
    {
        int start = select_pos_.at(select).first;
        int end = select_pos_.at(select).second;
        select_ = select;
        SelectText(start, end);
    }
}

void ChangeSelectionWindow::PulseDOAnalysis(int select)
{
    auto code = data_edit_->toPlainText();
    auto str = code.mid(code.indexOf(":=") + 2);

    auto str1 = str.section(", ", 0, 0);
    auto str2 = str.section(", ", 1, 1);
    if(str2.at(str2.size() - 1) == ';')
        str2 = str2.mid(0, str2.size() - 1);

    int s1 = code.indexOf(str1);
    int s2 = code.indexOf(str2, s1 + str1.size());

    select_pos_.append(qMakePair(s1, str1.size()));
    select_pos_.append(qMakePair(s2, str2.size()));

    if(select >= 0 && select < select_pos_.size())
    {
        int start = select_pos_.at(select).first;
        int end = select_pos_.at(select).second;
        select_ = select;
        SelectText(start, end);
    }
}

void ChangeSelectionWindow::SetDOAnalysis(int select)
{
    auto code = data_edit_->toPlainText();
    auto str = code.mid(6);

    auto str1 = str.section(", ", 0, 0);
    auto str2 = str.section(", ", 1, 1);
    if(str2.at(str2.size() - 1) == ';')
        str2 = str2.mid(0, str2.size() - 1);

    int s1 = code.indexOf(str1);
    int s2 = code.indexOf(str2, s1 + str1.size());

    select_pos_.append(qMakePair(s1, str1.size()));
    select_pos_.append(qMakePair(s2, str2.size()));

    if(select >= 0 && select < select_pos_.size())
    {
        int start = select_pos_.at(select).first;
        int end = select_pos_.at(select).second;
        select_ = select;
        SelectText(start, end);
    }
}

void ChangeSelectionWindow::WaitDIAnalysis(int select)
{
    auto code = data_edit_->toPlainText();
    auto str = code.mid(7);

    auto str1 = str.section(", ", 0, 0);
    auto str2 = str.section(", ", 1, 1);
    if(str2.at(str2.size() - 1) == ';')
        str2 = str2.mid(0, str2.size() - 1);

    int s1 = code.indexOf(str1);
    int s2 = code.indexOf(str2, s1 + str1.size());

    select_pos_.append(qMakePair(s1, str1.size()));
    select_pos_.append(qMakePair(s2, str2.size()));

    if(select >= 0 && select < select_pos_.size())
    {
        int start = select_pos_.at(select).first;
        int end = select_pos_.at(select).second;
        select_ = select;
        SelectText(start, end);
    }
}

void ChangeSelectionWindow::WaitDOAnalysis(int select)
{
    auto code = data_edit_->toPlainText();
    auto str = code.mid(7);

    auto str1 = str.section(", ", 0, 0);
    auto str2 = str.section(", ", 1, 1);
    if(str2.at(str2.size() - 1) == ';')
        str2 = str2.mid(0, str2.size() - 1);

    int s1 = code.indexOf(str1);
    int s2 = code.indexOf(str2, s1 + str1.size());

    select_pos_.append(qMakePair(s1, str1.size()));
    select_pos_.append(qMakePair(s2, str2.size()));

    if(select >= 0 && select < select_pos_.size())
    {
        int start = select_pos_.at(select).first;
        int end = select_pos_.at(select).second;
        select_ = select;
        SelectText(start, end);
    }
}

void ChangeSelectionWindow::WaitTimeAnalysis()
{
    auto code = data_edit_->toPlainText();
    auto str = code.mid(9);

    if(str.at(str.size() - 1) == ';')
        str = str.mid(0, str.size() - 1);

    int s1 = code.indexOf(str);

    select_pos_.append(qMakePair(s1, str.size()));

    select_ = 0;
    int start = select_pos_.at(select_).first;
    int end = select_pos_.at(select_).second;
    SelectText(start, end);
}

void ChangeSelectionWindow::UpdateTabBox(QString filter)
{
    auto code = data_edit_->toPlainText();
    auto head = code.section(" ", 0, 0);

    if(head == "MoveC")
    {
        switch(select_)
        {
        case 0:
        {
            current_type_ = "robtarget";
            UpdateTabBox(current_type_, filter);
            return;
        }
        case 1:
        {
            current_type_ = "robtarget";
            UpdateTabBox(current_type_, filter);
            return;
        }
        case 2:
        {
            current_type_ = "speeddata";
            UpdateTabBox(current_type_, filter);
            return;
        }
        case 3:
        {
            current_type_ = "zonedata";
            UpdateTabBox(current_type_, filter);
            return;
        }
        case 4:
        {
            current_type_ = "tooldata";
            UpdateTabBox(current_type_, filter);
            return;
        }
        default:
        {
            return;
        }
        }
    }
    else if(head == "MoveJ" || head == "MoveL" || head == "MoveAbsJ")
    {
        switch(select_)
        {
        case 0:
        {
            if(head == "MoveAbsJ")
                current_type_ = "jointtarget";
            else
                current_type_ = "robtarget";
            UpdateTabBox(current_type_, filter);
            return;
        }
        case 1:
        {
            current_type_ = "speeddata";
            UpdateTabBox(current_type_, filter);
            return;
        }
        case 2:
        {
            current_type_ = "zonedata";
            UpdateTabBox(current_type_, filter);
            return;
        }
        case 3:
        {
            current_type_ = "tooldata";
            UpdateTabBox(current_type_, filter);
            return;
        }
        default:
        {
            return;
        }
        }
    }
    else if(head == "Set")
    {
        current_type_ = "signaldo";
        UpdateTabBox(current_type_, filter);
    }
    else if(head == "Add")
    {
        current_type_ = "num";
        UpdateTabBox(current_type_, filter);
    }
    else if(head == "PulseDO")
    {
        if(select_ == 0)
        {
            current_type_ = "num";
        }
        else
        {
            current_type_ = "signaldo";
        }
        UpdateTabBox(current_type_, filter);
    }
    else if(head == "SetDO")
    {
        if(select_ == 0)
        {
            current_type_ = "signaldo";
        }
        else
        {
            current_type_ = "num";
        }
        UpdateTabBox(current_type_, filter);
    }
    else if(head == "WaitDI")
    {
        if(select_ == 0)
        {
            current_type_ = "signaldo";
        }
        else
        {
            current_type_ = "num";
        }
        UpdateTabBox(current_type_, filter);
    }
    else if(head == "WaitDO")
    {
        if(select_ == 0)
        {
            current_type_ = "signaldo";
        }
        else
        {
            current_type_ = "num";
        }
        UpdateTabBox(current_type_, filter);
    }
    else if(head == "WaitTime")
    {
        current_type_ = "num";
        UpdateTabBox(current_type_, filter);
    }
}

void ChangeSelectionWindow::UpdateTabBox(QString &type, QString filter)
{
    data_box_->clear();
    data_box_->SetRomn(1, 2);
    func_box_->clear();
    func_box_->SetRomn(0, 0);
    current_label_->setText(type);

    int r1 = 0, c1 = 0;

    if(type == "signaldi") //IO
    {
        for(auto &str : di_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }
    else if(type == "signaldo")
    {
        for(auto &str : do_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }
    else if(type == "signalai")
    {
        for(auto &str : ai_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }
    else if(type == "signalao")
    {
        for(auto &str : ao_list_)
        {
            data_box_->AddText(r1, c1++, str);
            if(c1 >1)
            {
                r1++;
                c1 = 0;
            }
        }
        iscreate_ = false;
        return;
    }

    auto list = VarInfo::instance()->GetTypeVar(type);

    data_box_->AddText(r1, c1++, tr("新建"));
    iscreate_ = true;

    for(auto &p : list)
    {
        if(!filter.isEmpty())
        {
            if(p.first.toUpper().indexOf(filter.toUpper()) == -1)
                continue;
        }

        if(p.second.action_scope == "local" && p.second.mod != mod_name_)
        {
            continue;
        }
        else if(p.second.action_scope != "all")
        {
            if(p.second.action_scope != "task")
            {
                if(p.second.mod != mod_name_)
                    continue;
            }
        }
        else if(p.second.action_scope != "task")
        {
            if(p.second.action_scope != "all")
            {
                if(p.second.mod != mod_name_)
                    continue;
            }
        }

        data_box_->AddText(r1, c1++, p.first);
        auto *item = data_box_->item(r1, c1 - 1);
        data_dim_.insert(item, p.second.dim);
        if(c1 > 1)
        {
            r1++;
            c1 = 0;
        }
    }
}

void ChangeSelectionWindow::DataEditClicked()
{
    auto sor = data_edit_->textCursor();
    sor.select(QTextCursor::WordUnderCursor);

    auto word = sor.selectedText();
    int pos = sor.position() - word.size();

    for(int i = 0; i < select_pos_.size(); i++)
    {
        auto &pir = select_pos_[i];

        if(pir.first <= pos && pos <= pir.first + pir.second)
        {
            select_ = i;
            SelectText(pir.first, pir.second);
            return;
        }
    }

    if(select_ != -1)
    {
        SelectText(select_pos_.at(select_).first, select_pos_.at(select_).second);
    }
    else
    {
        SelectText(select_pos_.at(0).first, select_pos_.at(select_).second);
    }
}

void ChangeSelectionWindow::Init(int select)
{
    auto code = data_edit_->toPlainText();
    auto head = code.section(" ", 0, 0);
    select_pos_.clear();
    select_ = -1;
    if(select == -1)
        select = 0;

    if(head == "MoveJ" || head == "MoveAbsJ" || head == "MoveC" || head == "MoveL")
    {
        MoveAnalysis(select);
    }
    else if(head == "Set")
    {
        SetAnalysis();
    }
    else if(head == "Add")
    {
        AddAnalysis(select);
    }
    else if(head == "PulseDO")
    {
        PulseDOAnalysis(1);
    }
    else if(head == "SetDO")
    {
        SetDOAnalysis(select);
    }
    else if(head == "WaitDI")
    {
        WaitDIAnalysis(select);
    }
    else if(head == "WaitDO")
    {
        WaitDOAnalysis(select);
    }
    else if(head == "WaitTime")
    {
        WaitTimeAnalysis();
    }
}

void ChangeSelectionWindow::SelectText(int start, int size)
{
    auto sor = data_edit_->textCursor();

    int end = start + size;

    sor.setPosition(start, QTextCursor::MoveAnchor);
    sor.setPosition(end, QTextCursor::KeepAnchor);

    data_edit_->setTextCursor(sor);

    UpdateTabBox();
}

ChangeSelectText::ChangeSelectText(QWidget *parent) : QTextBrowser (parent)
{
    this->setContextMenuPolicy(Qt::NoContextMenu);
}

void ChangeSelectText::SetParentPointer(ChangeSelectionWindow *pointer)
{
    parent_ = pointer;
}

void ChangeSelectText::mousePressEvent(QMouseEvent *event)
{
    QTextBrowser::mousePressEvent(event);

    if(!parent_.isNull())
    {
        parent_->DataEditClicked();
    }
}

void ChangeSelectText::mouseDoubleClickEvent(QMouseEvent *)
{
}

void ChangeSelectText::mouseMoveEvent(QMouseEvent *)
{
}

void ChangeSelectText::mouseReleaseEvent(QMouseEvent *)
{
}

PPtoRoutWindow::PPtoRoutWindow(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips1_label_ = new QLabel(tr("选定的例行程序:"), head_widget_);
    select__label_ = new QLabel(head_widget_);
    tips_filter_label_ = new QLabel(tr("活动过滤器:"), head_widget_);
    filter_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel(tr("从列表中选择一个例行程序。"), head_widget_);
    head_layout->addWidget(tips1_label_, 0, 0);
    head_layout->addWidget(select__label_, 0, 1);
    head_layout->addWidget(tips_filter_label_, 1, 2);
    head_layout->addWidget(filter_label_, 1, 3);
    head_layout->addWidget(tips2_label_, 2, 0);

    table_box_ = new TableBox(this);

    button_widget_ = new QWidget(this);
    auto *but_layout = new QHBoxLayout(button_widget_);
    filter_button_ = new ComboBoxObject(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    filter_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    filter_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    but_layout->addWidget(filter_button_);
    but_layout->addStretch();
    but_layout->addStretch();
    but_layout->addWidget(define_button_);
    but_layout->addWidget(cancel_button_);
    but_layout->setContentsMargins(0, 0, 0, 0);
    but_layout->setSpacing(0);

    layout->addWidget(head_widget_);
    layout->addWidget(table_box_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(head_widget_, 2);
    layout->setStretchFactor(table_box_, 5);
    layout->setStretchFactor(button_widget_, 1);

    filter_button_->SetFilterButton();
    filter_button_->hide();

    table_box_->horizontalHeader()->show();
    table_box_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_box_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_box_->SetRomn(1, 3);
    table_box_->setHorizontalHeaderLabels(QStringList() << tr("名称") << tr("类型") << tr("模块"));


    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void PPtoRoutWindow::Init(QString select, QString filter)
{
    select__label_->setText(select);
    filter_label_->clear();
    table_box_->clearContents();
    table_box_->SetRomn(0, 3);

    auto rout_list = EditData::instance()->GetRoutName();
    int i = 0;
    for(auto &rout : rout_list)
    {
        if(!filter.isEmpty())
        {
            if(rout.toUpper().indexOf(filter.toUpper()) == -1)
                continue;
        }

        table_box_->AddText(i, 0, rout);
        table_box_->AddText(i, 1, EditData::instance()->GetRoutType(rout));
        table_box_->AddText(i, 2, EditData::instance()->GetRoutMod(rout));
        i++;
    }
}

void PPtoRoutWindow::SetScale(double scale_w, double scale_h)
{
    table_box_->SetScale(scale_w, scale_h);
}

void PPtoRoutWindow::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;

    int row = table_box_->row(item);
    item = table_box_->item(row, 0);
    select__label_->setText(item->text());
}

void PPtoRoutWindow::DefineClicked()
{
    auto rout_name = select__label_->text();

    auto pair = EditData::instance()->GetRoutPoint(rout_name, 0);

    emit Define(pair.first);
}

void PPtoRoutWindow::ConnectInit()
{
    connect(define_button_, &QPushButton::clicked, this, &PPtoRoutWindow::DefineClicked);

    connect(cancel_button_, &QPushButton::clicked, this, &PPtoRoutWindow::Cancel);

    connect(table_box_, &TableBox::itemClicked, this, &PPtoRoutWindow::ItemClicked);
}
