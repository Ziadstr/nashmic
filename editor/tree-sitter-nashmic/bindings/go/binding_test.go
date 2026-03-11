package tree_sitter_nashmic_test

import (
	"testing"

	tree_sitter "github.com/smacker/go-tree-sitter"
	"github.com/tree-sitter/tree-sitter-nashmic"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_nashmic.Language())
	if language == nil {
		t.Errorf("Error loading Nashmic grammar")
	}
}
