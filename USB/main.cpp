#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>

constexpr size_t BUFFER_SIZE = 64;
HANDLE g_hidDevice = INVALID_HANDLE_VALUE;

const int SCREEN_WIDTH = 50;
const int SCREEN_HEIGHT = 10;
const char DINO_HEAD = '@';
const char DINO_BODY = '}';
const char OBSTACLE_CHAR = '#';
const char FLYING_OBSTACLE_CHAR = '*';
const char GROUND_CHAR = '_';
const char EMPTY_CHAR = ' ';

struct Obstacle {
    int x, y;
    bool isFlying;
};

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

bool findUSBDevice(HDEVINFO& deviceList, GUID& guid, PSP_DEVICE_INTERFACE_DETAIL_DATA& detailData) {
    DWORD index = 0;
    unsigned long requiredLength = 0;
    SP_DEVICE_INTERFACE_DATA interfaceData;
    unsigned long detailDataSize = 0;

    deviceList = SetupDiGetClassDevs(&guid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (deviceList == INVALID_HANDLE_VALUE) {
        return false;
    }

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    while (SetupDiEnumDeviceInterfaces(deviceList, nullptr, &guid, index++, &interfaceData)) {
        SetupDiGetDeviceInterfaceDetail(deviceList, &interfaceData, nullptr, 0, &detailDataSize, nullptr);
        requiredLength = detailDataSize;
        detailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(new BYTE[requiredLength]);
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(deviceList, &interfaceData, detailData, detailDataSize, &requiredLength, nullptr)) {
            delete[] detailData;
            SetupDiDestroyDeviceInfoList(deviceList);
            return false;
        }

        std::string devicePath(detailData->DevicePath);
        if (devicePath.find("vid_054c") != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool initializeUSBDevice(HANDLE& hidDevice, PSP_DEVICE_INTERFACE_DETAIL_DATA& detailData, HDEVINFO& deviceList) {
    CloseHandle(hidDevice);
    if (!detailData || detailData->DevicePath[0] == L'\0') {
        std::cerr << "Invalid device interface data or empty device path!" << std::endl;
        return false;
    }

    hidDevice = CreateFile(
            detailData->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
    );

    if (hidDevice == INVALID_HANDLE_VALUE) {
        DWORD errorCode = GetLastError();
        std::cerr << "Failed to open device, error: " << errorCode << std::endl;
        return false;
    }

    std::cout << "Device opened successfully: " << detailData->DevicePath << std::endl;
    SetupDiDestroyDeviceInfoList(deviceList);
    return true;
}

int fetchDeviceInput(HANDLE& hidDevice) {
    BYTE inputBuffer[BUFFER_SIZE] = {0};
    DWORD bytesRead = 0;

    if (!ReadFile(hidDevice, inputBuffer, sizeof(inputBuffer), &bytesRead, nullptr)) {
        DWORD errorCode = GetLastError();
        std::cerr << "ReadFile failed with error: " << errorCode << std::endl;
        return -1;
    }
    return inputBuffer[8];
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

class DinoGame {
private:
    bool isJumping = false;
    bool isCrouching = false;
    int dinoY = SCREEN_HEIGHT - 2;
    int jumpHeight = 0;
    int jumpCount = 0;
    std::vector<Obstacle> obstacles;
    int score = 0;

public:
    void start() {
        srand(time(0));
        while (true) {
            draw();
            handleInput();
            update();
            if (checkCollision()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        gameOver();
    }

private:
    void setCursorPosition(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void draw() {
        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                setCursorPosition(x, y);

                if (y == dinoY && x == 5 && !isCrouching) {
                    setColor(10);
                    std::cout << DINO_HEAD;
                } else if (y == dinoY + 1 && x == 5) {
                    setColor(10);
                    std::cout << DINO_BODY;
                } else {
                    bool obstacleDrawn = false;
                    for (const auto& obstacle : obstacles) {
                        if (obstacle.x == x && obstacle.y == y) {
                            setColor(obstacle.isFlying ? 14 : 12);
                            std::cout << (obstacle.isFlying ? FLYING_OBSTACLE_CHAR : OBSTACLE_CHAR);
                            obstacleDrawn = true;
                            break;
                        }
                    }
                    if (!obstacleDrawn) {
                        setColor(8);
                        std::cout << (y == SCREEN_HEIGHT - 1 ? GROUND_CHAR : EMPTY_CHAR);
                    }
                }
            }
        }
        setCursorPosition(0, SCREEN_HEIGHT);
        setColor(7);
        std::cout << "Score: " << score;
    }

    void handleInput() {
        int keyData = fetchDeviceInput(g_hidDevice);
        if (keyData == 40 && jumpCount < 2) {
            isJumping = true;
            jumpHeight = 3;
            jumpCount++;
        } else if (keyData == 24) {
            isCrouching = true;
        } else {
            isCrouching = false;
        }
    }

    void update() {
        for (auto& obstacle : obstacles) {
            obstacle.x--;
        }

        obstacles.erase(remove_if(obstacles.begin(), obstacles.end(), [](Obstacle& o) {
            return o.x < 0;
        }), obstacles.end());

        if (rand() % 10 == 0) {
            bool isFlying = rand() % 2;
            int obstacleY = isFlying ? SCREEN_HEIGHT - 2 : SCREEN_HEIGHT - 1;

            if (obstacles.empty() || obstacles.back().x <= SCREEN_WIDTH - 5) {
                obstacles.push_back({ SCREEN_WIDTH - 1, obstacleY, isFlying });
            }
        }

        if (isJumping) {
            if (jumpHeight > 0) {
                dinoY--;
                jumpHeight--;
            } else {
                if (dinoY < SCREEN_HEIGHT - 2) {
                    dinoY++;
                } else {
                    isJumping = false;
                }
            }
        }

        if (dinoY == SCREEN_HEIGHT - 2) {
            jumpCount = 0;
        }

        score++;
    }


    bool checkCollision() {
        for (const auto& obstacle : obstacles) {
            if (obstacle.x == 5) {
                if (isCrouching) {
                    if (obstacle.y == SCREEN_HEIGHT - 1) {
                        return true;
                    }
                } else {
                    if (obstacle.y == dinoY || obstacle.y == dinoY + 1) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void gameOver() {
        setCursorPosition(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2);
        setColor(12);
        std::cout << "GAME OVER!";
        setCursorPosition(SCREEN_WIDTH / 2 - 7, SCREEN_HEIGHT / 2 + 1);
        std::cout << "Final Score: " << score;
        while (true) {
            int keyData = fetchDeviceInput(g_hidDevice);
            if (keyData == 136) restartGame();
            else if (keyData == 72) {
                CloseHandle(g_hidDevice);
                exit(0);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void restartGame() {
        score = 0;
        dinoY = SCREEN_HEIGHT - 2;
        isJumping = false;
        isCrouching = false;
        jumpCount = 0;
        obstacles.clear();
        start();
    }
};

int main() {
    SetConsoleCP(1252);
    SetConsoleOutputCP(1252);

    hideCursor();

    GUID hidGuid;
    HDEVINFO deviceList;
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetailData;

    HidD_GetHidGuid(&hidGuid);

    if (!findUSBDevice(deviceList, hidGuid, deviceDetailData) || !initializeUSBDevice(g_hidDevice, deviceDetailData, deviceList)) {
        CloseHandle(g_hidDevice);
        SetupDiDestroyDeviceInfoList(deviceList);
        showCursor();
        return -1;
    }

    DinoGame game;
    game.start();

    CloseHandle(g_hidDevice);
    showCursor();
    return 0;
}
