#include <JuceHeader.h>
#include <fstream>

#include "UI/MainComponent.h"

class ResonixApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Resonix"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String&) override
    {
    }

private:
    class MainWindow : public juce::AudioAppComponent,
                        private juce::Timer
    {
    public:
        explicit MainWindow(const juce::String& name)
            : juce::AudioAppComponent()
        {
            setName(name);
            setSize(1240, 760);
            setOpaque(true);
            setLookAndFeel(&darkLAF);
            
            setAudioChannels(0, 2);
            centreWithSize(1240, 760);
            setVisible(true);
            
            startTimerHz(30);
            
            try { std::ofstream dbg("/tmp/resonix_mainwindow_ctor.txt"); dbg << "MainWindow created, size=" << getWidth() << "x" << getHeight() << "\n"; dbg.close(); } catch(...) {}
            
            // This is a fallback test: write a simple status file to confirm the ctor ran
            try { std::ofstream dbg("/tmp/resonix_ctor_success.txt"); dbg << "Constructor completed successfully\n"; dbg.close(); } catch(...) {}
            
            addMainContent();
        }

        void addMainContent()
        {
            content = std::make_unique<nebula::ui::MainComponent>();
            addAndMakeVisible(content.get());
        }

        void paint(juce::Graphics& g) override
        {
            // MainWindow paint - let child components handle rendering
            g.fillAll(juce::Colour(0xff333333));
        }

        ~MainWindow() override
        {
            setLookAndFeel(nullptr);
            shutdownAudio();
        }

        void prepareToPlay(int samplesPerBlockExpected, double newSampleRate) override
        {
            if (content)
                content->getAudioEngine().prepare(newSampleRate, samplesPerBlockExpected);
        }

        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
        {
            if (content && bufferToFill.buffer)
                content->getAudioEngine().process(*bufferToFill.buffer);
        }

        void releaseResources() override
        {
        }

        void resized() override
        {
            if (content)
                content->setBounds(getLocalBounds());
        }

    private:
        void timerCallback() override
        {
            repaint();
        }

        struct DarkLAF : public juce::LookAndFeel_V4
        {
            DarkLAF() { setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff1a1a1a)); }
        };

        DarkLAF darkLAF;
        std::unique_ptr<nebula::ui::MainComponent> content;
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(ResonixApplication)
