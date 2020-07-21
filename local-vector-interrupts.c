/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <metal/button.h>
#include <metal/cpu.h>
#include <metal/interrupt.h>
#include <metal/led.h>
#include <metal/switch.h>
#include <stdio.h>

#define RTC_FREQ	32768

#ifndef metal_led_ld0red
#define metal_led_ld0red metal_led_none
#endif

#ifndef metal_led_ld0green
#define metal_led_ld0green metal_led_none
#endif

#ifndef metal_led_ld0blue
#define metal_led_ld0blue metal_led_none
#endif

#define led0_red metal_led_ld0red
#define led0_green metal_led_ld0green
#define led0_blue metal_led_ld0blue

#ifndef metal_button_btn0
#define metal_button_btn0 metal_button_none
#endif

#ifndef metal_button_btn1
#define metal_button_btn1 metal_button_none
#endif

#ifndef metal_button_btn2
#define metal_button_btn2 metal_button_none
#endif

#ifndef metal_button_btn3
#define metal_button_btn3 metal_button_none
#endif

#define but0 metal_button_btn0
#define but1 metal_button_btn1
#define but2 metal_button_btn2
#define but3 metal_button_btn3

#ifndef metal_switch_sw3
#define metal_switch_sw3 metal_switch_none
#endif

#define swch3 metal_switch_sw3

int sw_status = 99;

void display_instruction (void) {
    printf("\n");
    printf("SIFIVE, INC.\n!!\n");
    printf("\n");
    printf("E31/E51 Coreplex IP Eval Kit 'local-interrupt' Example.\n\n");
    printf("Buttons 0-3 and Switch 3 are enabled as local interrupt sources.\n");
    printf("A 1s debounce timer is used between these interupts.\n");
    printf("Pressing Buttons 0-3 toggle LEDs, while turn ON Switch 3 to exit.\n");
    printf("\n");
}

void metal_riscv_cpu_intc_mtip_handler(void) {
    // Disable Timer interrupt
    metal_cpu_disable_timer_interrupt();
    printf("Awaken\n");

    // Enable local IRQs
    metal_button_clear_interrupt(but0);
    metal_button_enable_interrupt(but0);
    metal_button_clear_interrupt(but1);
    metal_button_enable_interrupt(but1);
    metal_button_clear_interrupt(but2);
    metal_button_enable_interrupt(but2);
    metal_button_clear_interrupt(but3);
    metal_button_enable_interrupt(but3);
    metal_switch_enable_interrupt(swch3);
}

void debounce (void) {
    struct metal_cpu cpu = metal_cpu_get(metal_cpu_get_current_hartid());

    // Disable local IRQs
    metal_button_disable_interrupt(but0);
    metal_button_disable_interrupt(but1);
    metal_button_disable_interrupt(but2);
    metal_button_disable_interrupt(but3);
    metal_switch_disable_interrupt(swch3);

    printf("Sleep for %d more times\n", sw_status--);
    metal_cpu_set_mtimecmp(cpu, metal_cpu_get_mtime(cpu) + RTC_FREQ);

    // Enable Timer interrupt
    metal_cpu_enable_timer_interrupt();
}

void metal_button_btn0_interrupt_handler (void) {
    printf("Button 0 was pressed. Toggle Red LED.\n");
    metal_led_toggle(led0_red);
    debounce();  
}
void metal_button_btn1_interrupt_handler (void) {
    printf("Button 1 was pressed. Toggle Green LED.\n");
    metal_led_toggle(led0_green);
    debounce();
}
void metal_button_btn2_interrupt_handler (void) {
    printf("Button 2 was pressed. Toggle Blue LED.\n");
    metal_led_toggle(led0_blue);
    debounce();
}
void metal_button_btn3_interrupt_handler (void) {
    printf("Button 3 was pressed. No LED change!\n");
    debounce();
}

void metal_switch_sw3_interrupt_handler(void) {
    metal_switch_disable_interrupt(swch3);
    printf("Switch 3 is on!\n");
    sw_status = 0;
    printf("Time to exit!\n");
}

int main (void)
{
    // Lets get start with getting LEDs and turn only RED ON
    metal_led_enable(led0_red);
    metal_led_enable(led0_green);
    metal_led_enable(led0_blue);
    metal_led_on(led0_red);
    metal_led_off(led0_green);
    metal_led_off(led0_blue);
 
    // Lets enable the Buttons and Switch interrupts
    if (metal_button_enable_interrupt(but0) == -1) {
        printf("BTN0 interrupt enable failed\n");
        return 5;
    }
    if (metal_button_enable_interrupt(but1) == -1) {
        printf("BTN1 interrupt enable failed\n");
        return 5;
    }
    if (metal_button_enable_interrupt(but2) == -1) {
        printf("BTN2 interrupt enable failed\n");
        return 5;
    }
    if (metal_button_enable_interrupt(but3) == -1) {
        printf("BTN3 interrupt enable failed\n");
        return 5;
    }
    if (metal_switch_enable_interrupt(swch3) == -1) {
        printf("SW3 interrupt enable failed\n");
        return 5;
    }

    metal_cpu_enable_interrupts();

    display_instruction();

    while (sw_status) {
    }

    return sw_status;
}
