#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/fs_file.hpp>

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

				// include
				{R"(
					Hello include{testdata/include.cud} World!
					And inc_var1 = ${inc_var1},
					inc_var2 = ${inc_var2}
				)", "Hello Hi World! And inc_var1 = Hello , inc_var2 = bla bla{bla{bla}}bla bla"},

				// access array element by index
				{R"(
					def{
						a{hello world map{how{map{are{you}}}} doing?}
						i{2}
					}
					Hi ${a ${i}} man? I'm ${a 3} ${a 1} ${a 0}.
				)", "Hi how{are{you}}man? I'm doing? world hello ."},

				// access array element by key
				{R"(
					def{
						a{map{x{bla bla} y{hey} z{asis{how{are{you}}}} bla}}
					}

					x = ${a{x}} y = ${a{y}} z = ${a{z}} bla = ${a{bla}}
				)", "x = bla bla y = hey z = how{are{you}}bla ="},

				// opt
				{R"(
					def{
						v{var}
					}
					opt{x{bla} y{${v}}}
				)", "opt{x{bla}y{var}}"},

				// size
				{R"(
					def{
						v{bla bla asis{hello} {bla bla} {bla bla bla} bla}
					}
					size{v}
				)", "6"},
				{R"(
					def{
						v{bla bla asis{hello} {bla bla} {bla bla bla} bla}
						var{v}
					}
					size{${var}}
				)", "6"},
			},
			[](auto& p){
				curlydoc::interpreter interpreter(std::make_unique<papki::fs_file>("none"));

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
