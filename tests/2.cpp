#include <QApplication>
#include <QTcpSocket>
#include <QTcpServer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QSslSocket>
#include <QBuffer>
#include <QSslKey>
#include <QSslCertificate>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class VoIPClient : public QWidget {
    Q_OBJECT

public:
    VoIPClient(QWidget *parent = nullptr)
        : QWidget(parent), server(nullptr), socket(nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        QLabel *ipLabel = new QLabel("IP Address:", this);
        ipInput = new QLineEdit("127.0.0.1", this);
        layout->addWidget(ipLabel);
        layout->addWidget(ipInput);

        QLabel *portLabel = new QLabel("Port:", this);
        portInput = new QLineEdit("12345", this);
        layout->addWidget(portLabel);
        layout->addWidget(portInput);

        QPushButton *connectButton = new QPushButton("Connect", this);
        layout->addWidget(connectButton);
        connect(connectButton, &QPushButton::clicked, this, &VoIPClient::startConnection);
        
        QPushButton *serverButton = new QPushButton("Start Server", this);
        layout->addWidget(serverButton);
        connect(serverButton, &QPushButton::clicked, this, &VoIPClient::startServer);

        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);

        audioInput = new QAudioInput(format, this);
        audioOutput = new QAudioOutput(format, this);
        buffer = new QBuffer(this);
        buffer->open(QIODevice::ReadWrite);
    }

private slots:
    void startServer() {
        int port = portInput->text().toInt();
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &VoIPClient::onNewConnection);
        if (server->listen(QHostAddress::Any, port)) {
            qDebug() << "Server started on port" << port;
        } else {
            qDebug() << "Failed to start server";
        }
    }

    void onNewConnection() {
        socket = qobject_cast<QSslSocket *>(server->nextPendingConnection());
        connect(socket, &QSslSocket::readyRead, this, &VoIPClient::onReadyRead);
        connect(socket, &QSslSocket::encrypted, this, &VoIPClient::onConnected);
        socket->setLocalCertificate("server.crt");
        socket->setPrivateKey("server.key");
        socket->startServerEncryption();
    }

    void startConnection() {
        QString host = ipInput->text();
        int port = portInput->text().toInt();
        
        socket = new QSslSocket(this);
        connect(socket, &QSslSocket::readyRead, this, &VoIPClient::onReadyRead);
        connect(socket, &QSslSocket::encrypted, this, &VoIPClient::onConnected);
        socket->connectToHostEncrypted(host, port);
    }

    void onConnected() {
        qDebug() << "Connected and encrypted!";
        startAudioStreaming();
    }

    void onReadyRead() {
        QByteArray data = socket->readAll();
        buffer->write(data);
        buffer->seek(0);
        audioOutput->start(buffer);
    }

    void startAudioStreaming() {
        connect(audioInput, &QAudioInput::readyRead, this, &VoIPClient::sendAudioData);
        audioInput->start(socket);
    }

    void sendAudioData() {
        QByteArray audioData = audioInput->readAll();
        socket->write(audioData);
    }

private:
    QTcpServer *server;
    QSslSocket *socket;
    QAudioInput *audioInput;
    QAudioOutput *audioOutput;
    QAudioFormat format;
    QBuffer *buffer;
    QLineEdit *ipInput;
    QLineEdit *portInput;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    VoIPClient client;
    client.show();
    return app.exec();
}

#include "main.moc"
