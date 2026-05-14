## Plan: Client/Server Shared Refactor Inventory

Refactor the client and server EDHOC-over-CoAP flows into small modules aligned
with existing coap utility conventions, while deduplicating repeated logic into
a shared coap/edhoc helper layer. Preserve behavior except low-risk correctness
fixes. Use status-returning functions and out-parameters for multi-value
outputs. Put public documentation in headers.

**Steps**

1. Phase 0: Repository plan artifact.
2. Create plans/client_server_shared_refactor_plan.md in the repository as first
   implementation step by copying this plan and keeping it in sync.
3. Phase 1: Shared contracts first (blocking).
4. Add shared headers under include/coap/shared with full API docs and ownership
   contracts.
5. Define status/out-param signatures before any call-site edits.
6. Phase 2: Shared implementation layer.
7. Implement shared request validation helpers.
8. Implement shared response construction helpers.
9. Implement shared EDHOC error-to-CoAP mapping helpers.
10. Phase 3: Client extraction.
11. Add client EDHOC flow module and split compose/process stages into small
    functions.
12. Add client CoAP exchange module to encapsulate response callback state and
    send/wait flow.
13. Reduce src/client.c to orchestration only.
14. Phase 4: Server extraction.
15. Add server EDHOC flow module and split message-1 and message-3 paths.
16. Replace repeated server handler blocks with shared helpers.
17. Reduce src/server.c to bootstrap + thin request dispatcher.
18. Phase 5: Wiring and verification.
19. Update CMakeLists.txt with new modules.
20. Build from clean directory, run server/client handshake, and verify
    malformed-request response behavior parity.

**File-by-file Function Inventory**

1. New file: include/coap/shared/edhoc_request.h
2. Function: coap_shared_validate_edhoc_request(const coap_pdu_t* request,
   content_format_edhoc_values_t expected_format, const uint8_t** payload,
   size_t* payload_len)
3. Function: coap_shared_is_edhoc_message_1(const uint8_t* payload, size_t
   payload_len)
4. Purpose: centralize request content-format and payload extraction logic used
   by server handler branches.

5. New file: src/coap/shared/edhoc_request.c
6. Implement coap_shared_validate_edhoc_request.
7. Implement coap_shared_is_edhoc_message_1.
8. Dependencies: include/coap/common/coap_config.h,
   include/coap/common/coap_status.h.

9. New file: include/coap/shared/edhoc_response.h
10. Function: coap_shared_add_edhoc_response_options(coap_pdu_t* response,
    content_format_edhoc_values_t content_format)
11. Function: coap_shared_add_payload_or_set_error(coap_pdu_t* response, const
    uint8_t* payload, size_t payload_len)
12. Purpose: centralize repeated option-setup and payload-attachment logic.

13. New file: src/coap/shared/edhoc_response.c
14. Implement coap_shared_add_edhoc_response_options.
15. Implement coap_shared_add_payload_or_set_error.
16. Dependencies: include/coap/common/tst_srv_handshake_helpers.h,
    include/coap/common/coap_config.h.

17. New file: include/coap/shared/edhoc_error_map.h
18. Function: coap_shared_map_edhoc_failure_to_response(struct edhoc_context*
    edhoc_ctx, coap_pdu_t* response, const char* operation_label, bool
    protocol_error)
19. Purpose: deduplicate repeated edhoc_error_get_code + log + response-code
    mapping sequences.

20. New file: src/coap/shared/edhoc_error_map.c
21. Implement coap_shared_map_edhoc_failure_to_response.
22. Behavior: set COAP_RESPONSE_CODE_BAD_REQUEST when protocol_error is true,
    else COAP_RESPONSE_CODE_INTERNAL_ERROR.

23. New file: include/edhoc/client/client_flow.h
24. Type: client_edhoc_flow_t opaque session wrapper.
25. Function: client_edhoc_flow_init(client_edhoc_flow_t** flow_out)
26. Function: client_edhoc_flow_compose_message_1(client_edhoc_flow_t* flow,
    uint8_t* payload, size_t payload_capacity, size_t* payload_len)
27. Function: client_edhoc_flow_process_message_2(client_edhoc_flow_t* flow,
    const uint8_t* payload, size_t payload_len)
28. Function: client_edhoc_flow_compose_message_3(client_edhoc_flow_t* flow,
    uint8_t* payload, size_t payload_capacity, size_t* payload_len)
29. Function: client_edhoc_flow_process_message_4(client_edhoc_flow_t* flow,
    const uint8_t* payload, size_t payload_len)
30. Function: client_edhoc_flow_deinit(client_edhoc_flow_t** flow_ptr)
31. Purpose: isolate libedhoc message sequencing from transport orchestration.

32. New file: src/edhoc/client/client_flow.c
33. Implement credential callback wrappers currently in src/client.c.
34. Implement message compose/process helpers with strict return-value
    propagation.
35. Keep CID prepend handling in compose_message_3 path only.

36. New file: include/coap/client_edhoc_exchange.h
37. Type: client_edhoc_exchange_t opaque transport state (response flag +
    receive buffer metadata).
38. Function: client_edhoc_exchange_init(client_edhoc_exchange_t** exchange_out,
    coap_context_t* context, coap_session_t* session, const coap_uri_t* uri,
    const coap_address_t* destination)
39. Function: client_edhoc_exchange_send(client_edhoc_exchange_t* exchange,
    const uint8_t* payload, size_t payload_len, content_format_edhoc_values_t
    content_format)
40. Function: client_edhoc_exchange_wait_and_get(client_edhoc_exchange_t*
    exchange, uint8_t* response_buffer, size_t response_capacity, size_t*
    response_len)
41. Function: client_edhoc_exchange_reset(client_edhoc_exchange_t* exchange)
42. Function: client_edhoc_exchange_deinit(client_edhoc_exchange_t**
    exchange_ptr)
43. Purpose: isolate callback state and repeated send/wait/receive extraction.

44. New file: src/coap/client_edhoc_exchange.c
45. Implement internal response callback using exchange app-data instead of
    global statics.
46. Reuse existing helpers from src/coap/client_utils.c for request construction
    and waiting.
47. Dependency note: keep callback signature compatible with libcoap.

48. Modified file: src/client.c
49. Keep only orchestration function run_client and minimal local setup.
50. Replace static globals and response handler with client_edhoc_exchange APIs.
51. Sequence in run_client: startup, parse URI/session, init flow, compose/send
    M1, process M2, compose/send M3, process M4, cleanup.
52. Preserve existing success/failure return semantics and logging behavior.

53. New file: include/edhoc/server/server_flow.h
54. Function: server_edhoc_handle_message_1(coap_session_t* session, const
    uint8_t* request_payload, size_t request_len, uint8_t* response_payload,
    size_t response_capacity, size_t* response_len)
55. Function: server_edhoc_handle_message_3(coap_session_t* session, const
    uint8_t* request_payload, size_t request_len, uint8_t* response_payload,
    size_t response_capacity, size_t* response_len)
56. Purpose: isolate EDHOC-specific message processing from CoAP request
    boilerplate.

57. New file: src/edhoc/server/server_flow.c
58. Implement context allocation/setup/attachment for message-1 path.
59. Implement CID extraction/validation + message-3 process + message-4 compose
    path.
60. Route failures through coap_shared_map_edhoc_failure_to_response caller
    contract.

61. New file: include/coap/server_edhoc_dispatch.h
62. Function: coap_server_dispatch_edhoc_post(coap_session_t* session, const
    coap_pdu_t* request, coap_pdu_t* response)
63. Purpose: thin dispatch wrapper called by edhoc_post_handler, using shared
    request/response helpers + server flow handlers.

64. New file: src/coap/server_edhoc_dispatch.c
65. Implement branch decision for message-1 vs continuation and invoke
    server_flow functions.
66. Apply shared helper functions for request validation and response
    option/payload work.

67. Modified file: src/server.c
68. Keep run_server focused on context/endpoints/resource registration/event
    loop.
69. Keep edhoc_post_handler as tiny wrapper delegating to
    coap_server_dispatch_edhoc_post.

70. Modified file: CMakeLists.txt
71. Add new source files to client and server targets.
72. Keep existing target names, output directories, and library link
    configuration unchanged.

73. Optional consistency-only modified file: include/client.h
74. Ensure top-level function docs remain consistent with new orchestration
    behavior.

75. Optional consistency-only modified file: include/server.h
76. Ensure top-level function docs remain consistent with dispatch-based flow.

**Dependencies and Parallelism**

1. Shared headers (steps 4-5) block shared implementation and call-site
   refactors.
2. Shared implementation (steps 7-9) blocks server dispatch refactor.
3. Client extraction (steps 11-13) can run in parallel with server flow
   extraction (steps 15-17) after shared contracts are in place.
4. CMake wiring happens after all new files are added.
5. Verification happens last.

**Duplicate-Code Consolidation Targets**

1. Server: repeated content-format and payload extraction blocks.
2. Server: repeated response option setup blocks.
3. Server: repeated EDHOC error extraction + response mapping blocks.
4. Client: repeated request-build/send/wait/receive blocks for message-1 and
   message-3 exchanges.

**Scope Boundaries**

1. Included: structural refactor, dedup through shared helpers, header
   documentation, low-risk safe fixes.
2. Excluded: protocol semantic changes, new automated tests, broad abstraction
   beyond verified duplication.
3. Kept role-specific: asymmetric CID prepend/extract behavior unless true
   duplication appears.

**Verification**

1. Configure and build from clean build directory.
2. Run server then client and verify successful EDHOC 1-4 flow.
3. Verify malformed request cases still map to expected BAD_REQUEST and
   INTERNAL_ERROR behavior.
4. Repeat handshake runs to check context lifecycle stability and cleanup
   determinism.

**Execution note**

1. During implementation, first create
   plans/client_server_shared_refactor_plan.md in the repository, then follow
   this plan phase order.

