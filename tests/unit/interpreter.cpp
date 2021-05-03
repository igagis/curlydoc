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
            },
            [](auto& p){
                curlydoc::interpreter interpreter;

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
