#include "gareplxx.h"
#include "goodasm.h"


#include <fstream>
#include <iostream>

#include <QDebug>

int gareplxx_encode(GoodASM *goodasm){
    GAReplXX rxx{goodasm, ".goodasm_history" };
    rxx.start();

    return 0;
}

static bool isbytes(QString s){
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



GAReplXX::GAReplXX(GoodASM *goodasm, const std::string& historyFile)
    : historyFile(historyFile) {
    this->goodasm=goodasm;

    // Set non-default replxx preferences
    using namespace std::placeholders;


    set_completion_callback(std::bind(&GAReplXX::CompleteContext, this, _1, _2));
    /*
    set_completion_count_cutoff(128);
    set_hint_callback(std::bind(&GAReplXX::Hint, this, _1, _2, _3));
    set_indent_multiline(false);
    set_max_history_size(1000);
    set_prompt(this->prompt);
    set_word_break_characters(Ifs);
    */

    // Add handler for window size changes
    install_window_change_handler();
    // load the history file if it exists
    if (!this->historyFile.empty()) {
        std::ifstream fileStream{ historyFile.c_str() };
        history_load(fileStream);
    }
    history_add(""); // Added to fix issue #137
}

void GAReplXX::start() {
    //Lab mode by default.
    goodasm->listadr=true;
    if(!goodasm->listbits)
        goodasm->listbytes=true;


    int i=0;
    do {
        this->prompt=goodasm->lang->name.toStdString()+"> ";

        // Prompt the user and get their input
        const char* rawInput{ nullptr };
        do {
            rawInput = input(this->prompt);
        } while ((rawInput == nullptr) && (errno == EAGAIN));
        if (rawInput == nullptr) {
            break;
        }
        std::string input{ rawInput };
        if (input.empty()) {
            // Handle a user hitting enter after an empty line
            continue;
        }

        history_add(input);
        QString line=QString(input.c_str());

        if(!isbytes(line)){
            goodasm->line=i;
            goodasm->load(line);
        }else{
            QByteArray bytes=QByteArray::fromHex(line.toLocal8Bit());
            goodasm->load(bytes);
        }
        std::cout<<goodasm->source().toStdString();

        goodasm->printErrors();
        goodasm->clear();
    } while (true);


    if (!this->historyFile.empty()) {
        history_sync(this->historyFile);
    }
}


replxx::Replxx::completions_t GAReplXX::CompleteContext(const std::string& line, int& lastWordLength) {
    Replxx::completions_t completions;
    for (auto& match : this->Matches(line)) {
        completions.emplace_back(match.data(), Replxx::Color::DEFAULT);
    }
    return completions;
}

std::vector<std::string> GAReplXX::Matches(const std::string& line) {
    std::vector<std::string> matches;
    /*
    std::string firstWord{ line.substr(0, WordBoundary(line.data())) };
    if (firstWord.length() == line.length()) {
        // Gather completions for a command
        for (auto const& command : this->commands) {
            auto& name{ command.first };
            if (Equal(name, firstWord, firstWord.length())) {
                matches.emplace_back(name.data());
            }
        }
    }*/

    foreach (auto c, goodasm->completions(QString(line.c_str()))) {
        matches.emplace_back(c.toStdString());
    }

    return matches;
}

