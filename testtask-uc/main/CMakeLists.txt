add_executable(testtask-uc
         main.c
         )
pico_enable_stdio_usb(testtask-uc 1)
pico_enable_stdio_uart(testtask-uc 0)

#pico_generate_pio_header(zoner_full ${CMAKE_CURRENT_LIST_DIR}/uart_pio.pio)

#add_subdirectory(KBD)
 # Import pico_stdlib which imports commonly used features
 target_link_libraries(testtask-uc pico_stdlib hardware_pwm hardware_uart)

 # Create map/bin/hex file etc.
 pico_add_extra_outputs(testtask-uc)

 # Add path via pico_set_program_url CMAKE
 example_auto_set_url(testtask-uc)