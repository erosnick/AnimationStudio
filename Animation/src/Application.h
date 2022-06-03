#pragma once

class Application {
public:
	virtual void startup() {}
	virtual void update(float deltaTime) {}
	virtual void run() {}
	virtual void render() {}
	virtual void shutdown() {}
};