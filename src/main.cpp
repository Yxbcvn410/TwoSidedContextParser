#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include "grammar.h"
#include "derivation_table.h"
#include "derive.h"

#define ERROR "\u001b[31;1mERROR:\u001b[37;0m "

namespace arg_parse = boost::program_options;

template<typename T>
void die(T error_message) {
    std::cout << ERROR << error_message << std::endl;
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
                  << "Copyright (C) 2020 Yxbcvn410's team" << std::endl;
        return 1;
    }

    // Init grammar
    grammar _grammar;
    if (args.count("grammar"))
        std::ifstream(args["grammar"].as<std::string>(), std::ios::in) >> _grammar;
    else
        die("grammar file not specified");
    // Init text
    std::string text;
    if (args.count("input-file")) {
        std::ifstream(args["grammar"].as<std::string>(), std::ios::in) >> text;
    } else if (args.count("input")){
        text = args["input"].as<std::string>();
    } else {
        std::cout << "Enter string to test" << std::endl;
        std::cin >> text;
    }
    // TODO
    return 0;
}