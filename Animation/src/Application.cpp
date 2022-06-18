#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DualQuaternionApplication.h"

#include <iostream>

int main()
{
    DualQuaternionApplication app;

    app.startup();

    app.run();

    return 0;
}