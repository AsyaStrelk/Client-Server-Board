#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QNetworkInterface>
#include <QString>
#include <vector>
#include <string>
#include <windows.h>
#include <iostream>
#include <string>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QTcpServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //com-port
    void ReadCOM();
    void WriteCOM();
    void connectCOM();
    void closeSerialPort();

    //tcp-server
    void  stopServer();
    void  startServer();

private:
    Ui::MainWindow *ui;
    QPushButton *startButton;
    QPushButton *stopButton;
    QTextEdit *textField;
    QComboBox *serialPortBox;
    //HANDLE hSerial;
    QSerialPort *serialPort;
    QTcpServer *tcpServer;
    int server_status;
    QMap<int,QTcpSocket *> SClients;

private slots:
    void startButtonClicked();
    void stopButtonClicked();
    void slotReadClient();
    void slotReadComPort();
    void slotDeleteClient();
    void addNewUser();
};
#endif // MAINWINDOW_H
