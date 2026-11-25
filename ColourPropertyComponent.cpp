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

#include "ColourPropertyComponent.h"

// The clickable swatch. PropertyComponent::resized() places the first child in the content area.
class ColourPropertyComponent::Swatch : public juce::Component {
public:
    explicit Swatch(ColourPropertyComponent &owner) : owner_(owner) {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }

    void paint(juce::Graphics &g) override {
        auto colour = juce::Colour::fromString(owner_.value_.toString());
        g.setColour(colour);
        g.fillRect(getLocalBounds().reduced(4));
        g.setColour(colour.contrasting());
        g.drawText(colour.toDisplayString(owner_.showAlpha_), getLocalBounds(), juce::Justification::centred);
    }

    void mouseUp(const juce::MouseEvent &e) override {
        if (e.mouseWasClicked()) owner_.showSelector();
    }

private:
    ColourPropertyComponent &owner_;
};

// The colour selector shown in the call-out box. It writes every change straight back to the
// owner's Value, and holds only a SafePointer to the owner, because the call-out box can outlive
// the row it was opened from (e.g. when the dialog is closed while the selector is still open).
class ColourPropertyComponent::Selector : public juce::ColourSelector, private juce::ChangeListener {
public:
    Selector(ColourPropertyComponent &owner, bool showAlpha)
        : juce::ColourSelector(juce::ColourSelector::showColourAtTop | juce::ColourSelector::showSliders | juce::ColourSelector::showColourspace
                               | (showAlpha ? juce::ColourSelector::showAlphaChannel : 0)),
          owner_(&owner) {
        setCurrentColour(juce::Colour::fromString(owner.value_.toString()), juce::dontSendNotification);
        addChangeListener(this);
    }

    ~Selector() override { removeChangeListener(this); }

private:
    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        if (owner_ != nullptr) owner_->value_ = getCurrentColour().toString();
    }

    juce::Component::SafePointer<ColourPropertyComponent> owner_;
};

ColourPropertyComponent::ColourPropertyComponent(const juce::Value &valueToControl, const juce::String &propertyName, bool showAlpha)
    : juce::PropertyComponent(propertyName), value_(valueToControl), showAlpha_(showAlpha), swatch_(std::make_unique<Swatch>(*this)) {
    value_.addListener(this);
    addAndMakeVisible(*swatch_);
}

ColourPropertyComponent::~ColourPropertyComponent() {
    value_.removeListener(this);
}

void ColourPropertyComponent::refresh() {
    repaint();
    swatch_->repaint();
}

void ColourPropertyComponent::paint(juce::Graphics &g) {
    juce::PropertyComponent::paint(g);
    g.setColour(findColour(juce::BooleanPropertyComponent::backgroundColourId));
    g.fillRect(swatch_->getBounds());
    g.setColour(findColour(juce::BooleanPropertyComponent::outlineColourId));
    g.drawRect(swatch_->getBounds());
}

void ColourPropertyComponent::valueChanged(juce::Value &) {
    refresh();
}

void ColourPropertyComponent::showSelector() {
    auto selector = std::make_unique<Selector>(*this, showAlpha_);
    selector->setSize(300, 280);
    juce::CallOutBox::launchAsynchronously(std::move(selector), swatch_->getScreenBounds(), nullptr);
}
