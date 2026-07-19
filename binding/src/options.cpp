#include "options.h"

#include <climits>
#include <cmath>
#include <functional>
#include <unordered_map>
#include <vector>

namespace {

using OptionHandler = std::function<bool(Napi::Env env, Napi::Value value,
                                          apex_options *out,
                                          ApexNodeOptionStorage *storage,
                                          std::string *error)>;

/* Copies a string into storage and returns a pointer stable for the
 * lifetime of the ApexNodeOptionStorage instance (owned_cstrings never
 * relocates the character buffers it holds, only the outer vector). */
char *store_cstring(ApexNodeOptionStorage *storage, const std::string &value) {
  std::vector<char> buf(value.begin(), value.end());
  buf.push_back('\0');
  storage->owned_cstrings.push_back(std::move(buf));
  return storage->owned_cstrings.back().data();
}

bool require_string(Napi::Value value, const std::string &name,
                     std::string *out_value, std::string *error) {
  if (!value.IsString()) {
    *error = name + " must be a string";
    return false;
  }
  *out_value = value.As<Napi::String>().Utf8Value();
  return true;
}

bool require_bool(Napi::Value value, const std::string &name, bool *out_value,
                   std::string *error) {
  if (!value.IsBoolean()) {
    *error = name + " must be a boolean";
    return false;
  }
  *out_value = value.As<Napi::Boolean>().Value();
  return true;
}

bool require_int(Napi::Value value, const std::string &name, int *out_value,
                  std::string *error) {
  if (!value.IsNumber()) {
    *error = name + " must be a number";
    return false;
  }
  double d = value.As<Napi::Number>().DoubleValue();
  if (!std::isfinite(d) || d != std::trunc(d)) {
    *error = name + " must be a finite integer";
    return false;
  }
  if (d < static_cast<double>(INT32_MIN) ||
      d > static_cast<double>(INT32_MAX)) {
    *error = name + " must be a finite integer within int32 range";
    return false;
  }
  *out_value = static_cast<int>(d);
  return true;
}

bool require_string_array(Napi::Value value, const std::string &name,
                           std::vector<std::string> *out_values,
                           std::string *error) {
  if (!value.IsArray()) {
    *error = name + " must be an array of strings";
    return false;
  }
  Napi::Array arr = value.As<Napi::Array>();
  out_values->clear();
  out_values->reserve(arr.Length());
  for (uint32_t i = 0; i < arr.Length(); ++i) {
    Napi::Value item = arr.Get(i);
    if (!item.IsString()) {
      *error = name + " must be an array of strings";
      return false;
    }
    out_values->push_back(item.As<Napi::String>().Utf8Value());
  }
  return true;
}

OptionHandler bool_field(bool apex_options::*field, std::string name) {
  return [field, name](Napi::Env, Napi::Value value, apex_options *out,
                        ApexNodeOptionStorage *, std::string *error) -> bool {
    bool parsed;
    if (!require_bool(value, name, &parsed, error)) return false;
    out->*field = parsed;
    return true;
  };
}

OptionHandler int_field(int apex_options::*field, std::string name) {
  return [field, name](Napi::Env, Napi::Value value, apex_options *out,
                        ApexNodeOptionStorage *, std::string *error) -> bool {
    int parsed;
    if (!require_int(value, name, &parsed, error)) return false;
    out->*field = parsed;
    return true;
  };
}

OptionHandler string_field(const char *apex_options::*field, std::string name) {
  return [field, name](Napi::Env, Napi::Value value, apex_options *out,
                        ApexNodeOptionStorage *storage, std::string *error) -> bool {
    std::string parsed;
    if (!require_string(value, name, &parsed, error)) return false;
    out->*field = store_cstring(storage, parsed);
    return true;
  };
}

template <typename EnumT>
OptionHandler enum_field(EnumT apex_options::*field, std::string name,
                          std::unordered_map<std::string, EnumT> values) {
  return [field, name, values](Napi::Env, Napi::Value value, apex_options *out,
                                ApexNodeOptionStorage *, std::string *error) -> bool {
    std::string parsed;
    if (!require_string(value, name, &parsed, error)) return false;
    auto it = values.find(parsed);
    if (it == values.end()) {
      *error = "invalid " + name + ": " + parsed;
      return false;
    }
    out->*field = it->second;
    return true;
  };
}

bool stylesheet_paths_handler(Napi::Env, Napi::Value value, apex_options *out,
                               ApexNodeOptionStorage *storage, std::string *error) {
  std::vector<std::string> items;
  if (!require_string_array(value, "stylesheetPaths", &items, error)) return false;
  storage->stylesheet_ptrs.clear();
  storage->stylesheet_ptrs.reserve(items.size());
  for (const auto &item : items) {
    storage->stylesheet_ptrs.push_back(store_cstring(storage, item));
  }
  out->stylesheet_paths =
      storage->stylesheet_ptrs.empty() ? nullptr : storage->stylesheet_ptrs.data();
  out->stylesheet_count = storage->stylesheet_ptrs.size();
  return true;
}

bool bibliography_files_handler(Napi::Env, Napi::Value value, apex_options *out,
                                 ApexNodeOptionStorage *storage, std::string *error) {
  std::vector<std::string> items;
  if (!require_string_array(value, "bibliographyFiles", &items, error)) return false;
  storage->bibliography_ptrs.clear();
  storage->bibliography_ptrs.reserve(items.size() + 1);
  for (const auto &item : items) {
    storage->bibliography_ptrs.push_back(store_cstring(storage, item));
  }
  storage->bibliography_ptrs.push_back(nullptr);
  out->bibliography_files = storage->bibliography_ptrs.data();
  return true;
}

bool script_tags_handler(Napi::Env, Napi::Value value, apex_options *out,
                          ApexNodeOptionStorage *storage, std::string *error) {
  std::vector<std::string> items;
  if (!require_string_array(value, "scriptTags", &items, error)) return false;
  storage->script_tag_ptrs.clear();
  storage->script_tag_ptrs.reserve(items.size() + 1);
  for (const auto &item : items) {
    storage->script_tag_ptrs.push_back(store_cstring(storage, item));
  }
  storage->script_tag_ptrs.push_back(nullptr);
  out->script_tags = storage->script_tag_ptrs.data();
  return true;
}

bool ast_filter_commands_handler(Napi::Env, Napi::Value value, apex_options *out,
                                  ApexNodeOptionStorage *storage, std::string *error) {
  std::vector<std::string> items;
  if (!require_string_array(value, "astFilterCommands", &items, error)) return false;
  storage->ast_filter_ptrs.clear();
  storage->ast_filter_ptrs.reserve(items.size());
  for (const auto &item : items) {
    storage->ast_filter_ptrs.push_back(store_cstring(storage, item));
  }
  out->ast_filter_commands =
      storage->ast_filter_ptrs.empty() ? nullptr : storage->ast_filter_ptrs.data();
  out->ast_filter_count = storage->ast_filter_ptrs.size();
  return true;
}

const std::unordered_map<std::string, apex_mode_t> &mode_values() {
  static const std::unordered_map<std::string, apex_mode_t> kValues = {
      {"commonmark", APEX_MODE_COMMONMARK},
      {"gfm", APEX_MODE_GFM},
      {"multimarkdown", APEX_MODE_MULTIMARKDOWN},
      {"kramdown", APEX_MODE_KRAMDOWN},
      {"unified", APEX_MODE_UNIFIED},
      {"quarto", APEX_MODE_QUARTO},
  };
  return kValues;
}

const std::unordered_map<std::string, OptionHandler> &option_handlers() {
  static const std::unordered_map<std::string, OptionHandler> kHandlers = {
      /* Booleans */
      {"enablePlugins", bool_field(&apex_options::enable_plugins, "enablePlugins")},
      {"allowExternalPluginDetection",
       bool_field(&apex_options::allow_external_plugin_detection,
                   "allowExternalPluginDetection")},
      {"enableTables", bool_field(&apex_options::enable_tables, "enableTables")},
      {"enableFootnotes", bool_field(&apex_options::enable_footnotes, "enableFootnotes")},
      {"enableDefinitionLists",
       bool_field(&apex_options::enable_definition_lists, "enableDefinitionLists")},
      {"enableSmartTypography",
       bool_field(&apex_options::enable_smart_typography, "enableSmartTypography")},
      {"enableMath", bool_field(&apex_options::enable_math, "enableMath")},
      {"enableCriticMarkup",
       bool_field(&apex_options::enable_critic_markup, "enableCriticMarkup")},
      {"enableWikiLinks", bool_field(&apex_options::enable_wiki_links, "enableWikiLinks")},
      {"enableTaskLists", bool_field(&apex_options::enable_task_lists, "enableTaskLists")},
      {"enableAttributes", bool_field(&apex_options::enable_attributes, "enableAttributes")},
      {"enableCallouts", bool_field(&apex_options::enable_callouts, "enableCallouts")},
      {"enablePyCallouts", bool_field(&apex_options::enable_py_callouts, "enablePyCallouts")},
      {"enableQuartoCallouts",
       bool_field(&apex_options::enable_quarto_callouts, "enableQuartoCallouts")},
      {"enableQuartoExtensions",
       bool_field(&apex_options::enable_quarto_extensions, "enableQuartoExtensions")},
      {"enableQuartoRaw", bool_field(&apex_options::enable_quarto_raw, "enableQuartoRaw")},
      {"enableQuartoExampleLists",
       bool_field(&apex_options::enable_quarto_example_lists, "enableQuartoExampleLists")},
      {"enableQuartoLineBlocks",
       bool_field(&apex_options::enable_quarto_line_blocks, "enableQuartoLineBlocks")},
      {"enableQuartoRomanLists",
       bool_field(&apex_options::enable_quarto_roman_lists, "enableQuartoRomanLists")},
      {"enableQuartoCodeAttrs",
       bool_field(&apex_options::enable_quarto_code_attrs, "enableQuartoCodeAttrs")},
      {"enableQuartoDiagrams",
       bool_field(&apex_options::enable_quarto_diagrams, "enableQuartoDiagrams")},
      {"enableQuartoShortcodes",
       bool_field(&apex_options::enable_quarto_shortcodes, "enableQuartoShortcodes")},
      {"enableQuartoStrictLists",
       bool_field(&apex_options::enable_quarto_strict_lists, "enableQuartoStrictLists")},
      {"enableQuartoXrefs", bool_field(&apex_options::enable_quarto_xrefs, "enableQuartoXrefs")},
      {"enableMarkedExtensions",
       bool_field(&apex_options::enable_marked_extensions, "enableMarkedExtensions")},
      {"enableDivs", bool_field(&apex_options::enable_divs, "enableDivs")},
      {"enableSpans", bool_field(&apex_options::enable_spans, "enableSpans")},
      {"enableGridTables", bool_field(&apex_options::enable_grid_tables, "enableGridTables")},
      {"stripMetadata", bool_field(&apex_options::strip_metadata, "stripMetadata")},
      {"enableMetadataVariables",
       bool_field(&apex_options::enable_metadata_variables, "enableMetadataVariables")},
      {"enableMetadataTransforms",
       bool_field(&apex_options::enable_metadata_transforms, "enableMetadataTransforms")},
      {"enableFileIncludes",
       bool_field(&apex_options::enable_file_includes, "enableFileIncludes")},
      {"unsafe", bool_field(&apex_options::unsafe, "unsafe")},
      {"validateUtf8", bool_field(&apex_options::validate_utf8, "validateUtf8")},
      {"githubPreLang", bool_field(&apex_options::github_pre_lang, "githubPreLang")},
      {"standalone", bool_field(&apex_options::standalone, "standalone")},
      {"pretty", bool_field(&apex_options::pretty, "pretty")},
      {"xhtml", bool_field(&apex_options::xhtml, "xhtml")},
      {"strictXhtml", bool_field(&apex_options::strict_xhtml, "strictXhtml")},
      {"paginate", bool_field(&apex_options::paginate, "paginate")},
      {"paginateSymbols", bool_field(&apex_options::paginate_symbols, "paginateSymbols")},
      {"terminalInlineImages",
       bool_field(&apex_options::terminal_inline_images, "terminalInlineImages")},
      {"hardbreaks", bool_field(&apex_options::hardbreaks, "hardbreaks")},
      {"nobreaks", bool_field(&apex_options::nobreaks, "nobreaks")},
      {"generateHeaderIds", bool_field(&apex_options::generate_header_ids, "generateHeaderIds")},
      {"headerAnchors", bool_field(&apex_options::header_anchors, "headerAnchors")},
      {"relaxedTables", bool_field(&apex_options::relaxed_tables, "relaxedTables")},
      {"perCellAlignment", bool_field(&apex_options::per_cell_alignment, "perCellAlignment")},
      {"allowMixedListMarkers",
       bool_field(&apex_options::allow_mixed_list_markers, "allowMixedListMarkers")},
      {"allowAlphaLists", bool_field(&apex_options::allow_alpha_lists, "allowAlphaLists")},
      {"enableSupSub", bool_field(&apex_options::enable_sup_sub, "enableSupSub")},
      {"enableStrikethrough",
       bool_field(&apex_options::enable_strikethrough, "enableStrikethrough")},
      {"enableAutolink", bool_field(&apex_options::enable_autolink, "enableAutolink")},
      {"obfuscateEmails", bool_field(&apex_options::obfuscate_emails, "obfuscateEmails")},
      {"embedImages", bool_field(&apex_options::embed_images, "embedImages")},
      {"enableImageCaptions",
       bool_field(&apex_options::enable_image_captions, "enableImageCaptions")},
      {"titleCaptionsOnly", bool_field(&apex_options::title_captions_only, "titleCaptionsOnly")},
      {"enableCitations", bool_field(&apex_options::enable_citations, "enableCitations")},
      {"suppressBibliography",
       bool_field(&apex_options::suppress_bibliography, "suppressBibliography")},
      {"linkCitations", bool_field(&apex_options::link_citations, "linkCitations")},
      {"showTooltips", bool_field(&apex_options::show_tooltips, "showTooltips")},
      {"enableIndices", bool_field(&apex_options::enable_indices, "enableIndices")},
      {"enableMmarkIndexSyntax",
       bool_field(&apex_options::enable_mmark_index_syntax, "enableMmarkIndexSyntax")},
      {"enableTextindexSyntax",
       bool_field(&apex_options::enable_textindex_syntax, "enableTextindexSyntax")},
      {"enableLeanpubIndexSyntax",
       bool_field(&apex_options::enable_leanpub_index_syntax, "enableLeanpubIndexSyntax")},
      {"suppressIndex", bool_field(&apex_options::suppress_index, "suppressIndex")},
      {"groupIndexByLetter",
       bool_field(&apex_options::group_index_by_letter, "groupIndexByLetter")},
      {"wikilinkSanitize", bool_field(&apex_options::wikilink_sanitize, "wikilinkSanitize")},
      {"embedStylesheet", bool_field(&apex_options::embed_stylesheet, "embedStylesheet")},
      {"enableAria", bool_field(&apex_options::enable_aria, "enableAria")},
      {"enableEmojiAutocorrect",
       bool_field(&apex_options::enable_emoji_autocorrect, "enableEmojiAutocorrect")},
      {"codeLineNumbers", bool_field(&apex_options::code_line_numbers, "codeLineNumbers")},
      {"highlightLanguageOnly",
       bool_field(&apex_options::highlight_language_only, "highlightLanguageOnly")},
      {"enableWidont", bool_field(&apex_options::enable_widont, "enableWidont")},
      {"codeIsPoetry", bool_field(&apex_options::code_is_poetry, "codeIsPoetry")},
      {"enableMarkdownInHtml",
       bool_field(&apex_options::enable_markdown_in_html, "enableMarkdownInHtml")},
      {"randomFootnoteIds", bool_field(&apex_options::random_footnote_ids, "randomFootnoteIds")},
      {"enableHashtags", bool_field(&apex_options::enable_hashtags, "enableHashtags")},
      {"styleHashtags", bool_field(&apex_options::style_hashtags, "styleHashtags")},
      {"proofreaderMode", bool_field(&apex_options::proofreader_mode, "proofreaderMode")},
      {"hrPageBreak", bool_field(&apex_options::hr_page_break, "hrPageBreak")},
      {"titleFromH1", bool_field(&apex_options::title_from_h1, "titleFromH1")},
      {"pageBreakBeforeFootnotes",
       bool_field(&apex_options::page_break_before_footnotes, "pageBreakBeforeFootnotes")},
      {"astFilterStrict", bool_field(&apex_options::ast_filter_strict, "astFilterStrict")},

      /* Integers */
      {"maxIncludeDepth", int_field(&apex_options::max_include_depth, "maxIncludeDepth")},
      {"terminalWidth", int_field(&apex_options::terminal_width, "terminalWidth")},
      {"terminalImageWidth",
       int_field(&apex_options::terminal_image_width, "terminalImageWidth")},
      {"tocMin", int_field(&apex_options::toc_min, "tocMin")},
      {"tocMax", int_field(&apex_options::toc_max, "tocMax")},

      /* Strings */
      {"baseDirectory", string_field(&apex_options::base_directory, "baseDirectory")},
      {"documentTitle", string_field(&apex_options::document_title, "documentTitle")},
      {"themeName", string_field(&apex_options::theme_name, "themeName")},
      {"cslFile", string_field(&apex_options::csl_file, "cslFile")},
      {"nocite", string_field(&apex_options::nocite, "nocite")},
      {"wikilinkExtension", string_field(&apex_options::wikilink_extension, "wikilinkExtension")},
      {"codeHighlighter", string_field(&apex_options::code_highlighter, "codeHighlighter")},
      {"codeHighlightTheme",
       string_field(&apex_options::code_highlight_theme, "codeHighlightTheme")},
      {"inputFilePath", string_field(&apex_options::input_file_path, "inputFilePath")},

      /* Enums (mode is handled separately, before this table is consulted) */
      {"outputFormat",
       enum_field<apex_output_format_t>(
           &apex_options::output_format, "outputFormat",
           {
               {"html", APEX_OUTPUT_HTML},
               {"json", APEX_OUTPUT_JSON},
               {"jsonFiltered", APEX_OUTPUT_JSON_FILTERED},
               {"markdown", APEX_OUTPUT_MARKDOWN},
               {"mmd", APEX_OUTPUT_MMD},
               {"commonmark", APEX_OUTPUT_COMMONMARK},
               {"kramdown", APEX_OUTPUT_KRAMDOWN},
               {"gfm", APEX_OUTPUT_GFM},
               {"terminal", APEX_OUTPUT_TERMINAL},
               {"terminal256", APEX_OUTPUT_TERMINAL256},
               {"man", APEX_OUTPUT_MAN},
               {"manHtml", APEX_OUTPUT_MAN_HTML},
               {"toc", APEX_OUTPUT_TOC},
           })},
      /* NOTE: apex.h's inline comment for critic_mode says "0=markup,
       * 1=accept, 2=reject", matching the brief's enum map verbatim. The
       * actual implementation (critic.h's critic_mode_t / critic.c /
       * apex.c's "0=accept, 1=reject, 2=markup" comment) uses the opposite
       * assignment; apex_options_default() sets critic_mode=2 for markup.
       * Mapped to match observed runtime behavior rather than the stale
       * apex.h comment -- see task-3-report.md. */
      {"criticMode",
       enum_field<int>(&apex_options::critic_mode, "criticMode",
                        {{"accept", 0}, {"reject", 1}, {"markup", 2}})},
      {"idFormat",
       enum_field<int>(&apex_options::id_format, "idFormat", {{"gfm", 0}, {"mmd", 1}})},
      {"captionPosition",
       enum_field<int>(&apex_options::caption_position, "captionPosition",
                        {{"above", 0}, {"below", 1}})},
      {"wikilinkSpace",
       enum_field<int>(&apex_options::wikilink_space, "wikilinkSpace",
                        {{"dash", 0}, {"none", 1}, {"underscore", 2}, {"space", 3}})},

      /* String arrays */
      {"stylesheetPaths", stylesheet_paths_handler},
      {"bibliographyFiles", bibliography_files_handler},
      {"scriptTags", script_tags_handler},
      {"astFilterCommands", ast_filter_commands_handler},
  };
  return kHandlers;
}

Napi::Array get_own_property_names(Napi::Env env, Napi::Object obj) {
  Napi::Function fn =
      env.Global()
          .As<Napi::Object>()
          .Get("Object")
          .As<Napi::Object>()
          .Get("getOwnPropertyNames")
          .As<Napi::Function>();
  return fn.Call({obj}).As<Napi::Array>();
}

}  // namespace

bool apex_node_options_from_js(Napi::Env env,
                               Napi::Value value,
                               apex_options *out,
                               ApexNodeOptionStorage *storage,
                               std::string *error) {
  if (value.IsUndefined() || value.IsNull()) {
    *out = apex_options_default();
    return true;
  }
  if (!value.IsObject()) {
    *error = "options must be an object";
    return false;
  }
  Napi::Object obj = value.As<Napi::Object>();

  /* mode must be resolved first: it selects the base defaults that every
   * other field then overrides. */
  if (obj.HasOwnProperty("mode")) {
    Napi::Value mode_val = obj.Get("mode");
    std::string mode_str;
    if (!require_string(mode_val, "mode", &mode_str, error)) return false;
    auto it = mode_values().find(mode_str);
    if (it == mode_values().end()) {
      *error = "invalid mode: " + mode_str;
      return false;
    }
    *out = apex_options_for_mode(it->second);
  } else {
    *out = apex_options_default();
  }

  Napi::Array names = get_own_property_names(env, obj);
  const auto &handlers = option_handlers();
  for (uint32_t i = 0; i < names.Length(); ++i) {
    std::string name = names.Get(i).As<Napi::String>().Utf8Value();
    if (name == "mode") continue;
    auto it = handlers.find(name);
    if (it == handlers.end()) {
      *error = "Unknown option: " + name;
      return false;
    }
    if (!it->second(env, obj.Get(name), out, storage, error)) {
      return false;
    }
  }
  return true;
}
