import pytest
import gumbo

HTML = b'''<!DOCTYPE html>
<!-- top-level-comment -->
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Lorem Ipsum</title>
</head>
<body>
    <!-- body comment -->
    <h1>Lorem Ipsum</h1>
    <p id="p1" class="foo">Lorem ipsum dolor sit amet, consectetur adipiscing elit.</p>
    <p id="p2" class="bar">
        <strong>Fusce sed enim ac urna<br>tincidunt egestas sed nec urna.</strong>
    </p>
    <p id="p3" class="cdata"><![CDATA[Sed efficitur bibendum euismod.]]></p>
</body>
</html>'''


output = gumbo.parse(HTML)


@pytest.fixture
def document():
    return output.document


@pytest.fixture
def root_tag():
    return output.root


@pytest.fixture
def text():
    return output.root.children[2].children[3].children[0]


@pytest.fixture
def comment():
    return output.root.children[2].children[1]


@pytest.fixture
def attributes():
    return root_tag().attributes


@pytest.fixture
def children():
    return root_tag().children


def test_document(document):
    document = output.document
    assert document.parent is None
    assert document.is_tag
    assert document.type == gumbo.GUMBO_NODE_DOCUMENT
    assert document.doctype == 'html'
    assert document.offset == 0
    assert document.has_doctype
    document.children
    assert str(document) == '<!DOCTYPE html>'


def test_tag(root_tag):
    assert root_tag.parent is not None
    assert root_tag.is_tag
    assert root_tag.type == gumbo.GUMBO_NODE_ELEMENT
    assert root_tag.tag_name == 'html'
    assert root_tag.attributes
    assert root_tag.children
    assert root_tag.offset == 43
    assert str(root_tag) == '<html>'
    assert root_tag.tag_namespace == gumbo.GUMBO_NAMESPACE_HTML
    assert gumbo.TAG_NAMESPACES[root_tag.tag_namespace] == 'http://www.w3.org/1999/xhtml'


def test_attributes(attributes):
    assert 'lang' in attributes
    assert attributes.get('lang') == 'en'
    assert attributes['lang'] == 'en'
    assert 'foo' not in attributes
    assert attributes.get('foo') is None
    assert len(attributes) == 1
    assert attributes.as_dict() == {'lang': 'en'}


def test_chidlren(children):
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


def test_soup():
    assert gumbo.get_soup(HTML)
