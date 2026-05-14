# Decouple Parsers And Split Message 3 Tests

Goal: extract message 1 and message 3 parsing from the handlers, update the dispatcher to route through parser dependencies, and split the tests so parser coverage and handler coverage use descriptive names across the project.

1. Add message 1 parser files and move CBOR TRUE prefix validation plus stripping out of the handler.
2. Add message 3 parser files and move connection-ID extraction plus session-match validation out of the handler.
3. Update the dispatcher dependency contract and default wiring to use parser callbacks instead of message-format predicates.
4. Keep handlers focused on protocol state changes and response composition only.
5. Split the current message 1 test file into parser and handler test files.
6. Use descriptive non-technical test names such as `test_parser_fails_on_invalid_data` and `test_handler_fails_on_empty_payload`.
7. Add a message 3 handler test file that covers every status code with only the necessary libedhoc mocks.
8. Update `tests/CMakeLists.txt` with the new test targets and the smallest source list needed for each one.
9. Build and run the focused parser, handler, and dispatch tests after the refactor.

## Completion Status

### ✅ Message 1 Parser Tests (Complete)

**File:** `/tests/src/edhoc/server/handshake/message_1/parser/test_message_1_parser.c`

**Refactored Tests:**

- `test_parser_returns_stripped_message_1_payload()` - validates valid message 1 CBOR TRUE prefix stripping
- `test_parser_fails_on_invalid_data()` - refactored from 3 individual test calls into scenario-table pattern with 3 cases (NULL payload, zero length, NULL output)
- `test_parser_fails_when_prefix_is_missing()` - validates missing CBOR TRUE prefix detection

**Scenario Header:** `/tests/include/edhoc/server/handshake/message_1/parser/tst_m1_hdl_scenarios.h`

- Defined `message_1_parser_test_case_t` typedef with positional initialization (description, payload, length, output)
- Matches message 3 parser scenario pattern for consistency

### ✅ Message 3 Parser Tests (Complete)

**File:** `/tests/src/edhoc/server/handshake/message_3/parser/test_message_3_parser.c`

**Test Cases:**

- `test_parser_returns_extracted_fields_for_valid_message_3_data()` - validates field extraction and buffer population
- `test_parser_fails_on_invalid_data()` - scenario table with 4 cases (NULL payload, zero length, NULL context, NULL output)
- `test_parser_fails_when_connection_id_extraction_fails()` - validates extraction error handling
- `test_parser_fails_when_connection_id_does_not_match_context()` - validates CID mismatch detection

**Scenario Header:** `/tests/include/edhoc/server/handshake/message_3/parser/tst_m1_hdl_scenarios.h`

- Defined `message_3_parser_test_case_t` typedef

**Stubs:** `/tests/src/edhoc/server/handshake/message_3/parser/parser_stubs.c`

- Mock implementations of `edhoc_extract_connection_id()` and `edhoc_connection_id_equal()`
- Controllable return values for testing error paths

### ✅ Message 3 Handler Tests (Complete)

**File:** `/tests/src/edhoc/server/handshake/message_3/test_message_3_handler.c`

**Test Cases:**

- `test_handler_fails_on_invalid_data()` - scenario table with 7 cases covering NULL/empty/missing arguments
- Helper functions for building variant request objects
- Error payload validation

### ✅ Parser API Documentation (Complete)

**message_1 srv_m3_parser.h:** Added Doxygen comment explaining CBOR TRUE prefix stripping behavior
**message_3 srv_m3_parser.h:** Added Doxygen comment explaining field extraction and CID validation flow

### ✅ Test Structure Standardization (Complete)

- All parser and handler tests follow explicit setup→execute→assert flow with blank-line separation
- Scenario table pattern applied to all invalid-data test cases for consistency
- Concise positional initialization matches message 3 parser style
- CMake targets `test_message_1_parser`, `test_message_3_parser`, and `test_message_3_handler` all build and pass
