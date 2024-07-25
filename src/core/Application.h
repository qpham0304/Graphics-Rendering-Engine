#pragma once

class Application
{
private:
	bool isRunning;
public:
	Application();
	~Application() = default;

	typedef int (*RunFunc)();
	void run();
	bool running();
	void close();
};

