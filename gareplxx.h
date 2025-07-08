#ifndef GAREPLXX_H
#define GAREPLXX_H

/* Previously, we used GNU Readline on unix and a simple scanf()
 * loop on Windows.  This was less than optimal, as all development
 * is done on unix and the Windows REPL frequently fell out of parity.
 *
 * The new method is the replxx library, which provides readline-like
 * features in a portable way.
 *
 * https://github.com/AmokHuginnsson/replxx
 */

class GoodASM;
#include <replxx.hxx>

int gareplxx_encode(GoodASM *goodasm);


// FIXME: Replace this separator with one from the real parser.
char const Ifs[]{ " \t\n\r\v\f=+*&^%$#@,/?<>;:`~'\"[]{}()|" };

class GAReplXX : public replxx::Replxx {
public:
    using Command = std::function<void(GAReplXX& rxx, const std::string& arg)>;

    GAReplXX(GoodASM *goodasm, const std::string& historyFile = "");
    void start();

private:
    GoodASM *goodasm=0;
    using CommandWithResult = std::function<bool(GAReplXX& cli, const std::string& arg)>;

    std::string historyFile;
    std::string prompt{ "goodasm> " };
    std::vector<std::pair<std::string, CommandWithResult>> commands;

    Replxx::completions_t CompleteContext(const std::string& line, int& lastWordLength);
    std::vector<std::string> Matches(const std::string& line);
};


#endif
