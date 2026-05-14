# Client CoAP Refactor Plan

Goal: Extract CoAP-only code from src/coap_client.c into helper functions, keeping EDHOC and non-CoAP code unchanged.

1. Identify CoAP-only blocks in run_client.
2. Add declarations to include/coap_client_utils.h with input-first/output-last signatures.
3. Implement helper functions in src/coap_client_utils.c.
4. Replace inlined CoAP logic in src/coap_client.c with helper calls.
5. Keep cleanup behavior through end_coap_session unchanged.
6. Wire helper source into build files.
7. Build and smoke test client/server flow.

Out of scope:

- Credential callbacks
- run_handshake
- send_message internals
- Non-CoAP modules
