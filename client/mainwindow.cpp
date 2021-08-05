#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget::setFixedSize(QSize(400,250));
    QWidget::setWindowTitle ("Not Connected");
    drawUI();
}

void MainWindow::drawUI()
{
    changeIpButton = new QPushButton("Connect", this);
    changeIpButton->setGeometry(QRect(QPoint(10,10), QSize(150, 50)));
    connect(changeIpButton, SIGNAL(released()), this, SLOT(handleChangeIpButton()));

//    redCheckBox = new QCheckBox("Красный светодиод", this);
//    redCheckBox->setGeometry(QRect(QPoint(10,80), QSize(100, 20)));
//    redCheckBox->setChecked(false);
//    redCheckBox->setDisabled(true);
//    blueCheckBox = new QCheckBox("Синий светодиод", this);
//    blueCheckBox->setGeometry(QRect(QPoint(10,100), QSize(100, 20)));
//    blueCheckBox->setChecked(false);
//    blueCheckBox->setDisabled(true);
//    greenCheckBox = new QCheckBox("Зеленый светодиод", this);
//    greenCheckBox->setGeometry(QRect(QPoint(10,120), QSize(100, 20)));
//    greenCheckBox->setChecked(false);
//    greenCheckBox->setDisabled(true);

    changeComboBox = new QComboBox(this);
    changeComboBox->addItem("");
    changeComboBox->addItem("Red");
    changeComboBox->addItem("Blue");
    changeComboBox->addItem("Green");
    changeComboBox->setCurrentIndex(0);
    changeComboBox->setGeometry(QRect(QPoint(10,100), QSize(150, 20)));

    changeCheckboxButton = new QPushButton("Change", this);
    changeCheckboxButton->setDisabled(true);
    changeCheckboxButton->setGeometry(QRect(QPoint(10,130), QSize(150, 50)));
    connect(changeCheckboxButton, SIGNAL(released()), this, SLOT(write()));

    lightTableWidget = new QTableWidget(3,2,this);
    lightTableWidget->setHorizontalHeaderLabels(QStringList() << tr("Color") << ("State"));
    lightTableWidget->setGeometry(QRect(QPoint(170,10), QSize(225, 120)));
    lightTableWidget->setItem(0, 0, new QTableWidgetItem("Red"));
    lightTableWidget->setItem(1, 0, new QTableWidgetItem("Blue"));
    lightTableWidget->setItem(2, 0, new QTableWidgetItem("Green"));
    lightTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(2, 1, new QTableWidgetItem("?"));
    buttonTableWidget = new QTableWidget(2,2,this);
    buttonTableWidget->setHorizontalHeaderLabels(QStringList() << tr("Button") << ("State"));
    buttonTableWidget->setGeometry(QRect(QPoint(170,140), QSize(225, 90)));
    buttonTableWidget->setItem(0, 0, new QTableWidgetItem("Button 1"));
    buttonTableWidget->setItem(1, 0, new QTableWidgetItem("Button 2"));
    buttonTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    buttonTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
}

void MainWindow::getIpAddress()
{
    bool ok = false;
    while (!ok)
    {
        QString text = QInputDialog::getText(this, tr("IP-Address"), tr("IP-Address:"), QLineEdit::Normal, "", &ok);
        if (!ok)
        {
           return;
        }
        QString ipRange = "(0?\\d?\\d|1\\d\\d|2[0-4]\\d|25[0-5])";
        QRegExp ipRegex ("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
        ok =  ipRegex.exactMatch(text) ? true : false;
        if (!ipRegex.exactMatch(text))
        {
             std::cout << "Wrong IP-Address" << std::endl;
             QMessageBox msgBox;
             msgBox.setWindowTitle("Wrong IP-Address");
             msgBox.setText("Wrong IP-Address");
             msgBox.exec();
             ok = false;
        }
        else {
            address = QHostAddress(text);
        }
    }
}

void MainWindow::tcpConnect()
{
    tcpSocket = new QTcpSocket(this);
    port = 9090;
    tcpSocket->connectToHost(address, port);
    if (tcpSocket->waitForConnected(1000))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("");
        msgBox.setText("Connected");
        msgBox.exec();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("");
        msgBox.setText("Connection error");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void MainWindow::readyRead()
{
    QByteArray data = tcpSocket->readAll();
    QString message = tr("data from COM port:%1%2%3%4%5%6%7%8").arg((data[0]&128)!=0)
              .arg((data[0]&64)!=0).arg((data[0]&32)!=0).arg((data[0]&16)!=0)
              .arg((data[0]&8)!=0).arg((data[0]&4)!=0).arg((data[0]&2)!=0).arg((data[0]&1)!=0);
    message.remove(0,19);
    if (message[3] == "1" and message[4] == "0")
    {
        buttonTableWidget->setItem(0, 1, new QTableWidgetItem("Off"));
        buttonTableWidget->setItem(1, 1, new QTableWidgetItem("On"));
    }
    else if (message[3] == "0" and message[4] == "1")
    {
        buttonTableWidget->setItem(0, 1, new QTableWidgetItem("On"));
        buttonTableWidget->setItem(1, 1, new QTableWidgetItem("Off"));
    }
    else
    {
        buttonTableWidget->setItem(0, 1, new QTableWidgetItem("Off"));
        buttonTableWidget->setItem(1, 1, new QTableWidgetItem("Off"));
    }

    lightTableWidget->setItem(0, 1, new QTableWidgetItem(message[7] == "0" ? "Off" : "On"));
    lightTableWidget->setItem(1, 1, new QTableWidgetItem(message[5] == "0" ? "Off" : "On"));
    lightTableWidget->setItem(2, 1, new QTableWidgetItem(message[6] == "0" ? "Off" : "On"));
}

void MainWindow::write()
{
    QByteArray data;
    if (changeComboBox->currentText() == "Red")
    {
        data = "q";
    }
    else if (changeComboBox->currentText() == "Blue")
    {
        data = "t";
    }
    else if (changeComboBox->currentText() == "Green")
    {
        data = "z";
    }
    else
        return;
    tcpSocket->write(data);
    changeComboBox->setCurrentIndex(0);
}

void MainWindow::handleChangeIpButton()
{
    getIpAddress();
    tcpConnect();

    QWidget::setWindowTitle (address.toString());
    changeCheckboxButton->setDisabled(false);
    buttonTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    buttonTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(0, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(1, 1, new QTableWidgetItem("?"));
    lightTableWidget->setItem(2, 1, new QTableWidgetItem("?"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
