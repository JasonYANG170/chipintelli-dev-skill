# Recipe: OTA Firmware Update for CI230X

> **SDK**: `CI230X_wifi_combo_sdk_release_v1.1.1`
> **Chips**: CI2305, CI2306 (LN882H + CI13xx)
> **Component**: `components/fota/`

> Applies to: CI2305 and CI2306; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci230x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci230x/recipes/ota.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

The CI230X supports two types of OTA (Over-The-Air) firmware updates:
1. **WiFi OTA**: Updates the LN882H firmware (application code)
2. **Audio OTA**: Updates the CI13xx voice chip firmware

Both use the FOTA (Firmware Over-The-Air) agent in `components/fota/ota_agent/`.

---

## FOTA Architecture

```
┌─────────────────────────────────────────────────────┐
│                    LN882H (WiFi)                     │
│                                                     │
│  ┌──────────┐    ┌───────────┐    ┌─────────────┐  │
│  │ HTTP     │───▶│ OTA Agent │───▶│ Flash Write │  │
│  │ Download │    │           │    │ (OTA area)  │  │
│  └──────────┘    └─────┬─────┘    └─────────────┘  │
│                        │                            │
│                  ┌─────┴─────┐                     │
│                  │ Boot Agent │                     │
│                  │ (restore)  │                     │
│                  └───────────┘                     │
│                                                     │
│  ┌──────────────────────────────────────────────┐  │
│  │         SDIO ──────────────────────▶         │  │
│  │         (Audio OTA to CI13xx)                │  │
│  └──────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
                         │ SDIO
                         ▼
┌─────────────────────────────────────────────────────┐
│                    CI13xx (Voice)                    │
│  • Receives firmware via SDIO                        │
│  • Writes to CI13xx flash                            │
│  • Reboots with new firmware                         │
└─────────────────────────────────────────────────────┘
```

## Flash Partition for OTA

The OTA partition is defined in `flash_partition_cfg.json`:

| Partition | Offset | Size | Purpose |
|-----------|--------|------|---------|
| APP | `0x00007000` | 1100 KB | Running application |
| OTA | `0x0011A000` | 672 KB | Download/restore area |

**OTA flow**: New firmware is downloaded to the OTA partition. On reboot, the boot agent verifies and copies the OTA image to the APP partition.

---

## OTA Image Format

### Image Header (`ota_types.h`)

```c
typedef struct {
    image_type_t  image_type;          // ORIGINAL, ORIGINAL_XZ, DIFF, DIFF_XZ
    image_ver_t   ver;                 // Version (major.minor)
    image_ver_t   ver_diff_depend;     // Base version for diff OTA
    uint32_t      img_size_orig;       // Original image size
    uint32_t      img_size_orig_xz;    // Compressed original size
    uint32_t      img_size_diff;       // Diff image size
    uint32_t      img_size_diff_xz;    // Compressed diff size
    uint32_t      img_crc32_orig;      // CRC32 of original
    uint32_t      img_crc32_orig_xz;   // CRC32 of compressed original
    uint32_t      img_crc32_diff;      // CRC32 of diff
    uint32_t      img_crc32_diff_xz;   // CRC32 of compressed diff
    uint8_t       res[212];            // Reserved
    uint32_t      header_crc32;        // CRC32 of header (except itself)
} image_hdr_t;
```

### Image Types

```c
typedef enum {
    IMAGE_TYPE_ORIGINAL     = 1u,  // Full image
    IMAGE_TYPE_ORIGINAL_XZ  = 2u,  // Compressed full image (XZ)
    IMAGE_TYPE_DIFF         = 3u,  // Diff image (requires base version)
    IMAGE_TYPE_DIFF_XZ      = 4u,  // Compressed diff image
} image_type_t;
```

- **ORIGINAL**: Complete firmware image. Simplest, largest download.
- **ORIGINAL_XZ**: XZ-compressed full image. Smaller download, requires decompression.
- **DIFF**: Binary diff from a specific base version. Smallest, but requires exact base version.
- **DIFF_XZ**: XZ-compressed diff. Smallest possible, most complex.

---

## OTA Agent API

### Port Layer (`ota_port.h`)

```c
// Flash driver interface
typedef int (*ota_flash_write_t)(uint32_t addr, const void *buf, uint32_t len);
typedef int (*ota_flash_read_t)(uint32_t addr, void *buf, uint32_t len);
typedef int (*ota_flash_erase_t)(uint32_t addr, uint32_t len);

// State management
typedef int (*ota_upg_state_set_t)(upg_state_t state);
typedef int (*ota_upg_state_get_t)(upg_state_t *state);

typedef struct {
    ota_flash_drv_t      flash_drv;     // read/write/erase
    ota_upg_state_set_t  upg_state_set; // set upgrade state
    ota_upg_state_get_t  upg_state_get; // get upgrade state
} ota_port_ctx_t;

// Initialize OTA port (registers flash driver + state functions)
ota_err_t ota_port_init(void);

// Get port context
ota_port_ctx_t *ota_get_port_ctx(void);
```

### Image Verification (`ota_image.h`)

```c
// Read image header from a partition
int image_header_read(partition_type_t type, image_hdr_t *header);

// Fast read image header from specific address
int image_header_fast_read(uint32_t start_addr, image_hdr_t *header);

// Verify image header (CRC, type, etc.)
int image_header_verify(image_hdr_t *header);

// Verify image body (CRC of entire image)
int image_body_verify(uint32_t start_addr, image_hdr_t *header);
```

### Boot Agent (`ota_agent.h`)

```c
typedef void (*jump_to_app_t)(uint32_t app_entity_offset);

// Boot-time upgrade agent
// Called by bootloader to check OTA partition and restore
int ota_boot_upgrade_agent(jump_to_app_t jump_to_app);
```

### Upgrade State Machine

```c
typedef enum {
    UPG_STATE_DOWNLOAD_ING  = 0,  // Downloading OTA image
    UPG_STATE_DOWNLOAD_OK   = 1,  // Download complete, verified
    UPG_STATE_RESTORE_ING   = 2,  // Restoring (copying OTA -> APP)
    UPG_STATE_RESTORE_OK    = 3,  // Restore complete
    UPG_STATE_RESTORE_FILED = 4,  // Restore failed
    UPG_STATE_REPORT_OK     = 5,  // Cloud notified of result
} upg_state_t;
```

---

## Enabling OTA

### WiFi OTA (LN882H firmware update)

In `project/ci230x-wifi-sdk-combo/CMakeLists.txt`:

```cmake
set(CIAS_AIOT_WIFI_OTA_ENABLE 1)
```

This includes:
- `app/cias_aiot_wifi/cias_ota/ota_wifi/*.c` -- WiFi OTA logic
- `components/net/httpsclient/` -- HTTP client for download

### Audio OTA (CI13xx firmware update)

```cmake
set(CIAS_AIOT_AUDIO_OTA_ENABLE 1)
```

This includes:
- `app/cias_aiot_wifi/cias_ota/ota_audio/*.c` -- Audio OTA agent
- SDIO communication to send firmware to CI13xx

### Both can be enabled simultaneously:

```cmake
set(CIAS_AIOT_WIFI_OTA_ENABLE  1)
set(CIAS_AIOT_AUDIO_OTA_ENABLE 1)
```

---

## OTA Implementation Flow

### WiFi OTA (LN882H)

```
1. Cloud notifies device of available update
   └─> Device receives OTA URL

2. Download phase (UPG_STATE_DOWNLOAD_ING)
   ├─ HTTP GET request to download URL
   ├─ Write received data to OTA partition (0x0011A000)
   └─ Verify image header + body CRC

3. Download complete (UPG_STATE_DOWNLOAD_OK)
   └─ Save state to NVDS

4. Reboot device

5. Boot phase (UPG_STATE_RESTORE_ING)
   ├─ Bootloader reads OTA partition
   ├─ ota_boot_upgrade_agent() called
   ├─ Verifies image header
   ├─ Erases APP partition
   ├─ Copies OTA image to APP partition
   └─ Verifies copied image

6. Restore complete (UPG_STATE_RESTORE_OK)
   ├─ Save state to NVDS
   └─ Jump to new application

7. Report result (UPG_STATE_REPORT_OK)
   └─ New firmware notifies cloud of success
```

### Audio OTA (CI13xx)

```
1. Cloud notifies device of CI13xx firmware update

2. Download phase
   ├─ Download CI13xx firmware via HTTP
   ├─ Store in LN882H flash (OTA or USER partition)
   └─ Verify firmware integrity

3. Transfer phase
   ├─ Send firmware to CI13xx via SDIO in chunks
   ├─ CI13xx writes to its own flash
   └─ Verify each chunk

4. Reboot CI13xx
   ├─ CI13xx boots with new firmware
   └─ CI13xx reports version to LN882H

5. Report result to cloud
```

---

## XZ Decompression

The FOTA agent supports XZ-compressed images via `xz_decompress.c`:

```c
// XZ decompression is used internally by the OTA agent
// when image_type is IMAGE_TYPE_ORIGINAL_XZ or IMAGE_TYPE_DIFF_XZ
```

XZ compression significantly reduces download size (typically 40-60% reduction for firmware images).

---

## Error Handling (`ota_err.h`)

The OTA agent returns error codes from `ota_err.h` (`ota_err_t` enum):

```c
typedef enum {
    OTA_ERR_NONE              = 0u,   // Success
    OTA_ERR_INVALID_PARAM,            // Invalid parameter

    OTA_ERR_NVDS_RW          = 10u,   // NVDS read/write failure
    OTA_ERR_UPG_STATE,                // Invalid upgrade state transition
    OTA_ERR_PARTITION_TAB,            // Partition table error
    OTA_ERR_DECOMPRESS,               // XZ decompression failure

    OTA_ERR_IMG_TYPE         = 20u,   // Invalid image type
    OTA_ERR_IMG_HEADER_READ,          // Image header read failure
    OTA_ERR_IMG_HEADER_VERIFY,        // Image header CRC verification failed
    OTA_ERR_IMG_ENTITY_VERIFY,        // Image body CRC verification failed

    OTA_ERR_IMPOSSIBLE_VER   = 30u,   // Version impossible (downgrade rejected)
    OTA_ERR_NOT_SUPPORT,              // Operation not supported
} ota_err_t;
```

---

## Best Practices

### 1. Always Verify Before Restore

The boot agent verifies the image header and body CRC before copying. Never skip verification:

```c pseudocode
// The boot agent verifies the image header and body CRC before copying.
// Use the real error codes from ota_err.h:
if (image_header_verify(&header) != 0) {
    // Header invalid -- abort OTA
    return OTA_ERR_IMG_HEADER_VERIFY;
}
if (image_body_verify(OTA_SPACE_OFFSET, &header) != 0) {
    // Body CRC failed -- abort OTA
    return OTA_ERR_IMG_ENTITY_VERIFY;
}
```

### 2. Handle Power Failure

If power fails during restore, the APP partition may be corrupted. The boot agent should:
- Check for valid OTA image on every boot
- If OTA partition has valid image and APP is invalid, retry restore
- If both are invalid, enter recovery mode

### 3. Version Management

Track firmware versions:
```c pseudocode
// Version management is application-level logic.
// The SDK does not define OTA_ERR_VERSION; use OTA_ERR_IMPOSSIBLE_VER
// when rejecting an impossible/downgrade version.
image_ver_t current_ver = {1, 0};  // v1.0
image_ver_t new_ver = {1, 1};      // v1.1

// Reject downgrade
if (new_ver.ver_major < current_ver.ver_major ||
    (new_ver.ver_major == current_ver.ver_major && new_ver.ver_minor <= current_ver.ver_minor)) {
    return OTA_ERR_IMPOSSIBLE_VER;
}
```

### 4. Use Compressed Images for Production

For production OTA updates, use `IMAGE_TYPE_ORIGINAL_XZ`:
- Smaller download (saves bandwidth and time)
- XZ decompression is fast on Cortex-M4
- CRC verification after decompression

### 5. Report Results to Cloud

After OTA completes (success or failure), report to cloud:
```c pseudocode
// In new firmware, after boot
upg_state_t state;
ota_port_ctx_t *port = ota_get_port_ctx();
port->upg_state_get(&state);

if (state == UPG_STATE_RESTORE_OK) {
    // cloud_report_ota_result() is application-specific;
    // implement using your cloud platform's API
    // cloud_report_ota_result(true, new_version);
    port->upg_state_set(UPG_STATE_REPORT_OK);  // Mark as reported
} else if (state == UPG_STATE_RESTORE_FILED) {
    // cloud_report_ota_result(false, 0);
}
```

---

## Flash Partition Considerations

The OTA partition (672 KB) must hold the entire compressed firmware image. If your application grows:

1. Check if the compressed image fits in 672 KB
2. If not, adjust partitions in `flash_partition_cfg.json`:
   ```json
   {"partition_type": "APP", "start_addr": "0x00007000", "size_KB": 900},
   {"partition_type": "OTA", "start_addr": "0x000E7000", "size_KB": 872}
   ```
3. Update linker script FLASH LENGTH to match APP size
4. Ensure 4K alignment and no overlaps
