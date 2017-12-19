#include <Arduino.h>
#include <iostream>

struct Pin {

    Pin( int id ) : id( id ) { }

    void set_output() { pinMode( id, OUTPUT ); }
    void set_input() { pinMode( id, INPUT ); }

    void set( bool v ) { digitalWrite( id, v ? HIGH : LOW ); }
    void set_high() { set( true ); }
    void set_low() { set( false ); }

  private:
    int id = 0;
};

Pin pins[] = { 32, 33, 25, 26, 27 };

void setup() {
    std::cout << "init\n";
    for ( auto p : pins )
        p.set_output();
}

void loop() {
    for ( unsigned i = 0; ; i++ ) {
        std::cout << "loop " << i << std::endl;
        for ( int j = 0; j < sizeof( pins ) / sizeof( int ); ++j ) {
            auto p = pins[j];
            p.set( i & (1 << j) ? HIGH : LOW );
        }
        delay( 500 ); // ms
    }
}
