#include <QDebug>
#include <QCryptographicHash>

#include "galanguage.h"
#include "gaparser.h"
#include "gasymboltable.h"


GASymbol::GASymbol(QString n){
    this->name=n;
    this->defined=false;
}

GASymbolTable::GASymbolTable(){
}

//Sorting function for vector pointers.
static bool lesserSymbol(GASymbol * left, GASymbol * right){
    return left->absval < right->absval;
}



//Returns sorted table of links to symbols in linear order.
QVector<GASymbol*> GASymbolTable::sorted(){
    QVector<GASymbol*> v;
    foreach(auto s, table)
        v.append(s);
    std::sort(v.begin(), v.end(), lesserSymbol);
    return v;
}

// Clear all symbols from the table.
void GASymbolTable::clear(){
    table.clear();
    tableByAdr.clear();
}

// What symbol names might complete a given prefix?
QStringList GASymbolTable::completions(QString prefix){
    QStringList l;
    foreach(auto s, table.keys())
        if(s.startsWith(prefix))
            l.append(s);
    return l;
}

//Apply a value to a symbol, creating that symbol if needed.
GASymbol* GASymbolTable::setSymbol(QString name, uint64_t absval){
    GASymbol* sym=findSymbol(name, true);
    assert(sym);          //Should never be null because we're creating it.
    sym->absval=absval;
    sym->defined=true;
    return sym;
}
//Same, but we're lazy, so it's an unparsed string as a number.
GASymbol* GASymbolTable::setSymbol(QString name, QString value){
    uint64_t val=0;
    bool okay=false;
    val=GAParser::str2uint(value,&okay);
    if(okay)
        return setSymbol(name,val);

    qDebug()<<"Unable to set symbol"<<name<<"to"<<value;
    return 0;
}

// Find a symbol, optionally creating it if missing.
GASymbol* GASymbolTable::findSymbol(QString name, bool autogen){
    //Don't match on register names as if they were symbols.
    assert(lang);
    if(lang->regnames.contains(name))
        return 0;


    if(!table.contains(name)){
        table[name]=new GASymbol(name);
        table[name]->referenced=autogen;    // Never used it.
        table[name]->defined=false;       // Never yet set.
    }
    return table[name];
}

// Find a symbol by address.
GASymbol* GASymbolTable::findSymbol(uint64_t absval){
    // We don't know the name, so we don't define empty symbols here.
    return tableByAdr[absval];
}


//Export the symbol as a table.
QString GASymbolTable::exportTable(){
    //FIXME: Sort this alphabetically or by address or something.
    QString e="";
    for (auto i = table.begin(), end = table.end(); i != end; i++){
        QString name=i.key();
        uint64_t adr=i.value()->absval;
        e.append(name+" = "+QString::asprintf("0x%04lx",(unsigned long) adr)+";\n");
    }
    return e;
}

/* The symbol hash is a hash of all symbols in the binary,
 * along with their values.  If two runs have the same hash,
 * we expect all future runs to be the same as well.
 */
QByteArray GASymbolTable::symbolhash(){
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.reset();
    for (auto i = table.begin(), end = table.end(); i != end; i++){
        QString name=i.key();
        uint64_t adr=i.value()->absval;
        hash.addData((name+"="+QString::number(adr)).toUtf8());
    }
    return hash.result();
}
//Count the number of table entries.
uint64_t GASymbolTable::count(){
    return table.count();
}

//Have all consumed symbols been defined?
bool GASymbolTable::complete(){
    for (auto i = table.begin(), end = table.end(); i != end; i++){
        GASymbol *s=i.value();   // Take the symbol.
        if(!s->defined)          // Has its value not been set?
            return false;
    }

    //Every symbol has a value.
    return true;
}

//Return a list of undefined symbols.
QStringList GASymbolTable::missingSymbols(){
    QStringList list;
    for (auto i = table.begin(), end = table.end(); i != end; i++){
        GASymbol *val=i.value();   // Take the symbol.
        if(!val->defined)          // Append if undefined.
            list.append(i.key());
    }
    return list;
}
