#include "wrappers.h"

namespace py = pybind11;
using namespace std;

namespace gumbo_python {

  const array<string, 7> node_types = {
    "document",
    "element",
    "text",
    "cdata",
    "comment",
    "whitespace",
    "template"
  };

  node_ptr make_node(GumboNode* node) {
    if (!node)
      return nullptr;
    else if (node->type == GUMBO_NODE_DOCUMENT)
      return std::make_unique<Document>(node);
    else if (node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_TEMPLATE)
      return std::make_unique<Tag>(node);
    else
      return std::make_unique<Text>(node);
  }

#pragma region NodeVector
  node_ptr NodeVector::next() {
    if (curr_index_ >= vector_->length)
      throw py::stop_iteration();
    node_ptr node = make_node(static_cast<GumboNode*>(vector_->data[curr_index_]));
    ++curr_index_;
    return node;
  }

  node_ptr NodeVector::get_item(unsigned int idx) const {
    if (idx >= vector_->length)
      throw py::index_error(std::to_string(idx));
    return make_node(static_cast<GumboNode*>(vector_->data[idx]));
  }
#pragma endregion

#pragma region Node
  unsigned int Node::offset() const {
    if (node_->type == GUMBO_NODE_DOCUMENT)
      return 0;
    else if (node_->type == GUMBO_NODE_ELEMENT || node_->type == GUMBO_NODE_TEMPLATE)
      return node_->v.element.start_pos.offset;
    else
      return node_->v.text.start_pos.offset;
  }
#pragma endregion

#pragma region Document
  Document::Document(GumboNode* node) : TagNode(node) {
    if (node_->v.document.has_doctype) {
      doctype_ = string(node_->v.document.name);
      str_ = "<Document " + doctype_ + ">";
    }
    else {
      str_ = "<Document>";
    }
  }
#pragma endregion

#pragma region Tag
  Tag::Tag(GumboNode* node) : TagNode(node) {
    tag_name_ = gumbo_normalized_tagname(node_->v.element.tag);
    str_ = "<" + tag_name_ + ">";
    GumboVector* raw_attributes = &node_->v.element.attributes;
    for (unsigned int i = 0; i < raw_attributes->length; ++i) {
      GumboAttribute* attr = static_cast<GumboAttribute*>(raw_attributes->data[i]);
      attrs_[attr->name] = attr->value;
    }
  }

  string_ptr Tag::text() const {
    if (node_->v.element.children.length == 1) {
      GumboNode* child_node = static_cast<GumboNode*>(node_->v.element.children.data[0]);
      if (child_node->type == GUMBO_NODE_TEXT)
        return make_unique<string>(child_node->v.text.text);
    }
    return nullptr;
  }

  py::object Tag::py_text() const {
    string_ptr txt = text();
    if (txt)
      return py::str(*txt);
    return py::none();
  }
#pragma endregion

#pragma region Text
  string Text::str() const {
    if (node_->type == GUMBO_NODE_TEXT)
      return str_;
    else if (node_->type == GUMBO_NODE_COMMENT)
      return "<!--" + str_ + "-->";     
    else
      return "<![CDATA[" + str_ + "]]>";
  }
#pragma endregion

#pragma region parse_html;
  std::unique_ptr<Output> parse(const std::string& html) {
    return std::make_unique<Output>(html);
}
#pragma endregion
}
