#ifndef FILECHOOSER_H
#define FILECHOOSER_H

#include <QWidget>

namespace Ui {
class FileChooser;
}

class FileChooser : public QWidget
{
    Q_OBJECT
    int type;
public:
    explicit FileChooser(int type=0, QWidget *parent = 0);
    ~FileChooser();
    QString getUrl();
    
signals:
    void url_changed(QString);

private slots:
    void on_browse_clicked();
    void reemit_change();

public slots:
    void selectFile(QString &filename);
    void selectFile(const char *filename);

private:
    Ui::FileChooser *ui;
};

#endif // FILECHOOSER_H
