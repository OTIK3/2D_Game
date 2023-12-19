#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class startMenu;
}

class startMenu : public QWidget
{
    Q_OBJECT

public:
    explicit startMenu(QWidget *parent = nullptr);
    ~startMenu();
signals:
    void signalStart(bool); //отправляет окну с игрой информацию какая кнопка нажата

private slots:
    void on_pushButton_clicked(); //обработка кнопки с человеком

    void on_pushButton_3_clicked(); //обработка кнопки с гномом

protected:
    void paintEvent(QPaintEvent *event); //отрисовка заднего фона

private:
    Ui::startMenu *ui;
};

#endif // STARTMENU_H
