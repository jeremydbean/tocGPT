#include <windows.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdio>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace {
constexpr UINT WM_APP_LOG_UPDATE = WM_APP + 1;
constexpr UINT WM_APP_STATUS_UPDATE = WM_APP + 2;

struct LauncherConfig {
    std::wstring mudExecutable = L".\\toc.exe";
    std::wstring workingDirectory = L".";
    std::wstring infoLogPath = L".\\log\\info.log";
    std::wstring wizLogPath = L".\\log\\wizinfo.log";
    bool autoRestart = true;
};

struct PostedLogChunk {
    std::wstring text;
};

struct StatusUpdate {
    std::wstring text;
};

bool StartsWith(const std::wstring &value, const std::wstring &prefix) {
    return value.size() >= prefix.size() &&
           std::equal(prefix.begin(), prefix.end(), value.begin());
}

std::wstring Trim(const std::wstring &text) {
    const std::wstring whitespace = L" \t\r\n";
    const auto start = text.find_first_not_of(whitespace);
    if (start == std::wstring::npos) {
        return L"";
    }
    const auto end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

std::wstring ToWide(const std::string &input) {
    std::wstring output;
    output.reserve(input.size());
    for (unsigned char ch : input) {
        output.push_back(static_cast<wchar_t>(ch));
    }
    return output;
}

void ApplyRelativePaths(LauncherConfig &config, const fs::path &baseDir) {
    if (baseDir.empty()) {
        return;
    }
    const auto resolve = [&baseDir](const std::wstring &value) -> std::wstring {
        fs::path candidate(value);
        if (candidate.empty()) {
            return baseDir.wstring();
        }
        if (candidate.is_absolute()) {
            return candidate.lexically_normal().wstring();
        }
        fs::path combined = baseDir / candidate;
        return combined.lexically_normal().wstring();
    };

    config.mudExecutable = resolve(config.mudExecutable);
    config.workingDirectory = resolve(config.workingDirectory);
    config.infoLogPath = resolve(config.infoLogPath);
    config.wizLogPath = resolve(config.wizLogPath);
}

std::optional<LauncherConfig> LoadConfig(const std::wstring &path) {
    LauncherConfig config;
    std::wifstream file(path);
    if (!file.is_open()) {
        return std::nullopt;
    }

    std::wstring line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || StartsWith(line, L"#")) {
            continue;
        }
        const auto eqPos = line.find(L'=');
        if (eqPos == std::wstring::npos) {
            continue;
        }
        std::wstring key = Trim(line.substr(0, eqPos));
        std::wstring value = Trim(line.substr(eqPos + 1));
        if (key == L"mud_executable") {
            config.mudExecutable = value;
        } else if (key == L"working_directory") {
            config.workingDirectory = value;
        } else if (key == L"info_log") {
            config.infoLogPath = value;
        } else if (key == L"wiz_log") {
            config.wizLogPath = value;
        } else if (key == L"auto_restart") {
            config.autoRestart = (value == L"1" ||
                                  StartsWith(value, L"y") ||
                                  StartsWith(value, L"Y") ||
                                  StartsWith(value, L"t") ||
                                  StartsWith(value, L"T"));
        }
    }

    return config;
}

void AppendText(HWND control, const std::wstring &text) {
    const int length = GetWindowTextLengthW(control);
    SendMessageW(control, EM_SETSEL, length, length);
    SendMessageW(control, EM_REPLACESEL, FALSE,
                 reinterpret_cast<LPARAM>(text.c_str()));
}

class MudProcess {
   public:
    bool Start(const LauncherConfig &config, bool createWindow) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (IsProcessActiveLocked()) {
            return true;
        }
        if (processInfo_.hProcess != nullptr) {
            ClearProcessInfoLocked();
        }

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        if (!createWindow) {
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
        }
        PROCESS_INFORMATION pi{};

        std::wstring cmdLine = L"\"" + config.mudExecutable + L"\"";
        std::vector<wchar_t> buffer(cmdLine.begin(), cmdLine.end());
        buffer.push_back(L'\0');

        std::wstring workDir = config.workingDirectory.empty()
                                   ? L"."
                                   : config.workingDirectory;

        BOOL created = CreateProcessW(
            nullptr, buffer.data(), nullptr, nullptr, FALSE,
            createWindow ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW, nullptr,
            workDir.c_str(), &si, &pi);
        if (!created) {
            return false;
        }

        processInfo_ = pi;
        manualStop_ = false;
        return true;
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        manualStop_ = true;
        if (processInfo_.hProcess == nullptr) {
            return;
        }
        TerminateProcess(processInfo_.hProcess, 0);
        WaitForSingleObject(processInfo_.hProcess, INFINITE);
        ClearProcessInfoLocked();
    }

    bool IsRunning() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return IsProcessActiveLocked();
    }

    HANDLE ProcessHandle() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return processInfo_.hProcess;
    }

    bool ManualStopRequested() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return manualStop_;
    }

    void HandleProcessExit() {
        std::lock_guard<std::mutex> lock(mutex_);
        ClearProcessInfoLocked();
    }

   private:
    void ClearProcessInfoLocked() {
        if (processInfo_.hThread) {
            CloseHandle(processInfo_.hThread);
        }
        if (processInfo_.hProcess) {
            CloseHandle(processInfo_.hProcess);
        }
        processInfo_ = PROCESS_INFORMATION{};
    }

    bool IsProcessActiveLocked() const {
        if (processInfo_.hProcess == nullptr) {
            return false;
        }
        DWORD code = 0;
        if (GetExitCodeProcess(processInfo_.hProcess, &code) == 0) {
            return false;
        }
        return code == STILL_ACTIVE;
    }

    PROCESS_INFORMATION processInfo_{};
    mutable std::mutex mutex_{};
    bool manualStop_ = false;
};

class LogWatcher {
   public:
    LogWatcher(HWND window, UINT targetMessage, std::wstring path)
        : window_(window), message_(targetMessage), path_(std::move(path)) {}

    void Start() {
        running_.store(true);
        thread_ = std::thread([this]() { Run(); });
    }

    void Stop() {
        running_.store(false);
        if (thread_.joinable()) {
            thread_.join();
        }
    }

   private:
    void Run() {
        std::uintmax_t knownSize = 0;
        while (running_.load()) {
            try {
                const auto size = fs::file_size(path_);
                if (size < knownSize) {
                    knownSize = 0;  // file rotated
                }
                if (size > knownSize) {
                    std::ifstream file(path_, std::ios::binary);
                    if (file.is_open()) {
                        file.seekg(static_cast<std::streamoff>(knownSize));
                        std::string data((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());
                        knownSize = size;
                        if (!data.empty()) {
                            auto *chunk = new PostedLogChunk;
                            chunk->text = ToWide(data);
                            PostMessageW(window_, message_,
                                         reinterpret_cast<WPARAM>(this),
                                         reinterpret_cast<LPARAM>(chunk));
                        }
                    } else {
                        knownSize = 0;
                    }
                }
            } catch (const fs::filesystem_error &) {
                knownSize = 0;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
        }
    }

    HWND window_ = nullptr;
    UINT message_ = 0;
    std::wstring path_;
    std::atomic<bool> running_{false};
    std::thread thread_;
};

class ProcessMonitor {
   public:
    ProcessMonitor(MudProcess &process, HWND window,
                   const LauncherConfig &config)
        : process_(process), window_(window), config_(config) {}

    void Start() {
        running_.store(true);
        thread_ = std::thread([this]() { Loop(); });
    }

    void Stop() {
        running_.store(false);
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void SetAutoRestart(bool enabled) { autoRestart_.store(enabled); }

   private:
    void Loop() {
        while (running_.load()) {
            HANDLE processHandle = process_.ProcessHandle();
            if (processHandle == nullptr) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            DWORD waitResult = WaitForSingleObject(processHandle, 1000);
            if (waitResult == WAIT_FAILED) {
                process_.HandleProcessExit();
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                continue;
            }
            if (waitResult == WAIT_OBJECT_0) {
                const bool manualStop = process_.ManualStopRequested();
                process_.HandleProcessExit();
                PostStatus(L"MUD stopped.");
                if (!manualStop && autoRestart_.load()) {
                    PostStatus(L"Restarting MUD...");
                    if (!process_.Start(config_, false)) {
                        PostStatus(L"Failed to restart MUD.");
                    } else {
                        PostStatus(L"MUD restarted.");
                    }
                }
            }
        }
    }

    void PostStatus(const std::wstring &text) {
        if (window_) {
            auto *status = new StatusUpdate{ text };
            PostMessageW(window_, WM_APP_STATUS_UPDATE, 0,
                         reinterpret_cast<LPARAM>(status));
        } else {
            std::wcout << L"[STATUS] " << text << std::endl;
        }
    }

    MudProcess &process_;
    HWND window_;
    LauncherConfig config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> autoRestart_{true};
    std::thread thread_;
};

struct GuiState {
    LauncherConfig config;
    MudProcess mudProcess;
    std::unique_ptr<LogWatcher> infoWatcher;
    std::unique_ptr<LogWatcher> wizWatcher;
    std::unique_ptr<ProcessMonitor> monitor;
    HWND infoLogControl = nullptr;
    HWND wizLogControl = nullptr;
    HWND statusControl = nullptr;
    HWND autoRestartCheck = nullptr;
};

void UpdateStatusControl(HWND control, const std::wstring &text) {
    SetWindowTextW(control, text.c_str());
}

void HandleLogUpdate(GuiState &state, WPARAM watcherPtr, LPARAM chunkPtr) {
    auto *chunk = reinterpret_cast<PostedLogChunk *>(chunkPtr);
    if (!chunk) {
        return;
    }
    HWND target = nullptr;
    if (state.infoWatcher && watcherPtr == reinterpret_cast<WPARAM>(state.infoWatcher.get())) {
        target = state.infoLogControl;
    } else if (state.wizWatcher &&
               watcherPtr == reinterpret_cast<WPARAM>(state.wizWatcher.get())) {
        target = state.wizLogControl;
    }
    if (target) {
        AppendText(target, chunk->text);
    }
    delete chunk;
}

void HandleStatusUpdate(GuiState &state, LPARAM param) {
    auto *status = reinterpret_cast<StatusUpdate *>(param);
    if (!status) {
        return;
    }
    if (state.statusControl) {
        UpdateStatusControl(state.statusControl, status->text);
    }
    delete status;
}

void ToggleMud(GuiState &state) {
    if (state.mudProcess.IsRunning()) {
        state.mudProcess.Stop();
        UpdateStatusControl(state.statusControl, L"MUD stopped.");
        return;
    }
    if (!state.mudProcess.Start(state.config, false)) {
        UpdateStatusControl(state.statusControl, L"Failed to start MUD.");
    } else {
        UpdateStatusControl(state.statusControl, L"MUD running.");
    }
}

void EnsureWatchers(GuiState &state, HWND window) {
    if (!state.infoWatcher) {
        state.infoWatcher = std::make_unique<LogWatcher>(window, WM_APP_LOG_UPDATE,
                                                         state.config.infoLogPath);
        state.infoWatcher->Start();
    }
    if (!state.wizWatcher) {
        state.wizWatcher = std::make_unique<LogWatcher>(window, WM_APP_LOG_UPDATE,
                                                        state.config.wizLogPath);
        state.wizWatcher->Start();
    }
}

void EnsureMonitor(GuiState &state, HWND window) {
    if (!state.monitor) {
        state.monitor = std::make_unique<ProcessMonitor>(state.mudProcess, window,
                                                         state.config);
        state.monitor->SetAutoRestart(state.config.autoRestart);
        state.monitor->Start();
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    GuiState *state = reinterpret_cast<GuiState *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_CREATE: {
            auto *createStruct = reinterpret_cast<LPCREATESTRUCTW>(lParam);
            state = reinterpret_cast<GuiState *>(createStruct->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));

            const int padding = 10;
            CreateWindowW(L"BUTTON", L"Start/Stop", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
                                                        BS_DEFPUSHBUTTON,
                          padding, padding, 120, 30, hwnd,
                          reinterpret_cast<HMENU>(1), nullptr, nullptr);

            state->autoRestartCheck = CreateWindowW(
                L"BUTTON", L"Auto Restart", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                padding + 140, padding, 140, 30, hwnd,
                reinterpret_cast<HMENU>(2), nullptr, nullptr);
            SendMessageW(state->autoRestartCheck, BM_SETCHECK,
                         state->config.autoRestart ? BST_CHECKED : BST_UNCHECKED, 0);

            state->statusControl = CreateWindowW(
                L"STATIC", L"MUD stopped.", WS_VISIBLE | WS_CHILD,
                padding, padding + 40, 400, 20, hwnd,
                reinterpret_cast<HMENU>(3), nullptr, nullptr);

            state->infoLogControl = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", nullptr,
                WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                padding, padding + 70, 360, 200, hwnd,
                reinterpret_cast<HMENU>(4), nullptr, nullptr);

            state->wizLogControl = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", nullptr,
                WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                padding, padding + 280, 360, 200, hwnd,
                reinterpret_cast<HMENU>(5), nullptr, nullptr);

            SetWindowTextW(state->infoLogControl, L"Info Log\r\n");
            SetWindowTextW(state->wizLogControl, L"Wiz Log\r\n");

            EnsureWatchers(*state, hwnd);
            EnsureMonitor(*state, hwnd);

            return 0;
        }
        case WM_COMMAND: {
            const int commandId = LOWORD(wParam);
            if (commandId == 1) {
                ToggleMud(*state);
            } else if (commandId == 2) {
                const bool enabled = SendMessageW(state->autoRestartCheck, BM_GETCHECK,
                                                  0, 0) == BST_CHECKED;
                state->config.autoRestart = enabled;
                if (state->monitor) {
                    state->monitor->SetAutoRestart(enabled);
                }
            }
            return 0;
        }
        case WM_APP_LOG_UPDATE:
            if (state) {
                HandleLogUpdate(*state, wParam, lParam);
            }
            return 0;
        case WM_APP_STATUS_UPDATE:
            if (state) {
                HandleStatusUpdate(*state, lParam);
            }
            return 0;
        case WM_DESTROY:
            if (state) {
                if (state->infoWatcher) {
                    state->infoWatcher->Stop();
                }
                if (state->wizWatcher) {
                    state->wizWatcher->Stop();
                }
                if (state->monitor) {
                    state->monitor->Stop();
                }
                if (state->mudProcess.IsRunning()) {
                    state->mudProcess.Stop();
                }
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int RunGuiLauncher(HINSTANCE instance, HINSTANCE, LPWSTR, int cmdShow,
                   LauncherConfig config) {
    GuiState state{};
    state.config = std::move(config);

    const wchar_t className[] = L"TOCLauncherWindow";
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, className, L"TOC Launcher",
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                420, 540, nullptr, nullptr, instance, &state);
    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, cmdShow);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

class ConsoleLogWatcher {
   public:
    ConsoleLogWatcher(std::wstring label, std::wstring path)
        : label_(std::move(label)), path_(std::move(path)) {}

    void Start() {
        running_.store(true);
        thread_ = std::thread([this]() { Loop(); });
    }

    void Stop() {
        running_.store(false);
        if (thread_.joinable()) {
            thread_.join();
        }
    }

   private:
    void Loop() {
        std::uintmax_t knownSize = 0;
        while (running_.load()) {
            try {
                const auto size = fs::file_size(path_);
                if (size < knownSize) {
                    knownSize = 0;
                }
                if (size > knownSize) {
                    std::ifstream file(path_, std::ios::binary);
                    if (file.is_open()) {
                        file.seekg(static_cast<std::streamoff>(knownSize));
                        std::string line;
                        while (std::getline(file, line)) {
                            if (!line.empty() && line.back() == '\r') {
                                line.pop_back();
                            }
                            std::wstring wideLine = ToWide(line);
                            std::wcout << L"[" << label_ << L"] " << wideLine << std::endl;
                        }
                        knownSize = size;
                    }
                }
            } catch (const fs::filesystem_error &) {
                knownSize = 0;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
        }
    }

    std::wstring label_;
    std::wstring path_;
    std::atomic<bool> running_{false};
    std::thread thread_;
};

void RunConsoleLauncher(const LauncherConfig &config) {
    MudProcess process;
    ProcessMonitor monitor(process, nullptr, config);
    monitor.SetAutoRestart(config.autoRestart);
    monitor.Start();

    ConsoleLogWatcher infoWatcher(L"INFO", config.infoLogPath);
    ConsoleLogWatcher wizWatcher(L"WIZ", config.wizLogPath);
    infoWatcher.Start();
    wizWatcher.Start();

    std::wcout << L"TOC MUD Launcher" << std::endl;
    std::wcout << L"Commands: start, stop, status, autorestart on/off, exit" << std::endl;

    bool running = true;
    while (running) {
        std::wcout << L"> ";
        std::wstring command;
        if (!(std::wcin >> command)) {
            break;
        }
        if (command == L"start") {
            if (process.Start(config, true)) {
                std::wcout << L"MUD started." << std::endl;
            } else {
                std::wcout << L"Failed to start MUD." << std::endl;
            }
        } else if (command == L"stop") {
            process.Stop();
            std::wcout << L"MUD stopped." << std::endl;
        } else if (command == L"status") {
            std::wcout << (process.IsRunning() ? L"MUD running." : L"MUD stopped.")
                       << std::endl;
        } else if (command == L"autorestart") {
            std::wstring option;
            if (std::wcin >> option) {
                bool enable = (option == L"on" || option == L"1");
                monitor.SetAutoRestart(enable);
                std::wcout << L"Auto restart " << (enable ? L"enabled" : L"disabled")
                           << std::endl;
            }
        } else if (command == L"exit") {
            running = false;
        } else {
            std::wcout << L"Unknown command." << std::endl;
        }
    }

    monitor.Stop();
    infoWatcher.Stop();
    wizWatcher.Stop();
    process.Stop();
}

}  // namespace

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    fs::path exeDir;
    wchar_t modulePath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, modulePath, MAX_PATH)) {
        exeDir = fs::path(modulePath).parent_path();
    }

    LauncherConfig config;
    if (!exeDir.empty()) {
        auto configPath = exeDir / L"launcher_config.ini";
        if (auto loaded = LoadConfig(configPath.wstring())) {
            config = *loaded;
        }
        ApplyRelativePaths(config, exeDir);
    }

    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    bool consoleMode = false;
    if (argv) {
        for (int i = 0; i < argc; ++i) {
            if (std::wstring(argv[i]) == L"--console") {
                consoleMode = true;
            }
        }
        LocalFree(argv);
    }

    if (consoleMode) {
        AllocConsole();
        FILE *dummy;
        freopen_s(&dummy, "CONIN$", "r", stdin);
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        RunConsoleLauncher(config);
        return 0;
    }

    return RunGuiLauncher(hInstance, hPrevInstance, lpCmdLine, nCmdShow, config);
}
