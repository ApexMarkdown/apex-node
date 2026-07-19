export type ApexMode =
  | "commonmark"
  | "gfm"
  | "multimarkdown"
  | "kramdown"
  | "unified"
  | "quarto";

export type ApexOutputFormat =
  | "html"
  | "json"
  | "jsonFiltered"
  | "markdown"
  | "mmd"
  | "commonmark"
  | "kramdown"
  | "gfm"
  | "terminal"
  | "terminal256"
  | "man"
  | "manHtml"
  | "toc";

export type ApexCriticMode = "markup" | "accept" | "reject";

export type ApexIdFormat = "gfm" | "mmd";

export type ApexCaptionPosition = "above" | "below";

export type ApexWikilinkSpace = "dash" | "none" | "underscore" | "space";

export interface ApexOptions {
  mode?: ApexMode;

  /* Feature flags */
  enablePlugins?: boolean;
  allowExternalPluginDetection?: boolean;
  enableTables?: boolean;
  enableFootnotes?: boolean;
  enableDefinitionLists?: boolean;
  enableSmartTypography?: boolean;
  enableMath?: boolean;
  enableCriticMarkup?: boolean;
  enableWikiLinks?: boolean;
  enableTaskLists?: boolean;
  enableAttributes?: boolean;
  enableCallouts?: boolean;
  enablePyCallouts?: boolean;
  enableQuartoCallouts?: boolean;
  enableQuartoExtensions?: boolean;
  enableQuartoRaw?: boolean;
  enableQuartoExampleLists?: boolean;
  enableQuartoLineBlocks?: boolean;
  enableQuartoRomanLists?: boolean;
  enableQuartoCodeAttrs?: boolean;
  enableQuartoDiagrams?: boolean;
  enableQuartoShortcodes?: boolean;
  enableQuartoStrictLists?: boolean;
  enableQuartoXrefs?: boolean;
  enableMarkedExtensions?: boolean;
  enableDivs?: boolean;
  enableSpans?: boolean;
  enableGridTables?: boolean;

  /* Critic markup mode */
  criticMode?: ApexCriticMode;

  /* Metadata handling */
  stripMetadata?: boolean;
  enableMetadataVariables?: boolean;
  enableMetadataTransforms?: boolean;

  /* File inclusion */
  enableFileIncludes?: boolean;
  maxIncludeDepth?: number;
  baseDirectory?: string;

  /* Output options */
  outputFormat?: ApexOutputFormat;
  unsafe?: boolean;
  validateUtf8?: boolean;
  githubPreLang?: boolean;
  standalone?: boolean;
  pretty?: boolean;
  xhtml?: boolean;
  strictXhtml?: boolean;
  stylesheetPaths?: string[];
  documentTitle?: string;

  /* Terminal / CLI rendering options */
  themeName?: string;
  terminalWidth?: number;
  paginate?: boolean;
  paginateSymbols?: boolean;
  terminalInlineImages?: boolean;
  terminalImageWidth?: number;

  /* Line break handling */
  hardbreaks?: boolean;
  nobreaks?: boolean;

  /* Header ID generation */
  generateHeaderIds?: boolean;
  headerAnchors?: boolean;
  idFormat?: ApexIdFormat;

  /* TOC depth defaults */
  tocMin?: number;
  tocMax?: number;

  /* Table options */
  relaxedTables?: boolean;
  captionPosition?: ApexCaptionPosition;
  perCellAlignment?: boolean;

  /* List options */
  allowMixedListMarkers?: boolean;
  allowAlphaLists?: boolean;

  /* Superscript and subscript */
  enableSupSub?: boolean;

  /* Strikethrough */
  enableStrikethrough?: boolean;

  /* Autolink options */
  enableAutolink?: boolean;
  obfuscateEmails?: boolean;

  /* Image options */
  embedImages?: boolean;
  enableImageCaptions?: boolean;
  titleCaptionsOnly?: boolean;

  /* Citation options */
  enableCitations?: boolean;
  bibliographyFiles?: string[];
  cslFile?: string;
  suppressBibliography?: boolean;
  linkCitations?: boolean;
  showTooltips?: boolean;
  nocite?: string;

  /* Index options */
  enableIndices?: boolean;
  enableMmarkIndexSyntax?: boolean;
  enableTextindexSyntax?: boolean;
  enableLeanpubIndexSyntax?: boolean;
  suppressIndex?: boolean;
  groupIndexByLetter?: boolean;

  /* Wiki link options */
  wikilinkSpace?: ApexWikilinkSpace;
  wikilinkExtension?: string;
  wikilinkSanitize?: boolean;

  /* Script injection options */
  scriptTags?: string[];

  /* Stylesheet embedding options */
  embedStylesheet?: boolean;

  /* ARIA accessibility options */
  enableAria?: boolean;

  /* Emoji options */
  enableEmojiAutocorrect?: boolean;

  /* Syntax highlighting options */
  codeHighlighter?: string;
  codeLineNumbers?: boolean;
  highlightLanguageOnly?: boolean;
  codeHighlightTheme?: string;

  /* Marked / integration-specific options */
  enableWidont?: boolean;
  codeIsPoetry?: boolean;
  enableMarkdownInHtml?: boolean;
  randomFootnoteIds?: boolean;
  enableHashtags?: boolean;
  styleHashtags?: boolean;
  proofreaderMode?: boolean;
  hrPageBreak?: boolean;
  titleFromH1?: boolean;
  pageBreakBeforeFootnotes?: boolean;

  /* Source file information for plugins */
  inputFilePath?: string;

  /* AST filter options */
  astFilterCommands?: string[];
  astFilterStrict?: boolean;
}

export declare function convert(markdown: string, options?: ApexOptions): string;
export declare const version: string;
