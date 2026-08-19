# SDK Version Index

Complete mapping of all local SDK directories to chip families and use cases.

## CI130X SDKs (2nd Gen, RISC-V Nuclei N300)

| SDK Directory | Version | Type | Chips | Key Components | Notes |
|---|---|---|---|---|---|
| `CI130X_SDK_Offline_V2.1.14` | 2.1.14 | Offline ASR | CI1302, CI1306 | Standard ASR, player, flash_control | **Recommended for CI130X** |
| `CI130X_SDK_Offline_V1.11.7` | 1.11.7 | Offline ASR | CI1302, CI1306 | Legacy ASR | Older version |
| `CI130X_SDK_ASR_Offline_V2.2.0` | 2.2.0 | ASR Offline | CI1302, CI1306 | Newer ASR algorithm | V2 algorithm |
| `CI130X_SDK_ASR_Offline_V1.12.16` | 1.12.16 | ASR Offline | CI1302, CI1306 | Legacy ASR algorithm | V1 algorithm |
| `CI130X_SDK_ALG_V2.7.14` | 2.7.14 | Algorithm | CI1306 | AEC, Denoise, BF, DOA, Dereverb | Audio algorithm focused |
| `CI130X_SDK_ALG_PRO_V2.6.3` | 2.6.3 | Algorithm Pro | CI1306 | Advanced DSP, deep denoise | Pro algorithm |
| `CI130X_SDK_Offline_IR_V2.0.10` | 2.0.10 | IR Control | CI1302, CI1306 | Voice + IR remote | IR learn/send |
| `CI130X_SDK_TwoMic_V1.0.2` | 1.0.2 | Two Mic | CI1306 | Dual-mic beamforming | 2-mic DOA + BF |
| `CI130X_SDK_LLM_AIOT_2.2.7(1)` | 2.2.7 | LLM AIoT | CI1306 | LLM, NLP, TTS, VPR, online+offline | AIoT with cloud LLM |
| `CI130X_SDK-CI130X_SDK_Offline_V2.0.10` | 2.0.10 | Offline ASR | CI1302, CI1306 | Standard ASR (duplicate) | Archived copy |
| `CI1302_SDK_V2.2.0_蓝牙sample` | 2.2.0 | BLE Sample | CI1302 | BLE + ASR sample | CI1302 BLE demo |

## CI13LC SDKs (3rd Gen Low-Cost, RISC-V Nuclei N300)

| SDK Directory | Version | Type | Chips | Key Components | Notes |
|---|---|---|---|---|---|
| `CI13LC_SDK_V2.0.15` | 2.0.15 | Offline ASR | CI1311-CI13642 | ASR, CWSL, NL, multi-intent | **Recommended for CI13LC** |
| `CI13LC_SDK_2.1.5` | 2.1.5 | Offline ASR | CI1311-CI13642 | ASR (newer) | Newer version |
| `CI13LC_SDK_IR_V2.0.15` | 2.0.15 | IR Control | CI13242, CI13322 | Voice + IR remote | IR variant |
| `CI13LC_SDK_NN_ENC_V1.2.6` | 1.2.6 | NN Encode | CI13322 | Neural network encoder | NN encoder focused |

## CI13XX SDKs (3rd Gen Unified, RISC-V Nuclei N300)

| SDK Directory | Version | Type | Chips | Key Components | Notes |
|---|---|---|---|---|---|
| `CI13XX_SDK_ASR_ALG_V2.7.12` | 2.7.12 | ASR + Algorithm | CI130X + CI13LC | ASR + AEC/BF/DOA/Denoise + TTS/VPR | **Recommended unified SDK** |
| `CI13XX_SDK_LLM_AIOT_2.1.2` | 2.1.2 | LLM AIoT | CI130X + CI13LC | LLM, NLP, TTS, VPR, BLE, IR, protocol | **Recommended for AIoT** |
| `CI13XX_SDK_LLM_AIoT_V1.0.10` | 1.0.10 | LLM AIoT | CI130X + CI13LC | Legacy LLM AIoT | Older version |

## CI230X SDKs (Wi-Fi Combo, ARM Cortex-M4 + RISC-V)

| SDK Directory | Version | Type | Chips | Key Components | Notes |
|---|---|---|---|---|---|
| `CI230X_wifi_combo_sdk_release_v1.1.1` | 1.1.1 | Wi-Fi Combo | CI2305, CI2306 | Wi-Fi, BLE, FOTA, net, AT | **Recommended for CI230X** |

## CI23LC SDKs (3rd Gen + BLE, RISC-V Nuclei N300)

| SDK Directory | Version | Type | Chips | Key Components | Notes |
|---|---|---|---|---|---|
| `CI23LC_SDK_BLE_V1.3.13` | 1.3.13 | BLE | CI2312, CI23242 | BLE + ASR + CWSL, app_ble demos | **Recommended for CI23LC** |
| `CI23LC_SDK_BLE_V1.2.2` | 1.2.2 | BLE | CI2312, CI23242 | BLE + ASR | Older version |
| `CI23LC_SDK_BLE_V1.1.12_alpha` | 1.1.12a | BLE | CI2312, CI23242 | BLE + ASR (alpha) | Alpha, not recommended |

## SDK Selection Decision Tree

```
1. What chip are you using?
   ├── CI1102/CI1103 → CI110X SDK (1st gen, see docs/软件开发/SDK/CI110X芯片SDK/)
   ├── CI1122 → CI112X SDK (1st gen, see docs/软件开发/SDK/CI112X芯片SDK/)
   ├── CI1301/CI1302/CI1303/CI1306
   │   ├── Need LLM/NLP/TTS/VPR? → CI130X_SDK_LLM_AIOT_2.2.7(1) or CI13XX_SDK_LLM_AIOT_2.1.2
   │   ├── Need advanced algorithm? → CI130X_SDK_ALG_V2.7.14 or CI130X_SDK_ALG_PRO_V2.6.3
   │   ├── Need two-mic? → CI130X_SDK_TwoMic_V1.0.2
   │   ├── Need IR? → CI130X_SDK_Offline_IR_V2.0.10
   │   └── Standard offline ASR → CI130X_SDK_Offline_V2.1.14 (latest stable)
   ├── CI1311/CI1312/CI13161/CI13162/CI13241/CI13242/CI13322/CI13642
   │   ├── Need CWSL? → CI13LC_SDK_V2.0.15 (cwsl_sample)
   │   ├── Need NL ASR? → CI13LC_SDK_V2.0.15 (nl_asr_sample)
   │   ├── Need IR? → CI13LC_SDK_IR_V2.0.15
   │   ├── Need NN encoder? → CI13LC_SDK_NN_ENC_V1.2.6
   │   └── Standard → CI13LC_SDK_2.1.5 (latest) or CI13LC_SDK_V2.0.15
   ├── CI2305/CI2306 → CI230X_wifi_combo_sdk_release_v1.1.1
   └── CI2312/CI23242
       └── CI23LC_SDK_BLE_V1.3.13 (latest BLE SDK)

2. Want unified SDK covering CI130X + CI13LC?
   ├── Need LLM/AIoT? → CI13XX_SDK_LLM_AIOT_2.1.2
   └── Need ASR + Algorithm? → CI13XX_SDK_ASR_ALG_V2.7.12
```

## SDK Component Comparison

| Component | CI130X Offline | CI13LC | CI13XX ASR_ALG | CI13XX LLM_AIOT | CI23LC BLE | CI230X WiFi |
|---|---|---|---|---|---|---|
| Offline ASR | Yes | Yes | Yes | Yes | Yes | Yes |
| CWSL (self-learning) | No | Yes | Yes | Yes | Yes | No |
| NL ASR (natural lang) | No | Yes | Yes | Yes | Yes | No |
| Multi-intent | No | Yes | Yes | Yes | Yes | No |
| AEC | Opt | No | Yes | Yes | No | No |
| Denoise | Opt | No | Yes | Yes | No | No |
| Beamforming | Opt | No | Yes | Yes | No | No |
| DOA | Opt | No | Yes | Yes | No | No |
| Dereverb | No | No | Yes | Yes | No | No |
| AGC/DRC/EQ | No | No | Yes | Yes | No | No |
| TTS | No | No | Yes | Yes | No | No |
| VPR (voiceprint) | No | No | No | Yes | No | No |
| NLP | No | No | No | Yes | No | No |
| BLE | No | No | No | Yes | Yes | Yes |
| Wi-Fi | No | No | No | No | No | Yes |
| IR | No | No | No | Yes | No | No |
| OTA | No | No | No | Yes | No | Yes |
| Sound event detect | No | No | Yes | No | No | No |
| LLM (cloud) | No | No | No | Yes | No | No |
