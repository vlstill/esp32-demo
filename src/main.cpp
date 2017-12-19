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

  private:
    int id = 0;
};

std::array< Pin, 5 > pins = { 32, 33, 25, 26, 27 };
std::array< Pin, 2 > ins = { 34, 35 };

void setup() {
    std::cout << "init\n";
    for ( auto p : pins )
        p.set_output();
    for ( auto p : ins )
        p.set_input_pulldown();
}

void loop() {
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
}
