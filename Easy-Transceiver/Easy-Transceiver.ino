// Borrowed from https://github.com/agustinmartino/wsjt_transceiver (GPL v3)
//
// Author: Agustin Martino (LU2HES)
// Author (minor): Dhiru Kholia (VU3CER)

#include <Wire.h>
#include <si5351.h>
#include <JTEncode.h>

// Relay pins
const int relay_1 = 6; // D6
const int relay_2 = 7; // D7
#define PTT_PIN 8 // D8

// https://www.qsl.net/yo4hfu/SI5351.html says,
//
// If it is necessary, frequency correction must to be applied. My Si5351
// frequency was too high, correction used 1.787KHz at 10MHz. Open again
// Si5351Example sketch, set CLK_0 to 1000000000 (10MHz). Upload. Connect a
// accurate frequency counter to CLK_0 output pin 10. Correction factor:
// (Output frequency Hz - 10000000Hz) x 100. Example: (10001787Hz - 10000000Hz)
// x 100 = 178700 Note: If output frequency is less than 10MHz, use negative
// value of correction, example -178700.

#define CORRECTION 200000 // !!! ATTENTION !!! Change this for your reference oscillator

// Global variables
#define SERIAL_TIMEOUT 20000

int buttonState = 0;
Si5351 si5351;
JTEncode jtencode;
unsigned long freq;
char message[] = "VU3CER VU3FOE MK68";
char call[] = "VU3FOE";
char loc[] = "MK68";
uint8_t dbm = 27;
uint16_t offset = 1200;
uint8_t tx_buffer[255];  // NOTE
uint8_t symbol_count;
double tone_delay, tone_spacing;
int vfo_ok = 1;
int beacon_enabled = 0;
const int ledPin = LED_BUILTIN;
bool message_available = false;
unsigned int timeout;

enum modes
{
  MODE_FT8,
  MODE_FT4,
  MODE_WSPR,
};

enum modes cur_mode;

// Prepare relays for TX
void pre_transmit()
{
  // Move T/R switch to TX (NO) position
  digitalWrite(relay_1, LOW);
  digitalWrite(relay_2, LOW);
  delay(30); // Songle SRD
}

void ptt(int state) {
  if (state == 1) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(PTT_PIN, HIGH);
  }
  else {
    digitalWrite(PTT_PIN, LOW);
    digitalWrite(ledPin, LOW);
  }
}

void tx()
{
  uint8_t i;

  // Important!
  pre_transmit();
  digitalWrite(PTT_PIN, HIGH);// Note
  digitalWrite(ledPin, HIGH);

  // Channel jumping hack
  // long n = random(-800, 2300);
  unsigned long nfreq = freq + offset;

  // Reset the tone to the base frequency and turn on the output
  si5351.set_clock_pwr(SI5351_CLK0, 1);

  // Now do the rest of the message
  for (i = 0; i < symbol_count; i++)
  {
    unsigned long timer = millis();
    si5351.set_freq((nfreq * 100) + (tx_buffer[i] * tone_spacing), SI5351_CLK0);
    // delay(tone_delay);
    while ((millis() - timer) <= tone_delay) {
      __asm__("nop\n\t");
    }
  }

  // Turn off the output
  si5351.set_clock_pwr(SI5351_CLK0, 0);
  digitalWrite(PTT_PIN, LOW);
  digitalWrite(ledPin, LOW);
  delay(100); // important

  // Turn off the relays -> Move T/R switch to RX (NC) position. RX Mode.
  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);
  delay(50);
}

// debug helper
void led_flash()
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
}

void setup_mode()
{
  // https://github.com/etherkit/JTEncode/blob/master/examples/Si5351JTDemo/Si5351JTDemo.ino
#define WSPR_TONE_SPACING 146 // ~1.46 Hz
#define FT8_TONE_SPACING 625  // ~6.25 Hz
#define FT4_TONE_SPACING 2083 // ~20.83 Hz

#define WSPR_DELAY 683 // Delay value for WSPR
#define FT8_DELAY 159  // Delay value for FT8
#define FT4_DELAY 45   // Delay value for FT4, value from LU2HES - thanks!

#define WSPR_DEFAULT_FREQ 14097050UL  // +300 Hz
#define FT8_DEFAULT_FREQ 14075000UL
#define FT4_DEFAULT_FREQ 14081000UL   // +1 KHz

#define FT8_SYMBOL_COUNT 79

  // FT4 stuff is buggy currently!
#define FT4_SYMBOL_COUNT 103  // based on weakmon, should be 105 instead

  if (cur_mode == MODE_WSPR) {
    freq = WSPR_DEFAULT_FREQ;
    symbol_count = WSPR_SYMBOL_COUNT; // From the library defines
    tone_spacing = WSPR_TONE_SPACING;
    tone_delay = WSPR_DELAY;
    jtencode.wspr_encode(call, loc, dbm, tx_buffer);
  } else if (cur_mode == MODE_FT8) {
    freq = FT8_DEFAULT_FREQ;
    symbol_count = FT8_SYMBOL_COUNT;
    tone_spacing = FT8_TONE_SPACING;
    tone_delay = FT8_DELAY;
    // jtencode.ft8_encode(message, tx_buffer); // buggy/limited
    // encoder(message, tx_buffer, 0);
  } else if (cur_mode == MODE_FT4) {
    freq = FT4_DEFAULT_FREQ;
    symbol_count = FT4_SYMBOL_COUNT;
    tone_spacing = FT4_TONE_SPACING;
    tone_delay = FT4_DELAY;
    // jtencode.ft8_encode(message, tx_buffer); // buggy/limited
    // encoder(message, tx_buffer, 1);
  }
}

void setup()
{
  int ret = 0;
  // Safety first!
  pinMode(PTT_PIN, OUTPUT);
  digitalWrite(PTT_PIN, LOW);

  // Setup serial and IO pins
  Serial.begin(38400);
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);
  // pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  // pinMode(TX_LED_PIN, OUTPUT);
  digitalWrite(ledPin, LOW);
  // digitalWrite(TX_LED_PIN, LOW);
  Serial.println("\n...");

  // Initialize the Si5351
  ret = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, CORRECTION);
  Serial.print("Si5351 init status (should be 1 always) = ");
  Serial.println(ret);
  if (ret != true) {
    Serial.flush();
    vfo_ok = 0;
    // abort();
  }

  // Set CLK0 output
  si5351.set_freq(freq * 100, SI5351_CLK0);
  // si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA); // Set for minimum power
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA); // Set for max power
  // delay(10000); // Keep TX on for 5 seconds for tunining purposes.
  si5351.set_clock_pwr(SI5351_CLK0, 0); // Disable the clock initially

  // Sanity checks
  if (!vfo_ok) {
    Serial.println("Check VFO connections!");
    led_flash();
    delay(50);
  }

  // Important to not start TX at 0 Hz ;)
  cur_mode = MODE_FT8;
  setup_mode();

  led_flash();
}

// Borrowed straight from LU2HES
void loop()
{
  char recibido;
  unsigned char rec_byte[2];
  unsigned int msg_index;

  if (Serial.available() > 0)
  {
    // Read the incoming byte:
    recibido = Serial.read();

    // New message
    if (recibido == 'm')
    {
      msg_index = 0;
      timeout = 0;
      while (msg_index < symbol_count && timeout < SERIAL_TIMEOUT)
      {
        if (Serial.available() > 0)
        {
          recibido = Serial.read();
          tx_buffer[msg_index] = recibido;
          msg_index++;
        }
        timeout += 1;
      }
      if (timeout >= SERIAL_TIMEOUT)
      {
        message_available = false;
        Serial.println("Timeout");
        led_flash();
        Serial.println(timeout, DEC);
        Serial.println(msg_index, DEC);
        for (unsigned char kk = 0; kk < symbol_count; kk++)
          Serial.println(tx_buffer[kk], DEC);
      }
      else
      {
        message_available = true;
        // led_flash();
        // Serial.println(timeout, DEC);
        // Serial.println(msg_index, DEC);
        Serial.print("m");
      }
    }

    // Change offset
    else if (recibido == 'o')
    {
      msg_index = 0;
      // Offset encoded in two bytes
      while (msg_index < 2)
      {
        if (Serial.available() > 0)
        {
          rec_byte[msg_index] = Serial.read();
          msg_index++;
        }
      }
      offset = rec_byte[0] + (rec_byte[1] << 8);
      Serial.print("o");
    }

    // Switch mode
    else if (recibido == 's')
    {
      if (cur_mode == MODE_FT8)
        cur_mode = MODE_FT4;
      else
        cur_mode = MODE_FT8;
      setup_mode();
      message_available = false;
      Serial.print("s");
    }

    else if (recibido == 'f')
    {
      led_flash();
    }

    // WSPR Mode
    else if (recibido == 'w')
    {
      cur_mode = MODE_WSPR;
      setup_mode();
      message_available = false;
    }

    // Transmit
    else if (recibido == 't')
    {
      if (message_available)
        tx();
    }

    // PTT on
    else if (recibido == 'p')
    {
      ptt(1);
    }

    // PTT off
    else if (recibido == 'z')
    {
      ptt(0);
    }

    else if (recibido == 'v')
    {
      // Send indication for ready!
      Serial.print("r");
    }

    // Relays setup -> Pre-transmit
    else if (recibido == 'r')
    {
      pre_transmit();
    }

    // Debug!
    else if (recibido == 'x')
    {
      Serial.print(cur_mode);
    }
    else if (recibido == 'y')
    {
      Serial.print(freq);
    }
  }
  delay(10);
}
