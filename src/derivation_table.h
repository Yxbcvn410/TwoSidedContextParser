//
// Created by alexander on 28.11.2020.
//

#ifndef _DERIVATION_TABLE_H
#define _DERIVATION_TABLE_H

#include <memory>
#include "grammar.h"

typedef std::pair<int, int> substr_marker;
typedef std::pair<int, substr_marker> var_tree_ref;

struct parse_tree_node {
    std::set<var_tree_ref> contexts;
    std::set<std::vector<var_tree_ref>> productions;

    parse_tree_node() = default;
};


class derivation_table {
    // TODO
public:
    // Инициализируем пустую таблицу по строке
    derivation_table(const std::vector<int> &string);

    // Размер кроны
    int size() const;

    // Задаём алфавит
    void set_alphabet(alphabet &labels);

    // Возвращаем ссылку на список возможных выводов для маркера подстроки
    std::map<int, parse_tree_node> &operator[](substr_marker marker);

    // Выводим дерево в формате DOT в произвольный поток. Используем алфавит, заданный через метод выше
    friend std::ostream &operator<<(std::ostream &out, derivation_table table);
};

#endif  // _DERIVATION_TABLE_H
