#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _server = new QTcpServer(this);
    process = new QProcess(this);
    connectStatus = new QLabel;

    QLabel *appVersion;
    appVersion = new QLabel;
    appVersion->setText("ver 1.0.0");
    appVersion->setAlignment(Qt::AlignRight);

    QLabel *signature;
    signature = new QLabel;
    signature->setText("Coded by MCansiz");
    signature->setAlignment(Qt::AlignRight);

    connectStatus->setText(" Not Connect ");

    ui->statusbar->setVisible(true);
    ui->statusbar->addPermanentWidget(connectStatus, 1);
    ui->statusbar->addPermanentWidget(appVersion, 2);
    ui->statusbar->addPermanentWidget(signature, 2);

    ui->GDBserverPort->setEnabled(false);

    qDebug() << "start";

    _server->listen(QHostAddress::AnyIPv4, ui->serverPort->text().toInt());

    if (_server->isListening()) {
        connect(_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
        clientConnectedSate(connectedState_e::connected);
    } else {
        clientConnectedSate(connectedState_e::connect_error);
    }

    connect(&cmdTimer, SIGNAL(timeout()), this, SLOT(cmdTimerSlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openServer_clicked()
{
    if (_server->isListening()) {
        for (QTcpSocket *socket : _sockets) {
            socket->close();
        }

        _server->close();

        clientConnectedSate(connectedState_e::disconnected);

    } else {
        _server->listen(QHostAddress::AnyIPv4, ui->serverPort->text().toInt());

        if (_server->isListening()) {
            connect(_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
            clientConnectedSate(connectedState_e::connected);
        } else {
            clientConnectedSate(connectedState_e::connect_error);
        }
    }
}

void MainWindow::send_msg(QTcpSocket *socketHandle, uint8_t id, uint8_t *msg, uint16_t size)
{
    QByteArray Data;

    Data.resize(size + 1);
    Data[0] = static_cast<uint8_t>(id);
    memcpy(Data.data() + 1, msg, Data.size());

    socketHandle->write(Data);

    socketHandle->waitForBytesWritten();

    wait(100);
}

void MainWindow::clientConnectedSate(connectedState_e status)
{
    if (status == connectedState_e::connected) {
        connectStatus->setText("Open Server ");
        ui->consol->append("> Open Server");
        ui->openServer->setText("Close");
    }

    else if (status == connectedState_e::disconnected) {
        connectStatus->setText("Close Server ");
        ui->consol->append("> Close Server");
        ui->openServer->setText("Open");
        disconnect(_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    } else if (status == connectedState_e::connect_error) {
        connectStatus->setText("Server Open ERROR ");
        ui->consol->append("> Server Open ERROR : " + _server->errorString());
        ui->openServer->setText("Open");
        disconnect(_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    }
}

void MainWindow::onNewConnection()
{
    QTcpSocket *clientSocket = _server->nextPendingConnection();
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(clientSocket,
            SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,
            SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

    _sockets.push_back(clientSocket);

    for (QTcpSocket *socket : _sockets) {
        socket->write(QByteArray::fromStdString(clientSocket->peerAddress().toString().toStdString()
                                                + " connected to server !\n"));
    }
}

void MainWindow::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState) {
        QTcpSocket *sender = static_cast<QTcpSocket *>(QObject::sender());
        _sockets.removeOne(sender);
    }
}

void MainWindow::onAcceptError(QAbstractSocket::SocketError error)
{
    qDebug() << "error " << error;
}

void MainWindow::onReadyRead()
{
    QTcpSocket *sender = static_cast<QTcpSocket *>(QObject::sender());
    QByteArray Data = sender->readAll();
    for (QTcpSocket *socket : _sockets) {
        if (socket != sender)
            socket->write(QByteArray::fromStdString(sender->peerAddress().toString().toStdString()
                                                    + ": " + Data.toStdString()));
    }

    QString str = QString(Data);
    //ui->consol->append(str);

    if (Data.at(0) == _CMD_GENERAL_STATUS) {
        generalStatus_u generalStatus;
        memcpy(generalStatus.byteArray, Data.data() + 1, sizeof(generalStatus.byteArray));

        mGDBserverPort = generalStatus.select.GDB_ServerPort;

        if (generalStatus.select.GDB_ServerStart == 0x01) {
            // start gdb server
            ui->consol->append("Starting GDB Server ..");
            startGDBserver();
        }

        if (generalStatus.select.GDB_ServerStart == 0x00) {
            // stop gdb server
            ui->consol->append("Stop GDB Server !");
            stopGDBserver();
        }
    }
}

void MainWindow::startGDBserver()
{
    QString workdir = qApp->applicationDirPath();
    QString appPath = ";" + qApp->applicationDirPath() + "\\Project\\st-link_GDB_server";
    QString database = ";" + qApp->applicationDirPath()
                       + "\\Project\\st-link_GDB_server\\st-linkCli\\Data_Base";
    QString stlink = ";" + qApp->applicationDirPath()
                     + "\\Project\\st-link_GDB_server\\st-linkCli\\bin";

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", env.value("PATH") + ";" + workdir);
    env.insert("PATH", env.value("PATH") + appPath);
    env.insert("PATH", env.value("PATH") + database);
    env.insert("PATH", env.value("PATH") + stlink);

    process->setWorkingDirectory(workdir);
    process->setProcessEnvironment(env);
    process->setProcessChannelMode(QProcess::MergedChannels);

    ui->GDBserverPort->setText(QString::number(mGDBserverPort));

    cmdTimer.start(100);

    process->terminate();
    process->kill();
    process->close();

    process->start("cmd /C ST-LINK_gdbserver.exe -p " + QString::number(mGDBserverPort)
                   + " -l 1 -d -s -cp .\\Project\\st-link_GDB_server\\st-linkCli\\bin -m 0 -k");
}

void MainWindow::stopGDBserver()
{
    process->terminate();
    process->kill();
    process->close();
    QProcess::execute("Taskkill /IM ST-LINK_gdbserver.exe /F");
    mGeneralStatus.select.GDBserverStatus = 0;

    ui->StlinkStatus->setText("GDB Not Started");
    ui->StlinkStatus->setStyleSheet("QLabel {  color : red; }");

    send_msg(_sockets[0],
             _CMD_GENERAL_STATUS,
             mGeneralStatus.byteArray,
             sizeof(mGeneralStatus.byteArray));
}

void MainWindow::cmdTimerSlot()
{
    if (process->isOpen()) {
        //QTextCodec *codec = QTextCodec::codecForName("IBM 866");

        QByteArray readdata = process->readAllStandardOutput();

        //qDebug() << ">" << QString(readdata).toLatin1().toHex();

        if (QString(readdata) != "") {
            qDebug() << "false";

            QString temp = QString(readdata);
            ui->consol->append(temp);

            if (temp.contains("ST-Link enumeration failed") == true) {
                process->close();

                ui->StlinkStatus->setText("Stlink Not Found");
                ui->StlinkStatus->setStyleSheet("QLabel {  color : red; }");

                mGeneralStatus.select.stlinkStatus = 0;
                mGeneralStatus.select.GDBserverStatus = 0;
                send_msg(_sockets[0],
                         _CMD_GENERAL_STATUS,
                         mGeneralStatus.byteArray,
                         sizeof(mGeneralStatus.byteArray));
            }

            if (temp.contains("Device connect error") == true) {
                process->close();

                ui->StlinkStatus->setText("GDB Not Started");
                ui->StlinkStatus->setStyleSheet("QLabel {  color : red; }");

                mGeneralStatus.select.stlinkStatus = 1;
                mGeneralStatus.select.GDBserverStatus = 0;
                send_msg(_sockets[0],
                         _CMD_GENERAL_STATUS,
                         mGeneralStatus.byteArray,
                         sizeof(mGeneralStatus.byteArray));
            }

            if (temp.contains("Waiting for debugger connection") == true) {
                ui->StlinkStatus->setText("Stlink Connected");
                ui->StlinkStatus->setStyleSheet("QLabel {  color : green; }");
                ui->GDBserverStatus->setText("GDB Server Started");
                ui->GDBserverStatus->setStyleSheet("QLabel {  color : green; }");

                mGeneralStatus.select.stlinkStatus = 0x01;
                mGeneralStatus.select.GDBserverStatus = 0x01;
                send_msg(_sockets[0],
                         _CMD_GENERAL_STATUS,
                         mGeneralStatus.byteArray,
                         sizeof(mGeneralStatus.byteArray));
            }
            if (temp.contains("Exit.") == true) {

                ui->consol->append("> Startin GDB Server ..");
                wait(1000);
                startGDBserver();

            }



            qDebug() << readdata.size();
            memset(mGDBCmd.byteArray, 0, sizeof(mGDBCmd.byteArray));
            memcpy(mGDBCmd.byteArray, readdata.data(), readdata.size());
            send_msg(_sockets[0], _CMD_GDB_COMMANDLINE, mGDBCmd.byteArray, readdata.size() + 1);

            // send all data
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "Closing";
    process->terminate();
    process->kill();
    process->close();

    QProcess::execute("Taskkill /IM ST-LINK_gdbserver.exe /F");

    event->accept();
}

void MainWindow::wait(int ms)
{
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < ms)
        QCoreApplication::processEvents();
}
