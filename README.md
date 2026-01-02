# S/PDIF driver for Pico

Pico用S/PDIFドライバです。  
ドライバのオリジナルは、amedasさんのesp_a2dp_sink_spdifです。  
https://github.com/amedes/esp_a2dp_sink_spdif.git  
  
ドライバを使用したサンプルプログラムとして、BT-S/PDIF変換器を同梱しています。  
サンプルプログラムのオリジナルはpico-examplesに含まれるa2dp_sink_demoです。  
a2dp_sink_demoのI2S出力をS/PDIF出力に変更したものです。  

![image](pico_a2dp_sink_spdif_wire1.jpg)

### Usage

1. 必要なもの
  1. Raspberry Pi Pico2 W / Pico2 WH
  2. 光デジタル通信コネクタ（例：[PLT133/T10W](https://akizukidenshi.com/catalog/g/g109598/)
2. 配線
  ![image](pico_a2dp_sink_spdif_wire2.jpg)

[Youtube](https://www.youtube.com/watch?v=362HxFF8WNY)に移植作業のライブ配信動画を掲載しています。

### Build

% git clone https://github.com/Ucchi98/pico_a2dp_sink_spdif.git  
% cd pico_a2dp_sink_spdif  
% cmake -S . -B build -DPICO_BOARD=pico2_w -DPICO_SDK_PATH=pico-sdkのディレクトリ  
% cd build  
% make  

作成には以下のライブラリを使用させていただきました。

- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [pico-examples](https://github.com/raspberrypi/pico-examples)
- [esp_a2dp_sink_spdif](https://github.com/Ucchi98/esp_a2dp_sink_spdif)
- [esp_a2dp_sink_spdif](https://github.com/amedes/esp_a2dp_sink_spdif)(Original)

## Authors

- **Ucchi98** - *S/PDIF driver for Pico* - [Ucchi98](https://github.com/Ucchi98)

## License

MIT License - see the [LICENSE](LICENSE.txt) file for details


