#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/lib/curlydoc/interpreter.hpp"

namespace{
tst::set set0("interpreter", [](auto& suite){
	suite.template add<std::pair<std::string, std::string>>(
			"eval_is_as_expected",
			// pairs are {input, expected output}
			{
				{"hello world!", "hello world!"},
				{"hello{} world!", "hello world!"},
				{"{hello now} world!", "{hello now} world!"},
				{"hi{}{hello now} world!", "hi{}{hello now} world!"},
				{"hello asis{world{and} by the way, {nice\"weather\"} }", "hello world{and} by the way, {nice\"weather\"}"},
				{"hello def{v{bla bla}}, I say ${v}", "hello , I say bla bla"},
				{"hello def{v{asis{bla{bla{bla}} bla}}}, I say ${v}", "hello , I say bla{bla{bla}} bla"},
				{"hello def{v{bla bla}} def{v2}, I say ${v}", "hello , I say bla bla"},
				{
					R"(
						hello
						def{
							v{bla bla}
							tmpl{asis{
								hello ${@} sir!
							}}
						}
						def{v2}
						I say tmpl{good}
					)",
					"hello I say hello good sir!"
				},
				{R"(
					hello
					def{
						v{bla bla}
						tmpl{asis{
							hello ${@} sir!
						}}
					}
					def{
						tmpl{asis{
							hello tmpl{${@}} sir!
						}}
					}
					I say tmpl{good}
				)",
				"hello I say hello hello good sir! sir!"},
				{R"(
					def{
						v1{hello world!}
					}

					Hi
					for{
						i{10 20 ${v1} {${v1}} }

						x = ${i} 
					}
					end
				)", "Hi x = 10 x = 20 x = hello x = world! x =\"\"{hello world!}end"},
				{R"(
					def{
						v{bla bla}
					}
					if{${v}}
				)", ""},
				{R"(
					def{
						v{bla bla}
					}
					if{${v}}then{Hello}
				)", "Hello"},
				{R"(
					def{
						v{bla bla}
						v1
					}
					if{${v1}}then{Hello}else{World}
				)", "World"},
				{R"(
					def{
						v{bla bla}
						v1
					}
					if{${v1}}then{Hello}
				)", ""},
				{R"(
					def{
						v{bla bla}
						v1
					}
					if{${v1}}else{World}
				)", "World"},

				// if inside then
				{R"(
					def{
						v{bla bla}
						v1
					}
					if{${v}}then{
						Hi
						if{${v1}}then{Hello}
					}else{World}
				)", "Hi"},

				// if inside else
				{R"(
					def{
						v{bla bla}
						v1
					}
					if{${v1}}else{
						Hi
						if{${v}}else{bla}
					}then{Hello}
				)", "Hi"},

				// map
				{R"(
					def{
						v{bla bla}
					}
					map{ hi{${v}} ${v} bye{hi}}
				)", "hi{bla bla} ${v} bye{hi}"},
				{R"(
					def{
						v{bla bla}
					}
					Hello 
					map{ hi{${v}} ${v} bye{hi}} world
				)", "Hello hi{bla bla} ${v} bye{hi} world"},
			},
			[](auto& p){
				curlydoc::interpreter interpreter{"none"};

				const auto in = treeml::read_ext(p.first);

				auto res = interpreter.eval(in.begin(), in.end());
				tst::check(
						res == treeml::read_ext(p.second),
						[&](auto&o){o << "res = " << treeml::to_non_ext(res);},
						SL
					);
			}
		);
});
}
