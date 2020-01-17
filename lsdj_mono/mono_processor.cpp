#include "mono_processor.hpp"

namespace lsdj
{
    void convertInstrument(lsdj_instrument_t* instrument)
    {
        if (instrument != nullptr && lsdj_instrument_get_panning(instrument) != LSDJ_PAN_NONE)
            lsdj_instrument_set_panning(instrument, LSDJ_PAN_LEFT_RIGHT);
    }

    void convertCommand(lsdj_command_t* command)
    {
        if (command->command == LSDJ_COMMAND_O && command->value != LSDJ_PAN_NONE)
        {
            command->value = LSDJ_PAN_LEFT_RIGHT;
        }
    }

    void convertTable(lsdj_table_t* table)
    {
        if (table == nullptr)
            return;
        
        for (int i = 0; i < LSDJ_TABLE_LENGTH; ++i)
        {
            convertCommand(lsdj_table_get_command1(table, i));
            convertCommand(lsdj_table_get_command2(table, i));
        }
    }

    void convertPhrase(lsdj_phrase_t* phrase)
    {
        if (phrase == nullptr)
            return;
        
        for (int i = 0; i < LSDJ_PHRASE_LENGTH; ++i)
            convertCommand(&phrase->commands[i]);
    }

    [[nodiscard]] bool alreadyEndsWithMono(const boost::filesystem::path& path)
    {
        const auto stem = path.stem().string();
        return stem.size() >= 5 && stem.substr(stem.size() - 5) == ".MONO";
    }

    [[nodiscard]] boost::filesystem::path addMonoSuffix(const boost::filesystem::path& path)
    {
        return path.parent_path() / (path.stem().string() + ".MONO" + path.extension().string());
    }

    bool MonoProcessor::shouldProcessSav(const boost::filesystem::path& path) const
    {
        return alreadyEndsWithMono(path);
    }

    bool MonoProcessor::shouldProcessLsdsng(const boost::filesystem::path& path) const
    {
        return alreadyEndsWithMono(path);
    }

    boost::filesystem::path MonoProcessor::constructSavDestinationPath(const boost::filesystem::path& path)
    {
        return addMonoSuffix(path);
    }

    boost::filesystem::path MonoProcessor::constructLsdsngDestinationPath(const boost::filesystem::path& path)
    {
        return addMonoSuffix(path);
    }

    bool MonoProcessor::processSong(lsdj_song_t& song)
    {
        if (processInstruments)
        {
            for (int i = 0; i < LSDJ_INSTRUMENT_COUNT; ++i)
                convertInstrument(lsdj_song_get_instrument(&song, i));
        }

        if (processTables)
        {
            for (int i = 0; i < LSDJ_TABLE_COUNT; ++i)
                convertTable(lsdj_song_get_table(&song, i));
        }

        if (processPhrases)
        {
            for (int i = 0; i < LSDJ_PHRASE_COUNT; ++i)
                convertPhrase(lsdj_song_get_phrase(&song, i));
        }
        
        return true;
    }
}
