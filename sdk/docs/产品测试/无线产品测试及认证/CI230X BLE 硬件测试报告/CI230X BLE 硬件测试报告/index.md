<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20BLE%20%E7%A1%AC%E4%BB%B6%E6%B5%8B%E8%AF%95%E6%8A%A5%E5%91%8A/CI230X%20BLE%20%E7%A1%AC%E4%BB%B6%E6%B5%8B%E8%AF%95%E6%8A%A5%E5%91%8A/ -->

# CI230X系列芯片硬件测试报告

## CI230X系列芯片 BLE 硬件测试报告

表1 测试环境

|  |  |
| --- | --- |
| 硬件版本 | CI-E05GT02S V4.0CI-E06GT02S V4.0 |
| 软件版本 | hci\_CI230X\_ble\_hci\_test\_(d573cdb2).bin |
| 匹配参数 | 3.3pF//1.5pF-1nH |

表2 BLE 硬件测试报表

| 测试项目 | 测试条件 | | 指标要求 | 测试结果Sample#1 | 测试结果Sample#2 | 测试结论 |
| --- | --- | --- | --- | --- | --- | --- |
| 功率（TX) | T(25℃)Vtyp=3.3V | 2402MHz | ≤10dBm | 3.6 | 2.7 | PASS |
| 2440MHz | ≤10dBm | 3.7 | 2.1 | PASS |
| 2480MHz | ≤10dBm | 4.2 | 3.4 | PASS |
| T(105℃)Vtyp=3.3V | 2402MHz | ≤10dBm | 3.1 | 2.7 | PASS |
| 2440MHz | ≤10dBm | 3.6 | 3.3 | PASS |
| 2480MHz | ≤10dBm | 3.7 | 3.5 | PASS |
| 灵敏度（RX） | 1M | 2402MHz | ≤-87dBm | -91 | -90.5 | PASS |
| 2440MHz | ≤-87dBm | -91 | -90.5 | PASS |
| 2480MHz | ≤-87dBm | -90 | -89.5 | PASS |
| 125K | 2402MHz | ≤-87dBm | -96 | -95 | PASS |
| 2440MHz | ≤-87dBm | -96 | -95 | PASS |
| 2480MHz | ≤-87dBm | -94 | -94 | PASS |
| 频偏（drift） | T(25℃) | 2402MHz | ≤±20ppm | 5 | 5.4 | PASS |
| 2440MHz | ≤±20ppm | 5.7 | 6 | PASS |
| 2480MHz | ≤±20ppm | 6.6 | 6.7 | PASS |
| T(105℃) | 2402MHz | ≤±20ppm | -2.5 | -4.1 | PASS |
| 2440MHz | ≤±20ppm | -3.4 | -4.1 | PASS |
| 2480MHz | ≤±20ppm | -4.4 | -6.1 | PASS |
| In-band emissions带内频谱辐射(dBm) | 2406MHzCH2 | +2MHz | ≤-20dB | -55.1 | -51.5 | PASS |
| -2MHz | ≤-20dB | -50.9 | -51.6 | PASS |
| ≥+3MHz | ≤-30dB | -50.8 | -53.5 | PASS |
| ≤-3MHz | ≤-30dB | -53.9 | -53.7 | PASS |
| 2440MHzCH19 | +2MHz | ≤-20dB | -51.8 | -52.5 | PASS |
| -2MHz | ≤-20dB | -51.5 | -52.5 | PASS |
| ≥+3MHz | ≤-30dB | -53.9 | -54.3 | PASS |
| ≤-3MHz | ≤-30dB | -54.1 | -53.9 | PASS |
| 2476MHzCH37 | +2MHz | ≤-20dB | -50.8 | -51.6 | PASS |
| -2MHz | ≤-20dB | -50.2 | -51.4 | PASS |
| ≥+3MHz | ≤-30dB | -53.5 | -53.2 | PASS |
| ≤-3MHz | ≤-30dB | -53.4 | -52.6 | PASS |
| Modulation characteristics调制特性(kHz) | 2402MHzCH0 | Δf1avg | 225~275kHz | 253 | 253 | PASS |
| Δf2max | ≥185kHz | 222 | 224 | PASS |
| Δf2avg | ≥185kHz | 209 | 212 | PASS |
| 2440MHzCH19 | Δf1avg | 225~275kHz | 253 | 253 | PASS |
| Δf2max | ≥185kHz | 229 | 232 | PASS |
| Δf2avg | ≥185kHz | 217 | 219 | PASS |
| 2480MHzCH39 | Δf1avg | 225~275kHz | 253 | 252 | PASS |
| Δf2max | ≥185kHz | 232 | 235 | PASS |
| Δf2avg | ≥185kHz | 220 | 222 | PASS |
| Carrier frequency offset and drift载波频率偏移和频率漂移(kHz) | 2402MHzCH0 | Freq Accuracy | -150~+150kHz | -70.4 | -62.4 | PASS |
| Freq Offset | ≤150kHz | -70.4 | -62.4 | PASS |
| Freq Drift | ≤±20kHz | 12 | 13 | PASS |
| Initial Freq Drift | ≤±20kHz | 11.2 | 12.4 | PASS |
| Max Drift Rate | ≤±20kHz | 0.1 | 0.2 | PASS |
| 2440MHzCH19 | Freq Accuracy | -150~+150kHz | -73.3 | -65.2 | PASS |
| Freq Offset | ≤150kHz | -73.3 | -65.2 | PASS |
| Freq Drift | ≤±20kHz | 14 | 14.7 | PASS |
| Initial Freq Drift | ≤±20kHz | 13.2 | 14 | PASS |
| Max Drift Rate | ≤±20kHz | 0.1 | 0.1 | PASS |
| 2480MHzCH39 | Freq Accuracy | -150~+150kHz | -76.3 | -68 | PASS |
| Freq Offset | ≤150kHz | -76.3 | -68 | PASS |
| Freq Drift | ≤±20kHz | 16.1 | 16.4 | PASS |
| Initial Freq Drift | ≤±20kHz | 15.4 | 15.6 | PASS |
| Max Drift Rate | ≤±20kHz | 0.2 | 0.1 | PASS |