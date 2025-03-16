#include <QCoreApplication>
#include <QTcpSocket>
#include <QTcpServer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QSslSocket>
#include <QBuffer>
#include <QSslKey>
#include <QSslCertificate>

class VoIPClient : public QObject {
    Q_OBJECT

public:
    VoIPClient(bool isServer, QString host = "127.0.0.1", int port = 12345, QObject *parent = nullptr)
        : QObject(parent), server(nullptr), socket(nullptr) {
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

        if (isServer) {
            server = new QTcpServer(this);
            connect(server, &QTcpServer::newConnection, this, &VoIPClient::onNewConnection);
            server->listen(QHostAddress::Any, port);
        } else {
            socket = new QSslSocket(this);
            connect(socket, &QSslSocket::readyRead, this, &VoIPClient::onReadyRead);
            connect(socket, &QSslSocket::encrypted, this, &VoIPClient::onConnected);
            socket->connectToHostEncrypted(host, port);
        }
    }

private slots:
    void onNewConnection() {
        socket = qobject_cast<QSslSocket *>(server->nextPendingConnection());
        connect(socket, &QSslSocket::readyRead, this, &VoIPClient::onReadyRead);
        connect(socket, &QSslSocket::encrypted, this, &VoIPClient::onConnected);
        socket->setLocalCertificate("server.crt");
        socket->setPrivateKey("server.key");
        socket->startServerEncryption();
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
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    bool isServer = (argc > 1 && QString(argv[1]) == "server");
    VoIPClient client(isServer);
    return app.exec();
}

#include "main.moc"
