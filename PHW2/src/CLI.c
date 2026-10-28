#include "ourGlobals.h"

char commandBuf[COMMAND_SIZE];
uint8_t commandIndex = 0;

// Parses the integer starting at commandBuf[startIndex] up to the newline character;
// Returns -1 if invalid; otherwise returns the integer
int parseIntToNull(uint8_t startIndex)
{
	int value = 0;
	char c;
	while ((c = commandBuf[startIndex++]) != '\0')
	{
		// check for invalid chars
		if (c < '0' || c > '9')
			return -1;

		value *= 10;
		value += (c - '0');

		// check for too many digits (we'll say over 4)
		if (value > 9999)
			return -1;
	}

	return value;
}

uint8_t nextIndex(uint8_t index)
{
	if (++index >= RING_SIZE)
		return 0;
	else
		return index;
}

void ProcessRingBuf(void)
{
	// Copy unread chars from ring buffer into command buffer
	while (ringTail != ringHead)
	{
		// copy one character
		commandBuf[commandIndex] = ringBuf[ringTail];
		ringTail = nextIndex(ringTail); // advance tail ptr

		if (commandBuf[commandIndex] == '\n')
		{									 // end of a command
			commandBuf[commandIndex] = '\0'; // add null terminator to support string ops
			ProcessCommand();
			commandIndex = 0; // reset command buffer
		}
		else
		{
			// advance command buffer index and check for overflow
			if (++commandIndex >= COMMAND_SIZE)
			{
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
		if (commandBuf[1] == '\0')
		{
			// no number specified
			channelNum = 12;
		}
		else
		{
			channelNum = parseIntToNull(1); // start at index 1 and attempt to parse int
		}

		if (channelNum == 12 || channelNum == 26 || channelNum == 27)
		{
			ADC_Channel = channelNum;

			printf("Channel %d selected.", channelNum);
		}
		// ignore command if invalid integer
		break;
	case 'P':
		int samplePeriod;
		if (commandBuf[1] == '\0')
		{
			// no number specified
			samplePeriod = 100;
		}
		else
		{
			samplePeriod = parseIntToNull(1); // start at index 1 and attempt to parse int
		}
		if (samplePeriod >= 5 && samplePeriod <= 1000)
		{
			Set_PIT_Period(samplePeriod);
			printf("Sampling period = %d us.", samplePeriod);
		}
		// ignore command if invalid integer
		break;
	case 'N':
		int numSamplesIn;
		if (commandBuf[1] == '\0')
		{
			// no number specified
			numSamplesIn = 20;
		}
		else
		{
			numSamplesIn = parseIntToNull(1); // start at index 1 and attempt to parse int
		}
		if (numSamplesIn >= 1 && numSamplesIn <= 2000)
		{
			numSamples = numSamplesIn; // set the global that determines sample count
			printf("Sample count = %d.", numSamples);
		}
		// ignore command if invalid integer
		break;
	case 'A':
		// Only process the command if it has no arguments (any passed parameters would be invalid)
		if (commandBuf[1] == '\0')
		{
			printf("Awaiting trigger.");

			state = AWAIT_STATE;

			Set_ADC_Waiting_Trigger();
		}
		break;
	case 'F':
		// Only process the command if it has no arguments (any passed parameters would be invalid)
		if (commandBuf[1] == '\0')
		{
			printf("Recording started.");

			state = RECORD_STATE;

			Set_ADC_Periodic_Sampling();
		}
		break;
	case 'S':
		// Only process the command if it has no arguments (any passed parameters would be invalid)
		if (commandBuf[1] == '\0')
		{
			state = SEND_STATE;
		}
		break;
	default:
		// Give no indication of an invalid command; just start a newline with the >
		break;
	}

	printf("\n> ");
}

void Send_Data(void)
{
	printf("Sending data (%d samples).\n", numSamples);

	uint16_t i;

	for (i = 0; i < numSamples; i++)
	{
		printf("%d: %.3f V\n", i, ((float)dataBuffer[i] / 65536) * 3.3f);
	}

	state = CLI_STATE;
}