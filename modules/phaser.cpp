#include "dsp.h"
#include "phaser.h"
#include <math.h>

using namespace daisysp;

//PhaserEngine stuff
void PhaserEngine::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    del_.Init();
    lfo_amp_  = 0.f;
    feedback_ = .2f;
    SetFreq(200.f);

    lfo_phase_ = 0.f;
    SetLfoFreq(.3);
    SetLfoDepth(.9);
}

float PhaserEngine::Process(float in)
{
    float lfo_sig = ProcessLfo();
    del_.SetDelay(lfo_sig + ap_freq_);

    float out = del_.Read();
    del_.Write(in + out * feedback_);

    return (in + out) * .5f; //equal mix
}

void PhaserEngine::SetLfoDepth(float depth)
{
    depth    = fclamp(depth, 0.f, .93f);
    lfo_amp_ = depth * ap_freq_;
}

void PhaserEngine::SetLfoFreq(float lfo_freq)
{
    lfo_freq = 4.f * lfo_freq / sample_rate_;
    lfo_freq *= lfo_freq_ < 0.f ? -1.f : 1.f;  //if we're headed down, keep going
    lfo_freq_ = fclamp(lfo_freq, -.25f, .25f); //clip at +/- .125 * sr
}

void PhaserEngine::SetFreq(float ap_freq)
{
	ap_freq = fmax(0.f, ap_freq); //clip at 0
}

void PhaserEngine::SetFeedback(float feedback)
{
    feedback_ = fclamp(feedback, 0.f, 1.f);
}

float PhaserEngine::ProcessLfo()
{
    lfo_phase_ += lfo_freq_;

    //wrap around and flip direction
    if(lfo_phase_ > 1.f)
    {
        lfo_phase_ = 1.f - (lfo_phase_ - 1.f);
        lfo_freq_ *= -1.f;
    }
    else if(lfo_phase_ < -1.f)
    {
        lfo_phase_ = -1.f - (lfo_phase_ + 1.f);
        lfo_freq_ *= -1.f;
    }

    return lfo_phase_ * lfo_amp_;
}

//Phaser Stuff
void Phaser::Init(float sample_rate)
{
    engines_[0].Init(sample_rate);
    engines_[1].Init(sample_rate);

	poles_ = 4;
    gain_frac_ = .5f;
}

float Phaser::Process(float in)
{
	float sig = 0.f;
	
    for(int i = 0; i < poles_; i++)
    {
        sig += engines_[i].Process(in);
    }

    return sig;
}

void Phaser::SetPoles(int poles){
	poles_ = DSY_CLAMP(poles, 1, 8);
}

void Phaser::SetLfoDepth(float depth)
{
	for(int i = 0; i < MAX_POLES; i++){
		engines_[i].SetLfoDepth(depth);
	}
}

void Phaser::SetLfoFreq(float lfo_freq)
{
	for(int i = 0; i < MAX_POLES; i++){
		engines_[i].SetLfoFreq(lfo_freq);
	}
}

void Phaser::SetFreq(float ap_freq)
{
	for(int i = 0; i < MAX_POLES; i++){
		engines_[i].SetFreq(ap_freq);
	}
}

void Phaser::SetFeedback(float feedback)
{
	for(int i = 0; i < MAX_POLES; i++){
		engines_[i].SetFeedback(feedback);
	}
}