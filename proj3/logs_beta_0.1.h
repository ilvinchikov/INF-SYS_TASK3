#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <mutex>
using namespace std;

#define LOGE(message) Logs::getInstance().write(Logs::Severity::error, message)
#define LOGW(message) Logs::getInstance().write(Logs::Severity::warning, message)
#define LOGI(message) Logs::getInstance().write(Logs::Severity::info, message)
#define LOGD(message) Logs::getInstance().write(Logs::Severity::debug, message)
#define LOGT(message) Logs::getInstance().write(Logs::Severity::trace, message)

// https://habr.com/ru/companies/otus/articles/779914/
// https://logging.apache.org/log4j/2.x/manual/customloglevels.html
// https://www.tutorialspoint.com/log4j/log4j_logging_levels.htm
// https://habr.com/ru/articles/543666/
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum5-dont-use-all_caps-for-enumerators

class Logs {
private:
    enum class Severity {trace, debug, info, warning, error}; // уровни логирования (Severity::trace)
    enum Output {only_file, only_console, file_and_console}; // место вывода (onlyfile)
    static Logs* m_instance;
    string m_name;
    Output m_out;
    Severity m_level;
    string m_format;
    static mutex m_mtx; // потокобезопасность
    Logs() {} // беспараметрич. констр
public: 
    Logs(const Logs &log) = delete; // удаление копирующего конструктора 
    Logs& operator=(const Logs &log) = delete; // удаление оператора присваивания копирования

    // получение единственного экземпляра
    static Logs* getInstance() {
        lock_guard<mutex> lock(m_mtx); // блокируем мьютекс
        if (m_instance == nullptr) {
            m_instance = new Logs();
        }
        return m_instance;
    }   

    // место вывода
    void setOutput(Output out) {
        m_out = out;
    }

    // уровень логирования
    void setLevel(Severity level) {
        m_level = level;
    }

    // название файла
    void setName(string name) {
        m_name = name;
    }
    // название файла (по умолчанию)
    void setName() {
        m_name = "";
    }

    // форматирование лога
    void setFormat(string format) {
        m_format = format;
    }
    // форматирование лога (по умолчанию)
    void setFormat() {
        m_format = "";
    }

    // написание лога
    template <typename T, typename... Args>
    void write(Severity level, T text, Args... args) {
        if (level < m_level) return; //  Если выбран уровень INFO, сообщения уровней TRACE и DEBUG игнорируются. 
        lock_guard<mutex> lock(mutex); // запрет на повторное использование, пока не завершится активный поток
        switch (m_out) {
            case only_console:
                writeConsole(level, text, args...);
                break;
            case only_file:
                writeFile(level, text, args...);
                break;
            case file_and_console:
                writeConsole(level, text, args...);
                writeFile(level, text, args...);
                break;
            default:
                writeConsole(level, text, args...);
                break;    
        };
    }

    // написание лога в консоль
    template <typename T>
    void writeConsole(Severity& level, T& text, ...) {
        cout << getResultedString(level, text) << endl;
    }

    // написание лога в файл
    template <typename T>
    void writeFile(Severity& level, T& text, ...) {
        string datetime = getDatetime("%F_%T");
        if (m_name.length() == 0 || m_name == "") {
            while (datetime.find(':') != std::string::npos) {
                datetime.replace(datetime.find(':'), 1, "-");
            }
            m_name = "log_" + datetime + ".log";
        }
        else if (m_name.find(".log") == std::string::npos) {
            m_name += ".log";
        }
        
        ofstream file(m_name);
        if (file.is_open()) {
            file << getResultedString(level, text) << endl;
        }
        else {
            cout << "Ошибка: не удалось создать файл.\n" << endl;
            return;
        }
        file.close();
    }

    // получение даты-времени (по умолчанию) 
    string getDatetime() {
        time_t time_now = time(0);
        char timeString[sizeof("yyyy-mm-dd hh:mm:ss")];
        strftime(timeString, sizeof(timeString), "%F %T", localtime(&time_now));
        return timeString;
    }

    // получение даты-времени // https://en.cppreference.com/w/cpp/chrono/c/strftime
    string getDatetime(string format) {
        time_t time_now = time(0);
        char timeString[sizeof("yyyy-mm-dd hh:mm:ss")];
        strftime(timeString, sizeof(timeString), format.data(), localtime(&time_now));
        return timeString;
    }

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

    template <typename T>
    string getResultedString(Severity& level, T& text, ...) {
        if (m_format.length() == 0 || m_format == "") {
            return (getDatetime() + " | " + getLevel(level) + " -> " + text); // 2023-09-22 12:10:00 | INFO -> User logged out. 
        }
        else {
            string str_form = m_format; //Шаблон {t} | {L} -> {m} дает результат 2023-09-22 12:10:00 | INFO -> User logged out. 
            if (m_format.find("{t}") != string::npos) str_form.replace(m_format.find("{t}"), 3, getDatetime()); // time
            if (m_format.find("{L}") != string::npos) str_form.replace(m_format.find("{L}"), 3, getLevel(level)); // level
            if (m_format.find("{m}") != string::npos)  str_form.replace(m_format.find("{m}"), 3, text); // message
            return str_form;
        }
    }
};