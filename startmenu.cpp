#include "startmenu.h"
#include "ui_startmenu.h"

startMenu::startMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::startMenu)
{

    setFixedSize(475, 273); //задаем фиксированный размер окна
    ui->setupUi(this);
}

startMenu::~startMenu()
{
    delete ui;
}

void startMenu::on_pushButton_clicked()
{
    emit signalStart(true); //если выбрали человека - true
}


void startMenu::on_pushButton_3_clicked()
{
    emit signalStart(false); //если выбрали гнома - false
}

void startMenu::paintEvent(QPaintEvent *)
{
    //создаем точно такой же серый квадрат сзади, но поменьше
    QPainter* painter = new QPainter();
    painter->begin(this);
    QBrush brush;
    brush.setColor("gray");
    brush.setStyle(Qt::SolidPattern);
    QRect rect(-1, -1, 475, 273);
    painter->setBrush(brush);
    painter->drawRect(rect);
    painter->end();
}

