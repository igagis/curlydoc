#pragma once

#include <curlydoc/translator.hpp>

namespace curlydoc{

class translator_to_html : public translator{
public:
	translator_to_html();

	void handle_paragraph(const treeml::forest& tokens)override;
};

}
