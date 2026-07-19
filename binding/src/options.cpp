#include "options.h"

#include <optional>

static bool parse_mode_string(const std::string &value,
                              apex_mode_t *out,
                              std::string *error) {
  if (value == "commonmark") {
    *out = APEX_MODE_COMMONMARK;
    return true;
  }
  if (value == "gfm") {
    *out = APEX_MODE_GFM;
    return true;
  }
  if (value == "multimarkdown") {
    *out = APEX_MODE_MULTIMARKDOWN;
    return true;
  }
  if (value == "kramdown") {
    *out = APEX_MODE_KRAMDOWN;
    return true;
  }
  if (value == "unified") {
    *out = APEX_MODE_UNIFIED;
    return true;
  }
  if (value == "quarto") {
    *out = APEX_MODE_QUARTO;
    return true;
  }
  *error = "invalid mode: " + value;
  return false;
}

bool apex_node_options_from_js(Napi::Env env,
                               Napi::Value value,
                               apex_options *out,
                               ApexNodeOptionStorage *storage,
                               std::string *error) {
  (void)env;
  (void)storage;
  if (value.IsUndefined() || value.IsNull()) {
    *out = apex_options_default();
    return true;
  }
  if (!value.IsObject()) {
    *error = "options must be an object";
    return false;
  }
  Napi::Object obj = value.As<Napi::Object>();
  Napi::Array names = obj.GetPropertyNames();
  std::optional<apex_mode_t> mode;
  for (uint32_t i = 0; i < names.Length(); ++i) {
    std::string name = names.Get(i).As<Napi::String>().Utf8Value();
    if (name == "mode") {
      Napi::Value mode_val = obj.Get(name);
      if (!mode_val.IsString()) {
        *error = "mode must be a string";
        return false;
      }
      apex_mode_t parsed;
      std::string mode_str = mode_val.As<Napi::String>().Utf8Value();
      if (!parse_mode_string(mode_str, &parsed, error)) {
        return false;
      }
      mode = parsed;
    } else {
      *error = "Unknown option: " + name;
      return false;
    }
  }
  if (mode.has_value()) {
    *out = apex_options_for_mode(*mode);
  } else {
    *out = apex_options_default();
  }
  return true;
}
