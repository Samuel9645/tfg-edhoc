# AGENTS — Concise Rules

Keep it minimal.

## Workflow & Artifact Locations

**Use prompts** in `.github/prompts/`:

- `discovery.prompt.md` → saves to `.ai/workspace/prds/<topic>.md`
- `plan.prompt.md` → saves to `.ai/workspace/plans/<topic>.md`
- `execute.prompt.md` → saves to `.ai/workspace/summaries/<topic>.md`
- `review.prompt.md` → reads from `.ai/workspace/summaries/<topic>.md`

**Sequence**:

1. Discover (PRD) → Plan (checklist) → Execute (implement) → Review (validate) → Document (`.ai/docs/`)

**After implementing** a feature, add the documentation into `.ai/docs/`.

**After refactoring**, update the documentation in `.ai/docs/` and reflect the changes.

Use TDD cycle for new features:

1. write test, commit,
2. implement minimal code to pass it, commit if tests pass
3. refactor run test, commit if tests pass

Test commit: tests(scope-description): add test for specific behavior

Implementation commit: feat(scope-description): implement specific behavior

Refactor commit: refactor(scope-description): <description of the refactor>

Rules

- Vendored dependencies live in `externals/` (`libcoap`, `libedhoc`,
  `unity`); wire new code through CMake instead of adding ad-hoc downloads.
- Prefer `*_result` structs (status + small payload).
- For large objects initialization functions (e.g. `struct edhoc_context`)
  return pointer and document ownership: caller must call
  `edhoc_context_deinit()` then `free()`.
- For large objects in other cases use [out] parameters.
- No `typedef` or macros for public `struct`/`enum`.
- Use clear prefixes (`com_`, `srv_`, `cli_`, `edhoc_`) and avoid abbreviations:
  prefer descriptive, semantic names (e.g. `com_edhoc_setup_context_result` not
  `ces_ctx_res`).
- App targets and tests build with `-Wall -Wextra -Wpedantic -Werror` plus
  AddressSanitizer/UndefinedBehaviorSanitizer; keep new code warning-free and
  sanitizer-clean.

WSL

- Use POSIX paths in code/CMake: `/home/jhon/.../libedhoc_mio`.
- From Windows tools use UNC: `\\wsl.localhost\\Ubuntu-24.04\\home\\jhon\\...`.
- Prefer `${CMAKE_SOURCE_DIR}` and relative paths.

Layers

- Core app entry points: `src/core/`, `include/app/` (`core_run_client()` and
  `core_run_server()`).
- EDHOC: `src/edhoc/`, `include/edhoc/` (protocol logic only).
- Transport/CoAP: `src/coap/`, `include/coap/` (wire parsing/serialization).
- Common: shared models/helpers.
- Client/Server: orchestration and lifecycle.

RFC & errors

- Follow RFC 9528 for EDHOC error mapping; map cases to enums (e.g.
  `COM_EDHOC_SETUP_CTX_ERR_*`).

Tests

- Unity test runners are generated from `tests/CMakeLists.txt` with
  `ruby externals/unity/auto/generate_test_runner.rb`.
- `ENABLE_UNITY_TESTS` defaults to `ON` in `cmake_configs/testing_config.cmake`;
  configure a build directory, then run `ctest --test-dir <build-dir>`.
- Test binaries already inherit `ASAN_OPTIONS=halt_on_error=1:abort_on_error=1`
  and `UBSAN_OPTIONS=halt_on_error=1:abort_on_error=1:print_stacktrace=1`.
- Tests use `--wrap` mocks; helpers should return `*_result` where possible.

Tiny examples

```c
// small result (use descriptive names)
enum operation_status { OPERATION_OK = 0, OPERATION_ERROR };
struct operation_result {
  const enum operation_status status;
  const struct com_readonly_buffer buffer; // small payload
};

// large object result with explicit ownership
struct edhoc_process_result {
  const enum operation_status status;
  struct edhoc_context* edhoc_context; // ownership transferred on success
};
// on success: caller -> edhoc_context_deinit(edhoc_context); free(edhoc_context);
```

Refs: `include/edhoc/common/com_edhoc_manage_context.h`,
`include/edhoc/server/handshake/message_1/srv_m1_process_result.h`,
`src/coap/common/internal/com_parse_edhoc_request_builders.c`.

## WSL tools troubleshooting

- Troubleshooting `apply_patch`:
  - `apply_patch` can fail with "File not found" even if the path worked
      earlier. This often comes from using a different path format or improper
      escaping of backslashes in the tool call.
  - If `apply_patch` fails, try the UNC WSL path (
      `\\wsl.localhost\\Ubuntu-24.04\\home\\jhon\\cuarto\\tfg\\libedhoc_mio\\AGENTS.md`)
      and ensure backslashes are escaped where required by the tool schema.

- Preparing to use `insert_edit_into_file`:
  - `insert_edit_into_file` requires three fields: `filePath` (absolute),
      `code` (the new file content as a string), and `explanation`.
  - This tool is suited for full-content replacements (e.g., `AGENTS.md`).
      When switching to it, prepare the replacement content and a short
      explanation of the edit.
  - Use a UNC path for the `filePath` and keep the `code` minimal to save
      tokens.

- Quick checklist when a patch fails:
    1. Verify the absolute path format (use UNC for WSL).
    2. Check escaping of backslashes in the tool call.
    3. If `apply_patch` still fails, use `insert_edit_into_file` supplying the
       absolute path, the replacement `code` and an `explanation`.

Keep these notes short — they are for tool-use troubleshooting only.
