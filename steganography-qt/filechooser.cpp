#include "filechooser.h"
#include "ui_filechooser.h"

#include <QFileDialog>

const char *FILTER[]={
    "Any(*.*)",
    "Image Files (*.png *.jpg *.bmp)"
};

FileChooser::FileChooser(int type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileChooser)
{
    ui->setupUi(this);
    this->type = type;
    connect(ui->url, SIGNAL(textChanged(QString)), this, SLOT(reemit_change()));
}

FileChooser::~FileChooser()
{
    delete ui;
}

void FileChooser::reemit_change(){
    emit url_changed(ui->url->text());
}

QString FileChooser::getUrl(){
    return this->ui->url->text();
}

void FileChooser::selectFile(QString &filename){
    ui->url->setText(filename);
}

void FileChooser::selectFile(const char *filename){
    ui->url->setText(filename);
}

void FileChooser::on_browse_clicked()
{
    QString url = this->ui->url->text();
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), url, tr(FILTER[this->type]));
    this->ui->url->setText(fileName);
}
