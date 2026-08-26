#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    void draw_corridor();

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_HPP