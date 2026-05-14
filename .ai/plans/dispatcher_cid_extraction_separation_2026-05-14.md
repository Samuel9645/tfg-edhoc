# Dispatcher CID Extraction Separation — Plan 2026-05-14

## Objective

Refactor the CoAP dispatcher to separate connection ID (CID) extraction and validation into two independent operations, removing the monolithic `srv_coap_extract_message_3` function.

## Changes

### 1. New CID Extraction API

**New Types:**
- `enum srv_coap_extract_cid_status`: Status codes for CID extraction (OK, ERR_EMPTY_BUFFER, ERR_EXTRACT)
- `struct srv_coap_extract_cid_result`: Contains extracted CID + message payload

**New Functions:**
- `srv_coap_extract_cid()`: Extract CID from Message 3 payload
- `srv_coap_connection_id_is_expected()`: Validate extracted CID against context

**Result Builders (Internal):**
- `srv_coap_extract_cid_ok()`: Build success result
- `srv_coap_extract_cid_failure()`: Build failure result

### 2. Dispatcher Dependencies

**Updated struct:**
- Replaced `extract_message_3` callback with `extract_cid` + `connection_id_is_expected`
- Both new operations return errors as `COAP_RESPONSE_CODE_INTERNAL_ERROR` on failure

**Dispatcher Flow:**
1. Extract CID (returns `SRV_COAP_EXTRACT_CID_ERR_*` on failure → Internal Error)
2. Validate CID (returns false on mismatch → Internal Error)
3. Use message payload for EDHOC processing

### 3. Test Refactoring

- Removed `srv_coap_extract_message_3` from public API
- Updated test suite to use new CID extraction/validation functions directly
- Dispatcher tests verify both failure paths return `COAP_RESPONSE_CODE_INTERNAL_ERROR`

## Files Modified

- `include/coap/server/extract_edhoc_message/srv_coap_extract_m3.h`: New API, deprecated internals
- `src/coap/server/extract_edhoc_message/srv_coap_extract_m3.c`: Implementation
- `src/coap/server/extract_edhoc_message/internal/srv_coap_extract_cid_result_builders.{h,c}`: Builders
- `src/coap/server/internal/srv_dispatch_engine.{h,c}`: Dispatcher deps update
- `src/coap/server/srv_dispatch.c`: Production deps binding
- `tests/src/coap/server/dispatch/tst_srv_dispatch_stubs.{h,c}`: Stubs for new deps
- `tests/src/coap/server/dispatch/tst_create_base_dependencies.c`: Base deps update
- `tests/src/coap/server/dispatch/test_coap_server_dispatch.c`: Split M3 failure tests
- `tests/src/coap/server/extract_edhoc_message/test_coap_extract_message_3.c`: Refactored for new API
- `CMakeLists.txt`: Registered CID result builders

## Test Results

✅ `test_coap_extract_message_3`: 6/6 PASSED
✅ `test_coap_server_dispatch`: 12/12 PASSED
✅ Server binary builds successfully

## Benefits

- **Separation of Concerns**: CID extraction and validation are now independent
- **Symmetry**: Matches dispatcher's extraction/validation pattern for Message 1
- **Error Isolation**: Clear error codes for each operation (empty buffer, extraction, validation)
- **Testability**: Each operation can be tested in isolation

