#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QListWidgetItem>

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
    void cl();

private slots:
    bool load();
    bool save();
    void add();
    void rm();
    void ren();
    void mod();
    void about();
    void squit();
    void change(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QString cur_note;
    QDir home;
    bool moded;
};

#endif // MAINWINDOW_H
