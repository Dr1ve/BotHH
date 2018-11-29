#ifndef APPLICATION_H
#define APPLICATION_H

class Bot;

class Application
{
public:
	Application();
	~Application();
	
	// main application loop
	void Loop();

private:
	Bot* m_bot;
};

#endif // !APPLICATION_H