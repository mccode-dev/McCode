#!/usr/bin/env python3
"""
inject-toc-sidebar.py

Adds two persistent elements to every page of a tex4ht-generated HTML
manual, both by finding content tex4ht already generated on one "master"
page (the front page, where \maketitle and \tableofcontents land) and
re-embedding it into every other generated page:

  1. A fixed-position table-of-contents sidebar on the left.
  2. A sticky header bar at the top showing the manual's title, linking
     back to the master/front page.

This is a pure build-time, static-HTML transformation -- no JavaScript,
no server features (SSI, fetch/AJAX) required, so it works identically
whether the resulting .tgz is served over HTTP or just extracted and
browsed locally via file://.

Usage:
    python3 inject-toc-sidebar.py <DOC>
where <DOC> is the manual's basename (e.g. "manual" or "Component_manual"),
run from the directory containing the generated <DOC>*.html files.
"""
import sys, re, glob, os

SIDEBAR_CSS = """
<style>
  #mccode-toc-sidebar {
    position: fixed; top: 0; left: 0; bottom: 0; width: 260px;
    overflow-y: auto; box-sizing: border-box; padding: 12px;
    border-right: 1px solid #ccc; background: #f7f7f7;
    font-size: 13px; line-height: 1.4;
  }
  #mccode-toc-sidebar ul { list-style: none; margin: 0; padding-left: 1em; }
  #mccode-toc-sidebar > ul { padding-left: 0; }
  #mccode-toc-sidebar a { text-decoration: none; color: #06c; }
  #mccode-toc-sidebar a:hover { text-decoration: underline; }
  #mccode-page-content { margin-left: 280px; }
  #mccode-page-header {
    position: sticky; top: 0; z-index: 100;
    background: #fff; border-bottom: 1px solid #ccc;
    padding: 8px 14px; margin: 0 0 1em 0;
    font-size: 15px; font-weight: bold;
  }
  #mccode-page-header a { text-decoration: none; color: #222; }
  #mccode-page-header a:hover { text-decoration: underline; }
  @media (max-width: 800px) {
    /* Narrow viewports: drop the fixed sidebar, show TOC inline at the top
       instead of clipping/overlapping the page content. */
    #mccode-toc-sidebar { position: static; width: auto; border-right: none;
      border-bottom: 1px solid #ccc; max-height: 40vh; }
    #mccode-page-content { margin-left: 0; }
  }
</style>
"""

def find_toc_source(doc):
    """Return (filename, toc_html) for whichever generated page contains
    the actual \\tableofcontents output -- tex4ht wraps it in
    <div class="tableofcontents">...</div> (the literal word 'Contents'
    inside it is just the first entry's link text, not a heading tag)."""
    for fn in sorted(glob.glob(f"{doc}*.html")):
        with open(fn, encoding="utf-8", errors="ignore") as f:
            content = f.read()
        m = re.search(
            r'(<div class="tableofcontents">.*?</div>)',
            content, re.IGNORECASE | re.DOTALL)
        if m:
            return fn, m.group(1)
    return None, None

def find_title(master_fn):
    """Extract the manual's title as plain text from the master page.
    Prefer the clean <h2 class="titleHead">...</h2> that \\maketitle
    produces; fall back to the <title> tag (which may be duplicated due
    to how tex4ht records TITLE metadata) if that class isn't found."""
    with open(master_fn, encoding="utf-8", errors="ignore") as f:
        content = f.read()
    m = re.search(r'<h2 class="titleHead">(.*?)</h2>', content, re.IGNORECASE | re.DOTALL)
    if not m:
        m = re.search(r'<title>(.*?)</title>', content, re.IGNORECASE | re.DOTALL)
    if not m:
        return None
    text = re.sub(r'<[^>]+>', ' ', m.group(1))   # strip any inline tags
    text = re.sub(r'\s+', ' ', text).strip()
    # crude de-duplication for the <title>-tag fallback case, where the
    # same title can appear twice separated by a comma-space
    half = len(text) // 2
    if len(text) > 20 and text[:half].strip().rstrip(',') == text[half:].strip().lstrip(', '):
        text = text[:half].strip().rstrip(',')
    return text

def linkify_images(content):
    """Wrap every <img> tag in <a href="SAME_SRC" target="_blank">, so
    clicking any figure opens the raw image standalone in a new tab."""
    def replacer(m):
        img_tag = m.group(0)
        src_match = re.search(r'src="([^"]+)"', img_tag)
        if not src_match:
            return img_tag
        return f'<a href="{src_match.group(1)}" target="_blank">{img_tag}</a>'
    return re.sub(r'<img\b[^>]*>', replacer, content, flags=re.IGNORECASE)

def inject(doc, toc_html, header_html):
    sidebar = f'<nav id="mccode-toc-sidebar">{toc_html}</nav>'
    files = sorted(glob.glob(f"{doc}*.html"))
    changed = 0
    for fn in files:
        with open(fn, encoding="utf-8", errors="ignore") as f:
            content = f.read()
        if 'id="mccode-toc-sidebar"' in content:
            continue  # already injected (re-run safety)
        content = linkify_images(content)
        # Insert CSS + sidebar right after <body ...>, then the header bar,
        # then open the content div; close it right before </body>.
        content, n1 = re.subn(
            r'(<body[^>]*>)',
            r'\1' + SIDEBAR_CSS + sidebar + header_html + '<div id="mccode-page-content">',
            content, count=1, flags=re.IGNORECASE)
        content, n2 = re.subn(
            r'(</body>)',
            r'</div>\1',
            content, count=1, flags=re.IGNORECASE)
        if n1 and n2:
            with open(fn, "w", encoding="utf-8") as f:
                f.write(content)
            changed += 1
    return changed

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(f"Usage: {sys.argv[0]} <DOC>")
    doc = sys.argv[1]
    src_fn, toc_html = find_toc_source(doc)
    if not toc_html:
        print(f"[inject-toc-sidebar] WARNING: could not find a table of "
              f"contents in any {doc}*.html file -- skipping sidebar/header "
              f"injection (pages left unmodified).", file=sys.stderr)
        sys.exit(0)  # non-fatal: don't break the build over this
    title = find_title(src_fn)
    base_page = os.path.basename(src_fn)
    if title:
        header_html = (f'<div id="mccode-page-header">'
                        f'<a href="{base_page}">{title}</a></div>')
    else:
        print(f"[inject-toc-sidebar] WARNING: could not extract a title "
              f"from {src_fn} -- injecting sidebar without a header bar.",
              file=sys.stderr)
        header_html = ''
    n = inject(doc, toc_html, header_html)
    print(f"[inject-toc-sidebar] TOC/title sourced from {src_fn}; "
          f"sidebar{'+header' if title else ''} injected into {n} page(s).")
