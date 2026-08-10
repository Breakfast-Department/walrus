# Contributing to Walrus

Thank you for your interest in contributing to Walrus.

This document explains how to set up the project, follow its development conventions, and submit changes.

## Development Setup

Clone the repository:

```bash
git clone https://github.com/ezravln/walrus.git
cd walrus
```

Configure and build:

```bash
cmake -B build
cmake --build build
```

Run the example:

```bash
./build/walrus-example
```

## Development Guidelines

### Code Style

* Use C99.
* Keep functions small and focused.
* Use descriptive names for functions and variables.
* Follow the existing naming conventions.
* Public APIs use the `wr_` prefix and lowercase `snake_case`.

### Architecture

Walrus separates platform and rendering concerns.

When adding or modifying functionality:

* Keep display-server-specific code in `src/backend/`.
* Keep graphics API-specific code in `src/renderer/`.
* Keep platform-independent logic in `src/core/`.
* Expose only necessary functionality through public headers.
* Avoid leaking backend or renderer implementation details into the public API.

### File Organization

```text
include/walrus/    Public headers
src/core/          Platform-independent code
src/backend/       Display server backends
src/renderer/      Renderer implementations
examples/          Example applications
docs/              Architecture and design documentation
```

For more information about the architecture, see [`docs/architecture.md`](docs/architecture.md).

## Making Changes

Before starting significant work:

1. Check existing issues and pull requests.
2. For major features or architectural changes, open an issue to discuss the approach first.

Create a feature branch:

```bash
git checkout -b feature/your-feature-name
```

Keep changes focused and avoid mixing unrelated changes in the same pull request.

## Commit Messages

Use short, descriptive commit messages.

Examples:

```text
feat: add window resize support
fix: correct EGL surface cleanup
docs: update architecture documentation
refactor: simplify backend initialization
```

Common prefixes include:

* `feat:` — New functionality
* `fix:` — Bug fixes
* `docs:` — Documentation changes
* `refactor:` — Code changes without behavior changes
* `test:` — Tests
* `build:` — Build system changes
* `chore:` — Maintenance

## Testing

Before submitting changes, make sure the project builds successfully:

```bash
cmake --build build
```

Run the example application:

```bash
./build/walrus-example
```

Check that:

* The project builds without warnings.
* Existing functionality still works.
* New functionality works as expected.
* Backend-specific changes are tested on the relevant backend when possible.

## Pull Requests

When opening a pull request:

1. Use a clear and descriptive title.
2. Explain what changed and why.
3. Link related issues when applicable.
4. Keep the pull request focused on a single purpose.
5. Include relevant testing information.

If your change affects the public API or architecture, explain the reasoning in the pull request description.

## Questions

If you have questions or are unsure about an implementation, open an issue before making a significant architectural change.

Thank you for contributing to Walrus!
