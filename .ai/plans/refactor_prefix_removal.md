# Refactor: Remove client_/server_ Prefixes

**Date:** March 22, 2026  
**Session:** File organization and naming refactor

## Overview

Removed redundant `client_` and `server_` filename prefixes from CoAP module
files since they are now organized in `client/` and `server/` subdirectories,
making the prefixes unnecessary.

## Files Renamed

### Client Module

- `src/coap/client/client_edhoc_exchange.c` → `src/coap/client/edhoc_exchange.c`
- `include/coap/client/client_edhoc_exchange.h` →
  `include/coap/client/edhoc_exchange.h`
- `src/coap/client/client_utils.c` → `src/coap/client/utils.c`
- `include/coap/client/client_utils.h` → `include/coap/client/cli_utils.h`

### Server Module

- `src/coap/server/server_edhoc_dispatch.c` → `src/coap/server/edhoc_dispatch.c`
- `include/coap/server/server_edhoc_dispatch.h` →
  `include/coap/server/edhoc_dispatch.h`
- `src/coap/server/server_utils.c` → `src/coap/server/utils.c`
- `include/coap/server/server_utils.h` → `include/coap/server/cli_utils.h`

## Files Modified (Include References Updated)

### Source Files

- `src/client.c` - Updated #include directives
- `src/server.c` - Updated #include directives
- `src/common/client_cleanup.c` - Updated #include directives
- `src/coap/client/edhoc_exchange.c` - Updated self-include and dependency
  includes
- `src/coap/server/edhoc_dispatch.c` - Updated self-include
- `src/coap/server/utils.c` - Updated self-include

### Header Files

- `include/common/cli_resources.h` - Updated #include directives

### Build Configuration

- `CMakeLists.txt` - Updated source file paths in `add_executable()`
  declarations for both `server` and `client` targets

## Changes Summary

### Include Path Updates

All #include statements were updated from:

```c
#include "coap/client/client_edhoc_exchange.h"
#include "coap/client/client_utils.h"
#include "coap/server/server_edhoc_dispatch.h"
#include "coap/server/server_utils.h"
```

To:

```c
#include "coap/client/edhoc_exchange.h"
#include "coap/client/cli_utils.h"
#include "coap/server/edhoc_dispatch.h"
#include "coap/server/cli_utils.h"
```

### CMakeLists.txt Updates

Updated file references in add_executable() declarations:

- Server target: `server_utils.c` → `utils.c`, `server_edhoc_dispatch.c` →
  `edhoc_dispatch.c`
- Client target: `client_utils.c` → `utils.c`, `client_edhoc_exchange.c` →
  `edhoc_exchange.c`

## Directory Structure Result

```
src/coap/
├── client/
│   ├── edhoc_exchange.c
│   ├── utils.c
│   ├── log_edhoc_error_response.c
│   └── client_example.c
├── server/
│   ├── edhoc_dispatch.c
│   ├── utils.c
│   └── map_edhoc_error_to_response.c
├── shared/
│   ├── edhoc_request.c
│   └── edhoc_response.c
└── common/
    └── helpers.c

include/coap/
├── client/
│   ├── edhoc_exchange.h
│   ├── cli_utils.h
│   └── log_edhoc_error_response.h
├── server/
│   ├── edhoc_dispatch.h
│   ├── cli_utils.h
│   └── map_edhoc_error_to_response.h
├── shared/
│   ├── edhoc_request.h
│   └── edhoc_response.h
└── common/
    ├── tst_srv_handshake_helpers.h
    ├── data_models.h
    ├── coap_config.h
    └── coap_status.h
```

## Rationale

- **Reduced redundancy:** File names no longer duplicate the directory structure
- **Improved readability:** Shorter, cleaner file names
- **Better organization:** The directory itself indicates whether code is client
  or server side
- **Consistency:** Follows the pattern already established by other modules (
  shared, common)

## Testing Status

- All file references have been updated
- No remaining references to old filenames detected
- Ready for compilation verification
