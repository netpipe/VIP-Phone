#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QVector>
#include <QList>
#include <QFormLayout>
#include "filechooser.h"

#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>
#include <QMessageBox>
#include <chrono>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString list[]={"Obraz:", "Steganogram:", "Plik do kodowania:", "Wyciągniete:"};
    for(int i=0; i<4; i++){
        this->files[i]=new FileChooser((i+1)%2);
        connect(this->files[i], SIGNAL(url_changed(QString)), this, SLOT(updateDetails()));
        ((QFormLayout*)ui->files_lay->layout())->addRow(new QLabel(list[i]), this->files[i]);
    }
    this->files[0]->selectFile(":/dog.jpg");
    this->files[1]->selectFile("out.png");
    this->files[2]->selectFile(":/kompas256.jpg");
    this->files[3]->selectFile("decoded.jpg");
    imgUrl="";

    connect(ui->r, SIGNAL(valueChanged(int)), this, SLOT(updateDetails()));
    connect(ui->g, SIGNAL(valueChanged(int)), this, SLOT(updateDetails()));
    connect(ui->b, SIGNAL(valueChanged(int)), this, SLOT(updateDetails()));

    connect(ui->scroll1->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->scroll2->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->scroll1->horizontalScrollBar(), SIGNAL(valueChanged(int)), ui->scroll2->horizontalScrollBar(), SLOT(setValue(int)));
    connect(ui->scroll2->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->scroll1->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->scroll2->horizontalScrollBar(), SIGNAL(valueChanged(int)), ui->scroll1->horizontalScrollBar(), SLOT(setValue(int)));

    connect(ui->zoom, SIGNAL(valueChanged(int)), this, SLOT(update_preview()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

const char *units[]={
    "B","KiB","MiB","GiB", "TiB"
};

void MainWindow::updateDetails(){
    if(this->files[0]->getUrl() != this->imgUrl){
        this->img = QImage(this->files[0]->getUrl());
        this->imgUrl = this->files[0]->getUrl();
    }
    int bits[3];
    bits[0] = ui->r->value();
    bits[1] = ui->g->value();
    bits[2] = ui->b->value();
    int b = bits[0]+bits[1]+bits[2];
    int pixels = img.width()*img.height();
    ui->resolution->setText(QString("%1 × %2").arg(img.width()).arg(img.height()));
    ui->pixels->setText(QString("%1").arg(pixels));
    int iCap = b*pixels/8;
    float cap = iCap;
    int unit = 0;
    while(cap>1024.0){
        cap/=1024.0;
        unit++;
    }
    ui->capacity->setText(QString("%1 %2").arg(QString::number(cap)).arg(units[unit]));


    if(this->files[SECRET_FILE]->getUrl() != this->secretUrl){
        QFile file(this->files[SECRET_FILE]->getUrl());
        file.open(QIODevice::ReadOnly);
        QByteArray data = file.readAll();
        this->secretSize = data.size();
        this->secretUrl = this->files[SECRET_FILE]->getUrl();
        float cap = secretSize;
        int unit = 0;
        while(cap>1024.0){
            cap/=1024.0;
            unit++;
        }
        ui->size->setText(QString("%1 %2").arg(QString::number(cap)).arg(units[unit]));
    }
    if(iCap -4 < this->secretSize){
        ui->save->setEnabled(false);
        ui->previewButton->setEnabled(false);
        ui->fill->setValue(0);
    }
    else{
        ui->save->setEnabled(true);
        ui->previewButton->setEnabled(true);
        ui->fill->setValue(this->secretSize*100.0/iCap);
    }
    int zoom = ui->zoom->value();
    ui->imgIn->setPixmap(QPixmap::fromImage(this->img.scaled(img.width()*zoom, img.height()*zoom)));
}

void MainWindow::update_preview(){
    int zoom = ui->zoom->value();
    ui->imgIn->setPixmap(QPixmap::fromImage(img.scaled(img.width()*zoom, img.height()*zoom)));
    if(this->steg.width()>0){
        ui->imgOut->setPixmap(QPixmap::fromImage(steg.scaled(steg.width()*zoom, steg.height()*zoom)));
    }
}

const uchar mask[]={
    0b00000001,    0b00000011,    0b00000111,    0b00001111,
    0b00011111,    0b00111111,    0b01111111,    0b11111111
};
const uchar cmask[]={
    0b11111111,    0b11111110,    0b11111100,    0b11111000,
    0b11110000,    0b11100000,    0b11000000,    0b10000000,
    0b00000000
};


int nSeed = 5323;
void rand_reset(int seed = 5323){
    nSeed = seed;
}
int pseudorandom(){
    nSeed = (8253729 * nSeed + 2396403);
    return (nSeed  % 32767) & 0x7fffff;
}

union Pixel{
    uint rgba;
    uchar bit[4];
    struct{
        //uchar a;
        uchar r;
        uchar g;
        uchar b;
        uchar a;
    } col;
};

auto start = std::chrono::high_resolution_clock::now();


void MainWindow::on_save_clicked()
{
    on_previewButton_clicked();
    this->steg.save(this->files[STENOGRAM_FILE]->getUrl());
}

void MainWindow::on_previewButton_clicked()
{
    QImage tmpImg(this->files[IMG_FILE]->getUrl());
    QImage img = tmpImg.convertToFormat(QImage::Format_ARGB32);

    int bits[3];
    bits[2] = ui->r->value();
    bits[1] = ui->g->value();
    bits[0] = ui->b->value();
    uint dataCapacity = img.width()*img.height()*(bits[0]+bits[1]+bits[2])/8;

    QFile file(this->files[SECRET_FILE]->getUrl());
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    uint size = data.size() + 4;
    uint tmp = size;
    QString ext = this->files[IMG_FILE]->getUrl().split(".").last();
    QByteArray md5 = QCryptographicHash::hash(data,QCryptographicHash::Md5);
    for(int i=0; i<4; i++){
        data.push_front((char)((tmp&0xff000000)>>24));
        tmp=tmp<<8;
    }

    uchar *secData = (uchar*) data.data();
    qDebug()<<"in memory: "<<*reinterpret_cast<int*>(secData);
    qDebug()<<"hex: "<<QString::number(secData[0], 16)<<";"<<QString::number(secData[1], 16)<<";"<<QString::number(secData[2], 16)<<";"<<QString::number(secData[3], 16);
    qDebug()<<"dec: "<<QString::number(secData[0], 10)<<";"<<QString::number(secData[1], 10)<<";"<<QString::number(secData[2], 10)<<";"<<QString::number(secData[3], 10);
    if(size > dataCapacity){
        QMessageBox::warning(this, "Błąd", "Plik nie mieści się w obrazie przy tej konfiguracji");
        return ;
    }

    qDebug()<<"fe: "<<int(data.at(0))<<int(data.at(data.size()-1));
    size+=md5.size();
    qDebug()<<md5.size()<<";"<<md5.toHex();

    data.append( md5 );


    bool rand = ui->rand_is->isChecked();
    rand_reset(ui->rand_seed->value());
    //char used[img.width()][img.height()];
    char **used;
    if(rand){
        used = new char*[img.width()];
        for(int i=0; i<img.width(); i++){
            used[i]=new char[img.height()];
            memset(used[i], 0, img.height());
        }
    }

    int x=0;
    int y=0;
    uint byte=0;
    int left = 7;
    qDebug()<<size;
    qDebug()<<"Coding";
    start = std::chrono::high_resolution_clock::now();
    while (byte<=size){
        Pixel p;
        p.rgba = img.pixel(x,y);
        uchar add[3];
        memset(add,0,3);
        for(int c = 0; c<3; c++){
            for(int bit = 0; bit<bits[c]; bit++){
                uchar v = (secData[byte]>>left)&1;
                add[c] += v<<bit;
                left--;
                if(left==-1){
                    left = 7;
                    byte++;
                    if(byte == size)
                        goto end;
                }
            }
        }
        end:
        p.col.r = (p.col.r&cmask[bits[0]]) | (add[0]);
        p.col.g = (p.col.g&cmask[bits[1]]) | (add[1]);
        p.col.b = (p.col.b&cmask[bits[2]]) | (add[2]);
        img.setPixel(x,y,p.rgba);
        if(rand){
            used[x][y]=true;
            x = pseudorandom()%img.width();
            y = pseudorandom()%img.height();
            while(used[x][y]){
                x++;
                if(x==img.width()){
                    x=0;
                    y++;
                    if(y==img.height()){
                        y=0;
                        x=0;
                    }

                }
            }
        }
        else{
            x++;
            if(x==img.width()){
                x=0;
                y++;
                if(y==img.height()){
                    qDebug()<<"EXIT: jak?";
                    return ;
                }
            }
        }
    }
    auto end =  std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now()-start
        ).count();
    qDebug()<<"Coded: "<<end<<"ms";
    this->steg = img;
    int zoom = ui->zoom->value();
    ui->imgOut->setPixmap(QPixmap::fromImage(steg.scaled(steg.width()*zoom, steg.height()*zoom)));
    if(rand){
        for(int i=0; i<img.width(); i++)
            delete used[i];
        delete used;
    }

    QString msg = "Kodowanie danych w obrazie zostało zakończone<br/>Czas kodowania:%1 ms";
    if(ui->popups->isChecked())
        QMessageBox::information(this, "Kodowanie zakończone", msg.arg(end));
}



void MainWindow::on_decode_clicked()
{
    QImage img(this->files[STENOGRAM_FILE]->getUrl());
    if(img.width()==0){
        QMessageBox::information(this, "Błąd wczytytania pliku", "Plik nie istnieje lub jest niepoprawnym plikiem graficznym");
        return;
    }
    img = img.convertToFormat(QImage::Format_ARGB32);
    qDebug()<<"Pierwszy RGB: "<<QString::number(img.pixel(0,0),16);

    int bits[3];
    bits[2] = ui->r->value();
    bits[1] = ui->g->value();
    bits[0] = ui->b->value();

    bool rand = ui->rand_is->isChecked();
    rand_reset(ui->rand_seed->value());
    char **used;
        if(rand){
        used = new char*[img.width()];
        for(int i=0; i<img.width(); i++){
            used[i]=new char[img.height()];
            memset(used[i], 0, img.height());
        }
    }

    int x=0;
    int y=0;
    int size = 0x7fffffff;
    int byte = 0;
    int bit = 0;
    uchar *data = new uchar[4];
    memset(data, 0, 4);
    uchar val=0;
    qDebug()<<"Decoding...";
    start = std::chrono::high_resolution_clock::now();
    while(byte<=size){
        Pixel pixel;
        pixel.rgba = img.pixel(x,y);
        for(int c=0; c<3; c++){
            for(int j=0; j<bits[c]; j++){
                val *= 2;
                val += pixel.bit[c] %2;
                pixel.bit[c] /= 2;
                bit++;
                if(bit==8){
                    bit=0;
                    data[byte]=val;
                    val=0;
                    byte++;
                    if(byte > size){
                        goto end;
                    }
                    if(byte==4){
                        int *s = reinterpret_cast<int*>(data);
                        size = *s;
                        qDebug()<<*reinterpret_cast<int*>(data);
                        delete data;
                        size+=16;
                        if(size>img.width()*img.height()*(bits[0]+bits[1]+bits[2])){
                            QMessageBox::warning(this, "Błąd", "Konfiguracja niepoprawna lub brak zakodowanego pliku");
                            return ;
                        }
                        data = new uchar[size];
                        //return ;
                    }
                }
            }
        }
        if(rand){
            used[x][y]=true;
            x = pseudorandom()%img.width();
            y = pseudorandom()%img.height();
            while(used[x][y]){
                x++;
                if(x>=img.width()){
                    x=0;
                    y++;
                    if(y==img.height()){
                        x=0;
                        y=0;
                    }
                }
            }
        }
        else{
            x++;
            if(x==img.width()){
                x=0;
                y++;
                if(y==img.height()){
                    qDebug()<<"EXIT: jak?";
                    return ;
                }
            }
        }
    }
    end:

    QByteArray copy;
    copy.append((char*) data+4, size-20);
    qDebug()<<"fe: "<<int(copy.at(0))<<int(copy.at(copy.size()-1));
    QByteArray md5 = QCryptographicHash::hash(copy,QCryptographicHash::Md5);
    QByteArray md5Loaded;
    md5Loaded.append((char*)data+size-16, 16);
    qDebug()<<"decoded: "<<md5.toHex();
    qDebug()<<"loaded: "<<md5Loaded.toHex();
    auto end =  std::chrono::duration_cast<std::chrono::milliseconds>( \
                std::chrono::high_resolution_clock::now()-start \
        ).count();
    qDebug()<<"Decoded: "<<end<<"ms";
    if(md5 == md5Loaded){
        QString msg("Dekodowanie danych z obrazu zostało zakończone pomyślnie<br/>Czas dekodowania: %1 ms");
        if(ui->popups->isChecked())
            QMessageBox::information(this, "Dekodowanie zakończone pomyślnie", msg.arg(end));
        QFile file(this->files[DECODED_FILE]->getUrl());
        file.open(QIODevice::WriteOnly);
        file.write((char*)data+4, size);
    }
    else{
        QMessageBox::warning(this, "Błąd", "Suma kontrolna nie zgadza się - niepoprawna konfiguracja");
    }
    delete [] data;
    if(rand){
        for(int i=0; i<img.width(); i++)
            delete [] used[i];
        delete used;
    }
}



void MainWindow::on_max_clicked()
{
    ui->r->setValue(8);
    ui->g->setValue(8);
    ui->b->setValue(8);
}

void MainWindow::on_opti_clicked()
{
    ui->r->setValue(3);
    ui->g->setValue(3);
    ui->b->setValue(3);
}



void MainWindow::on_filelist_clicked()
{
    QString lista="Pliki w zasobach programu:<br/>"\
                  "<b>:/dog.jpg</b> – duży plik jpg<br/>"\
                  "<b>:/kompas.png</b> – PNG 512x512<br/>"\
            "<b>:/kompas90.jpg</b> – jpg 512x512 90%<br/>"\
            "<b>:/kompas50.jpg</b> – jpg 512x512 50%<br/>"\
            "<b>:/kompas25.jpg</b> – jpg 512x512 25%<br/>"\
            ;
    QMessageBox::information(this, "Dostępne pliki", lista);
}
