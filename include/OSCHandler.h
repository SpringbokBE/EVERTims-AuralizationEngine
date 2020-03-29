#ifndef OSCHANDLER_H_INCLUDED
#define OSCHANDLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"
#include <map>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class OSCHandler :
	public Component,
	private OSCReceiver,
	public OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>,
	public ChangeBroadcaster
{
	public:

		OSCHandler();
		~OSCHandler(){};

		std::vector<int> getSourceImageIDs();
		std::vector<float> getSourceImageDelays();
		std::vector<float> getSourceImagePathsLength();
		std::vector<Eigen::Vector3f> getSourceImageDOAs();
		std::vector<Eigen::Vector3f> getSourceImageDODs();
		Array<float> getSourceImageAbsorption(const unsigned int sourceID);
		std::vector<float> getRT60Values();
		int getDirectPathId();
		String getMapContentForGUI();
		String getMapContentForLog();
		void clear(const bool force);
		void updateInternals();

	private:
    
		void oscMessageReceived(const OSCMessage& msg) override;
		void oscBundleReceived(const OSCBundle& bundle) override;
		void showConnectionErrorMessage(const String& messageText);

		int port = 3860;

		// Prepare struct for thread safe update (pointer swap based)
		struct localVariablesStruct
		{
			std::map<int,EL_ImageSource> sourceImageMap;
			std::map<String,EL_Source> sourceMap;
			std::map<String,EL_Listener> listenerMap;
			std::vector<float> valuesR60;
		};
    
		localVariablesStruct *current = new localVariablesStruct();
		localVariablesStruct *future = new localVariablesStruct();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCHandler)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // OSCHANDLER_H_INCLUDED