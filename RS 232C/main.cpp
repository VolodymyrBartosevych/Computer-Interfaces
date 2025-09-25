#include <iostream>
#include <windows.h>
#include <fstream>

using namespace std;

typedef unsigned long ulong;

void *open_serial_port(int num_port, int baud_rate, ulong parity, int stop_bits,int byte_size);
bool close_serial_port(void* comm_handle);
bool write_serial_port(void* comm_handle, ulong num_bytes,const void* buffer);
bool read_serial_port(void* comm_handle, ulong *num_of_bytes_read, ulong buffer_size , void* buffer);
void close_serial_port_option(void*& h_comm_dev);


void config_serial_port(void*& h_comm_dev);
void send_string(void* h_comm_dev);
void send_txt_file(void* h_comm_dev);
void receive_file(void* h_comm_dev);
void send_binary_file(void* h_comm_dev);

int main() {
    void *h_comm_dev = nullptr;
    int choice;

    while (true) {
        cout << "1. Configure serial port\n";
        cout << "2. Send string\n";
        cout << "3. Send file\n";
        cout << "4. Send binary file\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        string input;
        cin >> input;

        try {
            choice = stoi(input);
        } catch (...) {
            cout << "Invalid input, try again.\n";
            continue;
        }

        switch (choice) {
            case 1:
                while (true) {
                    cout << "1. Set and open serial port\n";
                    cout << "2. Close serial port\n";
                    cout << "3. Go back to main menu\n";
                    cout << "Enter choice: ";
                    cin >> input;

                    try {
                        int config_choice = stoi(input);
                        switch (config_choice) {
                            case 1:
                                config_serial_port(h_comm_dev);
                                break;
                            case 2:
                                close_serial_port_option(h_comm_dev);
                                break;
                            case 3:
                                break;
                            default:
                                cout << "Invalid choice. Try again.\n";
                        }
                    } catch (...) {
                        cout << "Invalid input, try again.\n";
                    }

                    if (input == "3") {
                        break;
                    }
                }
                break;
            case 2:
                send_string(h_comm_dev);
                break;
            case 3:
                send_txt_file(h_comm_dev);
                receive_file(h_comm_dev);
                break;
            case 4:
                send_binary_file(h_comm_dev);
                receive_file(h_comm_dev);
                break;
            case 5:
                cout << "Exiting program...\n";
                if (h_comm_dev != nullptr) {
                    close_serial_port(h_comm_dev);
                }
                return 0;
            default:
                cout << "Invalid choice, try again.\n";
        }
    }

    return 0;
}

void *open_serial_port(int num_port, int baud_rate, ulong parity, int stop_bits,int byte_size) {
    char port_name[16];
    DCB dcb;
    ulong fdw_evt_mask;

    switch (num_port) {
        case 1:
            strcpy(port_name, "COM1");
            break;
        case 2:
            strcpy(port_name, "COM2");
            break;
        case 3:
            strcpy(port_name, "COM3");
            break;
        case 4:
            strcpy(port_name, "COM4");
            break;
        case 5:
            strcpy(port_name, "COM5");
            break;
        case 6:
            strcpy(port_name, "COM6");
            break;
        case 7:
            strcpy(port_name, "COM7");
            break;
        case 8:
            strcpy(port_name, "COM8");
            break;
        case 9:
            strcpy(port_name, "COM9");
            break;
        case 10:
            strcpy(port_name, "COM10");
            break;
        default:
            return nullptr;
    }

    switch (baud_rate) {
        case 110:
            dcb.BaudRate = CBR_110;
            break;
        case 300:
            dcb.BaudRate = CBR_300;
            break;
        case 600:
            dcb.BaudRate = CBR_600;
            break;
        case 1200:
            dcb.BaudRate = CBR_1200;
            break;
        case 2400:
            dcb.BaudRate = CBR_2400;
            break;
        case 4800:
            dcb.BaudRate = CBR_4800;
            break;
        case 9600:
            dcb.BaudRate = CBR_9600;
            break;
        case 14400:
            dcb.BaudRate = CBR_14400;
            break;
        case 19200:
            dcb.BaudRate = CBR_19200;
            break;
        case 38400:
            dcb.BaudRate = CBR_38400;
            break;
        case 56000:
            dcb.BaudRate = CBR_56000;
            break;
        case 57600:
            dcb.BaudRate = CBR_57600;
            break;
        case 115200:
            dcb.BaudRate = CBR_115200;
            break;
        case 128000:
            dcb.BaudRate = CBR_128000;
            break;
        case 256000:
            dcb.BaudRate = CBR_256000;
            break;
        default:
            return nullptr;
    }

    void *h_comm_dev = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

    if (h_comm_dev != INVALID_HANDLE_VALUE) {
        SetupComm(h_comm_dev, 64, 64);
        dcb.DCBlength = sizeof(dcb);
        GetCommState(h_comm_dev, &dcb);

        switch (parity) {
            case NOPARITY:
                dcb.Parity = NOPARITY;
                break;
            case ODDPARITY:
                dcb.Parity = ODDPARITY;
                break;
            default:
                return nullptr;
        }

        switch (stop_bits) {
            case ONESTOPBIT:
                dcb.StopBits = ONESTOPBIT;
                break;
            case TWOSTOPBITS:
                dcb.StopBits = TWOSTOPBITS;
                break;
            default:
                return nullptr;
        }

        switch (byte_size) {
            case 5:
                dcb.ByteSize = 5;
                break;
            case 6:
                dcb.ByteSize = 6;
                break;
            case 7:
                dcb.ByteSize = 7;
                break;
            case 8:
                dcb.ByteSize = 8;
                break;
            default:
                return nullptr;
        }

        dcb.fParity = TRUE;
        dcb.fDtrControl = DTR_CONTROL_DISABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;

        SetCommState(h_comm_dev, &dcb);
        GetCommState(h_comm_dev, reinterpret_cast<LPDCB>(&fdw_evt_mask));
        SetCommMask(h_comm_dev, EV_TXEMPTY);
        return h_comm_dev;
    }
}

bool close_serial_port(void* comm_handle) {
    if ((comm_handle == nullptr) || (comm_handle == INVALID_HANDLE_VALUE)) {
        return false;
    }
    CloseHandle(comm_handle);
    return true;
}

bool write_serial_port(void* comm_handle, ulong num_bytes,const void* buffer) {
    if ((comm_handle == nullptr) || (comm_handle == INVALID_HANDLE_VALUE)) {
        return false;
    }
    ulong ip_evt_mask;
    ulong num_of_bytes_written;

    WriteFile(comm_handle, buffer, num_bytes, &num_of_bytes_written, nullptr);
    WaitCommEvent(comm_handle, &ip_evt_mask, nullptr);
    return true;
}

bool read_serial_port(void *comm_handle, ulong *num_of_bytes_read, ulong buffer_size, void *buffer) {
    COMSTAT comstat;
    ulong errors;
    ulong num_of_bytes_to_read;

    ClearCommError(comm_handle, &errors, &comstat);

    cout << "Stat.cbInQue: " << comstat.cbInQue << "\n";
    if (comstat.cbInQue > 0) {
        if (comstat.cbInQue > buffer_size) {
            num_of_bytes_to_read = buffer_size;
        } else {
            num_of_bytes_to_read = comstat.cbInQue;
        }


        if (ReadFile(comm_handle, buffer, num_of_bytes_to_read, num_of_bytes_read, nullptr)) {
            cout << "ReadFile: " << num_of_bytes_read << "\n";
        } else {
            cout << "ReadFile failed\n";
        }
    } else {
        *num_of_bytes_read = 0;
    }
    return true;
}

void config_serial_port(void*& h_comm_dev) {
    int num_port = 1;
    int baud_rate = 9600;
    ulong parity = NOPARITY;
    int stop_bits = ONESTOPBIT;
    int byte_size = 8;

    string input;

    cout << "Enter port number: (e.g., 1 for COM1. Default=COM1): ";
    cin.ignore();
    getline(cin, input);
    if (!input.empty()) {
        num_port = stoi(input);
    }

    cout << "Enter baud rate: (e.g., 9600. Default=9600): ";
    getline(cin, input);
    if (!input.empty()) {
        baud_rate = stoi(input);
    }

    cout << "Enter parity: (0=No parity, 1=Odd parity. Default=No parity): ";
    getline(cin, input);
    if (!input.empty()) {
        parity = stoul(input);
    }

    cout << "Enter stop bits: (1=One stop bit, 2=Two stop bits. Default=One stop bit): ";
    getline(cin, input);
    if (!input.empty()) {
        stop_bits = (stoi(input) == 2) ? TWOSTOPBITS : ONESTOPBIT;
    }

    cout << "Enter byte size: (5, 6, 7, 8. Default=8): ";
    getline(cin, input);
    if (!input.empty()) {
        byte_size = stoi(input);
    }

    if (h_comm_dev != nullptr) {
        close_serial_port(h_comm_dev);
    }

    h_comm_dev = open_serial_port(num_port, baud_rate, parity, stop_bits, byte_size);

    if (h_comm_dev == nullptr || h_comm_dev == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        cout << "Error opening serial port. Error code: " << error << "\n";
        h_comm_dev = nullptr;
    } else {
        cout << "Serial port configured and opened\n" << "\n";
    }
}



void send_string(void* h_comm_dev) {
    char buffer_in_out[64];
    ulong bytes_read;


    cin.ignore();
    cout << "Enter string to send: ";
    cin.getline(buffer_in_out, sizeof(buffer_in_out));

    if (!write_serial_port(h_comm_dev, strlen(buffer_in_out), buffer_in_out)) {
        cout << "Error writing to serial port\n";
        return;
    }

    cout << "Transmitting data...\n";
    Sleep(250);

    read_serial_port(h_comm_dev, &bytes_read, sizeof(buffer_in_out), buffer_in_out);

    cout << "Read: " << bytes_read << " bytes\n";

    if (bytes_read > 0) {
        cout << "Data read: " << buffer_in_out << "\n";
    }

    cout << "\n";
}

void close_serial_port_option(void*& h_comm_dev) {
    if (h_comm_dev != nullptr) {
        if (close_serial_port(h_comm_dev)) {
            cout << "Serial port closed successfully.\n";
            h_comm_dev = nullptr;
        } else {
            cout << "Failed to close serial port.\n";
        }
    } else {
        cout << "No serial port is currently open.\n";
    }
}

void send_txt_file(void *h_comm_dev) {
    string file_name;
    cout << "Enter file name: ";
    cin.ignore();
    getline(cin, file_name);

    ifstream file(file_name);

    if (!file.is_open()) {
        cout << "Error opening file\n";
        return;
    }

    string file_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ulong file_size = file_content.size();

    if (!write_serial_port(h_comm_dev, sizeof(file_size), &file_size)) {
        cout << "Error sending file size\n";
        return;
    }

    if (!write_serial_port(h_comm_dev, file_size, file_content.c_str())) {
        cout << "Error sending file content\n";
        return;
    }
    cout << "File sent successfully\n";
}

void receive_file(void *h_comm_dev) {
    ulong file_size = 0;
    ulong bytes_read = 0;

    if (!read_serial_port(h_comm_dev, &bytes_read, sizeof(file_size), &file_size)) {
        cout << "Error reading file size\n";
        return;
    }

    if (bytes_read != sizeof(file_size)) {
        cout << "Error reading file size\n";
        return;
    }

    cout << "File size: " << file_size << " bytes\n";

    char* buffer = new char[file_size];

    ulong total_bytes_read = 0;

    while (total_bytes_read < file_size) {
        ulong chunk_size = min(file_size - total_bytes_read, static_cast<ulong>(1024));

        if (!read_serial_port(h_comm_dev, &bytes_read, chunk_size, buffer + total_bytes_read)) {
            cout << "Error reading file\n";
            delete[] buffer;
            return;
        }

        total_bytes_read += bytes_read;
        cout << "Received: " << bytes_read << " bytes\n";
        if (bytes_read == 0) {
            cout << "No more data to read\n";
            break;
        }
    }

    if (total_bytes_read != file_size) {
        cout << "Error: Incomplete file received\n";
        delete[] buffer;
        return;
    }


    string out_file_name;
    cout << "Enter output file name: ";
    cin.ignore();
    getline(cin, out_file_name);

    ofstream out_file(out_file_name, ios::binary);
    if (!out_file) {
        cout << "Error opening output file\n";
        delete[] buffer;
        return;
    }

    out_file.write(buffer, file_size);
    out_file.close();

    cout << "File received and saved successfully\n";
    delete[] buffer;
}

void send_binary_file(void *h_comm_dev) {
    string file_name;
    cout << "Enter file name: ";
    cin.ignore();
    getline(cin, file_name);

    ifstream file(file_name, ios::binary | ios::ate);

    if (!file.is_open()) {
        cout << "Error opening file\n";
        return;
    }

    ulong file_size = static_cast<ulong>(file.tellg());
    file.seekg(0, ios::beg);

    char* buffer = new char[file_size];
    file.read(buffer, file_size);
    file.close();

    if (!write_serial_port(h_comm_dev, sizeof(file_size), &file_size)) {
        cout << "Error sending file size\n";
        delete[] buffer;
        return;
    }

    if (!write_serial_port(h_comm_dev, file_size, buffer)) {
        cout << "Error sending file content\n";
        delete[] buffer;
        return;
    }

    delete[] buffer;
    cout << "File sent successfully\n";
}