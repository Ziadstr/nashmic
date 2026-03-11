const vscode = require('vscode');
const { createFormattingProvider } = require('./formatter');

function activate(context) {
  const formatterDisposable = createFormattingProvider();
  context.subscriptions.push(formatterDisposable);

  const runCommand = vscode.commands.registerCommand(
    'nashmic.runFile',
    () => executeInTerminal('run')
  );
  context.subscriptions.push(runCommand);

  const buildCommand = vscode.commands.registerCommand(
    'nashmic.buildFile',
    () => executeInTerminal('build')
  );
  context.subscriptions.push(buildCommand);
}

function executeInTerminal(action) {
  const editor = vscode.window.activeTextEditor;

  if (!editor) {
    vscode.window.showWarningMessage('No active file to ' + action + '.');
    return;
  }

  const filePath = editor.document.fileName;

  if (!filePath.endsWith('.nsh')) {
    vscode.window.showWarningMessage(
      'Active file is not a NashmiC (.nsh) file.'
    );
    return;
  }

  editor.document.save().then(() => {
    const terminal = getOrCreateTerminal();
    terminal.show();
    terminal.sendText(`mansaf ${action} "${filePath}"`);
  });
}

let nashmicTerminal = null;

function getOrCreateTerminal() {
  if (nashmicTerminal && !nashmicTerminal.exitStatus) {
    return nashmicTerminal;
  }

  nashmicTerminal = vscode.window.createTerminal('NashmiC');

  vscode.window.onDidCloseTerminal((closedTerminal) => {
    if (closedTerminal === nashmicTerminal) {
      nashmicTerminal = null;
    }
  });

  return nashmicTerminal;
}

function deactivate() {
  nashmicTerminal = null;
}

module.exports = { activate, deactivate };
