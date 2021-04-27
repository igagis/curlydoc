#include <tst/set.hpp>
#include <tst/check.hpp>

#include <treeml/tree.hpp>

#include "../../src/cud2html/translator_to_html.hpp"

namespace{
tst::set set("traslator_to_html", [](auto& suite){
	suite.template add<std::pair<std::string, std::string>>(
			"curlydoc_element_to_html_element",
			{
				{"p{hello world!}", "<p>hello world!</p>"},
				{"b{bold text}", "<b>bold text</b>"},
				{"pre b{bold} post", "pre <b>bold</b> post"},
				{"mi\"b\"{dd}le", "mi<b>dd</b>le"},
			},
			[](const auto& p){
				const auto in = treeml::read_ext(p.first.c_str());

				curlydoc::translator_to_html tr("test.cud");

				tr.translate(in);

				auto str = tr.ss.str();
				tst::check(str == p.second, SL) << "str = " << str;
			}
		);
});
}
