#include "Ambi2binIRContainer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Ambi2binIRContainer::Ambi2binIRContainer()
{
	// load HRIR, ITD, ILD, etc.
	File hoa2binFile = getFileFromString("irs/hoa2bin_order2_IRC_1008_R_HRIR.bin");
	loadIR(hoa2binFile);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Ambi2binIRContainer::loadIR(const File & filename)
// load Ambisonic to binaural room impulse response from file
{
	FileInputStream istream(filename);
	if (istream.openedOk())
	{
		for (int j = 0; j < N_AMBI_CH; ++j) // loop ambi channels
		{
			for (int i = 0; i < AMBI2BIN_IR_LENGTH; ++i) // extract left ear IRs
			{
				istream.read(&ambi2binIrDict[j][0][i], sizeof(float));
			}

			for (int i = 0; i < AMBI2BIN_IR_LENGTH; ++i) // extract right ear IRs
			{
				istream.read(&ambi2binIrDict[j][1][i], sizeof(float));
			}
		}
	}
	else { throw std::ios_base::failure("Failed to open ABIR file"); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////