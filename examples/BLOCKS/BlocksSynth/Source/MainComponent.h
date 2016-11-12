
#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Audio.h"
#include "WaveshapeProgram.h"

//==============================================================================
/**
    A struct that handles the setup and layout of the DrumPadGridProgram
*/
struct SynthGrid
{
    SynthGrid (int cols, int rows)
        : numColumns (cols),
          numRows (rows)
    {
        constructGridFillArray();
    }

    /** Creates a GridFill object for each pad in the grid and sets its colour
        and fill before adding it to an array of GridFill objects
     */
    void constructGridFillArray()
    {
        gridFillArray.clear();

        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < numColumns; ++j)
            {
                DrumPadGridProgram::GridFill fill;

                int padNum = (i * 5) + j;

                fill.colour =  notes.contains (padNum) ? baseGridColour
                                                       : tonics.contains (padNum) ? Colours::white
                                                                                  : Colours::black;
                fill.fillType = DrumPadGridProgram::GridFill::FillType::gradient;
                gridFillArray.add (fill);
            }
        }
    }

    int getNoteNumberForPad (int x, int y) const
    {
        int xIndex = x / 3;
        int yIndex = y / 3;

        return 60 + ((4 - yIndex) * 5) + xIndex;
    }

    //==============================================================================
    int numColumns, numRows;
    float width, height;

    Array<DrumPadGridProgram::GridFill> gridFillArray;
    Colour baseGridColour = Colours::green;
    Colour touchColour = Colours::cyan;

    Array<int> tonics = { 4, 12, 20 };
    Array<int> notes = { 1, 3, 6, 7, 9, 11, 14, 15, 17, 19, 22, 24 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthGrid)
};

//==============================================================================
/**
    The main component
*/
class MainComponent   : public Component,
                        public TopologySource::Listener,
                        private TouchSurface::Listener,
                        private ControlButton::Listener
{
public:
    MainComponent()
    {
        setSize (600, 400);

        // Register MainContentComponent as a listener to the PhysicalTopologySource object
        topologySource.addListener (this);
    };

    ~MainComponent()
    {
        if (activeBlock != nullptr)
            detachActiveBlock();
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::lightgrey);
        g.drawText ("Connect a Lightpad Block to play.",
                    getLocalBounds(), Justification::centred, false);
    }

    void resized() override {}

    /** Overridden from TopologySource::Listener, called when the topology changes */
    void topologyChanged() override
    {
        // Reset the activeBlock object
        if (activeBlock != nullptr)
            detachActiveBlock();

        // Get the array of currently connected Block objects from the PhysicalTopologySource
        auto blocks = topologySource.getCurrentTopology().blocks;

        // Iterate over the array of Block objects
        for (auto b : blocks)
        {
            // Find the first Lightpad
            if (b->getType() == Block::Type::lightPadBlock)
            {
                activeBlock = b;

                // Register MainContentComponent as a listener to the touch surface
                if (auto surface = activeBlock->getTouchSurface())
                    surface->addListener (this);

                // Register MainContentComponent as a listener to any buttons
                for (auto button : activeBlock->getButtons())
                    button->addListener (this);

                // Get the LEDGrid object from the Lightpad and set its program to the program for the current mode
                if (auto grid = activeBlock->getLEDGrid())
                {
                    // Work out scale factors to translate X and Y touches to LED indexes
                    scaleX = static_cast<float> (grid->getNumColumns() - 1) / activeBlock->getWidth();
                    scaleY = static_cast<float> (grid->getNumRows() - 1)    / activeBlock->getHeight();

                    setLEDProgram (grid);
                }

                break;
            }
        }
    }

private:
    /** Overridden from TouchSurface::Listener. Called when a Touch is received on the Lightpad */
    void touchChanged (TouchSurface&, const TouchSurface::Touch& touch) override
    {
        if (currentMode == waveformSelectionMode && touch.isTouchStart)
        {
            // Change the displayed waveshape to the next one
            ++waveshapeMode;

            if (waveshapeMode > 3)
                waveshapeMode = 0;
            
            waveshapeProgram->setWaveshapeType (waveshapeMode);
        }
        else if (currentMode == playMode)
        {
            // Translate X and Y touch events to LED indexes
            int xLed = roundToInt (touch.startX * scaleX);
            int yLed = roundToInt (touch.startY * scaleY);

            // Limit the number of touches per second
            constexpr int maxNumTouchMessagesPerSecond = 100;
            auto now = Time::getCurrentTime();
            clearOldTouchTimes (now);

            int midiChannel = waveshapeMode + 1;

            // Send the touch event to the DrumPadGridProgram and Audio class
            if (touch.isTouchStart)
            {
                gridProgram->startTouch (touch.startX, touch.startY);
                audio.noteOn (midiChannel, layout.getNoteNumberForPad (xLed, yLed), touch.z);
            }
            else if (touch.isTouchEnd)
            {
                gridProgram->endTouch (touch.startX, touch.startY);
                audio.noteOff (midiChannel, layout.getNoteNumberForPad (xLed, yLed), 1.0);
            }
            else
            {
                if (touchMessageTimesInLastSecond.size() > maxNumTouchMessagesPerSecond / 3)
                    return;

                gridProgram->sendTouch (touch.x, touch.y, touch.z,
                                        layout.touchColour);

                // Send pitch change and pressure values to the Audio class
                audio.pitchChange (midiChannel, (touch.x - touch.startX) / activeBlock->getWidth());
                audio.pressureChange (midiChannel, touch.z);
            }

            touchMessageTimesInLastSecond.add (now);
        }
    }

    /** Overridden from ControlButton::Listener. Called when a button on the Lightpad is pressed */
    void buttonPressed (ControlButton&, Block::Timestamp) override {}

    /** Overridden from ControlButton::Listener. Called when a button on the Lightpad is released */
    void buttonReleased (ControlButton&, Block::Timestamp) override
    {
        // Turn any active synthesiser notes off
        audio.allNotesOff();

        // Switch modes
        if (currentMode == waveformSelectionMode)
            currentMode = playMode;
        else if (currentMode == playMode)
            currentMode = waveformSelectionMode;

        // Set the LEDGrid program to the new mode
        setLEDProgram (activeBlock->getLEDGrid());
    }

    /** Clears the old touch times */
    void clearOldTouchTimes (const Time now)
    {
        for (int i = touchMessageTimesInLastSecond.size(); --i >= 0;)
            if (touchMessageTimesInLastSecond.getReference(i) < now - juce::RelativeTime::seconds (0.33))
                touchMessageTimesInLastSecond.remove (i);
    }

    /** Removes TouchSurface and ControlButton listeners and sets activeBlock to nullptr */
    void detachActiveBlock()
    {
        if (auto surface = activeBlock->getTouchSurface())
            surface->removeListener (this);

        for (auto button : activeBlock->getButtons())
            button->removeListener (this);

        activeBlock = nullptr;
    }

    /** Sets the LEDGrid Program for the selected mode */
    void setLEDProgram (LEDGrid* grid)
    {
        if (currentMode == waveformSelectionMode)
        {
            // Create a new BitmapLEDProgram for the LEDGrid
            waveshapeProgram = new WaveshapeProgram (*grid, waveshapeMode);

            // Set the LEDGrid program
            grid->setProgram (waveshapeProgram);
        }
        else if (currentMode == playMode)
        {
            // Create a new DrumPadGridProgram for the LEDGrid
            gridProgram = new DrumPadGridProgram (*grid);

            // Set the LEDGrid program
            grid->setProgram (gridProgram);

            // Setup the grid layout
            gridProgram->setGridFills (layout.numColumns,
                                       layout.numRows,
                                       layout.gridFillArray);
        }
    }

    enum BlocksSynthMode
    {
        waveformSelectionMode = 0,
        playMode
    };

    BlocksSynthMode currentMode = playMode;

    //==============================================================================
    Audio audio;

    DrumPadGridProgram* gridProgram      = nullptr;
    WaveshapeProgram*   waveshapeProgram = nullptr;

    SynthGrid layout { 5, 5 };
    PhysicalTopologySource topologySource;
    Block::Ptr activeBlock;

    Array<juce::Time> touchMessageTimesInLastSecond;

    Colour waveshapeColour = Colours::red;

    int waveshapeMode = 0;
    
    float scaleX = 0.0;
    float scaleY = 0.0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
