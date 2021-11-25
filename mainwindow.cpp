#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messenger.h"
#include <QHostAddress>
#include <QSound>
#include <QDebug>
#include <QTimer>
#include <downloadmanager.h>
#include <QSettings>
#include <QSystemTrayIcon>

#ifdef FTP
#include "ftp-server/ftpgui.h"
#include "ftp-server/CSslSocket/csslsocket.h"
    FTPGUI *ftpgui;
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QString mediadir = "./Resource/";
    QPixmap oPixmap(32,32);
    oPixmap.load ( mediadir + "phone.png");
       QIcon oIcon( oPixmap );
    trayIcon = new QSystemTrayIcon(oIcon);

    QAction *quit_action = new QAction( "Exit", trayIcon );
    connect( quit_action, SIGNAL(triggered()), this, SLOT(on_exit()) );

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction( quit_action );

    trayIcon->setContextMenu( trayIconMenu);
    trayIcon->setVisible(true);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));



        QTimer *timerClock = new QTimer(this);
    //call log system
  //  QDate dNow(QDate::currentDate());
  //  qDebug() << "Today is" << dNow.toString("dd.MM.yyyy");

  //  QTime time = QTime::currentTime();
  // QString text = time.toString("hh:mm");
}

//void MainWindow::showMessage()
//{
//    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon();
//    trayIcon->showMessage(tr("MeatTracker"), tr("Meat Timer..."), icon, 100);
//}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
        Messenger msgr(ui->lineEdit->text().toLatin1());
             QSound::play( mediadir.toLatin1() + "sounds/ec2_mono.ogg");

#ifdef FTP
    if (adminftp==0){
    ftpgui = new FTPGUI;
    adminftp=1;
    }
    if (adminftp) { ftpgui->show();}
#endif

}

void MainWindow::on_actionexit_triggered()
{
    QApplication::exit();
}

//void MainWindow::createNewsTable(){

//    db.setDatabaseName("./news.sqlite");

//    if(db.open())    {       qDebug()<<"Successful database connection";    }
//    else    {       qDebug()<<"Error: failed database connection";    }

//    QString query;

//   // qDebug() << "test" << eownerID.toLatin1();

//    query.append("CREATE TABLE IF NOT EXISTS news("
//                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
//                    "origindex VARCHAR(100)," //rcoins index then coins.sqlite is stored on usbdrive as part of key/verify
//                    "addr VARCHAR(100),"
//                    "datetime INTEGER,"
//                    "class INTEGER,"
//                    "hold INTEGER"
//                    ");");


//    QSqlQuery create;
//    create.prepare(query);

//    if (create.exec())
//    {
//        qDebug()<<"Table exists or has been created";
//    }
//    else
//    {
//        qDebug()<<"Table not exists or has not been created";
//  //      qDebug()<<"ERROR! "<< create.lastError();
//    }
//    query.clear();
//    db.close();

//}


//QUdpSocket udp;
//QByteArray buffer;
//buffer.resize(udp.pendingDatagramSize());
//QHostAddress sender;
//qint16 senderPort;
//udp.readDatagram(buffer.data(), buffer.size());
//qDebug() << buffer;

//QUdpSocket udpSocket;
////https://stackoverflow.com/questions/41662338/send-a-file-over-sockets-qt
//void initUdp() {
//    udpSocket = new QUdpSocket(this);
//    udpSocket->bind(udpPort, QUdpSocket::ShareAddress);
//    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processUdpData()));
//}

//void processUdpData() {
//     while (udpSocket->hasPendingDatagrams()) {
//          QByteArray buffer;
//          buffer.resize(udpSocket->pendingDatagramSize());
//          udpSocket->readDatagram(buffer.data(), buffer.size());
//          qDebug() << buffer;
//     }
//}



//QFile file("/Users/mathieu/Documents/test.wav");
//if(!file.exists()) qDebug() << "not found file";
//file.open(QIODevice::ReadWrite);
//QByteArray data;
//data.append(file.readAll());
//file.close();
//QUdpSocket udpsock;
//udpsock.writeDatagram(data, QHostAddress::Any, 1441);


