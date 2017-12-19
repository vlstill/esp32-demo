#include <Arduino.h>
#include <iostream>
#include <array>

struct Pin {

    Pin( int id ) : id( id ) { }

    void set_output() { pinMode( id, OUTPUT ); }
    void set_input_pulldown() { pinMode( id, INPUT_PULLDOWN ); }
    void set_input_pullup() { pinMode( id, INPUT_PULLUP ); }

    void set( bool v ) { digitalWrite( id, v ? HIGH : LOW ); }
    void set_high() { set( true ); }
    void set_low() { set( false ); }

    bool get() { return digitalRead( id ); }

    int pin() { return id; }

  private:
    int id = 0;
};

std::array< Pin, 5 > pins = { 32, 33, 25, 26, 27 };
std::array< Pin, 2 > ins = { 34, 35 };

void set( bool v ) {
    for ( auto p : pins )
        p.set( v );
}

void setup() {
    std::cout << "init\n";
    for ( auto p : pins )
        p.set_output();
    for ( auto p : ins )
        p.set_input_pulldown();

    ledcSetup( 0, 20000 /* 20 kHz */, 16 /* 2^16 steps */ );
    for ( auto p : pins )
    ledcAttachPin( p.pin(), 0 );
    ledcWrite( 0, 65536 / 2 ); /* 50 % */
}

void loop() {
    const int ANALOG_IN = 36;
    const int ANALOG_MAX = 4096;

#if 0
    while ( true ) {
        int v = analogRead( ANALOG_IN ) * 256 / ANALOG_MAX;
        ledcWrite( 0, v * v );
        std::cout << v << std::endl;
    }
#endif

    int top = 256;
    while ( true ) {
        int d = (analogRead( ANALOG_IN ) * 10000 / ANALOG_MAX) + 1;
        std::cout << d << std::endl;
        int v = 0;
        while ( v <= top ) {
            int d = (analogRead( ANALOG_IN ) * 10000 / ANALOG_MAX) + 1;
            ++v;
            ledcWrite( 0, v * v );
            delayMicroseconds( d );
        }
        while ( v >= 0 ) {
            int d = (analogRead( ANALOG_IN ) * 10000 / ANALOG_MAX) + 1;
            ledcWrite( 0, v * v );
            delayMicroseconds( d );
            --v;
        }
    }
#if 0
    int top = 1000;
    while ( true ) {
        int v = 0;
        while ( v <= top ) {
            ++v;
            for ( int i = 0; i < top; ++i ) {
                set( i < v );
                delayMicroseconds( 1 );
            }
        }
        while ( v >= 0 ) {
            for ( int i = 0; i < top; ++i ) {
                set( i < v );
                delayMicroseconds( 1 );
            }
            --v;
        }
    }
#endif

#if 0
    for ( unsigned i = 0; ; i++ ) {
        std::cout << "loop " << i << std::endl;
        for ( int j = 0; j < pins.size(); ++j ) {
            auto p = pins[j];
            p.set( i & (1 << j) ? HIGH : LOW );
        }
        while ( !ins[0].get() && !ins[1].get() ) {
            delay( 100 );
        }
        if ( ins[0].get() ) {
            while ( ins[0].get() )
                delay( 100 );
            continue;
        }
        if ( ins[1].get() ) {
            while ( ins[1].get() )
                delay( 100 );
            i = -1;
        }
    }
#endif
}
