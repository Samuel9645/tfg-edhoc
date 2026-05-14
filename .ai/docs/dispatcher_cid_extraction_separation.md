# Dispatcher CID Extraction Separation — Documentation

## Overview

As of 2026-05-14, the CoAP dispatcher has been refactored to separate connection ID (CID) extraction and validation from the monolithic `srv_coap_extract_message_3` function.

## New Public API

### CID Extraction

```c
enum srv_coap_extract_cid_status {
  SRV_COAP_EXTRACT_CID_OK = 0,
  SRV_COAP_EXTRACT_CID_ERR_EMPTY_BUFFER,
  SRV_COAP_EXTRACT_CID_ERR_EXTRACT,  // Library error (e.g., CBOR failure)
};

struct srv_coap_extract_cid_result {
  const enum srv_coap_extract_cid_status status;
  const struct edhoc_connection_id cid;
  const struct com_readonly_buffer message_payload;  // M3 without CID prefix
};

// Extract CID and remaining message from Message 3 payload
struct srv_coap_extract_cid_result srv_coap_extract_cid(
    struct com_readonly_buffer request_buffer);
```

**Usage:**
```c
struct srv_coap_extract_cid_result result = srv_coap_extract_cid(request_buffer);
if (result.status != SRV_COAP_EXTRACT_CID_OK) {
  // Handle error: empty buffer or library failure
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

// result.cid contains the extracted connection ID
// result.message_payload contains Message 3 without CID prefix
```

### CID Validation

```c
// Check if extracted CID matches the EDHOC context
bool srv_coap_connection_id_is_expected(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx);
```

**Usage:**
```c
if (!srv_coap_connection_id_is_expected(&result.cid, edhoc_ctx)) {
  // CID mismatch
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}
```

## Dispatcher Integration

The dispatcher now follows this pattern for Message 3:

1. **Extract CID**: `deps->extract_cid(parsed_request)` → `SRV_COAP_EXTRACT_CID_OK` / error
2. **Validate CID**: `deps->connection_id_is_expected(&cid, context)` → true / false
3. **Process**: Use `result.message_payload` as the EDHOC message

**Error Handling:**
- CID extraction failure → `COAP_RESPONSE_CODE_INTERNAL_ERROR`
- CID validation failure → `COAP_RESPONSE_CODE_INTERNAL_ERROR`

## Change vs. Previous Implementation

### Before (Removed)
```c
// Monolithic function combining extraction and validation
struct srv_coap_extract_message_3_result srv_coap_extract_message_3(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx,
    struct com_writable_buffer error_response);
```

### After (Current)
```c
// Separate extraction
struct srv_coap_extract_cid_result srv_coap_extract_cid(
    struct com_readonly_buffer request_buffer);

// Separate validation
bool srv_coap_connection_id_is_expected(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx);
```

## Benefits

- **Modular Design**: Each operation has a single responsibility
- **Composable**: Can be used independently for different scenarios
- **Testable**: Both functions can be unit tested in isolation
- **Symmetry**: Matches the Message 1 extraction pattern (extraction + validation)
- **Clear Errors**: Distinguishes empty buffer from extraction library failures

## Deprecated APIs (Internal Only)

The following types remain for internal builder compatibility but are not part of the primary public API:

- `enum srv_coap_extract_message_3_status`
- `struct srv_coap_extract_message_3_result`
- Related functions

These should not be used in new code.

## Testing

All tests pass:
- `test_coap_extract_message_3`: 6/6 PASSED (tests new API directly)
- `test_coap_server_dispatch`: 12/12 PASSED (validates dispatcher behavior)

Separate test cases exist for:
- CID extraction with pointer advancement
- Empty buffer handling
- Library extraction errors
- CID validation: match, mismatch, null context
- Dispatcher error paths for both operations

