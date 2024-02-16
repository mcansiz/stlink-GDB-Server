#include "mainwindow.h"
#include "ui_mainwindow.h"





uint8_t _CMD_GENERAL_STATUS  = 0x01;
uint8_t _CMD_GDB_COMMANDLINE = 0x02;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QLabel *appVersion;
    appVersion = new QLabel;
    appVersion->setText("ver 1.0.0");
    appVersion->setAlignment(Qt::AlignRight);

    QLabel *signature;
    signature = new QLabel;
    signature->setText("Coded by MCansiz");
    signature->setAlignment(Qt::AlignRight);

    connectStatus = new QLabel;
    connectStatus->setText(" Not Connect ");

    ui->statusbar->setVisible(true);
    ui->statusbar->addPermanentWidget(connectStatus, 1);
    ui->statusbar->addPermanentWidget(appVersion, 2);
    ui->statusbar->addPermanentWidget(signature, 2);



}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_connectServer_clicked()
{
    if (!Client.isOpen())
    {

        Client.connectToHost(ui->ServerIP->text(), ui->ServerPort->text().toInt());

        if (Client.waitForConnected(1000))
        {

            connect(&Client, SIGNAL(readyRead()), this, SLOT(clientRead()));
            connect(&Client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
                    SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

            Client.write(QByteArray::fromStdString("Start\n"));

            clientConnectedSate(connectedState_e::connected);


        }else
        {
            clientConnectedSate(connectedState_e::connect_error);

        }


    }else
    {


        clientConnectedSate(connectedState_e::disconnected);
    }

}

void MainWindow::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "changed " << socketState ;

    if ((socketState == (QAbstractSocket::UnconnectedState  ))
        || (socketState == (QAbstractSocket::ClosingState ))

        )
    {
        clientConnectedSate(connectedState_e::disconnected);

    }
}

void MainWindow::clientConnectedSate(connectedState_e status)
{

    if (status == connectedState_e::connected)
    {

        connectStatus->setText("Connected Server ");
        ui->consol->append("> Connect to Server.");
        ui->connectServer->setText("Disconnect");
    }

    else if (status == connectedState_e::disconnected)
    {
        connectStatus->setText("Disconnect Server ");
        ui->consol->append("> Disconnect Server");
        ui->connectServer->setText("Connect");

        disconnect(&Client, SIGNAL(readyRead()), this, SLOT(clientRead()));
        disconnect(&Client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
                   SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
        Client.disconnectFromHost();
        Client.close();
        Client.flush();


    }
    else if (status == connectedState_e::connect_error)
    {
        connectStatus->setText("Connect Error ");
        ui->consol->append("> Connect Error");
        ui->connectServer->setText("Connect");

        disconnect(&Client, SIGNAL(readyRead()), this, SLOT(clientRead()));
        disconnect(&Client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
                   SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
        Client.disconnectFromHost();
        Client.close();
        Client.flush();


    }

}

void MainWindow::clientRead()
{


    QByteArray Data;
    Data = Client.readAll() ;

    if (Data.at(0) == _CMD_GENERAL_STATUS)
    {
        generalStatus_u generalStatus;
        memcpy(generalStatus.byteArray, Data.data() + 1 , sizeof(generalStatus.byteArray));

        if (generalStatus.select.stlinkStatus == 0x01)
        {
            ui->StlinkStatus->setText("Stlink Connected");
            ui->StlinkStatus->setStyleSheet("QLabel {  color : green; }");
            ui->consol->append("> Stlink Connected !");

        }
        if (generalStatus.select.stlinkStatus == 0x00)
        {
            ui->StlinkStatus->setText("Stlink Not Found");
            ui->StlinkStatus->setStyleSheet("QLabel {  color : red; }");
            ui->consol->append("> Stlink Not Found !");


        }

        if (generalStatus.select.GDBserverStatus == 0x01)
        {
            ui->GDBserverStatus->setText("GDB Server Started");
            ui->GDBserverStatus->setStyleSheet("QLabel {  color : green; }");

            ui->consol->append("> Started GDB Server !");


        }
        if (generalStatus.select.GDBserverStatus == 0x00)
        {
            ui->GDBserverStatus->setText("GDB Not Started");
            ui->GDBserverStatus->setStyleSheet("QLabel {  color : red; }");
            ui->consol->append("> Stoped GDB Server !");

        }

    }

    // gdb server command line
    if (Data.at(0) == _CMD_GDB_COMMANDLINE)
    {
        message_u gdbCmd;
        memcpy(gdbCmd.byteArray,Data.data() + 1 , sizeof(gdbCmd.byteArray));

        QString msg = QString((char *)gdbCmd.select.message);
        ui->consol->append(msg);

    }


}

void MainWindow::send_msg(QTcpSocket *socketHandle, uint8_t id, uint8_t *msg, uint16_t size)
{
    QByteArray Data;

    Data.resize(size + 1);
    Data[0] = static_cast<uint8_t>(id);
    memcpy(Data.data() + 1, msg, Data.size());

    socketHandle->write(Data);
}



void MainWindow::on_startGDBServer_clicked()
{
    generalStatus_u generalStatus;

    generalStatus.select.GDB_ServerPort = ui->GDBserverPort->text().toInt();
    generalStatus.select.GDB_ServerStart = 0x01;

    send_msg(&Client,_CMD_GENERAL_STATUS,generalStatus.byteArray,sizeof(generalStatus.byteArray));
}




void MainWindow::on_stopGDBserver_clicked()
{
    generalStatus_u generalStatus;
    generalStatus.select.GDB_ServerPort = ui->GDBserverPort->text().toInt();
    generalStatus.select.GDB_ServerStart = 0x00;
    send_msg(&Client,_CMD_GENERAL_STATUS,generalStatus.byteArray,sizeof(generalStatus.byteArray));
}

