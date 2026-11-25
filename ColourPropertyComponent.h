/*
 * MIT License
 *
 * Copyright (c) 2019-2025 Christof Ruch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

// A property row showing a colour swatch with its hex value. Clicking the swatch opens a
// juce::ColourSelector in a call-out box. The colour is stored in the Value as a
// juce::Colour::toString() string (AARRGGBB hex), the same format gin::ColourPropertyComponent uses,
// so it is a drop-in replacement for that class.
class ColourPropertyComponent : public juce::PropertyComponent, private juce::Value::Listener {
public:
    ColourPropertyComponent(const juce::Value &valueToControl, const juce::String &propertyName, bool showAlpha = false);
    ~ColourPropertyComponent() override;

    juce::Value &getValueObject() { return value_; }

    void refresh() override;
    void paint(juce::Graphics &g) override;

private:
    class Swatch;
    class Selector;

    void valueChanged(juce::Value &) override;
    void showSelector();

    juce::Value value_;
    bool showAlpha_;
    std::unique_ptr<Swatch> swatch_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourPropertyComponent)
};
