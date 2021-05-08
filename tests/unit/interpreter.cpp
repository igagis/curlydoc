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
				{"hello def{v{bla bla}} def{v2}, I say ${v}", "hello , I say bla bla"}, // #7
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
				)", "Hi x = 10 x = 20 x = hello x = world! x =\"\"{hello world!}end"}, // #10

				// if
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
				)", "World"}, // #15

				// if inside if
				{R"(
					def{
						v{bla}
					}
					if{if{${v}}then{true}}then{hello}
				)", "hello"},
				{R"(
					def{
						v
					}
					if{if{${v}}else{true}}then{hello}
				)", "hello"},
				{R"(
					def{
						v
					}
					if{if{${v}}then{true}}else{hello}
				)", "hello"},
				{R"(
					def{
						v{bla}
					}
					if{if{${v}}else{true}}else{hello}
				)", "hello"},

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

				// and
				{R"(
					if{bla}and{hi}then{hello}
				)", "hello"},
				{R"(
					def{
						v1
					}
					if{bla}and{${v1}}then{hello}else{bye}
				)", "bye"},
				{R"(
					def{
						v1{true}
					}
					if{bla}and{${v1}}then{hello}else{bye}
				)", "hello"},
				{R"(
					def{
						v1{true}
						v2
					}
					if{bla}and{${v1}}and{${v2}}then{hello}else{bye}
				)", "bye"},

				// if inside and
				{R"(
					def{
						v{bla}
					}
					if{bla}and{if{${v}}then{true}}then{hello}
				)", "hello"},
				{R"(
					def{
						v
					}
					if{bla}and{if{${v}}else{true}}then{hello}
				)", "hello"},

				// or
				{R"(
					if{bla}or{true}then{hello}
				)", "hello"},
				{R"(
					def{
						v1
					}
					if{${v1}}or{true}then{hello}
				)", "hello"},
				{R"(
					def{
						v1
						v2
					}
					if{${v1}}or{${v2}}then{hello}else{bye}
				)", "bye"},
				{R"(
					def{
						v1
						v2
					}
					if{true}or{${v1}}and{${v2}}then{hello}else{bye}
				)", "hello"},
				{R"(
					def{
						v1
						v2
					}
					if{${v1}}or{true}and{${v2}}then{hello}else{bye}
				)", "bye"},

				// if inside or
				{R"(
					def{
						v
						v2{bla}
					}
					if{${v}}or{if{${v2}}then{true}}then{hello}
				)", "hello"},
				{R"(
					def{
						v
						v2{bla}
					}
					if{${v}}or{if{${v}}else{true}}then{hello}
				)", "hello"},

				// not
				{R"(
					def{
						v
					}
					if{not{${v}}}then{hello}
				)", "hello"},
				{R"(
					def{
						v{bla}
					}
					if{not{${v}}}else{hello}
				)", "hello"},

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
