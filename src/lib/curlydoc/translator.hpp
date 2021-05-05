#pragma once

#include <unordered_map>

#include <treeml/tree_ext.hpp>

namespace curlydoc{

class translator{
public:
    typedef std::function<void(bool, const treeml::forest_ext&)> handler_type;

private:
    std::unordered_map<std::string, handler_type> handlers;

protected:
    void report_space(bool report);
public:
	translator();
    virtual ~translator(){}

	std::vector<std::string> list_tags()const;

    void add_tag(const std::string& tag, handler_type&& func);

    void translate(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end);

    void translate(const treeml::forest_ext& forest){
        this->translate(forest.begin(), forest.end());
    }

    virtual void on_word(const std::string& word) = 0;

	virtual void on_paragraph(const treeml::forest_ext& tree) = 0;

	virtual void on_bold(const treeml::forest_ext& tree) = 0;
	virtual void on_italic(const treeml::forest_ext& tree) = 0;
	virtual void on_underline(const treeml::forest_ext& tree) = 0;
	virtual void on_strikethrough(const treeml::forest_ext& tree) = 0;
	virtual void on_monospace(const treeml::forest_ext& tree) = 0;

	virtual void on_header1(const treeml::forest_ext& tree) = 0;
	virtual void on_header2(const treeml::forest_ext& tree) = 0;
	virtual void on_header3(const treeml::forest_ext& tree) = 0;
	virtual void on_header4(const treeml::forest_ext& tree) = 0;
	virtual void on_header5(const treeml::forest_ext& tree) = 0;
	virtual void on_header6(const treeml::forest_ext& tree) = 0;

	// TODO:
};

}
