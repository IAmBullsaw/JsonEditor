#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jdata.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QWidget>
#include <QFrame>
#include "clcard.h"
#include "constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    jdata{}
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::remove(QLayout* layout) {
    QLayoutItem* child;
    while (ui->card_list->count() != 0) {
        child = layout->takeAt(0);
        if (QWidget* w = child->widget())
            delete w;
        if (QLayout* l = child->layout())
            remove(l);
    }

    ui->card_list->update();
}

void MainWindow::populate_window() {
    std::vector<QJsonObject> new_cards = jdata.get_cards();
    remove(ui->card_list->layout());
    cards = new_cards;
    qDebug() << "populate_window: titles = ";
        foreach (QJsonObject o, new_cards) {
        QString s = o.value(c_title).toString();
        qDebug() << s;

        //Adds one line before card
        QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        ui->card_list->addWidget(line);

        // create and add card
        ClCard* c = new ClCard(s,this);
        ui->card_list->addWidget(c);

        c->setMinimumWidth(c_card_minimum_width);

        connect(c, SIGNAL(focus_changed(QString)),
                this, SLOT(focus_changed(QString)));

    }
    ui->card_list->update();
    qDebug() << "populate window: Done";
}

bool MainWindow::confirmThrowEdits() {
    return true;
}

void MainWindow::on_actionOpen_file_triggered() {

    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = confirmThrowEdits();
    }

    if (confirm) {
        ui->statusBar->showMessage("Opening file...");
        qDebug() << "Opening file";
        QString file_name =
            QFileDialog::getOpenFileName(this,
                                         "Choose a JSON to open",
                                         "",
                                         tr("Json Files (*.json *.JSON);;Text files (*.txt)")
                                         );
        qDebug() << "Open_file: chosen file '" << file_name << "'";

        if (file_name != "") {
            jdata.open_file(file_name);
            populate_window();
        }
    }
}

void MainWindow::on_actionQuit_triggered() {
    qDebug() << "Quit: quitting";
    jdata.close_file();
    QCoreApplication::quit();
}

void MainWindow::focus_changed(QString label)
{
    qDebug() << "focus_changed: listening";
    QJsonObject new_card;
    foreach (QJsonObject o, cards) {
        QString s = o.value(c_title).toString();
        if (s == label) {
            new_card = o;
            qDebug() << "found card: " + s;
            break;
        }
    }
    qDebug() << new_card;
    ui->lineEdit_title->setText( new_card.value(c_title).toString() );
    ui->lineEdit_description->setText( new_card.value(c_description).toString() );
    ui->lineEdit_id->setText( QString::number(new_card.value(c_id).toInt()) );

    QJsonArray arr = new_card.value(c_images).toArray();
    qDebug() << arr;
    QString s{""};
    foreach (QJsonValue v, arr) {
        s += v.toString() + ", ";
    }
    s.resize(s.size()-2);
    ui->lineEdit_images->setText( s );

    arr = new_card.value(c_text).toArray();
    qDebug() << arr;
    s = "";
    foreach (QJsonValue v, arr) {
        s += v.toString() + "\n\n";
    }
    s.resize(s.size()-2);

    ui->textEdit_text->setText( s );

}

void MainWindow::on_pushButton_saveCard_clicked()
{
    qDebug() << "saveCard_clicked: haha";
}
