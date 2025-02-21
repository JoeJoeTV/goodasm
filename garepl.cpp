#include "garepl.h"

#include "goodasm.h"
#include <iostream>
#include <QDebug>




#ifdef HASREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

static GoodASM* replgoodasm=0;

/* Attempt to complete on the contents of TEXT.  START and END show the
   region of TEXT that contains the word to complete.  We can use the
   entire line in case we want to do some simple parsing.  Return the
   array of matches, or NULL if there aren't any. */
char **garepl_completion (const char *text, int start, int end){
#ifdef HASREADLINE
    //Completion table looks like this, with all elements being freed after rendering.
    assert(replgoodasm);
    char **matches=replgoodasm->readline_completions(text, rl_line_buffer,
                                                       start,end);
    rl_attempted_completion_over = 1;
    return matches;
#else
    qDebug()<<"No completions on Windows.";
    return 0;
#endif
}

//Here we make fake readline functions for Windows.
#ifndef HASREADLINE
/*
char **(*rl_attempted_completion_function) (const char *text, int start, int end);
const char *rl_readline_name;
char *readline(char *prompt){
    return 0;
}
void add_history(const char* line){

}*/

//Encoding REPL, for trying out assembly.
int garepl_encode(GoodASM *goodasm){
    QTextStream s(stdin);
    QString l;
    int i=0;
    while(std::cout<<"goodasm> ", s.readLineInto(&l)){
        goodasm->load(l);
        std::cout<<"["<<i<<"] "<<goodasm->hexdump().toStdString();
        goodasm->printErrors();
        goodasm->clear();
    }
    return 0;
}

//Decoding REPL, for trying out disassembly.
int garepl_decode(GoodASM *goodasm){
    QTextStream s(stdin);
    QString l;
    int i=0;
    while(std::cout<<"goodasm> ", s.readLineInto(&l)){
        QByteArray bytes=QByteArray::fromHex(l.toLocal8Bit());
        goodasm->load(bytes);
        std::cout<<goodasm->source().toStdString();
        goodasm->printErrors();
        goodasm->clear();
    }
    return 0;
}

#else

static char name[]="goodasm";

bool isbytes(QString s){
    int count=0;
    s+=" ";

    for(int i=0; i<s.length(); i++){
        QChar c=s[i].toLower();
        if(
            !c.isDigit() &&
            !c.isSpace() &&
            !(c.toLatin1()>='a' && c.toLatin1()<='f')
            ){
            return false;
        }else if(c.isSpace()){
            if((count&1)==1)
                return false;
            count=0;
        }else{
            //Count the next letter.
            count++;
        }
    }

    //All characters are spaces, numbers, or A through F.
    //And nybbles are in pairs.
    return true;
}

//Encoding REPL, for trying out assembly.
int garepl_encode(GoodASM *goodasm){
    char prompt[] = "goodasm> ";
    char* lineinput;

    rl_readline_name = name;
    replgoodasm=goodasm; //For completion.

    //Lab mode by default.
    goodasm->listadr=true;
    if(!goodasm->listbits)
        goodasm->listbytes=true;

    /* Tell the completer that we want a crack first. */
    rl_attempted_completion_function = garepl_completion;

    int i = 0;
    while(1){
        lineinput = readline(prompt);
        if(!lineinput) exit(0);
        add_history(lineinput);

        QString line=QString(lineinput);

        if(!isbytes(line)){
            goodasm->line=i;
            goodasm->load(line);
        }else{
            QByteArray bytes=QByteArray::fromHex(QString(lineinput).toLocal8Bit());
            goodasm->load(bytes);
        }
        std::cout<<goodasm->source().toStdString();


        free(lineinput);
        goodasm->printErrors();
        goodasm->clear();
        i++;
    }
    return 0;
}

#endif
