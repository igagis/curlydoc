#include <tst/set.hpp>
#include <tst/check.hpp>

#include <treeml/tree.hpp>

#include "../../src/lib/curlydoc/interpreter.hpp"
#include "../../src/curlydoc-html/translator_to_html.hpp"

namespace{
const tst::set set("traslator_to_html", [](tst::suite& suite){
	suite.add<std::pair<std::string, std::string>>(
			"translate_to_html",
			{
				{"hello world!", "hello world!"},
				{"hi{} hello world!", "hi hello world!"},
				{"p{hello world!}", "\n<p>hello world!</p>"},
				{"b{bold text}", "<b>bold text</b>"},
				{"some b{bold text}", "some <b>bold text</b>"},
				{"i{italic text}", "<i>italic text</i>"},
				{"pre b{bold} post", "pre <b>bold</b> post"},
				{"mi\"b\"{dd}le", "mi<b>dd</b>le"},
				{"mi\"i\"{dd}le", "mi<i>dd</i>le"},
				{"some \"quoted\" stuff", "some quoted stuff"},
				{"some ins{br} stuff", "some\n<br/>\nstuff"},
				{"some ins{br}stuff", "some\n<br/>\nstuff"},
				{"some ins{br}\nstuff", "some\n<br/>\nstuff"},
				{"some\nins{br} stuff", "some\n<br/>\nstuff"},
				{"some\nins{br}\nstuff", "some\n<br/>\nstuff"},
			},
			[](const auto& p){
				const auto in = treeml::read_ext(p.first.c_str());

				curlydoc::translator_to_html tr;

				tr.translate(in);

				auto str = tr.ss.str();
				tst::check(str == p.second, SL) << "str = " << str;
			}
		);
	
	suite.add<std::pair<std::string, std::string>>(
			"interpreted_translate_to_html",
			{
				{"hello world!", "hello world!"},
				{"hi{} hello world!", "hi hello world!"},
				{"p{hello world!}", "\n<p>hello world!</p>"},
				{"b{bold text}", "<b>bold text</b>"},
				{"some b{bold text}", "some <b>bold text</b>"},
				{"i{italic text}", "<i>italic text</i>"},
				{"pre b{bold} post", "pre <b>bold</b> post"},
				{"mi\"b\"{dd}le", "mi<b>dd</b>le"},
				{"mi\"i\"{dd}le", "mi<i>dd</i>le"},
				{"some \"quoted\" stuff", "some quoted stuff"}
			},
			[](const auto& p){
				const auto in = treeml::read_ext(p.first.c_str());

				curlydoc::interpreter interpreter(nullptr);

				curlydoc::translator_to_html tr;

				interpreter.add_repeater_functions(tr.list_tags());

				tr.translate(interpreter.eval(in));

				auto str = tr.ss.str();
				tst::check(str == p.second, SL) << "str = " << str;
			}
		);
});
}
