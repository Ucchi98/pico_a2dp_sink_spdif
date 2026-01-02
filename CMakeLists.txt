cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_a2dp_sink_spdif C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

add_executable(pico_a2dp_sink_spdif
    a2dp_sink_demo.c
    btstack_audio_pico.c
    picow_bt_example_background.c
    picow_bt_example_common.c
    spdif.c
)

pico_generate_pio_header(pico_a2dp_sink_spdif
    ${CMAKE_CURRENT_LIST_DIR}/spdif.pio
)

target_include_directories(pico_a2dp_sink_spdif INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/
)

target_link_libraries(pico_a2dp_sink_spdif PRIVATE
    pico_stdlib
    hardware_pio
    hardware_dma
    hardware_irq
    pico_btstack_cyw43
    pico_cyw43_arch_threadsafe_background
    pico_btstack_classic
    pico_btstack_sbc_decoder
)

target_compile_definitions(pico_a2dp_sink_spdif PRIVATE
    # compile time configuration of S/PDIF
    PICO_DEFAULT_UART=0
    PICO_DEFAULT_UART_TX_PIN=12
    PICO_DEFAULT_UART_RX_PIN=13
    CONFIG_SPDIF_DATA_PIN=9
    CONFIG_SPDIF_QUEUE_LEN=4
    TEST_AUDIO=1
    CYW43_LWIP=0
    CYW43_ENABLE_BLUETOOTH=1
)

target_include_directories(pico_a2dp_sink_spdif PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_a2dp_sink_spdif)
