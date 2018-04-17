find _build/html/ -name "*.html" | while read html;do
  html-minifier --collapse-whitespace --remove-comments --remove-optional-tags --remove-redundant-attributes --remove-script-type-attributes --remove-tag-whitespace --use-short-doctype "$html"  -o "$html"
done
