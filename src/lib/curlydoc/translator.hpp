#pragma once

#include <unordered_map>
#include <optional>

#include <treeml/tree_ext.hpp>

namespace curlydoc{

class translator{
public:
    typedef std::function<void(bool, const treeml::forest_ext&)> handler_type;

private:
    std::unordered_map<std::string, handler_type> handlers;

	std::vector<std::string> cur_tag = {""};

	void handle_image(const treeml::forest_ext& forest);

	void handle_table(const treeml::forest_ext& forest);
	void handle_cell(const treeml::forest_ext& forest);

	void translate(bool space, const treeml::tree_ext& tree);
protected:
    void report_space(bool report);

	static bool is_options(const treeml::tree_ext& forest)noexcept;
	static void check_option(const treeml::tree_ext& forest);

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

	virtual void on_paragraph(const treeml::forest_ext& forest) = 0;

	virtual void on_bold(const treeml::forest_ext& forest) = 0;
	virtual void on_italic(const treeml::forest_ext& forest) = 0;
	virtual void on_underline(const treeml::forest_ext& forest) = 0;
	virtual void on_strikethrough(const treeml::forest_ext& forest) = 0;
	virtual void on_monospace(const treeml::forest_ext& forest) = 0;

	virtual void on_header1(const treeml::forest_ext& forest) = 0;
	virtual void on_header2(const treeml::forest_ext& forest) = 0;
	virtual void on_header3(const treeml::forest_ext& forest) = 0;
	virtual void on_header4(const treeml::forest_ext& forest) = 0;
	virtual void on_header5(const treeml::forest_ext& forest) = 0;
	virtual void on_header6(const treeml::forest_ext& forest) = 0;

	struct image_params{
		std::string url;
		std::optional<uint32_t> width;
		std::optional<uint32_t> height;
	};

	virtual void on_image(const image_params& params, const treeml::forest_ext& forest) = 0;

	struct cell{
		treeml::forest_ext::const_iterator begin;
		treeml::forest_ext::const_iterator end;
		std::tuple<size_t, size_t> span = {1, 1};
	};

	struct table_row{
		std::vector<cell> cells;
		size_t span = 0;
	};

	struct table{
		size_t num_cols = 0;
		std::vector<table_row> rows;

		size_t cur_row = 0;
		void push(cell&& c);
	};
private:
	std::vector<table> cur_table;
public:
	virtual void on_table(const table& tbl, const treeml::forest_ext& forest) = 0;
};

}
