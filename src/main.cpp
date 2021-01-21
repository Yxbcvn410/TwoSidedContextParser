#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include "grammar.h"
#include "derivation_table.h"
#include "derive.h"

#define ERROR "\e[31;1mCRITICAL ERROR:\u001b[37;0m "
#define WARNING "\e[33;1mWARNING:\u001b[37;0m "
#define INPUT "\e[36;1m> \u001b[37;0m"

namespace arg_parse = boost::program_options;

template<typename T>
void warn(T warning_message) {
    std::cerr << WARNING << warning_message << std::endl;
}

template<typename T>
void die(T error_message) {
    std::cerr << ERROR << error_message << "\nThe program will be terminated immediately" << std::endl;
    std::exit(-1);
}

int main(int argc, char **argv) {
    arg_parse::options_description desc("Supported options");
    desc.add_options()
            ("help", "display help message")
            ("version", "Display parser version")
            ("grammar,g", arg_parse::value<std::string>(), "Pass grammar file")
            ("input,i", arg_parse::value<std::string>(), "Pass text to parse")
            ("input-file,f", arg_parse::value<std::string>(), "Pass text file to parse (overrides --input)")
            ("output,o", arg_parse::value<std::string>(), "Place the output into <file>");

    arg_parse::positional_options_description pos_desc;
    pos_desc.add("input", 1);

    arg_parse::variables_map args{};
    try {
        arg_parse::store(arg_parse::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), args);
    } catch (boost::program_options::error &e) {
        die(e.what());
    }
    arg_parse::notify(args);

    if (args.count("help")) {
        std::cout << "Usage: " << NAME << " [options]" << std::endl
                  << desc << std::endl;
        return 1;
    }
    if (args.count("version")) {
        std::cout << NAME << " v" << VERSION << std::endl
                  << "Copyright (C) 2020-2021 Yxbcvn410 and his missing team" << std::endl;
        return 1;
    }

    // Warnings
    if (not args.count("input") and not args.count("input-file"))
        warn("text file not specified, string will be acquired via stdin");
    if (not args.count("output"))
        warn("output file not specified, tree will be placed to stdout");

    // Init grammar
    grammar _grammar;
    if (args.count("grammar"))
        try {
            auto grammar_in = std::ifstream(args["grammar"].as<std::string>(), std::ios::in);
            if (grammar_in.bad() or grammar_in.fail())
                die("grammar file missing or corrupt");
            grammar_in >> _grammar;
        } catch (std::exception &e) { die(e.what()); }
    else
        die("grammar file not specified");
    if (not _grammar.is_binary_normal_form())
        die("grammar form is not binary normal");

    // Init text
    std::string text;
    if (args.count("input-file")) {
        auto text_in = std::ifstream(args["input-file"].as<std::string>(), std::ios::in);
        if (text_in.bad() or text_in.fail())
            die("input text file missing or corrupt");
        text_in >> text;
    } else if (args.count("input")) {
        text = args["input"].as<std::string>();
    } else {
        std::cerr << INPUT;
        std::cin >> text;
    }

    // Build derivation
    std::vector<int> string;
    try {
        string = _grammar.convert_text(text);
    } catch (std::exception &e) {
        die("failed to resolve some symbols of string to parse");
    }
    derivation_table table = build_derivation(_grammar, string);
    table.set_start(_grammar.get_start_symbol());
    table.set_alphabet(_grammar.get_alphabet());
    if (not table[{0, table.size()}].count(_grammar.get_start_symbol()))
        die("provided string cannot be derived with given grammar");

    // Write tree
    if (args.count("output")) {
        std::ofstream stream(args["output"].as<std::string>(), std::ios::out);
        stream << table;
    } else {
        std::cout << std::endl << table << std::endl;
    }
    return 0;
}