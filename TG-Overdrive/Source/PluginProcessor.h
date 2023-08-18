/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



//==============================================================================
template <typename Type>
class Distortion
{
public:
	//==============================================================================
	Distortion()
	{
		auto& waveshaper = processorChain.template get<waveshaperIndex>(); // [5]
		//waveshaper.functionToUse = [] (Type x) { return jlimit (Type (-0.1), Type (0.1), x); };

		waveshaper.functionToUse = [](Type x)
		{
			return std::tanh(x);
		};

		auto& preGain = processorChain.template get<preGainIndex>();
		preGain.setGainDecibels(20.0f);

		auto& postGain = processorChain.template get<postGainIndex>();
		//postGain.setGainDecibels (-20.0f);
		postGain.setGainDecibels(-5.0f);
	}

	//==============================================================================
	void prepare(const juce::dsp::ProcessSpec& spec)
	{
		//auto& filter = processorChain.template get<filterIndex>();                      // [3]
		//filter.state = FilterCoefs::makeFirstOrderHighPass(spec.sampleRate, 1000.0f);  // [4]

		processorChain.prepare(spec);
	}

	//==============================================================================
	template <typename ProcessContext>
	void process(const ProcessContext& context) noexcept
	{
		processorChain.process(context); // [7]
	}

	//==============================================================================
	void reset() noexcept
	{
		processorChain.reset();     // [3]
	}
private:
	//==============================================================================
	enum {
		preGainIndex,
		waveshaperIndex,
		postGainIndex
	};

	using Filter = juce::dsp::IIR::Filter<float>;
	using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

	juce::dsp::ProcessorChain <juce::dsp::Gain<float>, juce::dsp::WaveShaper<float>, juce::dsp::Gain<float>> processorChain;
};

//==============================================================================
/**
*/
class OverdriveAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    OverdriveAudioProcessor();
    ~OverdriveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
	//==============================================================================

	Distortion<float> distortion;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverdriveAudioProcessor)
};