Contributing Guidelines

Thank you for your interest in contributing to libpvars, a C library API for creating variables in C such as lists and dictionaries.

How to Contribute:


Commit Message Guidelines
Clear, conventional commit messages that help document the project history.
feat:
	A new feature for the user. Eg: "feat: add plist_insert function"
fix:
 	A bug fix (user-facing). Eg: "fix: correct memory leak in pdict_destroy"
docs:
	Documentation only changes. Eg: "docs: update README with API reference"
style:
	Code style changes (formatting, missing semicolons, etc.). Eg: "style: reformat pdict.c function headers"
refactor:
	Code changes that neither fix a bug nor add a feature. Eg: "refactor: move internal helpers to pvars_helpers.c"
test:
	Adding missing tests or correcting existing tests. Eg: "test: add edge cases for plist_remove"


Coding Style and Principles
Since this is a C library, consistency is crucial for maintainability.
Standard: All code should adhere to the C11 standard (as defined in the Makefile).
Indentation: Use tabs for indentation equal to the 8 spaces. Do not mix tabs and spaces.
Brace Style: For functions, opening brace on a new line, closing brace on its own line. For everything else, opening brace on the same line, closing brace on its own line. else if, and else statements follow the same line as the closing brace

void my_function(int arg)
{
	if (arg > 0) {
		// tab-indented code
	}
}

if (result == 0) {
	// tab-indented code
} else if (result > 0) {
	// tab-indented code
} else {
	// tab-indented code
}

Naming: Functions and variables must be snake case - example_variable_name. Functions must be prefixed with the variable they act upon. Eg pdict_add_str - The dict is being acted upon.
Use double quotes for project headers (#include "pvars.h") and angle brackets for system headers (#include <stdlib.h>).
Error Handling: All public functions must clear pvars_errno at the start and set an appropriate error code (from perrno.h) upon failure.


Testing
Before submitting a Pull Request, you must ensure the library builds correctly and all tests pass.
Supported Environments (Test Matrix)
Contributions will be tested across the following core Operating Systems and Architectures to ensure portability and stability.

OS / Distribution 	| Architecture		| Compiler		| Notes

Linux (Ubuntu/Mint)	| x86_64		| GCC 9+ (C11)		| Primary development target.
Linux (Debian/Fedora)	| x86_64		| GCC 9+ (C11)		| Standard compatibility test.
macOS (Latest)		| arm64 (Apple Silicon)	| Clang (C11)		| Tested via GitHub Actions.
macOS (Latest)		| x86_64		| Clang (C11)		| Standard compatibility test.
Windows			| x86_64		| MinGW/TDM-GCC		| Expected to compile via MinGW
