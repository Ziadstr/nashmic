# NashmiC Tree-sitter for Neovim

## Setup

Add to your `init.lua` or create `after/plugin/nashmic.lua`:

```lua
local parser_config = require("nvim-treesitter.parsers").get_parser_configs()
parser_config.nashmic = {
  install_info = {
    url = "https://github.com/Ziadstr/nashmic",
    files = { "src/parser.c" },
    subdirectory = "editor/tree-sitter-nashmic",
    branch = "main",
  },
  filetype = "nashmic",
}

vim.filetype.add({ extension = { nsh = "nashmic" } })
```

## Install

```vim
:TSInstall nashmic
```

## Highlighting

Copy the queries to your Neovim config:

```bash
mkdir -p ~/.config/nvim/queries/nashmic
cp editor/tree-sitter-nashmic/queries/*.scm ~/.config/nvim/queries/nashmic/
```

## Verify

Open a `.nsh` file and run:

```vim
:InspectTree
```

You should see the parsed AST with proper node types.
