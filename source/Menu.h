#pragma once

#include <string>
#include <functional>
#include <utility>
#include <list>
#include "Input.h"
#include "WiiUScreen.h"
#include "ScreenUtils.h"
#include "VPADInput.h"

template <typename T>
class Menu {
public:
    Menu() {
        clear();
    }

    void setOptionsCallback(std::function<void(T)> cb) {
        callback = cb;
    }

    void setHeader(const std::string &hdr) {
        header = hdr;
    }

    void setFooter(const std::string &ftr) {
        footer = ftr;
    }

    void clear() {
        content.clear();
        options.clear();
        selected = 0;
    }

    void addText(const std::string &text = "") {
        content.emplace_back(false, text);
    }

    void addOption(const std::string &text, T data) {
        content.emplace_back(true, text);
        options.push_back(data);
    }

    void render() {
        int selectedDist = selected;
        WiiUScreen::clearScreen();
        printHeader();
        for (auto &row : content) {
            std::string cursor = "";
            if (row.first) {
                cursor += (selectedDist--) ? "  " : "> ";
            }
            drawString(cursor + row.second);
        }
        printFooter();
        WiiUScreen::flipBuffers();
    }

    void update(const Input *input) {
        if (input->data.buttons_d & Input::BUTTON_UP) {
            selected = (selected ? selected : options.size()) - 1;
        } else if (input->data.buttons_d & Input::BUTTON_DOWN) {
            selected = (selected < (options.size() - 1)) ? (selected + 1) : 0;
        }

        if (entrySelected(input)) {
            auto selectedOption = std::next(options.begin(), selected);
            callback(*selectedOption);
        }
    }

private:
    void drawString(const std::string &text = "") const {
        WiiUScreen::drawLine(text.c_str());
    }

    void printHeader() const {
        drawString(header);
        drawString(std::string(header.size() + 3, '='));
        drawString();
    }

    void printFooter() const {
        ScreenUtils::printTextOnScreen(CONSOLE_SCREEN_TV, 0, 27, footer.c_str());
        ScreenUtils::printTextOnScreen(CONSOLE_SCREEN_DRC, 0, 17, footer.c_str());
    }

    bool entrySelected(const Input *input) const {
        return input->data.buttons_d & Input::BUTTON_A;
    }

    std::function<void(T)> callback;
    std::string header, footer;
    std::list<std::pair<bool, std::string>> content;
    std::list<T> options;
    size_t selected = 0;
};
