#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "jdata.h"
#include "clcard.h"

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

    void on_pushButton_saveCard_clicked();

    void on_actionClose_file_triggered();

    void on_pushButton_newCard_clicked();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void remove(QLayout* layout);
    bool yesNoDialogue(QString const& question);
    void empty_fields();
    void closeEvent(QCloseEvent *event);
public slots:
    void onEditCard(const QString &string);
    void onEditCard();
    void onEditId(const QString &string);
    void onEditCheckbox(const int &in);
private:
    void reset_edited();
    void populate_window();
    Ui::MainWindow *ui;
    JData jdata;
    std::vector<QJsonObject> cards;
    unsigned int prev_edit_text_s;
    bool first_edit;
    void add_card_to_list(ClCard * const &c);
};

#endif // MAINWINDOW_H
