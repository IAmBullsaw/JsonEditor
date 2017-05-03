#ifndef CLCARD_H
#define CLCARD_H

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>

class ClCard : public QWidget
{
    Q_OBJECT
public:
    explicit ClCard(QString l, QWidget *parent = 0);
    void paintEvent(QPaintEvent *e) override;

signals:
    void focus_changed(QString label);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel* label;
    QObject* jsonobject;
};

#endif // CLCARD_H
