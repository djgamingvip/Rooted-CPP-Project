#pragma once

class Menu {
public:
    static void Initialize();
    static void Render();
    static bool IsEnabled() { return enabled; }
    
private:
    static bool enabled;
};