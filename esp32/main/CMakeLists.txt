idf_component_register(
    SRCS 
        "main.cpp" 
        "reading.cpp" 
        "lock.cpp" 
        "buttons.cpp" 
        "http_state.cpp" 
    INCLUDE_DIRS "." 
    PRIV_REQUIRES 
        esp_driver_gpio 
        esp_timer 
        esp_wifi 
        esp_event 
        esp_netif 
        esp_http_server 
        nvs_flash
)