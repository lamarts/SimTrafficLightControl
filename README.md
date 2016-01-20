# SimTrafficLightControl

This is a Embedded C program using the Atmega32 8 bit microcontroller.  
This program simulates a street traffic light control system using two sets of lights (LEDs for green, yellow and red), one going North-South and the other East-West direction.   On each corner there is an associated button for each pedestrian to push when he/she wants to cross the street.   The timer will count down shorter when the button is pressed.   There is also another button for "emergency" which flashes red on both "traffic lights".   Each button is tied to high and is high when pressed.

The code for the LCD can be removed as it is only used for the video demonstration.   I had planned on adding 'walk", "don't walk" however it was time to move on to other things.  
