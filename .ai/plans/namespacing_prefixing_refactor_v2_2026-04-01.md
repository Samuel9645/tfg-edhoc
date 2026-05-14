# Plan: Namespacing & Prefixing Refactor (V2)

## TL;DR

Apply a formal prefixing system across the entire codebase to establish module boundaries and avoid naming conflicts with libedhoc. Refactor in phases: common/core → edhoc/server → coap → final verification. Static functions keep base names (no prefix, no underscore). External libraries are untouched.

---

## Naming Rules (Confirmed)

| Category | Prefix | Rules |
|----------|--------|-------|
| **EDHOC Server** | `edhs_` | Structs/enums: `edhs_name_t`; functions: `edhs_name()`; static funcs: `name()` |
| **EDHOC Client** | `edhc_` | Structs/enums: `edhc_name_t`; functions: `edhc_name()`; static funcs: `name()` |
| **CoAP Server** | `cps_` | Structs/enums: `cps_name_t`; functions: `cps_name()`; static funcs: `name()` |
| **CoAP Client** | `cpc_` | Structs/enums: `cpc_name_t`; functions: `cpc_name()`; static funcs: `name()` |
| **Project Common** | `com_` | Structs/enums: `com_name_t`; functions: `com_name()`; static funcs: `name()` |
| **Macros** | (none) | No macros should be prefixed; project has minimal/no macros |
| **Static Functions** | (none) | No prefix, no leading underscore; base name only (e.g., `parse_data()`) |
| **Header Guards** | (unchanged) | Keep existing guards as-is |

---

## Implementation Phases

### **Phase 1: Common Base Library (Sequential)**

**Goal:** Refactor `include/common/` first, then `include/core/`. This is the foundation; other modules depend on it.

1. **Phase 1A: `include/common/` refactor**
   - Rename struct/enum definitions in header files (apply `com_` prefix)
   - Update all references in `.c` files throughout the codebase
   - Verify compilation after completion

2. **Phase 1B: `include/core/` refactor**
   - Rename struct/enum definitions in header files (apply `com_` prefix, as core is shared infrastructure)
   - Update all references in `.c` files
   - Verify compilation

**Status:** Pending approval gates after each sub-phase

---

### **Phase 2: EDHOC Server**

**Goal:** Refactor `edhoc/server/` and dependent code.

1. Rename handshake handlers and types with `edhs_` prefix
2. Update mapper dependencies in `coap/server/edhoc_mapper/` (names will change, but mappers themselves use `cps_` prefix)
3. Verify compilation

**Status:** Pending Phase 1 completion + approval

---

### **Phase 3: CoAP & Client**

**Goal:** Refactor CoAP and client modules.

1. Refactor `coap/server/` with `cps_` prefix
2. Refactor `coap/client/` with `cpc_` prefix (if it exists)
3. Refactor `edhoc/client/` with `edhc_` prefix (if it exists)
4. Update any cross-module dependencies

**Status:** Pending Phase 2 completion + approval

---

### **Phase 4: Final Cleanup & Verification**

**Goal:** Ensure consistency and no naming collisions.

1. Grep for any remaining functions starting with `edhoc_` (should be zero; these belong to libedhoc library)
2. Verify all struct references match new names (no undefined symbols)
3. Check that static functions have no prefix
4. Compilation and test suite pass

**Status:** Pending Phase 3 completion

---

## Critical Constraints

- **Do NOT change header guards** — they are already correct
- **Do NOT refactor external libraries** (`libcoap`, `libedhoc`, `unity` in `externals/`)
- **Static functions** keep base names only (no prefix, no underscore prefix)
- **Macros** are not prefixed (project has minimal macros)

---

## Verification Strategy

After each phase:

1. Run `make clean && make` to detect compilation errors
2. Run test suite to catch runtime issues
3. Grep for old names to catch missed references

---

## Decisions Made

- Static functions do NOT get underscores or prefixes; only global symbols get prefixes
- No macros are prefixed
- External libraries are off-limits
- Sequential approach for common/core ensures foundational stability before downstream modules

---

## Implementation Progress

### Phase 1A: `include/common/` refactoring

**Status:** ✅ COMPLETE - All 4 tests pass

**Changes Made:**

- Renamed types:
  - `common_request_payload_t` → `com_request_payload_t`
  - `common_response_buffer_t` → `com_response_buffer_t`
  - `session_resources_t` → `com_session_resources_t`
  - `emulation_status_t` → `com_emulation_status_t`

- Renamed enum values:
  - `EMULATION_SUCCESS` → `COM_EMULATION_SUCCESS`
  - `EMULATION_FAILURE` → `COM_EMULATION_FAILURE`

- Renamed all related functions with `com_` prefix

- Updated all callsites across src/ and tests/ directories

- **Compilation:** ✅ Clean, no errors
- **Tests:** ✅ 4/4 passing (test_dispatch, test_handshake, test_handshake_mocked_libedhoc, test_integration)

### Phase 1B: `include/core/` refactoring

**Status:** Pending approval
