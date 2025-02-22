/*
 * Copyright © 2015-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "definitions_cxx.hpp"
#include "dsp/filter/hpladder.h"
#include "dsp/filter/lpladder.h"
#include "dsp/filter/svf.h"
#include "model/mod_controllable/filters/filter_config.h"
#include "util/fixedpoint.h"
#include <cstdint>

class Sound;

namespace deluge::dsp::filter {

union LowPass {
	LpLadderFilter ladder;
	SVFilter svf;
	LowPass() {}
};

union HighPass {
	HpLadderFilter ladder;
	SVFilter svf;
	HighPass() {}
};

class FilterSet {
public:
	void reset();
	int32_t setConfig(q31_t lpfFrequency, q31_t lpfResonance, FilterMode lpfmode, q31_t lpfMorph, q31_t hpfFrequency,
	                  q31_t hpfResonance, FilterMode hpfmode, q31_t hpfMorph, q31_t filterGain, FilterRoute routing,
	                  bool adjustVolumeForHPFResonance, q31_t* overallOscAmplitude);

	void renderLong(q31_t* startSample, q31_t* endSample, int32_t numSamples, int32_t sampleIncrememt = 1);

	// expects to receive an interleaved stereo stream
	void renderLongStereo(q31_t* startSample, q31_t* endSample);

	// used to check whether the filter is used at all
	inline bool isLPFOn() { return LPFOn; }
	inline bool isHPFOn() { return HPFOn; }
	inline bool isOn() { return HPFOn || LPFOn; }

private:
	FilterMode lpfMode_;
	FilterMode lastLPFMode_;
	FilterMode hpfMode_;
	FilterMode lastHPFMode_;
	FilterRoute routing_;

	void renderLPFLong(q31_t* startSample, q31_t* endSample, int32_t sampleIncrement = 1);
	void renderLPFLongStereo(q31_t* startSample, q31_t* endSample);
	void renderHPFLongStereo(q31_t* startSample, q31_t* endSample);
	void renderHPFLong(q31_t* startSample, q31_t* endSample, int32_t sampleIncrement = 1);

	// all filters share a state. This is fine since they just hold plain data and initialization is handled by
	// reset/configure calls.  This is faster than using a variant at the cost of not throwing on incorrect access.
	// However since there are no invariants to uphold, the worst case scenario is an audio glitch so whatever
	// This cuts 250 bytes from the size of this class, which is fairly significant since every voice has a filterset
	LowPass lpfilter;
	HighPass hpfilter;

	bool LPFOn;
	bool HPFOn;
};
} // namespace deluge::dsp::filter
