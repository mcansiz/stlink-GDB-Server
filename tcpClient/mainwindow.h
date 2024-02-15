#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

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


private slots:
    void on_connectServer_clicked();
    void clientRead();
    void on_startGDBServer_clicked();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);


    void on_stopGDBserver_clicked();

private:
    Ui::MainWindow *ui;
    QLabel *connectStatus;
    QTcpSocket Client;

};
#endif // MAINWINDOW_H
