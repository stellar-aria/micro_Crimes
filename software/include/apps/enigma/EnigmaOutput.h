// Copyright (c) 2018, Jason Justian
//
// Based on Braids Quantizer, Copyright 2015 Émilie Gillet.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ENIGMAOUTPUT_H
#define ENIGMAOUTPUT_H
#include <algorithm>

#include "braids/quantizer.h"
#include "braids/quantizer_scales.h"
#include "hemisphere/midi.hpp"
#include "oc/scales.h"

enum EnigmaOutputType {
  NOTE3,
  NOTE4,
  NOTE5,
  NOTE6,
  NOTE7,
  MODULATION,
  EXPRESSION,
  TRIGGER,
  GATE,
};
constexpr const char* enigma_type_names[] = {
    "Note 3-Bit", "Note 4-Bit", "Note 5-Bit", "Note 6-Bit", "Note 7-Bit",
    "Modulation", "Expression", "Trigger",    "Gate"};
constexpr const char* enigma_type_short_names[] = {
    "Note-3", "Note-4", "Note-5",  "Note-6", "Note-7",
    "Mod",    "Expr",   "Trigger", "Gate"};

class EnigmaOutput {
 private:
  // Internal data
  uint8_t out;  // Output 0-4
  braids::Quantizer quantizer;
  int last_note = -1;
  int deferred_note = -1;

 public:
  // Saved data
  uint8_t tk;  // Track 0-4
  uint8_t ty;  // Type (see enum above)
  uint8_t sc;  // Scale
  uint8_t mc;  // MIDI channel (1-16, 0=off)

  void InitAs(uint8_t o) {
    out = o;
    tk = o;
    sc = 5;
    mc = 0;

    // Initialize quantizer
    quantizer.Init();
    quantizer.Configure(oc::Scales::GetScale(sc), 0xffff);

    // Set type based on output initialization
    if (o == 0) set_type(EnigmaOutputType::NOTE5);
    if (o == 1) set_type(EnigmaOutputType::MODULATION);
    if (o == 2) set_type(EnigmaOutputType::TRIGGER);
    if (o == 3) set_type(EnigmaOutputType::GATE);
  }

  // Getters
  uint8_t output() { return out; }
  uint8_t track() { return tk; }
  uint8_t type() { return ty; }
  uint8_t scale() { return sc; }
  uint8_t midi_channel() { return mc; }
  int GetDeferredNote() { return deferred_note; }

  // Setters
  void set_output(uint8_t output_) { out = std::clamp<uint8_t>(output_, 0, 3); }
  void set_track(uint8_t track_) { tk = std::clamp<uint8_t>(track_, 0, 3); }
  void set_type(uint8_t type_) {
    ty = std::clamp<uint8_t>(type_, 0, EnigmaOutputType::GATE);
  }
  void set_scale(uint8_t scale_) {
    sc = std::clamp<uint8_t>(scale_, 0, oc::Scales::NUM_SCALES - 1);
    quantizer.Init();
    quantizer.Configure(oc::Scales::GetScale(sc), 0xffff);
  }
  void set_midi_channel(uint8_t midi_channel_) {
    mc = std::clamp<uint8_t>(midi_channel_, 0, 16);
  }
  void SetDeferredNote(int midi_note_) { deferred_note = midi_note_; }

  /* Sends data to an output based on the current output type. The I/O methods
   * are public methods of the app, so make sure to send a type that supports
   * Out, ClockOut, and GateOut (i.e. HSApplication, or HemisphereApplet)
   */
  template <class C>
  void SendToDAC(C* app, uint16_t reg, int transpose = 0) {
    // Quantize a note based on how many bits
    if (ty <= EnigmaOutputType::NOTE7) {
      uint8_t bits = ty + 3;  // Number of bits
      uint8_t mask = 0;
      for (uint8_t s = 0; s < bits; s++) mask |= (0x01 << s);
      int note_shift = ty == EnigmaOutputType::NOTE7
                           ? 0
                           : 64;  // Note types under 7-bit start at Middle C
      int note_number = (reg & mask) + note_shift;
      note_number = std::clamp(note_number, 0, 127);
      app->Out(out, quantizer.Lookup(note_number) + transpose);
    }

    // Modulation based on low 8 bits
    if (ty == EnigmaOutputType::MODULATION ||
        ty == EnigmaOutputType::EXPRESSION) {
      app->Out(out, (reg & 0x00ff) * 6);
    }

    // Trigger sends a clock when low bit is high
    bool clock = reg & 0x0001;
    if (ty == EnigmaOutputType::TRIGGER && clock) {
      app->ClockOut(out);
    }

    // Gate goes high or low based on the low bit, and stays there until changed
    if (ty == EnigmaOutputType::GATE) {
      app->GateOut(out, clock);
    }
  }

  /* Sends data vi MIDI based on the current output type. */
  void SendToMIDI(uint16_t reg, int transpose = 0) {
    // Quantize a note based on how many bits
    if (ty <= EnigmaOutputType::NOTE7) {
      uint8_t bits = ty + 3;  // Number of bits
      uint8_t mask = 0;
      for (uint8_t s = 0; s < bits; s++) mask |= (0x01 << s);
      int note_shift = ty == EnigmaOutputType::NOTE7
                           ? 0
                           : 60;  // Note types under 7-bit start at Middle C
      int note_number = (reg & mask) + note_shift + (transpose / 128);
      note_number = std::clamp(note_number, 0, 127);

      if (midi_channel()) {
        if (last_note > -1) {
          usbMIDI.SendNoteOn( midi_channel(), last_note, 0);
        }
        usbMIDI.SendNoteOn( midi_channel(), note_number, 0x60);
        last_note = note_number;
      } else {
        deferred_note = note_number;
      }
    }

    // Modulation based on low 8 bits, shifted right for MIDI range
    if (ty == EnigmaOutputType::MODULATION && midi_channel()) {
      usbMIDI.SendControlChange( midi_channel(), 1, (reg & 0x00ff) >> 1);
    }

    // Expression based on low 8 bits; for MIDI, expression is a percentage of
    // channel volume
    if (ty == EnigmaOutputType::EXPRESSION && midi_channel()) {
      usbMIDI.SendControlChange( midi_channel(), 11, (reg & 0x00ff) >> 1);
    }

    // Trigger and Gate behave the same way with MIDI; They'll use the last note
    // that wasn't sent out via MIDI on its own output. If no such note is
    // available, then Trigger/Gate will do nothing.
    if ((ty == EnigmaOutputType::TRIGGER || ty == EnigmaOutputType::TRIGGER) &&
        midi_channel()) {
      if (deferred_note > -1 && (reg & 0x01)) {
        if (last_note > -1) {
          usbMIDI.SendNoteOff( midi_channel(), last_note, 0);
        }
        usbMIDI.SendNoteOn( midi_channel(), deferred_note, 0x60);
        last_note = deferred_note;
        deferred_note = -1;
      }
    }
  }

  void NoteOff() {
    if (midi_channel()) {
      if (last_note > -1) {
        usbMIDI.SendNoteOn( midi_channel(), last_note, 0);
      }
      last_note = -1;
      deferred_note = -1;
    }
  }
};

#endif  // ENIGMAOUTPUT_H
