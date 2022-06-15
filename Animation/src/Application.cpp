#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "AdditiveBlendingApplication.h"

#include <iostream>

int main()
{
    AdditiveBlendingApplication app;

    app.startup();

    app.run();

    return 0;
}