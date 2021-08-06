#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QTableWidget>
#include <QComboBox>
#include <QInputDialog>
#include <QRegExpValidator>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QAbstractSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Номер порта
#define _port 9090;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    // Кнопки
    QPushButton *changeIpButton;
    QPushButton *changeCheckboxButton;
    // Таблицы
    QTableWidget *buttonTableWidget;
    QTableWidget *lightTableWidget;
    // Комбобокс
    QComboBox *changeComboBox;
    // Функция для рисования графического интерфейса
    void drawUI();

    // ip адрес сервера
    QHostAddress address;
    // Функция для получения ip адреса сервера
    bool getIpAddress();
    // Номер порта
    quint16 port;
    // TCP сокет
    QTcpSocket *tcpSocket;
    // Функция для подключения к серверу по протоколу tcp
    void tcpConnect();
    // Сообщение от сервера
    QString message;

private slots:
    // Функция для обработки нажатия на кнопку "Connect"
    void handleChangeIpButton();
    // Функция для отправки сообщения на сервер
    void write();
    // Функция для чтения сообщения с сервера
    void readyRead();
};
#endif // MAINWINDOW_H
