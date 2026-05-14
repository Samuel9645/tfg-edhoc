# Server and Shared CoAP Refactor Plan

Goal: Extract CoAP-only flow from src/coap_server.c into server utilities and move duplicated client/server logic into shared coap_utils files.

1. Create include/coap_utils.h and src/coap_utils.c for duplicated client/server helpers.
2. Create include/coap_server_utils.h and src/coap_server_utils.c for server CoAP orchestration helpers.
3. Keep server resource handlers in src/coap_server.c.
4. Keep EDHOC handshake/message choreography unchanged.
5. Refactor run_server to orchestrate helper calls.
6. Reuse existing cleanup through end_coap_session on all error paths.
7. Add new source files to CMake targets.
8. Build and run smoke tests.

Shared candidate now:

- Export EDHOC shared secret helper used in both client and server encrypted message helpers.
