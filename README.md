# @apexmarkdown/apex

Fast, synchronous Markdown conversion for Node.js using the native
[Apex](https://github.com/ApexMarkdown/apex) engine. Apex combines
CommonMark, GitHub Flavored Markdown, MultiMarkdown, Kramdown, Quarto, and
dozens of opt-in extensions behind one API.

## Quick start

Install the package:

```bash
npm install @apexmarkdown/apex
```

Convert Markdown with CommonJS:

```js
const { convert } = require("@apexmarkdown/apex");

const html = convert("# Hello\n\nThis is **Apex**.");
console.log(html);
```

Or with an ES module:

```js
import apex from "@apexmarkdown/apex";

const html = apex.convert("# Hello\n\nThis is **Apex**.");
```

Both examples return:

```html
<h1 id="hello">Hello</h1>
<p>This is <strong>Apex</strong>.</p>
```

By default Apex reports `unified` with a broad feature set. Pass an
explicit mode when matching another Markdown processor, or when you want
citations and indices enabled:

```js
const { convert } = require("@apexmarkdown/apex");

const html = convert(markdown, {
  mode: "gfm",
  unsafe: false
});
```

## Why Apex?

- One processor for CommonMark, GFM, MultiMarkdown, Kramdown, Quarto, and
  Apex's combined `unified` mode.
- Tables, footnotes, definition lists, attributes, callouts, math, Critic
  Markup, citations, wiki links, file includes, and more.
- HTML, normalized Markdown, JSON AST, terminal, man page, and TOC output.
- Native prebuilt binaries with no system `cmark-gfm` or `libyaml`
  installation.
- Bundled TypeScript declarations.

See the [Apex syntax guide](https://github.com/ApexMarkdown/apex/wiki/Syntax)
and [mode comparison](https://github.com/ApexMarkdown/apex/wiki/Modes) for
the supported Markdown syntax.

## Common recipes

### GitHub Flavored Markdown

```js
const html = convert(markdown, { mode: "gfm" });
```

GFM mode enables tables, task lists, strikethrough, and autolinks while
using conservative raw-HTML handling.

### Enable selected extensions

Options override the defaults selected by `mode`:

```js
const html = convert(markdown, {
  mode: "commonmark",
  enableTables: true,
  enableFootnotes: true,
  enableCallouts: true
});
```

### Create a complete HTML document

```js
const document = convert(markdown, {
  standalone: true,
  documentTitle: "Release notes",
  stylesheetPaths: ["./site.css"],
  pretty: true
});
```

### Render for a terminal

```js
const output = convert(markdown, {
  outputFormat: "terminal256",
  terminalWidth: 100,
  themeName: "dark"
});
```

## API

### `convert(markdown, options?)`

Synchronously converts a Markdown string and returns a string in the selected
output format.

```ts
function convert(markdown: string, options?: ApexOptions): string;
```

`markdown` must be a string. `options` must be an object whose own properties
are documented below. Unknown options, invalid enum values, wrong value types,
non-finite numbers, fractional integers, and integers outside the signed
32-bit range throw a `TypeError`. A native conversion failure throws an
`Error`.

### `version`

A string containing the bundled Apex engine version:

```js
const { version } = require("@apexmarkdown/apex");
console.log(version); // for example, "1.1.12"
```

`version` is a string, not a function.

### TypeScript

The package includes declarations for `convert`, `version`, `ApexOptions`,
and every string union used by the API:

```ts
import type { ApexOptions } from "@apexmarkdown/apex";

const apex: typeof import("@apexmarkdown/apex") =
  require("@apexmarkdown/apex");

const options: ApexOptions = {
  mode: "gfm",
  generateHeaderIds: true
};

const html = apex.convert("# Typed", options);
```

## Modes and defaults

`mode` selects a complete set of defaults first. Every other supplied option
then overrides that set.

- `commonmark` — strict CommonMark with most extensions disabled.
- `gfm` — GitHub tables, task lists, strikethrough, and autolinks.
- `multimarkdown` — MultiMarkdown metadata, tables, footnotes, includes,
  definition lists, math, and citations.
- `kramdown` — Kramdown-compatible attributes, footnotes, definition lists,
  and related syntax.
- `unified` — Apex's combined feature set, including citations and indices.
- `quarto` — Quarto-oriented parsing and extensions.

If `mode` is omitted, Apex starts from its no-mode defaults, which report
`unified` but leave citations and index processing disabled. Pass
`mode: "unified"` when you want citations and indices enabled by default.

Defaults otherwise vary by mode. The descriptions below explain each
override rather than claiming one value across every mode.

## Complete option reference

<!-- markdownlint-disable MD013 -->

All option names are camelCase and case-sensitive.

### Parser modes and feature sets

- **`mode`** (`"commonmark" | "gfm" | "multimarkdown" | "kramdown" | "unified" | "quarto"`) — Select the base parser mode and its defaults.
- **`enableTables`** (`boolean`) — Enable pipe tables.
- **`enableFootnotes`** (`boolean`) — Enable reference and inline footnotes.
- **`enableDefinitionLists`** (`boolean`) — Enable definition-list syntax.
- **`enableSmartTypography`** (`boolean`) — Convert quotes, dashes, ellipses, and related punctuation.
- **`enableMath`** (`boolean`) — Enable inline and display math syntax.
- **`enableCriticMarkup`** (`boolean`) — Parse Critic Markup additions, deletions, substitutions, highlights, and comments.
- **`enableWikiLinks`** (`boolean`) — Parse `[[wiki links]]`.
- **`enableTaskLists`** (`boolean`) — Enable GFM task-list checkboxes.
- **`enableAttributes`** (`boolean`) — Enable inline attribute lists and related attribute syntax.
- **`enableCallouts`** (`boolean`) — Enable Bear/Obsidian `> [!TYPE]` callouts.
- **`enablePyCallouts`** (`boolean`) — Enable Python-Markdown `!!!` callouts.
- **`enableQuartoCallouts`** (`boolean`) — Enable Quarto callout blocks.
- **`enableQuartoExtensions`** (`boolean`) — Enable the general Quarto extension set.
- **`enableQuartoRaw`** (`boolean`) — Enable Quarto raw-content blocks.
- **`enableQuartoExampleLists`** (`boolean`) — Enable Quarto example lists.
- **`enableQuartoLineBlocks`** (`boolean`) — Enable Quarto line blocks.
- **`enableQuartoRomanLists`** (`boolean`) — Enable Quarto Roman-numeral lists.
- **`enableQuartoCodeAttrs`** (`boolean`) — Enable Quarto code-block attributes.
- **`enableQuartoDiagrams`** (`boolean`) — Enable Quarto diagram handling.
- **`enableQuartoShortcodes`** (`boolean`) — Enable Quarto shortcodes.
- **`enableQuartoStrictLists`** (`boolean`) — Use Quarto's stricter list parsing.
- **`enableQuartoXrefs`** (`boolean`) — Enable Quarto cross-references.
- **`enableMarkedExtensions`** (`boolean`) — Enable Apex's Marked-compatible extensions.
- **`enableDivs`** (`boolean`) — Enable fenced divs.
- **`enableSpans`** (`boolean`) — Enable bracketed spans with attributes.
- **`enableGridTables`** (`boolean`) — Enable Pandoc-style grid tables.

### Plugins

- **`enablePlugins`** (`boolean`) — Run Apex plugin processing.
- **`allowExternalPluginDetection`** (`boolean`) — Allow discovery of external plugins.

Plugins can execute external code. Enable them only when the installed plugins
and input are trusted.

### Critic Markup

- **`criticMode`** (`"markup" | "accept" | "reject"`) — Preserve Critic Markup, accept all tracked changes, or reject them.

### Metadata and file inclusion

- **`stripMetadata`** (`boolean`) — Remove parsed metadata blocks from rendered content.
- **`enableMetadataVariables`** (`boolean`) — Expand metadata variables such as `[%key]`.
- **`enableMetadataTransforms`** (`boolean`) — Enable transforms in metadata-variable expressions.
- **`enableFileIncludes`** (`boolean`) — Resolve Apex, MultiMarkdown, and compatible file-include syntax.
- **`maxIncludeDepth`** (`number`) — Set the maximum recursive include depth as a signed 32-bit integer.
- **`baseDirectory`** (`string`) — Resolve relative includes and file-backed resources from this directory.
- **`inputFilePath`** (`string`) — Supply the source path exposed to plugins and path-aware processing.

File includes and file-backed options can read local files. Set
`baseDirectory` deliberately when converting untrusted input.

### Output

- **`outputFormat`** (`"html" | "json" | "jsonFiltered" | "markdown" | "mmd" | "commonmark" | "kramdown" | "gfm" | "terminal" | "terminal256" | "man" | "manHtml" | "toc"`) — Select the returned representation.
- **`unsafe`** (`boolean`) — Allow raw HTML instead of applying cmark's safe-HTML handling.
- **`validateUtf8`** (`boolean`) — Replace invalid UTF-8 sequences with the Unicode replacement character.
- **`githubPreLang`** (`boolean`) — Put fenced-code languages on `<pre lang="...">` in GitHub style.
- **`standalone`** (`boolean`) — Emit a complete document instead of a fragment.
- **`pretty`** (`boolean`) — Pretty-print generated HTML.
- **`xhtml`** (`boolean`) — Render void elements using XHTML syntax.
- **`strictXhtml`** (`boolean`) — Use stricter polyglot XHTML scaffolding for standalone output.
- **`stylesheetPaths`** (`string[]`) — Add stylesheet paths to standalone output.
- **`documentTitle`** (`string`) — Set the standalone document title.

`unified` mode permits raw HTML by default, while strict compatibility modes
may not. Set `unsafe: false` explicitly when processing untrusted Markdown
that must not pass raw HTML through.

### Terminal rendering

- **`themeName`** (`string`) — Select the terminal rendering theme.
- **`terminalWidth`** (`number`) — Set the terminal width as a signed 32-bit integer; `0` lets Apex determine it.
- **`paginate`** (`boolean`) — Mark terminal output for pagination. The Node binding returns the rendered string and does not open a pager.
- **`paginateSymbols`** (`boolean`) — Prefer pager-compatible symbol rendering for terminal images.
- **`terminalInlineImages`** (`boolean`) — Render terminal images through a supported external viewer.
- **`terminalImageWidth`** (`number`) — Set inline terminal image width in character cells.

### Line breaks, headings, and TOCs

- **`hardbreaks`** (`boolean`) — Treat soft line endings as hard line breaks.
- **`nobreaks`** (`boolean`) — Render soft breaks without inserting line breaks.
- **`generateHeaderIds`** (`boolean`) — Generate IDs for headings.
- **`headerAnchors`** (`boolean`) — Emit heading anchor elements instead of relying only on IDs.
- **`idFormat`** (`"gfm" | "mmd"`) — Select GFM- or MultiMarkdown-style generated IDs.
- **`tocMin`** (`number`) — Set the minimum heading level included in generated TOCs.
- **`tocMax`** (`number`) — Set the maximum heading level included in generated TOCs.

### Tables and lists

- **`relaxedTables`** (`boolean`) — Accept relaxed tables without a standard separator row.
- **`captionPosition`** (`"above" | "below"`) — Place generated table captions above or below the table.
- **`perCellAlignment`** (`boolean`) — Enable alignment markers on individual table cells.
- **`allowMixedListMarkers`** (`boolean`) — Keep mixed bullet markers in one list where the mode permits.
- **`allowAlphaLists`** (`boolean`) — Enable alphabetic ordered-list markers.

### Inline syntax and autolinks

- **`enableSupSub`** (`boolean`) — Enable superscript and subscript syntax.
- **`enableStrikethrough`** (`boolean`) — Enable `~~strikethrough~~`.
- **`enableAutolink`** (`boolean`) — Automatically link recognized URLs and email addresses.
- **`obfuscateEmails`** (`boolean`) — Obfuscate email addresses in generated links.

### Images

- **`embedImages`** (`boolean`) — Embed supported local images as base64 data URLs.
- **`enableImageCaptions`** (`boolean`) — Render eligible images as figures with captions.
- **`titleCaptionsOnly`** (`boolean`) — Require an image title instead of falling back to alt text for captions.

### Citations and bibliographies

- **`enableCitations`** (`boolean`) — Parse citation syntax and process bibliography data. Unlike the Apex CLI, setting bibliography options here does not enable citations automatically.
- **`bibliographyFiles`** (`string[]`) — Load BibTeX, CSL JSON, or CSL YAML bibliography files. Pair with `enableCitations: true`.
- **`cslFile`** (`string`) — Apply a Citation Style Language file. Pair with `enableCitations: true`.
- **`suppressBibliography`** (`boolean`) — Process citations without appending a bibliography.
- **`linkCitations`** (`boolean`) — Link in-text citations to bibliography entries.
- **`showTooltips`** (`boolean`) — Add citation details as tooltips where supported.
- **`nocite`** (`string`) — Include uncited bibliography entries using a nocite expression.

See the [citation guide](https://github.com/ApexMarkdown/apex/wiki/Citations)
for syntax and supported bibliography formats.

### Indices

- **`enableIndices`** (`boolean`) — Parse index entries and generate an index.
- **`enableMmarkIndexSyntax`** (`boolean`) — Enable mmark index syntax.
- **`enableTextindexSyntax`** (`boolean`) — Enable TextIndex syntax.
- **`enableLeanpubIndexSyntax`** (`boolean`) — Enable Leanpub index syntax.
- **`suppressIndex`** (`boolean`) — Parse index entries without appending the generated index.
- **`groupIndexByLetter`** (`boolean`) — Group generated index entries by initial letter.

### Wiki links

- **`wikilinkSpace`** (`"dash" | "none" | "underscore" | "space"`) — Choose how spaces are written in wiki-link targets.
- **`wikilinkExtension`** (`string`) — Append an extension to generated wiki-link targets.
- **`wikilinkSanitize`** (`boolean`) — Sanitize generated wiki-link targets.

### Styles, scripts, accessibility, and emoji

- **`scriptTags`** (`string[]`) — Add raw script-tag snippets to standalone output.
- **`embedStylesheet`** (`boolean`) — Embed stylesheet content instead of linking it.
- **`enableAria`** (`boolean`) — Add supported ARIA roles, labels, and relationships.
- **`enableEmojiAutocorrect`** (`boolean`) — Normalize supported emoji-related syntax and names.

### Syntax highlighting

- **`codeHighlighter`** (`string`) — Select an external highlighter such as Pygments, Skylighting, or Shiki.
- **`codeLineNumbers`** (`boolean`) — Request line numbers from syntax highlighting.
- **`highlightLanguageOnly`** (`boolean`) — Highlight only code blocks that declare a language.
- **`codeHighlightTheme`** (`string`) — Select the external highlighter theme.

External highlighting may execute a highlighter installed on the system.

### Integration-specific rendering

- **`enableWidont`** (`boolean`) — Prevent a lone final word where supported by moving it with a nonbreaking space.
- **`codeIsPoetry`** (`boolean`) — Apply poetry-oriented handling to code-like text.
- **`enableMarkdownInHtml`** (`boolean`) — Parse Markdown inside eligible HTML blocks.
- **`randomFootnoteIds`** (`boolean`) — Randomize generated footnote IDs to avoid collisions.
- **`enableHashtags`** (`boolean`) — Recognize hashtags.
- **`styleHashtags`** (`boolean`) — Add styling hooks to recognized hashtags.
- **`proofreaderMode`** (`boolean`) — Enable proofreader-oriented rendering behavior.
- **`hrPageBreak`** (`boolean`) — Treat horizontal rules as page breaks where supported.
- **`titleFromH1`** (`boolean`) — Derive the document title from the first level-one heading.
- **`pageBreakBeforeFootnotes`** (`boolean`) — Insert a page break before the footnotes section.

### AST filters

- **`astFilterCommands`** (`string[]`) — Run Pandoc-style JSON AST filter commands in order.
- **`astFilterStrict`** (`boolean`) — Fail conversion when an AST filter fails instead of continuing.

AST filters execute external commands. Do not enable filters supplied by
untrusted input.

<!-- markdownlint-enable MD013 -->

## Supported platforms

Prebuilt native addons are published for:

- macOS arm64
- macOS x64
- Linux arm64
- Linux x64

The package requires Node.js 20 or newer. Windows and other
platform/architecture combinations do not currently have prebuilt addons.
No system `cmark-gfm` or `libyaml` installation is required on supported
platforms; both libraries are statically linked.

If installation omits the optional platform package, loading Apex throws an
error identifying the detected platform. Ensure optional dependencies are not
disabled by your package-manager configuration.

## Native and security considerations

`convert()` runs synchronously on the calling thread. For large documents or
high-throughput servers, consider performing conversions in worker threads.

Some options intentionally access the filesystem or execute external
programs: plugins, file includes, bibliography and CSL files, stylesheet and
image embedding, syntax highlighters, and AST filters. Restrict these features
when processing untrusted Markdown.

## Local development

```bash
git clone --recurse-submodules https://github.com/ApexMarkdown/apex-node.git
cd apex-node
npm install
npm run build
npm test
```

If the repository was cloned without submodules:

```bash
git submodule update --init --recursive
```

To produce the prebuilt package for the current platform:

```bash
npm run package:prebuild
```

This writes `prebuilds/@apexmarkdown/apex-<platform>-<arch>/`.

## More documentation

- [Apex documentation](https://github.com/ApexMarkdown/apex/wiki)
- [Syntax](https://github.com/ApexMarkdown/apex/wiki/Syntax)
- [Modes](https://github.com/ApexMarkdown/apex/wiki/Modes)
- [Command-line options](https://github.com/ApexMarkdown/apex/wiki/Command-Line-Options)
- [Citations](https://github.com/ApexMarkdown/apex/wiki/Citations)
- [Tables](https://github.com/ApexMarkdown/apex/wiki/Tables)
- [Apex CLI and C library](https://github.com/ApexMarkdown/apex)

## License

MIT
