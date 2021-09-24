#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QSystemTrayIcon>
#include <QSqlDatabase>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:
    void on_pushButton_clicked();

    void on_actionexit_triggered();

private:
    Ui::MainWindow *ui;
QString mediadir;


QSystemTrayIcon *trayIcon;
QMenu *trayIconMenu;
QSqlDatabase db;
};

#endif // MAINWINDOW_H
