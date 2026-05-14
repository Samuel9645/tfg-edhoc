# Plan: Extract Common CoAP Data + Encapsulate Validators

**Created:** 2026-03-31

## Objective

Refactor the client-side CoAP model by extracting shared transport/endpoint data (context, session, uri, destination) into a common struct that both `coap_client_exchange_t` and `coap_client_exchange_session_data_t` embed. Remove wrapper-only structs. Project-wide encapsulate repeated struct validation checks into readable inline validators to eliminate verbose compound conditions.

## Motivation

- **Wrapper-only structs**: `coap_session_data_t` and `coap_endpoint_data_t` exist only to be re-wrapped by client exchange input, adding unnecessary indirection (nested field access like `session_data->session_data.context`).
- **Code duplication**: Client exchange state and init input both store the same 4 fields (context, session, uri, destination), violating DRY principle.
- **Readability**: Repeated inline condition checks (e.g., `!exchange || !common_response_buffer_is_writable(response_data)` vs. `!exchange->have_response || exchange->incoming_message_len == 0 || exchange->incoming_message_len > response_data->payload_capacity`) make code harder to maintain.

## Structure Transformation

### Before

```c
coap_client_exchange_t {
  coap_context_t* context;    // redundant with session_data
  coap_session_t* session;    // redundant with session_data  
  coap_uri_t uri;             // redundant with endpoint_data
  coap_address_t destination; // redundant with endpoint_data
  // ... state fields
}

coap_client_exchange_session_data_t {
  coap_session_data_t session_data;       // wraps context, session
  coap_endpoint_data_t endpoint_data;     // wraps uri, destination
}
```

### After

```c
coap_client_exchange_common_data_t {
  coap_context_t* context;
  coap_session_t* session;
  coap_uri_t uri;
  coap_address_t destination;
}

coap_client_exchange_t {
  coap_client_exchange_common_data_t common;
  bool have_response;
  uint8_t incoming_message[MAX_PDU_SIZE];
  size_t incoming_message_len;
  coap_pdu_code_t last_response_code;
}

coap_client_exchange_session_data_t {
  coap_client_exchange_common_data_t common;  // pure init input
}
```

## Implementation Phases

### Phase 1: Extract & Reshape Client Exchange Model

1. Create `coap_client_exchange_common_data_t` in `include/coap/client/exchange.h` containing transport/endpoint data.
2. Reshape `coap_client_exchange_t` to embed `common` + runtime state fields.
3. Reshape `coap_client_exchange_session_data_t` to embed only `common`.
4. Update `src/coap/client/exchange.c:coap_client_exchange_init()` to validate and read fields from `session_data->common`.
5. Update `src/core/client.c` struct initialization to use new layout.
6. Search and fix any remaining stale nested access patterns.

### Phase 2: Remove Useless Wrapper-Only Structs

1. Delete `coap_session_data_t` and `coap_endpoint_data_t` from `include/coap/common/data_models.h`.
2. Inline or remove validators dependent only on removed wrappers.

### Phase 3: Encapsulate Repeated Validators (Project-Wide)

Create public validators to eliminate compound inline condition checks:

1. **coap_client_exchange_response_size_fits(size_t msg_len, size_t capacity)**
   - Replaces: `exchange.c:63` and `exchange.c:153` buffer overflow checks
   - Location: `include/coap/client/exchange.h`

2. **coap_response_indicates_success(coap_pdu_code_t code)**
   - Replaces: Repeated checks against `COAP_RESPONSE_CODE_CHANGED`
   - Location: `include/coap/common/tst_srv_handshake_helpers.h`

3. **edhoc_server_common_request_is_valid(edhoc_server_common_request_data_t*)**
   - Extract from: `src/edhoc/server/handshake.c:42-44` (currently private)
   - Destination: `include/edhoc/server/message_3_handler.h`

4. **common_response_buffer_is_valid(common_response_buffer_t*)**
   - Complements existing `is_writable()`, checks `payload != NULL`
   - Location: `include/common/data_models.h`

5. **edhoc_client_process_message_args_valid(handshake*, payload*, len*)**
   - Consolidates: `src/edhoc/client/handshake.c:76` and `128` (identical guards)
   - Location: `include/edhoc/client/message_3_handler.h`

6. **edhoc_error_context_is_ready(edhoc_ctx*, response_data*)**
   - Replaces: `src/coap/server/map_error_to_response.c:16` pre-condition
   - Location: `include/edhoc/server/`

Replace all inline compound conditions throughout codebase with validator calls.

### Phase 4: Verification

1. Rebuild all targets: `cmake --build build`
2. Run tests: `ctest --test-dir build --output-on-failure`
3. Grep for stale patterns: `session_data.session_data`, `endpoint_data.uri`, etc.
4. Confirm no unresolved references to removed wrapper types.

## Files Modified

**Headers:**

- `include/coap/client/exchange.h` (new common struct, both client/session data)
- `include/coap/common/data_models.h` (remove wrappers, add validators)
- `include/coap/common/tst_srv_handshake_helpers.h` (new validator)
- `include/edhoc/server/message_3_handler.h` (extract validator)
- `include/edhoc/client/message_3_handler.h` (new validator)
- `include/edhoc/server/common.h` or similar (new error context validator)
- `include/common/data_models.h` (new buffer validator)

**Sources:**

- `src/coap/client/exchange.c` (update init, use validators)
- `src/core/client.c` (update initialization)
- `src/coap/common/helpers.c` (use new validators)
- `src/edhoc/server/handshake.c` (use extracted validator)
- `src/edhoc/client/handshake.c` (use new validator)
- `src/coap/server/map_error_to_response.c` (use new validator)
- `tests/src/core/test_integration.c` (if struct instantiation present)

## Validation Constraints

- **Runtime behavior**: Client exchange send/wait/reset behavior unchanged.
- **Field access**: No change to public API signatures.
- **Memory layout**: Net reduction in indirection depth (fewer nested pointer chases).

## Scope Decisions

**Included:**

- Extract common transport/endpoint data into shared substruct
- Both `coap_client_exchange_t` and `coap_client_exchange_session_data_t` embed the common struct
- Encapsulate repeated validation patterns (audit found 6 candidates with >1 occurrence across multiple layers)

**Excluded:**

- Protocol behavior changes
- Unrelated server/EDHOC internals
- Single-use validators (low ROI)
