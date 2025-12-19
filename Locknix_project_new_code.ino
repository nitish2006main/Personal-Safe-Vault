#include <util/delay.h>
#include <avr/io.h>
#include <LCD.h>
#define bitn(p) (0x01 << (p))
#define checkbit(x,y) ((x) & (y))
int angle = 0, var = 0;
LCD screen;
const int COLS = 7;
//using system_locked as a global variable for the main function and runvaultSequence function
//system_locked will always be true whenever the user either fails or succeeds in opening the vault
//system_locked is used to check whether the users has exited the sequence or not, to allow the user to reset the program or leave it as it is. 
bool system_locked = false; //this is to check whether the the person has reached the end of the program, where it locks the system, or the beginning of the program

//function prototypes
bool runVaultSequence(); //this function lets you run the sequence for the vault system
void buzzerPattern(); //this is a simple buzzer pattern for UI
void showMessage(string line1, string line2, int milliseconds); //this is the main function for displaying messages. No pointers or dynamic arrays, since each line can only hold up to 16 characters
bool checkInput(int expectedBit); //faster way of checking the input of a bit
bool playPatternCorrect(const int pattern[], int length); //this pattern checker is only for 1-d arrays
bool playPatternCorrect(int pattern[][COLS], int rows); //this pattern is for the 2-d arrays
void showPattern(int pattern[][COLS], int rows); //this is linked to the 2-d arrays
bool leverQuestion(int zLow, int zHigh); //this is for checking the range of acceptable values as the answer
void servo(); //function to move the servo motor

int main(void) 
{
    DDRC = 0xFF; // buzzer
    DDRD = 0x00; // inputs
    screen.init();
    const int automatic_open_pattern[10] = {3,2,3,3,2,3,2,3,3,2}; //this pattern is used at the beginning of the program if you want to directly access the vault and open it
    const int secret_reset_pattern[4] = {2,3,2,3};   //this pattern is used once the program is done, and you want to set it up again to its initial start
    while (1)
    {
        
        if (!system_locked)
        {
            if (playPatternCorrect(automatic_open_pattern, 10)) 
            {
                // Servo open sequence
                for (int m = 10; m <= 90; m += 10) {
                    {
                        angle = m;
                        servo();
                        _delay_ms(25);
                    }
                }
                _delay_ms(3000);
                angle = 0;
                servo();
                system_locked = true;
            }
            else //if the user cannot find the pattern, then it will automatically run the vault sequence program
            {
                showMessage("Press 1", "to start", 2000);
                while(!checkInput(2));
                while(checkInput(2));
                runVaultSequence();
            }
        }
        else // system locked
        {
            showMessage("System Locked!", "Enter reset pattern", 2000);
            if (playPatternCorrect(secret_reset_pattern, 4)) {
                system_locked = false;
                showMessage("System Reset", "Access Restored", 2000);
            }
        }
    }
}
//I used bool for the vault sequence so that whenever the user gets the sequence wrong
//in the middle of the sequence, it returns a value and exits, rather than continuing the sequence
bool runVaultSequence()
{
    bool is_unlocked = false; //this is to check whether the user is able to play the first step and see if they can move on or retry again, until the system has to be locked
    const int pattern[4][7] = {{2,3,3,3,2,3,2},{3,2,2,3,2,3,2},{2,2,2,3,3,2,3},{3,3,2,3,2,2,2}};
    showMessage("Welcome to your", "personal vault", 2000);
    showMessage("security system", NULL, 2000);
    showMessage("Please proceed", "with the steps", 2000);
    showMessage("to unlock the", "door..........", 2000);
    showMessage("Remember the", "pattern and use", 2000);
    showMessage("tactswitches", "to match", 2000);
    showMessage("the pattern", NULL, 2000);
    showPattern(pattern, 0);
    
    // Sequential patterns
    if (playPatternCorrect(pattern, 0)) 
    {
        is_unlocked = true;
        buzzerPattern();
        showMessage("Congrats on", "passing the", 2000);
        showMessage("first step.", "", 2000);
    }
    else 
    {
    // Loop through remaining patterns
        for (int i = 1; i < 4 && !is_unlocked; i++) 
        {
            showMessage("New Pattern", "Created", 2000);
            showPattern(pattern, i);
            if (playPatternCorrect(pattern, i)) 
            {
                is_unlocked = true;
                buzzerPattern();
                showMessage("Congrats on", "passing the", 2000);
                showMessage("first step.", "", 2000);
            }
            else
            {
                showMessage("Start", "Again", 2000);
            }
        }
    }

    if (is_unlocked == false) {
        {
            showMessage("Vault Locked!", "Access Denied", 2000);
            system_locked = true;
            return false;
        }
    }

    // Lever questions
    showMessage("What is my", "favourite fruit?", 2000);
    showMessage("A) Apple B) Banana", "C) Orange D) Mango", 4000);
    showMessage("Choose your answer", "Using the lever", 2000);
    if (!leverQuestion(291, 298))
    {
        system_locked = true;
        return false;
    }
    showMessage("How much do", "I weigh in lbs?", 2000);
    showMessage("A) 120 B) 140", "C) 160 D) 180", 4000);
    showMessage("Choose your answer", "Using the lever", 2000);
    if (!leverQuestion(280, 285))
    {
        system_locked = true;
        return false;
    }
    showMessage("Who is my", "best friend?", 2000);
    showMessage("A) Alice B) Bob", "C) Charlie D) David", 4000);
    showMessage("Choose your answer", "Using the lever", 2000);
    if (!leverQuestion(285, 291))
    {
        system_locked = true;
        return false;
    }
    // Final door open
    buzzerPattern();
    showMessage("The door", "will open", 2000);

    // Servo open
    for (int m = 10; m <= 90; m += 10) 
    {
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
    system_locked = true;
    return true;
}
//Buzzer Sound Sequence
void buzzerPattern() 
{
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

//Display function
void showMessage(string line1, string line2, int milliseconds) 
{
    screen.cmd(0x01);
    screen.line1(0); 
    screen.string(line1); 
    screen.line2(0); 
    screen.string(line2); 
    _delay_ms(milliseconds);
}

//Digital input check
bool checkInput(int expectedBit) 
{
    return checkbit(PIND, bitn(expectedBit));
}
//checking for the secret patterns. Not part of vaultSequence function
bool playPatternCorrect(const int pattern[], int length) 
{
    int countcorr = 0;
    while (countcorr < length) 
    {
        if (checkInput(pattern[countcorr])) 
        {
            screen.string("Correct!");
            _delay_ms(500);
            screen.cmd(0x01);
            countcorr++;
        } 
        else 
        {
            showMessage("Wrong!", " ");
            return false;
        }
    }
    return true;
}
//Pattern Game
bool playPatternCorrect(int pattern[][COLS], int rows) 
{
    int countcorr = 0; 
    int countincorr = 0;
    while (countcorr < COLS) 
    {
        if (checkInput(pattern[rows][countcorr])) 
        {
            screen.string("Correct!");
            _delay_ms(1000);
            screen.cmd(0x01);
            countcorr++;
        } 
        else 
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
        // Too many mistakes
        if (countincorr >= 3) 
        {
            return false;
        }
    }
    return true;
}
//function for showing the patterns in pattern game
void showPattern(int pattern[][COLS], int rows) 
{
    screen.cmd(0x01);  // clear screen
    screen.line1(0);
    screen.string("Pattern Step:");
    screen.line2(0);
    for (int i = 0; i < COLS; i++) 
    {
        screen.showvalue(pattern[rows][i]);  // show each number in pattern
        screen.line2(i+1);      
    }
    _delay_ms(1000);  
    screen.cmd(0x01); // clear screen after showing
}

bool leverQuestion(int zLow, int zHigh) 
{
    int zval;
    ADMUX = 0x40;
    ADCSRA = 0xC7;           
    while (checkbit(ADCSRA, bitn(ADSC)));
    zval = ADCW;
    while(1)
    {
        if (zval > zLow && zval < zHigh) 
        {
            showMessage("Correct!", NULL, 2000);
            return true;
        } 
        else if (zval > 277 && zval < 280) 
        {
            _delay_ms(2000);
            screen.cmd(0x01);
        } 
        else 
        {
            return false;
        }
    }
}

// ====== Servo motor function ======
void servo() 
{
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

