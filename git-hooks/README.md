# Git Hooks

This directory contains git hook scripts for the Fusain project.

## Available Hooks

### pre-commit

Runs comprehensive checks before allowing a commit:

1. **Code Formatting Check** - Ensures all C files are properly formatted with clang-format
2. **CI Test Suite** - Runs full test suite with 1,000,000 fuzz rounds

**Installation:**
```bash
task install-git-hooks
```

**Usage:**
- The hook runs automatically on `git commit`
- To bypass (use sparingly): `git commit --no-verify`
- To uninstall: `rm .git/hooks/pre-commit`

**Note:** The CI test suite with 1M rounds will take several minutes. This ensures comprehensive testing but may slow down your commit workflow. Consider using `--no-verify` for work-in-progress commits and running the full hook before pushing.

## Customization

To modify the hooks:
1. Edit the hook file in this directory
2. Reinstall with `task install-git-hooks`

The hooks are version-controlled, so changes will be shared across the team.
