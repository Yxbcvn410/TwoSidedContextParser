//
// Created by alexander on 18.12.2020.
//

#include <sstream>
#include "grammar.h"

grammar::grammar() {
    start = 1;
}

std::vector<rule> grammar::all_rules() const {
    return rules;
}

std::vector<rule> grammar::no_context_rules() const {
    std::vector<rule> no_cxt;
    for (auto &rule : rules)
        if (rule.contexts.empty())
            no_cxt.push_back(rule);
    return no_cxt;
}

std::vector<int> grammar::convert_text(const std::string &text) const {
    std::vector<int> string;
    for (const char &c : text) {
        if (not terminals.count(c))
            throw std::exception(); // Undefined terminal
        string.push_back(terminals.at(c));
    }
    return string;
}

int grammar::get_start_symbol() const {
    return start;
}

alphabet grammar::get_alphabet() const {
    alphabet _alphabet;
    for (auto &pair : non_terminals)
        _alphabet[pair.second] = pair.first;
    for (auto &pair : terminals)
        _alphabet[pair.second] = pair.first;
    return _alphabet;
}

bool grammar::is_binary_normal_form() const {
    std::set<int> terms;
    // TODO
}

void grammar::build_interactive() {
    // TODO Temporary solution, get rid of it in major releases
    using namespace std;
    cout << "Start symbol?" << endl;
    std::string word;
    getline(cin, word);
    add_non_terminal(word);
    cout << "Note: every single-character word is treated as terminal" << endl;
    while (true) {
        cout
                << R"(Define a rule, one operator per line, print "end" to complete rule, "done" to finish. Origin symbol?)"
                << endl;
        rule _rule{};
        getline(cin, word);
        if (word == "done")
            return;
        _rule.origin = add_non_terminal(word);
        std::string line;
        while (getline(cin, line)) {
            if (line == "done")
                return;
            else if (line == "end") {
                rules.push_back(_rule);
                break;
            } else {
                istringstream iss(line);
                iss >> word;
                bool ctxt = true;
                context_type tp;
                if (word == "<")
                    tp = LEFT;
                else if (word == ">")
                    tp = RIGHT;
                else if (word == "<=")
                    tp = LEFT_EXT;
                else if (word == ">=")
                    tp = RIGHT_EXT;
                else ctxt = false;
                if (ctxt)
                    iss >> word;
                std::vector<int> l;
                if (word.size() == 1)
                    l.push_back(add_terminal(word[0]));
                else
                    l.push_back(add_non_terminal(word));
                while (iss >> word) {
                    if (word.size() == 1)
                        l.push_back(add_terminal(word[0]));
                    else
                        l.push_back(add_non_terminal(word));
                }
                if (not l.empty()) {
                    if (ctxt)
                        _rule.contexts.insert({l, tp});
                    else
                        _rule.productions.insert(l);
                }
            }
        }
    }
}

std::istream &operator>>(std::istream &in, grammar &_grammar) {
    // TODO
    return in;
}

int grammar::add_non_terminal(const std::string &nt) {
    int index = terminals.size() + non_terminals.size() + 1;
    if (not non_terminals.count(nt))
        non_terminals[nt] = index;
    else
        return non_terminals[nt];
    return index;
}

int grammar::add_terminal(char t) {
    int index = terminals.size() + non_terminals.size() + 1;
    if (not terminals.count(t))
        terminals[t] = index;
    else
        return terminals[t];
    return index;
}
