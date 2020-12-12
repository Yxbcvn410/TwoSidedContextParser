//
// Created by Alexander on 12.12.2020.
//

#include "derivation_table.h"

derivation_table::derivation_table(const std::vector<int> &string) {
    this->_size = string.size();
    for (int i = 0; i < string.size(); ++i)
        for (int j = i + 2; j <= string.size(); ++j)
            this->substr_derivations[{i, j}] = {};
    for (int i = 0; i < string.size(); ++i)
        this->substr_derivations[{i, i + 1}][i] = {};
}

int derivation_table::size() const {
    return _size;
}

void derivation_table::set_alphabet(const alphabet &labels) {
    this->_alphabet = labels;
}

std::map<int, parse_tree_node> &derivation_table::operator[](substr_marker marker) {
    return this->substr_derivations[marker];
}

std::ostream &operator<<(std::ostream &out, const derivation_table &table) {
    // TODO(lullo-alsu): Здесь код, генерирующий DOT
    return out;
}


