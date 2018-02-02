#pragma once

#include <gumbo/gumbo.h>
#include <pybind11/pybind11.h>

#include <string>
#include <exception>
#include <unordered_map>
#include <array>
#include <memory>

namespace gumbo_python {

  extern const std::array<std::string, 7> node_types;

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
    NodeVector* iter() { return this; }

    /// For Python __next__ method
    node_ptr next();

    /// Get an item from NodeVector by index
    node_ptr get_item(unsigned int index) const;

    /// Get NodeVector length
    unsigned int len() const { return vector_->length; }
  };


  class Node {
  protected:
    GumboNode* node_;
    std::string str_;

  public:
    explicit Node(GumboNode* node) : node_(node) {}
    virtual ~Node() {};

    /// Get string representation of the node
    std::string str() const { return str_; }

    /// Get node type
    std::string type() const { return node_types[node_->type]; }

    /// Get node's parent
    node_ptr parent() const { return make_node(node_->parent); }

    /// Get node offset
    unsigned int offset() const;
  };

  class TagNode : public Node {
  public:
    explicit TagNode(GumboNode* node) : Node(node) {}
    virtual ~TagNode() {}

    // Pybind11 does not allow to expose pure virtual classes
    // so we need to have some implementation
    virtual NodeVector children() const { return NodeVector(nullptr); }
  };

  class Document : public TagNode {
  public:
    explicit Document(GumboNode* node);

    /// Chid nodes for the Document:
    /// a html node and any top-level comment nodes.
    NodeVector children() const override { return NodeVector(&node_->v.document.children); }
  };

  class Tag : public TagNode {
  private:
    std::string tag_name_;
    std::unordered_map<std::string, std::string> attrs_;

  public:
    explicit Tag(GumboNode* node);

    std::string tag_name() const { return tag_name_; }

    std::unordered_map<std::string, std::string> attributes() const { return attrs_; }

    NodeVector children() const override { return NodeVector(&node_->v.document.children); }

    /// Returns tag's text content if the tag has only one child
    /// and this child is a TextNode, otherwise returns nullptr
    std::unique_ptr<std::string> text() const;
  };


  class Text : public Node {
  public:
    explicit Text(GumboNode* node) : Node(node) { str_ = node_->v.text.text; }
  };

  class Output {
  private:
    GumboOutput* output_;

  public:
    explicit Output(const std::string& html) {
      output_ = gumbo_parse(html.c_str());
    }

    ~Output() {
      gumbo_destroy_output(&kGumboDefaultOptions, output_);
    }

    /// The root <html> node
    node_ptr root() const { return make_node(output_->root); }

    /// Document node representing the HTML document
    node_ptr document() const { return make_node(output_->document); }
  };
}
