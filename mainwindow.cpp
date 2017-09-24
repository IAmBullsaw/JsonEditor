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
    connect(ui->lineEdit_headerhref , SIGNAL(textEdited(QString)),
            this, SLOT(onEditCard(QString const&)));
    connect(ui->lineEdit_headeralt , SIGNAL(textEdited(QString)),
            this, SLOT(onEditCard(QString const&)));
    connect(ui->lineEdit_title, SIGNAL(textEdited(QString)),
            this, SLOT(onEditCard(QString const&)));
    connect(ui->plainTextEdit_content, SIGNAL(textChanged()),
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
        int id = o.value(c_id).toInt();
        qDebug() << s;

        //Adds one line before card
        QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        ui->card_list->addWidget(line);

        // create and add card
        ClCard* c = new ClCard(s,id,this);
        ui->card_list->addWidget(c);

        c->setMinimumWidth(c_card_minimum_width);

        connect(c, SIGNAL(focus_changed(QString,int)),
                this, SLOT(focus_changed(QString,int)));

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
    connect(c, SIGNAL(focus_changed(QString,int)),
            this, SLOT(focus_changed(QString,int)));
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

void MainWindow::set_id(int id)
{
    ui->lineEdit_id->setText( QString::number(id));
}

void MainWindow::set_title(QString title)
{
    ui->lineEdit_title->setText(title);
}

void MainWindow::set_description(QString description)
{
    ui->lineEdit_description->setText(description);
}

void MainWindow::set_headerhref(QString href)
{
    ui->lineEdit_headerhref->setText(href);
}

void MainWindow::set_headeralt(QString alt)
{
    ui->lineEdit_headeralt->setText(alt);
}

bool MainWindow::is_link(QString const& str) {
    // Link regex
    // ^\({1}([^){1}\n]+)\){1}\[{1}([^]{1}\n]+)\]{1}$
    // new one
    // ^\(([^)]+)\)\[([^]\n]+)\]$
    QRegularExpression re("^\\(([^)]+)\\)\\[([^]\\n]+)\\]$");
    QRegularExpressionMatch match = re.match(str);
    return match.hasMatch();
}

bool MainWindow::is_image(QString const& str) {
    // Image regex
    // ^\({1}([^){1}]+)\){1}\[{1}([^]{1}]+)\]{1}{{1}([^}\n]*)}{1}$
    QRegularExpression re("^\\({1}([^){1}]+)\\){1}\\[{1}([^]{1}]+)\\]{1}{{1}([^}\\n]*)}{1}$");
    QRegularExpressionMatch match = re.match(str);
    return match.hasMatch();
}

QJsonObject MainWindow::build_link(QString const& str) {
    QRegularExpression re("^\\(([^)]+)\\)\\[([^]\\n]+)\\]$");
    QRegularExpressionMatch match = re.match(str);
    QString alt = match.captured(1);
    QString href = match.captured(2);
    QJsonObject link;
    link.insert(c_content_alt, QJsonValue(alt));
    link.insert(c_content_href, QJsonValue(href));
    return link;
}

QJsonObject MainWindow::build_image(QString const& str) {
    QRegularExpression re("^\\({1}([^){1}]+)\\){1}\\[{1}([^]{1}]+)\\]{1}{{1}([^}\\n]*)}{1}$");
    QRegularExpressionMatch match = re.match(str);
    QString alt = match.captured(1);
    QString href = match.captured(2);
    QString image_text = match.captured(3);
    QJsonObject img;
    img.insert(c_content_alt, QJsonValue(alt));
    img.insert(c_content_href, QJsonValue(href));
    img.insert(c_content_image_text, QJsonValue(image_text));
    return img;
}
QJsonArray MainWindow::getContent(QStringList const& lis) {
    QJsonArray content;
    for (QString s: lis) {
        if (is_link(s)){
            QJsonObject link = build_link(s);
            content.append(QJsonValue(c_content_li));
            content.append(QJsonValue(link));

        } else if (is_image(s)) {
            QJsonObject img = build_image(s);
            content.append(QJsonValue(c_content_img));
            content.append(QJsonValue(img));
        } else if (!s.isEmpty()){
            content.append(QJsonValue(c_content_p));
            content.append(QJsonValue(s));
        }
    }
    qDebug() << content;
    return content;
}


void MainWindow::set_content(QJsonArray content)
{
    qDebug() << "focus_changed: arr = " << content;
    QString s{""};

    QJsonArray::iterator it = content.begin();
    for (int count = content.size()/2; count > 0; --count)
    {
        QString v = (*it).toString();
        qDebug() << "V: " << v << " count: " << count;
        ++it;
        if (v == c_content_p)
        {
            s += (*it).toString();
            s += "\n\n";
        }
        else if (v == c_content_li)
        {
            QJsonObject o = (*it).toObject();
            s += "(";
            s += o.value(c_content_alt).toString();
            s += ")[";
            s += o.value(c_content_href).toString();
            s += "]";
            s += "\n\n";
        }
        else if (v == c_content_img)
        {
            QJsonObject o = (*it).toObject();
            s += "(";
            s += o.value(c_content_alt).toString();
            s += ")[";
            s += o.value(c_content_href).toString();
            s += "]{";
            s += o.value(c_content_image_text).toString();
            s += "}";
            s += "\n\n";
        }
        else
        {
            qDebug() << "set_content: don't know how to process '" << v << "'";
        }
        ++it;
    }

    first_edit = true; // don't want to trigger editText slot too early...
    ui->plainTextEdit_content->document()->setPlainText( s );
}

void MainWindow::focus_changed(QString label, int id)
{
    qDebug() << "focus_changed: listening";
    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = yesNoDialogue("Do you want to close the file and loose all changes?");
    }
    if (confirm) {
        QJsonObject new_card;
        foreach (QJsonObject o, jdata.get_cards()) {
            int i = o.value(c_id).toInt();
            if (i == id) {
                new_card = o;
                break;
            }
        }
        qDebug() << new_card;
        set_id(new_card.value(c_id).toInt());
        set_title(new_card.value(c_title).toString());
        set_description(new_card.value(c_description).toString());
        set_headerhref(new_card.value(c_headerhref).toString());
        set_headeralt(new_card.value(c_headeralt).toString());
        set_content(new_card.value(c_content).toArray());
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
        QJsonValue id(ui->lineEdit_id->text().toInt());
        qDebug() << "id: " << id;
        QJsonValue title(ui->lineEdit_title->text());
        QJsonValue description(ui->lineEdit_description->text());
        QJsonValue header_href(ui->lineEdit_headerhref->text());
        QJsonValue header_alt(ui->lineEdit_headeralt ->text());
        QStringList lis = (ui->plainTextEdit_content->document()->toPlainText()).split("\n\n");
        QJsonArray content = getContent(lis);

        card.insert(c_id, id);
        card.insert(c_title, title);
        card.insert(c_description, description);
        card.insert(c_headerhref , header_href);
        card.insert(c_headeralt, header_alt);
        card.insert(c_content, content);
        qDebug() << "saveCard_clicked: inserting new card";
        qDebug() << card;
        jdata.insert_card(card,ui->lineEdit_id->text());
        populate_window();
        reset_edited();
        jdata.setSaved(false);
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
    ui->lineEdit_id->clear();
    ui->lineEdit_title->clear();
    ui->lineEdit_description->clear();
    ui->lineEdit_headerhref->clear();
    ui->lineEdit_headeralt->clear();
    ui->plainTextEdit_content->clear();
    qDebug() << "empty_fields: done";
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    bool answer{true};
    if (jdata.isEdited()) {
        answer = yesNoDialogue("You have unsaved changes!\nAre you sure you want to quit?");
    } else if (!jdata.isSaved()){
        answer = yesNoDialogue("You have added a card but not saved!\nAre you sure you want to quit?");
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

void MainWindow::on_actionSave_file_triggered()
{
    qDebug() << "Saving file...";
    bool confirm{true};
    if (jdata.isEdited()) {
        confirm = yesNoDialogue("Current card has been edited! Do you want to add a new card and loose all changes?");
    }
    if (confirm) {
        if (jdata.get_current_file().isNull()) {
            qDebug() << "filename is null";
            QString filename = QFileDialog::getSaveFileName(this,
                                                            "Choose a filename!",
                                                            "",
                                                            tr("Json Files (*.json *.JSON);;Text files (*.txt)")
                                                            );
            if (filename != "") {
                jdata.set_file(filename);
            }
        }
        jdata.save_file();
        reset_edited();
    }
}
