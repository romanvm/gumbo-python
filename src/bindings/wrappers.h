#pragma once

#include <gumbo/gumbo.h>
#include <pybind11/pybind11.h>

#include <string>
#include <unordered_map>
#include <array>
#include <memory>
#include <regex>
#include <functional>

namespace gumbo_python {

  extern std::array<std::string, 3> tag_namespaces;

  extern std::unordered_map<const char*, GumboNamespaceEnum> tag_namespace_map;

  extern std::array<std::string, 4> attr_namespace_values;

  extern std::array<std::string, 4> attr_namespace_urls;

  class Node;

  using node_ptr = std::unique_ptr<Node>;

  node_ptr make_node(GumboNode* node);

  class NodeVector {
  private:
    GumboVector* vector_;
    unsigned int curr_index_ = 0;

  public:
    explicit NodeVector(GumboVector* vector) : vector_(vector) {}

    /// For Python __iter__ method
    NodeVector* iter();

    /// For Python __next__ method
    node_ptr next();

    /// Get an item from NodeVector by index
    node_ptr get_item(unsigned int index) const;

    /// Get NodeVector length
    unsigned int len() const { return vector_->length; }
  };

  class Attribute {
  private:
    GumboAttribute* attr_;

  public:
    explicit Attribute(GumboAttribute* attr) : attr_(attr) {}

    const char* name() const { return attr_->name; }

    const char* value() const { return attr_->value; }

    int attr_namespace() const { return attr_->attr_namespace; }

    std::string str() const { return "<Attribute " + std::string(attr_->name) + " = \"" + attr_->value + "\">"; }
  };

  class AttributeMap {
  private:
    GumboVector* attrs_;
    unsigned int curr_index_ = 0;

  public:
    explicit AttributeMap(GumboVector* attrs) : attrs_(attrs) {}

    AttributeMap* iter();

    Attribute next();

    Attribute get_item(const char* attr_name) const;

    bool contains(const char* attr_name) const;

    pybind11::dict as_dict() const;

    unsigned int len() const { return attrs_->length; }
  };

  class Node {
  protected:
    GumboNode* node_;

  public:
    explicit Node(GumboNode* node) : node_(node) {}
    virtual ~Node() {};

    /// Get node's parent
    node_ptr parent() const { return make_node(node_->parent); }

    virtual bool is_tag() const { return false; }

    /// Get string representation of the node
    virtual std::string str() const { return ""; }

    /// Get node type as int
    int type() const { return node_->type; }

    /// Get node offset
    unsigned int offset() const;

    /// Get node index within parent
    size_t index_within_parent() { return node_->index_within_parent; }
  };

  class TagNode : public Node {
  protected:
    GumboVector* children_;

  public:
    TagNode(GumboNode* node, GumboVector* children) : Node(node), children_(children) {}
    virtual ~TagNode() {}

    virtual NodeVector children() const { return NodeVector(children_); }

    virtual bool is_tag() const override { return true; }
  };

  class Document : public TagNode {
  public:
    explicit Document(GumboNode* node) : TagNode(node, &node->v.document.children) {}

    /// Get document doctype. Returns empty string if HTML has no doctype.
    const char* name() const { return node_->v.document.name; }

    std::string str() const override { return "<!DOCTYPE " + std::string(node_->v.document.name) + ">"; }

    /// True if there was an explicit doctype token as opposed to it being omitted.
    bool has_doctype() const { return node_->v.document.has_doctype; }

    // Fields from the doctype token, copied verbatim.
    const char* public_identifier() const { return node_->v.document.public_identifier; }
    const char* system_identifier() const { return node_->v.document.system_identifier; }
  };

  class Tag : public TagNode {
  private:
    const char* tag_name_;

  public:
    explicit Tag(GumboNode* node) : TagNode(node, &node->v.element.children) {
      tag_name_ = gumbo_normalized_tagname(node_->v.element.tag);
    }

    const char* tag_name() const { return tag_name_ ; }

    AttributeMap attributes() const { return AttributeMap(&node_->v.element.attributes); }

    std::string str() const override { return "<" + std::string(tag_name_) + ">"; }

    int tag_namespace() const { return node_->v.element.tag_namespace; }
  };

  class Text : public Node {
  public:
    explicit Text(GumboNode* node) : Node(node) {}

    std::string str() const override;

    /// Get clean text content without comment or cdata markers if any
    const char* text() const { return node_->v.text.text; }
  };

  class Output {
  private:
    GumboOutput* output_;

  public:
    explicit Output(const char* html) { output_ = gumbo_parse(html); }

    Output(const char* html, const char* fragment_ctx, const char* fragment_namespace);

    ~Output() { gumbo_destroy_output(output_); }

    /// The root <html> node
    node_ptr root() const { return make_node(output_->root); }

    /// Document node representing the HTML document
    node_ptr document() const { return make_node(output_->document); }
  };

  std::unique_ptr<Output> parse(const char* html);

  std::unique_ptr<Output> parse_fragment(const char* html, const char* container,
    const char* fragment_namespace);
}
