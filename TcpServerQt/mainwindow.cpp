#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TcpServer");

    textField = new QTextEdit(" ",this);
    textField->setGeometry(QRect(QPoint(20,20),QSize(420,360)));
    textField->setReadOnly(true);

    startButton = new QPushButton("Start", this);
    startButton->setGeometry(QRect(QPoint(20,420),QSize(200,40)));
    connect(startButton, SIGNAL(released()), this, SLOT(startButtonClicked()));

    stopButton = new QPushButton("Stop", this);
    stopButton->setGeometry(QRect(QPoint(240,420),QSize(200,40)));
    connect(stopButton, SIGNAL(released()), this, SLOT(stopButtonClicked()));
    stopButton->setEnabled(false);

    serialPortBox=new QComboBox(this);
    serialPortBox->setGeometry(QRect(QPoint(20,390),QSize(420,20)));

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(addNewUser()));
    serialPort = new QSerialPort(this);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(slotReadComPort()));

    //get serialports

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        serialPortBox->addItem(info.portName());
    }
}

void MainWindow::startButtonClicked()
{
    textField->clear();
    textField->append("starting server...");
    serialPortBox->setEnabled(false);
    startButton->setEnabled(false);
    stopButton->setEnabled(true);

    startServer();
    connectCOM();
}

void MainWindow::stopButtonClicked()
{
    serialPortBox->setEnabled(true);
    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    stopServer();
    closeSerialPort();
}

void MainWindow::startServer()
{
    if (!tcpServer->listen(QHostAddress::Any, 9090) && server_status==0) {
        textField->append(tcpServer->errorString());
        } else {
        server_status=1;
        textField->append(tcpServer->isListening()+" TCPSocket listen on port");
    }
}

void MainWindow::stopServer()
{
    if(server_status==1){
        foreach(int i,SClients.keys()){
        QTextStream os(SClients[i]);
        SClients[i]->close();
        SClients.remove(i);
        }
        tcpServer->close();
        server_status=0;
        textField->append("server was stopped...");
    }
}

void MainWindow::addNewUser()
{
    if(server_status==1){
        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        uint32_t idUserSocs=clientSocket->socketDescriptor();
        textField->append("new connection: "+clientSocket->peerAddress().toString());

        SClients[idUserSocs]=clientSocket;
        connect(SClients[idUserSocs],SIGNAL(readyRead()),this, SLOT(slotReadClient()));
        //connect(SClients[idUserSocs],SIGNAL(disconnected()),this, SLOT(slotDeleteClient()));
        std::cout << SClients.size();
    }
}

void MainWindow::slotDeleteClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    uint32_t idUserSocs=clientSocket->socketDescriptor();
    SClients[idUserSocs]->close();
    SClients.remove(idUserSocs);
    textField->append("client was disconnected, clent id: "+idUserSocs);
}

void MainWindow::slotReadClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idUserSocs=clientSocket->socketDescriptor();
    QByteArray data = clientSocket->readAll();
    textField->append("data from client:"+data);
    std::cout << data.size();
    serialPort->write(data);
}

void MainWindow::slotReadComPort()
{
    const QByteArray data = serialPort->readAll();
    if (data.size()!=1){
    textField->append("data reading from port error");
    stopButtonClicked();
    }
    textField->append(tr("data from COM port:%1%2%3%4%5%6%7%8").arg((data[0]&128)!=0)
                      .arg((data[0]&64)!=0).arg((data[0]&32)!=0).arg((data[0]&16)!=0)
                      .arg((data[0]&8)!=0).arg((data[0]&4)!=0).arg((data[0]&2)!=0).arg((data[0]&1)!=0));
    //send to client
    foreach(int i,SClients.keys()){
    QTextStream os(SClients[i]);
    os << data;
    }
}

void MainWindow::connectCOM()
{
    serialPort->setPortName(serialPortBox->currentText());
    if (serialPort->open(QIODevice::ReadWrite)) {
        serialPort->setBaudRate(CBR_115200);
        serialPort->setReadBufferSize(8);
        textField->append(tr("Connected to %1 : %2, %3, %4, %5, %6").arg(serialPort->portName())
                          .arg(serialPort->baudRate()).arg(serialPort->dataBits())
                          .arg(serialPort->parity()).arg(serialPort->stopBits()).arg(serialPort->flowControl()));
        } else {
        textField->append("Open COM error: "+ serialPort->errorString());
    }
}

void MainWindow::closeSerialPort()
{
    if (serialPort->isOpen())
        serialPort->close();
    textField->append("COM port was disconnected");   
}

MainWindow::~MainWindow()
{
    closeSerialPort();
    stopServer();
    delete(serialPort);
    delete(tcpServer);
    delete ui;
}




