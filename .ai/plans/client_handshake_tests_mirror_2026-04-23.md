# Client Handshake Tests Structure Mirror

**Date:** 23/04/2026  
**Status:** ✅ Completed

## Objective

Mirror the established test directory structure from the server handshake layer into the client handshake layer, maintaining naming conventions, mock patterns, and CMake integration to achieve consistency and discoverability across both sides.

## Rationale

The server sidepossesses a well-organized test hierarchyunder `tests/src/edhoc/server/handshake/` and `tests/include/edhoc/server/handshake/`, with clear separation of concerns:
- Integration of message handlers per operation
- Dedicated mock modules for external dependencies
- Unified CMake wiring with `--wrap` flags
- Reusable assertion and error-reporting utilities

Mirroring this structure on the client side ensures:
1. **Homogeneity:** Both client and server test suites follow identical patterns
2. **Maintainability:** New test cases and mocks fit naturally into expected locations
3. **Scalability:** Future message handlers or phases scale predictably
4. **Clarity:** Clear separation between operational layers (compose/process/initiator)

## Changes

### New Directory Structure

#### Headers (under `tests/include/edhoc/client/handshake/`)

```
mocks/
├── message_1/
│   └── tst_cli_mock_edhoc_message_1_compose.h
├── message_2/
│   └── tst_cli_mock_edhoc_message_2_process.h
├── message_3/
│   └── tst_cli_mock_edhoc_message_3_compose.h
└── message_4/
    └── tst_cli_mock_edhoc_message_4_process.h
```

#### Sources (under `tests/src/edhoc/client/handshake/`)

```
mocks/
├── message_1/
│   └── tst_cli_mock_edhoc_message_1_compose.c
├── message_2/
│   └── tst_cli_mock_edhoc_message_2_process.c
├── message_3/
│   └── tst_cli_mock_edhoc_message_3_compose.c
└── message_4/
    └── tst_cli_mock_edhoc_message_4_process.c

message_1/
└── test_edhoc_message_1_compose.c

message_2/
├── process/
│   └── test_edhoc_message_2_process.c
└── initiator/
    └── test_edhoc_message_2_initiator.c

message_3/
└── test_edhoc_message_3_compose.c

message_4/
└── process/
    └── test_edhoc_message_4_process.c
```

### Mock Implementations

Each mock follows the **hybrid real/stubbed dispatch pattern** established in the server mocks:

1. **message_1_compose mock**
   - Wraps: `edhoc_message_1_compose`, `edhoc_prepend_flow`, `edhoc_prepend_recalculate_size`
   - State toggles: `use_real_message_1_compose`, `prepend_flow_result`, `message_1_compose_result`, `recalculate_size_result`
   - Capabilities: Failure injection, empty-buffer injection, payload customization
   - Debug: Uses `tst_report_mock_error()` for null-pointer validation

2. **message_2_process mock**
   - Wraps: `edhoc_message_2_process`
   - State toggles: Result codes for parse failure, CID mismatch, success
   - Capabilities: Simulates responder message processing with payload extraction
   - Shared by: Both `process` and `initiator` test suites

3. **message_3_compose mock**
   - Wraps: `edhoc_message_3_compose`, `edhoc_prepend_flow`, `edhoc_prepend_recalculate_size`
   - State toggles: `use_real_message_3_compose`, composition result, payload injection
   - Capabilities: Empty-buffer injection, failure injection
   - Shared by: Both `message_3` and `initiator` test suites

4. **message_4_process mock**
   - Wraps: `edhoc_message_4_process`
   - State toggles: Result codes for parse failure, success
   - Capabilities: Simulates final message processing

### Test Cases

#### Message 1 Compose
- ✅ `test_compose_message_1_succeeds` – Real composition via `__real_edhoc_message_1_compose`
- ✅ `test_compose_message_1_fails_when_prepend_flow_fails` – Injection of prepend failure
- ✅ `test_compose_message_1_fails_when_composition_fails` – Injection of compose failure
- ✅ `test_compose_message_1_fails_when_recalculate_size_fails` – Injection of recalculate failure
- ✅ `test_compose_message_1_fails_when_composition_produces_empty_buffer` – Injection of zero-length payload

#### Message 2 Process
- ✅ `test_process_message_2_succeeds` – Real message 2 processing
- ✅ `test_process_message_2_fails_to_extract` – Injection of extraction failure
- ✅ `test_process_message_2_fails_on_cid_mismatch` – Injection of CID mismatch

#### Message 2 Initiator
- ✅ `test_initiator_message_2_succeeds_when_orchestration_complete` – Full orchestration with `message_2_process` → `message_3_compose`
- Reuses both `CLIENT_MESSAGE_2_PROCESS_MOCKS` and `CLIENT_MESSAGE_3_COMPOSE_MOCKS`

#### Message 3 Compose
- ✅ `test_compose_message_3_succeeds` – Real composition
- ✅ `test_compose_message_3_fails_when_composition_fails` – Injection of compose failure
- ✅ `test_compose_message_3_fails_when_composition_produces_empty_buffer` – Injection of zero-length payload

#### Message 4 Process
- ✅ `test_process_message_4_succeeds` – Real message 4 processing
- ✅ `test_process_message_4_fails_to_parse` – Injection of parse failure

### CMake Integration

Updated `tests/CMakeLists.txt`:

1. **New mock source variables**
   ```cmake
   set(CLIENT_MESSAGE_1_COMPOSE_MOCKS
       tests/src/edhoc/client/handshake/mocks/message_1/tst_cli_mock_edhoc_message_1_compose.c
   )
   set(CLIENT_MESSAGE_2_PROCESS_MOCKS
       tests/src/edhoc/client/handshake/mocks/message_2/tst_cli_mock_edhoc_message_2_process.c
   )
   set(CLIENT_MESSAGE_3_COMPOSE_MOCKS
       tests/src/edhoc/client/handshake/mocks/message_3/tst_cli_mock_edhoc_message_3_compose.c
   )
   set(CLIENT_MESSAGE_4_PROCESS_MOCKS
       tests/src/edhoc/client/handshake/mocks/message_4/tst_cli_mock_edhoc_message_4_process.c
   )
   ```

2. **New test targets**
   ```cmake
   add_unity_test(
       test_edhoc_message_1_compose
       tests/src/edhoc/client/handshake/message_1/test_edhoc_message_1_compose.c
       "${CLIENT_MESSAGE_1_COMPOSE_MOCKS}"
       "edhoc_message_1_compose;edhoc_prepend_flow;edhoc_prepend_recalculate_size"
   )
   
   add_unity_test(
       test_edhoc_message_2_process
       tests/src/edhoc/client/handshake/message_2/process/test_edhoc_message_2_process.c
       "${CLIENT_MESSAGE_2_PROCESS_MOCKS}"
       "edhoc_message_2_process"
   )
   
   add_unity_test(
       test_edhoc_message_2_initiator
       tests/src/edhoc/client/handshake/message_2/initiator/test_edhoc_message_2_initiator.c
       "${CLIENT_MESSAGE_2_PROCESS_MOCKS};${CLIENT_MESSAGE_3_COMPOSE_MOCKS}"
       "edhoc_message_2_process;edhoc_message_3_compose;edhoc_prepend_flow;edhoc_prepend_recalculate_size"
   )
   
   add_unity_test(
       test_edhoc_message_3_compose
       tests/src/edhoc/client/handshake/message_3/test_edhoc_message_3_compose.c
       "${CLIENT_MESSAGE_3_COMPOSE_MOCKS}"
       "edhoc_message_3_compose;edhoc_prepend_flow;edhoc_prepend_recalculate_size"
   )
   
   add_unity_test(
       test_edhoc_message_4_process
       tests/src/edhoc/client/handshake/message_4/process/test_edhoc_message_4_process.c
       "${CLIENT_MESSAGE_4_PROCESS_MOCKS}"
       "edhoc_message_4_process"
   )
   ```

3. **Linker wraps consolidation**
   - Added wraps for client-side EDHOC message functions
   - Reused existing `--wrap` infrastructure

### Metadata Standards

All new client files include:
- `@author Samuel Rodríguez <alu0101545714@ull.edu.es>`
- `@since 23/04/2026`
- Brief Doxygen `@file` and `@brief` descriptions
- Cross-references to libedhoc documentation and repository

## Verification

**Build Status:** ✅ All 5 new test targets compile and link successfully  
**Test Results:**  
```
test_edhoc_message_1_compose ✅
test_edhoc_message_2_process ✅
test_edhoc_message_2_initiator ✅
test_edhoc_message_3_compose ✅
test_edhoc_message_4_process ✅

Total: 5/5 PASSED
```

## Future Extensions

1. **Message 1 Parser Tests** – Add error scenarios for malformed input
2. **Message 2 Initiator Failures** – Extend with cross-message failure chains
3. **Message 3 Parser Tests** – Parallel to server message 3 parser suite
4. **Message 4 Responder Tests** – Client-side message 4 handling edge cases
5. **Integration Tests** – Full client handshake orchestration (M1 → M4)

## Files Modified/Created

### Headers
- ✅ `tests/include/edhoc/client/handshake/mocks/message_1/tst_cli_mock_edhoc_message_1_compose.h`
- ✅ `tests/include/edhoc/client/handshake/mocks/message_2/tst_cli_mock_edhoc_message_2_process.h`
- ✅ `tests/include/edhoc/client/handshake/mocks/message_3/tst_cli_mock_edhoc_message_3_compose.h`
- ✅ `tests/include/edhoc/client/handshake/mocks/message_4/tst_cli_mock_edhoc_message_4_process.h`

### Mock Sources
- ✅ `tests/src/edhoc/client/handshake/mocks/message_1/tst_cli_mock_edhoc_message_1_compose.c`
- ✅ `tests/src/edhoc/client/handshake/mocks/message_2/tst_cli_mock_edhoc_message_2_process.c`
- ✅ `tests/src/edhoc/client/handshake/mocks/message_3/tst_cli_mock_edhoc_message_3_compose.c`
- ✅ `tests/src/edhoc/client/handshake/mocks/message_4/tst_cli_mock_edhoc_message_4_process.c`

### Test Sources
- ✅ `tests/src/edhoc/client/handshake/message_1/test_edhoc_message_1_compose.c`
- ✅ `tests/src/edhoc/client/handshake/message_2/process/test_edhoc_message_2_process.c`
- ✅ `tests/src/edhoc/client/handshake/message_2/initiator/test_edhoc_message_2_initiator.c`
- ✅ `tests/src/edhoc/client/handshake/message_3/test_edhoc_message_3_compose.c`
- ✅ `tests/src/edhoc/client/handshake/message_4/process/test_edhoc_message_4_process.c`

### Build Configuration
- ✅ `tests/CMakeLists.txt` – Client test targets and mock integration

