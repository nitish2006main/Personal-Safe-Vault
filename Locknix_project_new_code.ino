#include <util/delay.h>
#include <avr/io.h>
#include <LCD.h>

#define bitn(p) (0x01 << (p))
#define checkbit(x,y) ((x) & (y))

LCD screen;

int angle = 0;
int countcorr = 0, countincorr = 0;
int xval, yval, zval;

// ====== Buzzer sequence for success ======
void buzzerPattern() {
    PORTC = 0xFF; 
    _delay_ms(100);
    PORTC = 0x00; 
    _delay_ms(50);
    PORTC = 0xFF; 
    _delay_ms(50);
    PORTC = 0x00; 
    _delay_ms(25);
    PORTC = 0xFF; 
    _delay_ms(50);
    PORTC = 0x00; 
    _delay_ms(100);
    PORTC = 0xFF; 
    _delay_ms(1000);
    PORTC = 0x00;
}

// ====== Display helper ======
void showMessage(const char *line1, const char *line2, int delayMs) {
    screen.cmd(0x01);
    if (line1) { 
        screen.line1(0); 
        screen.string(line1); 
    }
    if (line2) { 
        screen.line2(0); 
        screen.string(line2); 
    }
    _delay_ms(delayMs);
}

// ====== Digital input check ======
bool checkInput(int expectedBit) {
    return checkbit(PIND, bitn(expectedBit));
}

// ====== Pattern Game ======
bool playPattern(const int sequence[], int length) {
    countcorr = 0; 
    countincorr = 0;

    while (1) {
        if (checkInput(sequence[countcorr])) {
            // Correct input
            {
                screen.string("Correct!");
                _delay_ms(1000);
                screen.cmd(0x01);
                countcorr++;
            }
        } else {
            // Wrong input → restart pattern
            {
                screen.line1(0);
                screen.string("Wrong! Restart");
                screen.line2(0);
                screen.showvalue(++countincorr);
                screen.line2(4);
                screen.string("/3 wrong");
                _delay_ms(1000);
                screen.cmd(0x01);
                countcorr = 0;
            }
        }

        // Sequence complete
        if (countcorr == length) {
            {
                buzzerPattern();
                showMessage("Congrats on", "passing the", 2000);
                showMessage("first step.", NULL, 2000);
                return 1; 
            }
        }

        // Too many mistakes
        if (countincorr >= 3) {
            {
                showMessage("New Pattern", "Created", 2000);
                return 0; 
            }
        }
    }
}
void showPattern(const int sequence[], int length) {
    for (int i = 0; i < length; i++) {
        screen.cmd(0x01);  // clear screen
        screen.line1(0);
        screen.string("Pattern Step:");
        screen.line2(0);
        screen.showvalue(sequence[i]);  // show the current number in pattern
        _delay_ms(1000);                // hold each step for 1 second
    }
    screen.cmd(0x01); // clear screen after showing
}

// ====== Lever Game Helpers ======
int readADC(uint8_t channel) {
    ADMUX = 0x40 | channel;    // AVcc reference
    ADCSRA = 0xC7;             // start conversion
    while (checkbit(ADCSRA, bitn(ADSC))) 
    {
        ; // wait for conversion
    }
    return ADCW;
}

void leverQuestion(int zLow, int zHigh) {
    while (1) {
        zval = readADC(7);
        if (zval > zLow && zval < zHigh) {
            {
                showMessage("Correct!", NULL, 2000);
                return 0;
            }
        } else if (zval > 277 && zval < 280) {
            {
                continue;
            }
        } else {
            {
                showMessage("Wrong! No", "entry allowed", 2000);
                while(1);
            }
        }
    }
}

// ====== Servo motor function ======
void servo() {
  for (int i = 0; i<20;i++)
  {
    int tem = (5+(angle/10)); 
    TCNT0 = 0;
    while (var < 200)
    {
      if (TCNT0 == 25)
      {
        var++;
        TCNT0 = 0;
      }
      if (var == tem)
      {
        PORTB = 0x00;
      }
    }
    if (var == 200)
    {
      PORTB = 0xFF;
      var = 0;
    }
  }
}

// ====== Main ======
int main(void) {
    DDRC = 0xFF; // buzzer
    DDRD = 0x00; // inputs
    screen.init();

    const int pattern1[] = {2,3,3,3,2,3,2};
    const int pattern2[] = {3,2,2,3,2,3,2};
    const int pattern3[] = {2,2,2,3,3,2,3};
    const int pattern4[] = {3,3,2,3,2,2,2};

    while (1) {
        showMessage("Press 1", "to start", 500);
        if (checkInput(2)) {
            {
                showMessage("Welcome to your", "personal vault", 2000);
                showMessage("security system", NULL, 2000);
                showMessage("Please proceed", "with the steps", 2000);
                showMessage("to unlock the", "door..........", 2000);
                showMessage("Remember the", "pattern and use", 2000);
                showMessage("tactswitches", "to match", 2000);
                showMessage("the pattern", NULL, 2000);

                showPattern(pattern1, 7);
                
                // Sequential patterns
                int unlocked = 0;
                int unlocked = 0;
                if (playPattern(pattern1, 7)) {
                    unlocked = 1;
                }
                else {
                    // Show pattern 2 if user failed pattern 1
                    showPattern(pattern2, 7);
                    if (playPattern(pattern2, 7)) {
                        unlocked = 1;
                    }
                    else {
                        // Show pattern 3 if user failed pattern 2
                        showPattern(pattern3, 7);
                        if (playPattern(pattern3, 7)) {
                            unlocked = 1;
                        }
                        else {
                            // Show pattern 4 if user failed pattern 3
                            showPattern(pattern4, 7);
                            if (playPattern(pattern4, 7)) {
                                unlocked = 1;
                            }
                        }
                    }
                }

                if (!unlocked) {
                    {
                        showMessage("Vault Locked!", "Access Denied");
                        while (1); // lock forever
                    }
                }

                // Lever questions
                showMessage("What is my", "favourite fruit?", 2000);
                showMessage("A) Apple B) Banana", "C) Orange D) Mango", 4000);
                showMessage("Choose your answer", "Using the lever", 2000);
                leverQuestion("Choose your answer", "Using the lever", 291, 298);
                
                showMessage("How much do", "I weigh in lbs?", 2000);
                showMessage("A) 120 B) 140", "C) 160 D) 180", 4000);
                showMessage("Choose your answer", "Using the lever", 2000);
                leverQuestion(280, 285);
                
                showMessage("Who is my", "best friend?", 2000);
                showMessage("A) Alice B) Bob", "C) Charlie D) David", 4000);
                showMessage("Choose your answer", "Using the lever", 2000);
                leverQuestion(285, 291);

                // Final door open
                buzzerPattern();
                showMessage("The door", "will open", 2000);

                // Servo open
                for (int m = 10; m <= 90; m += 10) {
                    {
                        angle = m;
                        servo();
                        _delay_ms(25);
                    }
                }

                showMessage("Please place the", "lever to default", 2000);
                screen.cmd(0x01);
                _delay_ms(3000);
                angle = 0;
                servo();
            }
        }
    }
}

