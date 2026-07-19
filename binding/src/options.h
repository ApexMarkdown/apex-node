#pragma once

#include <napi.h>
#include <string>
#include <vector>
#include "apex/apex.h"

struct ApexNodeOptionStorage {
  std::vector<std::string> owned_strings;
  std::vector<std::vector<char>> owned_cstrings;
  std::vector<const char *> stylesheet_ptrs;
  std::vector<char *> bibliography_ptrs;
  std::vector<char *> script_tag_ptrs;
  std::vector<const char *> ast_filter_ptrs;
};

bool apex_node_options_from_js(Napi::Env env,
                               Napi::Value value,
                               apex_options *out,
                               ApexNodeOptionStorage *storage,
                               std::string *error);
