#include <unordered_map>
#include <stack>

#include "markdown.h"

namespace projectfarm::shared::markdown
{
    auto ReadColor(std::uint32_t& index,
                   std::string_view text,
                   const shared::graphics::colors::Color& defaultColor) noexcept
        -> std::pair<shared::graphics::colors::Color, bool>
    {
        auto colorFound {false};

        std::string colorBuffer;

        auto colorIndex = index + 1;

        while (colorIndex < text.size())
        {
            auto colorC = text[colorIndex];

            if (colorC == ')')
            {
                colorFound = true;
                break;
            }

            colorBuffer += colorC;
            ++colorIndex;
        }

        shared::graphics::colors::Color color {defaultColor};

        // if a color was found, don't add that text to
        // the part
        if (colorFound)
        {
            index = colorIndex;

            if (auto foundColor = graphics::colors::FromString(colorBuffer); foundColor)
            {
                color = *foundColor;
            }
            else
            {
                color = defaultColor;
            }
        }

        return { color, colorFound };
    }

    std::vector<TextPart> GetTextParts(std::string_view text,
                                       shared::graphics::colors::Color defaultColor) noexcept
    {
        static const std::unordered_map<char, TextPartTypes> controlCharacters
        {
            { '*', TextPartTypes::Italic },
            { '_', TextPartTypes::Bold },
            { '$', TextPartTypes::BoldItalic },
        };

        if (text.empty())
        {
            return {};
        }

        std::vector<TextPart> parts;

        TextPartTypes typeState {TextPartTypes::Normal};

        shared::graphics::colors::Color colorState { defaultColor };

        auto isControlCharacter = [](char c) -> bool
        {
            for (const auto& controlCharacter : controlCharacters)
            {
                if (controlCharacter.first == c)
                {
                    return true;
                }
            }

            return false;
        };

        auto savePart = [&parts, &typeState, &colorState](uint32_t index,
                                                          uint32_t partEndPos,
                                                          std::string_view readText)
        {
          if (readText.empty())
          {
              return;
          }

          TextPart part;
          part._text = readText;
          part._originalTextStartIndex = index;
          part._endTextPosition = partEndPos;
          part._type = typeState;
          part._color = colorState;

          parts.push_back(part);
        };

        auto readColor = [&colorState, &defaultColor, &savePart, &typeState](uint32_t& i,
                                                                             uint32_t& partStartPosition,
                                                                             std::string_view textToProcess,
                                                                             std::string readText) -> bool
        {
            auto originalIndex = i;
            auto colorFinished {false};

            auto c = textToProcess[i];
            if (c != '(')
            {
                return false;
            }

            std::string colorText;
            auto originalStartPosition {i};

            ++i;
            for (; i < textToProcess.size(); ++i)
            {
                c = textToProcess[i];
                if (c == ')')
                {
                    colorFinished = true;
                    break;
                }

                colorText += c;
            }

            // the color markup wasn't completed
            if (i >= textToProcess.size())
            {
                i = originalIndex;
                return false;
            }

            if (!readText.empty())
            {
                for (const auto& controlCharacter : controlCharacters)
                {
                    if (typeState != TextPartTypes::Normal &&
                        typeState == controlCharacter.second)
                    {
                        // as we are finishing an existing part, remove
                        // the control character from the beginning rather
                        // than add a control character to the end, as we
                        // shouldn't be saving parts with control characters
                        // both at the beginning and the end
                        readText = readText.substr(1);
                    }
                }
            }

            savePart(partStartPosition, colorFinished ? i + 1 : i, readText);

            originalStartPosition = i;
            partStartPosition = colorFinished ? originalStartPosition + 1 : originalStartPosition;

            if (auto color = shared::graphics::colors::FromString(colorText); color)
            {
                colorState = *color;
            }
            else
            {
                colorState = defaultColor;
            }

            return true;
        };

        auto readBackslash = [&isControlCharacter](uint32_t i, std::string_view textToProcess) -> bool
        {
            if (i == textToProcess.size() - 1)
            {
                return false;
            }

            auto c = textToProcess[i];

            if (c != '\\')
            {
                return false;
            }

            auto c2 = textToProcess[i + 1];

            return isControlCharacter(c2) || c2 == '(';
        };

        auto readUntilControlCharacter = [&isControlCharacter, &readColor, &readBackslash](uint32_t& i,
                                                                                           uint32_t& partStartPosition,
                                                                                           std::string_view textToProcess)
                -> std::string
        {
            std::string line;

            partStartPosition = i;

            for (; i < textToProcess.size(); ++i)
            {
                auto c = textToProcess[i];

                auto isBackslash = readBackslash(i, textToProcess);

                if (isBackslash)
                {
                    ++i;
                    c = textToProcess[i];
                }
                else
                {
                    if (readColor(i, partStartPosition, textToProcess, line))
                    {
                        line = "";
                        continue;
                    }

                    if (isControlCharacter(c))
                    {
                        break;
                    }
                }

                line += c;
            }

            return line;
        };

        auto readAllOfNextControlCharacter = [&typeState, &readColor, &readBackslash](uint32_t& i,
                                                                                      uint32_t& partStartPosition,
                                                                                      uint32_t& partEndPosition,
                                                                                      std::string_view textToProcess)
                -> std::string
        {
            auto controlCharacter = textToProcess[i++];

            auto controlCharacterIter = controlCharacters.find(controlCharacter);
            if (controlCharacterIter == controlCharacters.end())
            {
                // TODO: Handle this error...
                return "error";
            }

            typeState = controlCharacterIter->second;

            std::string line;
            line += controlCharacter;

            // in case this control section is not complete, we can revert
            auto previousStartPos = partStartPosition;

            // set the here in case we need to read a color, then the correct
            // starting pos will be saved
            partStartPosition = i;

            for (; i < textToProcess.size(); ++i)
            {
                auto c = textToProcess[i];

                auto isBackslash = readBackslash(i, textToProcess);

                if (isBackslash)
                {
                    ++i;
                    c = textToProcess[i];
                }

                if (!isBackslash && readColor(i, partStartPosition, textToProcess, line))
                {
                    // if we read a color, a new part would have been added
                    // start off this new part with the current control character
                    line = controlCharacter;
                    continue;
                }

                line += c;

                if (!isBackslash && c == controlCharacter)
                {
                    break;
                }
            }

            if (line.size() > 1 &&
                line[0] == controlCharacter &&
                line[line.size() - 1] == controlCharacter)
            {
                line = line.substr(1, line.size() - 2);

                partStartPosition = i - static_cast<uint32_t>(line.size());
                partEndPosition = i + 1;
            }
            else
            {
                partStartPosition = previousStartPos;
                partEndPosition = i;
            }

            return line;
        };

        auto partStartPosition {0u};

        for (auto i = 0u; i < text.size(); ++i)
        {
            typeState = TextPartTypes::Normal;

            auto readText = readUntilControlCharacter(i, partStartPosition, text);

            savePart(partStartPosition, i, readText);

            if (i >= text.size())
            {
                break;
            }

            auto partEndPosition {0u};
            readText = readAllOfNextControlCharacter(i, partStartPosition, partEndPosition, text);

            savePart(partStartPosition, partEndPosition, readText);
        }

        return parts;
    }

    uint32_t GetMarkdownPositionFromPartPosition(std::string_view text, uint32_t partPosition) noexcept
    {
        if (text.empty())
        {
            return 0;
        }

        auto parts = GetTextParts(text);
        if (parts.empty())
        {
            // perhaps we have just color markup as our text
            return static_cast<uint32_t>(text.size());
        }

        auto markdownPosition {0u};

        for (auto i = 0u; i < parts.size(); ++i)
        {
            const auto& part = parts[i];

            if (part._text.size() < partPosition)
            {
                partPosition -= static_cast<uint32_t>(part._text.size());
            }
            else
            {
                if (partPosition == part._text.size())
                {
                    // for text such as `$text$(blue)hi`
                    if (i < parts.size() - 1 &&
                        parts[i + 1]._originalTextStartIndex - part._endTextPosition > 1)
                    {
                        markdownPosition = parts[i + 1]._originalTextStartIndex;
                    }
                    // for text such as `$text$(blue)`
                    // if the end of the text is color markup, there won't
                    // be a part for it
                    else if (i == parts.size() - 1 &&
                            part._endTextPosition < text.size())
                    {
                        markdownPosition = static_cast<uint32_t>(text.size());
                    }
                    else
                    {
                        markdownPosition = part._endTextPosition;
                    }
                }
                else
                {
                    markdownPosition = part._originalTextStartIndex + partPosition;
                }

                break;
            }
        }

        return markdownPosition;
    }
}