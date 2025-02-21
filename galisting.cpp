#include <QDebug>
#include <qsystemdetection.h>
#include "galisting.h"
#include "goodasm.h"

GAListing::GAListing() {
    clearStops();
}

//Called between runs, to clear column measurements.
void GAListing::clearStops(){
    /* For now, we don't clear the tab stops.  They grow
     * when wider spaces are needed, but they never shrink.
     */

    //Good for android and iOS
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    label_len=5;
    code_len=10;
    comment_len=20;
    return;
#endif

    //Good for unix.
    label_len=8;
    code_len=32;
    comment_len=51;

}

QString GAListing::bitstring(uint8_t b){
    QString s="";
    for(int i=0; i<8; i++){
        s+=(b&(0x80>>i))?"1":"0";
        if(i==3)
            s+=".";
    }
    return s;
}

//Returns the working name of the current module without suffixes.
QString GAListing::moduleName(GoodASM *goodasm){
    QString fn=goodasm->filename;
    QString name="";
    for(int i=0; i<fn.length(); i++){
        QChar c=fn[i];
        if(c.isLetterOrNumber())
            name+=c;
        else
            return name;
    }
    if(name.length()>0)
        return name;

    return "nameless";
}


//Formats a source line for printing.
QString GAListing::formatSource(QString label, QString code,
                                QString comment, QString comment2){
    /* Every source line beings with a potential label, then some code
     * and finally a comment.  This fuction is largely just concatenating
     * with padding, but we cannot use QString::arg() because of its
     * trouble with format string injection when presented with I/O addresses.
     */

    //Simple example with just a comment.
    if(label.length()==0 && code.length()==0 && comment2.length()==0)
        return comment+"\n";

    QString s=label;
    if(s.length()>label_len-2) label_len=s.length()+2;
    while(s.length()<label_len) s+=" ";
    s+=code;
    if(comment!=""){
        if(s.length()>code_len-2) code_len=s.length()+2;
        while(s.length()<code_len) s+=" ";
        s+=comment;
        if(comment2!=""){
            if(s.length()>comment_len-2) comment_len=s.length()+2;
            while(s.length()<comment_len) s+=" ";
            s+=comment2;
        }
    }
    s+="\n";

    return s;
}
