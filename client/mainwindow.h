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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton *changeIpButton;
    QPushButton *changeCheckboxButton;
    QCheckBox *redCheckBox;
    QCheckBox *blueCheckBox;
    QCheckBox *greenCheckBox;
    QTableWidget *buttonTableWidget;
    QTableWidget *lightTableWidget;
    QComboBox *changeComboBox;
    void drawUI();

    QHostAddress address;
    bool getIpAddress();
    quint16 port = 9090;
    QTcpSocket *tcpSocket;
    void tcpConnect();
    QString message;

private slots:
    void handleChangeIpButton();
    void write();
    void readyRead();
};
#endif // MAINWINDOW_H
