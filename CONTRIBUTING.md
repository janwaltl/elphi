
# Code style
Standard C++20 is used, targeting both g++ and clang++ compilers.

Code not respecting clang-format, clang-tidy configs will NOT be merged. 

Feel free to raise an issue if you encounter any false-positives.

Set your IDE to format on save.

`clangd` language server uses `.clang-tidy` to show the correct diagnostics 
on the fly. It is supported in VSCode(`vscode-clangd`), neovim(`nvim-lsp`) 
and other IDEs might support some sort of clang-tidy integration too.

# Commit conventions
- Keep commits small, atomic, focused on one thing only.
- Write descriptive commit messages.
   - Using Angular style: `type(topic): Message`
     - `feat` for introducing a new feature.
	 - `fix` for bug fixing.
	 - `docs` for comments, documentation.
	 - `style` code formatting, reordering functions...
	 - `refactor` improving existing code, use if you have the need to write `feat: Improve`.
	 - `test` anything to do with writing tests.
	 - `ci` Github actions.
	 - `build` Build system, CMake, scripts...
	 - `chore` for anything else.

# Branches & Pull requests

Semi-linear history is [manually](https://github.com/github-community/community/discussions/8940) enforced.

Branch your pull requests from the `main` branch. Please use `feat/` prefix or other 
Angular-style prefixes mentioned above. 

All pull requests must rebase on the latest main before they can be merged. 
Branches are always merged, never forwarded or rebased onto main.

No squashing is done to preserve individual commits so please keep your branch clean,
use `git rebase -i` to tidy it up if needed.

