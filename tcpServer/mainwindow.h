#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QProcess>
#include <QCloseEvent>
#include <QElapsedTimer>
#include "QMessageBox"
#include "QTextCodec"
#include "cstring"
#include "qprocess.h"

#define _CMD_GENERAL_STATUS   0x01
#define _CMD_GDB_COMMANDLINE  0x02

typedef enum
{
    connected,
    disconnected,
    connect_error,

}connectedState_e ;


typedef struct
{
    // uint8_t ID;
    uint8_t message[1000];


}message_t;

typedef union
{
    message_t select;
    uint8_t byteArray[1001];



}message_u;

typedef union __attribute__((packed))
{
    struct name
    {
        uint16_t stlinkStatus;
        uint16_t GDBserverStatus;
        uint16_t GDB_ServerStart;
        uint16_t GDB_ServerPort;
    }select;
    uint8_t byteArray[16];

}generalStatus_u;




QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void send_msg(QTcpSocket* socketHandle, uint8_t id, uint8_t *msg, uint16_t size);
    void clientConnectedSate(connectedState_e status);

    void startGDBserver();
    void stopGDBserver();

    void closeEvent(QCloseEvent *event) override;

    void wait(int ms);

private:
    Ui::MainWindow *ui;
    QTcpServer  *_server;
    QList<QTcpSocket*>  _sockets;
    QLabel *connectStatus;
    QTcpSocket Client;

    QProcess *process;
    QTimer cmdTimer;


    uint16_t mGDBserverPort = 61234;
    generalStatus_u mGeneralStatus;
    message_u mGDBCmd;


public slots:
    //void extracted(QTcpSocket *&clientSocket);
    void onNewConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onAcceptError(QAbstractSocket::SocketError error);

    void onReadyRead();

    // void clientRead();

    void cmdTimerSlot();

private slots:
    // void on_pushButton_clicked();
    // void on_pushButton_2_clicked();
     void on_openServer_clicked();
};
#endif // MAINWINDOW_H
