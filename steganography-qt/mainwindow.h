#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class FileChooser;

const int IMG_FILE = 0;
const int SECRET_FILE = 2;
const int STENOGRAM_FILE = 1;
const int DECODED_FILE = 3;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void update_preview();
    void on_save_clicked();
    void updateDetails();

    void on_decode_clicked();


    void on_max_clicked();

    void on_opti_clicked();

    void on_previewButton_clicked();

    void on_filelist_clicked();

private:
    Ui::MainWindow *ui;
    FileChooser *files[4];
    QImage img;
    QImage steg;
    int secretSize;
    QString imgUrl;
    QString secretUrl;
};

#endif // MAINWINDOW_H
