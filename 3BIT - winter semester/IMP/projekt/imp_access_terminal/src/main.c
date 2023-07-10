/**********************************************************/
/*                                                        */
/* File: main.c                                           */
/* Author: David Chocholaty <xchoch09@stud.fit.vutbr.cz>  */
/* Project: Project for the course IMP                    */
/*          - M - ESP32: Access terminal                  */
/* Description: Access terminal using a terminal          */
/* with a keyboard and two led diodes (open / closed).    */
/*                                                        */
/**********************************************************/

#include <esp_task_wdt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

// The interrupt flag.
#define ESP_INTR_FLAG_DEFAULT 0

/******************************/
/*          TIMEOUTS          */
/******************************/

// The watchdog timeout value for the initialization (in seconds).
#define WATCHDOG_TIMEOUT_S 2
// The debounce time for keypad buttons (in milliseconds). 
#define DEBOUNCE_TIME_MS 10

/******************************/
/*         LED DIODES         */
/******************************/

// The pin for the green led.
#define GREEN_LED_PIN GPIO_NUM_26
// The pin for the red led.
#define RED_LED_PIN GPIO_NUM_3

/******************************/
/*        MATRIX KEYPAD       */
/******************************/
// The pin for the first row.
#define MATRIX_KP_ROW_1 GPIO_NUM_12
// The pin for the second row.
#define MATRIX_KP_ROW_2 GPIO_NUM_25
// The pin for the third row.
#define MATRIX_KP_ROW_3 GPIO_NUM_17
// The pin for the fourth row.
#define MATRIX_KP_ROW_4 GPIO_NUM_27
// The pin for the first column.
#define MATRIX_KP_COL_1 GPIO_NUM_14
// The pin for the second column.
#define MATRIX_KP_COL_2 GPIO_NUM_13
// The pin for the third column.
#define MATRIX_KP_COL_3 GPIO_NUM_16

// Default configuration for the matrix keypad.
#define MATRIX_KP_DEFAULT_CONFIG()    \
{                                     \
    .row_num = 4,                     \
    .col_num = 3,                     \
    .rows = (int[]) {MATRIX_KP_ROW_1, \
        MATRIX_KP_ROW_2,              \
        MATRIX_KP_ROW_3,              \
        MATRIX_KP_ROW_4},             \
    .cols = (int[]) {MATRIX_KP_COL_1, \
        MATRIX_KP_COL_2,              \
        MATRIX_KP_COL_3}              \
}

/******************************/
/*          PASSWORD          */
/******************************/

// Default configuration for the password handling structure.
#define PASSWORD_DEFAULT_CONFIG() \
{                                 \
    .change_password_auth_state = false, \
    .change_password_auth_flag = false, \
    .change_password_write_state = false, \
    .password_length = 4,         \
    .password_position = 0,       \
    .password_reference = (char[]) {'1', '2', '3', '4', '\0'}, \
    .password_data = (char[]) {'x', 'x', 'x', 'x', '\0'}  \
}

/******************************/
/*        MATRIX KEYPAD       */
/******************************/
typedef struct matrix_kp {
    const int *rows;
    const int *cols;
    uint32_t row_num;
    uint32_t col_num;
} matrix_kp_t;

/******************************/
/*          PASSWORD          */
/******************************/
typedef struct password {
    char *password_reference;
    char *password_data;
    uint32_t password_length;
    uint32_t password_position;
    bool change_password_auth_state;
    bool change_password_auth_flag;
    bool change_password_write_state;
} password_t;

static password_t password = PASSWORD_DEFAULT_CONFIG();

// Queue to handle gpio event from isr.
static QueueHandle_t gpio_evt_queue = NULL;

// Tag for logging.
static const char* TAG = "AccessTerminal";

/******************************/
/*     SIGNALIZATION CODES    */
/******************************/

/*
 * Signalization code for the access allowed (the green LED lights up
 * for 1000 ms).
 */
static void access_allowed_signalization(void)
{
    // Register task to be associated with a watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    ESP_ERROR_CHECK(gpio_set_level(RED_LED_PIN, 0));
    ESP_ERROR_CHECK(gpio_set_level(GREEN_LED_PIN, 1));

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    // Explicitly feed the watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_reset());

    ESP_ERROR_CHECK(gpio_set_level(RED_LED_PIN, 1));
    ESP_ERROR_CHECK(gpio_set_level(GREEN_LED_PIN, 0));

    // Unsubscribe the task from the watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
}

/*
 * Signalization code for the access denied (the red LED flashes three times).
 */
static void access_denied_signalization(void)
{
    // Register task to be associated with a watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(gpio_set_level(RED_LED_PIN, 0));
        vTaskDelay(120 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(gpio_set_level(RED_LED_PIN, 1));
        vTaskDelay(120 / portTICK_PERIOD_MS);

        // Explicitly feed the watchdog.
        ESP_ERROR_CHECK(esp_task_wdt_reset());
    }

    // Unsubscribe the task from the watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
}

/*
 * Signalization code for a successful password change (the green
 * and the red LEDs flash three times).
 */
static void password_changed_signalization(void)
{
    // Register task to be associated with a watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(gpio_set_level(RED_LED_PIN, 0));
        ESP_ERROR_CHECK(gpio_set_level(GREEN_LED_PIN, 0));
        vTaskDelay(120 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(gpio_set_level(RED_LED_PIN, 1));
        ESP_ERROR_CHECK(gpio_set_level(GREEN_LED_PIN, 1));
        vTaskDelay(120 / portTICK_PERIOD_MS);

        // Explicitly feed the watchdog.
        ESP_ERROR_CHECK(esp_task_wdt_reset());
    }

    ESP_ERROR_CHECK(gpio_set_level(GREEN_LED_PIN, 0));

    // Unsubscribe the task from the watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
}

/*
 * The gpio isr handler for all matrix rows.
 */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);    
}

/*
 * The function for debouncing the matrix keypad button press.
 */
static int debounce(uint32_t io_num)
{
    // Register task to be associated with a watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    int previous_level;
    int current_level;

    previous_level = gpio_get_level(io_num);

    for (int i = 0; i < DEBOUNCE_TIME_MS; i++) {
        vTaskDelay(1 / portTICK_PERIOD_MS); // Wait for 1 millisecond.

        current_level = gpio_get_level(io_num);

        if (current_level != previous_level) {            
            i = 0; // Start again.
            previous_level = current_level;
        }

        // Explicitly feed the watchdog.
        ESP_ERROR_CHECK(esp_task_wdt_reset());
    }

    // Unsubscribe the task from the watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));

    return current_level;
}

/*
 * The callback for the matrix keypad row isr.
 */
static void matrix_kp_row_isr_callback(void* arg)
{    
    uint32_t io_num;
    char value;

    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY) == pdTRUE) {           
            if (debounce(io_num) == 0) {
                // Switch the matrix keypad gpio levels to check in which column
                // the pressed pad is.
                ESP_ERROR_CHECK(gpio_set_level(io_num, 0));
                ESP_ERROR_CHECK(gpio_set_level(MATRIX_KP_COL_1, 1));
                ESP_ERROR_CHECK(gpio_set_level(MATRIX_KP_COL_2, 1));
                ESP_ERROR_CHECK(gpio_set_level(MATRIX_KP_COL_3, 1));
                
                if (gpio_get_level(MATRIX_KP_COL_1) == 0) {                    
                    switch (io_num) {
                        case MATRIX_KP_ROW_1:
                            value = '1';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;
                        
                        case MATRIX_KP_ROW_2:
                            value = '4';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;

                        case MATRIX_KP_ROW_3:
                            value = '7';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;

                        case MATRIX_KP_ROW_4:
                            password.password_position = 0;                         
                            password.change_password_auth_state = true;

                            // If * was pressed when entering new password,
                            // reset the write state.
                            if (password.change_password_write_state) {
                                password.change_password_write_state = false;
                                password.change_password_auth_state = false;
                                password.password_position = 0;

                                access_denied_signalization();
                            }

                            ESP_LOGI(TAG, "value: *");
                            break;

                        default:
                            break;
                    }

                    // Loop here while pressed until the user lets go.
                    while (gpio_get_level(MATRIX_KP_COL_1) == 0) {
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }

                } else if (gpio_get_level(MATRIX_KP_COL_2) == 0) {
                    switch (io_num) {
                        case MATRIX_KP_ROW_1:
                            value = '2';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;
                        
                        case MATRIX_KP_ROW_2:
                            value = '5';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;

                        case MATRIX_KP_ROW_3:
                            value = '8';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;

                        case MATRIX_KP_ROW_4:
                            value = '0';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;

                        default:
                            break;
                    }

                    // Loop here while pressed until the user lets go.
                    while (gpio_get_level(MATRIX_KP_COL_2) == 0) {
                        vTaskDelay(10 / portTICK_PERIOD_MS);           
                    }

                } else if (gpio_get_level(MATRIX_KP_COL_3) == 0) {
                    switch (io_num) {
                        case MATRIX_KP_ROW_1:
                            value = '3';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;
                        
                        case MATRIX_KP_ROW_2:
                            value = '6';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;

                        case MATRIX_KP_ROW_3:
                            value = '9';

                            password.password_data[password.password_position] = value;
                            password.password_position++;

                            ESP_LOGI(TAG, "value: %c", value);
                            break;

                        case MATRIX_KP_ROW_4:
                            if (password.change_password_auth_flag) {
                                // The # pad was pressed after the successful
                                // password check when setting the new password.
                                // The # pad has to be pressed as a first pad
                                // after the successful password check. Otherwise,
                                // the auth flag is deleted and the user has to start
                                // again from the start.

                                password.change_password_auth_flag = false;
                                password.change_password_write_state = true;
                            } else {
                                // The # pad was pressed independently
                                // to setting the new password or the user
                                // did not pressed the # pad as a first pad
                                // after the successful password check, so,
                                // the auth flag is deleted.

                                password.password_position = 0;

                                // If # was pressed when entering new password,
                                // reset write state.
                                if (password.change_password_write_state) {
                                    password.change_password_write_state = false;                                    
                                }

                                access_denied_signalization();
                            }

                            ESP_LOGI(TAG, "value: #");                            
                            break;

                        default:
                            break;
                    }

                    // Loop here while pressed until the user lets go.
                    while (gpio_get_level(MATRIX_KP_COL_3) == 0) {
                        vTaskDelay(10 / portTICK_PERIOD_MS);                
                    }

                }                

                // Set the matrix keypad gpio levels back.
                ESP_ERROR_CHECK(gpio_set_level(io_num, 1));
                ESP_ERROR_CHECK(gpio_set_level(MATRIX_KP_COL_1, 0));
                ESP_ERROR_CHECK(gpio_set_level(MATRIX_KP_COL_2, 0));
                ESP_ERROR_CHECK(gpio_set_level(MATRIX_KP_COL_3, 0));

                // Reset flag after the first key after authorization
                // was entered.             
                if (password.change_password_auth_flag) {
                    password.change_password_auth_flag = false;
                    password.change_password_auth_state = false;
                    password.password_position = 0;                    

                    access_denied_signalization();
                }                

                // The length of the entered password is the same as the length
                // of the checked password.
                if (password.password_position == password.password_length) {
                    password.password_position = 0;

                    // Check if the user tries to set a new password. 
                    if (password.change_password_write_state) {
                        // The user entered the new password which should be set
                        // as a new one. So, this state is still valid and
                        // the new password will be set.
                        
                        password.change_password_write_state = false;

                        // Set the new password
                        strcpy(password.password_reference, password.password_data);

                        ESP_LOGI(TAG, "password has been successfully changed");

                        password_changed_signalization();

                    } else {
                        // Test if the entered password is correct.
                        if (strcmp(password.password_reference, password.password_data) == 0) {
                            // The entered password is correct.

                            // Check if the user wants to set the new password.
                            if (password.change_password_auth_state) {                            
                                // Successful password check when changing password.
                                password.change_password_auth_state = false;
                                password.change_password_auth_flag = true;

                                ESP_LOGI(TAG, "successful authentication when changing the password");
                            } else {
                                // Otherwise, the user just to want to enter.
                                ESP_LOGI(TAG, "passwords match");

                                access_allowed_signalization();                                
                            }
                            
                        } else {
                            // The entered password is incorrect.

                            // Check if the user wants to set the new password (auth state).
                            if (password.change_password_auth_state) {
                                // Delete the auth state.
                                password.change_password_auth_state = false;
                            }
                            
                            // The entered password is incorrect.
                            ESP_LOGI(TAG, "passwords are different");

                            access_denied_signalization();                                                    
                        }
                    }                                  
                }
            }
        }
    }

    // Delete the current task.
    vTaskDelete(NULL);
}

/*
 * The function for the matrix keypad configuration.
 */
static void configure_matrix_keypad(const matrix_kp_t* matrix_keypad)
{
    // Register task to be associated with a watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    // Initialize the config structure.
    gpio_config_t io_conf = {                
        // Set as input mode.
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        // Disable pull-down mode.
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        // Enable pull-up mode.
        .pull_up_en = GPIO_PULLUP_ENABLE
    };    

    // Config matrix pad cols.
    io_conf.intr_type = GPIO_INTR_DISABLE;

    for (int i = 0; i < matrix_keypad->col_num; i++) {
        io_conf.pin_bit_mask = 1ULL << matrix_keypad->cols[i];
        ESP_ERROR_CHECK(gpio_config(&io_conf));

        // Set cols to low.
        ESP_ERROR_CHECK(gpio_set_level(matrix_keypad->cols[i], 0));

        // Explicitly feed the watchdog.
        ESP_ERROR_CHECK(esp_task_wdt_reset());
    }

    // Config matrix pad rows.
    io_conf.intr_type = GPIO_INTR_NEGEDGE;

    for (int i = 0; i < matrix_keypad->row_num; i++) {
        io_conf.pin_bit_mask = 1ULL << matrix_keypad->rows[i];
        ESP_ERROR_CHECK(gpio_config(&io_conf));

        // Set rows to high.
        ESP_ERROR_CHECK(gpio_set_level(matrix_keypad->rows[i], 1));

        // Explicitly feed the watchdog.
        ESP_ERROR_CHECK(esp_task_wdt_reset());
    }

    // Create a queue to handle gpio event from isr.
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // Start gpio task.    
    xTaskCreate(matrix_kp_row_isr_callback, "matrix_kp_row_isr_callback", 2048, NULL, 5, NULL);

    // Install gpio isr service.
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));

    // Hook isr handler for specific gpio pin.
    ESP_ERROR_CHECK(gpio_isr_handler_add(MATRIX_KP_ROW_1, gpio_isr_handler, (void*) MATRIX_KP_ROW_1));
    ESP_ERROR_CHECK(gpio_isr_handler_add(MATRIX_KP_ROW_2, gpio_isr_handler, (void*) MATRIX_KP_ROW_2));
    ESP_ERROR_CHECK(gpio_isr_handler_add(MATRIX_KP_ROW_3, gpio_isr_handler, (void*) MATRIX_KP_ROW_3));
    ESP_ERROR_CHECK(gpio_isr_handler_add(MATRIX_KP_ROW_4, gpio_isr_handler, (void*) MATRIX_KP_ROW_4));

    // Unsubscribe the task from the watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
}

/*
 * Function for the leds configuration.
 */
static void configure_leds(void)
{
    // Register task to be associated with a watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    gpio_reset_pin(RED_LED_PIN);
    gpio_reset_pin(GREEN_LED_PIN);

    // Initialize the config structure.
    gpio_config_t io_conf = {                
        // Set as input mode.
        .mode = GPIO_MODE_OUTPUT,
        // Disable pull-down mode.
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        // Enable pull-up mode.
        .pull_up_en = GPIO_PULLUP_DISABLE,
        // Disable interrupt.
        .intr_type = GPIO_INTR_DISABLE
    };

    io_conf.pin_bit_mask = 1ULL << RED_LED_PIN;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    io_conf.pin_bit_mask = 1ULL << GREEN_LED_PIN;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_ERROR_CHECK(gpio_set_level(RED_LED_PIN, 1));
    ESP_ERROR_CHECK(gpio_set_level(GREEN_LED_PIN, 0));

    // Unsubscribe the task from the watchdog.
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
}

/*
 * The main function of the application.
 */
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initializing the task watchdog subsystem with an interval of 10 seconds.
    // Set as do not panic -> false.
    ESP_ERROR_CHECK(esp_task_wdt_init(WATCHDOG_TIMEOUT_S, false));

    matrix_kp_t matrix_keypad = MATRIX_KP_DEFAULT_CONFIG();

    configure_leds();
    configure_matrix_keypad(&matrix_keypad);        

    while(1) {} // Preventing return from main.
}
