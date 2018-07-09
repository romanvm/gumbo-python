#include "wrappers.h"

namespace py = pybind11;
using namespace std;

namespace gumbo_python {
#pragma region namespaces

array<string, 3> tag_namespaces = {
  "http://www.w3.org/1999/xhtml",
  "http://www.w3.org/2000/svg",
  "http://www.w3.org/1998/Math/MathML"
};

array<string, 4> attr_namespace_values = {
  "none",
  "xlink",
  "xml",
  "xmlns"
};

array<string, 4> attr_namespace_urls = {
  "http://www.w3.org/1999/xhtml",
  "http://www.w3.org/1999/xlink",
  "http://www.w3.org/XML/1998/namespace",
  "http://www.w3.org/2000/xmlns"
};

#pragma endregion

#pragma region make_node
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
#pragma endregion

#pragma region NodeVector
  NodeVector* NodeVector::iter() {
    curr_index_ = 0;
    return this;
  }

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

#pragma region AttributeMap
  AttributeMap* AttributeMap::iter() {
    curr_index_ = 0;
    return this;
  }

  pair<const char*, const char*> AttributeMap::next() {
    if (curr_index_ >= attrs_->length)
      throw py::stop_iteration();
    GumboAttribute* attr_pair = static_cast<GumboAttribute*>(attrs_->data[curr_index_]);
    pair<const char*, const char*> item = make_pair(attr_pair->name, attr_pair->value);
    ++curr_index_;
    return item;
  }

  const char* AttributeMap::get_item(const char* attr) const {
    GumboAttribute* attr_pair = gumbo_get_attribute(attrs_, attr);
    if (!attr_pair)
      throw py::key_error(attr);
    return attr_pair->value;
  }

  py::object AttributeMap::get(const char* attr) const {
    GumboAttribute* attr_pair = gumbo_get_attribute(attrs_, attr);
    if (!attr_pair)
      return py::none();
    return py::str(attr_pair->value);
  }

  int AttributeMap::get_namespace(const char* attr) const {
    GumboAttribute* attr_pair = gumbo_get_attribute(attrs_, attr);
    if (!attr_pair)
      throw py::key_error(attr);
    return attr_pair->attr_namespace;
  }

  bool AttributeMap::contains(const char* attr) const {
    return gumbo_get_attribute(attrs_, attr) != nullptr;
  }

  py::dict AttributeMap::as_dict() const {
    py::dict attr_dict;
    for (unsigned int i = 0; i < attrs_->length; ++i) {
      GumboAttribute* attr_pair = static_cast<GumboAttribute*>(attrs_->data[i]);
      attr_dict[attr_pair->name] = attr_pair->value;
    }
    return attr_dict;
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

#pragma region Text
  string Text::str() const {
    string str{ node_->v.text.text };
    if (node_->type == GUMBO_NODE_TEXT)
      return str;
    else if (node_->type == GUMBO_NODE_WHITESPACE)
      return "<Whitespace '" + str + "'>";
    else if (node_->type == GUMBO_NODE_COMMENT)
      return "<!--" + str + "-->";     
    else
      return "<![CDATA[" + str + "]]>";
  }
#pragma endregion

#pragma region parse;
  unique_ptr<Output> parse(const char* html) {
    return make_unique<Output>(html);
}
#pragma endregion
}
