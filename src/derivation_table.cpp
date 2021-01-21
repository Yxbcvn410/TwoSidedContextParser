//
// Created by Alexander on 12.12.2020.
//

#include <queue>
#include <sstream>
#include "derivation_table.h"

derivation_table::derivation_table(const std::vector<int> &string) {
    this->_size = string.size();
    this->_start = 0;
    for (int i = 0; i < string.size(); ++i)
        for (int j = i + 2; j <= string.size(); ++j)
            this->substr_derivations[{i, j}] = {};
    for (int i = 0; i < string.size(); ++i)
        this->substr_derivations[{i, i + 1}][string[i]] = {};
}

int derivation_table::size() const {
    return _size;
}

void derivation_table::set_start(int start) {
    _start = start;
}

void derivation_table::set_alphabet(const alphabet &labels) {
    this->_alphabet = labels;
}

std::map<int, parse_tree_node> &derivation_table::operator[](substr_marker marker) {
    return this->substr_derivations[marker];
}

std::string generate_name(const var_tree_ref &ref, int index_helper) {
    std::ostringstream oss;
    oss << "vertex_" << ref.first << "_" << ref.second.first << "_" << ref.second.second << "_" << index_helper;
    return oss.str();
}

std::ostream &operator<<(std::ostream &out, derivation_table &table) {
    out << "digraph ParseTree {\n";
    std::queue<var_tree_ref> roots;
    roots.push({table._start, {0, table.size()}});
    int root_counter = 0;
    std::set<var_tree_ref> defined_roots;
    while (not roots.empty()) {
        std::queue<var_tree_ref> queue;
        queue.push(roots.front());
        out << "\t" << table.generate_def(queue.front(), root_counter);
        std::set<var_tree_ref> defined;
        while (not queue.empty()) {
            auto ref = queue.front();
            // Contexts
            for (const auto &context : table[ref.second][ref.first].contexts) {
                if (not defined_roots.count(context)) {
                    out << "\t\n\t" << table.generate_def(context, root_counter + (int) roots.size());
                    roots.push(context);
                    defined_roots.insert(context);
                }
                out << "\t" << generate_name(ref, root_counter) << " -> "
                    << generate_name(context, root_counter + (int) roots.size() - 1) << " [style=\"dotted\"];\n";
            }

            // Productions
            int prod_counter = 0;
            for (const auto &production : table[ref.second][ref.first].productions) {
                if (production.empty())
                    continue;
                if (not defined.count(production[0])) {
                    if (production.size() == 1) {
                        out << "\t\n\t" << table.generate_def(production[0], root_counter);
                        queue.push(production[0]);
                        defined.insert(production[0]);
                    } else {
                        out << "\t\n\tsubgraph cluster_" << generate_name(ref, root_counter) << "_derivation_"
                            << prod_counter << " {\n" << "\t\tcolor=blue style=dashed\n";
                        for (auto iter = production.rbegin(); iter != production.rend(); iter++) {
                            out << "\t\t" << table.generate_def(*iter, root_counter);
                            queue.push(*iter);
                            defined.insert(*iter);
                        }
                        prod_counter++;
                        out << "\t}\n\n";
                    }
                }
                for (const auto &node : production)
                    out << "\t" << generate_name(ref, root_counter) << " -> " << generate_name(node, root_counter)
                        << ";\n";
            }

            // Terminal
            if (ref.second.second - ref.second.first == 1 and table[ref.second][ref.first].productions.empty())
                out << "\t" << generate_name(ref, root_counter) << " -> " << "string:" << ref.second.first << ";\n";
            queue.pop();
        }
        roots.pop();
        root_counter++;
    }

    // Acquire terminal nodes
    std::vector<var_tree_ref> terminal;
    for (int i = 0; i < table.size(); ++i)
        for (const auto &pair : table[{i, i + 1}])
            if (pair.second.productions.empty())
                terminal.push_back({pair.first, {i, i + 1}});

    out << "\t\n\tstring [shape=\"record\", label=\"|";
    for (const var_tree_ref &term : terminal)
        out << " <" << term.second.first << "> " << table._alphabet[term.first] << " |";
    out << "\"];\n}" << std::endl;
    return out;
}

std::string derivation_table::generate_def(const var_tree_ref &ref, int index_helper, const std::string &label) {
    std::ostringstream oss;
    oss << generate_name(ref, index_helper) << " [label=\""
        << (label.empty() ? _alphabet[ref.first] : label) << "\"];\n";
    return oss.str();
}
