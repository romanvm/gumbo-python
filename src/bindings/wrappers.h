#pragma once

#include <gumbo/gumbo.h>

#include <pybind11/pybind11.h>

#include <string>
#include <vector>
#include <exception>
#include <unordered_map>
#include <array>

namespace gumbo_python {

	extern const std::array<std::string, 7> node_types;

	class NodeTypeError : public std::exception {
	private:
		std::string message_;

	public:
		explicit NodeTypeError(const std::string& message) : message_(message) {}

		const char* what() const throw() { return message_.c_str(); }
	};

	class HtmlNode;

	class NodeVector {
	private:
		GumboVector* vector_;
		unsigned int index_;

	public:
		explicit NodeVector(GumboVector* vector) : vector_(vector), index_(0) {}

		NodeVector* iter() { return this; }

		HtmlNode next();

		HtmlNode get_item(unsigned int index);

		unsigned int len() { return vector_->length; }
	};


	class HtmlNode {
	private:
		GumboNode* node_;
		std::string tag_name_;
		std::unordered_map<std::string, std::string> attributes_;
		std::string text_;

		void check_if_element(const std::string& err_message);

	public:
		explicit HtmlNode(GumboNode* node);

		NodeVector children();

		bool has_chidlren();

		std::string node_type() { return node_types[node_->type]; }

		std::unordered_map<std::string, std::string>& attributes();

		std::string tag_name();

		std::string text();

		unsigned int offset();
	};


	class parse_html {
	private:
		GumboOutput* output_;

	public:
		parse_html(const std::string& html) : output_(nullptr) {
			output_ = gumbo_parse(html.c_str());
		}

		~parse_html() {
			if (output_ != nullptr) {
				gumbo_destroy_output(&kGumboDefaultOptions, output_);
			}
		}

		HtmlNode enter() { return HtmlNode(output_->root); }

		bool exit(pybind11::object t, pybind11::object v, pybind11::object tb) {
			gumbo_destroy_output(&kGumboDefaultOptions, output_);
			output_ = nullptr;
			return false;
		}
	};

}
