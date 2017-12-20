#include <Arduino.h>
#include <iostream>
#include <array>

#include "driver/timer.h"

extern "C" int ets_printf( const char *, ... );

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

Pin status( 27 );
bool st = true;

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

    void _set( int p, bool v ) {
        vpins[ p ] = v;
    }

    void set( int p, bool v ) {
        _set( p, v );
        write();
    }

    struct Proxy {

        Proxy( ShiftRegister &sr, int ix ) : _sr( sr ), _ix( ix ) { }

        Proxy &operator=( bool v ) {
            _sr.set( _ix, v );
        }

        operator bool() const { return _sr[ _ix ]; }

      private:
        ShiftRegister &_sr;
        size_t _ix;
    };

    Proxy operator[]( size_t ix ) {
        return Proxy( *this, ix );
    }

//  private:
    Pin data;
    Pin clock;
    std::array< bool, length > vpins;
};

void IRAM_ATTR _interrupt( void *self );

template< size_t length >
struct PWMShiftRegister {

    const uint64_t DIVIDER = 2;
    const uint64_t SCALE = TIMER_BASE_CLK / DIVIDER;
    const uint64_t PWM_FREQ = 100;
    const uint64_t STEPS = 256;
    const uint64_t STEP = SCALE / PWM_FREQ / STEPS;

    PWMShiftRegister( Pin data, Pin clock ) : sr( data, clock ) {

        /* Select and initialize basic parameters of the timer */
		timer_config_t config;
		config.divider = DIVIDER;
		config.counter_dir = TIMER_COUNT_UP;
		config.counter_en = TIMER_PAUSE;
		config.alarm_en = TIMER_ALARM_EN;
		config.intr_type = TIMER_INTR_LEVEL;
		config.auto_reload = true;
		timer_init( TIMER_GROUP_0, TIMER_0, &config );

		/* Timer's counter will initially start from value below.
		   Also, if auto_reload is set, this value will be automatically reload on alarm */
		timer_set_counter_value( TIMER_GROUP_0, TIMER_0, 0ULL );

        /* Configure the alarm value and the interrupt on alarm. */
	    timer_set_alarm_value( TIMER_GROUP_0, TIMER_0, STEP );
		timer_enable_intr( TIMER_GROUP_0, TIMER_0 );
		timer_isr_register( TIMER_GROUP_0, TIMER_0, &_interrupt, // < length >,
							static_cast< void * >( this ), ESP_INTR_FLAG_IRAM, nullptr );

		timer_start( TIMER_GROUP_0, TIMER_0 );
        status.set_high();
    }

	void tick() {
        status.set( st = !st );
        ++val;
        bool dirty = false;
        for ( int i = 0; i < length; ++i ) {
            bool v = (255 - vpins[ i ]) < val;
            if ( v != sr.vpins[ i ] )
                dirty = dirty || true;
            sr._set( i, v );
        }
        if ( dirty )
            sr.write();
//        ets_printf( "tick\n" );
    }

    void set( int p, uint8_t v ) {
        vpins[ p ] = v;
    }

//  private:
    uint8_t val = 0;
    ShiftRegister< length > sr;
    std::array< uint8_t, length > vpins = {};
};

void IRAM_ATTR _interrupt( void *self ) {
    status.set_low();
    TIMERG0.int_clr_timers.t0 = 1;
    /* After the alarm has been triggered
     * we need enable it again, so it is triggered the next time */
    TIMERG0.hw_timer[ TIMER_0 ].config.alarm_en = TIMER_ALARM_EN;
    static_cast< PWMShiftRegister< 8 > * >( self )->tick();
}

template< size_t l >
void cycle( ShiftRegister< l > &sr ) {
    for ( ;; ) {
        sr.set_all_low();
        delay( 500 );
        sr.set_all_high();
        delay( 500 );
    }
}

void setup() {
    status.set_output();
    PWMShiftRegister< 8 > psr( 25, 26 );
    std::array< uint8_t, 8 > vs = { 2, 8, 32, 128, 255, 128, 32, 8 };

    while ( true ) {
        for ( int i = 0; i < 8; ++i ) {
            for ( int j = 0; j < 8; ++j ) {
                psr.set( j, vs[ (i + j) % 8 ] );
            }
            delay( 100 );
        }
    }
}

void loop() {
}
