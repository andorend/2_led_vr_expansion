#define NUM_LEDS (2*1+2*1)
#define LEFT_LED_PIN_RED 3
#define LEFT_LED_PIN_GREEN 5
#define LEFT_LED_PIN_BLUE 6

#define RIGHT_LED_PIN_RED 9
#define RIGHT_LED_PIN_GREEN 10
#define RIGHT_LED_PIN_BLUE 11

#define BRIGHTNESS 255 //range is 0..255 with 255 beeing the MAX brightness

#define UPDATES_PER_SECOND 60
#define TIMEOUT 3000
#define MODE_ANIMATION 0
#define MODE_AMBILIGHT 1
#define MODE_BLACK 2
uint8_t mode = MODE_ANIMATION;

uint8_t currentBrightness = BRIGHTNESS;
byte MESSAGE_PREAMBLE[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
uint8_t PREAMBLE_LENGTH = 10;
uint8_t current_preamble_position = 0;

unsigned long last_serial_available = -1L;

byte buffer[3];

// Filler animation attributes
uint8_t startIndex = 0;

void setup()
{
  Serial.begin(1000000);
  pinMode(LEFT_LED_PIN_RED, OUTPUT);
  pinMode(LEFT_LED_PIN_GREEN, OUTPUT);
  pinMode(LEFT_LED_PIN_BLUE, OUTPUT);
  pinMode(RIGHT_LED_PIN_RED, OUTPUT);
  pinMode(RIGHT_LED_PIN_GREEN, OUTPUT);
  pinMode(RIGHT_LED_PIN_BLUE, OUTPUT);
}

void loop()
{
  switch (mode) {
    case MODE_ANIMATION:
      fillLEDsFromPaletteColors();
      break;

    case MODE_AMBILIGHT:
      processIncomingData();
      break;

    case MODE_BLACK:
      showBlack();
      break;
  }
}
void processIncomingData()
{
  if (waitForPreamble(TIMEOUT))
  {
    for (int ledNum = 0; ledNum < NUM_LEDS+1; ledNum++)
    {
      //we always have to read 3 bytes (RGB!)
      //if it is less, we ignore this frame and wait for the next preamble
      if (Serial.readBytes((char*)buffer, 3) < 3) return;


      if(ledNum < NUM_LEDS)
      {          
        byte blue = buffer[0];
        byte green = buffer[1];
        byte red = buffer[2];
        if (ledNum == 1){
          // left eye led
          analogWrite(LEFT_LED_PIN_RED, red);
          analogWrite(LEFT_LED_PIN_GREEN, green);
          analogWrite(LEFT_LED_PIN_BLUE, blue);
        }else if(ledNum == 3){
          // right eye led
          analogWrite(RIGHT_LED_PIN_RED, red);
          analogWrite(RIGHT_LED_PIN_GREEN, green);
          analogWrite(RIGHT_LED_PIN_BLUE, blue);
          
        }
      }
      
    }
  }
  else
  {
    //if we get here, there must have been data before(so the user already knows, it works!)
    //simply go to black!
    mode = MODE_BLACK;
  }
}

bool waitForPreamble(int timeout)
{
  last_serial_available = millis();
  current_preamble_position = 0;
  while (current_preamble_position < PREAMBLE_LENGTH)
  {
    if (Serial.available() > 0)
    {
      last_serial_available = millis();

      if (Serial.read() == MESSAGE_PREAMBLE[current_preamble_position])
      {
        current_preamble_position++;
      }
      else
      {
        current_preamble_position = 0;
      }
    }

    if (millis() - last_serial_available > timeout)
    {
      return false;
    }
  }
  return true;
}

void fillLEDsFromPaletteColors()
{
  if (Serial.available() > 0)
  {
    mode = MODE_AMBILIGHT;
  }
}

void showBlack()
{
  if (currentBrightness > 0)
  {
    currentBrightness--;
  }


  if (Serial.available() > 0)
  {
    mode = MODE_AMBILIGHT;
  }
}
