# Client Handshake Split With Error Reporting (2026-04-04)

## Goal

Refactor client EDHOC handshake to mirror server folder-per-step architecture, with per-step result enums and per-message error composition modules. Ensure the client reports EDHOC error payloads after session establishment when Message 2 processing, Message 3 composition, or Message 4 processing fails.

## Structural Changes

- Split client handshake into modular folders under include/src:
- common
- init
- message_1
- message_2
- message_3
- message_4
- Replace monolithic status enum with step-scoped result enums.
- Keep lifecycle state/deinit in common module.
- Keep facade API in include/edhoc/client/handshake.h and src/edhoc/client/handshake.c delegating to step handlers.

## Naming Contract

### Struct and Type Convention

- Struct style matches server typedef convention:
  - `typedef struct edh_cli_handshake { ... } edh_cli_handshake_t;`
- Step result enums and status constants aligned with operations:
  - `EDH_CLI_INIT_OK/ERR_*`
  - `EDH_CLI_MSG1_COMPOSE_OK/ERR_*`
  - `EDH_CLI_MSG2_PROCESS_OK/ERR_*`
  - `EDH_CLI_MSG3_COMPOSE_OK/ERR_*`
  - `EDH_CLI_MSG4_PROCESS_OK/ERR_*`

### API Parameter Naming

- `state`: Handshake state parameter (replaces generic "handshake")
- `message_X`: Input payload descriptor for process operations (e.g., `message_2`, `message_4`)
- `message_X`: Output buffer descriptor for compose operations (e.g., `message_1`, `message_3`)
- `message_X_error`: Output buffer descriptor explicitly for error payloads in process operations (e.g., `message_2_error`, `message_4_error`)
- All buffers documented as `@param[out]` (write-only) not `@param[in,out]`

### Internal Helper Naming

- Result creation helpers: `edh_cli_message_X_create_result(...)` (create_object pattern)

## Error Composition Modules

- Added client message-local errors APIs:
- message_2/errors.h + errors.c
- message_3/errors.h + errors.c
- message_4/errors.h + errors.c
- Handler-level APIs:
- edh_cli_message_2_handler_add_error(...)
- edh_cli_message_3_handler_add_error(...)
- edh_cli_message_4_handler_add_error(...)

## Failure Reporting Flow

For Message 2 process / Message 3 compose / Message 4 process failure:

1. Compose EDHOC error payload via message-local errors API into request buffer.
2. Set outgoing request length to composed error payload size.
3. Reset exchange once.
4. Send EDHOC error payload as CoAP request.
5. Attempt one receive call for closure/logging.
6. Cleanup and return failure regardless of receive outcome.

## Validation Strategy

Rather than maintaining custom step-local validators, leverage shared data-model validators from `common/data_models.h`:

- `com_request_payload_is_valid(const com_request_payload_t* payload)` — validates input descriptors
- `com_response_buffer_is_writable(const com_response_buffer_t* buffer)` — validates writable output buffers
- Removed redundant custom helpers:
  - `include/edhoc/client/handshake/common/validation.h`
  - `src/edhoc/client/handshake/common/validation.c`
- This consolidation clarifies intent: struct-based input/output validation is generic, not step-specific.

## Build Integration

- Updated client source list in CMake to include all new client handshake module files.
- Removed validation.c from client-specific sources (no longer needed).
- Linked shared EDHOC error helper implementation used by client error modules.

## Validation

- CMake build: success.
- Integration test: test_integration passed.
