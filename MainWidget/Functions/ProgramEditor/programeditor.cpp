#include "programeditor.h"
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

ProgramEditor::ProgramEditor(QWidget *parent, int type) : WindowObject(type, parent)
{
    Init(); //为保证分析模块时变量能够正常添加，此函数必须最先执行

    button_widget_->hide();


    stack_widget_ = new StackedWidObject(this);

    edit_main_ = new ProgEditMain(this);
    task_order_ = new TaskOrder(this);
    modular_ = new Modular(this);
    routine_ = new Routine(this);

    stack_widget_->SetMainWidget(edit_main_);
    stack_widget_->AddWidget(task_order_, "task_order");
    stack_widget_->AddWidget(modular_, "modular");
    stack_widget_->AddWidget(routine_, "routine");

    insert_expres_ = new InsertExpresWindow(this);
    change_datatype_ = new ChangeDataType(this);
    create_newvar_ = new CreateNewVar(this);
    add_func_ = new AddFunc(this);

    stack_widget_->AddWidget(insert_expres_, "insert_expres");
    stack_widget_->AddWidget(change_datatype_, "change_datatype");
    stack_widget_->AddWidget(create_newvar_, "create_newvar");
    stack_widget_->AddWidget(add_func_, "add_func");

    ConnectInit();

    task_order_->ReadTask();
}

void ProgramEditor::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);    

    if(stack_widget_->GetName() != "main")
    {
        main_widget_->resize(this->size());
        main_widget_->move(0, 0);

        stack_widget_->resize(main_widget_->size());
        stack_widget_->move(main_widget_->pos());
        return;
    }
    else
    {
        stack_widget_->resize(this->width(), this->height() - this->GetIcoLabel()->height() - this->GetUnderLabel()->height());
        stack_widget_->move(main_widget_->pos());
    }
}

void ProgramEditor::ConnectInit()
{
    connect(edit_main_->task_button_, &QPushButton::clicked,
            [=]()
    {
        HideWidget();
        stack_widget_->Change("task_order");
        resizeEvent(nullptr);
    });

    connect(edit_main_->module_button_, &QPushButton::clicked,
            [=]()
    {
        HideWidget();
        modular_->Init();
        stack_widget_->Change("modular");
        resizeEvent(nullptr);
    });

    connect(edit_main_->order_button_, &QPushButton::clicked,
            [=]()
    {
        HideWidget();
        stack_widget_->Change("routine");
        resizeEvent(nullptr);
    });

    connect(task_order_->show_button_, &QPushButton::clicked,
            [=]()
    {
        HideWidget();
        stack_widget_->Change("modular");
        resizeEvent(nullptr);
    });

    connect(task_order_, &TaskOrder::CreateSig,
            [=]()
    {
        auto mod = EditData::instance()->GetRoutMod("main");

        if(mod.isEmpty())
            return;

        modular_->ReadMod();
        routine_->ReadRoutine(mod);
        edit_main_->code_edit_->SetData(mod);
    });

    connect(task_order_, &TaskOrder::DeleteSig,
            [=]()
    {
        EditData::instance()->AnalysisData();

        modular_->ReadMod();
        edit_main_->code_edit_->clear();
        routine_->table_box_->clearContents();
        routine_->table_box_->SetRomn(0, 3);
    });

    connect(task_order_, &TaskOrder::LoadTaSig,
            [=]()
    {
        auto mod = EditData::instance()->GetRoutMod("main");

        if(mod.isEmpty())
            return;

        modular_->ReadMod();
        routine_->ReadRoutine(mod);
        edit_main_->code_edit_->SetData(mod);

        this->ShowWidget();
    });

    connect(insert_expres_, &InsertExpresWindow::ShowChangeDataType,
            [=]()
    {
        HideWidget();
        change_datatype_->Init("num");
        stack_widget_->Change("change_datatype");
        resizeEvent(nullptr);
    });

    connect(change_datatype_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        HideWidget();
        stack_widget_->Change("insert_expres");
        resizeEvent(nullptr);
    });

    connect(change_datatype_, &ChangeDataType::ChangeType,
            [=](QString type)
    {
        HideWidget();
        insert_expres_->ChangeDataType(type);
        stack_widget_->Change("insert_expres");
        resizeEvent(nullptr);
    });

    connect(create_newvar_, &CreateNewVar::CreateVar,
            [=]()
    {
        HideWidget();
        insert_expres_->CreateVared();
        stack_widget_->Change("insert_expres");
        resizeEvent(nullptr);
    });

    connect(insert_expres_, &InsertExpresWindow::ShowCreateNewVar,
            [=](QString type)
    {
        HideWidget();
        QString mod_name = edit_main_->code_edit_->GetMod();
        create_newvar_->Init(type, mod_name);
        stack_widget_->Change("create_newvar");
        resizeEvent(nullptr);
    });

    connect(insert_expres_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        ShowWidget();
        stack_widget_->Change();
    });

    connect(create_newvar_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        HideWidget();
        stack_widget_->Change("insert_expres");
        resizeEvent(nullptr);
    });

    connect(modular_, &Modular::RevMod,
            [=](QString mod_name)
    {
        if(edit_main_->code_edit_->GetMod() == mod_name)
        {
            edit_main_->code_edit_->clear();
        }
    });

    connect(routine_, &Routine::RevMod,
            [=](QString mod_name)
    {
        if(edit_main_->code_edit_->GetMod() == mod_name)
        {
            edit_main_->code_edit_->SetData(mod_name);
            routine_->ReadRoutine(mod_name);
        }
    });

    connect(insert_expres_, &InsertExpresWindow::ReturnCode,
            [=](QString code, bool rep)
    {
        edit_main_->code_edit_->RetInsertExpres(code, rep);

        ShowWidget();
        stack_widget_->Change();
    });

    connect(add_func_, &AddFunc::CancelClicked,
            [=]()
    {
        ShowWidget();
        stack_widget_->Change();
    });

    connect(add_func_, &AddFunc::AddFuncClicked,
            [=](QString code)
    {
        edit_main_->code_edit_->RetAddFunc(code);

        ShowWidget();
        stack_widget_->Change();
    });

    connect(modular_->table_box_, &TableBox::itemDoubleClicked, this, &ProgramEditor::ShowRoutine);

    connect(task_order_->open_button_, &QPushButton::clicked, this, &ProgramEditor::ShowWidget);

    connect(task_order_->table_box_, &TableBox::itemDoubleClicked, this, &ProgramEditor::ShowWidget);

    connect(modular_->retreat_button_, &QPushButton::clicked, this, &ProgramEditor::ShowWidget);

    connect(modular_->show_button_, &QPushButton::clicked, this, &ProgramEditor::ReadRoutine);

    connect(routine_->retreat_button_, &QPushButton::clicked, this, &ProgramEditor::ShowWidget);

    connect(routine_->show_button_, &QPushButton::clicked, this, &ProgramEditor::ChangeRoutine);

    connect(edit_main_->code_edit_, &CodeEdit::ShowInsertExpres, this, &ProgramEditor::ShowInsertExpres);

    connect(edit_main_->code_edit_, &CodeEdit::ShowAddFunc, this, &ProgramEditor::ShowAddFunc);

    connect(create_newvar_, &CreateNewVar::CreateVar, edit_main_->code_edit_, &CodeEdit::CreateVar);
}

void ProgramEditor::ReadRoutine()
{
    auto *curr_item = modular_->table_box_->currentItem();
    if(curr_item == nullptr)
    {
        return;
    }
    auto *name_item = modular_->table_box_->item(modular_->table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        return;
    }

    routine_->ReadRoutine(name_item->text()); //为例行程序添加数据
    edit_main_->code_edit_->SetData(name_item->text());

    ShowWidget();
}

void ProgramEditor::ChangeRoutine()
{
    auto *curr_item = routine_->table_box_->currentItem();
    if(curr_item == nullptr)
    {
        return;
    }
    auto *name_item = routine_->table_box_->item(routine_->table_box_->row(curr_item), 0);
    if(name_item == nullptr)
    {
        return;
    }

    QString code_name = name_item->text();
    code_name = code_name.mid(0, code_name.indexOf("("));

    QString code = EditData::instance()->GetRoutCodeTree(code_name);

    int row = edit_main_->code_edit_->Convert(code);

    edit_main_->code_edit_->ChangeShow(row);

    ShowWidget();
    stack_widget_->Change();
}

void ProgramEditor::HideWidget()
{
    this->GetIcoLabel()->hide();
    this->GetTitleLabel()->hide();
    this->GetUnderLabel()->hide();
}

void ProgramEditor::ShowWidget()
{
    this->GetIcoLabel()->show();
    this->GetTitleLabel()->show();
    this->GetUnderLabel()->show();

    stack_widget_->Change();
    resizeEvent(nullptr);
}

void ProgramEditor::ShowRoutine()
{
}

void ProgramEditor::ShowInsertExpres(QString data, QString type, bool lerg, bool rep)
{
    insert_expres_->Init(data, edit_main_->code_edit_->GetMod(), type, lerg, rep);

    HideWidget();
    stack_widget_->Change("insert_expres");
    resizeEvent(nullptr);
}

void ProgramEditor::ShowAddFunc()
{
    add_func_->Init();

    HideWidget();
    stack_widget_->Change("add_func");
    resizeEvent(nullptr);
}

void ProgramEditor::Init()
{
    DataType::instance(); //对数据类型进行初始化
    QSharedPointer<VarInfo> var_shared = VarInfo::instance(); //获取变量类
    QSharedPointer<EditData> edit_data = EditData::instance(); //获取数据类

    connect(edit_data.data(), &EditData::ModRem, var_shared.data(), &VarInfo::ModRename);
    connect(edit_data.data(), &EditData::ModDel, var_shared.data(), &VarInfo::ModDel);
    connect(edit_data.data(), &EditData::ClearMod, var_shared.data(), &VarInfo::ClearMod);
    connect(edit_data.data(), &EditData::RoutMove, var_shared.data(), &VarInfo::RoutMove);
    connect(edit_data.data(), &EditData::RoutDel, var_shared.data(), &VarInfo::RoutDel);
    connect(edit_data.data(), &EditData::RoutRename, var_shared.data(), &VarInfo::RoutRename);
}

void ProgramEditor::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);
    resizeEvent(nullptr);

    edit_main_->SetScale(scale_w, scale_h);
    task_order_->SetScale(scale_w, scale_h);
    modular_->SetScale(scale_w, scale_h);
    routine_->SetScale(scale_w, scale_h);
    insert_expres_->SetScale(scale_w, scale_h);
    create_newvar_->SetScale(scale_w, scale_h);
    change_datatype_->SetScale(scale_w, scale_h);
    add_func_->SetScale(scale_w, scale_h);
}
