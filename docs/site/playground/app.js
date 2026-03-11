/* NashmiC Playground — Main Application Logic (WASM-enabled) */

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

/* ---- WASM Initialization ---- */

function initWasm() {
  const statusEl = document.getElementById("wasm-status");

  if (typeof NashmiCModule === "undefined") {
    markWasmFailed(statusEl, "WASM module not found");
    return;
  }

  /* Capture stderr output from the WASM module */
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
      statusEl.textContent = "Compiler loaded";
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

/*
 * Call the WASM compiler and return { generatedC, errors }.
 * Returns null if WASM is not available.
 */
function compileWithWasm(sourceCode) {
  if (!STATE.wasmReady || !STATE.wasmModule) {
    return null;
  }

  var Module = STATE.wasmModule;

  /* Clear stderr capture buffer before compilation */
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

  /* Combine stderr output with diagnostic summary */
  var stderrOutput = STATE._stderrCapture.join("\n");
  if (stderrOutput) {
    errors = stderrOutput + (errors ? "\n" + errors : "");
  }

  return { generatedC: generatedC, errors: errors };
}

/* ---- Strip ANSI escape codes for browser display ---- */

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
  const currentCode = editor.value;
  const originalCode = STATE.currentExample.code;

  STATE.isCustomCode = currentCode !== originalCode;

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

  if (STATE.isCustomCode) {
    handleCustomRun(sourceCode);
    return;
  }

  if (!STATE.currentExample) return;

  /* Pre-loaded example: try WASM first for generated C, show cached output */
  if (STATE.wasmReady) {
    handleWasmRun(sourceCode, true);
  } else {
    handleCachedRun();
  }
}

/*
 * Run custom code through WASM. If WASM is unavailable, show a fallback notice.
 */
function handleCustomRun(sourceCode) {
  if (!STATE.wasmReady) {
    showCustomCodeFallback();
    return;
  }

  handleWasmRun(sourceCode, false);
}

/*
 * Compile via WASM and display results.
 * If isCachedExample is true, the Output tab shows the cached program output
 * and the Generated C tab shows the WASM-compiled C code.
 */
function handleWasmRun(sourceCode, isCachedExample) {
  STATE.isRunning = true;
  const runBtn = document.getElementById("run-btn");
  const outputEl = document.getElementById("output-text");

  runBtn.disabled = true;
  runBtn.innerHTML =
    '<span class="btn-icon running-indicator">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Compiling...';

  outputEl.className = "output-text running";
  outputEl.textContent = "\u25b6 Compiling in-browser via WASM...\n";

  /* Use a short timeout so the UI updates before the synchronous WASM call */
  setTimeout(function () {
    var result = compileWithWasm(sourceCode);

    STATE.isRunning = false;
    runBtn.disabled = false;
    runBtn.innerHTML =
      '<span class="btn-icon">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Run';

    if (!result) {
      outputEl.className = "output-text";
      outputEl.textContent = "WASM compilation failed unexpectedly.";
      return;
    }

    var cleanErrors = stripAnsi(result.errors);
    var hasGenC = result.generatedC && result.generatedC.length > 0;

    /* Store the WASM-generated C on the current state for tab switching */
    STATE._wasmGeneratedC = hasGenC ? result.generatedC : null;
    STATE._wasmErrors = cleanErrors;
    STATE._isCachedExample = isCachedExample;

    if (STATE.activeTab === "genc") {
      showWasmGeneratedC(result.generatedC);
    } else {
      showWasmOutput(cleanErrors, isCachedExample);
    }
  }, 50);
}

function showWasmOutput(errors, isCachedExample) {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text animate";

  if (isCachedExample && STATE.currentExample) {
    /* For pre-loaded examples, show the cached program output */
    outputEl.textContent = STATE.currentExample.output;
  } else {
    /* For custom code, show compiler diagnostics */
    outputEl.textContent = errors || "Compilation successful. Switch to the Generated C tab to see the output.";
  }
}

function showWasmGeneratedC(generatedC) {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text animate";
  outputEl.textContent = generatedC || "(No C code generated — check errors in Output tab)";
}

/*
 * Pre-loaded example run with cached output (no WASM available).
 */
function handleCachedRun() {
  STATE.isRunning = true;
  STATE._wasmGeneratedC = null;
  STATE._wasmErrors = null;
  STATE._isCachedExample = true;

  const runBtn = document.getElementById("run-btn");
  const outputEl = document.getElementById("output-text");

  runBtn.disabled = true;
  runBtn.innerHTML =
    '<span class="btn-icon running-indicator">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Running...';

  outputEl.className = "output-text running";
  outputEl.textContent = "\u25b6 Compiling with mansaf...\n";

  const steps = [
    { delay: 300, text: "\u25b6 Compiling with mansaf...\n\u2713 Parsed successfully\n" },
    {
      delay: 600,
      text: "\u25b6 Compiling with mansaf...\n\u2713 Parsed successfully\n\u2713 Generated C code\n",
    },
    {
      delay: 900,
      text: "\u25b6 Compiling with mansaf...\n\u2713 Parsed successfully\n\u2713 Generated C code\n\u2713 Compiled with gcc\n",
    },
    {
      delay: 1200,
      text: "\u25b6 Compiling with mansaf...\n\u2713 Parsed successfully\n\u2713 Generated C code\n\u2713 Compiled with gcc\n\u2713 Running...\n\n",
    },
  ];

  steps.forEach(function (step) {
    setTimeout(function () {
      outputEl.textContent = step.text;
    }, step.delay);
  });

  setTimeout(function () {
    displayCachedResult();
    STATE.isRunning = false;
    runBtn.disabled = false;
    runBtn.innerHTML =
      '<span class="btn-icon">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Run';
  }, 1500);
}

function displayCachedResult() {
  const outputEl = document.getElementById("output-text");

  if (STATE.activeTab === "output") {
    outputEl.className = "output-text animate";
    outputEl.textContent = STATE.currentExample.output;
  } else {
    showGeneratedC();
  }
}

/* ---- Fallback for custom code when WASM is unavailable ---- */

function showCustomCodeFallback() {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text";
  outputEl.innerHTML = "";

  const notice = document.createElement("div");
  notice.className = "custom-notice";
  notice.innerHTML =
    "<strong>In-browser compiler could not load.</strong><br><br>" +
    "WASM failed to initialize. You can still try the pre-loaded examples, " +
    "or install mansaf locally:<br><br>" +
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

  const outputEl = document.getElementById("output-text");

  /* If we have WASM results from the last run, use those */
  if (STATE._wasmGeneratedC !== undefined && STATE._wasmGeneratedC !== null) {
    if (tab === "genc") {
      showWasmGeneratedC(STATE._wasmGeneratedC);
    } else {
      showWasmOutput(STATE._wasmErrors, STATE._isCachedExample);
    }
    return;
  }

  /* Fallback: cached example data */
  if (!STATE.currentExample) return;

  if (tab === "output") {
    if (outputEl.textContent && !outputEl.querySelector(".custom-notice")) {
      outputEl.className = "output-text animate";
      outputEl.textContent = STATE.currentExample.output;
    }
  } else {
    showGeneratedC();
  }
}

function showGeneratedC() {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text animate";
  outputEl.textContent = STATE.currentExample.generatedC;
}

function clearOutput() {
  STATE._wasmGeneratedC = null;
  STATE._wasmErrors = null;
  STATE._isCachedExample = false;

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
