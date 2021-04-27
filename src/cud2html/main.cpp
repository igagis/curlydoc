#include "translator_to_html.hpp"

int main(int argc, const char** argv){
	curlydoc::translator_to_html translator("sample.cud");

	std::cout << "Hello cdoc2html!" << '\n';

	return 0;
}
