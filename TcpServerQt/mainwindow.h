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
    
    //запуск сервера
    void  stopServer();
    //остановка сервера
    void  startServer();    
    
    //подключение по com-порту
    void connectCOM();
    //закрытие com-порта
    void closeSerialPort();
    //чтение данных с микроконтроллера
    void ReadCOM();
    //отправка данных на микроконтроллер
    void WriteCOM();
    
private:
    Ui::MainWindow *ui;
    //кнопки запуска и остановки сервера
    QPushButton *startButton;
    QPushButton *stopButton;
    //текстовое поле состояния сервера 
    QTextEdit *textField;
    //список доступных com-портов
    QComboBox *serialPortBox;
    
    //выбранный com-порт
    QSerialPort *serialPort;
    
    //tcp-сервер
    QTcpServer *tcpServer;
    //статус сервера on/off(1/0)
    int server_status; 
    //список клиентов, подключенных к серверу
    QMap<int,QTcpSocket *> SClients;

private slots:
    //обработчик нажатия на кнопку старта сервера
    void startButtonClicked();
    //обработчик нажатия на кнопку остановки сервера
    void stopButtonClicked();
    //получение данных от клиента
    void slotReadClient();
    //получение данных от микроконтроллера
    void slotReadComPort();
    //удаление клиента
    void slotDeleteClient();
    //добавление клиента
    void addNewUser();
};
#endif // MAINWINDOW_H
