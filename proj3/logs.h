#ifndef LOGS_H
#define LOGS_H

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <mutex>
#include "UsefulFunctions.h"
using namespace std;

/** �������: ������� ���������� ��������� � ���. */
#define LOGE(message) Logs::getInstance()->write(Logs::Severity::error, message)
#define LOGW(message) Logs::getInstance()->write(Logs::Severity::warning, message)
#define LOGI(message) Logs::getInstance()->write(Logs::Severity::info, message)
#define LOGD(message) Logs::getInstance()->write(Logs::Severity::debug, message)
#define LOGT(message) Logs::getInstance()->write(Logs::Severity::trace, message)

/** ��������� ���������� */
// https://habr.com/ru/companies/otus/articles/779914/
// https://logging.apache.org/log4j/2.x/manual/customloglevels.html
// https://www.tutorialspoint.com/log4j/log4j_logging_levels.htm
// https://habr.com/ru/articles/543666/
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum5-dont-use-all_caps-for-enumerators

class Logs {
private:
    /** ���� ��� ������������������ ����������� ������� */
    static mutex m_mtx; 

public:

    /** ������������ ������� �����������. ������: (Severity::trace) */
    enum class Severity {trace, debug, info, warning, error};

    /** ������������ ��������� ������. ������: (onlyfile) */
    enum Output {only_file, only_console, file_and_console};

    /** ���� ������ ������ ����� */
    Output m_out;

    /** ���� ������ ����������� */
    Severity m_level;
    
    /** ���� ���������� */
    static Logs* m_instance;

    /** ���� ������������ ������� ��� ������ ����� */
    string m_format;

    /** ������������������ ����������� */
    Logs() {} 

    /** �������� ����������� ������������ */
    Logs(const Logs &log) = delete;
    
    /** �������� ��������� ������������ ����������� */
    Logs& operator=(const Logs &log) = delete;
 
    /**  
     * ��������� ������������� ���������� ������� ������
     * �������� ������ ��� �������� ��������
     * @return ��������� �� ��������� ������� ������ 
     */
    static Logs* getInstance() {
        lock_guard<mutex> lock(m_mtx); // ��������� �������
        if (m_instance == nullptr) {
            m_instance = new Logs();
        }
        return m_instance;
    }   

    /** ��������� ����� ������ ����� 
     * @param out - ����� ����� ������
    */
    void setOutput(Output out) {
        m_out = out;
    }

    /** ��������� ������ ����������� 
     * @param level - ����� ����� ������ �����������
    */
    void setLevel(Severity level) {
        m_level = level;
    }

    /** ��������� ������� ����������� 
     * @param format - ����� ����� �������
    */
    void setFormat(string format) {
        m_format = format;
    }

    /** ������������ ������� ����������� �� ��������� */
    void setFormat() {
        m_format = "";
    }

    /** ������������� ����������� 
     * @param level - ������� ����������� ��� ������� ������
     * @param text - ������������ ��� ������, ������� ��������� � ����������� (�����������, �����)
     * @param filename - �������� ����� ��� �����������. ������������ ������ ��� ����������� � ����. 
     * �������������� ��������. �� ���������: "". 
     * @param sourcefile - ����-��������, � ������� ����������� �����������. 
     * ������: (src/main.cpp:45). �������������� ��������. �� ���������: "". 
     * @param sourceline - ����� ������ � ����-���������, � ������� ����������� �����������. 
     * ������: (src/main.cpp:45). �������������� ��������. �� ���������: "". 
    */
    template <typename T>
    void write(Severity level, T text, string filename = "", string sourcefile = "", int sourceline = -1) {
        if (level < m_level) return; //  ���� ������ ������� INFO, ��������� ������� TRACE � DEBUG ������������. 
        lock_guard<mutex> lock(mutex); // ������ �� ��������� �������������, ���� �� ���������� �������� �����
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
    /** ������������� ����������� � ������� 
     * @param level - ������� ����������� ��� ������� ������
     * @param text - ������������ ��� ������, ������� ��������� � ����������� (�����������, �����)
     * @param sourcefile - ����-��������, � ������� ����������� �����������. 
     * @param sourceline - ����� ������ � ����-���������, � ������� ����������� �����������.
    */
    template <typename T>
    void writeConsole(Severity& level, T& text, string& sourcefile, int& sourceline) {
        cout << getResultedString(level, text, sourcefile, sourceline) << endl;
    }

    /** ������������� ����������� � ����
     * @param level - ������� ����������� ��� ������� ������
     * @param text - ������������ ��� ������, ������� ��������� � ����������� (�����������, �����)
     * @param filename - �������� ����� ��� �����������.
     * @param sourcefile - ����-��������, � ������� ����������� �����������. 
     * @param sourceline - ����� ������ � ����-���������, � ������� ����������� �����������.
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
            cout << "������: �� ������� ������� ����.\n" << endl;
            return;
        }
        file.close();
    }

    /** ��������� ���� � ������� (�� ���������)
     * @return ������, � ������� "yyyy-mm-dd hh:mm:ss"
    */
    string getDatetime() {
        time_t time_now = time(0);
        char timeString[80];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %X", localtime(&time_now));
        return timeString;
    }

    /** ��������� ���� � ������� � ����������� �������
     * @return ������, � ������� "yyyy-mm-dd*hh:mm:ss", ��� * - ������������ ������
     * https://en.cppreference.com/w/cpp/chrono/c/strftime - ��������� ���������� �� ��������
    */
    string getDatetime(string format) {
        time_t time_now = time(0);
        char timeString[80];
        strftime(timeString, sizeof(timeString), format.data(), localtime(&time_now));
        return timeString;
    }

    
    /** ��������� ������ ����������� � ������� ������
     * @param level - ������� �����������, ������� ���� ������������� � ��������� ������
     * @return ������� ����������� � ��������� �������
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

    /** ��������� �������� ������ �����������, ������ �� ��������� �������
     * ������ {t} | {L} -> {m} ���� ��������� 2023-09-22 12:10:00 | INFO -> User logged out. 
     * ������ {t} | {L} | {S}:{l} -> {m} ���� ��������� 2023-09-22 12:10:00 | INFO | src/main.cpp:45 -> User logged out. 
     * @param level - ������� �����������, ������� ���� ������������� � ��������� ������
     * @param text - ������������ ��� ������, ������� ��������� � ����������� (�����������, �����)
     * @param sourcefile - ����-��������, � ������� ����������� �����������. 
     * @param sourceline - ����� ������ � ����-���������, � ������� ����������� �����������.
     * @return ������ �����������
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

// ������������� ����������� ����������
Logs* Logs::m_instance = nullptr;
mutex Logs::m_mtx;

#endif //STORE_H