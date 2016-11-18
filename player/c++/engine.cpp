// ======================
//    Wesley Miravete
//         2016
// ======================

// Notes:
// Try to use as small of types as you can for optimization purposes
// Avoid operations and conversions when possible
// (Frive == floppy drive)
// Toggling the pin is neccessary because the writing from 0 to 1 to 0 again sometimes overlooks the 1, especially on a crappy frive.

#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <thread>
#include <signal.h>
#include "engine.hpp"
#include "notes.hpp"

// #define STEPFRIVEF stepFrive_sliding // stepFrive_oscillating or stepFrive_sliding

// =========
//  Globals
// =========
// ------------------------------
//  Individual frive information
// ------------------------------
byte pins[6][3] = {
	// Dir, step, led
	{27, 3, 4},
	{5, 12, 16},
	{20, 21, 22},
	{23, 24, 25},
	{13, 26, 6},
	{17, 18, 19}
};
// Keep track of amount of frives for loop purposes
byte friveCount = 6;

// 3.5" frives have 80 tracks and 5.25" have 50
// Subtract 8 to add a bit of padding
byte MAX_POSITIONS[]    = {72, 72, 72, 72, 72, 72}; // Constants
byte currentPositions[] = {0, 0, 0, 0, 0, 0};
byte currentDirection[] = {0, 0, 0, 0, 0, 0};
bool currentVoltage[]   = {0, 0, 0, 0, 0, 0};

unsigned int currentPeriod[] = {0, 0, 0, 0, 0, 0}; // Current period is how long until another step
unsigned int currentTick[]   = {0, 0, 0, 0, 0, 0}; // Counts how long has passed since the frive has been stepped

// Initialize the stepmethod variable (0 = Sliding, 1 = Oscillating)
static bool stepmethod;

// =============
//   Functions
// =============
int setup()
{
	wiringPiSetupGpio();
	for (byte i = 0; i < friveCount; i++)
	{
		pinMode(pins[i][0], OUTPUT); // Direction pin
		pinMode(pins[i][1], OUTPUT); // Step pin
		pinMode(pins[i][2], OUTPUT); // LED pin
	}

	// Starting the serial device and returning file descriptor
	int fd;
	if ((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
	{
		std::cout << "Could not open serial device." << std::endl;
		return 1;
	}
	return fd;
}

// NOTE: I've noticed that the sliding method seems to produce notes that are an octave higher than they should be.
// This is probably due to the fact that an octave is just double/half the frequency of the previous/next octave, respectably, which
// could mean that for whatever reason the sliding method produces notes of double frequency

void stepFrive_oscillating(byte frive)
{
	// Switch the direction every time the step pin is at a high voltage
	currentDirection[frive] = currentVoltage[frive] ? !currentDirection[frive] : currentDirection[frive];
	digitalWrite(pins[frive][0], currentDirection[frive]);

	// Toggle the step pin
	currentVoltage[frive] = !currentVoltage[frive];
	digitalWrite(pins[frive][1], currentVoltage[frive]);
}

void stepFrive_sliding(byte frive)
{
	// Switch directions when reaching either end
	if (currentPositions[frive] >= MAX_POSITIONS[frive])
	{
		currentDirection[frive] = 1;
		digitalWrite(pins[frive][0], 1);
	}
	else if (currentPositions[frive] <= 0)
	{
		currentDirection[frive] = 0;
		digitalWrite(pins[frive][0], 0);
	}

	// Toggle the step pin
	currentVoltage[frive] = !currentVoltage[frive];
	digitalWrite(pins[frive][1], currentVoltage[frive]);

	// Keep track of current motor arm position
	if (currentVoltage[frive])
	{
		if (currentDirection[frive] == 1)
		{
			currentPositions[frive] -= 1;
		}
		else
		{
			currentPositions[frive] += 1;
		}
	}
}

void STEPFRIVEF(byte frive)
{
	// Just a standin function in case I ever want to go back to the preprocessor definition
	if (stepmethod) stepFrive_oscillating(frive); else stepFrive_sliding(frive);
}

void tick()
{
	// Get delta time
	static unsigned int time = 0;
	static unsigned int lasttime = 0;
	time = micros();
	unsigned int delta = time-lasttime;
	lasttime = time;

	// Go through each frive and check if it's time to call a stepping function
	// (I don't use a for loop to iterate over each one, because I'm afraid it'll slow it down)
	if (currentPeriod[0]>0)
	{
		currentTick[0] += delta;
		if (currentTick[0] >= currentPeriod[0])
		{
			STEPFRIVEF(0);
			currentTick[0]=0;
		}
	}
	if (currentPeriod[1]>0)
	{
		currentTick[1] += delta;
		if (currentTick[1] >= currentPeriod[1])
		{
			STEPFRIVEF(1);
			currentTick[1]=0;
		}
	}
	if (currentPeriod[2]>0)
	{
		currentTick[2] += delta;
		if (currentTick[2] >= currentPeriod[2])
		{
			STEPFRIVEF(2);
			currentTick[2]=0;
		}
	}
	if (currentPeriod[3]>0)
	{
		currentTick[3] += delta;
		if (currentTick[3] >= currentPeriod[3])
		{
			STEPFRIVEF(3);
			currentTick[3]=0;
		}
	}
	if (currentPeriod[4]>0)
	{
		currentTick[4] += delta;
		if (currentTick[4] >= currentPeriod[4])
		{
			STEPFRIVEF(4);
			currentTick[4]=0;
		}
	}
	if (currentPeriod[5]>0)
	{
		currentTick[5] += delta;
		if (currentTick[5] >= currentPeriod[5])
		{
			STEPFRIVEF(5);
			currentTick[5]=0;
		}
	}
	delayMicroseconds(5); // Prevent the loop from going too fast and giving itself a bad time
}

void resetAll(bool method)
{
	for (byte i = 0; i < friveCount; i++)
	{
		currentPeriod[i] = 0; // Stop playing any notes

		digitalWrite(pins[i][0], 0);
		for (int pos=0; pos<MAX_POSITIONS[i]; pos++) // MAX_POSITIONS[i] is divided by two because this isn't toggling
		{
			digitalWrite(pins[i][1], 1);
			delay(1);
			digitalWrite(pins[i][1], 0);
			delay(1);
		}

		digitalWrite(pins[i][0], 1);
		for (int pos=0; pos<(method ? MAX_POSITIONS[i]/2 : MAX_POSITIONS[i]); pos++) // Stop halfway if using the oscillating method
		{
			digitalWrite(pins[i][1], 1);
			delay(1);
			digitalWrite(pins[i][1], 0);
			delay(1);
		}
		digitalWrite(pins[i][0], 0);
	}
}

void serialLoop(int fd, unsigned int currentPeriod[], const int notes[])
{
	signed char data;
	byte note;
	byte frive;
	while(1)
	{
		// Read the serial pins hooked up to the other pi to set the notes
		data = serialGetchar(fd);
		if (data==-1) continue; // timeout catcher

		// Get bits out of received char
		note = (data >> 3) & 0b00011111;
		frive = data & 0b00000111;
		if (note < 32 && frive < 8) // Make sure recieved values are in range
			currentPeriod[frive] = stepmethod ? notes[note] : notes[note]*2;

		digitalWrite(pins[frive][2], int(note!=0)); // Turn on the LED
	}
}

void serialLoop2(int fd, unsigned int currentPeriod[], const int notes[])
{
	signed char notedata;
	signed char frivedata; 
	byte note;
	byte frive;
	while(1)
	{
		// Read the serial pins hooked up to the other pi to set the notes
		notedata = serialGetchar(fd);
		if (notedata==-1) continue; // timeout catcher
		frivedata = serialGetchar(fd);
		if (frivedata==-1) continue; // timeout catcher

		// Get bits out of received char
		note = notedata & 0b00111111; // The note can only be between 0-64
		frive = frivedata & 0b00000111; // The frive can only be between 0-8
		if (note < 64 && frive < 8) // Make sure recieved values are in range
			currentPeriod[frive] = stepmethod ? notes[note] : notes[note]*2;

		digitalWrite(pins[frive][2], int(note!=0)); // Turn on the LED
	}
}

void onExit(int s)
{
	std::cout << "Resetting pins to 0... ";
	for (byte i = 0; i < friveCount; i++)
	{
		digitalWrite(pins[i][0], 0);
		digitalWrite(pins[i][1], 0);
		digitalWrite(pins[i][2], 0);
	}
	std::cout << "Done." << std::endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc > 4 || argc < 2) // First argument is the program name, never forgetti
	{
		std::cout << "Invalid number of arguments." << std::endl;
		return 1;
	}
	stepmethod = argv[1][0]-48; // Get first char of input (only one char should be the input) then subtract 48 from the ascii number

	std::cout << "Binding exit cleanup function... " << std::flush;
	signal(SIGINT, onExit);
	delay(500);
	std::cout << "Done." << std::endl;

	std::cout << "Setting up... " << std::flush;
	int fd = setup();
	delay(500);
	std::cout << "Done." << std::endl;

	if (argc < 4 or bool(argv[3][0]-48)) // Fourth optional argument for whether to reset or not
	{
		std::cout << "Resetting frives... " << std::flush;
		resetAll(0); // For now just always reset it fully because it seems to make a louder noise when oscillating
		delay(500);
		std::cout << "Done." << std::endl;
	}

	std::cout << "Starting serial thread loop... " << std::flush;
	bool serialTwo = bool(argv[2][0]-48);
	std::cout << "Using " << (serialTwo ? "two" : "one") << " byte communication... " << std::flush;
	const int *notes = serialTwo ? notes64 : notes32;
	std::thread sl((serialTwo ? serialLoop2 : serialLoop), fd, std::ref(currentPeriod), std::ref(notes));
	delay(500);
	std::cout << "Done." << std::endl;

	std::cout << "Ready to begin." << std::endl;

	while(1)
	{
		tick();
	}

	return 0;
}
