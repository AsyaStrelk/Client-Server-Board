#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Задаем номер порта
    port = _port;
    // Создаем сокет
    tcpSocket = new QTcpSocket(this);

    // Рисуем графический интерфейс
    drawUI();
}

/**
 * @brief Рисуем графический интерфейс
 */
void MainWindow::drawUI()
{
    // Устанавливаем размер приложения
    setFixedSize(QSize(400,250));
    // Устанавливаем title приложения
    setWindowTitle ("Not Connected");

    // Кнопка для подключения к серверу
    changeIpButton = new QPushButton("Connect", this);
    changeIpButton->setGeometry(QRect(QPoint(10,10), QSize(150, 50)));
    connect(changeIpButton, SIGNAL(released()), this, SLOT(handleChangeIpButton()));

    // Комбобокс для выбора светодиода который должени быть изменен
    changeComboBox = new QComboBox(this);
    changeComboBox->addItem("");
    changeComboBox->addItem("Red");
    changeComboBox->addItem("Blue");
    changeComboBox->addItem("Green");
    changeComboBox->setCurrentIndex(0);
    changeComboBox->setGeometry(QRect(QPoint(10,100), QSize(150, 20)));

    // Кнопка для отправки сообщения на сервер
    changeCheckboxButton = new QPushButton("Change", this);
    // Делаем неактивной кнопку отправки на сервер, если не было установлено подключение
    changeCheckboxButton->setDisabled(true);
    changeCheckboxButton->setGeometry(QRect(QPoint(10,130), QSize(150, 50)));
    connect(changeCheckboxButton, SIGNAL(released()), this, SLOT(write()));

    // Таблицы с данными, полученными с сервера
    // таблица с состоянием светодиодов
    lightTableWidget = new QTableWidget(3,2,this);
    lightTableWidget->setHorizontalHeaderLabels(QStringList() << tr("Color") << ("State"));
    lightTableWidget->setGeometry(QRect(QPoint(170,10), QSize(225, 120)));
    lightTableWidget->setItem(0, 0, new QTableWidgetItem("Red"));
    lightTableWidget->setItem(1, 0, new QTableWidgetItem("Blue"));
    lightTableWidget->setItem(2, 0, new QTableWidgetItem("Green"));
    lightTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(2, 1, new QTableWidgetItem("?"));
    // Таблица с состоянием кнопок
    buttonTableWidget = new QTableWidget(2,2,this);
    buttonTableWidget->setHorizontalHeaderLabels(QStringList() << tr("Button") << ("State"));
    buttonTableWidget->setGeometry(QRect(QPoint(170,140), QSize(225, 90)));
    buttonTableWidget->setItem(0, 0, new QTableWidgetItem("Button 1"));
    buttonTableWidget->setItem(1, 0, new QTableWidgetItem("Button 2"));
    buttonTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    buttonTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
}

/**
 * @brief Получаем ip адрес сервера
 * @return true ip адрес получен
 *         false ip адрес не получен
 */
bool MainWindow::getIpAddress()
{
    bool ok = false;
    // Пока не введен правильный ip адрес
    while (!ok)
    {
        // Окно ввода ip адреса
        QString text = QInputDialog::getText(this, tr("IP-Address"), tr("IP-Address:"), QLineEdit::Normal, "", &ok);
        // Если нажата кнопка "Canсel"
        if (!ok)
        {
           // Адрес не введен
           return false;
        }
        // Регулярное выражение, которое проверяет, введено ли число от 0 до 255
        QString ipRange = "([0-1]?\\d?\\d|2[0-4]\\d|25[0-5])";
        // Создаем регулярное выражение
        QRegExp ipRegex ("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
        // Определяем, является ли введенное сообщение ip адресом
        ok =  ipRegex.exactMatch(text) ? true : false;
        // Если введен не ip адрес
        if (!ipRegex.exactMatch(text))
        {
            // Выводим сообщение об ошибке
            QMessageBox::warning(this, "","Wrong IP-Address");
            ok = false;
        }
        else {
            // Сохраняем ip адрес
            address = QHostAddress(text);
        }
    }
    // Адрес введен
    return true;
}

/**
 * @brief Подключаемся к серверу по протоколу tcp
 */
void MainWindow::tcpConnect()
{
    // Подключаем сокет к серверу
    tcpSocket->connectToHost(address, port);
    // Устанавливаем таймаут для подключения
    if (tcpSocket->waitForConnected(1000))
    {
        // Подключение завершено удачно
        QMessageBox::warning(this, "","Connected");
    }
    else
    {
        // Подключение не завершено
        QMessageBox::warning(this, "","Connection error");
        exit(EXIT_FAILURE);
    }
    // Устанавливаем сигнал для чтения данных с сервера
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

/**
 * @brief Чтение сообщения с сервера
 */
void MainWindow::readyRead()
{
    QByteArray data;
    // Читаем данные с сервера
    data = tcpSocket->readAll();
    // Преобразуем байт в строку
    message = tr("data from COM port:%1%2%3%4%5%6%7%8").arg((data[0]&128)!=0)
              .arg((data[0]&64)!=0).arg((data[0]&32)!=0).arg((data[0]&16)!=0)
              .arg((data[0]&8)!=0).arg((data[0]&4)!=0).arg((data[0]&2)!=0).arg((data[0]&1)!=0);
    // Удаляем ненужные элементы
    message.remove(0,19);
    // Определяем какая кнопка была нажата
    if (message[3] == "1" and message[4] == "0")
    {
        // Нажата вторая кнопка
        buttonTableWidget->setItem(0, 1, new QTableWidgetItem("Off"));
        buttonTableWidget->setItem(1, 1, new QTableWidgetItem("On"));
    }
    else if (message[3] == "0" and message[4] == "1")
    {
        // Нажата первая кнопка
        buttonTableWidget->setItem(0, 1, new QTableWidgetItem("On"));
        buttonTableWidget->setItem(1, 1, new QTableWidgetItem("Off"));
    }
    else
    {
        buttonTableWidget->setItem(0, 1, new QTableWidgetItem("Off"));
        buttonTableWidget->setItem(1, 1, new QTableWidgetItem("Off"));
    }
    // Определяем состояние красного светодиода
    lightTableWidget->setItem(0, 1, new QTableWidgetItem(message[7] == "0" ? "Off" : "On"));
    // Определяем состояние синего светодиода
    lightTableWidget->setItem(1, 1, new QTableWidgetItem(message[5] == "0" ? "Off" : "On"));
    // Определяем состояние зеленого светодиода
    lightTableWidget->setItem(2, 1, new QTableWidgetItem(message[6] == "0" ? "Off" : "On"));
}

/**
 * @brief Отправка сообщения на сервер
 */
void MainWindow::write()
{
    QByteArray data;
    // Определяем состояние какого светодиода нужно изменить
    switch (changeComboBox->currentIndex())
    {
        // Выбран красный светодиод
        case 1:
            data = "q";
            break;
        // Выбран синий светодиод
        case 2:
            data = "t";
            break;
        // Выбран зеленый светодион
        case 3:
            data = "z";
            break;
        // Кнопка была нажата по ошибке
        default:
            return;
    }

    // Счетчик неудачных попыток отправки сообщения на сервер
    int count = 0;
    for(;;)
    {
        // Успешно отправилось на сервер
        if (tcpSocket->write(data) != -1)
        {
            break;
        }
        // Неуспешно отправилось на сервер
        else
        {
            // Увеличиваем счетчик неудачных попыток
            count++;
        }
        // Если количество неудачеых попыток достигло 10, то выходим из программы
        if (count == 10)
        {
            // Выводим сообщение об ошибке
            QMessageBox::warning(this, "","Writing error");
            exit(EXIT_FAILURE);
        }
    }

    // Устанавливаем значение комбобокса на пустую строку
    changeComboBox->setCurrentIndex(0);
}

/**
 * @brief Обработка нажатия на кнопку "Connect"
 */
void MainWindow::handleChangeIpButton()
{
    // Не подключаемся, если не введен ip адрес
    if (!getIpAddress())
    {
        return;
    }
    // Подключаемся к серверу
    tcpConnect();
    // Устанавливаем title приложения
    setWindowTitle (address.toString());
    // Делаем активной кнопку отправки на сервер
    changeCheckboxButton->setDisabled(false);
    // Устанавливаем значения в таблице состояний при повторном подключении
    buttonTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    buttonTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(2, 1, new QTableWidgetItem("?"));
}

/**
 * @brief Деструктор
 */
MainWindow::~MainWindow()
{
    // Отключаемя от сервера
    tcpSocket->close();
    tcpSocket->~QTcpSocket();
    delete ui;
}
