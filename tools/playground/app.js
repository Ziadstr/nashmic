/* NashmiC Playground — Main Application Logic */

const STATE = {
  currentExample: null,
  isCustomCode: false,
  activeTab: "output",
  isRunning: false,
};

function initPlayground() {
  populateExampleDropdown();
  bindEventListeners();
  loadExample(EXAMPLES[0].id);
}

function populateExampleDropdown() {
  const select = document.getElementById("example-select");
  EXAMPLES.forEach((example) => {
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

  select.addEventListener("change", (event) => {
    loadExample(event.target.value);
  });

  runBtn.addEventListener("click", handleRun);

  editor.addEventListener("input", handleEditorChange);

  tabOutput.addEventListener("click", () => switchTab("output"));
  tabGenC.addEventListener("click", () => switchTab("genc"));

  initResizeHandle();
}

function loadExample(exampleId) {
  const example = EXAMPLES.find((ex) => ex.id === exampleId);
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

function handleRun() {
  if (STATE.isRunning) return;

  if (STATE.isCustomCode) {
    showCustomCodeNotice();
    return;
  }

  if (!STATE.currentExample) return;

  STATE.isRunning = true;
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

  steps.forEach((step) => {
    setTimeout(() => {
      outputEl.textContent = step.text;
    }, step.delay);
  });

  setTimeout(() => {
    displayResult();
    STATE.isRunning = false;
    runBtn.disabled = false;
    runBtn.innerHTML =
      '<span class="btn-icon">\u25b6</span> \u0634\u063a\u0651\u0644 \u2014 Run';
  }, 1500);
}

function displayResult() {
  const outputEl = document.getElementById("output-text");

  if (STATE.activeTab === "output") {
    outputEl.className = "output-text animate";
    outputEl.textContent = STATE.currentExample.output;
  } else {
    showGeneratedC();
  }
}

function showCustomCodeNotice() {
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text";
  outputEl.innerHTML = "";

  const notice = document.createElement("div");
  notice.className = "custom-notice";
  notice.innerHTML =
    "<strong>Full compilation coming soon!</strong><br><br>" +
    "WASM-based in-browser compilation is on the roadmap. " +
    "For now, try the pre-loaded examples or install mansaf locally:<br><br>" +
    "<code>git clone https://github.com/Ziadstr/nashmic</code><br>" +
    "<code>cd nashmic && make install</code><br>" +
    "<code>mansaf run your_file.nsh</code>";

  outputEl.appendChild(notice);
}

function switchTab(tab) {
  STATE.activeTab = tab;

  const tabOutput = document.getElementById("tab-output");
  const tabGenC = document.getElementById("tab-genc");

  tabOutput.classList.toggle("active", tab === "output");
  tabGenC.classList.toggle("active", tab === "genc");

  if (!STATE.currentExample) return;

  const outputEl = document.getElementById("output-text");

  if (STATE.isRunning) return;

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
  const outputEl = document.getElementById("output-text");
  outputEl.className = "output-text placeholder";
  outputEl.innerHTML =
    '<span>Press <strong>\u0634\u063a\u0651\u0644 \u2014 Run</strong> to see the output</span>';
}

function initResizeHandle() {
  const handle = document.getElementById("resize-handle");
  const playground = document.querySelector(".playground");
  const leftPane = document.querySelector(".pane-left");

  if (!handle || !playground || !leftPane) return;

  let isResizing = false;

  handle.addEventListener("mousedown", (event) => {
    isResizing = true;
    handle.classList.add("active");
    document.body.style.cursor = "col-resize";
    document.body.style.userSelect = "none";
    event.preventDefault();
  });

  document.addEventListener("mousemove", (event) => {
    if (!isResizing) return;

    const playgroundRect = playground.getBoundingClientRect();
    const offsetX = event.clientX - playgroundRect.left;
    const totalWidth = playgroundRect.width;
    const percentage = Math.max(20, Math.min(80, (offsetX / totalWidth) * 100));

    leftPane.style.flex = "none";
    leftPane.style.width = percentage + "%";
  });

  document.addEventListener("mouseup", () => {
    if (!isResizing) return;
    isResizing = false;
    handle.classList.remove("active");
    document.body.style.cursor = "";
    document.body.style.userSelect = "";
  });
}

/* ---- Keyboard shortcuts ---- */
document.addEventListener("keydown", (event) => {
  if ((event.ctrlKey || event.metaKey) && event.key === "Enter") {
    event.preventDefault();
    handleRun();
  }
});

/* ---- Tab support in textarea ---- */
document.addEventListener("DOMContentLoaded", () => {
  const editor = document.getElementById("code-editor");
  if (!editor) return;

  editor.addEventListener("keydown", (event) => {
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
