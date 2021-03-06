#include "clcard.h"
#include <QDebug>

ClCard::ClCard(QString l, int id, QWidget *parent)
    :id{id}, QWidget(parent) {
    label = new QLabel(l,this);
    this->setContentsMargins(0,0,0,0);
}

void ClCard::mousePressEvent(QMouseEvent *event) {
    event->accept();
    qDebug() << "clcard: making = " +this->label->text();
    emit focus_changed(this->label->text(),this->id);
}

void ClCard::paintEvent(QPaintEvent *e) {
    QPainter painter(this);

   // painter.drawRoundedRect(0,5,width(), height(),3,3);

    QWidget::paintEvent(e);
}
