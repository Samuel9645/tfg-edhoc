# Internal Session Storage Dictionary — Discovery

## Problem Description

The EDHOC server currently stores the active session context inside CoAP session
data. The target change is to move that ownership boundary out of the CoAP
session and into a static internal dictionary that maps connection IDs to
session contexts.

Message 1 should register the newly created EDHOC context in the dictionary.
Message 3 should recover the context from the dictionary instead of reading it
from CoAP session data.

## Context and Affected Users

- Affects the EDHOC server dispatcher path for Message 1 and Message 3.
- Affects multi-session handling, where multiple concurrent EDHOC sessions must
  be tracked safely.
- Affects tests and stubs that currently assume the context is stored in CoAP
  session data.

## Identified Constraints

- The dictionary is static and has a fixed maximum capacity.
- The dictionary stores only pointers to `struct edhoc_context`; it does not own
  the lifetime of the contexts.
- The dispatcher remains responsible for removing entries and cleaning up
  contexts.
- On Message 1 registration failure, the dispatcher must clean up the newly
  created context and return `COAP_RESPONSE_CODE_INTERNAL_ERROR`.
- On Message 3 lookup failure, the dispatcher must return
  `COAP_RESPONSE_CODE_INTERNAL_ERROR`.
- If the extracted CID does not match the expected connection ID for the
  retrieved context, the dispatcher must return
  `COAP_RESPONSE_CODE_INTERNAL_ERROR`.

## Corner Cases to Cover

- Dictionary full when registering a new context.
- Duplicate CID when registering a new context.
- Lookup for a CID that does not exist.
- Lookup that returns `NULL` because the context was removed or lost.
- CID mismatch after a successful lookup.
- Message 3 extraction failure before lookup.
- Session termination after success and after error, ensuring dictionary cleanup
  happens in both cases.

## Verifiable Success Criteria

- Message 1 stores the session context through `set_context_by_cid` instead of
  CoAP session data.
- Message 3 retrieves the session context through `get_context_by_cid` instead
  of CoAP session data.
- A full dictionary causes Message 1 registration to fail with internal error
  and cleans up the context.
- A missing context during Message 3 causes internal error.
- Existing tests and new integration tests cover the success path and the two
  new failure paths.

## Notes for Planning

This discovery can be used as the base for `/plan` and for the implementation
tasks around dispatcher dependency updates, dictionary management, and test
adaptation.
