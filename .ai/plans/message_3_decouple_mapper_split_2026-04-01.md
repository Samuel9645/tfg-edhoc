# Message 3 Decoupling and Mapper Split Plan (2026-04-01)

## Objective

Refactor EDHOC Message 3 handling to match the Message 1 architecture:

- EDHOC layer returns a typed result (no CoAP coupling).
- CoAP adapter layer maps/logs that result.
- Mapper code is split by message into dedicated files under `edhoc_mapper/`.

## Scope

1. Add Message 3 result model (`status enum result type`).
2. Refactor Message 3 handler to return typed result and remove legacy mapper
   usage.
3. Split mapper logic into:
    - `message_1_mapper`
    - `message_3_mapper`
4. Remove legacy generic mapper API and umbrella header.
5. Update dispatcher dependencies to process Message 3 result like Message 1.
6. Update CMake and tests/stubs for new interfaces.

## Acceptance Criteria

1. `edhoc_server_handle_message_3` no longer depends on CoAP mapper APIs.
2. No references remain to `coap_server_map_edhoc_failure_to_response`.
3. Message 1 and Message 3 mapper logic live in separate files under
   `coap/server/edhoc_mapper/`.
4. Dispatcher uses `handle_message_3` + `process_message_3_result` pattern.
5. Build succeeds and dispatch tests still validate external response behavior.

## Implementation Notes (Final)

1. Message 3 result type was simplified from wrapper struct to enum-only type:

- `edhoc_server_message_3_result_t` is now the enum itself.
- Helper constructors were removed.
- `src/edhoc/server/handshake/message_3_result.c` was deleted.

2. Message 1 status constants were renamed from `CSM1_*` to `ESHM1_*`.
2. Message 3 status constants were renamed from `CSM3_*` to `ESHM3_*`.
3. Failure logging in mappers was refactored:

- Helper functions now return an error string.
- Logging is centralized in `coap_server_process_message_1_result` and
  `coap_server_process_message_3_result`.

5. Relative include paths were normalized to project include style
   (for example `edhoc/server/handshake/message_3_handler.h`).
2. Top-level file comments and `@since` tags were normalized:

- Date format is `dd/mm/yyyy`.
- Existing dates were preserved when present and only reformatted.

## Target Files

- `include/edhoc/server/handshake/message_3_result.h` (new)
- `include/edhoc/server/handshake/message_3_handler.h`
- `src/edhoc/server/message_3_handler.c`
- `include/coap/server/edhoc_mapper/srv_m1_mapper.h` (new)
- `include/coap/server/edhoc_mapper/srv_m3_mapper.h` (new)
- `src/coap/server/edhoc_mapper/message_1_mapper.c` (new)
- `src/coap/server/edhoc_mapper/message_3_mapper.c` (new)
- `src/coap/server/internal/dispatch_engine.h`
- `src/coap/server/internal/srv_dispatch_engine.c`
- `src/coap/server/dispatch.c`
- `CMakeLists.txt`
- `tests/include/coap/server/dispatch/tst_m1_stubs.h`
- `tests/src/coap/server/dispatch/stubs.c`

## Removed Files

- `src/edhoc/server/handshake/message_3_result.c`

## Verification

- Search:
  -
  `rg "coap_server_map_edhoc_failure_to_response|COAP_SERVER_EDHOC_PROTOCOL_ERROR|COAP_SERVER_EDHOC_INTERNAL_ERROR"`
- Build:
    - `cmake -S . -B build`
    - `cmake --build build -j`
- Tests:
    - `ctest --test-dir build --output-on-failure`

## Status

- Plan executed and completed.
- Build and tests passing after refactor and naming/documentation updates.
