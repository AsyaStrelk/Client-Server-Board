#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TcpServer");

    //отрисовка текстового поля состояния сервера
    textField = new QTextEdit(" ",this);
    textField->setGeometry(QRect(QPoint(20,20),QSize(420,360)));
    textField->setReadOnly(true);

    //кнопка запуска сервера
    startButton = new QPushButton("Start", this);
    startButton->setGeometry(QRect(QPoint(20,420),QSize(200,40)));
    connect(startButton, SIGNAL(released()), this, SLOT(startButtonClicked()));

    //кнопка остановки сервера
    stopButton = new QPushButton("Stop", this);
    stopButton->setGeometry(QRect(QPoint(240,420),QSize(200,40)));
    connect(stopButton, SIGNAL(released()), this, SLOT(stopButtonClicked()));
    stopButton->setEnabled(false);

    //создание сервера
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(addNewUser()));

    //дескриптор порта
    serialPort = new QSerialPort(this);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(slotReadComPort()));

    //отрисовка меню com-портов
    serialPortBox=new QComboBox(this);
    serialPortBox->setGeometry(QRect(QPoint(20,390),QSize(420,20)));

    //получение и заполнение списка com-портов
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        serialPortBox->addItem(info.portName());
    }
}

/**
 * @brief Запуск сервера, подключение к выбранному com-порту по нажатию на "Start"
 */
void MainWindow::startButtonClicked()
{
    //запись в поле состояния сервера, блокировка кнопок
    textField->clear();
    textField->append("starting server...");
    serialPortBox->setEnabled(false);
    startButton->setEnabled(false);
    stopButton->setEnabled(true);

    //запуск сервера, подключение к com-порту
    startServer();
    connectCOM();
}

/**
 * @brief Остановка сервера, отключение от порта по нажатию на "Stop"
 */
void MainWindow::stopButtonClicked()
{
    serialPortBox->setEnabled(true);
    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    //остановка сервера, отключение от порта
    stopServer();
    closeSerialPort();
}

/**
 * @brief Запуск tcp-сервера
 */
void MainWindow::startServer()
{
    //попытка создания сокета, если сокет не создан, то вывод сообщения об ошибке, иначе успешное создание
    if (!tcpServer->listen(QHostAddress::Any, 9090) && server_status==0)
    {
        textField->append(tcpServer->errorString());
    }
    else
    {
        server_status=1; // сервер запущен
        textField->append(tcpServer->isListening()+" TCPSocket listen on port");
    }
}

/**
 * @brief Остановка tcp-сервера
 */
void MainWindow::stopServer()
{
    if(server_status==1)
    {
        //очистка списка клиентов
        foreach(int i,SClients.keys())
        {
        QTextStream os(SClients[i]);
        SClients[i]->close();
        SClients.remove(i);
        }
        //закрытие сокета
        tcpServer->close();
        server_status=0;
        textField->append("server was stopped...");
    }
}

/**
 * @brief Добавление нового клиента по приходу запроса на соединение
 */
void MainWindow::addNewUser()
{
    //если сервер запущен,то происходит добавление нового клиента
    if(server_status==1)
    {
        //получение и запись информации о новом соединетнии
        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        uint32_t idUserSocs=clientSocket->socketDescriptor();
        textField->append("new connection: "+clientSocket->peerAddress().toString());
        SClients[idUserSocs]=clientSocket;
        //установка обработчика на сигнал о передаче данных от клиента
        connect(SClients[idUserSocs],SIGNAL(readyRead()),this, SLOT(slotReadClient()));
        std::cout << SClients.size();
    }
}

/**
 * @brief Удаление клиента из списка клиентов
 */
void MainWindow::slotDeleteClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    //удаление отключившегося клиента из раздачи
    uint32_t idUserSocs=clientSocket->socketDescriptor();
    SClients[idUserSocs]->close();
    SClients.remove(idUserSocs);
    textField->append("client was disconnected, clent id: " + idUserSocs);
}

/**
 * @brief Чтение данных, полученных от клиента
 * с последующей передачей на микроконтроллер
 */
void MainWindow::slotReadClient()
{
    //получение информации о выбранных клиентом изменениях в состоянии светодиодов микроконтроллера
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idUserSocs=clientSocket->socketDescriptor();
    QByteArray data = clientSocket->readAll();
    textField->append("data from client:"+data);
    //пересылка данных микроконтроллеру
    serialPort->write(data);
}

/**
 * @brief Чтение данных, полученных от микроконтроллера
 * с последующей передачей клиенту
 */
void MainWindow::slotReadComPort()
{
    //получение информации о состоянии светодиодов микроконтроллера
    const QByteArray data = serialPort->readAll();
    //проверка данных на соответствие формату по размеру
    if (data.size()!=1)
    {
    textField->append("data reading from port error");
    stopButtonClicked();
    }
    //разложение полученного пакета на биты
    textField->append(tr("data from COM port:%1%2%3%4%5%6%7%8").arg((data[0]&128)!=0)
                      .arg((data[0]&64)!=0).arg((data[0]&32)!=0).arg((data[0]&16)!=0)
                      .arg((data[0]&8)!=0).arg((data[0]&4)!=0).arg((data[0]&2)!=0).arg((data[0]&1)!=0));
    //отправка данных клиентам из списка
    foreach(int i,SClients.keys())
    {
    QTextStream os(SClients[i]);
    os << data;
    }
}

/**
 * @brief Установка соедиения с микроконтроллером через com-порт
 */
void MainWindow::connectCOM()
{
    //открытие com-порта
    serialPort->setPortName(serialPortBox->currentText());
    //если порт открыт удачно, то происходит настройка соединения, иначе сообщение об ошибке
    if (serialPort->open(QIODevice::ReadWrite))
    {
        //настройка cоединения с com-портом
        serialPort->setBaudRate(CBR_115200);
        serialPort->setReadBufferSize(8);
        textField->append(tr("Connected to %1 : %2, %3, %4, %5, %6").arg(serialPort->portName())
                          .arg(serialPort->baudRate()).arg(serialPort->dataBits())
                          .arg(serialPort->parity()).arg(serialPort->stopBits()).arg(serialPort->flowControl()));
    }
    else
    {
        textField->append("Open COM error: "+ serialPort->errorString());
    }
}

/**
 * @brief Закрытие com-порта
 */
void MainWindow::closeSerialPort()
{
    //закрытие com-порта, если он открыт
    if (serialPort->isOpen())
        serialPort->close();
    textField->append("COM port was disconnected");   
}

/**
 * @brief Деструктор
 */
MainWindow::~MainWindow()
{
    //остановка сервера, отключение от порта
    closeSerialPort();
    stopServer();
    //очистка памяти
    delete(serialPort);
    delete(tcpServer);
    delete ui;
}
