#include "PsxNewLib.h"
//#include <DigitalIO.h>

/** \brief Attention Delay
 *
 * Time between attention being issued to the controller and the first clock
 * edge (us).
 */
const byte ATTN_DELAY = 50;

/** \brief Clock Period
 *
 * Inverse of clock frequency, i.e.: time for a *full* clock cycle, from falling
 * edge to the next falling edge.
 */
const byte CLK_PERIOD = 40;

// Must be < CLK_PERIOD / 2
const byte HOLD_TIME = 2;


template <uint8_t PIN_ATT, uint8_t PIN_CMD, uint8_t PIN_DAT, uint8_t PIN_CLK>
class PsxControllerBitBang: public PsxController {
private:
	uint8_t att = PIN_ATT;
	uint8_t clk = PIN_CLK;
	uint8_t cmd = PIN_CMD;
	uint8_t dat = PIN_DAT;

	//#define <PIN_ATT> att
	//#define <PIN_CLK> clk
	//#define <PIN_CMD> cmd
	//#define <PIN_DAT> dat

protected:
	virtual void attention () override {
		digitalWrite(att, LOW);
		//att.low ();
		delayMicroseconds (ATTN_DELAY);
	}
	
	virtual void noAttention () override {
		//~ delayMicroseconds (5);
		digitalWrite(cmd, HIGH);
		digitalWrite(clk, HIGH);
		digitalWrite(att, HIGH);

		//cmd.high ();
		//clk.high ();
		//att.high ();
		delayMicroseconds (ATTN_DELAY);
	}
	
	virtual byte shiftInOut (const byte out) override {
		byte in = 0;

		// 1. The clock is held high until a byte is to be sent.

		for (byte i = 0; i < 8; ++i) {
			// 2. When the clock edge drops low, the values on the line start to
			// change
			digitalWrite(clk, LOW);
			//clk.low ();

			delayMicroseconds (HOLD_TIME);
			
			if (bitRead (out, i)) {
				digitalWrite(cmd, HIGH);
				//cmd.high ();
			} else {
				digitalWrite(cmd, LOW);
				//cmd.low ();
			}

			delayMicroseconds (CLK_PERIOD / 2 - HOLD_TIME);

			// 3. When the clock goes from low to high, value are actually read
			digitalWrite(clk, HIGH);
			//clk.high ();

			delayMicroseconds (HOLD_TIME);
			
			if (dat) {
				bitSet (in, i);
			}

			delayMicroseconds (CLK_PERIOD / 2 - HOLD_TIME);
		}

		return in;
	}

public:
	virtual boolean begin () override {

		pinMode(att, OUTPUT);
		pinMode(cmd, OUTPUT);
		pinMode(clk, OUTPUT);
		pinMode(dat, INPUT_PULLUP);

		digitalWrite(att, HIGH);
		digitalWrite(cmd, HIGH);
		digitalWrite(clk, HIGH);


		//att.config (OUTPUT, HIGH);    // HIGH -> Controller not selected
		//cmd.config (OUTPUT, HIGH);
		//clk.config (OUTPUT, HIGH);
		//dat.config (INPUT, HIGH);     // Enable pull-up

		return PsxController::begin ();
	}
};
