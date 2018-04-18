# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
SPHINXPROJ    = esphomelib
SOURCEDIR     = .
BUILDDIR      = _build


doxygen:
	doxygen Doxygen

html:
	$(SPHINXBUILD) -M html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

cleanhtml:
	rm -rf "$(BUILDDIR)/html/*"

minifyhtml: html
	./minify.sh

dist: cleanhtml doxygen html minifyhtml
	cd "$(BUILDDIR)/html" && git add --all && git commit -m "Deploy to gh-pages"

help:
	$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

webserver: html
	cd "$(BUILDDIR)/html" && python2 -m SimpleHTTPServer

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
