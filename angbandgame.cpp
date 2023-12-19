#include "angbandgame.h"
#include "ui_angbanggame.h"

AngBangGame::AngBangGame(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AngBangGame)
{
    start = new startMenu; //выделяем память для стартового окна
    start->show(); //показываем стартовое окно
    connect(start, &startMenu::signalStart, this, &AngBangGame::slotsStart); //соединяем стартовое окно и окно с игрой

    setFixedSize(SIZE_WINDOW, SIZE_WINDOW); //задаем фиксированные размеры окна с игрой
    ui->setupUi(this);

    QImage image;
    //загружаем картинку и позицию для человека
    image.load(":/picture/человек.png");
    human.s = image;
    human.x = 300;
    human.y = 300;

    //загружаем картинку для воина
    image.load(":/picture/воин1.png");
    soldier.s = image;

    //загружаем картинку и позицию для гнома
    image.load(":/picture/гном1.png");
    dwarf.x = 300;
    dwarf.y = 300;
    dwarf.s = image;

    //загружаем картинку для гнома-воителя
    image.load(":/picture/гном-воин1.png");
    dWarrior.s = image;

    //загружаем картинку и позицию для щита
    image.load(":/picture/щит1.png");
    shield.x = 460;
    shield.y = 370;
    shield.s = image;

    //загружаем картинку для орка
    image.load(":/picture/орк.png");
    orc.s = image;

}

AngBangGame::~AngBangGame()
{
    delete start; //освобождаем выделенную память
    delete ui;
}

void AngBangGame::slotsStart(bool flag)
{
    if (flag) //если true - то это человек, иначе - гном
        person = HUMAN;
    else
        person = DWARF;
    show(); //показать окно с игрой
    startTimer(30); //начать таймер
    start->close(); //закрыть стартовое окно
}




void AngBangGame::paintEvent(QPaintEvent*)
{
    QPainter* painter = new QPainter(); //выделяем память для отрисовки
    painter->begin(this); //обновляем рисунок
    QBrush brush; //переменная для фона
    brush.setColor("#2b2a2a"); //ставим цвет фона черно-серым
    brush.setStyle(Qt::SolidPattern); //говорит о том, что будем менять задний фон
    QRect rect(-1, -1, SIZE_WINDOW, SIZE_WINDOW); //создаем сзади квадрат
    painter->setBrush(brush); //делаем его серым
    painter->drawRect(rect); //вставляем в игрвое окно

    loadMap(painter); //рисуем карту

    //в зависимости от того, что необходимо в данный момент в игре - рисуем нужного персонажа
    if (person == HUMAN){
        painter->drawImage(human.x, human.y, human.s); //рисует картинку
        movePerson(human); //заставляет двигаться персонажа
    }
    //аналогично и с другими персонажами
    if (person == SOLDIER){
        painter->drawImage(soldier.x, soldier.y, soldier.s);
        movePerson(soldier);
    }

    if (person == DWARF){
        painter->drawImage(dwarf.x, dwarf.y, dwarf.s);
        movePerson(dwarf);
    }
    if (person == WARRIOR){
        painter->drawImage(dWarrior.x, dWarrior.y, dWarrior.s);
        movePerson(dWarrior);
    }

    //если персонаж поднял щит, то он пропадает (не рисуем его больше)
    if (person == HUMAN || person == DWARF)
        painter->drawImage(shield.x, shield.y, shield.s);

    moveEnemy(); //враг всегда двигается

    painter->drawImage(orc.x, orc.y, orc.s); //рисуем иконку врага
    painter->end(); //заканчиваем рисовать
}




void AngBangGame::keyPressEvent(QKeyEvent *event)
{
    int key = event->key(); //дает номер клавиши
    //если номер совпадает с некоторыми клавишами, то
    //move становится указателем для дальнейших действий в программе
    if (key == Qt::Key_Left){
        move = LEFT;
    }
    else if (key == Qt::Key_Right){
        move = RIGHT;
    }
    else if (key == Qt::Key_Up){
        move = UP;
    }
    else if (key == Qt::Key_Down){
        move = DOWN;
    }
}




void AngBangGame::timerEvent(QTimerEvent *)
{
    //каждый тик таймера перерисовываем поле
    repaint();
}



void AngBangGame::loadMap(QPainter* painter)
{
    //постепенно создаются позиции клеток поля, и оно после рисуется
    if (act == ROOM1){
        createFirstRoom();
        act = BRIDGE;
    }

    if (act == BRIDGE){
        createBridge();
        act = ROOM2;
    }

    if (act == ROOM2){
        createSecondRoom();
    }

    //рисуем клетки стен первой комнаты
    for(int i = 0; i < listRoom1.size(); i++){
        painter->drawImage(listRoom1[i].x, listRoom1[i].y, listRoom1[i].s);
    }

    //рисуем клетки стен моста
    for(int i = 0; i < listBridge.size(); i++){
        painter->drawImage(listBridge[i].x, listBridge[i].y, listBridge[i].s);
    }

    //рисуем клетки стен второй комнаты
    for(int i = 0; i < listRoom2.size(); i++){
        painter->drawImage(listRoom2[i].x, listRoom2[i].y, listRoom2[i].s);
    }


    fillInsideMap(painter);//делаем внутри замка пол

    //изначально ставим орка на позицию первой клетки во 2 комнате
    if (act == ROOM2){
        orc.x = listFreeRoom2[0].x;
        orc.y = listFreeRoom2[0].y;
        act = GAME;
    }
}




void AngBangGame::fillInsideMap(QPainter* painter)
{
    QImage image; //переменная картинки
    image.load(":/picture/пол30_30.jpg"); //загружаем картинку пола

    //изначальное расположение пола - начальная позиция стены комнаты 1
    int x = listRoom1[0].x;
    int y = listRoom1[0].y;


    stone.s = image; //внедряем картинку в переменную для отрисовки
    for(int i = 1; i < VALUE_ROOM1; i++){ //будем рисовать комнату 1
        for(int j = 0; j < VALUE_ROOM1 - 1; j++){ //рисуем только во внутреннем квадрате
            //ставим позиции клетки
            stone.x = x + SIZE_STONE*j;
            stone.y = y + SIZE_STONE*i;

            //запоминаем все свободные клетки комнаты 1, кроме последних
            if (i + 1 < VALUE_ROOM1 && j + 2 < VALUE_ROOM1)
                listFreeRoom1.push_back(stone);

            //рисуем клетку пола
            painter->drawImage(stone.x, stone.y, stone.s);
        }
    }

    //запоминаем начало прохода к мосту, используя его координаты
    stone.x = x_door - SIZE_STONE;
    stone.y = y_door;
    listFreeRoom1.push_back(stone);

    for(int i = 0; i < VALUE_BRIDGE/2; i++){ //будем рисовать мост
        //ставим позиции клетки
        stone.x = x_door + i*SIZE_STONE;
        stone.y = y_door;

        listFreeBridge.push_back(stone); //запоминаем свободные клетки моста
        painter->drawImage(stone.x, stone.y, stone.s); //рисуем клетки моста
    }

    //выставляем изначаные позиции стен комнаты 2
    x = listRoom2[0].x;
    y = listRoom2[0].y;

    for(int i = 1; i < VALUE_ROOM2; i++){ //будем рисовать 2 комнату
        for(int j = 0; j < VALUE_ROOM2 - 1; j++){ //рисуем внутренний квадрат
            //ставим позиции клетки
            stone.x = x + SIZE_STONE*j;
            stone.y = y + SIZE_STONE*i;

            //если это не последний ряд и строчка, то запоминаем свободные клетки комнаты 2
            if (i + 1 < VALUE_ROOM2 && j + 2 < VALUE_ROOM2)
                listFreeRoom2.push_back(stone);

            painter->drawImage(stone.x, stone.y, stone.s); //рисуем клетки пола комнаты 2
        }
    }
}



void AngBangGame::movePerson(Person &p)
{
    Person pC = p; //запоминаем персонажа, чтобы его проверить

    //в зависимости от того, что мы нажали
    //будем меняться позиция персонажа с определенной скоростью
    if (move == LEFT){
        pC.x -= SPEED;
    }
    if (move == RIGHT){
        pC.x += SPEED;
    }
    if (move == DOWN){
        pC.y += SPEED;
    }
    if (move == UP){
        pC.y -= SPEED;
    }


    //если персонаж находится внутри свободных клеток, то меняем позицию персонажа
    if (touchRoom(pC)){
        p = pC;
        //если дотронулись до щита, то меняем персонажа, в зависимости от выбранного типа
        if (touchShield(p)){
            if (person == HUMAN){
                soldier.x = p.x;
                soldier.y = p.y;
                p = soldier;
                person = SOLDIER;
            }
            else if (person == DWARF){
                dWarrior.x = p.x;
                dWarrior.y = p.y;
                p = dWarrior;
                person = WARRIOR;
            }
        }

        //если дотронулись до врага, то проверяется кто победитель и происходит выход из программы
        if (touchOrc(p)){
            QString winner = person == SOLDIER || person == WARRIOR ?
                        "победитель -> игрок": "победитель -> орк";
            QMessageBox::information(this, "игра окончена", winner);
            exit(1);
        }
    }
    move = STAND; //если нет движения - то стоим
}

void AngBangGame::moveEnemy()
{

    int n = listFreeRoom2.size(); //запоминаем размер массива свободных клеток у комнаты 2
    int xMin = listFreeRoom2[0].x; //сохраняем позицию x первой свободной клетки
    int xMax = listFreeRoom2[n - 1].x + SIZE_ORC; //сохраняем позицию x последней свободной клетки
    int yMin = listFreeRoom2[0].y; //сохраняем позицию y первой свободной клетки
    int yMax = listFreeRoom2[n - 1].y + SIZE_ORC; //сохраняем позицию y последней свободной клетки

    if (moveOrc == BEGIN){ //если враг начинает двигаться
        if ((orc.x >= xMin && orc.x <= xMax) && //если же нужно двигаться вправо - то двигаемся
            (orc.y >= yMin && orc.y <= yMax)){
            orc.x += 3;
        }
        else { //если враг заканчивает двигаться
            orc.y += 3; //двигаемся вниз
            if (orc.y >= yMax)
                moveOrc = END;
        }
    }
    else {
        if ((orc.x >= xMin) &&
            (orc.y >= yMin && orc.y <= yMax + SIZE_ORC)){ //двигаемся влево
            orc.x -= SPEED/2;
        }
        else {
            orc.y -= SPEED/2;
            if (orc.y - SPEED/2 < yMin){ //двигаемся вверх и снова меняем на начало движения
                moveOrc = BEGIN;
                orc.x += 3;
            }
        }
    }
}





bool AngBangGame::touchRoom(Person p)
{ 
    int x = p.x; //запоминаем позицию x персонажа
    int y = p.y; //запоминаем позицию y персонажа

    int ans = false; //изначально стену комнаты не коснулись

    for(int i = 0; i < listFreeRoom1.size(); i++){ //проверяем коснулись ли стенок комнаты 1
        //позиция свободной клетки
        int xF = listFreeRoom1[i].x;
        int yF = listFreeRoom1[i].y;
        //логика, которая позволяет вычислить стукнулся герой о стенку или нет
        //из за того, что картинки разного размера, то по пикселям доделываем
        if ((x >= xF && x <= xF + SIZE_STONE + (person == DWARF? 15: 5))&& //
              (y >= yF && y <= yF + SIZE_STONE + (person == DWARF? 6: 3))){
            ans = true;
        }
    }



    for(int i = 0; i < listFreeBridge.size(); i++){ //проверяем коснулись ли стенок моста
        //позиция свободной клетки
        int xF = listFreeBridge[i].x;
        int yF = listFreeBridge[i].y;
        //такая же логика по сути, как и у верхнего условия
        if ((x >= xF && x <= xF + SIZE_STONE)&&
              (y >= yF - 3 && y <= yF + 3)){
            ans = true;
        }
    }

    for(int i = 0; i < listFreeRoom2.size(); i++){ //проверяем коснулись ли стенок комнаты 2
        //позиция свободной клетки
        int xF = listFreeRoom2[i].x;
        int yF = listFreeRoom2[i].y;
        //такая же логика по сути, как и у верхнего условия
        if ((x >= xF && x <= xF + SIZE_STONE + (person == DWARF? 15: 5))&&
              (y >= yF && y <= yF + SIZE_STONE + (person == DWARF? 6: 3))){
            ans = true;
        }
    }

    return ans;
}

bool AngBangGame::touchShield(Person p)
{
    //если персонаж человек или гном, то создаем ширину и высоту персонажей
    if (person == HUMAN || person == DWARF){
        int width = 0, height = 0;
        if (person == HUMAN){
            width = SIZE_W_HUMAN;
            height = SIZE_H_HUMAN;
        }
        if (person == DWARF){
            width = SIZE_W_DWARF - 5;
            height = SIZE_H_DWARF;
        }

        for (int i = shield.x; i < shield.x + 24; i++){ //проверяем все точки в щите
            for (int j = shield.y; j < shield.y + 27; j++){
                if ((i >= p.x + width/2 && i <= p.x + width)&&
                    (j >= p.y && j <= p.y + height)) //если щита коснулись, то выводим true
                {
                    return true;
                }
            }
        }
    }

    return false; //если щита не коснулись или персонажи не те, то false
}



bool AngBangGame::touchOrc(Person p)
{
    int width = 0, height = 0;
    //в зависимости от персонажа создаем его длину и ширину
    if (person == HUMAN){
        width = SIZE_W_HUMAN;
        height = SIZE_H_HUMAN;
    }
    if (person == SOLDIER){
        width = SIZE_W_SOLDIER;
        height = SIZE_H_SOLDIER;
    }
    if (person == DWARF){
        width = SIZE_W_DWARF - 5;
        height = SIZE_H_DWARF;
    }
    if (person == WARRIOR){
        width = SIZE_W_WARRIOR - 5;
        height = SIZE_H_WARRIOR;
    }

    //проверяем все позиции точек внутри орка
    for (int i = orc.x; i < orc.x + SIZE_ORC - 3; i++){
        for (int j = orc.y; j < orc.y + SIZE_ORC - 3; j++){
            if ((i >= p.x + width/2 && i <= p.x + width)&& //если орка коснулись, то выводим true
                (j >= p.y && j <= p.y + height))
            {
                return true;
            }
        }
    }

    return false; //в другом случае выводится false
}



void AngBangGame::createFirstRoom()
{
    QImage image;
    image.load(":/picture/стена30_30.jpg"); //загружаем кортинку стены
    //задаем начальные позиции клетки и запоминаем ее
    stone.x = START_POZ;
    stone.y = stone.x;
    stone.s = image;
    listRoom1.push_back(stone);

    //создаем позиции 4-ёх стенок
    for(int i = 1; i < VALUE_ROOM1 * 4; i++){
        //на основе предыдущего элемента сдвигаем след. позицию клетки
        stone.x = listRoom1[i - 1].x;
        stone.y = listRoom1[i - 1].y;


        if (i < VALUE_ROOM1){ //если это первая стена
            stone.x += SIZE_STONE; //сдвигаем позиции влево
        }
        else if (i >= VALUE_ROOM1 && i < 2*VALUE_ROOM1){ //если вторая стена
            stone.y += SIZE_STONE; //сдвигаем вниз
        }
        else if (i >= 2*VALUE_ROOM1 && i < 3*VALUE_ROOM1){ //если 3 стена
            stone.x -= SIZE_STONE; //сдвигаем влево
        }
        else {
            stone.y -= SIZE_STONE; //и если последняя стенка сдвигаем вверх
        }
        //включаем в список клетку
        listRoom1.push_back(stone);
    }
    //удаляем из списка позиций клетку, которая является начало к мосту
    x_door = listRoom1[DOOR].x;
    y_door = listRoom1[DOOR].y;
    listRoom1.removeAt(DOOR);
}




void AngBangGame::createBridge()
{
    QImage image;
    image.load(":/picture/стена30_30.jpg"); //загружаем картинку стены
    //задаем начальные позиции первой клетки и запоминаем ее в списке
    stone.x = listRoom1[DOOR - 1].x + SIZE_STONE;
    stone.y = listRoom1[DOOR - 1].y;
    stone.s = image;
    listBridge.push_back(stone);
    //так же клетку начальной позиции моста запоминаем
    stone.y = listRoom1[DOOR].y;
    listBridge.push_back(stone);

    for(int i = 1; i < VALUE_BRIDGE; i++){
        //задаем позицию клеток, сдвигая их вправо
        stone.x = listBridge[i - 1].x + SIZE_STONE;
        stone.y = listBridge[i - 1].y;
        listBridge.push_back(stone);
        stone.y = listRoom1[DOOR].y;
        //включаем в список клетку
        listBridge.push_back(stone);
    }
}




void AngBangGame::createSecondRoom()
{
    QImage image;
    image.load(":/picture/стена30_30.jpg"); //загружаем картинку стены
    int n = listBridge.size(); //запоминаем размер комнаты
    //из списка клеток моста делаем новую позицию комнаты
    stone.x = listBridge[n - 1].x + SIZE_STONE;
    stone.y = listBridge[n - 1].y - 2*SIZE_STONE;
    stone.s = image;
    listRoom2.push_back(stone);

    //из 4 стен создаем комнату
    for(int i = 1; i < VALUE_ROOM2 * 4; i++){
        //задаем позицию клетки
        stone.x = listRoom2[i - 1].x;
        stone.y = listRoom2[i - 1].y;

        if (i < VALUE_ROOM2){ //если это 1 стена
            stone.x += SIZE_STONE; //сдвигаем клетку влево
        }
        else if (i >= VALUE_ROOM2 && i < 2*VALUE_ROOM2){ //если 2 стена
            stone.y += SIZE_STONE; //сдвигаем клетку вниз
        }
        else if (i >= 2*VALUE_ROOM2 && i < 3*VALUE_ROOM2){ //если 3 стена
            stone.x -= SIZE_STONE; //сдвигаем клетку влево
        }
        else {
            stone.y -= SIZE_STONE; //на 4 стене все сдвигаем вверх
        }
        listRoom2.push_back(stone); //запоминаем все позиции клеток стены
    }
    listRoom2.removeAt(listRoom2.size() - 2); //удаляем клетку, которая загараживает мост
}



