//
// Created by alexander on 18.12.2020.
//

#include <sstream>
#include "grammar.h"

grammar::grammar() {
    start = 0;
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
    for (const auto& r : rules) {
        for (const auto& c : r.contexts) {
            if (c.second == NONE) {
                if (c.first.empty() or c.first.size() > 2)
                    return false;
            } else {
                if (c.first.size() != 1)
                    return false;
            }
        }
    }
    return true;
}

enum parse_mode {
    AWAIT,
    WORD,
    QUOTED,
    SPECIAL
};

struct word {
    std::string text;
    int ln = 0, col = 0;
    parse_mode word_type = WORD;

    bool operator==(const char *_text) const {
        return this->text == _text;
    }

    bool operator!=(const char *_text) const {
        return this->text != _text;
    }
};


std::vector<word> get_words(std::istream &in) {
    std::vector<word> words;
    int ln = 1, col = 1;
    std::vector<std::set<char>> special_char_groups = {{';'},
                                                       {',', ':', '=', '<', '>', '|', '(', ')', '~', '&'}};
    int sp_char_group = -1;
    std::set<char> delimiters = {' ', '\n', '\r', -1};
    char next;
    parse_mode mode = AWAIT;
    std::string current;
    while (not in.eof()) {
        next = in.get();
        parse_mode snap;
        bool word_end = false;
        if (mode == QUOTED) {
            if (next == '"' or next == '\n') {
                word_end = true;
                snap = mode;
                if (next == '"')
                    current += next;
                mode = AWAIT;
            }
        } else if (next == '"') {
            if (mode == AWAIT) {
                col += current.size();
                current = "";
            }
            else {
                word_end = true;
                snap = mode;
            }
            mode = QUOTED;
        } else if (mode == AWAIT) {
            sp_char_group = -1;
            for (int i = 0; i < special_char_groups.size(); i++)
                if (special_char_groups[i].count(next))
                    sp_char_group = i;
            if (sp_char_group != -1) {
                col += current.size();
                current = "";
                mode = SPECIAL;
            } else if (delimiters.count(next)) {
                ln++;
            } else {
                col += current.size();
                current = "";
                mode = WORD;
            }
        } else if (delimiters.count(next)) {
            word_end = true;
            snap = mode;
            mode = AWAIT;
        } else if (mode == WORD) {
            sp_char_group = -1;
            for (int i = 0; i < special_char_groups.size(); i++)
                if (special_char_groups[i].count(next))
                    sp_char_group = i;
            if (delimiters.count(next) or sp_char_group != -1) {
                word_end = true;
                snap = mode;
                mode = delimiters.count(next) ? AWAIT : SPECIAL;
            }
        } else if (mode == SPECIAL) {
            int new_sp_char_group = -1;
            for (int i = 0; i < special_char_groups.size(); i++)
                if (special_char_groups[i].count(next))
                    new_sp_char_group = i;
            if (sp_char_group != new_sp_char_group) {
                word_end = true;
                snap = mode;
                sp_char_group = new_sp_char_group;
                mode = delimiters.count(next) ? AWAIT : sp_char_group == -1 ? WORD : SPECIAL;
            }
        }

        if (word_end) {
            word _word;
            _word.text = current;
            _word.ln = ln;
            _word.col = col;
            _word.word_type = snap;
            col += current.size();
            current = "";
            words.push_back(_word);
        }
        if (next == '\n') {
            ln++;
            col = 1;
        }
        current += next;
    }
    if (mode != AWAIT) {
        word _word;
        _word.text = current;
        _word.ln = ln;
        _word.col = col;
        _word.word_type = mode;
        current = "";
        words.push_back(_word);
    }
    return words;
}

std::logic_error parse_error(const std::string &err, const word &_word) {
    std::ostringstream oss;
    oss << "grammar parsing failed\n" << err << "\nat ln " << _word.ln << ", col " << _word.col << " of <grammar>";
    return std::logic_error(oss.str().c_str());
}

std::logic_error unexpected_symbol(const word &_word) {
    std::ostringstream oss;
    oss << "Unexpected symbol '" << _word.text << "'";
    return parse_error(oss.str(), _word);
}

enum rule_parse_mode {
    BEGIN,
    AXIOM_DEF,
    TERMINAL_DEF,
    RULE_DEF,
    AWAIT_DEF
};

std::istream &operator>>(std::istream &stream, grammar &_grammar) {
    auto words = get_words(stream);
    rule_parse_mode mode = BEGIN;
    bool lhs = true, parenthesis = false;
    context_type context = NONE;
    std::vector<int> letters;
    rule building_rule = {};
    std::string terminal_name;
    for (const auto &_word : words) {
        if (mode == BEGIN) {
            if (_word != "grammar")
                throw parse_error("Axiom definition not found", _word);
            mode = AXIOM_DEF;
        } else if (mode == AXIOM_DEF) {
            if (_word == "(") {
                if (parenthesis)
                    throw unexpected_symbol(_word);
                parenthesis = true;
            } else if (_word == ")") {
                if (not parenthesis)
                    throw unexpected_symbol(_word);
                parenthesis = false;
            } else if (parenthesis) {
                if (_grammar.start != 0)
                    throw parse_error("Invalid axiom definition", _word);
                else if (_word.word_type == WORD)
                    _grammar.start = _grammar.add_non_terminal(_word.text);
                else
                    throw unexpected_symbol(_word);
            } else if (_word == ";") {
                if (_grammar.start == 0)
                    throw parse_error("Empty axiom definition", _word);
                mode = AWAIT_DEF;
            }
        } else if (mode == TERMINAL_DEF) {
            throw parse_error("Terminal definitions not supported", _word);
        } else if (mode == RULE_DEF) {
            if (lhs) {
                if (_word == "=")
                    lhs = false;
                else if (building_rule.origin == 0)
                    building_rule.origin = _grammar.add_non_terminal(_word.text);
                else
                    throw unexpected_symbol(_word);
            } else {
                if (letters.empty()) {
                    bool is_ctx = true;
                    if (_word == "<" or _word == ">=~")
                        context = LEFT;
                    else if (_word == "<=" or _word == ">~")
                        context = LEFT_EXT;
                    else if (_word == ">")
                        context = RIGHT;
                    else if (_word == ">=")
                        context = RIGHT_EXT;
                    else
                        is_ctx = false;
                    if (is_ctx)
                        continue;
                }
                if (_word.word_type == WORD)
                    letters.push_back(_grammar.add_non_terminal(_word.text));
                else if (_word.word_type == QUOTED) {
                    if (_word.text.empty() or _word.text[0] != '"' or _word.text.back() != '"')
                        throw parse_error("Closing quote missing", _word);
                    for (int i = 1; i < _word.text.size() - 1; i++)
                        letters.push_back(_grammar.add_terminal(_word.text[i]));
                } else if (_word == "&" or _word == ";") {
                    building_rule.contexts.insert({letters, context});
                    context = NONE;
                    letters = {};
                    if (_word == ";") {
                        _grammar.rules.push_back(building_rule);
                        mode = AWAIT_DEF;
                    }
                } else
                    throw unexpected_symbol(_word);
            }
        } else if (mode == AWAIT_DEF) {
            lhs = true;
            if (_word == "terminal") {
                mode = TERMINAL_DEF;
            } else if (_word.word_type == WORD) {
                mode = RULE_DEF;
                building_rule = {};
                building_rule.origin = _grammar.add_non_terminal(_word.text);
            } else
                throw unexpected_symbol(_word);
        }
    }
    if (mode != AWAIT_DEF)
        throw parse_error("Unexpected EOF", words.back());
    return stream;
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