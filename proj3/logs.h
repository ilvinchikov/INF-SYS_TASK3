#ifndef LOGS_H
#define LOGS_H

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <mutex>
#include "UsefulFunctions.h"
using namespace std;

/** Макросы: простое добавление сообщений в лог. */
#define LOGE(message) Logs::getInstance()->write(Logs::Severity::error, message)
#define LOGW(message) Logs::getInstance()->write(Logs::Severity::warning, message)
#define LOGI(message) Logs::getInstance()->write(Logs::Severity::info, message)
#define LOGD(message) Logs::getInstance()->write(Logs::Severity::debug, message)
#define LOGT(message) Logs::getInstance()->write(Logs::Severity::trace, message)

/** Источники информации */
// https://habr.com/ru/companies/otus/articles/779914/
// https://logging.apache.org/log4j/2.x/manual/customloglevels.html
// https://www.tutorialspoint.com/log4j/log4j_logging_levels.htm
// https://habr.com/ru/articles/543666/
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum5-dont-use-all_caps-for-enumerators

class Logs {
private:
    /** Поле для потокобезопасности выполняемых методов */
    static mutex m_mtx; 

public:

    /** Перечисление уровней логирования. Пример: (Severity::trace) */
    enum class Severity {trace, debug, info, warning, error};

    /** Перечисление вариантов вывода. Пример: (onlyfile) */
    enum Output {only_file, only_console, file_and_console};

    /** Поле режима вывода логов */
    Output m_out;

    /** Поле уровня логирования */
    Severity m_level;
    
    /** Поле экземпляра */
    static Logs* m_instance;

    /** Поле собственного формата для вывода логов */
    string m_format;

    /** Беспараметрический конструктор */
    Logs() {} 

    /** Удаление копирующего конструктора */
    Logs(const Logs &log) = delete;
    
    /** Удаление оператора присваивания копирования */
    Logs& operator=(const Logs &log) = delete;
 
    /**  
     * Получение единственного экземпляра объекта класса
     * Является частью для паттерна синглтон
     * @return Указатель на экземпляр объекта класса 
     */
    static Logs* getInstance() {
        lock_guard<mutex> lock(m_mtx); // блокируем мьютекс
        if (m_instance == nullptr) {
            m_instance = new Logs();
        }
        return m_instance;
    }   

    /** Изменение место вывода логов 
     * @param out - новый режим вывода
    */
    void setOutput(Output out) {
        m_out = out;
    }

    /** Изменение уровня логирования 
     * @param level - новый режим уровня логирования
    */
    void setLevel(Severity level) {
        m_level = level;
    }

    /** Изменение формата логирования 
     * @param format - новый режим формата
    */
    void setFormat(string format) {
        m_format = format;
    }

    /** Установление формата логирования по умолчанию */
    void setFormat() {
        m_format = "";
    }

    /** Осуществление логирования 
     * @param level - уровень логирования для данного метода
     * @param text - произвольный тип данных, который выводится в логирование (комментарий, текст)
     * @param filename - название файла для логирования. Используется только при логирования в файл. 
     * Необязательный параметр. По умолчанию: "". 
     * @param sourcefile - файл-источник, в котором призводится логирование. 
     * Пример: (src/main.cpp:45). Необязательный параметр. По умолчанию: "". 
     * @param sourceline - номер строки в файл-источнике, в котором призводится логирование. 
     * Пример: (src/main.cpp:45). Необязательный параметр. По умолчанию: "". 
    */
    template <typename T>
    void write(Severity level, T text, string filename = "", string sourcefile = "", int sourceline = -1) {
        if (level < m_level) return; //  Если выбран уровень INFO, сообщения уровней TRACE и DEBUG игнорируются. 
        lock_guard<mutex> lock(mutex); // запрет на повторное использование, пока не завершится активный поток
        switch (m_out) {
            case only_console:
                writeConsole(level, text, sourcefile, sourceline);
                break;
            case only_file:
                writeFile(level, text, filename, sourcefile, sourceline);
                break;
            case file_and_console:
                writeConsole(level, text, sourcefile, sourceline);
                writeFile(level, text, filename, sourcefile, sourceline);
                break;
            default:
                writeConsole(level, text, sourcefile, sourceline);
                break;    
        };
    }
    /** Осуществление логирования в консоли 
     * @param level - уровень логирования для данного метода
     * @param text - произвольный тип данных, который выводится в логирование (комментарий, текст)
     * @param sourcefile - файл-источник, в котором призводится логирование. 
     * @param sourceline - номер строки в файл-источнике, в котором призводится логирование.
    */
    template <typename T>
    void writeConsole(Severity& level, T& text, string& sourcefile, int& sourceline) {
        cout << getResultedString(level, text, sourcefile, sourceline) << endl;
    }

    /** Осуществление логирования в файл
     * @param level - уровень логирования для данного метода
     * @param text - произвольный тип данных, который выводится в логирование (комментарий, текст)
     * @param filename - название файла для логирования.
     * @param sourcefile - файл-источник, в котором призводится логирование. 
     * @param sourceline - номер строки в файл-источнике, в котором призводится логирование.
    */
    template <typename T>
    void writeFile(Severity& level, T& text, string& filename, string& sourcefile, int& sourceline) {
        string datetime = getDatetime("%Y-%m-%d_%X");
        if (filename.length() == 0 || filename == "") {
            while (datetime.find(':') != std::string::npos) {
                datetime.replace(datetime.find(':'), 1, "-");
            }
            filename = "log_" + datetime + ".log";
        }
        else if (filename.find(".log") == std::string::npos) {
            filename += ".log";
        }
        
        ofstream file(filename);
        if (file.is_open()) {
            file << getResultedString(level, text, sourcefile, sourceline) << endl;
        }
        else {
            cout << "Ошибка: не удалось создать файл.\n" << endl;
            return;
        }
        file.close();
    }

    /** Получение даты и времени (по умолчанию)
     * @return Строка, в формате "yyyy-mm-dd hh:mm:ss"
    */
    string getDatetime() {
        time_t time_now = time(0);
        char timeString[80];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %X", localtime(&time_now));
        return timeString;
    }

    /** Получение даты и времени в определённом формате
     * @return Строка, в формате "yyyy-mm-dd*hh:mm:ss", где * - произвольный символ
     * https://en.cppreference.com/w/cpp/chrono/c/strftime - подробная информация по форматам
    */
    string getDatetime(string format) {
        time_t time_now = time(0);
        char timeString[80];
        strftime(timeString, sizeof(timeString), format.data(), localtime(&time_now));
        return timeString;
    }

    
    /** Получение уровня логирования в формате строки
     * @param level - уровень логирования, который надо преобразовать в строковый формат
     * @return Уровень логирования в строковом формате
    */
    string getLevel(Severity& level) {
        switch (level)
        {
        case Severity::trace:
            return "TRACE";
            break;
        case Severity::debug:
            return "DEBUG";
            break;
        case Severity::info:
            return "INFO";
            break;
        case Severity::warning:
            return "WARNING";
            break;
        case Severity::error:
            return "ERROR";
            break;
        default:
            return "???";
            break;
        }
    }

    /** Получение итоговой строки логирования, исходя из заданного формата
     * Шаблон {t} | {L} -> {m} дает результат 2023-09-22 12:10:00 | INFO -> User logged out. 
     * Шаблон {t} | {L} | {S}:{l} -> {m} дает результат 2023-09-22 12:10:00 | INFO | src/main.cpp:45 -> User logged out. 
     * @param level - уровень логирования, который надо преобразовать в строковый формат
     * @param text - произвольный тип данных, который выводится в логирование (комментарий, текст)
     * @param sourcefile - файл-источник, в котором призводится логирование. 
     * @param sourceline - номер строки в файл-источнике, в котором призводится логирование.
     * @return Строка логирования
    */
    template <typename T>
    string getResultedString(Severity& level, T& text, string& sourcefile, int& sourceline) {
        UsefulFunctions us;
        if (m_format.length() == 0 || m_format == "") {
            return (getDatetime() + " | " + getLevel(level) + ((sourcefile.length() == 0 || sourcefile == "") ? "" : " | " + sourcefile)  + ((sourceline > 0) ? " | line:" + us.toString(sourceline) : "") + " -> " + text);
        }
        else {
            string str_form = m_format;  
            if (m_format.find("{t}") != string::npos) str_form.replace(m_format.find("{t}"), 3, getDatetime()); // time
            if (m_format.find("{L}") != string::npos) str_form.replace(m_format.find("{L}"), 3, getLevel(level)); // level
            if (m_format.find("{m}") != string::npos)  str_form.replace(m_format.find("{m}"), 3, text); // message
            if (m_format.find("{S}") != string::npos)  str_form.replace(m_format.find("{S}"), 3, "src/" + sourcefile); // src file
            if (m_format.find("{l}") != string::npos)  str_form.replace(m_format.find("{l}"), 3, us.toString(sourceline)); // line in src file
            return str_form;
        }
    }
};

// инициализация статических переменных
Logs* Logs::m_instance = nullptr;
mutex Logs::m_mtx;

#endif //STORE_H