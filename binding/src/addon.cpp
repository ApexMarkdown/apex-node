#include <napi.h>
#include <string>
#include "apex/apex.h"
#include "options.h"

static Napi::Value Convert(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "markdown must be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string markdown = info[0].As<Napi::String>().Utf8Value();
  ApexNodeOptionStorage storage;
  apex_options options;
  std::string error;
  Napi::Value opts_val = info.Length() >= 2 ? info[1] : env.Undefined();
  if (!apex_node_options_from_js(env, opts_val, &options, &storage, &error)) {
    Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
    return env.Null();
  }

  char *html = apex_markdown_to_html(markdown.c_str(), markdown.size(), &options);
  if (!html) {
    Napi::Error::New(env, "Apex conversion failed").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::String result = Napi::String::New(env, html);
  apex_free_string(html);
  return result;
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("convert", Napi::Function::New(env, Convert));
  exports.Set("version", Napi::String::New(env, apex_version_string()));
  return exports;
}

NODE_API_MODULE(apex_node, Init)
