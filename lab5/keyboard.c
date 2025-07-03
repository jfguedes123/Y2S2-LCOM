#include "keyboard.h"
#include "i8042.h"
#define KEYBOARD_IRQ 1

int keyboard_hook_id = 1;

int(kbd_subscribe_int)(uint8_t *bit_no){
    if (bit_no == NULL) return 1;
    *bit_no = keyboard_hook_id;
    if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_hook_id)) return 1;
    return 0;
}

int(kbd_unsubscribe_int)(){
    if (sys_irqrmpolicy(&keyboard_hook_id)) return 1;
    return 0;
}

uint8_t scancode = 0;

void (kbc_ih)() {
    uint8_t status;
    int attempts = 3;
    while (attempts > 0) {
        if (util_sys_inb(KBC_STATUS_REG, &status)) return;
        if ((status & OUT_BUFFER)) { 
            if ((status & ERRORS)) {
                return;
            } else {
                if (util_sys_inb(KBC_WRITE_CMD, &scancode))
                return;
            }
        }
        attempts--;
    }
    return;
}

//int (kbc_write_command)(uint8_t address, uint8_t command) {}

//int (kbc_read_out_buf)(uint8_t buffer) {}

//Ciclo for
//Leitura de status reg
//Teste de IBF/OBF
//Protegido por sleeps
