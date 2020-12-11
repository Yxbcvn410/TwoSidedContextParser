//
// Created by Alexander on 11.12.2020.
//

#include "derive.h"

bool algorithm_pass(derivation_table &table, const std::vector<rule> &rules) {
    bool needs_repetition = false;
    for (int len = 1; len < table.size(); ++len) {
        for (int i = 0; i + len - 1 < table.size(); ++i) {
            for (const auto &rule : rules) {
                if (rule.productions.empty())
                    continue;
                bool rule_applies = true;
                parse_tree_node node;
                for (const auto &production : rule.productions) {
                    if (production.size() == 1) {
                        if (table[{i, i + len}].count(production[0]) != 0)
                            node.productions.insert({{production[0], {i, i + len}}});
                        else
                            rule_applies = false; // rule does not match
                    } else if (production.size() == 2) {
                        bool flag = false;
                        for (int s_len = 1; s_len < len; ++s_len) {
                            if (table[{i, i + s_len}].count(production[0]) != 0 and
                                table[{i + s_len, i + len}].count(production[0]) != 0) {
                                node.productions.insert({{production[0], {i,         i + s_len}},
                                                         {production[1], {i + s_len, i + len}}});
                                flag = true;
                            }
                        }
                        rule_applies &= flag;
                    } else rule_applies = false; // Not binary normal form
                }
                for (const auto &context : rule.contexts) {
                    substr_marker context_marker;
                    switch (context.second) {
                        case LEFT:
                            context_marker = {0, i};
                            break;
                        case RIGHT:
                            context_marker = {i + len, table.size()};
                            break;
                        case LEFT_EXT:
                            context_marker = {0, i + len};
                            break;
                        case RIGHT_EXT:
                            context_marker = {i, table.size()};
                            break;
                    }
                    if (context.first.size() == 1 and table[context_marker].count(context.first[0]) != 0) {
                        node.contexts.insert({context.first[0], context_marker});
                    } else rule_applies = false;
                }
                if (rule_applies) {
                    if (table[{i, i + len}].count(rule.origin) != 0) {
                        table[{i, i + len}][rule.origin] = node;
                        if (i == 0 or i + len == table.size())
                            needs_repetition = true;
                    }
                }
            }
        }
    }
    return needs_repetition;
}

derivation_table build_derivation(const grammar &_grammar, const std::vector<int> &string) {
    derivation_table table(string);
    algorithm_pass(table, _grammar.no_context_rules());
    auto all_grammar_rules = _grammar.all_rules();
    while (algorithm_pass(table, all_grammar_rules)) {}
    return table;
}