#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <mutex>
using namespace std;

/** Макросы: простое добавление сообщений в лог. */
#define LOGE(message) Logs::getInstance().write(Logs::Severity::error, message)
#define LOGW(message) Logs::getInstance().write(Logs::Severity::warning, message)
#define LOGI(message) Logs::getInstance().write(Logs::Severity::info, message)
#define LOGD(message) Logs::getInstance().write(Logs::Severity::debug, message)
#define LOGT(message) Logs::getInstance().write(Logs::Severity::trace, message)

/** Источники информации */
// https://habr.com/ru/companies/otus/articles/779914/
// https://logging.apache.org/log4j/2.x/manual/customloglevels.html
// https://www.tutorialspoint.com/log4j/log4j_logging_levels.htm
// https://habr.com/ru/articles/543666/
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum5-dont-use-all_caps-for-enumerators

class Logs {
private:
    /** Перечисление уровней логирования. Пример: (Severity::trace) */
    enum class Severity {trace, debug, info, warning, error};

    /** Перечисление вариантов вывода. Пример: (onlyfile) */
    enum Output {only_file, only_console, file_and_console};

    /** Поле экземпляра */
    static Logs* m_instance;

    /** Поле названия файла логирования */
    string m_name;

    /** Поле режима вывода логов */
    Output m_out;

    /** Поле уровня логирования */
    Severity m_level;

    /** Поле собственного формата для вывода логов */
    string m_format;

    /** Поле для потокобезопасности выполняемых методов */
    static mutex m_mtx; 

    /** Беспараметрический конструктор */
    Logs() {} 
public: 
    Logs(const Logs &log) = delete; // удаление копирующего конструктора 
    Logs& operator=(const Logs &log) = delete; // удаление оператора присваивания копирования
 
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

    /** Изменение место вывода логов */
    void setOutput(Output out) {
        m_out = out;
    }

    /** Изменение уровня логирования */
    void setLevel(Severity level) {
        m_level = level;
    }

    /** Изменение названия файла логирования */
    void setName(string name) {
        m_name = name;
    }

    /** Установление названия файла логирования по умолчанию */
    void setName() {
        m_name = "";
    }

    /** Изменение формата логирования */
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
    */
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
    /** Осуществление логирования в консоли 
     * @param level - уровень логирования для данного метода
     * @param text - произвольный тип данных, который выводится в логирование (комментарий, текст)
    */
    template <typename T>
    void writeConsole(Severity& level, T& text, ...) {
        cout << getResultedString(level, text) << endl;
    }

    /** Осуществление логирования в файл
     * @param level - уровень логирования для данного метода
     * @param text - произвольный тип данных, который выводится в логирование (комментарий, текст)
    */
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

    /** Получение даты и времени (по умолчанию)
     * @return Строка, в формате "yyyy-mm-dd hh:mm:ss"
    */
    string getDatetime() {
        time_t time_now = time(0);
        char timeString[sizeof("yyyy-mm-dd hh:mm:ss")];
        strftime(timeString, sizeof(timeString), "%F %T", localtime(&time_now));
        return timeString;
    }

    /** Получение даты и времени в определённом формате
     * @return Строка, в формате "yyyy-mm-dd*hh:mm:ss", где * - произвольный символ
     * https://en.cppreference.com/w/cpp/chrono/c/strftime - подробная информация по форматам
    */
    string getDatetime(string format) {
        time_t time_now = time(0);
        char timeString[sizeof("yyyy-mm-dd hh:mm:ss")];
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
     * @param level - уровень логирования, который надо преобразовать в строковый формат
     * @param text - произвольный тип данных, который выводится в логирование (комментарий, текст)
     * @return Строка логирования
    */
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