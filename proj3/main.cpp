#include <iostream>
#include <windows.h>
#include "logs.h" // логи

int main(int argc, char** argv) {

	LOGI("Task started.");
	Logs log;
	log.write(Logs::Severity::error, "xd", "test.log"); // file_and_console
	Logs::getInstance()->write(Logs::Severity::error, "Hi"); // console
	Logs::getInstance()->setOutput(Logs::file_and_console); // изменение режима
	Logs::getInstance()->write(Logs::Severity::error, "Privet", "VIVOD_LOGA");  // file_and_console
	Logs::getInstance()->write(Logs::Severity::error, "Nihao", "USHEL_LOGA", "main.cpp", 12);  // file_and_console

	system("pause");
	return 0;
}
