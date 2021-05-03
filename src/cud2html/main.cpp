#include <fstream>

#include <clargs/parser.hpp>
#include <utki/string.hpp>
#include <papki/fs_file.hpp>

#include "translator_to_html.hpp"

int main(int argc, const char** argv){
	clargs::parser cli;

	auto positional = cli.parse(argc, argv);

	if(positional.size() != 1){
		std::cout << "error: input file is not given" << '\n';
		return 1;
	}

	// curlydoc::translator_to_html translator(std::string(positional.front()));

	std::string out_file_name = utki::split(positional.front(), '.').front() + ".html";

	std::cout << "Hello cdoc2html!" << '\n';

	std::cout << "output file name = " << out_file_name << '\n';

	// translator.eval(treeml::read_ext(papki::fs_file(positional.front())));

	std::ofstream outf(out_file_name, std::ios::binary);

	outf << "<!doctype html>" "\n"
			"<html lang=en>" "\n"
			"<head>" "\n"
			"<meta charset=utf-8>" "\n"
			"<title>curlydoc</title>" "\n"
			"</head>" "\n"
			"<body>"
		;

	// outf << translator.ss.str();

	outf << "\n" "</body>" "\n"
			"</html>" "\n";

	return 0;
}
