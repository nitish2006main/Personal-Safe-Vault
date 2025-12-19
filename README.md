# Personal-Safe-Vault
This is a modified version of a personal safe/vault. As we all know, most safes only have a 4 digit code that you have to enter to open the vault. I decided to make mine a little more fun by adding some sort of minigames to unlock it. 

The first minigame is like a pattern game. It displays a pattern on the screen, and after its gone, you need to remember it. Using the 2 onboard tactswitches, you need to use those to input the pattern. You have like 3 tries to restart the same pattern, then you have around 3 more patterns as a backup, or else the code stops there. But all you need to do is finish 1 pattern successfully. The reason why I added a set of 4 patterns to go through is to implement an array logic, as I'm trying to learn to use arrays in different ways. 

After, its a lever game. Personally, this was fun to make, since it's like a multiple choice question, and the way you would choose your answer is by moving a lever up and down, with a A-D answer selection. A at the top, D at the bottom. And basically how this works is that the accelerometer sets a range for each A-D answer, using values from 0-1023 (10 bits). IF ever you get it wrong (which you shouldn't, if it's your OWN safe), then the code stops. 

After both minigames are done, then the door automatically opens. 

Now you may be thinking, what happens if someone tries to break in the vault multiple times and remembers the answers. Well, you could always change the questions and answers in the actual program, and boom, now they have to suffer a couple times before trying to open it again, and you would just keep on changing the answers. 

I made this project for a competition, and the way it works is that you have only a certain limited access to components, since we are placed under "levels". So if i were to move on to the next level, i would be able to use bluetooth, onboard keypad, and many more other items. 

Even though it's not the most impressive due to the fact that it doesn't have crazy components, i personally enjoyed the experience of making this project, and I learned many things such as patience, research, debugging, and so on so fourth. 

And obviously after all the firmware, i assembled the entire project with all the wires, breadboard and physical design. 
