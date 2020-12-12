//
// Created by alexander on 28.11.2020.
//

#ifndef _GRAMMAR_H
#define _GRAMMAR_H

#include <vector>
#include <set>
#include <iostream>
#include <map>

enum context_type {
    LEFT,
    RIGHT,
    LEFT_EXT,
    RIGHT_EXT
};

typedef std::vector<int> production;
typedef std::pair<production, context_type> context_operator;

struct rule {
    int origin;
    std::set<context_operator> contexts;
    std::set<production> productions;
};

typedef std::map<int, std::string> alphabet;

class grammar {
    // TODO
public:
    grammar();

    // Возвращает все правила
    std::vector<rule> all_rules() const;

    // Возвращает правила, не имебщие операторов контекста
    std::vector<rule> no_context_rules() const;

    // Переводит строку в список в соответствии со словарём
    std::vector<int> convert_text(const std::string &text) const;

    // Возвращает числовое представление стартового символа
    int get_start_symbol() const;

    // Возвращает сопоставление чисел строкам в описании символов
    alphabet get_alphabet() const;

    // Считывает правила из потока
    friend std::istream &operator>>(std::istream &stream, grammar &_grammar);

    // Проверка на бинарную нормальную форму
    bool is_binary_normal_form() const;
};

#endif  // _GRAMMAR_H
