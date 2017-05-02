#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jdata.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    jdata{}
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populate_window()
{
    QStringList titles{jdata.get_card_titles()};
    qDebug() << "populate_window: titles = ";
    foreach (QString s, titles) {
        qDebug() << s;
        QLabel* l = new QLabel(s);
        ui->card_list->addWidget(l);
    }
    qDebug() << "populate window: Done";
}

void MainWindow::on_actionOpen_file_triggered()
{
    ui->statusBar->showMessage("Opening file...");
    qDebug() << "Opening file";
    QString file_name =
            QFileDialog::getOpenFileName(this,
                                         "Choose a JSON to open",
                                         "",
                                         tr("Json Files (*.json *.JSON);;Text files (*.txt)")
                                         );
    qDebug() << "Open_file: chosen file '" << file_name << "'";

    if (file_name != "")
    {
        jdata.open_file(file_name);
        populate_window();
    }
}

void MainWindow::on_actionQuit_triggered()
{
    qDebug() << "Quit: quitting";
    jdata.close_file();
    QCoreApplication::quit();
}
