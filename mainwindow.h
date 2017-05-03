#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "jdata.h"

#include <QMainWindow>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void on_actionOpen_file_triggered();
    void on_actionQuit_triggered();
    void focus_changed(QString);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void populate_window();
    Ui::MainWindow *ui;
    JData jdata;
    std::vector<QJsonObject> cards;
};

#endif // MAINWINDOW_H
