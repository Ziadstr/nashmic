const vscode = require('vscode');
const { execFile } = require('child_process');
const path = require('path');

const MANSAF_NOT_FOUND_MESSAGE =
  'mansaf not found. Install NashmiC from https://github.com/Ziadstr/nashmic to enable formatting.';

function createFormattingProvider() {
  return vscode.languages.registerDocumentFormattingEditProvider('nashmic', {
    provideDocumentFormattingEdits(document) {
      return new Promise((resolve) => {
        const filePath = document.fileName;

        execFile('mansaf', ['fmt', '--write', filePath], (error) => {
          if (error) {
            if (isCommandNotFound(error)) {
              vscode.window.showInformationMessage(MANSAF_NOT_FOUND_MESSAGE);
              resolve([]);
              return;
            }

            vscode.window.showErrorMessage(
              `NashmiC formatter failed: ${error.message}`
            );
            resolve([]);
            return;
          }

          const fullRange = new vscode.Range(
            document.positionAt(0),
            document.positionAt(document.getText().length)
          );

          const { readFileSync } = require('fs');
          const updatedContent = readFileSync(filePath, 'utf-8');

          resolve([vscode.TextEdit.replace(fullRange, updatedContent)]);
        });
      });
    },
  });
}

function isCommandNotFound(error) {
  return (
    error.code === 'ENOENT' ||
    (error.message && error.message.includes('not found'))
  );
}

module.exports = { createFormattingProvider };
