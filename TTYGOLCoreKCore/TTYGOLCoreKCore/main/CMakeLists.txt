idf_component_register(
                    SRCS 
						main.c 
						L_Core/lcd/lcd.c
						L_Core/ui/ui.c
						L_Core/ui/ui_log.c
						L_Core/bluetooth/ble.c
                	    L_Core/bluetooth/ble-server.c                	
						K_Core/K_Core.c
						K_Core/taskmanager.c
						K_Core/K_Core.c
						K_Core/taskmanager.c						
						K_Core/adc/adc.c
						K_Core/adc/adc-table.c
						K_Core/communication/communication.c
						K_Core/communication/parser.c
						K_Core/execution/bcode.c
						K_Core/execution/cmdprocessor.c
						K_Core/tools/tools.c
					INCLUDE_DIRS "."
                    REQUIRES 
						LovyanGFX
						lvgl
						app_update
						esp_https_ota
						esp_event
						esp_timer
						spi_flash
						esp_wifi
						esp_partition
						esp_hw_support
						bt
						fatfs
						sdmmc)

component_compile_options(-fexceptions -x c++ -O0)
set_property(TARGET ${COMPONENT_TARGET} PROPERTY CXX_STANDARD 17)
