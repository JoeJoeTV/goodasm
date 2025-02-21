//Interface for GoodASM from Javascript
#include "gaobject.h"


GAObject::GAObject() {
    goodasm.listadr=true;

}

QString GAObject::updateSlot(const QString &msg) {
    //qDebug() << "Called the C++ slot with message:" << msg;
    goodasm.load(msg);

    return goodasm.source();
}
