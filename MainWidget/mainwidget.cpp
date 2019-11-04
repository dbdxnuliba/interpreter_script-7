#include "mainwidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>

MainWidget::MainWidget(QWidget *parent) : QFrame(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    upper_widget_ = new UpperWidget(this);
    show_widgets_ = new ShowWidgets(this);
    lower_widget_ = new LowerWidget(this);

    layout->addWidget(upper_widget_, 0, 0, 1, 3);
    layout->addWidget(show_widgets_, 1, 0, 8, 3);
    layout->addWidget(lower_widget_, 9, 0, 1, 3);
    layout->setContentsMargins(0, 0, 0, 0); //取消控件与控件之间的间隔
    layout->setSpacing(0); //取消控件与控件之间的间隔

    this->setLayout(layout);
    this->setFrameShape(QFrame::Box);
    scale_w_ = 1;
    scale_h_ = 1;

    WinFunInit();
    ConnectInit();

    upper_widget_->close_button_->hide(); //隐藏关闭按钮
    lower_widget_->quick_button_->tips_action_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/lower_week1_3.png)");
    lower_widget_->quick_button_->tips_increment_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/increment_nullptr.png)");
}

void MainWidget::WinFunInit()
{
    win_all_tasks_ = new AllTasks(this);
    win_all_tasks_->hide();

    win_event_log_ = new EventLog(this);
    win_event_log_->hide();

    win_func_menu_ = new FunctionsMenu(this);
    win_func_menu_->hide();

    hotedit_ = new HotEdit(this);
    hotedit_->setWindowTitle("hotedit");
    hotedit_->hide();

    file_manager_ = new FileManager(this);
    file_manager_->setWindowTitle("file_manager");
    file_manager_->hide();

    input_out_ = new IntOut(this);
    input_out_->setWindowTitle("input_out");
    input_out_->hide();

    backup_recovery_ = new BackupRecovery(this);
    backup_recovery_->setWindowTitle("backup_recovery");
    backup_recovery_->hide();

    calib_ = new Calibration(this);
    calib_->setWindowTitle("calib");
    calib_->hide();

    auto_produce_ = new AutoProduce(this);
    auto_produce_->setWindowTitle("auto_produce");
    auto_produce_->hide();

    system_info_ = new SystemInfo(this);
    system_info_->setWindowTitle("system_info");
    system_info_->hide();

    program_data_ = new ProgramData(this);
    program_data_->setWindowTitle("program_data");
    program_data_->hide();

    control_panel_ = new ControlPanel(this);
    control_panel_->setWindowTitle("control_panel");
    control_panel_->hide();

    manual_control_ = new ManualControl(this);
    manual_control_->setWindowTitle("manual_control");
    manual_control_->hide();

    program_edit_ = new ProgramEditor(this);
    program_edit_->setWindowTitle("program_edit");
    program_edit_->hide();
    auto_produce_->SetCodeEdit(program_edit_->edit_main_->code_edit_);

    quick_window_ = new QuickWidget(this);
    quick_window_->hide();

    task_widget_ = new TaskQuickWidget(this);
    task_widget_->hide();

    speed_widget_ = new SpeedWidget(this);
    speed_widget_->hide();

    step_mod_widget_ = new StepModWidget(this);
    step_mod_widget_->hide();

    run_mod_widget_ = new RunModWidget(this);
    run_mod_widget_->hide();

    increment_widget_ = new IncrementWidget(this);
    increment_widget_->hide();

    unit_quick_widget_ = new UnitQuickWidget(this);
    unit_quick_widget_->hide();
}

void MainWidget::ConnectInit()
{
    connect(this, &MainWidget::SendScale, upper_widget_, &UpperWidget::SetScale);
    connect(this, &MainWidget::SendScale, lower_widget_, &LowerWidget::SetScale);
    connect(this, &MainWidget::SendScale, win_all_tasks_, &AllTasks::SetScale);
    connect(this, &MainWidget::SendScale, win_event_log_, &EventLog::SetScale);
    connect(this, &MainWidget::SendScale, win_func_menu_, &FunctionsMenu::SetScale);
    connect(this, &MainWidget::SendScale, file_manager_, &FileManager::SetScale);
    connect(this, &MainWidget::SendScale, input_out_, &IntOut::SetScale);
    connect(this, &MainWidget::SendScale, backup_recovery_, &BackupRecovery::SetScale);
    connect(this, &MainWidget::SendScale, calib_, &Calibration::SetScale);
    connect(this, &MainWidget::SendScale, auto_produce_, &AutoProduce::SetScale);
    connect(this, &MainWidget::SendScale, system_info_, &SystemInfo::SetScale);
    connect(this, &MainWidget::SendScale, program_data_, &ProgramData::SetScale);
    connect(this, &MainWidget::SendScale, control_panel_, &ControlPanel::SetScale);
    connect(this, &MainWidget::SendScale, manual_control_, &ManualControl::SetScale);
    connect(this, &MainWidget::SendScale, program_edit_, &ProgramEditor::SetScale);
    connect(this, &MainWidget::SendScale, unit_quick_widget_, &UnitQuickWidget::SetScale);
    connect(win_func_menu_->move_zero_, &IcoButton::clicked,
            [=]()
    {
        rclib::RobotClient::initance()->moveToZero();
    });

    connect(upper_widget_->task_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0))); //获取窗口相对于屏幕的坐标

        if(win_all_tasks_->isHidden())
        {
            win_all_tasks_->resize(show_widgets_->size());
            win_all_tasks_->move(p.x(), p.y() + upper_widget_->height());
            win_all_tasks_->show();
        }
        else
        {
            win_all_tasks_->hide();
        }
    });

    connect(upper_widget_->menu_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        if(win_func_menu_->isHidden())
        {
            win_func_menu_->resize(upper_widget_->menu_button_->width() + upper_widget_->task_button_->width() + upper_widget_->state_button_->width() + 10*scale_w_, show_widgets_->height());
            win_func_menu_->move(p.x(), p.y() + upper_widget_->height());
            win_func_menu_->show();
        }
        else
        {
            win_func_menu_->hide();
        }
    });

    connect(win_func_menu_, &FunctionsMenu::ShowWindow,
            [=]()
    {
        upper_widget_->menu_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/menu_button_do.png)");
    });

    connect(win_func_menu_, &FunctionsMenu::HideWindow,
            [=]()
    {
        upper_widget_->menu_button_->setStyleSheet("border-image: url(:/image/Image/MainWidget/menu_button.png)");
    });

    connect(hotedit_, &HotEdit::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->hotedit_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(hotedit_, &HotEdit::HideWindow,
            [=]()
    {
        lower_widget_->save_button->hotedit_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(file_manager_, &FileManager::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->file_manager_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(file_manager_, &FileManager::HideWindow,
            [=]()
    {
        lower_widget_->save_button->file_manager_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(input_out_, &IntOut::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->int_out_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(input_out_, &IntOut::HideWindow,
            [=]()
    {
        lower_widget_->save_button->int_out_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(backup_recovery_, &BackupRecovery::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->backup_recovery_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(backup_recovery_, &BackupRecovery::HideWindow,
            [=]()
    {
        lower_widget_->save_button->backup_recovery_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(calib_, &Calibration::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->calibration_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(calib_, &Calibration::HideWindow,
            [=]()
    {
        lower_widget_->save_button->calibration_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(auto_produce_, &AutoProduce::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->auto_produce_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(auto_produce_, &AutoProduce::HideWindow,
            [=]()
    {
        lower_widget_->save_button->auto_produce_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(system_info_, &SystemInfo::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->system_info_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(system_info_, &SystemInfo::HideWindow,
            [=]()
    {
        lower_widget_->save_button->system_info_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(program_data_, &ProgramData::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->program_data_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(program_data_, &ProgramData::HideWindow,
            [=]()
    {
        lower_widget_->save_button->program_data_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(program_edit_, &ProgramEditor::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->program_Editor_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(program_edit_, &ProgramEditor::HideWindow,
            [=]()
    {
        lower_widget_->save_button->program_Editor_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(control_panel_, &ControlPanel::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->control_panel_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(control_panel_, &ControlPanel::HideWindow,
            [=]()
    {
        lower_widget_->save_button->control_panel_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(manual_control_, &ManualControl::ShowWindow,
            [=]()
    {
        lower_widget_->save_button->manual_control_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(manual_control_, &ManualControl::HideWindow,
            [=]()
    {
        lower_widget_->save_button->manual_control_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(quick_window_, &QuickWidget::ShowWindow,
            [=]()
    {
        lower_widget_->quick_button_->setStyleSheet("background-color: rgb(79, 79, 79)");
    });

    connect(quick_window_, &QuickWidget::HideWindow,
            [=]()
    {
        lower_widget_->quick_button_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(&quick_window_->task_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        if(task_widget_->isHidden())
        {
            task_widget_->resize(250*scale_w_, 384*scale_h_);
            task_widget_->move(p.x() + this->width() - quick_window_->width() - task_widget_->width(), p.y() + 53*scale_h_);
            task_widget_->show();
        }
        else
        {
            task_widget_->hide();
        }
    });

    connect(&quick_window_->speed_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        if(speed_widget_->isHidden())
        {
            speed_widget_->resize(250*scale_w_, 384*scale_h_);
            speed_widget_->move(p.x() + this->width() - quick_window_->width() - speed_widget_->width(), p.y() + 53*scale_h_);
            speed_widget_->show();
        }
        else
        {
            speed_widget_->hide();
        }
    });

    connect(&quick_window_->step_mod_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        if(step_mod_widget_->isHidden())
        {
            step_mod_widget_->resize(250*scale_w_, 384*scale_h_);
            step_mod_widget_->move(p.x() + this->width() - quick_window_->width() - step_mod_widget_->width(), p.y() + 53*scale_h_);
            step_mod_widget_->show();
        }
        else
        {
            step_mod_widget_->hide();
        }
    });

    connect(step_mod_widget_->join_button_, &QPushButton::clicked,
            [=]()
    {
        quick_window_->step_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_join.png)");
    });

    connect(step_mod_widget_->out_button_, &QPushButton::clicked,
            [=]()
    {
        quick_window_->step_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_out.png)");
    });

    connect(step_mod_widget_->skip_button_, &QPushButton::clicked,
            [=]()
    {
        quick_window_->step_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_skip.png)");
    });

    connect(step_mod_widget_->lower_button_, &QPushButton::clicked,
            [=]()
    {
        quick_window_->step_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/step_mod_lower.png)");
    });

    connect(&quick_window_->run_mod_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        if(run_mod_widget_->isHidden())
        {
            run_mod_widget_->resize(250*scale_w_, 384*scale_h_);
            run_mod_widget_->move(p.x() + this->width() - quick_window_->width() - run_mod_widget_->width(), p.y() + 53*scale_h_);
            run_mod_widget_->show();
        }
        else
        {
            run_mod_widget_->hide();
        }
    });

    connect(run_mod_widget_->one_button_, &QPushButton::clicked,
            [=]()
    {
        quick_window_->run_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/one_run_mod_button.png)");
    });

    connect(run_mod_widget_->series_button_, &QPushButton::clicked,
            [=]()
    {
        quick_window_->run_mod_button_.setStyleSheet("border-image: url(:/image/Image/MainWidget/series_run_mod_button.png)");
    });

    connect(&quick_window_->increment_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        if(increment_widget_->isHidden())
        {
            increment_widget_->resize(250*scale_w_, 384*scale_h_);
            increment_widget_->move(p.x() + this->width() - quick_window_->width() - increment_widget_->width(), p.y() + 53*scale_h_);
            increment_widget_->show();
        }
        else
        {
            increment_widget_->hide();
        }
    });

    connect(increment_widget_, &IncrementWidget::ShowWidgetSize,
            [=](int width, int height)
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        increment_widget_->resize(width*scale_w_, height*scale_h_);
        increment_widget_->move(p.x() + this->width() - quick_window_->width() - increment_widget_->width(), p.y() + 53*scale_h_);
    });

    connect(&quick_window_->unit_button_, &QPushButton::clicked,
            [=]()
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        if(unit_quick_widget_->isHidden())
        {
            unit_quick_widget_->resize(250*scale_w_, 384*scale_h_);
            unit_quick_widget_->move(p.x() + this->width() - quick_window_->width() - unit_quick_widget_->width(), p.y() + 53*scale_h_);
            unit_quick_widget_->show();
        }
        else
        {
            unit_quick_widget_->hide();
        }
    });

    connect(unit_quick_widget_, &UnitQuickWidget::ModSize,
            [=](int width, int height)
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        unit_quick_widget_->resize(width*scale_w_, height*scale_h_);
        unit_quick_widget_->move(p.x() + this->width() - quick_window_->width() - unit_quick_widget_->width(), p.y() + 53*scale_h_);
        unit_quick_widget_->show();
    });

    connect(win_event_log_, &EventLog::ShowWindow,
            [=]()
    {
        upper_widget_->state_button_->setStyleSheet("background-color: rgb(192, 192, 192)");
    });

    connect(win_event_log_, &EventLog::HideWindow,
            [=]()
    {
        upper_widget_->state_button_->setStyleSheet("background-color: rgb(255, 255, 255)");
    });

    connect(auto_produce_, &AutoProduce::ShowEdit,
            [=]()
    {
        win_func_menu_->program_Editor_->click();
    });

    SaveButton *button = lower_widget_->save_button;
    connect(button->hotedit_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(hotedit_);});
    connect(button->int_out_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(input_out_);});
    connect(button->manual_control_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(manual_control_);});
    connect(button->auto_produce_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(auto_produce_);});
    connect(button->program_Editor_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(program_edit_);});
    connect(button->program_data_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(program_data_);});
    connect(button->backup_recovery_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(backup_recovery_);});
    connect(button->calibration_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(calib_);});
    connect(button->control_panel_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(control_panel_);});
    connect(button->file_manager_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(file_manager_);});
    connect(button->system_info_, &QPushButton::clicked, [=](){show_widgets_->InsertWidget(system_info_);});

    connect(upper_widget_->state_button_, &StateButton::clicked, this, &MainWidget::ShowEventLog);
    connect(win_func_menu_->hotedit_, &IcoButton::clicked, this, &MainWidget::ShowHotEdit);
    connect(win_func_menu_->event_log_, &IcoButton::clicked, this, &MainWidget::ShowEventLog);
    connect(win_func_menu_->file_manager_, &IcoButton::clicked, this, &MainWidget::ShowFileManager);
    connect(win_func_menu_->int_out_, &IcoButton::clicked, this, &MainWidget::ShowIntOut);
    connect(win_func_menu_->backup_recovery_, &IcoButton::clicked, this, &MainWidget::ShowBackReco);
    connect(win_func_menu_->calibration_, &IcoButton::clicked, this, &MainWidget::ShowCalibration);
    connect(win_func_menu_->auto_produce_, &IcoButton::clicked, this, &MainWidget::ShowAutoProduce);
    connect(win_func_menu_->system_info_, &IcoButton::clicked, this, &MainWidget::ShowSystemInfo);
    connect(win_func_menu_->program_data_, &IcoButton::clicked, this, &MainWidget::ShowProgramData);
    connect(win_func_menu_->control_panel_, &IcoButton::clicked, this, &MainWidget::ShowControlPanel);
    connect(win_func_menu_->manual_control_, &IcoButton::clicked, this, &MainWidget::ShowManualControl);
    connect(win_func_menu_->program_Editor_, &IcoButton::clicked, this, &MainWidget::ShowProgramEdit);
    connect(upper_widget_->close_button_, &QPushButton::clicked, this, &MainWidget::CloseWindow);
    connect(lower_widget_->quick_button_, &QuickButton::clicked, this, &MainWidget::ShowQuickWidget);

    connect(speed_widget_, &SpeedWidget::SpeedChange, upper_widget_->state_button_, &StateButton::SetSpeed);
    connect(program_data_->show_varinfo_->GetCreateWidget(), &CreateNewVar::CreateVar, program_edit_->edit_main_->code_edit_, &CodeEdit::CreateVar);
    connect(manual_control_->GetCreateNewVar(), &CreateNewVar::CreateVar, program_edit_->edit_main_->code_edit_, &CodeEdit::CreateVar);

    connect(win_event_log_->log_widget_, &LogWidget::PoPLogWidget, this, &MainWidget::PopEventLog);

    connect(auto_produce_, &AutoProduce::LoadTaSig, program_edit_->task_order_, &TaskOrder::LoadTask);

    connect(TeachState::instance().data(), &TeachState::RunMod, this, &MainWidget::PopAutoProduce);
}

void MainWidget::HideWidget()
{
    if(!win_all_tasks_->isHidden())
    {
        win_all_tasks_->hide();
    }
}

void MainWidget::CloseWindow()
{
    QWidget *widget = show_widgets_->GetShow();
    SaveButton *button = lower_widget_->save_button;

    if(widget == nullptr)
        return;
    QString title = widget->windowTitle();

    if(title == "hotedit")
    {
        button->RemButton(button->hotedit_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "file_manager")
    {
        button->RemButton(button->file_manager_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "input_out")
    {
        button->RemButton(button->int_out_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "backup_recovery")
    {
        button->RemButton(button->backup_recovery_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "calib")
    {
        button->RemButton(button->calibration_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "auto_produce")
    {
        button->RemButton(button->auto_produce_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "system_info")
    {
        button->RemButton(button->system_info_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "program_data")
    {
        button->RemButton(button->program_data_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "control_panel")
    {
        button->RemButton(button->control_panel_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "manual_control")
    {
        button->RemButton(button->manual_control_);
        show_widgets_->HideWidget(widget);
    }
    else if(title == "program_edit")
    {
        button->RemButton(button->program_Editor_);
        show_widgets_->HideWidget(widget);
    }

    if(button->GetSize() == 0) //所有窗口已关闭
        upper_widget_->close_button_->hide(); //隐藏关闭按钮
}

void MainWidget::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    this->resize(640*scale_w_, 480*scale_h_);
    HideWidget();

    emit SendScale(scale_w_, scale_h_);
}

void MainWidget::PopEventLog(int row)
{
    if(win_event_log_->isHidden())
    {
        QPoint p(this->mapToGlobal(QPoint(0, 0)));

        win_event_log_->resize(show_widgets_->size());
        win_event_log_->move(p.x(), p.y() + upper_widget_->height());
        win_event_log_->show();
        win_event_log_->ChangeLogWidget(row);
    }
}

void MainWidget::PopAutoProduce(RunMode mode)
{
    if(mode == RunMode::AutoMod)
    {
        win_func_menu_->auto_produce_->click();
    }
}

void MainWidget::ShowEventLog()
{
    QPoint p(this->mapToGlobal(QPoint(0, 0)));

    if(win_event_log_->isHidden())
    {
        win_event_log_->resize(show_widgets_->size());
        win_event_log_->move(p.x(), p.y() + upper_widget_->height());
        win_event_log_->show();
    }
    else
    {
        win_event_log_->hide();
    }

    if(!win_func_menu_->isHidden())
        win_func_menu_->hide();
}

void MainWidget::ShowHotEdit()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->hotedit_)) //已存在
    {
        show_widgets_->InsertWidget(hotedit_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->hotedit_)) //添加成功
        {
            show_widgets_->InsertWidget(hotedit_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show(); //显示关闭按钮
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowFileManager()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->file_manager_)) //已存在
    {
        show_widgets_->InsertWidget(file_manager_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->file_manager_)) //添加成功
        {
            show_widgets_->InsertWidget(file_manager_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowIntOut()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->int_out_)) //已存在
    {
        show_widgets_->InsertWidget(input_out_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->int_out_)) //添加成功
        {
            show_widgets_->InsertWidget(input_out_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowBackReco()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->backup_recovery_)) //已存在
    {
        show_widgets_->InsertWidget(backup_recovery_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->backup_recovery_)) //添加成功
        {
            show_widgets_->InsertWidget(backup_recovery_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowCalibration()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->calibration_)) //已存在
    {
        show_widgets_->InsertWidget(calib_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->calibration_)) //添加成功
        {
            show_widgets_->InsertWidget(calib_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowAutoProduce()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->auto_produce_)) //已存在
    {
        show_widgets_->InsertWidget(auto_produce_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->auto_produce_)) //添加成功
        {
            show_widgets_->InsertWidget(auto_produce_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowSystemInfo()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->system_info_)) //已存在
    {
        show_widgets_->InsertWidget(system_info_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->system_info_)) //添加成功
        {
            show_widgets_->InsertWidget(system_info_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowProgramData()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->program_data_)) //已存在
    {
        show_widgets_->InsertWidget(program_data_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->program_data_)) //添加成功
        {
            show_widgets_->InsertWidget(program_data_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowControlPanel()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->control_panel_)) //已存在
    {
        show_widgets_->InsertWidget(control_panel_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->control_panel_)) //添加成功
        {
            show_widgets_->InsertWidget(control_panel_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowManualControl()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->manual_control_)) //已存在
    {
        show_widgets_->InsertWidget(manual_control_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->manual_control_)) //添加成功
        {
            show_widgets_->InsertWidget(manual_control_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowProgramEdit()
{
    if(lower_widget_->save_button->IsExist(lower_widget_->save_button->program_Editor_)) //已存在
    {
        show_widgets_->InsertWidget(program_edit_);
        win_func_menu_->hide();
    }
    else
    {
        if(lower_widget_->save_button->AddButton(lower_widget_->save_button->program_Editor_)) //添加成功
        {
            show_widgets_->InsertWidget(program_edit_);
            win_func_menu_->hide();
            upper_widget_->close_button_->show();
        }
        else //添加失败，也就是页面已经打开了六个
        {
            QMessageBox::warning(this, tr("视图太多"), tr("打开的视图不能超过6个"));
        }
    }
}

void MainWidget::ShowQuickWidget()
{
    QPoint p(this->mapToGlobal(QPoint(0, 0)));

    if(quick_window_->isHidden())
    {
        quick_window_->resize(upper_widget_->close_button_->width(), this->height() - lower_widget_->height());
        quick_window_->move(p.x() + this->width() - quick_window_->width(), p.y());
        quick_window_->show();
    }
    else
    {
        quick_window_->hide();
    }
}

ShowWidgets::ShowWidgets(QWidget *parent) : QWidget(parent)
{
    fm_ = new QFrame(this);
    fm_->setStyleSheet("border-image: url(:/image/Image/MainWidget/screen.png)");
    widget_ = nullptr;
}

void ShowWidgets::resizeEvent(QResizeEvent *)
{
    fm_->resize(this->size());
    fm_->move(0, 0);

    if(widget_ != nullptr)
    {
        widget_->resize(this->size());
        widget_->move(this->pos());
    }
}

QWidget *ShowWidgets::GetShow()
{
    return widget_;
}

void ShowWidgets::InsertWidget(QWidget *widget)
{
    if(widget_ != nullptr)
    {
        widget_->hide(); //先隐藏已有界面
        widget_ = widget; //开始替换
    }
    else
    {
        widget_ = widget;
    }

    widget->resize(this->size());
    widget->move(this->pos());
    widget->show();
}

void ShowWidgets::HideWidget(QWidget *widget)
{
    widget->hide();

    widget_ = nullptr;
}
