#include <fstream>

#include <clargs/parser.hpp>
#include <utki/string.hpp>
#include <papki/fs_file.hpp>

#include <curlydoc/interpreter.hpp>

#include "translator_to_html.hpp"

namespace{
void translate(const std::string& file_name, bool save_evaled){
	std::string out_file_name = utki::split(file_name, '.').front() + ".html";
	std::string evaled_file_name;
	if(save_evaled){
		evaled_file_name = utki::split(file_name, '.').front() + ".cud_evaled";
	}


	curlydoc::interpreter interpreter(
			std::make_unique<papki::fs_file>(file_name)
		);

	curlydoc::translator_to_html translator;

	interpreter.add_repeater_functions(utki::make_span(translator.list_tags()));

	std::cout << "Hello cdoc2html!" << '\n';

	std::cout << "output file name = " << out_file_name << '\n';

	interpreter.init_std_lib();
	
	auto evaled = interpreter.eval();

	if(save_evaled){
		std::ofstream outf(evaled_file_name, std::ios::binary);

		outf << treeml::to_non_ext(evaled);
	}

	translator.translate(evaled);

	std::ofstream outf(out_file_name, std::ios::binary);

	outf << "<!doctype html>" "\n"
			"<html lang=en>" "\n"
			"<head>" "\n"
			"<meta charset=utf-8>" "\n"
			"<title>curlydoc</title>" "\n"
			R"(
			<style>
				table{
					border-spacing: 0;
					border-collapse: collapse;
				}
				td, th{
					border-width: 1px;
					border-color: black;
					border-style: solid;
				}
			</style>
			)" "\n"
			"</head>" "\n"
			"<body>"
		;

	outf << translator.ss.str();

	outf << "\n" "</body>" "\n"
			"</html>" "\n";
}
}

int main(int argc, const char** argv){
	clargs::parser cli;

	bool save_evaled = false;

	cli.add("save-evaled", "save interpreter output", [&save_evaled](){save_evaled = true;});

	auto positional = cli.parse(argc, argv);

	if(positional.empty()){
		std::cout << "error: input file is not given" << '\n';
		return 1;
	}

	for(const auto& f : positional){
		translate(f, save_evaled);
	}

	return 0;
}
