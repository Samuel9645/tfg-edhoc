## Plan: RFC-Aligned Header Refactor Refresh

Update the include/src layout to a clear transport-vs-protocol split, keep
naming collision-safe, and apply namespace-safe symbol prefixes in one
coordinated pass. This plan started from the pre-refactor baseline and includes
archival in the plans folder.

**Steps**

1. Baseline validation and scope lock: confirm current tree and record exact
   starting points for all targeted paths.
2. Phase 1, directory and header re-organization: create include/core and target
   subfolders; move or rename public headers to the new layer-oriented
   structure.
3. Phase 1 details, mandatory renames:

- include/client.h -> include/core/client.h
- include/server.h -> include/core/server.h
- include/psa_setup.h -> include/core/psa_setup.h
- include/coap/client/edhoc_exchange.h -> include/coap/client/exchange.h
- include/coap/server/edhoc_dispatch.h -> include/coap/server/srv_dispatch.h
- include/edhoc/client/flow.h -> include/edhoc/client/message_3_handler.h
- include/edhoc/server/flow.h -> include/edhoc/server/message_3_handler.h
- include/coap/common/coap_config.h -> include/coap/coap_config.h
- include/edhoc/common/coap_config.h -> include/edhoc/edhoc_config.h
- include/coap/shared/edhoc_request.h ->
  include/coap/shared/srv_parse_edhoc_request.h
- include/coap/shared/edhoc_response.h -> include/coap/shared/coap_com_response.h

4. Phase 1 details, credentials consolidation:

- merge include/edhoc/credentials/client_private_key.h and shared client
  credential declarations into include/edhoc/credentials/client_keys.h
- merge include/edhoc/credentials/server_private_key.h and shared server
  credential declarations into include/edhoc/credentials/server_keys.h
- retire include/edhoc/credentials/public_data.h only after all call sites are
  migrated

5. Phase 1 details, private-header policy:

- remove include/coap/server/internal/edhoc_dispatch_engine.h from public
  include API
- keep private declaration alongside implementation under
  src/coap/server/internal

6. Phase 2, source file renames to match headers:

- src/coap/client/edhoc_exchange.c -> src/coap/client/exchange.c
- src/coap/client/log_edhoc_error_response.c ->
  src/coap/client/log_error_response.c
- src/coap/server/edhoc_dispatch.c -> src/coap/server/dispatch.c
- src/coap/server/map_edhoc_error_to_response.c ->
  src/coap/server/map_error_to_response.c
- src/coap/shared/edhoc_request.c -> src/coap/shared/request.c
- src/coap/shared/edhoc_response.c -> src/coap/shared/response.c
- src/edhoc/client/flow.c -> src/edhoc/client/handshake.c
- src/edhoc/server/flow.c -> src/edhoc/server/handshake.c

7. Phase 3, include migration: update all include directives across src,
   include, and tests to new paths and names, including config collision-safe
   names coap/coap_config.h and edhoc/edhoc_config.h.
2. Phase 4, prefix rollout in one pass: apply naming map to all public
   declarations and definitions, then update all call sites.

- core: tfg_
- coap/server: coap_server_
- edhoc/server: server_edhoc_
- coap/client: coap_client_
- edhoc/client: client_edhoc_
- common: tfg_common_

9. Phase 5, CMake and test integration: update root CMakeLists source lists and
   tests/CMakeLists include assumptions so internal headers are not exposed
   through public include paths.
2. Phase 6, verification:

- clean configure and full build
- run tests
- grep checks for removed names (flow.h, coap_config.h old paths, edhoc_dispatch
  old
  names)
- ensure no include/coap/server/internal references remain outside server
  internals

**Status snapshot (2026-03-25)**

- Compile succeeded after structural updates.
- Include and source tree refactor is in progress.
- Prefix rollout and full test validation remain pending.
