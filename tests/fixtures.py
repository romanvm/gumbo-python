import pytest
import gumbo

HTML = b'''<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
    "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<!-- top-level-comment -->
<html xmlns="http://www.w3.org/1999/xhtml/" xml:lang="en" lang="en-us">
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
    <p xmlns:xlink="http://www.w3.org/xlink" class="namespace" xlink:href="http://www.google.com">
        This is used to test attribute namespaces.
    </p>
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
    return output.root.attributes


@pytest.fixture
def children():
    return root_tag().children
