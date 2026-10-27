#include <MKL25Z4.H>
#include "UART.h"
#include "ourGlobals.h"
#include "timers.h"

// Parses the integer starting at commandBug[startIndex] up to the newline character;
// Returns -1 if invalid; otherwise returns the integer
int parseIntToNewline(uint8_t startIndex)
{
	int value = 0;
	char c;
	while ((c = commandBuf[startIndex++]) != '\n')
	{
        // check for invalid chars
        if (c < '0' || c > '9') return -1;

		value *= 10;
		value += (c - '0');

        // check for too many digits (we'll say over 4)
        if (value > 9999) return -1;
	}

	return value;
}

uint8_t nextIndex(uint8_t index) {
    if (++index >= RING_SIZE) return 0;
	else return index;
}

void ProcessRingBuf(void) {
	// Copy unread chars from ring buffer into command buffer
	while (ringTail != ringHead) {
		// copy one character
		commandBuf[commandIndex] = ringBuf[ringTail];
		ringTail = nextIndex(ringTail); // advance tail ptr

		if (commandBuf[commandIndex] == '\n') {	// end of a command
			commandBuf[commandIndex] = '\0'; 	// add null terminator to support string ops
			ProcessCommand();	
			commandIndex = 0;					// reset command buffer
		} else {
			// advance command buffer index and check for overflow
			if (++commandIndex >= COMMAND_SIZE) {
				// OVERFLOW!! Should never happen
				// TODO: Turn on RED led
			}
		}
	}
}

void ProcessCommand(void)
{
	switch (commandBuf[0]) // first character of command
	{
	case 'C':
        int channelNum;
		if (commandBuf[1] == '\n') {
            // no number specified
            channelNum = 12;
        } else {
            channelNum = parseIntToNewline(1); // start at index 1 and attempt to parse int
            if (channelNum == 12 || channelNum == 26 || channelNum == 27)
            {
                ADC0->SC1[0] = ADC_SC1_ADCH(channelNum);

                printf("Channel %d selected.", channelNum);
            }
            // ignore command if invalid integer
        }
		break;
	case 'P':
		int samplePeriod;
		if (commandBuf[1] == '\n') {
            // no number specified
            samplePeriod = 100;
        } else {
            samplePeriod = parseIntToNewline(1); // start at index 1 and attempt to parse int
            if (samplePeriod >= 5 && samplePeriod <= 1000)
            {
				Set_PIT_Period(samplePeriod);
            }
            // ignore command if invalid integer
        }
		break;
	case 'N':
        int numSamplesIn;
		if (commandBuf[1] == '\n') {
            // no number specified
            numSamplesIn = 20;
        } else {
            numSamplesIn = parseIntToNewline(1); // start at index 1 and attempt to parse int
            if (numSamplesIn >= 1 && numSamplesIn <= 2000)
            {
                numSamples = numSamplesIn;	// set the global that determines sample count
            }
            // ignore command if invalid integer
        }
		break;
	case 'A':
		/* code */
		break;
	case 'F':
		/* code */
		break;
	case 'S':
		/* code */
		break;
	default:
		break;
	}

	// If A received, await trigger
	// If trigger met, start conversion

	// If S receieved, send data
}