/* NashmiC Playground — Main Application Logic (WASM + Remote Execution) */

const STATE = {
  currentExample: null,
  isCustomCode: false,
  activeTab: "output",
  isRunning: false,
  wasmReady: false,
  wasmModule: null,
  nshCompile: null,
  nshGetErrors: null,
  wasmFree: null,
};

/* ---- Embedded NashmiC Runtime ---- */
/* This gets prepended to the generated C so it compiles standalone on Wandbox */

const NSH_RUNTIME = `
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <locale.h>
#include <time.h>

void nsh_runtime_init(void) { setlocale(LC_ALL, ""); }
void nsh_runtime_shutdown(void) { fflush(stdout); fflush(stderr); }

void nsh_itba3(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

char *nsh_i2ra(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread = getline(&line, &len, stdin);
    if (nread == -1) { free(line); return NULL; }
    if (nread > 0 && line[nread - 1] == '\\n') line[nread - 1] = '\\0';
    return line;
}

void nsh_itla3(int code) { nsh_runtime_shutdown(); exit(code); }

void nsh_array_push(void **data, int64_t *len, int64_t *cap,
                    const void *elem, int64_t elem_size) {
    if (*len >= *cap) {
        int64_t new_cap = (*cap == 0) ? 8 : (*cap) * 2;
        void *new_data = realloc(*data, new_cap * elem_size);
        if (!new_data) { fprintf(stderr, "out of memory\\n"); exit(1); }
        *data = new_data;
        *cap = new_cap;
    }
    memcpy((char *)(*data) + (*len) * elem_size, elem, elem_size);
    (*len)++;
}

int64_t nsh_array_len(int64_t len) { return len; }
void nsh_array_free(void *data) { free(data); }

char *nsh_str_concat(const char *a, const char *b) {
    if (!a) a = "";
    if (!b) b = "";
    size_t la = strlen(a), lb = strlen(b);
    char *r = malloc(la + lb + 1);
    if (!r) { fprintf(stderr, "out of memory\\n"); exit(1); }
    memcpy(r, a, la);
    memcpy(r + la, b, lb);
    r[la + lb] = '\\0';
    return r;
}

int64_t nsh_str_len(const char *s) { return s ? (int64_t)strlen(s) : 0; }

static const char *PROVERBS[] = {
    "patience turns sour to sweet",
    "the rope of lies is short",
    "the neighbor before the house",
    "a friend in need is a friend indeed",
    "knowledge is light, ignorance is darkness"
};
static int drobi_seeded = 0, drobi_count = 0;

void nsh_drobi(void) {
    if (!drobi_seeded) { srand((unsigned)time(NULL)); drobi_seeded = 1; }
    drobi_count++;
    if (drobi_count > 5) { printf("enough jokes — get back to coding\\n"); return; }
    printf("%s\\n", PROVERBS[rand() % 5]);
}

void nsh_mansaf(void) { printf("mansaf recipe — best served with jameed\\n"); }
void nsh_sahteen(void) { printf("bon appetit!\\n"); }
void nsh_nashmi(void) { printf("NashmiC — Franco-Arab Programming Language\\n"); }

/* ---- stdlib: riyadiyat (math) ---- */
#include <math.h>
double nsh_jadhr(double x) { return sqrt(x); }
double nsh_qowa(double x, double n) { return pow(x, n); }
double nsh_mutlaq(double x) { return fabs(x); }
int64_t nsh_ashwa2i(int64_t min, int64_t max) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
    if (min > max) { int64_t t = min; min = max; max = t; }
    return min + (int64_t)(rand() % (max - min + 1));
}
double nsh_jeta(double x) { return sin(x); }
double nsh_jeta_tamam(double x) { return cos(x); }
double nsh_dal(double x) { return tan(x); }
double nsh_ardiye(double x) { return floor(x); }
double nsh_sa2fiye(double x) { return ceil(x); }
double nsh_da2reb(double x) { return round(x); }
double nsh_aqall(double a, double b) { return (a < b) ? a : b; }
double nsh_akthar(double a, double b) { return (a > b) ? a : b; }
double nsh_log_tabi3i(double x) { return log(x); }
double nsh_log10_nsh(double x) { return log10(x); }

/* ---- stdlib: nusoos (strings) ---- */
#include <ctype.h>
typedef struct { char **items; int64_t len; int64_t cap; } NshStringArray;
void nsh_string_array_free(NshStringArray *arr) {
    if (!arr) return;
    for (int64_t i = 0; i < arr->len; i++) free(arr->items[i]);
    free(arr->items); arr->items = NULL; arr->len = 0; arr->cap = 0;
}
int64_t nsh_toul(const char *s) { return s ? (int64_t)strlen(s) : 0; }
char *nsh_badel(const char *s, const char *old_s, const char *new_s) {
    if (!s || !old_s || !new_s) return s ? strdup(s) : strdup("");
    size_t ol = strlen(old_s); if (ol == 0) return strdup(s);
    size_t nl = strlen(new_s);
    int cnt = 0; const char *p = s;
    while ((p = strstr(p, old_s))) { cnt++; p += ol; }
    size_t rl = strlen(s) + cnt * ((int64_t)nl - (int64_t)ol);
    char *out = malloc(rl + 1), *d = out; const char *src = s;
    while ((p = strstr(src, old_s))) {
        size_t ch = (size_t)(p - src); memcpy(d, src, ch); d += ch;
        memcpy(d, new_s, nl); d += nl; src = p + ol;
    }
    strcpy(d, src); return out;
}
int nsh_yihtawi(const char *s, const char *sub) { return (s && sub) ? (strstr(s, sub) != NULL) : 0; }
int nsh_bdaya(const char *s, const char *pfx) { return (s && pfx) ? (strncmp(s, pfx, strlen(pfx)) == 0) : 0; }
int nsh_nihaya(const char *s, const char *sfx) {
    if (!s || !sfx) return 0;
    size_t sl = strlen(s), xl = strlen(sfx);
    return xl <= sl && strcmp(s + sl - xl, sfx) == 0;
}
char *nsh_a3la(const char *s) {
    if (!s) return strdup("");
    size_t l = strlen(s); char *o = malloc(l+1);
    for (size_t i = 0; i < l; i++) o[i] = (char)toupper((unsigned char)s[i]);
    o[l] = '\\0'; return o;
}
char *nsh_asfal(const char *s) {
    if (!s) return strdup("");
    size_t l = strlen(s); char *o = malloc(l+1);
    for (size_t i = 0; i < l; i++) o[i] = (char)tolower((unsigned char)s[i]);
    o[l] = '\\0'; return o;
}
char *nsh_qass_haddi(const char *s, int64_t start, int64_t length) {
    if (!s) return strdup("");
    int64_t sl = (int64_t)strlen(s);
    if (start < 0) start = 0; if (start >= sl) return strdup("");
    if (length < 0) length = 0; if (start + length > sl) length = sl - start;
    char *o = malloc((size_t)length+1); memcpy(o, s+start, (size_t)length);
    o[length] = '\\0'; return o;
}
char nsh_harf_3ind(const char *s, int64_t i) {
    if (!s) return '\\0';
    int64_t sl = (int64_t)strlen(s);
    if (i < 0 || i >= sl) return '\\0';
    return s[i];
}
char *nsh_juz2(const char *s, int64_t start, int64_t end) {
    if (!s) return strdup("");
    int64_t sl = (int64_t)strlen(s);
    if (start < 0) start = 0; if (end > sl) end = sl;
    if (start >= end) return strdup("");
    int64_t l = end - start; char *o = malloc((size_t)l+1);
    memcpy(o, s+start, (size_t)l); o[l] = '\\0'; return o;
}
char *nsh_karrer(const char *s, int64_t n) {
    if (!s || n <= 0) return strdup("");
    size_t sl = strlen(s), total = sl * (size_t)n;
    char *o = malloc(total+1), *p = o;
    for (int64_t i = 0; i < n; i++) { memcpy(p, s, sl); p += sl; }
    *p = '\\0'; return o;
}
`;

/* ---- WASM Initialization ---- */

function initWasm() {
  const statusEl = document.getElementById("wasm-status");

  if (typeof NashmiCModule === "undefined") {
    markWasmFailed(statusEl, "WASM module not found");
    return;
  }

  STATE._stderrCapture = [];

  NashmiCModule({
    printErr: function (text) {
      STATE._stderrCapture.push(text);
    }
  }).then(function (Module) {
    STATE.wasmModule = Module;
    STATE.nshCompile = Module.cwrap("nsh_compile", "number", ["string"]);
    STATE.nshGetErrors = Module.cwrap("nsh_get_errors", "number", []);
    STATE.wasmFree = Module._free;
    STATE.wasmReady = true;

    if (statusEl) {
      statusEl.textContent = "Compiler ready";
      statusEl.className = "wasm-status ready";
    }
  }).catch(function (err) {
    markWasmFailed(statusEl, err.message || "Failed to load");
  });
}

function markWasmFailed(statusEl, reason) {
  STATE.wasmReady = false;
  if (statusEl) {
    statusEl.textContent = "WASM unavailable";
    statusEl.className = "wasm-status failed";
    statusEl.title = reason;
  }
}

/* ---- WASM Transpile: NashmiC → C ---- */

function compileWithWasm(sourceCode) {
  if (!STATE.wasmReady || !STATE.wasmModule) return null;

  var Module = STATE.wasmModule;
  STATE._stderrCapture = [];

  var resultPtr = STATE.nshCompile(sourceCode);
  var generatedC = "";
  if (resultPtr) {
    generatedC = Module.UTF8ToString(resultPtr);
    STATE.wasmFree(resultPtr);
  }

  var errPtr = STATE.nshGetErrors();
  var errors = "";
  if (errPtr) {
    errors = Module.UTF8ToString(errPtr);
    STATE.wasmFree(errPtr);
  }

  var stderrOutput = STATE._stderrCapture.join("\n");
  if (stderrOutput) {
    errors = stderrOutput + (errors ? "\n" + errors : "");
  }

  return { generatedC: generatedC, errors: errors };
}

/* ---- Remote Execution via Wandbox ---- */

function buildStandaloneC(generatedC) {
  /* Strip the #include "nsh_runtime.h" line — we inline the runtime */
  var code = generatedC.replace(/#include\s*"nsh_runtime\.h"\s*\n?/g, "");
  return NSH_RUNTIME + "\n" + code;
}

function executeOnWandbox(fullCCode) {
  return fetch("https://wandbox.org/api/compile.json", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      code: fullCCode,
      compiler: "gcc-head-c",
      options: "-std=gnu11 -O2",
      "compiler-option-raw": "-w\n-lm"
    })
  })
  .then(function (response) {
    if (!response.ok) throw new Error("Wandbox returned " + response.status);
    return response.json();
  })
  .then(function (data) {
    return {
      output: data.program_output || "",
      compilerError: data.compiler_error || "",
      programError: data.program_error || "",
      status: data.status || "0"
    };
  });
}

/* ---- Strip ANSI escape codes ---- */

function stripAnsi(text) {
  return text.replace(/\x1b\[[0-9;]*m/g, "");
}

/* ---- Playground Core ---- */

function initPlayground() {
  populateExampleDropdown();
  bindEventListeners();
  loadExample(EXAMPLES[0].id);
  initWasm();
}

function populateExampleDropdown() {
  const select = document.getElementById("example-select");
  EXAMPLES.forEach(function (example) {
    const option = document.createElement("option");
    option.value = example.id;
    option.textContent = example.name;
    select.appendChild(option);
  });
}

function bindEventListeners() {
  const select = document.getElementById("example-select");
  const runBtn = document.getElementById("run-btn");
  const editor = document.getElementById("code-editor");
  const tabOutput = document.getElementById("tab-output");
  const tabGenC = document.getElementById("tab-genc");

  select.addEventListener("change", function (event) {
    loadExample(event.target.value);
  });

  runBtn.addEventListener("click", handleRun);
  editor.addEventListener("input", handleEditorChange);
  tabOutput.addEventListener("click", function () { switchTab("output"); });
  tabGenC.addEventListener("click", function () { switchTab("genc"); });

  initResizeHandle();
}

function loadExample(exampleId) {
  const example = EXAMPLES.find(function (ex) { return ex.id === exampleId; });
  if (!example) return;

  STATE.currentExample = example;
  STATE.isCustomCode = false;

  const editor = document.getElementById("code-editor");
  const select = document.getElementById("example-select");
  const fileName = document.getElementById("file-name");

  editor.value = example.code;
  select.value = example.id;
  fileName.textContent = example.id + ".nsh";

  clearOutput();
  updateRunButtonState();
}

function handleEditorChange() {
  if (!STATE.currentExample) return;

  const editor = document.getElementById("code-editor");
  STATE.isCustomCode = editor.value !== STATE.currentExample.code;

  const fileName = document.getElementById("file-name");
  fileName.textContent = STATE.isCustomCode
    ? "custom.nsh"
    : STATE.currentExample.id + ".nsh";

  updateRunButtonState();
}

function updateRunButtonState() {
  const runBtn = document.getElementById("run-btn");
  runBtn.classList.toggle("custom-mode", STATE.isCustomCode);
}

/* ---- Run Handler ---- */

function handleRun() {
  if (STATE.isRunning) return;

  const editor = document.getElementById("code-editor");
  const sourceCode = editor.value;

  if (!STATE.wasmReady) {
    /* No WASM — use cached output for pre-loaded examples */
    if (!STATE.isCustomCode && STATE.currentExample) {
      handleCachedRun();
    } else {
      showCustomCodeFallback();
    }
    return;
  }

  /* Transpile NashmiC → C via WASM, then execute via Wandbox */
  runFullPipeline(sourceCode);
}

function runFullPipeline(sourceCode) {
  STATE.isRunning = true;
  const runBtn = document.getElementById("run-btn");
  const outputEl = document.getElementById("output-text");

  runBtn.disabled = true;
  runBtn.innerHTML =
    '<span class="btn-icon running-indicator">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Transpiling...';

  outputEl.className = "output-text running";
  outputEl.textContent = "\u25b6 Transpiling NashmiC \u2192 C...\n";

  setTimeout(function () {
    /* Step 1: Transpile */
    var result = compileWithWasm(sourceCode);

    if (!result || !result.generatedC) {
      STATE.isRunning = false;
      runBtn.disabled = false;
      runBtn.innerHTML =
        '<span class="btn-icon">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Run';

      var cleanErrors = result ? stripAnsi(result.errors) : "Transpilation failed.";
      outputEl.className = "output-text error";
      outputEl.textContent = cleanErrors;

      STATE._wasmGeneratedC = null;
      STATE._wasmErrors = cleanErrors;
      STATE._programOutput = null;
      return;
    }

    var cleanErrors = stripAnsi(result.errors);
    STATE._wasmGeneratedC = result.generatedC;
    STATE._wasmErrors = cleanErrors;

    outputEl.textContent = "\u25b6 Transpiling NashmiC \u2192 C...\n\u2713 Transpiled successfully\n\u25b6 Compiling & running C code...\n";
    runBtn.innerHTML =
      '<span class="btn-icon running-indicator">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Running...';

    /* Step 2: Build standalone C and execute */
    var fullC = buildStandaloneC(result.generatedC);

    executeOnWandbox(fullC)
      .then(function (execResult) {
        STATE.isRunning = false;
        runBtn.disabled = false;
        runBtn.innerHTML =
          '<span class="btn-icon">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Run';

        var programOutput = "";
        if (execResult.compilerError) {
          programOutput += "Compiler:\n" + execResult.compilerError + "\n";
        }
        if (execResult.output) {
          programOutput += execResult.output;
        }
        if (execResult.programError) {
          programOutput += execResult.programError;
        }
        if (!programOutput) {
          programOutput = "(no output)";
        }

        STATE._programOutput = programOutput;

        if (STATE.activeTab === "genc") {
          showGenCTab();
        } else {
          outputEl.className = "output-text animate";
          outputEl.textContent = programOutput;
        }
      })
      .catch(function (err) {
        STATE.isRunning = false;
        runBtn.disabled = false;
        runBtn.innerHTML =
          '<span class="btn-icon">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Run';

        STATE._programOutput = null;
        outputEl.className = "output-text error";
        outputEl.textContent =
          "Failed to execute: " + err.message + "\n\n" +
          "The code transpiled successfully. Switch to the Generated C tab to see the output.\n" +
          "You can copy the C code and compile it locally with:\n" +
          "  gcc -std=gnu11 -O2 -o output code.c -lm";
      });
  }, 50);
}

/* ---- Cached run (no WASM, pre-loaded examples only) ---- */

function handleCachedRun() {
  STATE.isRunning = true;
  STATE._wasmGeneratedC = null;
  STATE._wasmErrors = null;
  STATE._programOutput = null;

  const runBtn = document.getElementById("run-btn");
  const outputEl = document.getElementById("output-text");

  runBtn.disabled = true;
  runBtn.innerHTML =
    '<span class="btn-icon running-indicator">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Running...';

  outputEl.className = "output-text running";
  outputEl.textContent = "\u25b6 Running...\n";

  setTimeout(function () {
    STATE.isRunning = false;
    runBtn.disabled = false;
    runBtn.innerHTML =
      '<span class="btn-icon">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Run';

    STATE._programOutput = STATE.currentExample.output;
    outputEl.className = "output-text animate";
    outputEl.textContent = STATE.currentExample.output;
  }, 800);
}

/* ---- Fallback for custom code when WASM is unavailable ---- */

function showCustomCodeFallback() {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text";
  outputEl.innerHTML = "";

  const notice = document.createElement("div");
  notice.className = "custom-notice";
  notice.innerHTML =
    "<strong>In-browser compiler not available.</strong><br><br>" +
    "Install mansaf locally to run custom code:<br><br>" +
    "<code>git clone https://github.com/Ziadstr/nashmic</code><br>" +
    "<code>cd nashmic && make install</code><br>" +
    "<code>mansaf run your_file.nsh</code>";

  outputEl.appendChild(notice);
}

/* ---- Tab Switching ---- */

function switchTab(tab) {
  STATE.activeTab = tab;

  const tabOutput = document.getElementById("tab-output");
  const tabGenC = document.getElementById("tab-genc");

  tabOutput.classList.toggle("active", tab === "output");
  tabGenC.classList.toggle("active", tab === "genc");

  if (STATE.isRunning) return;

  if (tab === "genc") {
    showGenCTab();
  } else {
    showOutputTab();
  }
}

function showGenCTab() {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text animate";

  if (STATE._wasmGeneratedC) {
    outputEl.textContent = STATE._wasmGeneratedC;
  } else if (STATE.currentExample && STATE.currentExample.generatedC) {
    outputEl.textContent = STATE.currentExample.generatedC;
  } else {
    outputEl.textContent = "(Run the code first to see generated C)";
  }
}

function showOutputTab() {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text animate";

  if (STATE._programOutput) {
    outputEl.textContent = STATE._programOutput;
  } else if (STATE.currentExample) {
    outputEl.textContent = STATE.currentExample.output || "(Run the code to see output)";
  }
}

function clearOutput() {
  STATE._wasmGeneratedC = null;
  STATE._wasmErrors = null;
  STATE._programOutput = null;

  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text placeholder";
  outputEl.innerHTML =
    '<span>Press <strong>\u0634\u063a\u0651\u0644 \u2014 Run</strong> to see the output</span>';
}

/* ---- Resize Handle ---- */

function initResizeHandle() {
  const handle = document.getElementById("resize-handle");
  const playground = document.querySelector(".playground");
  const leftPane = document.querySelector(".pane-left");

  if (!handle || !playground || !leftPane) return;

  let isResizing = false;

  handle.addEventListener("mousedown", function (event) {
    isResizing = true;
    handle.classList.add("active");
    document.body.style.cursor = "col-resize";
    document.body.style.userSelect = "none";
    event.preventDefault();
  });

  document.addEventListener("mousemove", function (event) {
    if (!isResizing) return;

    const playgroundRect = playground.getBoundingClientRect();
    const offsetX = event.clientX - playgroundRect.left;
    const totalWidth = playgroundRect.width;
    const percentage = Math.max(20, Math.min(80, (offsetX / totalWidth) * 100));

    leftPane.style.flex = "none";
    leftPane.style.width = percentage + "%";
  });

  document.addEventListener("mouseup", function () {
    if (!isResizing) return;
    isResizing = false;
    handle.classList.remove("active");
    document.body.style.cursor = "";
    document.body.style.userSelect = "";
  });
}

/* ---- Keyboard shortcuts ---- */
document.addEventListener("keydown", function (event) {
  if ((event.ctrlKey || event.metaKey) && event.key === "Enter") {
    event.preventDefault();
    handleRun();
  }
});

/* ---- Tab support in textarea ---- */
document.addEventListener("DOMContentLoaded", function () {
  const editor = document.getElementById("code-editor");
  if (!editor) return;

  editor.addEventListener("keydown", function (event) {
    if (event.key === "Tab") {
      event.preventDefault();
      const start = editor.selectionStart;
      const end = editor.selectionEnd;
      editor.value =
        editor.value.substring(0, start) +
        "    " +
        editor.value.substring(end);
      editor.selectionStart = start + 4;
      editor.selectionEnd = start + 4;
      handleEditorChange();
    }
  });
});

/* ---- Initialize ---- */
document.addEventListener("DOMContentLoaded", initPlayground);
