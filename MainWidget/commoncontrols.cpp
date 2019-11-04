#include "commoncontrols.h"
#include "MainWidget/Functions/ProgramEditor/programeditordata.h"
#include "MainWidget/Functions/ManualControl/manualcontrol.h"
#include "RobotClient/robotClient.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <random>
#include <QMessageBox>
#include <QApplication>
#include <QJsonObject>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJsonArray>
#include <cmath>
#include <QDir>
#include <QDebug>

QSharedPointer<VarInfo> VarInfo::varinfo_point_;
QSharedPointer<DataType> DataType::data_type_point_;

Keyboard::Keyboard(QWidget *parent) : QWidget(parent)
{
    edit_ = new KeyLineEdit(this);
    cap_ = new QPushButton("CAP", this);
    shift_ = new QPushButton("Shift", this);
    intl_ = new QPushButton("Int'l", this);
    blank_space_ = new QPushButton(" ", this);
    upper_ = new QPushButton("↑", this);
    lower_ = new QPushButton("↓", this);
    left_ = new QPushButton("←", this);
    right_ = new QPushButton("→", this);
    end_ = new QPushButton("End", this);
    home_ = new QPushButton("Home", this);
    left_square_ = new QPushButton("[", this);
    right_square_ = new QPushButton("]", this);
    delete_ = new QPushButton("  ", this);
    one_ = new KeyButton(this);
    two_ = new KeyButton(this);
    three_ = new KeyButton(this);
    four_ = new KeyButton(this);
    five_ = new KeyButton(this);
    six_ = new KeyButton(this);
    seven_ = new KeyButton(this);
    eight_ = new KeyButton(this);
    nine_ = new KeyButton(this);
    zero_ = new KeyButton(this);
    reduce_ = new KeyButton(this);
    equal_ = new KeyButton(this);
    keya_ = new QPushButton(this);
    keyb_ = new QPushButton(this);
    keyc_ = new QPushButton(this);
    keyd_ = new QPushButton(this);
    keye_ = new QPushButton(this);
    keyf_ = new QPushButton(this);
    keyg_ = new QPushButton(this);
    keyh_ = new QPushButton(this);
    keyi_ = new QPushButton(this);
    keyj_ = new QPushButton(this);
    keyk_ = new QPushButton(this);
    keyl_ = new QPushButton(this);
    keym_ = new QPushButton(this);
    keyn_ = new QPushButton(this);
    keyo_ = new QPushButton(this);
    keyp_ = new QPushButton(this);
    keyq_ = new QPushButton(this);
    keyr_ = new QPushButton(this);
    keys_ = new QPushButton(this);
    keyt_ = new QPushButton(this);
    keyu_ = new QPushButton(this);
    keyv_ = new QPushButton(this);
    keyw_ = new QPushButton(this);
    keyx_ = new QPushButton(this);
    keyy_ = new QPushButton(this);
    keyz_ = new QPushButton(this);
    zzts0_ = new KeyButton(this);
    zzts1_ = new KeyButton(this);
    zzts2_ = new KeyButton(this);
    zzts3_ = new KeyButton(this);
    zzts4_ = new KeyButton(this);
    zzts5_ = new KeyButton(this);
    zzts6_ = new KeyButton(this);
    zzts7_ = new KeyButton(this);
    button_widget_ = new QWidget(this);
    determine_button_ = new QPushButton(tr("确认"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);

    scale_w_ = 1;
    scale_h_ = 1;
    iscap_ = false;

    ConnectInit();
    Init();

    determine_button_->setFlat(true);
    cancel_button_->setFlat(true);

    delete_->setStyleSheet("border-image: url(:/image/Image/MainWidget/keyboard_delete.png)");

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
}

void Keyboard::resizeEvent(QResizeEvent *)
{

    double &scalew = scale_w_;
    double &scaleh = scale_h_;

    edit_->resize(575*scalew, 26*scaleh);
    cap_->resize(71*scalew, 41*scaleh);
    shift_->resize(cap_->size());
    intl_->resize(shift_->size());
    blank_space_->resize(205*scalew, 41*scaleh);
    upper_->resize(39*scalew, 41*scaleh);
    lower_->resize(upper_->size());
    left_->resize(lower_->size());
    right_->resize(left_->size());
    end_->resize(intl_->size());
    home_->resize(end_->size());
    left_square_->resize(left_->size());
    right_square_->resize(left_square_->size());
    delete_->resize(52*scalew, 41*scaleh);
    one_->resize(upper_->size());
    two_->resize(one_->size());
    three_->resize(two_->size());
    four_->resize(two_->size());
    five_->resize(two_->size());
    six_->resize(two_->size());
    seven_->resize(two_->size());
    eight_->resize(two_->size());
    nine_->resize(two_->size());
    zero_->resize(two_->size());
    reduce_->resize(two_->size());
    equal_->resize(reduce_->size());
    keya_->resize(two_->size());
    keyb_->resize(two_->size());
    keyc_->resize(two_->size());
    keyd_->resize(two_->size());
    keye_->resize(two_->size());
    keyf_->resize(two_->size());
    keyg_->resize(two_->size());
    keyh_->resize(two_->size());
    keyi_->resize(two_->size());
    keyj_->resize(two_->size());
    keyk_->resize(two_->size());
    keyl_->resize(two_->size());
    keym_->resize(two_->size());
    keyn_->resize(two_->size());
    keyo_->resize(two_->size());
    keyp_->resize(two_->size());
    keyq_->resize(two_->size());
    keyr_->resize(two_->size());
    keys_->resize(two_->size());
    keyt_->resize(two_->size());
    keyu_->resize(two_->size());
    keyv_->resize(two_->size());
    keyw_->resize(two_->size());
    keyx_->resize(two_->size());
    keyy_->resize(two_->size());
    keyz_->resize(two_->size());
    zzts0_->resize(keyz_->size());
    zzts1_->resize(zzts0_->size());
    zzts2_->resize(zzts0_->size());
    zzts3_->resize(zzts0_->size());
    zzts4_->resize(zzts0_->size());
    zzts5_->resize(zzts0_->size());
    zzts6_->resize(zzts0_->size());
    zzts7_->resize(zzts0_->size());
    button_widget_->resize(this->width(), 43*scaleh);
    determine_button_->resize(126*scalew, button_widget_->height());
    cancel_button_->resize(determine_button_->size());

    edit_->move(32*scalew, 20*scaleh);
    cap_->move(19*scalew, 160*scaleh);
    shift_->move(11*scalew, 210*scaleh);
    intl_->move(11*scalew, 261*scaleh);
    blank_space_->move(175*scalew, 260*scaleh);
    upper_->move(blank_space_->x() + blank_space_->width() + 5*scalew, blank_space_->y());
    lower_->move(upper_->x() + upper_->width() + 5*scalew, upper_->y());
    left_->move(lower_->x() + lower_->width() + 5*scalew, lower_->y());
    right_->move(left_->x() + left_->width() + 5*scalew, left_->y());
    end_->move(right_->x() + right_->width() + 5*scalew, right_->y());
    home_->move(528*scalew, 210*scaleh);
    left_square_->move(541*scalew, 109*scaleh);
    right_square_->move(left_square_->x() + left_square_->width() + 5*scalew, left_square_->y());
    delete_->move(578*scalew, 58*scaleh);
    one_->move(50*scalew, 58*scaleh);
    two_->move(one_->x() + one_->width() + 5*scalew, one_->y());
    three_->move(two_->x() + two_->width() + 5*scalew, two_->y());
    four_->move(three_->x() + three_->width() + 5*scalew, three_->y());
    five_->move(four_->x() + four_->width() + 5*scalew, four_->y());
    six_->move(five_->x() + five_->width() + 5*scalew, five_->y());
    seven_->move(six_->x() + six_->width() + 5*scalew, six_->y());
    eight_->move(seven_->x() + seven_->width() + 5*scalew, seven_->y());
    nine_->move(eight_->x() + eight_->width() + 5*scalew, eight_->y());
    zero_->move(nine_->x() + nine_->width() + 5*scalew, nine_->y());
    reduce_->move(zero_->x() + zero_->width() + 5*scalew, zero_->y());
    equal_->move(reduce_->x() + reduce_->width() + 5*scalew, reduce_->y());
    keyq_->move(101*scalew, 109*scaleh);
    keyw_->move(keyq_->x() + keyq_->width() + 5*scalew, keyq_->y());
    keye_->move(keyw_->x() + keyw_->width() + 5*scalew, keyw_->y());
    keyr_->move(keye_->x() + keye_->width() + 5*scalew, keye_->y());
    keyt_->move(keyr_->x() + keyr_->width() + 5*scalew, keyr_->y());
    keyy_->move(keyt_->x() + keyt_->width() + 5*scalew, keyt_->y());
    keyu_->move(keyy_->x() + keyy_->width() + 5*scalew, keyy_->y());
    keyi_->move(keyu_->x() + keyu_->width() + 5*scalew, keyu_->y());
    keyo_->move(keyi_->x() + keyi_->width() + 5*scalew, keyi_->y());
    keyp_->move(keyo_->x() + keyo_->width() + 5*scalew, keyo_->y());
    keya_->move(cap_->x() + cap_->width() + 5*scalew, cap_->y());
    keys_->move(keya_->x() + keya_->width() + 5*scalew, keya_->y());
    keyd_->move(keys_->x() + keys_->width() + 5*scalew, keys_->y());
    keyf_->move(keyd_->x() + keyd_->width() + 5*scalew, keyd_->y());
    keyg_->move(keyf_->x() + keyf_->width() + 5*scalew, keyf_->y());
    keyh_->move(keyg_->x() + keyg_->width() + 5*scalew, keyg_->y());
    keyj_->move(keyh_->x() + keyh_->width() + 5*scalew, keyh_->y());
    keyk_->move(keyj_->x() + keyj_->width() + 5*scalew, keyj_->y());
    keyl_->move(keyk_->x() + keyk_->width() + 5*scalew, keyk_->y());
    keyz_->move(shift_->x() + shift_->width() + 5*scalew, shift_->y());
    keyx_->move(keyz_->x() + keyz_->width() + 5*scalew, keyz_->y());
    keyc_->move(keyx_->x() + keyx_->width() + 5*scalew, keyx_->y());
    keyv_->move(keyc_->x() + keyc_->width() + 5*scalew, keyc_->y());
    keyb_->move(keyv_->x() + keyv_->width() + 5*scalew, keyv_->y());
    keyn_->move(keyb_->x() + keyb_->width() + 5*scalew, keyb_->y());
    keym_->move(keyn_->x() + keyn_->width() + 5*scalew, keyn_->y());
    zzts0_->move(intl_->x() + intl_->width() + 5*scalew, intl_->y());
    zzts1_->move(zzts0_->x() + zzts0_->width() + 5*scalew, zzts0_->y());
    zzts2_->move(keym_->x() + keym_->width() + 5*scalew, keym_->y());
    zzts3_->move(zzts2_->x() + zzts2_->width() + 5*scalew, zzts2_->y());
    zzts4_->move(zzts3_->x() + zzts3_->width() + 5*scalew, zzts3_->y());
    zzts5_->move(keyl_->x() + keyl_->width() + 5*scalew, keyl_->y());
    zzts6_->move(zzts5_->x() + zzts5_->width() + 5*scalew, zzts5_->y());
    zzts7_->move(zzts6_->x() + zzts6_->width() + 5*scalew, zzts6_->y());
    button_widget_->move(0, intl_->y() + intl_->height() + 20*scaleh);
    cancel_button_->move(button_widget_->width() - cancel_button_->width(), 0);
    determine_button_->move(cancel_button_->x() - determine_button_->width(), cancel_button_->y());
}

void Keyboard::Init()
{
    if(iscap_) //之前为大写时需要对字体进行还原
    {
        QFont font(cap_->font());
        font.setWeight(50); //对字体进行还原
        cap_->setFont(font);
    }
    iscap_ = false; //默认为小写
    isintl_ = false; //默认显示正常字母
    isshift_ = false; //默认shift键未按下
    edit_->clear();
    edit_->setFocus();

    NumNormal();
    LetterSmall();
    ZztsOne();
}

void Keyboard::Init(QString text)
{
    if(iscap_) //之前为大写时需要对字体进行还原
    {
        QFont font(cap_->font());
        font.setWeight(50); //对字体进行还原
        cap_->setFont(font);
    }
    iscap_ = false; //默认为小写
    isintl_ = false; //默认显示正常字母
    isshift_ = false; //默认shift键未按下
    edit_->setText(text);
    edit_->setFocus();
    edit_->selectAll();

    NumNormal();
    LetterSmall();
    ZztsOne();
}

void Keyboard::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    resizeEvent(nullptr);
}

void Keyboard::ButtonClicked()
{
    auto button = dynamic_cast<QPushButton*>(sender());

    ProcessCenter(button->text());
}

void Keyboard::NumNormal()
{
    one_->setText("1");
    one_->setTitle("!");

    two_->setText("2");
    two_->setTitle("\"");

    three_->setText("3");
    three_->setTitle("#");

    four_->setText("4");
    four_->setTitle("$");

    five_->setText("5");
    five_->setTitle("%");

    six_->setText("6");
    six_->setTitle("&");

    seven_->setText("7");
    seven_->setTitle("{");

    eight_->setText("8");
    eight_->setTitle("(");

    nine_->setText("9");
    nine_->setTitle(")");

    zero_->setText("0");
    zero_->setTitle("}");

    reduce_->setText("-");
    reduce_->setTitle("_");

    equal_->setText("=");
    equal_->setTitle("?");
}

void Keyboard::NumSymbol()
{
    one_->setText("!");
    one_->setTitle("1");

    two_->setText("\"");
    two_->setTitle("2");

    three_->setText("#");
    three_->setTitle("3");

    four_->setText("$");
    four_->setTitle("4");

    five_->setText("%");
    five_->setTitle("5");

    six_->setText("&&"); //一个&将会无法显示
    six_->setTitle("6");

    seven_->setText("{");
    seven_->setTitle("7");

    eight_->setText("(");
    eight_->setTitle("8");

    nine_->setText(")");
    nine_->setTitle("9");

    zero_->setText("}");
    zero_->setTitle("0");

    reduce_->setText("_");
    reduce_->setTitle("-");

    equal_->setText("?");
    equal_->setTitle("=");
}

void Keyboard::LetterSmall()
{
    keya_->setText("a");
    keyb_->setText("b");
    keyc_->setText("c");
    keyd_->setText("d");
    keye_->setText("e");
    keyf_->setText("f");
    keyg_->setText("g");
    keyh_->setText("h");
    keyi_->setText("i");
    keyj_->setText("j");
    keyk_->setText("k");
    keyl_->setText("l");
    keym_->setText("m");
    keyn_->setText("n");
    keyo_->setText("o");
    keyp_->setText("p");
    keyq_->setText("q");
    keyr_->setText("r");
    keys_->setText("s");
    keyt_->setText("t");
    keyu_->setText("u");
    keyv_->setText("v");
    keyw_->setText("w");
    keyx_->setText("x");
    keyy_->setText("y");
    keyz_->setText("z");
}

void Keyboard::LetterLarge()
{
    keya_->setText("A");
    keyb_->setText("B");
    keyc_->setText("C");
    keyd_->setText("D");
    keye_->setText("E");
    keyf_->setText("F");
    keyg_->setText("G");
    keyh_->setText("H");
    keyi_->setText("I");
    keyj_->setText("J");
    keyk_->setText("K");
    keyl_->setText("L");
    keym_->setText("M");
    keyn_->setText("N");
    keyo_->setText("O");
    keyp_->setText("P");
    keyq_->setText("Q");
    keyr_->setText("R");
    keys_->setText("S");
    keyt_->setText("T");
    keyu_->setText("U");
    keyv_->setText("V");
    keyw_->setText("W");
    keyx_->setText("X");
    keyy_->setText("Y");
    keyz_->setText("Z");
}

void Keyboard::ZztsOne()
{
    zzts0_->setText("´");
    zzts0_->setTitle("`");

    zzts1_->setText("\\");
    zzts1_->setTitle("~");

    zzts2_->setText(",");
    zzts2_->setTitle("<");

    zzts3_->setText(".");
    zzts3_->setTitle(">");

    zzts4_->setText("/");
    zzts4_->setTitle(":");

    zzts5_->setText(";");
    zzts5_->setTitle("|");

    zzts6_->setText("'");
    zzts6_->setTitle("*");

    zzts7_->setText("+");
    zzts7_->setTitle("@");
}

void Keyboard::ZztsTwo()
{
    zzts0_->setText("`");
    zzts0_->setTitle("´");

    zzts1_->setText("~");
    zzts1_->setTitle("\\");

    zzts2_->setText("<");
    zzts2_->setTitle(",");

    zzts3_->setText(">");
    zzts3_->setTitle(".");

    zzts4_->setText(":");
    zzts4_->setTitle("/");

    zzts5_->setText("|");
    zzts5_->setTitle(";");

    zzts6_->setText("*");
    zzts6_->setTitle("'");

    zzts7_->setText("@");
    zzts7_->setTitle("+");
}

void Keyboard::IntlSmall()
{
    one_->setText("¿");
    one_->setTitle("");

    two_->setText("¡");
    two_->setTitle("");

    three_->setText("@");
    three_->setTitle("");

    four_->setText("¢");
    four_->setTitle("");

    five_->setText("£");
    five_->setTitle("");

    six_->setText("¥");
    six_->setTitle("");

    seven_->setText("§");
    seven_->setTitle("");

    eight_->setText("°");
    eight_->setTitle("");

    nine_->setText("±");
    nine_->setTitle("");

    zero_->setText("µ");
    zero_->setTitle("");

    reduce_->setText("-");
    reduce_->setTitle("");

    equal_->setText("=");
    equal_->setTitle("");

    keyq_->setText("à");

    keyw_->setText("á");

    keye_->setText("â");

    keyr_->setText("ã");

    keyt_->setText("ä");

    keyy_->setText("å");

    keyu_->setText("æ");

    keyi_->setText("ç");

    keyo_->setText("ð");

    keyp_->setText("ñ");

    keya_->setText("è");

    keys_->setText("é");

    keyd_->setText("ê");

    keyf_->setText("ë");

    keyg_->setText("ì");

    keyh_->setText("í");

    keyj_->setText("î");

    keyk_->setText("ï");

    keyl_->setText("þ");

    zzts5_->setText(";");
    zzts5_->setTitle("");

    zzts6_->setText("'");
    zzts6_->setTitle("");

    zzts7_->setText("+");
    zzts7_->setTitle("");

    keyz_->setText("ò");

    keyx_->setText("ó");

    keyc_->setText("ô");

    keyv_->setText("õ");

    keyb_->setText("ö");

    keyn_->setText("ù");

    keym_->setText("ú");

    zzts2_->setText("û");
    zzts2_->setTitle("");

    zzts3_->setText("ü");
    zzts3_->setTitle("");

    zzts4_->setText("/");
    zzts4_->setTitle("");

    zzts0_->setText("ý");
    zzts0_->setTitle("");

    zzts1_->setText("ÿ");
    zzts1_->setTitle("");
}

void Keyboard::IntlLarge()
{
    one_->setText("¿");
    one_->setTitle("");

    two_->setText("¡");
    two_->setTitle("");

    three_->setText("@");
    three_->setTitle("");

    four_->setText("¢");
    four_->setTitle("");

    five_->setText("£");
    five_->setTitle("");

    six_->setText("¥");
    six_->setTitle("");

    seven_->setText("§");
    seven_->setTitle("");

    eight_->setText("°");
    eight_->setTitle("");

    nine_->setText("±");
    nine_->setTitle("");

    zero_->setText("µ");
    zero_->setTitle("");

    reduce_->setText("-");
    reduce_->setTitle("");

    equal_->setText("=");
    equal_->setTitle("");

    keyq_->setText("À");

    keyw_->setText("Á");

    keye_->setText("Â");

    keyr_->setText("Ã");

    keyt_->setText("Ä");

    keyy_->setText("Å");

    keyu_->setText("Æ");

    keyi_->setText("Ç");

    keyo_->setText("Ð");

    keyp_->setText("Ñ");

    keya_->setText("È");

    keys_->setText("É");

    keyd_->setText("Ê");

    keyf_->setText("Ë");

    keyg_->setText("Ì");

    keyh_->setText("Í");

    keyj_->setText("Î");

    keyk_->setText("Ï");

    keyl_->setText("Þ");

    zzts5_->setText(";");
    zzts5_->setTitle("");

    zzts6_->setText("'");
    zzts6_->setTitle("");

    zzts7_->setText("+");
    zzts7_->setTitle("");

    keyz_->setText("Ò");

    keyx_->setText("Ó");

    keyc_->setText("Ô");

    keyv_->setText("Õ");

    keyb_->setText("Ö");

    keyn_->setText("Ù");

    keym_->setText("Ú");

    zzts2_->setText("Û");
    zzts2_->setTitle("");

    zzts3_->setText("Ü");
    zzts3_->setTitle("");

    zzts4_->setText("/");
    zzts4_->setTitle("");

    zzts0_->setText("Ý");
    zzts0_->setTitle("");

    zzts1_->setText("ß");
    zzts1_->setTitle("");
}

void Keyboard::ProcessCenter(QString text)
{
    if(text == "CAP")
    {
        if(iscap_) //之前为大写
        {
            QFont font(cap_->font());
            font.setWeight(50); //对字体进行还原
            cap_->setFont(font);
            iscap_ = false;
            if(isintl_) //显示的是国际化字母
            {
                IntlSmall(); //调整为国际化小写字母
            }
            else //显示的是正常字母
            {
                LetterSmall();
            }
            if(isshift_) //如果shift键被按下
            {
                isshift_ = false;
                QFont f(shift_->font());
                f.setWeight(50);
                shift_->setFont(f);
            }
        }
        else
        {
            QFont font(cap_->font());
            font.setWeight(75); //对字体进行加粗
            cap_->setFont(font);
            iscap_ = true;
            if(isintl_)
            {
                if(isshift_)
                {
                    isshift_ = false;
                    QFont f(shift_->font());
                    f.setWeight(50);
                    shift_->setFont(f);
                }
                IntlLarge();
            }
            else
            {
                if(isshift_)
                {
                    isshift_ = false;
                    QFont f(shift_->font());
                    f.setWeight(50);
                    shift_->setFont(f);
                    NumNormal();
                    ZztsOne();
                }
                LetterLarge();
            }
        }
        return;
    }
    else if(text == "Int'l")
    {
        if(isintl_)
        {
            QFont font(intl_->font());
            font.setWeight(50);
            intl_->setFont(font);
            isintl_ = false;
            if(iscap_)
            {
                if(isshift_)
                {
                    isshift_ = false;
                    QFont f(shift_->font());
                    f.setWeight(50);
                    shift_->setFont(f);
                }
                NumNormal();
                LetterLarge();
                ZztsOne();
            }
            else
            {
                if(isshift_)
                {
                    isshift_ = false;
                    QFont f(shift_->font());
                    f.setWeight(50);
                    shift_->setFont(f);
                }
                NumNormal();
                LetterSmall();
                ZztsOne();
            }
        }
        else
        {
            QFont font(intl_->font());
            font.setWeight(75);
            intl_->setFont(font);
            isintl_ = true;
            if(iscap_)
            {
                if(isshift_)
                {
                    isshift_ = false;
                    QFont f(shift_->font());
                    f.setWeight(50);
                    shift_->setFont(f);
                }
                IntlLarge();
            }
            else
            {
                if(isshift_)
                {
                    isshift_ = false;
                    QFont f(shift_->font());
                    f.setWeight(50);
                    shift_->setFont(f);
                }
                IntlSmall();
            }
        }
        return;
    }
    else if(text == "Shift")
    {
        if(isshift_)
        {
            QFont font(shift_->font());
            font.setWeight(50);
            shift_->setFont(font);
            isshift_ = false;
            if(iscap_)
            {
                if(!isintl_)
                {
                    NumNormal();
                    LetterLarge();
                    ZztsOne();
                }
            }
            else
            {
                if(isintl_)
                    IntlSmall();
                else
                {
                    NumNormal();
                    LetterSmall();
                    ZztsOne();
                }
            }
        }
        else
        {
            QFont font(shift_->font());
            font.setWeight(75);
            shift_->setFont(font);
            isshift_ = true;
            if(iscap_)
            {
                if(!isintl_)
                {
                    NumSymbol();
                    LetterSmall();
                    ZztsTwo();
                }
            }
            else
            {
                if(!isintl_)
                {
                    NumSymbol();
                    LetterLarge();
                    ZztsTwo();
                }
                else
                {
                    IntlLarge();
                }
            }
        }
        return;
    }
    else if(text == "↑" || text == "←")
    {
        edit_->cursorBackward(false); //向左移动光标，不选中内容
    }
    else if(text == "↓" || text == "→")
    {
        edit_->cursorForward(false); //向右移动光标，不选中内容
    }
    else if(text == "End")
    {
        edit_->end(false);
    }
    else if(text == "Home")
    {
        edit_->home(false);
    }
    else if(text == "  ") //删除
    {
        edit_->backspace();
    }
    else
    {
        if(text == "&&")
            text = "&";
        edit_->insert(text);
        if(isshift_)
        {
            QFont font(shift_->font());
            font.setWeight(50);
            shift_->setFont(font);
            isshift_ = false;

            if(iscap_)
            {
                if(!isintl_)
                {
                    NumNormal();
                    LetterLarge();
                    ZztsOne();
                }

            }
            else
            {
                if(!isintl_)
                {
                    NumNormal();
                    LetterSmall();
                    ZztsOne();
                }
                else
                {
                    IntlSmall();
                }
            }
        }
    }
}

void Keyboard::ConnectInit()
{
    connect(cap_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(shift_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(intl_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(blank_space_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(upper_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(lower_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(left_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(right_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(end_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(home_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(left_square_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(right_square_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(delete_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(one_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(two_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(three_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(four_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(five_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(six_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(seven_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(eight_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(nine_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zero_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(reduce_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(equal_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keya_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyb_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyc_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyd_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keye_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyf_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyg_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyh_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyi_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyj_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyk_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyl_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keym_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyn_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyo_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyp_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyq_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyr_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keys_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyt_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyu_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyv_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyw_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyx_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyy_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(keyz_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts0_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts1_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts2_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts3_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts4_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts5_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts6_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);
    connect(zzts7_, &QPushButton::clicked, this, &Keyboard::ButtonClicked);

    connect(determine_button_, &QPushButton::clicked,
            [=]()
    {
        emit GetData(edit_->text());
    });
}

KeyButton::KeyButton(QWidget *parent) : QPushButton(parent)
{
    title_label_ = new QLabel(this);

    title_label_->setStyleSheet("color: rgb(0, 0, 255)");
    title_label_->setAlignment(Qt::AlignCenter);
}

void KeyButton::resizeEvent(QResizeEvent *)
{
    title_label_->resize(this->width()/3, this->height()/3);

    title_label_->move(this->width() - title_label_->width(), 0);
}

QString KeyButton::GetTitle()
{
    return title_label_->text();
}

void KeyButton::setTitle(QString title)
{
    title_label_->setText(title);
}

FilterKey::FilterKey(QWidget *parent) : QFrame(parent)
{
    title_label_ = new QLabel(tr("活动过滤器"), this);
    edit_ = new KeyLineEdit(this);
    keya_ = new QPushButton("A", this);
    keyb_ = new QPushButton("B", this);
    keyc_ = new QPushButton("C", this);
    keyd_ = new QPushButton("D", this);
    keye_ = new QPushButton("E", this);
    keyf_ = new QPushButton("F", this);
    keyg_ = new QPushButton("G", this);
    keyh_ = new QPushButton("H", this);
    keyi_ = new QPushButton("I", this);
    keyj_ = new QPushButton("J", this);
    keyk_ = new QPushButton("K", this);
    keyl_ = new QPushButton("L", this);
    keym_ = new QPushButton("M", this);
    keyn_ = new QPushButton("N", this);
    keyo_ = new QPushButton("O", this);
    keyp_ = new QPushButton("P", this);
    keyq_ = new QPushButton("Q", this);
    keyr_ = new QPushButton("R", this);
    keys_ = new QPushButton("S", this);
    keyt_ = new QPushButton("T", this);
    keyu_ = new QPushButton("U", this);
    keyv_ = new QPushButton("V", this);
    keyw_ = new QPushButton("W", this);
    keyx_ = new QPushButton("X", this);
    keyy_ = new QPushButton("Y", this);
    keyz_ = new QPushButton("Z", this);
    delete_ = new QPushButton(" ", this);
    clear_button_ = new QPushButton(tr("清除"), this);
    reset_button_ = new QPushButton(tr("重置"), this);
    change_button_ = new QPushButton("123 > ", this);
    determine_button_ = new QPushButton(tr("过滤器"), this);

    scale_w_ = 1;
    scale_h_ = 1;
    isnum_ = false; //默认为字母状态

    title_label_->setAlignment(Qt::AlignCenter);
    delete_->setStyleSheet("border-image: url(:/image/Image/MainWidget/keyboard_delete.png)");

    ConnectInit();
}

void FilterKey::resizeEvent(QResizeEvent *)
{
    double &scalew = scale_w_;
    double &scaleh = scale_h_;

    title_label_->resize(78*scalew, 24*scaleh);
    edit_->resize(289*scalew, 24*scaleh);
    keya_->resize(41*scalew, 41*scaleh);
    keyb_->resize(41*scalew, 41*scaleh);
    keyc_->resize(41*scalew, 41*scaleh);
    keyd_->resize(41*scalew, 41*scaleh);
    keye_->resize(41*scalew, 41*scaleh);
    keyf_->resize(41*scalew, 41*scaleh);
    keyg_->resize(41*scalew, 41*scaleh);
    keyh_->resize(41*scalew, 41*scaleh);
    keyi_->resize(41*scalew, 41*scaleh);
    keyj_->resize(41*scalew, 41*scaleh);
    keyk_->resize(41*scalew, 41*scaleh);
    keyl_->resize(41*scalew, 41*scaleh);
    keym_->resize(41*scalew, 41*scaleh);
    keyn_->resize(41*scalew, 41*scaleh);
    keyo_->resize(41*scalew, 41*scaleh);
    keyp_->resize(41*scalew, 41*scaleh);
    keyq_->resize(41*scalew, 41*scaleh);
    keyr_->resize(41*scalew, 41*scaleh);
    keys_->resize(41*scalew, 41*scaleh);
    keyt_->resize(41*scalew, 41*scaleh);
    keyu_->resize(41*scalew, 41*scaleh);
    keyv_->resize(41*scalew, 41*scaleh);
    keyw_->resize(41*scalew, 41*scaleh);
    keyx_->resize(41*scalew, 41*scaleh);
    keyy_->resize(41*scalew, 41*scaleh);
    keyz_->resize(41*scalew, 41*scaleh);
    delete_->resize(41*scalew, 41*scaleh);
    clear_button_->resize(70*scalew, 40*scaleh);
    reset_button_->resize(92*scalew, 40*scaleh);
    change_button_->resize(82*scalew, 40*scaleh);
    determine_button_->resize(102*scalew, 40*scaleh);

    title_label_->move(0, 0);
    edit_->move(title_label_->x() + title_label_->width(), title_label_->y());
    keya_->move(edit_->x(), edit_->y() + edit_->height());
    keyb_->move(keya_->x() + keya_->width() + scalew, keya_->y());
    keyc_->move(keyb_->x() + keyb_->width() + scalew, keyb_->y());
    keyd_->move(keyc_->x() + keyc_->width() + scalew, keyc_->y());
    keye_->move(keyd_->x() + keyd_->width() + scalew, keyd_->y());
    keyf_->move(keye_->x() + keye_->width() + scalew, keye_->y());
    keyg_->move(keyf_->x() + keyf_->width() + scalew, keyf_->y());
    keyh_->move(keya_->x(), keya_->y() + keya_->height() + scaleh);
    keyi_->move(keyh_->x() + keyh_->width() + scalew, keyh_->y());
    keyj_->move(keyi_->x() + keyi_->width() + scalew, keyi_->y());
    keyk_->move(keyj_->x() + keyj_->width() + scalew, keyj_->y());
    keyl_->move(keyk_->x() + keyk_->width() + scalew, keyk_->y());
    keym_->move(keyl_->x() + keyl_->width() + scalew, keyl_->y());
    keyn_->move(keym_->x() + keym_->width() + scalew, keym_->y());
    keyo_->move(keyh_->x(), keyh_->y() + keyh_->height() + scaleh);
    keyp_->move(keyo_->x() + keyo_->width() + scalew, keyo_->y());
    keyq_->move(keyp_->x() + keyp_->width() + scalew, keyp_->y());
    keyr_->move(keyq_->x() + keyq_->width() + scalew, keyq_->y());
    keys_->move(keyr_->x() + keyr_->width() + scalew, keyr_->y());
    keyt_->move(keys_->x() + keys_->width() + scalew, keys_->y());
    keyu_->move(keyt_->x() + keyt_->width() + scalew, keyt_->y());
    keyv_->move(keyo_->x(), keyo_->y() + keyo_->height() + scaleh);
    keyw_->move(keyv_->x() + keyv_->width() + scalew, keyv_->y());
    keyx_->move(keyw_->x() + keyw_->width() + scalew, keyw_->y());
    keyy_->move(keyx_->x() + keyx_->width() + scalew, keyx_->y());
    keyz_->move(keyy_->x() + keyy_->width() + scalew, keyy_->y());
    if(isnum_) //为数字状态时
        delete_->move(keyw_->x() + keyw_->width() + scalew, keyw_->y());
    else //为字母状态时
        delete_->move(keyz_->x() + 2*(keyz_->width() + scalew), keyz_->y());
    clear_button_->move(16*scalew, keyv_->y() + keyv_->height() + 10*scaleh);
    reset_button_->move(clear_button_->x() + clear_button_->width() + 4*scalew, clear_button_->y());
    change_button_->move(reset_button_->x() + reset_button_->width() + 4*scalew, reset_button_->y());
    determine_button_->move(change_button_->x() + change_button_->width() + 4*scalew, change_button_->y());
}

void FilterKey::showEvent(QShowEvent *)
{
    emit ShowWindow();
}

void FilterKey::hideEvent(QHideEvent *)
{
    emit HideWindow();
}

void FilterKey::Init()
{
    isnum_ = false; //默认为字母状态
    change_button_->setText("123 > ");
    edit_->clear();
    filter_text_.clear();
    ShowLetter();
}

QString FilterKey::GetFilter()
{
    return filter_text_;
}

void FilterKey::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    this->resizeEvent(nullptr);
}

void FilterKey::ButtonClicked()
{
    QPushButton *button = dynamic_cast<QPushButton*>(sender());
    ProcessCenter(button->text());
}

void FilterKey::ShowNum()
{
    auto scalew = static_cast<int>(scale_w_);

    keyd_->hide();
    keye_->hide();
    keyf_->hide();
    keyg_->hide();
    keyk_->hide();
    keyl_->hide();
    keym_->hide();
    keyn_->hide();
    keyr_->hide();
    keys_->hide();
    keyt_->hide();
    keyu_->hide();
    keyy_->hide();
    keyz_->hide();
    keyx_->hide();
    delete_->move(keyw_->x() + keyw_->width() + scalew, keyw_->y());

    keya_->setText("7");
    keyb_->setText("8");
    keyc_->setText("9");
    keyh_->setText("4");
    keyi_->setText("5");
    keyj_->setText("6");
    keyo_->setText("1");
    keyp_->setText("2");
    keyq_->setText("3");
    keyv_->setText("_");
    keyw_->setText("0");
}

void FilterKey::ShowLetter()
{
    auto scalew = static_cast<int>(scale_w_);

    keyd_->show();
    keye_->show();
    keyf_->show();
    keyg_->show();
    keyk_->show();
    keyl_->show();
    keym_->show();
    keyn_->show();
    keyr_->show();
    keys_->show();
    keyt_->show();
    keyu_->show();
    keyy_->show();
    keyz_->show();
    keyx_->show();
    delete_->move(keyz_->x() + 2*(keyz_->width() + scalew), keyz_->y());

    keya_->setText("A");
    keyb_->setText("B");
    keyc_->setText("C");
    keyh_->setText("H");
    keyi_->setText("I");
    keyj_->setText("J");
    keyo_->setText("O");
    keyp_->setText("P");
    keyq_->setText("Q");
    keyv_->setText("V");
    keyw_->setText("W");
}

void FilterKey::ProcessCenter(QString text)
{
    if(text == "123 > ")
    {
        change_button_->setText(" < ABC");
        isnum_ = true;
        ShowNum();
        return;
    }
    else if(text == " < ABC")
    {
        change_button_->setText("123 > ");
        isnum_ = false;
        ShowLetter();
        return;
    }
    else if(text == " ")
    {
        edit_->backspace();
        return;
    }
    else
        edit_->insert(text);
}

void FilterKey::ConnectInit()
{
    connect(clear_button_, &QPushButton::clicked,
            [=]()
    {
        edit_->clear();
    });

    connect(reset_button_, &QPushButton::clicked,
            [=]()
    {
        edit_->clear();
    });

    connect(determine_button_, &QPushButton::clicked,
            [=]()
    {
        filter_text_ = edit_->text();
        emit GetData(filter_text_);
    });

    connect(change_button_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(delete_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keya_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyb_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyc_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyd_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keye_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyf_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyg_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyh_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyi_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyj_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyk_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyl_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keym_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyn_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyo_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyp_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyq_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyr_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keys_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyt_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyu_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyv_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyw_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyx_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyy_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
    connect(keyz_, &QPushButton::clicked, this, &FilterKey::ButtonClicked);
}

DataType::DataType(QObject *parent) : QObject(parent)
{
}

DataType::~DataType()
{
}

QSharedPointer<DataType> DataType::instance()
{
    if(data_type_point_.isNull())
    {
        data_type_point_ = QSharedPointer<DataType>(new DataType);

        data_type_point_->GetFile();
    }
    return data_type_point_;
}

bool DataType::IsExist(QString &name)
{

    for(auto it = data_type_info_list_.constBegin(); it != data_type_info_list_.constEnd(); it++)
    {
        if(it->first == name)
            return true;
    }

    return false;
}

bool DataType::IsBase(QString &name)
{
    if(!IsExist(name))
        return false;
    return data_type_info_list_.at(GetInfoRow(name)).second.isbase;
}

QSharedPointer<QList<QPair<QString, QString> > > DataType::GetForms(QString &name)
{
    if(!IsExist(name))
        return QSharedPointer<QList<QPair<QString, QString>>>();

    if(data_type_info_list_.at(GetInfoRow(name)).second.isbase)
        return QSharedPointer<QList<QPair<QString, QString>>>();

    return data_type_info_list_.at(GetInfoRow(name)).second.form;
}

QStringList DataType::GetTypeName()
{
    QStringList list;

    for(auto pr : data_type_info_list_)
    {
        list.append(pr.first);
    }

    return list;
}

QStringList DataType::GetShowTypeName()
{
    QStringList list;

    QStringList keys = GetTypeName();

    for(QString type : keys)
    {
        if(data_type_info_list_.at(GetInfoRow(type)).second.isshow)
            list.append(type);
    }
    return list;
}

void DataType::GetFile()
{
    QString path = qApp->applicationDirPath() + "/data/RAPID/DataType/";

    QDir dir(path);
    dir.setFilter(QDir::Files);

    QStringList fo = dir.entryList();
    for(QString fi : fo)
    {
        QFile file(path + fi);

        if(!file.open(QIODevice::ReadOnly))
        {
            continue;
        }
        else
        {
            QByteArray data = file.readAll();

            Analysis(data);
        }
    }
}

void DataType::Analysis(QByteArray &json)
{
    QJsonParseError error;
    QJsonDocument document(QJsonDocument::fromJson(json, &error));

    if(error.error != QJsonParseError::NoError)
        return;

    QJsonObject rootjson = document.object();
    if(!rootjson.contains("type_name") || !rootjson.contains("isshow") || !rootjson.contains("isbase"))
        return;

    QString name = rootjson.value("type_name").toString();
    auto pair = VarInfo::instance()->GetStcVarName(name);
    VarInfo::instance()->var_name_map_.insert(name, VarInfo::VarName(pair.first, 0, pair.second));

    TypeInfo to;
    to.isbase = rootjson.value("isbase").toBool();
    to.isshow = rootjson.value("isshow").toBool();
    to.isdata = rootjson.value("isdata").toBool();

    if(!to.isbase) //复合类型
    {
        if(to.form.isNull())
        {
            to.form = QSharedPointer<QList<QPair<QString, QString>>>(new QList<QPair<QString, QString>>);
        }

        QJsonObject form = rootjson.value("form").toObject();
        QStringList str_list = form.keys();

        for(QString str : str_list)
        {
            QString type = form.value(str).toString();
            str = str.mid(str.indexOf("_") + 1);

            to.form->push_back(qMakePair(str, type));
        }
    }

    if(to.isdata) //存在内置变量
    {
        QJsonArray data = rootjson.value("data").toArray();

        for(int i = 0; i < data.count(); i++)
        {
            QJsonObject object = data.at(i).toObject();

            QString name = object.value("name").toString();
            VarDataInfo info(name);
            info.data_type = object.value("type").toString();
            info.type = object.value("Subo_type").toString();
            if(info.type.isEmpty())
                info.type = info.data_type;
            info.var = object.value("val").toString();
            info.init_var = info.var;
            info.nature = 0;
            info.genre = false;
            info.action_scope = "all";

            VarInfo::instance()->AddVar(name, info);
        }
    }

    data_type_info_list_.append(qMakePair(name, to));
}

int DataType::GetInfoRow(QString name)
{
    for(int i = 0; i < data_type_info_list_.size(); i++)
    {
        if(data_type_info_list_.at(i).first == name)
            return i;
    }
    return -1;
}

ImageButton::ImageButton(QString text, QString image_path, QWidget *parent) : QFrame(parent)
{
    Init(image_path);
    text_label_->setText(text);
}

ImageButton::ImageButton(QString image_path, QWidget *parent) : QFrame(parent)
{
    Init(image_path);
}

ImageButton::ImageButton(QWidget *parent) : QFrame(parent)
{
    QString temp;
    Init(temp);
}

QLabel *ImageButton::GetImageLabel()
{
    return image_label_;
}

QLabel *ImageButton::GetTextLabel()
{
    return text_label_;
}

void ImageButton::SetImage(QString image_path)
{
    QString image = "border-image: url(" + image_path + ")";
    image_label_->setStyleSheet(image);
}

void ImageButton::SetText(QString text)
{
    text_label_->setText(text);
}

void ImageButton::SetButton(QString text, QString image_path)
{
    SetText(text);
    SetImage(image_path);
}

void ImageButton::SetIsSelect(bool isselect)
{
    isselect_ = isselect;

    if(isselect_) //已选中
    {
        this->setFrameShape(QFrame::Box);
    }
    else
    {
        this->setFrameShape(QFrame::NoFrame);
    }
}

bool ImageButton::GetIsSelect()
{
    return isselect_;
}

void ImageButton::SetLeverSelect(bool leverselect)
{
    lever_select_ = leverselect;
}

bool ImageButton::GetLeverSelect()
{
    return lever_select_;
}

QString ImageButton::GetText()
{
    return text_label_->text();
}

void ImageButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        this->SetIsSelect(true);
        emit clicked();
    }
}

void ImageButton::Init(QString &image_path)
{
    isselect_ = false;

    image_label_ = new QLabel(this);
    text_label_ = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(image_label_);
    layout->addWidget(text_label_);
    layout->setStretchFactor(image_label_, 3);
    layout->setStretchFactor(text_label_, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    QString image = "border-image: url(" + image_path + ")";
    image_label_->setStyleSheet(image);
    text_label_->setAlignment(Qt::AlignCenter);
}

VarInfo::VarInfo(QObject *parent) : QObject(parent)
{
    shmutex_ = new QReadWriteLock;
}

VarInfo::~VarInfo()
{
    delete shmutex_;
}

QSharedPointer<VarInfo> VarInfo::instance()
{
    if(varinfo_point_.isNull())
    {
        varinfo_point_ = QSharedPointer<VarInfo>(new VarInfo);
    }

    return varinfo_point_;
}

QVariant VarInfo::GetVar(QString type, QString dim)
{
    QReadLocker lk(shmutex_);
    if(!DataType::instance()->IsExist(type))
    {
        return QVariant();
    }
    dim.replace("{", "").replace("}", "");

    QString code = "[";
    QStringList dimlist = dim.split(",");
    int dim_num = dimlist.size();
    QString temp;

    if(DataType::instance()->IsBase(type)) //基础类型
    {
        if(type == "num")
            temp = "0";
        else if(type == "string")
            temp = "\"\"";
        else if(type == "bool")
            temp = "FALSE";
    }
    else //符合类型
    {
        QSharedPointer<QList<QPair<QString, QString>>> form = DataType::instance()->GetForms(type);

        int size = form->size();

        temp.append("[");

        for(int i = 0; i < size; i++)
        {
            QString type_name = form->at(i).second;
            temp.append(GetForms(type_name));

            if(i + 1 < size)
                temp.append(",");
        }

        temp.append("]");
    }

    if(dim_num == 1)
    {
        int num = dimlist.at(0).toInt();

        for(int i = 0; i < num; i++)
        {
            if(code == "[")
                code.append(temp);
            else
                code.append("," + temp);
        }
    }
    else if(dim_num == 2)
    {
        int num1 = dimlist.at(0).toInt();
        int num2 = dimlist.at(1).toInt();

        for(int x = 0; x < num1; x++)
        {
            code.append("[");
            for(int y = 0; y < num2; y++)
            {
                code.append(temp);
                if(y + 1 < num2)
                    code.append(",");
            }
            if(x + 1 < num1)
                code.append("],");
            else
                code.append("]");
        }
    }
    else if(dim_num == 3)
    {
        int num1 = dimlist.at(0).toInt();
        int num2 = dimlist.at(1).toInt();
        int num3 = dimlist.at(2).toInt();

        for(int x = 0; x < num1; x++)
        {
            code.append("[");
            for(int y = 0; y < num2; y++)
            {
                code.append("[");
                for(int z = 0; z < num3; z++)
                {
                    code.append(temp);
                    if( z + 1 < num3)
                        code.append(",");
                }
                if(y + 1 < num2)
                    code.append("],");
                else
                    code.append("]");
            }
            if(x + 1 < num1)
                code.append("],");
            else
                code.append("]");
        }
    }

    code.append("]");
    return code;
}

QString VarInfo::GetForms(QString type)
{
    QSharedPointer<QList<QPair<QString, QString>>> form = DataType::instance()->GetForms(type);

    if(form.isNull())
    {
        if(type == "num")
            return "0";
        else if(type == "string")
            return "\"\"";
        else if(type == "bool")
            return "FALSE";
    }

    QString code = "[";


    for(int i = 0; i < form->size(); i++)
    {

        QString temp = GetForms(form->at(i).second);

        if(i + 1 < form->size())
            temp.append(",");

        code.append(temp);
    }

    code.append("]");
    return code;
}

bool VarInfo::RemoveVar(QString var_name, QString data_type, QString action_scope)
{
    QWriteLocker lk(shmutex_);
    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end();)
    {
        if(!it->genre || it.key() != var_name)
        {
            it++;
            continue;
        }
        if(!action_scope.isEmpty())
        {
            if(action_scope != it->action_scope)
            {
                it++;
                continue;
            }
        }
        else //当未指明作用域时默认作用域为全局和任务
        {
            if(it->action_scope != "all" && it->action_scope != "task")
            {
                it++;
                continue;
            }
        }
        if(!data_type.isEmpty() && data_type != it->data_type)
        {
            it++;
            continue;
        }

        if(it->data_type == "wobjdata") //工件坐标
        {
            auto frame = rclib::RobotClient::initance()->getRobotFrame();

            frame.deleteWorkFrame(it.key().toStdString());

            rclib::RobotClient::initance()->setRobotFrame(frame);
        }
        else if(it->data_type == "tooldata") //工具坐标
        {
            auto frame = rclib::RobotClient::initance()->getRobotFrame();

            frame.deleteToolFrame(it.key().toStdString());

            rclib::RobotClient::initance()->setRobotFrame(frame);
        }

        auto &mod_data = EditData::instance()->GetModData(it->mod);
        QString code = GetCode(it.value(), var_name);

        if(it->action_scope == "all" || it->action_scope == "local" || it->action_scope == "task")
        {
            mod_data.mod_code.removeAll(code);
        }
        else
        {
            mod_data.routine_data[it->action_scope].code.removeAll(code);
        }

        varinfo_map_.erase(it);

        lk.unlock();
        emit VarRemove(it->mod);
        TeachState::instance()->SetCodeState();

        return true;
    }

    return false;
}

QString VarInfo::GetCode(VarDataInfo &var_info, QString var_name)
{
    QString var_code;

    if(var_info.action_scope == "local")
        var_code.append("LOCAL ");

    switch (var_info.nature)
    {
    case 0:
    {
        var_code.append("VAR ");
        break;
    }
    case 1:
    {
        var_code.append("PERS ");
        break;
    }
    case 2:
    {
        var_code.append("CONST ");
        break;
    }
    default:
        break;
    }

    var_code.append(var_info.data_type + " ");
    var_code.append(var_name);

    if(var_info.dim.isEmpty())
    {
        if(var_info.var.toString().isEmpty())
            var_code.append(";");
        else
            var_code.append(":=" + var_info.var.toString() + ";");
    }
    else
    {
        var_code.append(var_info.dim + ":=" + var_info.var.toString() + ";");
    }

    return var_code;
}

VarDataInfo &VarInfo::GetUcsVarInfo(QString var_name)
{
    QReadLocker lk(shmutex_);
    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); ++it)
    {
        if(it.key() == var_name)
        {
            if(it->data_type == "wobjdata" || it->data_type == "tooldata")
            {
                return it.value();
            }
        }
    }
    throw ABBException("VarDataInfo", "GetUcsVarInfo", "变量不存在");
}

VarDataInfo &VarInfo::GetVarInfo(QString var_name, QString mod_name, QString action_scope)
{
    QReadLocker lk(shmutex_);
    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); ++it)
    {
        if(it.key() != var_name)
            continue;
        if(it->mod != mod_name)
            continue;
        if(it->action_scope != action_scope)
            continue;

        return it.value();
    }

    throw ABBException("VarDataInfo", "GetVarInfo", "变量不存在");
}

QString VarInfo::GetUnused(QString type)
{
    if(type.isEmpty() || !var_name_map_.contains(type))
    {
        type = "var_temp_";

        while(true)
        {
            std::default_random_engine e;
            std::random_device r;
            std::uniform_int_distribution<int> u(0, 99999);
            e.seed(r());

            type.append(QString::number(u(e)));
            if(!IsExist(type))
                return type;
        }
    }

    auto &varname = var_name_map_[type];
    auto name = varname.head + QString::number(varname.num + varname.weight);

    return name;
}

QPair<QString, int> VarInfo::GetStcVarName(QString &type)
{
    QString head = type;
    int num = 1;

    if(type == "bool")
    {
        head = "flag";
    }
    else if(type == "clock")
    {
        head = "clock";
    }
    else if(type == "confdata")
    {
        head = "robconf";
    }
    else if(type == "extjoint")
    {
        head = "extjoint";
    }
    else if(type == "jointtarget")
    {
        head = "jpos";
        num = 10;
    }
    else if(type == "loaddata")
    {
        head = "load";
    }
    else if(type == "num")
    {
        head = "reg";
    }
    else if(type == "orient")
    {
        head = "orient";
    }
    else if(type == "pos")
    {
        head = "pos";
    }
    else if(type == "pose")
    {
        head = "pose";
    }
    else if(type == "robjoint")
    {
        head = "robjoint";
    }
    else if(type == "robtarget")
    {
        head = "p";
        num = 10;
    }
    else if(type == "signalai")
    {
        head = "signalai";
    }
    else if(type == "signalao")
    {
        head = "signalao";
    }
    else if(type == "signaldi")
    {
        head = "signaldi";
    }
    else if(type == "signaldo")
    {
        head = "signaldo";
    }
    else if(type == "speeddata")
    {
        head = "speed";
    }
    else if(type == "string")
    {
        head = "string";
    }
    else if(type == "tooldata")
    {
        head = "tool";
    }
    else if(type == "wobjdata")
    {
        head = "wobj";
    }
    else if(type == "zonedata")
    {
        head = "zone";
    }

    return qMakePair(head, num);
}

void VarInfo::RoutRename(QString old_name, QString new_name)
{
    QWriteLocker lock(shmutex_);
    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); ++it)
    {
        if(it->action_scope == old_name)
            it->action_scope = new_name;
    }
}

void VarInfo::RoutMove(QString rout_name, QString new_name)
{
    QWriteLocker lock(shmutex_);
    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); ++it)
    {
        if(it->action_scope == rout_name)
        {
            it->mod = new_name;
        }
    }
}

void VarInfo::ModRename(QString old_name, QString new_name)
{
    QWriteLocker lock(shmutex_);
    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); ++it)
    {
        if(it->mod == old_name)
            it->mod = new_name;
    }
}

void VarInfo::ModDel(QString mod_name)
{
    QWriteLocker lk(shmutex_);
    QList<QPair<std::string, bool>> coordinate; //保存坐标用于后面一次性删除，真为工具假为工件

    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end();)
    {
        if(it->mod == mod_name)
        {
            if(it->data_type == "wobjdata") //工件坐标
            {
                coordinate.append(qMakePair(it.key().toStdString(), false));
            }
            else if(it->data_type == "tooldata") //工具坐标
            {
                coordinate.append(qMakePair(it.key().toStdString(), true));
            }

            it = varinfo_map_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if(!coordinate.isEmpty())
    {
        auto frame = rclib::RobotClient::initance()->getRobotFrame();

        for(auto var : coordinate)
        {
            if(var.second)
                frame.deleteToolFrame(var.first);
            else
                frame.deleteWorkFrame(var.first);
        }
        rclib::RobotClient::initance()->setRobotFrame(frame);
    }
}

void VarInfo::RoutDel(QString rout_name)
{
    QWriteLocker lk(shmutex_);
    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end();)
    {
        if(it->action_scope == rout_name)
            it = varinfo_map_.erase(it);
        else
            ++it;
    }
}

void VarInfo::ClearMod()
{
    QWriteLocker lk(shmutex_);
    QList<QPair<std::string, bool>> coordinate; //保存坐标用于后面一次性删除，真为工具假为工件

    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end();)
    {
        if(it->genre)
        {
            if(it->data_type == "wobjdata") //工件坐标
            {
                coordinate.append(qMakePair(it.key().toStdString(), false));
            }
            else if(it->data_type == "tooldata") //工具坐标
            {
                coordinate.append(qMakePair(it.key().toStdString(), true));
            }

            it = varinfo_map_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if(!coordinate.isEmpty())
    {
        auto frame = rclib::RobotClient::initance()->getRobotFrame();

        for(auto var : coordinate)
        {
            if(var.second)
                frame.deleteToolFrame(var.first);
            else
                frame.deleteWorkFrame(var.first);
        }

        rclib::RobotClient::initance()->setRobotFrame(frame);
    }
}

bool VarInfo::AddVar(QString &name, VarDataInfo &info)
{
    if(IsExist(name, info))
        return false;

    QWriteLocker lk(shmutex_);

    varinfo_map_.insert(name, info);
    TeachState::instance()->SetCodeState();

    QString head;
    if(!var_name_map_.contains(info.data_type))
    {
        auto pair = GetStcVarName(info.data_type);
        var_name_map_.insert(info.data_type, VarName(pair.first, 0, pair.second));
        head = pair.first;
    }
    else
    {
        head = var_name_map_.value(info.data_type).head;
    }
    if(name.size() > head.size())
    {
        auto name_head = name.mid(0, head.size());
        if(name_head == head)
        {
            bool isint = false;
            int name_num = name.mid(name_head.size()).toInt(&isint);
            if(isint)
            {
                auto &vn = var_name_map_[info.data_type];
                if(name_num > vn.num)
                {
                    vn.num = name_num;
                }
            }
        }
    }

    return true;
}

bool VarInfo::IsExist(QString &name)
{
    QReadLocker lock(shmutex_);

    if(!varinfo_map_.contains(name))
        return false;
    return true;
}

bool VarInfo::IsExist(QString &name, VarDataInfo &info)
{
    QReadLocker lk(shmutex_);

    if(varinfo_map_.contains(name))
        return true;
    else
        return false;

    auto list = varinfo_map_.values(name);

    if(info.data_type == "wobjdata" || info.data_type == "tooldata") //因为坐标变量与服务端相关联的特殊性，所以坐标变量将会唯一
    {
        if(!list.isEmpty())
            return true;
    }

    for(auto in : list)
    {
        if(in.action_scope == info.action_scope)
        {
            if(in.action_scope == "local") //如果是本地化变量需要检查所属模块是否一样
            {
                if(in.mod == info.mod)
                    return true;
            }
            else
                return true;
        }
    }

    return false;
}

QList<VarDataInfo> VarInfo::GetVarInfo(QString name)
{
    if(!IsExist(name))
        return QList<VarDataInfo>();

    QReadLocker lk(shmutex_);

    return varinfo_map_.values(name);
}

QStringList VarInfo::GetVarName()
{
    QReadLocker lk(shmutex_);
    return varinfo_map_.keys();
}

QList<QPair<QString, VarDataInfo> > VarInfo::GetTypeVar(QString &type)
{
    QReadLocker lk(shmutex_);
    QList<QPair<QString, VarDataInfo>> type_var_list;

    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); it++)
    {
        if(it->data_type == type)
            type_var_list.append(qMakePair(it.key(), it.value()));
    }

    return type_var_list;
}

QStringList VarInfo::GetVarName(QString mod_name)
{
    QReadLocker lk(shmutex_);
    QMap<QString, int> map;
    QStringList list;

    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); it++)
    {
        if(it->mod == mod_name)
            map.insert(it.key(), 0);
    }

    list = map.keys();

    return list;
}

QStringList VarInfo::GetVarName(QString mod_name, QString rout_name)
{
    QReadLocker lk(shmutex_);
    QStringList list;

    for(auto it = varinfo_map_.begin(); it != varinfo_map_.end(); it++)
    {
        if(it->mod == mod_name && it->action_scope == rout_name)
            list.append(it.key());
    }

    return list;
}

ThreeSectionWindow::ThreeSectionWindow(QWidget *parent) : QFrame(parent)
{
    QVBoxLayout *th_se_wi_layout = new QVBoxLayout(this);

    upper_widget_ = new QWidget(this);
    in_widget_ = new QFrame(this);
    lower_widget_ = new QWidget(this);

    th_se_wi_layout->addWidget(upper_widget_);
    th_se_wi_layout->addWidget(in_widget_);
    th_se_wi_layout->addWidget(lower_widget_);
    th_se_wi_layout->setStretchFactor(upper_widget_, 2);
    th_se_wi_layout->setStretchFactor(in_widget_, 4);
    th_se_wi_layout->setStretchFactor(lower_widget_, 1);
    th_se_wi_layout->setContentsMargins(0, 0, 0, 0);
    th_se_wi_layout->setSpacing(0);

    this->setLayout(th_se_wi_layout);
}

VarEditWindow::VarEditWindow(QWidget *parent) : ThreeSectionWindow(parent)
{
    QVBoxLayout *upper_layout = new QVBoxLayout(upper_widget_);

    QHBoxLayout *name_layout = new QHBoxLayout;
    tips_name_label_ = new QLabel(tr("名称:"), upper_widget_);
    name_label_ = new QLabel(upper_widget_);
    name_layout->addWidget(tips_name_label_);
    name_layout->addWidget(name_label_);

    tips_label_ = new QLabel(tr("点击一个字段以编辑值"), upper_widget_);

    upper_layout->addLayout(name_layout);
    upper_layout->addWidget(tips_label_);
    upper_widget_->setLayout(upper_layout);

    QPalette p;
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    lower_widget_->setAutoFillBackground(true);
    lower_widget_->setPalette(p);
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
}

void VarEditWindow::SetNameLabel(QString str)
{
    name_label_->setText(str);
}

CreateNewVar::CreateNewVar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *window_layout = new QHBoxLayout(this);

    main_widget_ = new QWidget(this);
    keyboard_ = new Keyboard(this);
    mod_var_window_ = new ModVarWindow(this);

    window_layout->addWidget(main_widget_);
    window_layout->addWidget(keyboard_);
    window_layout->addWidget(mod_var_window_);
    window_layout->setContentsMargins(0, 0, 0, 0);
    window_layout->setSpacing(0);
    this->setLayout(window_layout);

    mod_var_window_->head_widget_->hide();
    mod_var_window_->hide();

    QVBoxLayout *layout = new QVBoxLayout(main_widget_);

    QVBoxLayout *wid_layout = new QVBoxLayout;

    QHBoxLayout *tips_layout = new QHBoxLayout;
    tips_type_label_ = new QLabel(tr("数据类型:"), main_widget_);
    type_name_label_ = new QLabel(main_widget_);
    tips_task_label_ = new QLabel(tr("当前任务:"), main_widget_);
    task_name_label_ = new QLabel(main_widget_);
    tips_layout->addWidget(tips_type_label_);
    tips_layout->addWidget(type_name_label_);
    tips_layout->addWidget(tips_task_label_);
    tips_layout->addWidget(task_name_label_);
    tips_layout->setContentsMargins(0, 0, 0, 0);
    tips_layout->setSpacing(0);

    QHBoxLayout *name_layout = new QHBoxLayout;
    name_label_ = new QLabel(tr("名称:"), main_widget_);
    name_edit_ = new QLineEdit(main_widget_);
    name_key_button_ = new QPushButton("...", main_widget_);
    name_label_->setAlignment(Qt::AlignCenter);
    name_layout->addStretch();
    name_layout->addWidget(name_label_);
    name_layout->addWidget(name_edit_);
    name_layout->addWidget(name_key_button_);
    name_layout->setContentsMargins(0, 0, 0, 0);
    name_layout->setSpacing(0);
    name_layout->setStretchFactor(name_label_, 2);
    name_layout->setStretchFactor(name_edit_, 4);
    name_layout->setStretchFactor(name_key_button_, 1);

    QHBoxLayout *range_layout = new QHBoxLayout;
    range_label_ = new QLabel(tr("范围:"), main_widget_);
    range_box_ = new QComboBox(main_widget_);
    range_label_->setAlignment(Qt::AlignCenter);
    range_layout->addWidget(range_label_);
    range_layout->addWidget(range_box_);
    range_layout->setContentsMargins(0, 0, 0, 0);
    range_layout->setSpacing(0);
    range_layout->setStretchFactor(range_label_, 2);
    range_layout->setStretchFactor(range_box_, 5);

    QHBoxLayout *type_layout = new QHBoxLayout;
    type_label_ = new QLabel(tr("存储类型:"), main_widget_);
    type_box_ = new QComboBox(main_widget_);
    type_label_->setAlignment(Qt::AlignCenter);
    type_layout->addWidget(type_label_);
    type_layout->addWidget(type_box_);
    type_layout->setContentsMargins(0, 0, 0, 0);
    type_layout->setSpacing(0);
    type_layout->setStretchFactor(type_label_, 2);
    type_layout->setStretchFactor(type_box_, 5);

    QHBoxLayout *task_layout = new QHBoxLayout;
    task_label_ = new QLabel(tr("任务:"), main_widget_);
    task_box_ = new QComboBox(main_widget_);
    task_label_->setAlignment(Qt::AlignCenter);
    task_layout->addWidget(task_label_);
    task_layout->addWidget(task_box_);
    task_layout->setContentsMargins(0, 0, 0, 0);
    task_layout->setSpacing(0);
    task_layout->setStretchFactor(task_label_, 2);
    task_layout->setStretchFactor(task_box_, 5);

    QHBoxLayout *mod_layout = new QHBoxLayout;
    mod_label_ = new QLabel(tr("模块:"), main_widget_);
    mod_box_ = new QComboBox(main_widget_);
    mod_label_->setAlignment(Qt::AlignCenter);
    mod_layout->addWidget(mod_label_);
    mod_layout->addWidget(mod_box_);
    mod_layout->setContentsMargins(0, 0, 0, 0);
    mod_layout->setSpacing(0);
    mod_layout->setStretchFactor(mod_label_, 2);
    mod_layout->setStretchFactor(mod_box_, 5);

    QHBoxLayout *routine_layout = new QHBoxLayout;
    routine_label_ = new QLabel(tr("例行程序:"), main_widget_);
    routine_box_ = new QComboBox(main_widget_);
    routine_label_->setAlignment(Qt::AlignCenter);
    routine_layout->addWidget(routine_label_);
    routine_layout->addWidget(routine_box_);
    routine_layout->setContentsMargins(0, 0, 0, 0);
    routine_layout->setSpacing(0);
    routine_layout->setStretchFactor(routine_label_, 2);
    routine_layout->setStretchFactor(routine_box_, 5);

    QHBoxLayout *dimen_layout = new QHBoxLayout;
    dimen_label_ = new QLabel(tr("维数:"), main_widget_);
    dimen_box_ = new QComboBox(main_widget_);
    dimen_edit_ = new QLineEdit(main_widget_);
    dimen_key_button_ = new QPushButton("...", main_widget_);
    dimen_label_->setAlignment(Qt::AlignCenter);
    dimen_layout->addWidget(dimen_label_);
    dimen_layout->addWidget(dimen_box_);
    dimen_layout->addWidget(dimen_edit_);
    dimen_layout->addWidget(dimen_key_button_);
    dimen_layout->setStretchFactor(dimen_label_, 2);
    dimen_layout->setStretchFactor(dimen_box_, 2);
    dimen_layout->setStretchFactor(dimen_edit_, 2);
    dimen_layout->setStretchFactor(dimen_key_button_, 1);

    button_widget_ = new QWidget(main_widget_);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);
    init_var_button_ = new QPushButton(tr("初始值"), button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    init_var_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    init_var_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addWidget(init_var_button_);
    button_layout->addStretch();
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);
    button_widget_->setLayout(button_layout);

    wid_layout->addStretch();
    wid_layout->addLayout(tips_layout);
    wid_layout->addStretch();
    wid_layout->addLayout(name_layout);
    wid_layout->addStretch();
    wid_layout->addLayout(range_layout);
    wid_layout->addStretch();
    wid_layout->addLayout(type_layout);
    wid_layout->addStretch();
    wid_layout->addLayout(task_layout);
    wid_layout->addStretch();
    wid_layout->addLayout(mod_layout);
    wid_layout->addStretch();
    wid_layout->addLayout(routine_layout);
    wid_layout->addStretch();
    wid_layout->addLayout(dimen_layout);
    wid_layout->setContentsMargins(0, 0, 0, 0);

    layout->addLayout(wid_layout);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->setStretchFactor(wid_layout, 5);
    layout->setStretchFactor(button_widget_, 1);
    main_widget_->setLayout(layout);

    dimen_box_->addItem("NULL");
    dimen_box_->addItem("1");
    dimen_box_->addItem("2");
    dimen_box_->addItem("3");

    array_size_ = new ArraySize(this);
    array_size_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    ConnectInit();
}

void CreateNewVar::Init(QString type, QString mod)
{
    /*
     VAR : 变量
     VARc : 不可修改
     PERS : 可变量
     CONST : 常量

     ALL : 全局
     TASK : 任务
    */
    type_name_label_->setText(type);
    Init(mod); // VAR ALL
    init_var_ = VarInfo::instance()->GetForms(type);

    name_edit_->setText(VarInfo::instance()->GetUnused(type));

    if(type == "clock" || type == "signalai" || type == "signalao" || type == "signaldi" || type == "signaldo") //VARc
    {
        type_box_->setEnabled(false);
        init_var_button_->hide();
    }
    else if(type == "loaddata" || type == "tooldata" || type == "wobjdata") //TASK PERS
    {
        range_box_->setCurrentIndex(1);
        type_box_->setCurrentIndex(1);
        routine_box_->setEnabled(false);

        if(type == "tooldata")
            init_var_ = "[TRUE,[[0,0,0],[1,0,0,0]],[-1,[0,0,0],[1,0,0,0],0,0,0]]";
        else if(type == "wobjdata")
            init_var_ = "[FALSE,TRUE,"",[[0,0,0],[1,0,0,0]],[[0,0,0],[1,0,0,0]]]";
        else if(type == "loaddata")
            init_var_ = "[0,[0,0,0],[1,0,0,0],0,0,0]";
    }
    else if(type == "pose") //TASK
    {
        type_box_->setCurrentIndex(1);
        routine_box_->setEnabled(false);
    }
    else if(type == "robtarget" || type == "speeddata" || type == "string" || type == "zonedata") //CONST
    {
        type_box_->setCurrentIndex(2);
    }
}

void CreateNewVar::SetScale(double scale_w, double scale_h)
{
    keyboard_->SetScale(scale_w, scale_h);
    mod_var_window_->SetScale(scale_w, scale_h);

    if(!array_size_->isHidden())
    {
        array_size_->resize(this->size());
        array_size_->move(0, 0);
    }
    array_size_->SetScale(scale_w, scale_h);
}

void CreateNewVar::ConnectInit()
{
    connect(type_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            [=](int index)
    {
        range_box_->setEnabled(true);
        if(index == 1) //可变量
        {
            routine_box_->setCurrentIndex(0);
            routine_box_->setEnabled(false);

            if(range_box_->count() == 2 && range_box_->count() == 2)
            {
                range_box_->addItem(tr("本地"));
                range_box_->setCurrentIndex(0);
            }

            return;
        }
        else if(index == 2) //常量
        {
            if(range_box_->count() == 3)
            {
                range_box_->removeItem(range_box_->count() - 1);
                range_box_->setCurrentIndex(0);
            }
        }
        else //变量
        {
            if(range_box_->count() == 2)
            {
                range_box_->addItem(tr("本地"));
                range_box_->setCurrentIndex(0);
            }

        }

        if(range_box_->currentText() != tr("任务"))
            routine_box_->setEnabled(true);
    });

    connect(range_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            [=]()
    {
        if(type_box_->currentIndex() == 1)
            return;

        if(range_box_->currentText() == tr("任务")) //任务
        {
            routine_box_->setEnabled(false);
        }
        else
        {
            routine_box_->setEnabled(true);
        }
    });

    connect(mod_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            [=](int)
    {
        QString mod_name = mod_box_->currentText();
        QStringList rout_list = EditData::instance()->GetRoutName(mod_name);

        routine_box_->clear();

        rout_list.push_front("NULL");

        routine_box_->addItems(rout_list);
    });

    connect(routine_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            [=](int index)
    {
        if(index == 0) //无
        {
            range_box_->setCurrentIndex(0);
            range_box_->setEnabled(true);
        }
        else
        {
            range_box_->setCurrentIndex(1);
            range_box_->setEnabled(false);
        }
    });

    connect(dimen_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            [=](int index)
    {
        if(index == 0) //无
        {
            dimen_edit_->clear();
            dimen_edit_->setEnabled(false);
            dimen_key_button_->setEnabled(false);
            init_var_button_->show();
            return;
        }
        else if(index == 1)
        {
            dimen_edit_->setText("{1}");
        }
        else if(index == 2)
        {
            dimen_edit_->setText("{1,1}");
        }
        else
        {
            dimen_edit_->setText("{1,1,1}");
        }

        dimen_edit_->setEnabled(true);
        dimen_key_button_->setEnabled(true);
        init_var_button_->hide();
    });

    connect(name_key_button_, &QPushButton::clicked,
            [=]()
    {
        keyboard_->Init(name_edit_->text());

        main_widget_->hide();
        keyboard_->show();
    });

    connect(keyboard_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        keyboard_->hide();
        main_widget_->show();
    });

    connect(keyboard_, &Keyboard::GetData,
            [=](QString data)
    {
        if(data.isEmpty())
        {
            QMessageBox::warning(this, tr("提示"), tr("请输入变量名称"));
            return;
        }

        name_edit_->setText(data);

        keyboard_->hide();
        main_widget_->show();
    });

    connect(init_var_button_, &QPushButton::clicked,
            [=]()
    {
        main_widget_->hide();

        mod_var_window_->Init(type_name_label_->text(), name_edit_->text());
        mod_var_window_->show();
    });

    connect(mod_var_window_, &ModVarWindow::Cancel,
            [=]()
    {
        mod_var_window_->hide();

        main_widget_->show();
    });

    connect(mod_var_window_, &ModVarWindow::Define,
            [=](QString code)
    {
        init_var_ = code;

        mod_var_window_->hide();
        main_widget_->show();
    });

    connect(define_button_, &QPushButton::clicked, this, &CreateNewVar::GetVar);

    connect(dimen_key_button_, &QPushButton::clicked, this, &CreateNewVar::ShowArraySize);

    connect(array_size_, &ArraySize::Define, this, &CreateNewVar::ArraySizeDim);

    connect(array_size_, &ArraySize::Cancel, array_size_, &ArraySize::hide);
}

void CreateNewVar::GetVar()
{
    VarDataInfo var_info("");
    QString var_code;

    QString var_name = name_edit_->text();
    if(var_name.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请输入变量名"));
        return;
    }
    if(var_name.indexOf(QRegExp("^[A-Za-z0-9]+$")) == -1)
    {
        QMessageBox::warning(this, tr("提示"), tr("您输入的变量名格式有误"));
        return;
    }
    bool isint;
    var_name.toInt(&isint);
    if(isint)
    {
        QMessageBox::warning(this, tr("提示"), tr("您输入的变量名格式有误"));
        return;
    }

    auto range_index = range_box_->currentText();
    if(range_index == tr("全局"))
    {
        var_info.action_scope = "all";
    }
    else if(range_index == tr("本地"))
    {
        QString temp = routine_box_->currentText();

        if(temp == "NULL")
            var_info.action_scope = "local";
        else
            var_info.action_scope = temp;
    }
    else //任务
    {
        var_info.action_scope = "task";
    }
    var_info.data_type = type_name_label_->text();
    var_info.type = var_info.data_type;
    var_info.nature = type_box_->currentIndex();
    var_info.genre = true;
    var_info.mod = mod_box_->currentText();
    var_info.dim = dimen_edit_->text();

    if(var_info.action_scope == "local")
        var_code.append("LOCAL ");

    switch (var_info.nature)
    {
    case 0:
    {
        var_code.append("VAR ");
        break;
    }
    case 1:
    {
        var_code.append("PERS ");
        break;
    }
    case 2:
    {
        var_code.append("CONST ");
        break;
    }
    default:
        break;
    }

    var_code.append(var_info.data_type + " ");
    var_code.append(var_name);

    if(dimen_box_->currentText() == "NULL")
    {
        var_info.var = init_var_;
        var_info.init_var = init_var_;

        if(init_var_.isEmpty())
            var_code.append(";");
        else
            var_code.append(":=" + var_info.var.toString() + ";");
    }
    else
    {
        var_info.var = VarInfo::instance()->GetVar(var_info.data_type, var_info.dim);
        var_info.init_var = var_info.var;

        var_code.append(var_info.dim + ":=" + var_info.var.toString() + ";");
    }

    if(VarInfo::instance()->IsExist(var_name, var_info))
    {
        QMessageBox::warning(this, tr("提示"), tr("该变量已存在"));
        return;
    }

    if(var_info.data_type == "tooldata" || var_info.data_type == "wobjdata")
    {
        QString code = var_info.var.toString();

        QList<double> xyz;
        Quaternion qn;
        Euler er;
        int isok;

        if(var_info.data_type == "tooldata") //工具坐标
        {
            auto too = Convert::DataTypeToVarFormData("tooldata", code);

            xyz = Convert::XYZCodeToDoubles(too->subnode.at(1)->subnode.at(0)->code);
            if(xyz.isEmpty())
            {
                QMessageBox::warning(this, tr("提示"), tr("该坐标的值出现问题"));
                return;
            }
            qn.SetData(too->subnode.at(1)->subnode.at(1)->code);
            er = Convert::QuaternionToEuler(qn);
            rclib::Terminal tel(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

            rclib::RobotFrame frame = rclib::RobotClient::initance()->getRobotFrame();
            isok = frame.addToolFrame(var_name.toStdString(), tel);
            rclib::RobotClient::initance()->setRobotFrame(frame);
        }
        else //工件坐标
        {
            auto too = Convert::DataTypeToVarFormData("wobjdata", code);

            xyz = Convert::XYZCodeToDoubles(too->subnode.at(3)->subnode.at(0)->code);
            if(xyz.isEmpty())
            {
                QMessageBox::warning(this, tr("提示"), tr("该坐标的值出现问题"));
                return;
            }
            qn.SetData(too->subnode.at(3)->subnode.at(1)->code);
            er = Convert::QuaternionToEuler(qn);
            rclib::Terminal tel(xyz.at(0), xyz.at(1), xyz.at(2), er.roll, er.pitch, er.yaw);

            rclib::RobotFrame frame = rclib::RobotClient::initance()->getRobotFrame();
            isok = frame.addWorkFrame(var_name.toStdString(), tel);
            rclib::RobotClient::initance()->setRobotFrame(frame);
        }

        if(isok != 0)
        {
            QMessageBox::warning(this, tr("提示"), tr("该坐标已存在"));
            return;
        }
    }

    var_info.var_name = var_name;
    VarInfo::instance()->AddVar(var_name, var_info);

    if(var_info.action_scope == "all" || var_info.action_scope == "local" || var_info.action_scope == "task")
    {
        EditData::instance()->ModAddCode(var_info.mod, var_code);
    }
    else
    {
        EditData::instance()->RoutAddCode(var_info.action_scope, 0, var_code);
    }

    emit CreateVar(mod_box_->currentText());
}

void CreateNewVar::ShowArraySize()
{
    array_size_->resize(this->size());
    array_size_->move(0, 0);
    array_size_->show();
    array_size_->Init(name_edit_->text(), dimen_box_->currentIndex());
}

void CreateNewVar::ArraySizeDim(QString dim_str)
{
    dimen_edit_->setText(dim_str);

    array_size_->hide();
}

void CreateNewVar::Init(QString &mod)
{
    range_box_->clear();
    type_box_->clear();
    mod_box_->clear();
    routine_box_->clear();
    task_box_->clear();
    dimen_box_->setCurrentIndex(0);
    dimen_edit_->clear();
    dimen_edit_->setEnabled(false);
    dimen_key_button_->setEnabled(false);
    task_name_label_->setText(EditData::instance()->GetTaskName());

    name_edit_->clear();

    range_box_->addItem(tr("全局"));
    range_box_->addItem(tr("任务"));
    range_box_->addItem(tr("本地"));

    task_box_->addItem(TeachState::instance()->GetTaskName());

    type_box_->addItem(tr("变量"));
    type_box_->addItem(tr("可变量"));
    type_box_->addItem(tr("常量"));

    auto mod_list = EditData::instance()->GetModName(false);
    mod_box_->addItems(mod_list);
    mod_box_->setCurrentText(mod);

    auto rout_list = EditData::instance()->GetRoutName(mod);
    routine_box_->addItem("NULL");
    routine_box_->addItems(rout_list);
    routine_box_->setCurrentIndex(0);

    init_var_.clear();
    init_var_button_->show();
    range_box_->setEnabled(true);
    routine_box_->setEnabled(true);
    type_box_->setEnabled(true);
}

double sin_deg(double angle)
{
    auto pi = 3.14159265359;

    return sin(angle * pi / 180);
}

double cos_deg(double angle)
{
    auto pi = 3.14159265359;

    return cos(angle * pi / 180);
}

double tan_deg(double angle)
{
    auto pi = 3.14159265359;

    return tan(angle * pi / 180);
}

double acos_deg(double value)
{
    auto pi = 3.14159265359;

    return acos(value) * 180 / pi;
}

double atan2_deg(double y, double x)
{
    auto pi = 3.14159265359;

    return atan2(y, x) * 180 / pi;
}

Quaternion Convert::EulerToQuaternion(Euler &er)
{
    double alpha = er.roll;
    double beta = er.pitch;
    double gamma = er.yaw;

    double r11 = cos_deg(alpha)*cos_deg(beta);
    double r21 = sin_deg(alpha)*cos_deg(beta);
    double r31 = -sin_deg(beta);
    double r12 = cos_deg(alpha)*sin_deg(beta)*sin_deg(gamma)-sin_deg(alpha)*cos_deg(gamma);
    double r22 = sin_deg(alpha)*sin_deg(beta)*sin_deg(gamma)+cos_deg(alpha)*cos_deg(gamma);
    double r32 = cos_deg(beta)*sin_deg(gamma);
    double r13 = cos_deg(alpha)*sin_deg(beta)*cos_deg(gamma)+sin_deg(alpha)*sin_deg(gamma);
    double r23 = sin_deg(alpha)*sin_deg(beta)*cos_deg(gamma)-cos_deg(alpha)*sin_deg(gamma);
    double r33 = cos_deg(beta)*cos_deg(gamma);

    double q0 = (1 + r11 + r22 + r33)/4;
    double q1 = (1 + r11 - r22 - r33)/4;
    double q2 = (1 - r11 + r22 - r33)/4;
    double q3 = (1 - r11 - r22 + r33)/4;
    if(q0 >= (q1-EPSLON) && q0 >= (q2-EPSLON) && q0 >= (q3-EPSLON))
    {
        q0 = sqrt(q0);
        q1 = (r32 - r23)/(4*q0);
        q2 = (r13 - r31)/(4*q0);
        q3 = (r21 - r12)/(4*q0);
    }
    else if(q1 >= (q0-EPSLON) && q1 >= (q2-EPSLON) && q1 >= (q3-EPSLON))
    {
        q1 = sqrt(q1);
        q0 = (r32 - r23)/(4*q1);
        q2 = (r21 + r12)/(4*q1);
        q3 = (r13 + r31)/(4*q1);
    }
    else if(q2 >= (q0-EPSLON) && q2 >= (q1-EPSLON) && q2 >= (q3-EPSLON))
    {
        q2 = sqrt(q2);
        q0 = (r13 - r31)/(4*q2);
        q1 = (r21 + r12)/(4*q2);
        q3 = (r32 + r23)/(4*q2);
    }
    else if(q3 >= (q0-EPSLON) && q3 >= (q1-EPSLON) && q3 >= (q2-EPSLON))
    {
        q3 = sqrt(q3);
        q0 = (r21 - r12)/(4*q3);
        q1 = (r13 + r31)/(4*q3);
        q2 = (r32 + r23)/(4*q3);
    }

    Quaternion qt;
    qt.w = q0;
    qt.x = q1;
    qt.y = q2;
    qt.z = q3;

    return qt;
}

bool num_is_zero(double num)
{
    if(num < EPSLON && num > -EPSLON){
        return true;
    }
    else{
        return false;
    }
}

Euler Convert::QuaternionToEuler(Quaternion &qn)
{
    double q0 = qn.w;
    double q1 = qn.x;
    double q2 = qn.y;
    double q3 = qn.z;

    double r11 = 1-2*q2*q2-2*q3*q3;
    double r21 = 2*q1*q2+2*q0*q3;
    double r31 = 2*q1*q3-2*q0*q2;
    double r12 = 2*q1*q2-2*q0*q3;
    double r22 = 1-2*q1*q1-2*q3*q3;
    double r32 = 2*q2*q3+2*q0*q1;
    double r13 = 2*q1*q3+2*q0*q2;
    double r23 = 2*q2*q3-2*q0*q1;
    double r33 = 1-2*q1*q1-2*q2*q2;

    double alpha, beta, gamma;

    double cosbeta = sqrt(r11*r11+r21*r21);
    if(num_is_zero(cosbeta))
    {
        alpha = 0;
        if(r31 > 0){
            beta = -180/2;
            gamma = -atan2_deg(r12, r22);
        }
        else{
            beta = 180/2;
            gamma = atan2_deg(r12, r22);
        }
    }
    else
    {
        beta = atan2_deg(-r31, cosbeta);
        alpha = atan2_deg(r21/cosbeta, r11/cosbeta);
        gamma = atan2_deg(r32/cosbeta, r33/cosbeta);
    }

    Euler er;
    er.roll = alpha;
    er.pitch = beta;
    er.yaw = gamma;

    return er;
}

double Convert::AngleToRadian(double angle)
{
    auto mpi = 3.14159265358979323846;
    double radian = angle * mpi / 180;

    return radian;
}

double Convert::RadianToAngle(double radian)
{
    auto mpi = 3.14159265358979323846;
    double angle = radian * 180 / mpi;

    return angle;
}

QList<double> Convert::XYZCodeToDoubles(QString code)
{
    QString tmp_code = code;
    tmp_code.replace("[", "").replace("]", "");

    QStringList list = tmp_code.split(",");
    if(list.size() != 3)
        return QList<double>();

    QList<double> dblist;

    bool isokx, isoky, isokz;
    dblist.append(list.at(0).toDouble(&isokx));
    dblist.append(list.at(1).toDouble(&isoky));
    dblist.append(list.at(2).toDouble(&isokz));

    if(!isokx || !isoky || !isokz)
        return QList<double>();
    else
        return dblist;
}

QString Convert::PositDoubleToPositCode(double x, double y, double z, Euler &er)
{
    Quaternion qn = EulerToQuaternion(er);

    QString code = QString("[[%1,%2,%3],[%4,%5,%6,%7]]").arg(x).arg(y).arg(z).arg(qn.w).arg(qn.x).arg(qn.y).arg(qn.z);

    return code;
}

QSharedPointer<VarFormData> Convert::DataTypeToVarFormData(QString data_type, QString code)
{
    if(data_type.isEmpty() || !DataType::instance()->IsExist(data_type))
        return QSharedPointer<VarFormData>();

    data_type.replace(" ", "");

    auto form = DataType::instance()->GetForms(data_type);

    auto pointer = QSharedPointer<VarFormData>(new VarFormData);
    pointer->type = data_type;

    if(form.isNull()) //基础类型
    {
        if(code.isEmpty())
        {
            if(data_type == "num")
                pointer->code = "0";
            else if(data_type == "bool")
                pointer->code = "FALSE";
            else
                pointer->code = "\"\"";
        }
        else
        {
            pointer->code = code;
        }

        return pointer;
    }
    else //复合类型
    {
        QString tmp_code = "[";
        for(int i = 0; i < form->size(); i++)
        {
            auto type = form->at(i).second;
            auto sub = GetForms(type);

            pointer->subnode.append(sub);
            sub->parent = pointer.toWeakRef();
            sub->name = form->at(i).first;
            sub->type = type;

            tmp_code.append(sub->code);
            if(i + 1 < form->size())
                tmp_code.append(",");
        }
        tmp_code.append("]");
        pointer->code = tmp_code;
        pointer->type = data_type;
    }

    code.replace(";", "").replace(" ", "");
    if(code.isEmpty())
        return pointer;

    pointer->code = code;
    pointer->LastUpdate();

    return pointer;
}

QSharedPointer<VarFormData> Convert::GetForms(QString data_type)
{
    auto form = DataType::instance()->GetForms(data_type);

    auto pointer = QSharedPointer<VarFormData>(new VarFormData);

    if(form.isNull())
    {
        if(data_type == "num")
        {
            pointer->code = "0";
        }
        else if(data_type == "string")
        {
            pointer->code = "\"\"";
        }
        else if(data_type == "bool")
        {
            pointer->code = "FALSE";
        }

        return pointer;
    }

    QString code = "[";
    for(int i = 0; i < form->size(); i++)
    {
        auto sub = GetForms(form->at(i).second);
        pointer->subnode.append(sub);
        sub->parent = pointer.toWeakRef();

        sub->name = form->at(i).first;
        sub->type = form->at(i).second;

        code.append(sub->code);
        if(i + 1 < form->size())
            code.append(",");
    }
    code.append("]");
    pointer->code = code;

    return pointer;
}

Euler::Euler()
{
    roll = 0.0;
    pitch = 0.0;
    yaw = 0.0;
}

Euler::Euler(double x, double y, double z)
{
    SetData(x, y, z);
}

void Euler::SetData(double x, double y, double z)
{
    roll = x;
    pitch = y;
    yaw = z;
}

Quaternion Euler::GetQuaternion()
{
    Quaternion qn;
    return qn;
}

VarFormData::VarFormData(QString nm, QString cd, QString tp, QWeakPointer<VarFormData> pn)
{
    name = nm;
    code = cd;
    type = tp;
    parent = pn;
}

void VarFormData::Update()
{
    if(subnode.isEmpty())
        return;

    code = "[";

    for(int i = 0; i < subnode.size(); i++)
    {
        auto sub = subnode.at(i);

        sub->Update();

        code.append(sub->code);
        if(i + 1 < subnode.size())
            code.append(",");
    }
    code.append("]");
}

void VarFormData::LastUpdate()
{
    if(subnode.isEmpty())
        return;

    QString str = code.mid(1, code.size() - 2);

    int bk_num = 0;
    int sub_num = 0;
    QString tmp_str;
    for(int i = 0; i < str.size(); i++)
    {
        QString s = str.at(i);

        if(s == "[")
        {
            tmp_str.append(s);
            bk_num++;
        }
        else if(s == "]")
        {
            tmp_str.append(s);
            bk_num--;

            if(bk_num == 0 && i + 1 >= str.size())
            {
                auto sub = subnode.at(sub_num++);
                sub->code = tmp_str;
                tmp_str.clear();
                sub->LastUpdate();
            }
        }
        else if(s == ",")
        {
            if(bk_num != 0)
            {
                tmp_str.append(s);
            }
            else
            {
                auto sub = subnode.at(sub_num++);
                sub->code = tmp_str;
                tmp_str.clear();
                sub->LastUpdate();
            }
        }
        else
        {
            tmp_str.append(s);
        }
    }
    if(!tmp_str.isEmpty() && sub_num < subnode.size())
    {
        auto sub = subnode.at(sub_num);
        sub->code = tmp_str;
        tmp_str.clear();
        sub->LastUpdate();
    }
}

Quaternion::Quaternion()
{
    w = 0.0;
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

Quaternion::Quaternion(QString code)
{
    SetData(code);
}

void Quaternion::SetData(QString code)
{
    QString tmp_code = code;
    tmp_code.replace("[", "").replace("]", "");

    QStringList list = tmp_code.split(",");
    if(list.size() != 4)
        return;

    bool isok1, isok2, isok3, isok4;
    double tmp1 = list.at(0).toDouble(&isok1);
    double tmp2 = list.at(1).toDouble(&isok2);
    double tmp3 = list.at(2).toDouble(&isok3);
    double tmp4 = list.at(3).toDouble(&isok4);

    if(!isok1 || !isok2 || !isok3 || !isok4)
        return;

    w = tmp1;
    x = tmp2;
    y = tmp3;
    z = tmp4;
}

VarDataInfo::VarDataInfo()
{
}

VarDataInfo::VarDataInfo(QString name)
{
    var_name = name;
}

void VarDataInfo::SetVar(QVariant newvar)
{
    QString code;

    code = VarInfo::instance()->GetCode(*this, var_name); //获取本变量所对应的代码

    if(code.isEmpty())
        return;

    auto vardata = Convert::DataTypeToVarFormData(data_type, code.section(":=", 1, 1));

    if(data_type == "wobjdata")
    {
        auto sub = vardata->subnode.at(2);
        sub->code = newvar.toString();
        sub->LastUpdate();
    }
    else if(data_type == "tooldata")
    {
        auto sub = vardata->subnode.at(1);
        sub->code = newvar.toString();
        sub->LastUpdate();
    }

    vardata->Update();

    var = newvar;

    ModuleData &mod_data = EditData::instance()->GetModData(mod);

    for(int i = 0; i < mod_data.mod_code.size(); i++)
    {
        if(mod_data.mod_code.at(i) == code)
        {
            QString newcode = code.section(":=", 0, 0) + ":=" + vardata->code + ";";

            mod_data.mod_code[i] = newcode;
            EditData::instance()->ModDataMid(mod);
            return;
        }
    }

    for (int i = 0; i < mod_data.routine_data[action_scope].code.size(); ++i)
    {
        if(mod_data.routine_data[action_scope].code.at(i) == code)
        {
            QString newcode = code.section(":=", 0, 0) + ":=" + vardata->code + ";";

            mod_data.routine_data[action_scope].code[i] = newcode;
            EditData::instance()->ModDataMid(mod);
            return;
        }
    }
}

void VarDataInfo::SetVar(QSharedPointer<VarFormData> pointer)
{
    QString code;

    code = VarInfo::instance()->GetCode(*this, var_name); //获取本变量所对应的代码

    if(code.isEmpty())
        return;

    var = QVariant(pointer->code);

    ModuleData &mod_data = EditData::instance()->GetModData(mod);

    for(int i = 0; i < mod_data.mod_code.size(); i++)
    {
        if(mod_data.mod_code.at(i) == code)
        {
            QString newcode = code.section(":=", 0, 0) + ":=" + pointer->code + ";";

            mod_data.mod_code[i] = newcode;
            EditData::instance()->ModDataMid(mod);
            return;
        }
    }

    for (int i = 0; i < mod_data.routine_data[action_scope].code.size(); ++i)
    {
        if(mod_data.routine_data[action_scope].code.at(i) == code)
        {
            QString newcode = code.section(":=", 0, 0) + ":=" + pointer->code + ";";

            mod_data.routine_data[action_scope].code[i] = newcode;
            EditData::instance()->ModDataMid(mod);
            return;
        }
    }
}

ModVarWindow::ModVarWindow(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    head_widget_ = new QWidget(this);
    QVBoxLayout *head_layout = new QVBoxLayout(head_widget_);

    QHBoxLayout *label_layout = new QHBoxLayout;
    ico_label_ = new QLabel(head_widget_);
    title_label_ = new QLabel(tr("编辑"), head_widget_);
    ico_label_->setStyleSheet("border-image: url(:/image/Image/MainWidget/program_data.png)");
    label_layout->addWidget(ico_label_);
    label_layout->addWidget(title_label_);
    label_layout->setContentsMargins(0, 0, 0, 0);
    label_layout->setSpacing(0);
    label_layout->setStretchFactor(ico_label_, 1);
    label_layout->setStretchFactor(title_label_, 15);

    QLabel *label_test = new QLabel(head_widget_);
    label_test->setFrameShape(QFrame::HLine);
    head_layout->addLayout(label_layout);
    head_layout->addWidget(label_test);
    head_layout->setContentsMargins(0, 0, 0, 0);
    head_layout->setSpacing(0);

    QGridLayout *tips_layout = new QGridLayout;
    tips_name_label_ = new QLabel(tr("名称:"), this);
    name_label_ = new QLabel(this);
    tips_label_ = new QLabel(tr("点击一个字段以编辑值。"), this);
    tips_layout->addWidget(tips_name_label_, 0, 0);
    tips_layout->addWidget(name_label_, 0, 1);
    tips_layout->addWidget(tips_label_, 1, 0);
    tips_layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *list_layout_ = new QVBoxLayout;

    QHBoxLayout *list_label_layout = new QHBoxLayout;
    list_name_label_ = new QLabel(tr("名称"), this);
    list_val_label_ = new QLabel(tr("值"), this);
    list_type_label_ = new QLabel(tr("类型"), this);
    list_name_label_->setFrameShape(QFrame::Box);
    list_val_label_->setFrameShape(QFrame::Box);
    list_type_label_->setFrameShape(QFrame::Box);
    list_label_layout->addWidget(list_name_label_);
    list_label_layout->addWidget(list_val_label_);
    list_label_layout->addWidget(list_type_label_);
    list_label_layout->setContentsMargins(0, 0, 0, 0);
    list_label_layout->setSpacing(0);

    list_box_ = new ListBoxObject(this);
    list_layout_->addLayout(list_label_layout);
    list_layout_->addWidget(list_box_);
    list_layout_->setContentsMargins(0, 0, 0, 0);
    list_layout_->setSpacing(0);

    button_widget_ = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_widget_);

    true_button_ = new QPushButton("TRUE", button_widget_);
    false_button_ = new QPushButton("FALSE", button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    true_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    false_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    true_button_->setFlat(true);
    false_button_->setFlat(true);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    button_layout->addWidget(true_button_);
    button_layout->addWidget(false_button_);
    button_layout->addStretch();
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    layout->addWidget(head_widget_);
    layout->addLayout(tips_layout);
    layout->addLayout(list_layout_);
    layout->addWidget(button_widget_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(head_widget_, 1);
    layout->setStretchFactor(tips_layout, 4);
    layout->setStretchFactor(list_layout_, 9);
    layout->setStretchFactor(button_widget_, 2);

    scale_w_ = 1;
    scale_h_ = 1;

    keyboard_ = new Keyboard(this);
    num_key_ = new NumKey(this);

    keyboard_->hide();
    num_key_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    list_name_label_->setAutoFillBackground(true);
    list_val_label_->setAutoFillBackground(true);
    list_type_label_->setAutoFillBackground(true);
    button_widget_->setPalette(p);
    list_name_label_->setPalette(p);
    list_val_label_->setPalette(p);
    list_type_label_->setPalette(p);

    ConnectInit();
}

void ModVarWindow::HideTitle()
{
    head_widget_->hide();
}

void ModVarWindow::Init(QString var_type, QString var_name)
{
    name_label_->setText(var_name);

    varform_pointer_ = Convert::DataTypeToVarFormData(var_type);
    pointer_ = varform_pointer_;
    clicked_item_.clear();

    true_button_->hide();
    false_button_->hide();

    mod_name_.clear();
    action_scope_.clear();

    isstatic_ = false;

    AddListItem(pointer_);
}

void ModVarWindow::Init(QWeakPointer<VarFormData> point, QString var_name, QString mod_name, QString action_scope, bool is_static)
{
    name_label_->setText(var_name);

    pointer_ = point;
    clicked_item_.clear();

    true_button_->hide();
    false_button_->hide();

    mod_name_ = mod_name;
    action_scope_ = action_scope;

    isstatic_ = is_static;

    AddListItem(pointer_);
}

void ModVarWindow::SetScale(double scale_w, double scale_h)
{
    keyboard_->SetScale(scale_w, scale_h);

    AddListItem(pointer_);
    clicked_item_.clear();

    if(!true_button_->isHidden())
    {
        true_button_->hide();
        false_button_->hide();
    }

    scale_w_ = scale_w;
    scale_h_ = scale_h;

    if(!num_key_->isHidden())
    {
        num_key_->resize(200*scale_w, 260*scale_h);
        num_key_->move(this->width() - num_key_->width(), button_widget_->y() - num_key_->height());
        num_key_->Clear();
        num_key_->hide();
    }
}

void ModVarWindow::AddListItem(QWeakPointer<VarFormData> pointer)
{
    if(list_box_->count() != 0)
        list_box_->clear();

    this->AddListItem(pointer, 0);
}

void ModVarWindow::DefineClicked()
{
    if(!num_key_->isHidden())
    {
        num_key_->hide();
    }

    if(isstatic_)
    {
        cancel_button_->click();
        return;
    }

    if(varform_pointer_.isNull())
    {
        QString var_name = name_label_->text();
        if(var_name != "*")
        {
            auto &info = VarInfo::instance()->GetVarInfo(var_name, mod_name_, action_scope_);
            info.SetVar(pointer_.lock());
        }

        emit Defineed();
    }
    else
    {
        emit Define(pointer_.lock()->code);
    }

    if(!clicked_item_.isNull())
        clicked_item_->HideEdit(false);
    if(!varform_pointer_.isNull())
        varform_pointer_.clear();
}

void ModVarWindow::CancelClicked()
{
    if(!num_key_->isHidden())
    {
        num_key_->hide();
    }

    if(!clicked_item_.isNull())
        clicked_item_->HideEdit(false);
    if(!varform_pointer_.isNull())
        varform_pointer_.clear();

    emit Cancel();
}

void ModVarWindow::KeyboardGetData(QString data)
{
    if(clicked_item_.isNull())
    {
        keyboard_->hide();
        return;
    }

    QString val = "\"" + data + "\"";

    clicked_item_->SetVal(val);

    keyboard_->hide();

    this->Refresh();
}

void ModVarWindow::ItemClicked(QString type, ModVarListBoxItem *item)
{
    if(isstatic_)
    {
        return;
    }
    if(!clicked_item_.isNull())
    {
        clicked_item_->HideEdit(false);
    }
    clicked_item_ = item;

    list_box_->setCurrentRow(item->GetNum());

    if(type == "bool")
    {
        true_button_->show();
        false_button_->show();
        return;
    }
    else {
        true_button_->hide();
        false_button_->hide();
    }

    num_key_->Clear();
    num_key_->hide();

    if(type == "string")
    {
        QString code = item->GetVal();
        code = code.mid(1, code.size() - 2);

        keyboard_->Init(code);
        keyboard_->resize(this->size());
        keyboard_->move(0, 0);
        keyboard_->show();
    }
    else {
        num_key_->resize(200*scale_w_, 260*scale_h_);
        num_key_->move(this->width() - num_key_->width(), button_widget_->y() - num_key_->height());
        num_key_->Init(item->ShowEdit());
        num_key_->show();
    }
}

void ModVarWindow::TrueButtonClicked()
{
    if(clicked_item_.isNull())
    {
        true_button_->hide();
        false_button_->hide();
        return;
    }
    clicked_item_->SetVal("TRUE");

    this->Refresh();
}

void ModVarWindow::FalseButtonClicked()
{
    if(clicked_item_.isNull())
    {
        true_button_->hide();
        false_button_->hide();
        return;
    }
    clicked_item_->SetVal("FALSE");

    this->Refresh();
}

void ModVarWindow::ConnectInit()
{
    connect(keyboard_->cancel_button_, &QPushButton::clicked,
            [=]()
    {
        keyboard_->hide();
    });

    connect(num_key_, &NumKey::CancelButton,
            [=]()
    {
        num_key_->hide();

        clicked_item_->HideEdit(false);
    });

    connect(num_key_, &NumKey::DefineButton,
            [=]()
    {
        num_key_->hide();

        clicked_item_->HideEdit(true);

        this->Refresh();
    });

    connect(define_button_, &QPushButton::clicked, this, &ModVarWindow::DefineClicked);

    connect(cancel_button_, &QPushButton::clicked, this, &ModVarWindow::CancelClicked);

    connect(keyboard_, &Keyboard::GetData, this, &ModVarWindow::KeyboardGetData);

    connect(true_button_, &QPushButton::clicked, this, &ModVarWindow::TrueButtonClicked);

    connect(false_button_, &QPushButton::clicked, this, &ModVarWindow::FalseButtonClicked);
}

void ModVarWindow::AddListItem(QWeakPointer<VarFormData> pointer, int num)
{
    auto point = pointer.lock();
    if(point.isNull())
        return;

    auto item = new ModVarListBoxItem(pointer, num, this);
    item->SetNum(list_box_->count());
    list_box_->AddWidget(item);

    connect(item, &ModVarListBoxItem::Clicked, this, &ModVarWindow::ItemClicked);

    for(auto sub : point->subnode)
    {
        AddListItem(sub, num + 1);
    }
}

void ModVarWindow::Refresh()
{
    pointer_.lock()->Update();
    for(int i = 0; i < list_box_->count(); i++)
    {
        auto item = list_box_->item(i);

        auto item_widget = dynamic_cast<ModVarListBoxItem*>(list_box_->itemWidget(item));

        item_widget->Refresh();
    }
}

ModVarListBoxItem::ModVarListBoxItem(QWeakPointer<VarFormData> pointer, int num, QWidget *parent) : QWidget(parent)
{
    pointer_ = pointer;

    QHBoxLayout *layout = new QHBoxLayout(this);

    QString name = pointer_.lock()->name;
    QString val = pointer_.lock()->code;
    QString type = pointer_.lock()->type;
    isclick_ = false;

    QString temp;
    for(int i = 0; i < num; i++)
        temp.append(" ");

    if(name.isEmpty())
    {
        if(type == "bool" || type == "string" || type == "num")
        {
            name = temp + type + " :=";
            isclick_ = true;
        }
        else
        {
            name = temp + type + ":";
        }
    }
    else
    {
        if(type == "bool" || type == "string" || type == "num")
        {
            name = temp + name + ":=";
            isclick_ = true;
        }
        else
        {
            name = temp + name + ":";
        }
    }

    if(val.size() > 30)
        val = val.mid(0, 30) + "...";

    name_label_ = new QLabel(name, this);
    val_label_ = new QLabel(val, this);
    val_edit_ = new QLineEdit(this);
    type_label_ = new QLabel(type, this);

    layout->addWidget(name_label_);
    layout->addWidget(val_label_);
    layout->addWidget(val_edit_);
    layout->addWidget(type_label_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(name_label_, 1);
    layout->setStretchFactor(val_label_, 1);
    layout->setStretchFactor(val_edit_, 1);
    layout->setStretchFactor(type_label_, 1);

    val_edit_->hide();
}

QLineEdit *ModVarListBoxItem::ShowEdit()
{
    val_label_->hide();
    val_edit_->show();

    val_edit_->setText(val_label_->text());

    return val_edit_;
}

void ModVarListBoxItem::HideEdit(bool issave)
{
    if(issave)
    {
        QString text = val_edit_->text();
        this->SetVal(text);
    }

    val_edit_->hide();
    val_label_->show();
}

void ModVarListBoxItem::SetNum(int num)
{
    num_ = num;
}

int ModVarListBoxItem::GetNum()
{
    return num_;
}

void ModVarListBoxItem::SetVal(QString val)
{
    pointer_.lock()->code = val;

    if(val.size() > 30)
        val = val.mid(0, 30) + "...";

    val_label_->setText(val);
}

QString ModVarListBoxItem::GetVal()
{
    return pointer_.lock()->code;
}

QString ModVarListBoxItem::GetType()
{
    return pointer_.lock()->type;
}

void ModVarListBoxItem::Refresh()
{
    auto val = pointer_.lock()->code;

    if(val.size() > 30)
        val = val.mid(0, 30) + "...";

    val_label_->setText(val);
}

void ModVarListBoxItem::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(isclick_)
        {
            emit Clicked(GetType(), this);
        }
    }
}

NumKey::NumKey(QWidget *parent) : QFrame (parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *key_layout = new QGridLayout;
    key0_ = new QPushButton("0", this);
    key1_ = new QPushButton("1", this);
    key2_ = new QPushButton("2", this);
    key3_ = new QPushButton("3", this);
    key4_ = new QPushButton("4", this);
    key5_ = new QPushButton("5", this);
    key6_ = new QPushButton("6", this);
    key7_ = new QPushButton("7", this);
    key8_ = new QPushButton("8", this);
    key9_ = new QPushButton("9", this);
    left_move_ = new QPushButton("←", this);
    right_move_ = new QPushButton("→", this);
    delete_ = new QPushButton(this);
    plus_minus_ = new QPushButton("+/-", this);
    point_ = new QPushButton(".", this);
    f_e_ = new QPushButton("F-E", this);
    key0_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key1_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key2_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key3_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key4_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key5_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key6_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key7_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key8_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key9_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    left_move_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_move_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    delete_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plus_minus_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    point_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    f_e_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    key_layout->addWidget(key7_, 0, 0);
    key_layout->addWidget(key8_, 0, 1);
    key_layout->addWidget(key9_, 0, 2);
    key_layout->addWidget(left_move_, 0, 3);
    key_layout->addWidget(key4_, 1, 0);
    key_layout->addWidget(key5_, 1, 1);
    key_layout->addWidget(key6_, 1, 2);
    key_layout->addWidget(right_move_, 1, 3);
    key_layout->addWidget(key1_, 2, 0);
    key_layout->addWidget(key2_, 2, 1);
    key_layout->addWidget(key3_, 2, 2);
    key_layout->addWidget(delete_, 2, 3);
    key_layout->addWidget(key0_, 3, 0);
    key_layout->addWidget(plus_minus_, 3, 1);
    key_layout->addWidget(point_, 3, 2);
    key_layout->addWidget(f_e_, 3, 3);
    key_layout->setContentsMargins(0, 0, 0, 0);
    key_layout->setSpacing(0);

    auto *button_layout = new QHBoxLayout;
    define_button_ = new QPushButton(tr("确定"), this);
    cancel_button_ = new QPushButton(tr("取消"), this);
    button_layout->addWidget(define_button_);
    button_layout->addWidget(cancel_button_);

    layout->addLayout(key_layout);
    layout->addLayout(button_layout);

    delete_->setStyleSheet("border-image: url(:/image/Image/MainWidget/keyboard_delete.png)");

    this->setFrameShape(QFrame::Box);

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);

    ConnectInit();
}

void NumKey::Init(QPointer<QLineEdit> edit)
{
    edit_ = edit;
    edit_->installEventFilter(this); //为编辑框添加事件过滤器
    edit_->setFocus();
    isf_ = false;
}

void NumKey::Clear()
{
    if(edit_.isNull())
        return;

    edit_.clear();
}

QPushButton *NumKey::GetFEButton()
{
    return f_e_;
}

void NumKey::ButtonClicked()
{
    if(edit_.isNull())
        return;

    auto button = dynamic_cast<QPushButton*>(sender());

    if(button == left_move_)
    {
        edit_->cursorBackward(false);
    }
    else if(button == right_move_)
    {
        edit_->cursorForward(false);
    }
    else if(button == delete_)
    {
        edit_->backspace();
    }
    else if(button == plus_minus_) //修改正负
    {
        QString text = edit_->text();

        if(text.isEmpty())
            text = "-";
        else if(text.at(0) == '-')
            text = text.mid(1);
        else
            text.push_front("-");

        edit_->setText(text);
    }
    else if(button == f_e_) //在科学计数法与非科学计数法之间切换
    {
        if(isf_)
        {
            isf_ = false;
            double val = edit_->text().toDouble();

            edit_->setText(QString::number(val, 'f'));
        }
        else
        {
            isf_ = true;
            double val = edit_->text().toDouble();

            edit_->setText(QString::number(val, 'e'));
        }
    }
    else if(button == point_) //小数点
    {
        QString text = edit_->text();
        if(text.indexOf(".") != -1)
        {
            return;
        }
        if(text.isEmpty())
        {
            edit_->setText("0.");
        }
        else
        {
            edit_->insert(button->text());
        }
    }
    else {
        edit_->insert(button->text());
    }
}

void NumKey::ConnectInit()
{
    connect(define_button_, &QPushButton::clicked,
            [=]()
    {
        if(edit_.isNull())
            return;

        double val = edit_->text().toDouble();
        QString str = QString::number(val, 'f');
        int p = str.indexOf(".");

        if(p != -1)
        {
            for(int i = str.size() - 1; i > p; i--)
            {
                if(str.at(i) == '0')
                {
                    str = str.left(i);
                }
                else {
                    break;
                }
            }
        }
        if(str.at(str.size() - 1) == '.')
            str = str.left(str.size() - 1);

        edit_->setText(str);

        this->Clear();
        emit DefineButton();
    });

    connect(cancel_button_, &QPushButton::clicked,
            [=]()
    {
        this->Clear();
        emit CancelButton();
    });

    connect(key0_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key1_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key2_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key3_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key4_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key5_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key6_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key7_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key8_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(key9_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(left_move_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(right_move_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(delete_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(plus_minus_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(point_, &QPushButton::clicked, this, &NumKey::ButtonClicked);

    connect(f_e_, &QPushButton::clicked, this, &NumKey::ButtonClicked);
}

bool NumKey::eventFilter(QObject *object, QEvent *event)
{
    if(object == edit_)
    {
        if(event->type() == QEvent::FocusOut) //只拦截编辑框失去焦点事件
        {
            return true;
        }
    }
    return QFrame::eventFilter(object, event);
}

ABBException::ABBException(QString text)
{
    exp_str_ = text;
}

ABBException::ABBException(QString &text)
{
    exp_str_ = text;
}

ABBException::ABBException(QString class_name, QString func_name, QString error_str, int number)
{
    exp_str_ = "class:" + class_name + " function:" + func_name + " error:" + error_str;
    if(number != 0)
        exp_str_.append(" number:" + QString::number(number));
}

const char *ABBException::what() const noexcept
{
    return exp_str_.toUtf8().data();
}

KeyLineEdit::KeyLineEdit(QWidget *parent) : QLineEdit (parent)
{
}

void KeyLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
}

void KeyLineEdit::focusOutEvent(QFocusEvent *)
{
}

ArraySize::ArraySize(QWidget *parent) : QWidget (parent)
{
    auto *layout = new QVBoxLayout(this);

    head_widget_ = new QFrame(this);
    auto *head_layout = new QGridLayout(head_widget_);
    tips_label_ = new QLabel(tr("定义数组大小:"), head_widget_);
    var_name_label_ = new QLabel(head_widget_);
    tips2_label_ = new QLabel(tr("点击需要更改的维数"), head_widget_);
    head_layout->addWidget(tips_label_, 0, 0);
    head_layout->addWidget(var_name_label_, 0, 1);
    head_layout->addWidget(tips2_label_, 1, 0);

    table_widget_ = new QTableWidget(this);

    button_widget_ = new QWidget(this);
    auto *bt_layout = new QHBoxLayout(button_widget_);
    define_button_ = new QPushButton(tr("确定"), button_widget_);
    cancel_button_ = new QPushButton(tr("取消"), button_widget_);
    define_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    define_button_->setFlat(true);
    cancel_button_->setFlat(true);
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addStretch();
    bt_layout->addWidget(define_button_);
    bt_layout->addWidget(cancel_button_);
    bt_layout->setContentsMargins(0, 0, 0, 0);
    bt_layout->setSpacing(0);

    layout->addWidget(head_widget_);
    layout->addWidget(table_widget_);
    layout->addWidget(button_widget_);
    layout->setStretchFactor(head_widget_, 3);
    layout->setStretchFactor(table_widget_, 7);
    layout->setStretchFactor(button_widget_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    table_widget_->horizontalHeader()->show();
    table_widget_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_widget_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_widget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //设置表格列宽自动随窗口大小进行调整
    table_widget_->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置禁止编辑
    table_widget_->setItemDelegate(new NoFocus(this)); //去除虚线框
    table_widget_->setShowGrid(false); //隐藏网格线
    table_widget_->verticalHeader()->hide(); //隐藏行表头
    table_widget_->setColumnCount(3);
    table_widget_->setHorizontalHeaderLabels(QStringList() << tr("维数") << tr("大小") << "");
    current_item_ = nullptr;

    scale_w_ = scale_h_ = 1;

    num_key_ = new NumKey(this);
    num_key_->hide();
    num_key_->plus_minus_->hide();
    num_key_->point_->hide();
    num_key_->f_e_->hide();

    QPalette p;
    p.setColor(QPalette::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(p);
    p.setColor(QPalette::Window, QColor(192, 192, 192));
    button_widget_->setAutoFillBackground(true);
    button_widget_->setPalette(p);

    ConnectInit();
}

void ArraySize::Init(QString var_name, int size)
{
    var_name_label_->setText(var_name);

    table_widget_->clearContents();
    table_widget_->setRowCount(size);
    table_widget_->setColumnCount(3);
    for(int i = 0; i < size; i++)
    {
        table_widget_->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        table_widget_->setItem(i, 1, new QTableWidgetItem(QString::number(1)));
        table_widget_->setItem(i, 2, new QTableWidgetItem(""));
    }
}

void ArraySize::SetScale(double scale_w, double scale_h)
{
    scale_w_ = scale_w;
    scale_h_ = scale_h;

    if(!num_key_->isHidden())
    {
        num_key_->resize(210*scale_w_, 267*scale_h_);
        num_key_->move(430*scale_w_, 54*scale_h_);
    }
}

void ArraySize::ItemClicked(QTableWidgetItem *item)
{
    if(item == nullptr)
        return;
    int item_row = table_widget_->row(item);
    item = table_widget_->item(item_row, 1);

    if(current_item_ == item)
    {
        return;
    }
    else if(current_item_ != nullptr)
    {
        if(!edit_key_.isNull())
            table_widget_->removeCellWidget(table_widget_->row(current_item_), 1);
    }

    current_item_ = item;
    edit_key_ = new QLineEdit(item->text());

    table_widget_->setCellWidget(item_row, 1, edit_key_);

    num_key_->resize(210*scale_w_, 267*scale_h_);
    num_key_->move(430*scale_w_, 54*scale_h_);
    num_key_->show();
    num_key_->Init(edit_key_);
}

void ArraySize::DefineClicked()
{
    QString dim_str = "{";

    for(int i = 0; i < table_widget_->rowCount(); i++)
    {
        auto *item = table_widget_->item(i, 1);

        dim_str.append(item->text());
        if(i + 1 < table_widget_->rowCount())
            dim_str.append(",");
    }

    dim_str.append("}");

    emit Define(dim_str);

    if(!num_key_->isHidden())
        num_key_->hide();
}

void ArraySize::KeyDefine()
{
    if(edit_key_.isNull() || current_item_ == nullptr)
    {
        num_key_->hide();
        return;
    }

    auto text = edit_key_->text();

    current_item_->setText(text);
    table_widget_->removeCellWidget(table_widget_->row(current_item_), 1);
    current_item_ = nullptr;

    num_key_->hide();
}

void ArraySize::KeyCancel()
{
    if(edit_key_.isNull() || current_item_ == nullptr)
    {
        num_key_->hide();
        return;
    }

    table_widget_->removeCellWidget(table_widget_->row(current_item_), 1);
    current_item_ = nullptr;

    num_key_->hide();
}

void ArraySize::ConnectInit()
{
    connect(table_widget_, &QTableWidget::itemClicked, this, &ArraySize::ItemClicked);

    connect(cancel_button_, &QPushButton::clicked, this, &ArraySize::Cancel);

    connect(define_button_, &QPushButton::clicked, this, &ArraySize::DefineClicked);

    connect(num_key_, &NumKey::DefineButton, this, &ArraySize::KeyDefine);

    connect(num_key_, &NumKey::CancelButton, this, &ArraySize::KeyCancel);
}
