## Two-sided context grammar parser

### Information

This program takes a  [grammar with two-sided contexts](https://arxiv.org/abs/1405.5598)
and a string of the same alphabet and the generates a derivation tree for it.

The grammar must be written in the [extended Backus-Naur notation](https://users.utu.fi/mikbar/pub/TUCSDissertationD204.pdf)
and match the binary normal form criteria.

The derivation tree generated is in the DOT format,
which can be then visualized using GraphViz or [this website](https://dreampuf.github.io/GraphvizOnline/).

### Building

You can build the program with CMake. 

`boost::program_options` is required.

### Usage

The string to generate derivation from can be passed via CLI, via stdin or as a file.

Examples:

```
obparser -g ./tests/an_bn_cn -i aabbcc -o tree.dot
String "aabbcc" will be derived.

obparser aabbcc  -g ./tests/an_bn_cn -o tree.dot
String "aabbcc" will be derived.

obparser -g ./tests/an_bn_cn -f string.txt -o tree.dot
Content of string.txt will be derived.

obparser -g ./tests/an_bn_cn -o tree.dot
> aabbcc
String "aabbcc" will be derived.
```

Type `obparser --help` for more info.