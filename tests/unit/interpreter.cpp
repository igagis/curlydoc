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
