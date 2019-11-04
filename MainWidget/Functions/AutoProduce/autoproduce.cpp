#include "autoproduce.h"
#include "teachstate.h"
#include "RobotClient/robotClient.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTextBlock>
#include <QTimer>

using namespace rclib;

AutoProduce::AutoProduce(QWidget *parent, int type) : WindowObject(type, parent)
{
    loader_button_ = new QPushButton(tr("加载程序..."), button_widget_);
    transfer_button_ = new QPushButton(tr("PP 移至 Main"), button_widget_);
    debug_button_ = new ComboBoxObject(tr("调试"), button_widget_);

    loader_button_->setFlat(true);
    transfer_button_->setFlat(true);
    debug_button_->setFlat(true);

    QHBoxLayout *main_layout = new QHBoxLayout(main_widget_);

    brosw_widget_ = new QWidget(main_widget_);
    auto *bro_layout = new QHBoxLayout(brosw_widget_);
    code_row_ = new CodeRow(brosw_widget_);
    code_edit_ = new AutoCodeEdit(brosw_widget_);
    bro_layout->addWidget(code_row_);
    bro_layout->addWidget(code_edit_);
    bro_layout->setContentsMargins(0, 0, 0, 0);
    bro_layout->setSpacing(0);
    bro_layout->setStretchFactor(code_row_, 1);
    bro_layout->setStretchFactor(code_edit_, 20);

    debug_widget_ = new DebugWidget(main_widget_);

    main_layout->addWidget(brosw_widget_);
    main_layout->addWidget(debug_widget_);
    main_layout->setStretchFactor(brosw_widget_, 5);
    main_layout->setStretchFactor(debug_widget_, 1);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);

    update_pointer_ = new QTimer(this);
    file_manger_ = new TaskFileManger(this);

    timer_start_ = false;
    isshow_ = false;
    code_edit_->SetCodeRow(code_row_);
    file_manger_->hide();

    debug_widget_->hide();

    ConnectInit();
}

void AutoProduce::resizeEvent(QResizeEvent *)
{
    WindowObject::resizeEvent(nullptr);

    loader_button_->resize(128*ScaleW(), button_widget_->height());
    transfer_button_->resize(loader_button_->size());
    debug_button_->resize(transfer_button_->size());

    loader_button_->move(0, 0);
    transfer_button_->move(loader_button_->x() + loader_button_->width(), loader_button_->y());
    debug_button_->move(button_widget_->width() - debug_button_->width(), transfer_button_->y());
}

void AutoProduce::showEvent(QShowEvent *)
{
    WindowObject::showEvent(nullptr);
    if(timer_start_)
    {
        if(!update_pointer_->isActive())
            update_pointer_->start(100);
    }
    isshow_ = true;

    if(!TeachState::instance()->GetRunState() && !code_edit_->toPlainText().isEmpty()) //如果当前程序并不在执行并且当前已经显示了代码则将所显示的代码更新为程序编辑器中的代码编辑框中所显示的代码
    {
        code_edit_->setText(progedit_code_edit_->toHtml());
    }
}

void AutoProduce::hideEvent(QHideEvent *)
{
    WindowObject::hideEvent(nullptr);
    if(timer_start_)
    {
        if(update_pointer_->isActive())
            update_pointer_->stop();
        else
            timer_start_ = false;
    }
    isshow_ = false;
}

void AutoProduce::SetCodeEdit(CodeEdit *ct)
{
    progedit_code_edit_ = ct;
}

void AutoProduce::SetScale(double scale_w, double scale_h)
{
    WindowObject::SetScale(scale_w, scale_h);

    resizeEvent(nullptr);

    if(!file_manger_->isHidden())
    {
        file_manger_->resize(this->size());
        file_manger_->move(0, 0);
    }
    file_manger_->SetScale(scale_w, scale_h);
}

void AutoProduce::StartRun()
{
    if(!update_pointer_->isActive())
    {
        if(!this->isHidden() && isshow_)
            update_pointer_->start(100);
        timer_start_ = true;
    }
}

void AutoProduce::UpdatePointer()
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
    auto sor = code_row_->textCursor();
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


    code_row_->setTextCursor(sor);
    int val = code_row_->verticalScrollBar()->value();
    code_edit_->verticalScrollBar()->setValue(val);
}

void AutoProduce::PPtoMain()
{
    if(TeachState::instance()->GetRunState()) //如果正在运行则更新程序指针
    {
        ProgramPointer point;
        point.nFunction = EditData::instance()->GetRoutMainPoint();
        point.nSentence = 0;

        RobotClient::initance()->setPointer(point);
    }
    else //否则就只显示代码
    {
        code_edit_->setText(progedit_code_edit_->toHtml());
    }
}

void AutoProduce::LoadTask()
{
    QString mes = tr("任何未保存的程序更改将会丢失，确定要继续？");
    auto messg = QMessageBox::question(this, tr("加载程序"), mes, QMessageBox::Yes | QMessageBox::No);
    if(messg == QMessageBox::Yes)
    {
        file_manger_->resize(this->size());
        file_manger_->move(0, 0);
        file_manger_->show();
        file_manger_->Init(false);
    }
}

void AutoProduce::FileLoadTask()
{
    file_manger_->hide();

    emit LoadTaSig();
}

void AutoProduce::ConnectInit()
{
    connect(debug_widget_, &DebugWidget::HideWindow,
            [=]()
    {
        debug_button_->UpperArrow();
    });

    connect(debug_widget_, &DebugWidget::ShowWindow,
            [=]()
    {
        debug_button_->LowerArrow();
    });

    connect(debug_button_, &ComboBoxObject::clicked,
            [=]()
    {
        if(debug_widget_->isHidden())
        {
            debug_widget_->show();
        }
        else {
            debug_widget_->hide();
        }
    });

    connect(update_pointer_, &QTimer::timeout, this, &AutoProduce::UpdatePointer);

    connect(transfer_button_, &QPushButton::clicked, this, &AutoProduce::PPtoMain);

    connect(loader_button_, &QPushButton::clicked, this, &AutoProduce::LoadTask);

    connect(file_manger_, &TaskFileManger::LoadTask, this, &AutoProduce::FileLoadTask);

    connect(file_manger_, &TaskFileManger::CancelButton, file_manger_, &TaskFileManger::hide);

    connect(debug_widget_->edit_fun_, &QPushButton::clicked, this, &AutoProduce::ShowEdit);
}

DebugWidget::DebugWidget(QWidget *parent) : QFrame (parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    modify_pos_ = new QPushButton(tr("修改位置"), this);
    show_act_point_ = new QPushButton(tr("显示动作指针"), this);
    show_fun_point_ = new QPushButton(tr("显示程序指针"), this);
    edit_fun_ = new QPushButton(tr("编辑程序"), this);
    modify_pos_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    show_act_point_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    show_fun_point_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_fun_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->addWidget(modify_pos_);
    layout->addWidget(show_act_point_);
    layout->addWidget(show_fun_point_);
    layout->addWidget(edit_fun_);
    layout->addStretch();
    layout->addStretch();
    layout->addStretch();
    layout->addStretch();
    layout->addStretch();

    this->setLayout(layout);

    this->setFrameShape(QFrame::Box);

    modify_pos_->setEnabled(false);
    show_act_point_->setEnabled(false);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 250, 250));
    this->setAutoFillBackground(true);
    this->setPalette(p);
}

void DebugWidget::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void DebugWidget::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

AutoCodeEdit::AutoCodeEdit(QWidget *parent) : QTextBrowser (parent)
{
    tbsbar_ = new TBScrollBar(this);

    this->setVerticalScrollBar(tbsbar_);
    this->horizontalScrollBar()->hide();
    this->setLineWrapMode(QTextBrowser::NoWrap);
    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setStyleSheet("QScrollBar{width: 8px; background: rgba(126, 192, 238, 40%)}");

    auto font = this->font();
    font.setPointSize(font.pointSize() + 5);
    this->setFont(font);

    code_row_ = nullptr;

    QPalette p(this->palette());
    p.setColor(QPalette::Text, QColor(72, 128, 0)); //将文字颜色改为绿色
    this->setPalette(p);
}

void AutoCodeEdit::SetCodeRow(CodeRow *row)
{
    code_row_ = row;

    auto font(this->font());
    code_row_->SetFont(font);
}

void AutoCodeEdit::SetData(QString mod_name)
{
    this->setText(EditData::instance()->ExportMod(mod_name));

    auto &md = EditData::instance()->GetModData(mod_name);
    row_main_ = md.mod_code.size() + 1;

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

QString AutoCodeEdit::GetMod()
{
    return mod_name_;
}

int AutoCodeEdit::Convert(QString rout_name, int row)
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

void AutoCodeEdit::mousePressEvent(QMouseEvent *)
{
}

void AutoCodeEdit::mouseDoubleClickEvent(QMouseEvent *)
{
}

void AutoCodeEdit::mouseMoveEvent(QMouseEvent *)
{
}

void AutoCodeEdit::mouseReleaseEvent(QMouseEvent *)
{
}
