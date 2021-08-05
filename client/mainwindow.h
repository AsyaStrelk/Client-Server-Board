#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QUdpSocket>
#include <QInputDialog>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QCheckBox>
#include <QTcpSocket>
#include <QTableWidget>
#include <QTextEdit>
#include <QComboBox>
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

private slots:
    void handleChangeIpButton();
    void write();
private:
    QPushButton *changeIpButton;
    QPushButton *changeCheckboxButton;
    QCheckBox *redCheckBox;
    QCheckBox *blueCheckBox;
    QCheckBox *greenCheckBox;
    QTableWidget *buttonTableWidget;
    QTableWidget *lightTableWidget;
    QComboBox *changeComboBox;

private slots:
    void readyRead();

private:
    QHostAddress address;
    void getIpAddress();
    quint16 port;
    QTcpSocket *tcpSocket;
    void tcpConnect();
private:
    void drawUI();
};
#endif // MAINWINDOW_H
