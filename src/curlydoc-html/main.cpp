/*
curlydoc - document markup language translator

Copyright (C) 2021 Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include <fstream>

#include <clargs/parser.hpp>
#include <curlydoc/interpreter.hpp>
#include <papki/fs_file.hpp>
#include <utki/string.hpp>

#include "translator_to_html.hpp"

namespace {
void translate(std::string_view file_name, bool save_evaled)
{
	std::string out_file_name = utki::split(file_name, '.').front() + ".html";
	std::string evaled_file_name;
	if (save_evaled) {
		evaled_file_name = utki::split(file_name, '.').front() + ".cudoc_evaled";
	}

	curlydoc::interpreter interpreter(std::make_unique<papki::fs_file>(file_name));

	curlydoc::translator_to_html translator;

	interpreter.add_repeater_functions(translator.list_tags());

	std::cout << "Hello curlydoc-html!" << '\n';

	std::cout << "output file name = " << out_file_name << '\n';

	auto evaled = interpreter.eval();

	if (save_evaled) {
		std::ofstream outf(evaled_file_name, std::ios::binary);

		outf << treeml::to_non_ext(evaled);
	}

	translator.translate(evaled);

	std::ofstream outf(out_file_name, std::ios::binary);

	outf << "<!doctype html>"
			"\n"
			"<html lang=en>"
			"\n"
			"<head>"
			"\n"
			"<meta charset=utf-8>"
			"\n"
			"<title>curlydoc</title>"
			"\n"
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
			)"
			"\n"
			"</head>"
			"\n"
			"<body>";

	outf << translator.ss.str();

	outf << "\n"
			"</body>"
			"\n"
			"</html>"
			"\n";
}
} // namespace

int main(int argc, const char** argv)
{
	clargs::parser cli;

	bool save_evaled = false;

	cli.add("save-evaled", "save interpreter output", [&save_evaled]() {
		save_evaled = true;
	});

	auto positional = cli.parse(argc, argv);

	if (positional.empty()) {
		std::cout << "error: input file is not given" << '\n';
		return 1;
	}

	for (const auto& f : positional) {
		translate(f, save_evaled);
	}

	return 0;
}
