#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/fs_file.hpp>

#include "../../src/lib/curlydoc/interpreter.hpp"

namespace{
tst::set set0("interpreter", [](tst::suite& suite){
	suite.add<std::pair<std::string, std::string>>(
			"eval_is_as_expected",
			// pairs are {input, expected output}
			{
				{"hello world!", "hello world!"},
				{"hello{} world!", "hello world!"},
				{"hi{}hello now world!", "hi{}hello now world!"},
				{"hello asis{world{and} by the way, {nice\"weather\"} }", "hello world{and} by the way, {nice\"weather\"}"},
				{"hello defs{v{bla bla}}, I say ${v}", "hello , I say bla bla"},
				{"hello defs{v{asis{bla{bla{bla}} bla}}}, I say ${v}", "hello , I say bla{bla{bla}} bla"},
				{"hello defs{v{bla bla}} defs{v2}, I say ${v}", "hello , I say bla bla"}, // #6
				{
					R"(
						hello
						defs{
							v{bla bla}
							tmpl{asis{
								hello ${@} sir!
							}}
						}
						defs{v2}
						I say tmpl{good}
					)",
					"hello I say hello good sir!"
				},
				{R"(
					hello
					defs{
						v{bla bla}
						tmpl{asis{
							hello ${@} sir!
						}}
					}
					defs{
						tmpl{asis{
							hello tmpl{${@}} sir!
						}}
					}
					I say tmpl{good}
				)",
				"hello I say hello hello good sir! sir!"},
				{R"(
					defs{
						v1{hello world!}
					}

					Hi
					for{
						i{10 20 ${v1} g{${v1}} }

						x = ${i} 
					}
					end
				)", "Hi x = 10 x = 20 x = hello x = world! x = g{hello world!}end"}, // #9

				// if
				{R"(
					defs{
						v{bla bla}
					}
					if{${v}}
				)", ""},
				{R"(
					defs{
						v{bla bla}
					}
					if{${v}}then{Hello}
				)", "Hello"},
				{R"(
					defs{
						v{bla bla}
						v1
					}
					if{${v1}}then{Hello}else{World}
				)", "World"},
				{R"(
					defs{
						v{bla bla}
						v1
					}
					if{${v1}}then{Hello}
				)", ""},
				{R"(
					defs{
						v{bla bla}
						v1
					}
					if{${v1}}else{World}
				)", "World"}, // #14

				// if inside if
				{R"(
					defs{
						v{bla}
					}
					if{if{${v}}then{true}}then{hello}
				)", "hello"},
				{R"(
					defs{
						v
					}
					if{if{${v}}else{true}}then{hello}
				)", "hello"},
				{R"(
					defs{
						v
					}
					if{if{${v}}then{true}}else{hello}
				)", "hello"},
				{R"(
					defs{
						v{bla}
					}
					if{if{${v}}else{true}}else{hello}
				)", "hello"}, // #18

				// if inside then
				{R"(
					defs{
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
					defs{
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
					defs{
						v1
					}
					if{bla}and{${v1}}then{hello}else{bye}
				)", "bye"},
				{R"(
					defs{
						v1{true}
					}
					if{bla}and{${v1}}then{hello}else{bye}
				)", "hello"},
				{R"(
					defs{
						v1{true}
						v2
					}
					if{bla}and{${v1}}and{${v2}}then{hello}else{bye}
				)", "bye"}, // #24

				// if inside and
				{R"(
					defs{
						v{bla}
					}
					if{bla}and{if{${v}}then{true}}then{hello}
				)", "hello"},
				{R"(
					defs{
						v
					}
					if{bla}and{if{${v}}else{true}}then{hello}
				)", "hello"}, // #26

				// or
				{R"(
					if{bla}or{true}then{hello}
				)", "hello"},
				{R"(
					defs{
						v1
					}
					if{${v1}}or{true}then{hello}
				)", "hello"},
				{R"(
					defs{
						v1
						v2
					}
					if{${v1}}or{${v2}}then{hello}else{bye}
				)", "bye"},
				{R"(
					defs{
						v1
						v2
					}
					if{true}or{${v1}}and{${v2}}then{hello}else{bye}
				)", "hello"},
				{R"(
					defs{
						v1
						v2
					}
					if{${v1}}or{true}and{${v2}}then{hello}else{bye}
				)", "bye"}, // #31

				// if inside or
				{R"(
					defs{
						v
						v2{bla}
					}
					if{${v}}or{if{${v2}}then{true}}then{hello}
				)", "hello"},
				{R"(
					defs{
						v
						v2{bla}
					}
					if{${v}}or{if{${v}}else{true}}then{hello}
				)", "hello"},

				// not
				{R"(
					defs{
						v
					}
					if{not{${v}}}then{hello}
				)", "hello"},
				{R"(
					defs{
						v{bla}
					}
					if{not{${v}}}else{hello}
				)", "hello"},

				// map
				{R"(
					defs{
						v{bla bla}
					}
					map{ hi{${v}} ${v} bye{hi}}
				)", "hi{bla bla} ${v} bye{hi}"},
				{R"(
					defs{
						v{bla bla}
					}
					Hello 
					map{ hi{${v}} ${v} bye{hi}} world
				)", "Hello hi{bla bla} ${v} bye{hi} world"},

				// include
				{R"(
					Hello include{testdata/include.cudoc} World!
					And inc_var1 = ${inc_var1},
					inc_var2 = ${inc_var2}
				)", "Hello Hi World! And inc_var1 = Hello , inc_var2 = bla bla{bla{bla}}bla bla"},

				// access array element by index
				{R"(
					defs{
						a{hello world map{how{map{are{you}}}} doing?}
						i{2}
					}
					Hi at{${i} ${a}} man? I'm at{3 ${a}} at{1 ${a}} at{0 ${a}}.
				)", "Hi how{are{you}}man? I'm doing? world hello ."},
				{R"(
					defs{
						a{hello world map{how{map{are{you}}}} doing?}
						i{-2}
					}
					Hi at{${i} ${a}} man? I'm at{-1 ${a}} at{-3 ${a}} at{-4 ${a}}.
				)", "Hi how{are{you}}man? I'm doing? world hello ."}, // #40

				// access array element by key
				{R"(
					defs{
						a{map{
							x{bla bla}
							y{hey}
							z{asis{
								how{are{you}}
							}}
							bla
						}}
					}

					x = get{x ${a}} y = get{y ${a}} z = get{z ${a}} bla = get{bla ${a}}
				)", "x = bla bla y = hey z = how{are{you}}bla ="},

				// slice
				{R"(
					defs{
						v{bla bla bla}
					}
					slice{0 end ${v}}
				)", "bla bla bla"},
				{R"(
					defs{
						v{asis{bla bla{hi} bla hello{world{how{are{you}}}}}}
					}
					slice{0 end ${v}}
				)", "bla bla{hi}bla hello{world{how{are{you}}}}"},
				{R"(
					defs{
						v{asis{bla bla{hi} bla hello{world{how{are{you}}}}}}
					}
					slice{0 4 ${v}}
				)", "bla bla{hi}bla hello{world{how{are{you}}}}"},
				{R"(
					defs{
						v{asis{bla bla{hi} bla hello{world{how{are{you}}}}}}
					}
					slice{2 end ${v}}
				)", "bla hello{world{how{are{you}}}}"},
				{R"(
					defs{
						v{asis{bla bla{hi} bla hello{world{how{are{you}}}}}}
					}
					slice{-2 end ${v}}
				)", "bla hello{world{how{are{you}}}}"},
				{R"(
					defs{
						v{asis{bla bla{hi} bla hello{world{how{are{you}}}}}}
					}
					slice{1 -1 ${v}}
				)", "bla{hi}bla"},
				{R"(
					defs{
						v{asis{bla bla{hi} bla hello{world{how{are{you}}}}}}
					}
					slice{-3 3 ${v}}
				)", "bla{hi}bla"},
				{R"(
					defs{
						v{asis{bla bla{hi} bla hello{world{how{are{you}}}}}}
					}
					slice{-2 -1 ${v}}
				)", "bla"}, // #49

				// prm
				{R"(
					defs{
						v{var}
					}
					prm{x{bla} y{${v}}}
				)", "prm{x{bla}y{var}}"},

				// size
				{R"(
					defs{
						v{bla bla asis{hello} g{bla bla} g{bla bla bla} bla}
					}
					size{${v}}
				)", "6"},
				{R"(
					defs{
						v{bla bla asis{hello} g{bla bla} g{bla bla bla} bla}
						var{v}
					}
					size{hello asis{world{bla bla} bla} hi}
				)", "4"},

				// is_word
				{R"(
					if{is_word{bla}}then{hello}
				)", "hello"},
				{R"(
					if{not{is_word{bla}}}else{hello}
				)", "hello"},
				{R"(
					defs{
						v
					}
					if{is_word{${v}}}else{hello}
				)", "hello"},
				{R"(
					defs{
						v{bla}
					}
					if{is_word{${v}}}then{hello}else{world}
				)", "hello"},
				{R"(
					defs{
						v{map{bla{hi}}}
					}
					if{is_word{${v}}}then{world}else{hello}
				)", "hello"},

				// val
				{R"(
					val{hello}
				)", "hello"},
				{R"(
					val{asis{hello{bla}}}
				)", "hello"},
				{R"(
					val{asis{hello{bla{bla}}}}
				)", "hello"},
				{R"(
					defs{
						v
					}
					val{${v}}
				)", ""},

				// args
				{R"(
					args{bla}
				)", ""},
				{R"(
					args{map{bla{hello}}}
				)", "hello"},
				{R"(
					args{map{bla{hello world}}}
				)", "hello world"},
				{R"(
					defs{
						v{asis{
							hello world{bla bla{hi}}
						}}
					}
					args{map{bla{${v}}}}
				)", "hello world{bla bla{hi}}"},
				{R"(
					defs{
						v
					}
					args{${v}}
				)", ""},

				// eq
				{R"(
					if{eq{bla bla}}then{hello}
				)", "hello"},
				{R"(
					if{eq{bla hi}}else{hello}
				)", "hello"},
				{R"(
					if{eq{asis{bla{bla bla} bla{bla bla}}}}then{hello}
				)", "hello"},
				{R"(
					if{eq{asis{bla{bla bla} bla{bla hi}}}}else{hello}
				)", "hello"},

				// gt
				{R"(
					if{gt{10 0}}then{hello}else{world}
				)", "hello"},
				{R"(
					if{gt{10 3}}then{hello}else{world}
				)", "hello"},
				{R"(
					if{gt{0 10}}then{hello}else{world}
				)", "world"},
				{R"(
					if{gt{3 10}}then{hello}else{world}
				)", "world"},
				{R"(
					if{gt{10 -3}}then{hello}else{world}
				)", "hello"},
				{R"(
					if{gt{-3 10}}then{hello}else{world}
				)", "world"},
				{R"(
					if{gt{-3 -5}}then{hello}else{world}
				)", "hello"},
				{R"(
					if{gt{-5 -3}}then{hello}else{world}
				)", "world"},
				{R"(
					defs{
						v{4}
					}
					if{gt{${v} 10}}then{hello}else{world}
				)", "world"},
				{R"(
					defs{
						v{4}
						v2{9}
					}
					if{gt{${v} ${v2}}}then{hello}else{world}
				)", "world"},
				{R"(
					defs{
						v{4}
						v2{9}
					}
					if{gt{${v2} ${v}}}then{hello}else{world}
				)", "hello"},
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
