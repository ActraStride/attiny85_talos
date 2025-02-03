// ATTINY85

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#define F_CPU 32768UL
#include <util/delay.h>


volatile uint8_t debounce_timer = 0;
volatile uint8_t button_state = 0;

// Configurar Timer0 (inicialmente apagado)
void setup_timer() {
    TCCR0A |= (1 << WGM01);  // Modo CTC
    OCR0A = 3;               // 4096 Hz / 8 / 4 = 128 Hz (1 ms)
}

// Habilitar Timer0 (activar prescaler)
void enable_timer() {
    TCCR0B |= (1 << CS01);   // Prescaler 8
    TIMSK |= (1 << OCIE0A);  // Habilitar interrupción
}

// Deshabilitar Timer0   (desactivar prescaler)
void disable_timer() {
    TCCR0B = 0;              // Detener el temporizador
    TIMSK &= ~(1 << OCIE0A); // Deshabilitar interrupción
}


// Función para configurar el Timer1 en modo PWM
void setupServo() {
    DDRB |= (1 << PB1); // Configurar PB1 como salida (servo)
    TCCR1 |= (1 << PWM1A) | (1 << COM1A1); // Modo PWM y salida en PB1
    TCCR1 |= (1 << CS12); 
    OCR1C = 81; // Valor máximo del contador para 20 ms
}

void disableServo() {
    TCCR1 &= ~((1 << COM1A1) | (1 << PWM1A)); // Desactivar PWM
    DDRB &= ~(1 << PB1); // Configurar PB1 como entrada
    PORTB &= ~(1 << PB1); // Deshabilitar la resistencia de pull-up interna
}

void moveServo(uint16_t position) {
    uint16_t pulseWidth = 4 + ((position * 5) / 180);
    OCR1A = pulseWidth; // Establecer el ciclo de trabajo
}

void resetServo() {
    setupServo();
    moveServo(0);
    _delay_ms(1000); // Esperar 1 segundo para que el servo se mueva
    disableServo();
}

void setupLED() {
    DDRB |= (1 << PB0);   // Configurar PB0 como salida (LED)
    PORTB &= ~(1 << PB0); // Asegurar que el LED esté apagado inicialmente
}

void setupButton() {
    DDRB &= ~(1 << PB2);  // Configurar PB2 como entrada (botón)
    PORTB |= (1 << PB2);  // Habilitar resistencia de pull-up interna en PB2
    GIMSK |= (1 << PCIE); // Habilitar interrupciones por cambio de pin
    PCMSK |= (1 << PCINT2); // Habilitar interrupción en PB2 (PCINT2)
    sei(); // Habilitar interrupciones globales
}

void


// Interrupción del Timer0 (cada 1 ms)
ISR(TIMER0_COMPA_vect) {
    if (debounce_timer > 0) {
        debounce_timer--;
        if (debounce_timer == 0) {  // Si terminó el debounce
            if (!(PINB & (1 << PB2))) { // Si sigue presionado
                button_state = 1;  // Confirmar la acción
            }
            disable_timer();  // Desactivar el temporizador
        }
    }
}

// Interrupción por cambio de estado en PB2
ISR(PCINT0_vect) {
    if (debounce_timer == 0) {  // Si no hay debounce activo
        debounce_timer = 10;    // Iniciar debounce
        enable_timer();         // Activar Timer0 solo cuando se presiona
    }
}

int main(void) {
    setupLED();
    setupButton();
    setup_timer(); // Solo inicializamos pero no activamos el Timer0
    setupServo();
    resetServo();

    while (1) {
        if (button_state) { // Si el botón fue validado
            cli(); // Deshabilitar interrupciones globales
            button_state = 0; // Resetear estado
            PORTB ^= (1 << PB0); // Alternar LED
            _delay_ms(200);
            PORTB ^= (1 << PB0); // Alternar LED
            _delay_ms(200);
            PORTB ^= (1 << PB0); // Alternar LED
            _delay_ms(200);
            PORTB ^= (1 << PB0); // Alternar LED
            setupServo();
            moveServo(90);
            _delay_ms(7000); // Esperar 70 segundos bloqueando el sistema
            PORTB ^= (1 << PB0); // Alternar LED
            _delay_ms(200);
            PORTB ^= (1 << PB0); // Alternar LED
            _delay_ms(200);
            PORTB ^= (1 << PB0); // Alternar LED
            _delay_ms(200);
            PORTB ^= (1 << PB0); // Alternar LED
            setupServo();
            moveServo(0);
            _delay_ms(1000);
            disableServo();
            sei(); // Habilitar interrupciones globales nuevamente
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_mode();


        }
    }
}
