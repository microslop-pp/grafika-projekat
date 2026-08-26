#ifndef MYAPP_HPP
#define MYAPP_HPP
#include <engine/core/App.hpp>

namespace app {

class MyApp : public engine::core::App {
    void app_setup() override;
};

} // app

#endif //MYAPP_HPP

//find app/ -type f \( -name "*.hpp" -o -name "*.cpp" -o -name "CMakeLists.txt" -o -name "*.json" \) -exec sh -c 'echo "=== {} ===" && cat "{}" && echo "\n"' \; > project_context.txt