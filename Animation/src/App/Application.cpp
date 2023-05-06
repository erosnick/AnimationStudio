#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DemoApplication.h"

#include <iostream>

int main()
{
    DemoApplication app;

    app.startup();

    app.run();

    return 0;
}