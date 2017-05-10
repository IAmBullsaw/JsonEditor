#include "jdata.h"
#include "constants.h"
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <vector>

JData::JData()
    :edited{false},id_edited{false}, jdoc{}, cur_file{}
{}

bool JData::set_file(QString const& filename) {
    bool success{true};
    QString temp{get_current_file()};
    cur_file.setFileName(filename);
    if (!cur_file.exists()) {
        cur_file.setFileName(temp);
        success = false;
    }
    return success;
}

QString JData::get_current_file() const {
    return cur_file.fileName();
}

void JData::open_file(QString const& filename) {
    if (set_file(filename)) {
        open_file();
        load_file();
        close_file();
    }
}

QStringList JData::get_card_titles() const
{
    QStringList titles;
    foreach (QJsonValue const& v, jarray) {
        QString name = v.toObject().value(c_title).toString();
        titles.append(name);
    }
    return titles;
}

std::vector<QJsonObject> JData::get_cards() const {
    std::vector<QJsonObject> cards;

    foreach (QJsonValue const& v, jarray) {
        cards.push_back(v.toObject());
    }
    return cards;
}

QJsonObject JData::get_card(QString const& title) const {
    qDebug() << title;
    QJsonObject sdf;
    return sdf;
}

void JData::open_file() {
    /* Make sure things are set up correctly before opening a file */
    check_file_status();
    qDebug() << "open_file: Current file open '" + get_current_file() + "'";

    cur_file.setFileName(get_current_file());
    if (!cur_file.open(QIODevice::ReadWrite)) {
        qDebug() << "open_file: couldn't open file '" + get_current_file() + "'";
    }
}

void JData::load_file()
{
    qDebug() << "load_file: loading '" + get_current_file() +"'";
    QString text{cur_file.readAll()};
    jdoc = QJsonDocument::fromJson(text.toUtf8());
    if (!jdoc.isNull()) {
        qDebug() << "load_file: loaded file '" + get_current_file() +"'";
        if (jdoc.isArray()) {
            qDebug() << "load_file: It's an array!";
            jarray = jdoc.array();
            qDebug() << "load_file: array count = " + QString::number(jarray.count());
        } else {
            qDebug() << "load_file: **ERROR** It is not my card data";
        }
    } else {qDebug() << "load_file: **ERROR** doc is null";}
}

bool JData::isId_edited() const
{
    return id_edited;
}

void JData::setId_edited(bool value)
{
    id_edited = value;
}

void JData::insert_card(QJsonObject const& card, QString const& id)
{
    bool found{false};
    qDebug() << "insert_card: finding card to replace";
    for(int i{0}; i < jarray.size(); i++) {
        QJsonValueRef ref = jarray[i];
        if (ref.toObject()[c_id] == id.toInt()) {
            qDebug() << "insert_card: found one";
            found = true;
            ref = QJsonValue(card);
        }
    }
    if (!found) {
        jarray.append(card);
    }
    qDebug() << "insert_card: size = " << jarray.size();
    foreach (QJsonValue v, jarray) {
        qDebug() << v;
    }
}

int JData::get_next_id() const
{
    return jarray.size();
}

bool JData::isEdited() const
{
    return edited;
}

void JData::setEdited(bool value)
{
    edited = value;
}

void JData::close_file() {
    if (cur_file.isOpen())
        cur_file.close();
}

bool JData::save_file()
{
    QString filename = get_current_file() + ".savetest";
    qDebug() << "Saving the file: " << filename;

    QFile savefile(filename);
    if (!savefile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonDocument ndock(this->jarray);
    savefile.write(ndock.toJson());
    return true;
}

bool JData::check_file_status() const {
    bool status{true};

    if (get_current_file() == "") {
        qDebug() << "check_file_status: filename not set";
        status = false;
    }
    if (cur_file.isOpen()) {
        qDebug() << "check_file_status: file is open";
        status = false;
    }
    if (edited) {
        qDebug() << "check_file_status: file not saved";
        status = false;
    }

    return status;
}

