"""Test Gumbo Python wrappers API"""

from .fixtures import *


def test_document(document):
    document = output.document
    assert document.parent is None
    assert document.is_tag
    assert document.type == gumbo.GUMBO_NODE_DOCUMENT
    assert document.name == 'html'
    assert document.offset == 0
    assert document.has_doctype
    assert document.public_identifier == '-//W3C//DTD XHTML 1.1//EN'
    assert document.system_identifier == 'http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd'
    document.children
    assert str(document) == '<!DOCTYPE html>'


def test_tag(root_tag):
    assert root_tag.parent is not None
    assert root_tag.is_tag
    assert root_tag.type == gumbo.GUMBO_NODE_ELEMENT
    assert root_tag.tag_name == 'html'
    assert root_tag.attributes
    assert root_tag.children
    assert root_tag.offset == 129
    assert str(root_tag) == '<html>'
    assert root_tag.tag_namespace == gumbo.GUMBO_NAMESPACE_HTML
    assert gumbo.TAG_NAMESPACES[root_tag.tag_namespace] == 'http://www.w3.org/1999/xhtml'


def test_attributes(attributes):
    assert 'lang' in attributes
    assert attributes.get('lang') == 'en-us'
    assert attributes['lang'] == 'en-us'
    assert 'foo' not in attributes
    assert attributes.get('foo') is None
    assert len(attributes) == 3
    assert len([(attr, value) for attr, value in attributes]) == 3
    assert attributes.as_dict() == {
        'xmlns': 'http://www.w3.org/1999/xhtml/',
        'xml:lang': 'en',
        'lang': 'en-us'
        }


def test_attributes_with_namespace(attributes_with_namespace):
    # Attribute namespace parsing seems broken in Gumbo and I always get 0
    namespace = attributes_with_namespace.get_namespace('xlink:href')
    assert namespace == gumbo.GUMBO_ATTR_NAMESPACE_NONE
    assert gumbo.ATTR_NAMESPACE_VALUES[namespace] == 'none'
    assert gumbo.ATTR_NAMESPACE_URLS[namespace] == 'http://www.w3.org/1999/xhtml'


def test_children(children):
    assert len(children) == 3
    assert children[1]
    iterator = iter(children)
    assert iterator
    assert next(iterator)


def test_text(text):
    assert text.parent is not None
    assert not text.is_tag
    assert text.type == gumbo.GUMBO_NODE_TEXT
    assert text.text == 'Lorem Ipsum'
    assert str(text) == 'Lorem Ipsum'


def test_comment(comment):
    assert comment.type == gumbo.GUMBO_NODE_COMMENT
    assert comment.text == ' body comment '
    assert str(comment) == '<!-- body comment -->'


def test_parse_fragment():
    output = gumbo.parse_fragment(b'<p>Lorem ipsum</p>')
    assert len(output.root.children) == 1
