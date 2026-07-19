#include "options.h"

bool apex_node_options_from_js(Napi::Env env,
                               Napi::Value value,
                               apex_options *out,
                               ApexNodeOptionStorage *storage,
                               std::string *error) {
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
  if (names.Length() > 0) {
    *error = "Unknown option: " +
             names.Get(uint32_t(0)).As<Napi::String>().Utf8Value();
    return false;
  }
  *out = apex_options_default();
  return true;
}
