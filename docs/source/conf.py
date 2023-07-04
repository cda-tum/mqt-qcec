"""Sphinx configuration file."""
from __future__ import annotations

import sys

if sys.version_info < (3, 10, 0):
    import importlib_metadata as metadata
else:
    from importlib import metadata

from typing import TYPE_CHECKING

import pybtex.plugin
from pybtex.style.formatting.unsrt import Style as UnsrtStyle
from pybtex.style.template import field, href

if TYPE_CHECKING:
    from pybtex.database import Entry
    from pybtex.richtext import HRef

# -- Project information -----------------------------------------------------
project = "QCEC"
author = "Lukas Burgholzer"

release = metadata.version("mqt.qcec")
version = ".".join(release.split(".")[:3])
language = "en"
project_copyright = "Chair for Design Automation, Technical University of Munich"

# -- General configuration ---------------------------------------------------
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autosummary",
    "sphinx.ext.mathjax",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "sphinx.ext.githubpages",
    "sphinxcontrib.bibtex",
    "sphinx_copybutton",
    "hoverxref.extension",
    "nbsphinx",
    "sphinxext.opengraph",
    "sphinx_autodoc_typehints",
]

nbsphinx_execute = "auto"
highlight_language = "python3"
nbsphinx_execute_arguments = [
    "--InlineBackend.figure_formats={'svg', 'pdf'}",
    "--InlineBackend.rc=figure.dpi=200",
]
nbsphinx_kernel_name = "python3"

autosectionlabel_prefix_document = True

hoverxref_auto_ref = True
hoverxref_domains = ["cite", "py"]
hoverxref_roles = []
hoverxref_mathjax = True
hoverxref_role_types = {
    "ref": "tooltip",
    "p": "tooltip",
    "labelpar": "tooltip",
    "class": "tooltip",
    "meth": "tooltip",
    "func": "tooltip",
    "attr": "tooltip",
    "property": "tooltip",
}
exclude_patterns = ["_build", "build", "**.ipynb_checkpoints", "Thumbs.db", ".DS_Store", ".env"]

typehints_use_signature = True
typehints_use_signature_return = True
typehints_use_rtype = False
napoleon_use_rtype = False


class CDAStyle(UnsrtStyle):
    """Custom style for including PDF links."""

    def format_url(self, _e: Entry) -> HRef:
        """Format URL field as a link to the PDF."""
        url = field("url", raw=True)
        return href()[url, "[PDF]"]


pybtex.plugin.register_plugin("pybtex.style.formatting", "cda_style", CDAStyle)

bibtex_bibfiles = ["refs.bib"]
bibtex_default_style = "cda_style"

copybutton_prompt_text = r"(?:\(venv\) )?(?:\[.*\] )?\$ "
copybutton_prompt_is_regexp = True
copybutton_line_continuation_character = "\\"

autosummary_generate = True

# -- Options for HTML output -------------------------------------------------
html_theme = "furo"
html_baseurl = "https://qcec.readthedocs.io/en/latest/"
html_static_path = ["_static"]
html_theme_options = {
    "light_logo": "mqt_dark.png",
    "dark_logo": "mqt_light.png",
    "source_repository": "https://github.com/cda-tum/qcec/",
    "source_branch": "main",
    "source_directory": "docs/source",
    "navigation_with_keys": True,
}
