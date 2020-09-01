#include <array>
#include <math.h>

#include "BinauralEncoder.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

BinauralEncoder::BinauralEncoder()
{
	// load HRIR filters
	File hrirFile = getFileFromString("irs/ClubFritz1_hrir.bin");
	loadHrir(hrirFile);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void BinauralEncoder::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
// local equivalent of prepareToPlay
{
	// resize FIR
	for (int i = 0; i < 2; i++)
	{
		hrirFirFuture[i].init(samplesPerBlockExpected, HRIR_LENGTH);
		hrirFir[i].init(samplesPerBlockExpected, HRIR_LENGTH);
	}

	// resize buffers
	workingBuffer.setSize(1, samplesPerBlockExpected);

	// keep local copies
	localSampleRate = sampleRate;
	localSamplesPerBlockExpected = samplesPerBlockExpected;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void BinauralEncoder::encodeBuffer(const AudioBuffer<float>& source, AudioBuffer<float>& destination)
// binaural encoding of source 1st channel (mono) to destination (stereo)
{
	// update crossfade
	updateCrossfade();

	for (int i = 0; i < 2; i++)
	{
		// copy 1st channel of mono source to ith channel of destination
		destination.copyFrom(i, 0, source, 0, 0, localSamplesPerBlockExpected);

		if (crossfadeOver)
		{
			// simply apply FIR
			hrirFir[i].process(destination.getWritePointer(i));
		}
		else
		{
			// duplicate input buffer
			workingBuffer.copyFrom(0, 0, destination, i, 0, localSamplesPerBlockExpected);

			// apply past and future FIRs
			hrirFir[i].process(destination.getWritePointer(i));
			hrirFirFuture[i].process(workingBuffer.getWritePointer(0));

			// crossfade mix
			destination.applyGain(i, 0, localSamplesPerBlockExpected, 1.0f - crossfadeGain);
			workingBuffer.applyGain(crossfadeGain);
			destination.addFrom(i, 0, workingBuffer, 0, 0, localSamplesPerBlockExpected);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void BinauralEncoder::updateCrossfade()
// update crossfade mechanism
{
	// either update crossfade
	if (crossfadeGain < 1.0)
	{
		crossfadeGain = fmin(crossfadeGain + 0.1, 1.0);
	}
	// or stop crossfade mechanism if not already stopped
	else if (!crossfadeOver)
	{
		// set past = future
		for (int earId = 0; earId < 2; earId++)
		{
			hrirFir[earId] = hrirFirFuture[earId];
		}

		// reset crossfade internals
		crossfadeGain = 1.0; // just to make sure for the last loop using crossfade gain
		crossfadeOver = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void BinauralEncoder::setPosition(double azim, double elev)
// set current HRIR filters
{
	// get azim / elev indices in hrir array along with associated gains for panning across HRIR
	// (panning across 4 nearest neighbors (in azim/elev) of current position

	// TODO: fix temporary workaround
	if (azim < -M_PI || azim > M_PI || elev < -M_PI / 2 || elev > M_PI / 2) return;

	// make sure values are in expected range
	jassert(azim >= -M_PI && azim <= M_PI);
  jassert(elev >= -M_PI / 2 && elev <= M_PI / 2);

	// rad 2 deg
	azim *= (180 / M_PI);
	elev *= (180 / M_PI);

	// mapping to hrir coordinates
	elev += 90.f;
	azim *= -1.f;
	if (azim < 0) { azim += 360.f; }

	// get azim / elev neighboring indices
	azimId[0] = fmod(std::floor(azim / AZIM_STEP), N_AZIM_VALUES);
	azimId[1] = azimId[0] + 1;
	elevId[0] = fmod(std::floor(elev / ELEV_STEP), N_ELEV_VALUES);
	elevId[1] = elevId[0] + 1;

	// deal with extrema
	if (elevId[0] == (N_ELEV_VALUES - 1)) { elevId[1] = elevId[0] - 1; }

	// get associated gains
	double azimGainHigh = fmod((azim / AZIM_STEP), N_AZIM_VALUES) - azimId[0];
	double elevGainHigh = fmod((elev / ELEV_STEP), N_ELEV_VALUES) - elevId[0];

	// fill hrir array
	for (int earId = 0; earId < 2; earId++)
	{
		for (int i = 0; i < hrir[0].size(); ++i)
		{
			hrir[earId][i] =
				(1.0f - azimGainHigh) * (1.0f - elevGainHigh) * hrirDict[azimId[0]][elevId[0]][earId][i] // low low
				+ (1.0f - azimGainHigh) * elevGainHigh * hrirDict[azimId[0]][elevId[1]][earId][i] // low high
				+ azimGainHigh * (1.0f - elevGainHigh) * hrirDict[azimId[1]][elevId[0]][earId][i] // high low
				+ azimGainHigh * elevGainHigh * hrirDict[azimId[1]][elevId[1]][earId][i]; // high high
		}

		// update FIR content
		hrirFirFuture[earId].setImpulseResponse(hrir[earId].data());
	}

	// trigger crossfade mechanism
	crossfadeGain = 0.0f;
	crossfadeOver = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void BinauralEncoder::loadHrir(const File& hrirFile)
// load a given HRIR set
{
	// open file
	FileInputStream istream_hrir(hrirFile);
	if (istream_hrir.openedOk())
	{
		// if hrirDict_ has already been filled with an HRIR, skip insert step below
		bool rewriteHrirDict = false;
		if (hrirDict.size() > 0) { rewriteHrirDict = true; }

		// loop over array first dimension (azim values)
		for (int j = 0; j < N_AZIM_VALUES; ++j) // loop azim
		{
			// resize
			if (!rewriteHrirDict)
			{
				hrirDict.insert(std::make_pair(j, std::array<HrirBuffer, N_ELEV_VALUES>()));
			}

			// loop over elev values
			for (int i = 0; i < N_ELEV_VALUES; ++i) // loop elev
			{
				istream_hrir.read(hrirDict[j][i][0].data(), HRIR_LENGTH * sizeof(float));
				istream_hrir.read(hrirDict[j][i][1].data(), HRIR_LENGTH * sizeof(float));
			}
		}
	}
	// if file failed to open
	else { throw std::ios_base::failure("Failed to open HRIR file"); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////