#ifndef GAOBJECT_H
#define GAOBJECT_H

#include<QObject>
#include<QDebug>

#include "goodasm.h"

class GAObject : public QObject
{
    Q_OBJECT
public:
    GAObject();
public slots:
    QString updateSlot(const QString &msg);

private:
    GoodASM goodasm;
};

#endif // GAOBJECT_H
