#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jdata.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QWidget>
#include <QFrame>
#include <QMessageBox>
#include <QCloseEvent>
#include "clcard.h"
#include "constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    jdata{},
    prev_edit_text_s{0},
    first_edit{false}
{
    ui->setupUi(this);

    connect(ui->lineEdit_description, SIGNAL(textEdited(QString)),
            this, SLOT(onEditCard(QString const&)));
    connect(ui->lineEdit_id, SIGNAL(textEdited(QString)),
            this, SLOT(onEditId(QString const&)));
    connect(ui->checkBox_unread,SIGNAL(stateChanged(int)),
            this,SLOT(onEditCheckbox(int)));
    connect(ui->lineEdit_images, SIGNAL(textEdited(QString)),
            this, SLOT(onEditCard(QString const&)));
    connect(ui->lineEdit_title, SIGNAL(textEdited(QString)),
            this, SLOT(onEditCard(QString const&)));
    connect(ui->plainTextEdit_text, SIGNAL(textChanged()),
            this, SLOT(onEditCard()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onEditCheckbox(int const& in) {
    jdata.setEdited(true);
    qDebug() << "onEditCheckbox: " + QString::number(in);
}

void MainWindow::reset_edited()
{
    qDebug() << "resetting edited";
    jdata.setEdited(false);
    jdata.setId_edited(false);
}

void MainWindow::onEditCard(QString const& string) {
    jdata.setEdited(true);
    qDebug() << "onEditCard: " + string;
}

void MainWindow::onEditId(QString const& string) {
    jdata.setId_edited(true);
    jdata.setEdited(true);
    qDebug() << "onEditCard: id_edited " + string;
}

void MainWindow::onEditCard() {
    if (first_edit) {
        first_edit = false;
    } else {
        jdata.setEdited(true);
    }
    qDebug() << "onEditText";
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
    empty_fields();
    reset_edited();
    qDebug() << "populate window: Done";
}

void MainWindow::add_card_to_list(ClCard* const& c) {
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    ui->card_list->addWidget(line);

    ui->card_list->addWidget(c);
    connect(c, SIGNAL(focus_changed(QString)),
            this, SLOT(focus_changed(QString)));
}

/*!
 * \brief MainWindow::confirmThrowEdits
 * \return true if pressing OK
 */
bool MainWindow::yesNoDialogue(const QString &question) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("File has been edited!");
    msgBox.setText(question);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

void MainWindow::on_actionOpen_file_triggered() {

    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = yesNoDialogue("Do you want to open new file and loose your changes?");
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
    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = yesNoDialogue("Do you want to close the file and loose all changes?");
    }
    if (confirm) {
        empty_fields();
        reset_edited();
        jdata.close_file();
        QCoreApplication::quit();
    }
}

void MainWindow::focus_changed(QString label)
{
    qDebug() << "focus_changed: listening";
    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = yesNoDialogue("Do you want to close the file and loose all changes?");
    }
    if (confirm) {
        QJsonObject new_card;
        foreach (QJsonObject o, jdata.get_cards()) {
            QString s = o.value(c_title).toString();
            if (s == label) {
                new_card = o;
                break;
            }
        }
        qDebug() << new_card;
        ui->lineEdit_title->setText( new_card.value(c_title).toString() );
        ui->lineEdit_description->setText( new_card.value(c_description).toString() );
        ui->lineEdit_id->setText( QString::number(new_card.value(c_id).toInt()) );

        QJsonArray arr = new_card.value(c_images).toArray();
        qDebug() << "focus_changed: arr = " << arr;
        QString s{""};
        foreach (QJsonValue v, arr) {
            s += v.toString() + ", ";
        }
        s.resize(s.size()-2);
        ui->lineEdit_images->setText( s );

        arr = new_card.value(c_text).toArray();
        qDebug() << "focus_changed: text = " << arr;
        s = "";
        foreach (QJsonValue v, arr) {
            s += v.toString() + "\n\n";
        }
        s.resize(s.size()-2);

        first_edit = true; // don't want to trigger editText slot too early...
        ui->plainTextEdit_text->document()->setPlainText( s );
        reset_edited();
    }
}

void MainWindow::on_pushButton_saveCard_clicked()
{
    bool confirm{true};
    if (jdata.isId_edited() ) {
        confirm = yesNoDialogue("ID on this card has changed, are you sure you want to replace it?");
    }
    if (confirm) {
        qDebug() << "saveCard_clicked: making new card";
        QJsonObject card;
        QJsonValue id(ui->lineEdit_id->text());
        QJsonValue title(ui->lineEdit_title->text());
        QJsonValue description(ui->lineEdit_description->text());
        QJsonValue text(ui->plainTextEdit_text->document()->toPlainText());
        QJsonValue images(ui->lineEdit_images->text());
        card.insert(c_id, id);
        card.insert(c_title, title);
        card.insert(c_description, description);
        card.insert(c_text, text);
        card.insert(c_images, images);
        qDebug() << "saveCard_clicked: inserting new card";
        jdata.insert_card(card,ui->lineEdit_id->text());

        add_card_to_list(new ClCard(ui->lineEdit_title->text(),this) );

        reset_edited();
    }
}

void MainWindow::on_actionClose_file_triggered()
{
    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = yesNoDialogue("Do you want to close the file and loose all changes?");
    }
    if (confirm) {
        remove(ui->card_list->layout());
        empty_fields();
        reset_edited();
        jdata.close_file();
    }
    qDebug() << "close_file: Closed file";
}

void MainWindow::empty_fields() {
    ui->lineEdit_title->clear();
    ui->lineEdit_description->clear();
    ui->lineEdit_id->clear();
    ui->lineEdit_images->clear();
    ui->plainTextEdit_text->clear();
    qDebug() << "empty_fields: done";
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    bool answer{true};
    if (jdata.isEdited()) {
        answer = yesNoDialogue("You have unsaved changes!\nAre you sure you want to quit?");
    }
    if (!answer) {
        event->ignore();
    } else {
        reset_edited();
        event->accept();
    }
}

void MainWindow::on_pushButton_newCard_clicked()
{
    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = yesNoDialogue("Current card has been edited! Do you want to add a new card and loose all changes?");
    }
    if (confirm) {
        empty_fields();
        ui->lineEdit_id->setText( QString::number(jdata.get_next_id()) );
        reset_edited();
    }
}
