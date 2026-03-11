#include <napi.h>

typedef struct TSLanguage TSLanguage;

extern "C" TSLanguage *tree_sitter_nashmic();

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports["name"] = Napi::String::New(env, "nashmic");
  auto language = Napi::External<TSLanguage>::New(env, tree_sitter_nashmic());
  exports["language"] = language;
  return exports;
}

NODE_API_MODULE(tree_sitter_nashmic_binding, Init)
