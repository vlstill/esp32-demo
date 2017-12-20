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

void setup() { }

template< size_t length >
struct ShiftRegister {

    ShiftRegister( Pin data, Pin clock ) : data( data ), clock( clock )
    {
        data.set_output();
        data.set_low();
        clock.set_output();
        clock.set_low();
        set_all_low();
    }

    void set_all( bool v ) {
        data.set( v );
        for ( int i = 0; i < length; ++i ) {
            clock.set_high();
            clock.set_low();
        }
    }

    void set_all_high() { set_all( true ); }
    void set_all_low() { set_all( false ); }

    void write() {
        for ( int i = 0; i < length; ++i ) {
            data.set( vpins[ length - i - 1 ] );
            clock.set_high();
            clock.set_low();
        }
    }

    void set( int p, bool v ) {
        vpins[ p ] = v;
        write();
    }

    struct Proxy {

        Proxy( ShiftRegister &sr, int ix ) : _sr( sr ), _ix( ix ) { }

        Proxy &operator=( bool v ) {
            _sr.set( _ix, v );
        }

      private:
        ShiftRegister &_sr;
        size_t _ix;
    };

    Proxy operator[]( size_t ix ) {
        return Proxy( *this, ix );
    }

    Pin data;
    Pin clock;
    std::array< bool, length > vpins;
};

template< int l >
void cycle( ShiftRegister< l > &sr ) {
    for ( ;; ) {
        sr.set_all_low();
        delay( 500 );
        sr.set_all_high();
        delay( 500 );
    }
}

void loop() {
    ShiftRegister< 8 > sr( 25, 26 );

    while ( true ) {
        for ( int i = 7; i >= 0; --i ) {
            sr[ i ] = true;
            delay( 100 );
            sr[ i ] = false;
        }
    }

    while ( true ) { }
}
