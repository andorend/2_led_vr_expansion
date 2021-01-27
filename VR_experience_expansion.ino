

#define NUM_LEDS (2*1+2*1)
#define LED_LEFT_R 3
#define LED_LEFT_G 5
#define LED_LEFT_B 6

#define LED_RIGHT_R 9
#define LED_RIGHT_G 10
#define LED_RIGHT_B 11
#define BRIGHTNESS 255 //range is 0..255 with 255 beeing the MAX brightness

// --------------------------------------------------------------------------------------------
// NO CHANGE REQUIRED BELOW THIS LINE
// --------------------------------------------------------------------------------------------

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
uint8_t startIndex = 0;


void setup()
{
  Serial.begin(1000000);

  pinMode(LED_LEFT_R, OUTPUT);
  pinMode(LED_LEFT_G, OUTPUT);
  pinMode(LED_LEFT_B, OUTPUT);
  
  pinMode(LED_RIGHT_R, OUTPUT);
  pinMode(LED_RIGHT_G, OUTPUT);
  pinMode(LED_RIGHT_B, OUTPUT);
  
  /*FastLED.clear(true);
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(currentBrightness);
  FastLED.setDither(0);*/
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
        if(ledNum%2 && ledNum < 2){
          analogWrite(LED_LEFT_R, red);
          analogWrite(LED_LEFT_G, green);
          analogWrite(LED_LEFT_B, blue);
        }else if(ledNum%2 && ledNum < 4){
          analogWrite(LED_RIGHT_R, red);
          analogWrite(LED_RIGHT_G, green);
          analogWrite(LED_RIGHT_B, blue);
        }
        //ledsTemp[ledNum] = CRGB(red, green, blue);
      }
      
      /*
      else if (ledNum == NUM_LEDS)
      {
        //this must be the "postamble" 
        //this last "color" is actually a closing preamble
        //if the postamble does not match the expected values, the colors will not be shown
        if(buffer[0] == 85 && buffer[1] == 204 && buffer[2] == 165) {
          //the preamble is correct, update the leds!

          // TODO: can we flip the used buffer instead of copying the data?
          for (int ledNum = 0; ledNum < NUM_LEDS; ledNum++)
          {
            leds[ledNum]=ledsTemp[ledNum];
          }
      
          if (currentBrightness < BRIGHTNESS)
          {
            currentBrightness++;
            FastLED.setBrightness(currentBrightness);
          }
          
          //send LED data to actual LEDs
          FastLED.show();
        }
      }*/
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
    // FastLED.setBrightness(currentBrightness);
  }

  // FastLED.delay(1000 / UPDATES_PER_SECOND);

  if (Serial.available() > 0)
  {
    mode = MODE_AMBILIGHT;
  }
}
