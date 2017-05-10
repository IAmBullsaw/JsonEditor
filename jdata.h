#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
class JData
{
public:
    JData();
    bool set_file(QString const& filename); /* returns true if it succeeded*/
    QString get_current_file() const;

    void open_file(QString const& filename);

    QJsonObject get_card(QString const& title) const;

    QStringList get_card_titles() const;
    void close_file();
    bool save_file();

    void set_current_card(QString title);
    bool check_file_status() const;

    std::vector<QJsonObject> get_cards() const;
    bool isEdited() const;
    void setEdited(bool value);

    bool isId_edited() const;
    void setId_edited(bool value);

    void insert_card(QJsonObject const& card, QString const& id);
    int get_next_id() const;

    bool isSaved() const;
    void setSaved(bool value);

private:
    void open_file();
    void load_file();

    bool edited; /* true if anything changed since last save */
    bool id_edited; /* true if anything changed the card id */
    bool saved; /* true if all is saved to file */
    QJsonDocument jdoc;
    QJsonArray jarray;
    QJsonObject jobject;
    QFile cur_file;
};

#endif // DATA_H
